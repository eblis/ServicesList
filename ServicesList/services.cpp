/*
ServicesList plugin for Miranda IM

Copyright © 2006 Cristian Libotean

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "services.h"

static CRITICAL_SECTION csServices;
static int sbFindServices = 0;

CREATESERVICEFUNCTION realCreateServiceFunction = NULL;
CREATETRANSIENTSERVICEFUNCTION realCreateTransientServiceFunction = NULL;
CALLSERVICEFUNCTION realCallServiceFunction = NULL;

bool use_hi_res = false;
LARGE_INTEGER hr_freq;

void RefreshServiceInList(int index)
{
	if (hOptDlg)
	{
		SendMessage(hOptDlg, SLM_UPDATESERVICE, index, 0);
	}
}

int InitServices()
{
	InitializeCriticalSection(&csServices);
	
	sbFindServices = DBGetContactSettingByte(NULL, ModuleName, "DiscoveryMode", 2);
	HookRealServices(0);
	HookRealServices(1); //hook dynamic services too, if required
	
	if(QueryPerformanceFrequency(&hr_freq))
	{
		use_hi_res = true;
	}

	return 0;
}

int DestroyServices()
{
	UnhookRealServices();
	DeleteCriticalSection(&csServices);
	
	return 0;
}

void HookRealServices(int bDynamic)
{
	if (bDynamic)
	{
		if (sbFindServices > 1) //dynamic
		{
			Log("Hooking real CallService(). Replacing 0x%p with 0x%p", pluginLink->CallService, HackCallServiceFunction);
		
			realCallServiceFunction = pluginLink->CallService;
			
			pluginLink->CallService = HackCallServiceFunction;
		}
	}
	else{
		if (sbFindServices > 0) //static
		{
			//save real functions
			realCreateServiceFunction = pluginLink->CreateServiceFunction;
			realCreateTransientServiceFunction = pluginLink->CreateTransientServiceFunction;
			
			//substitute our own
			pluginLink->CreateServiceFunction = HackCreateServiceFunction;
			pluginLink->CreateTransientServiceFunction = HackCreateTransientServiceFunction;
			
		}
	}
}

void UnhookRealServices()
{
	if (sbFindServices > 0) //static or dynamic
	{
		EnterCriticalSection(&csServices);
		
		//restore real functions
		pluginLink->CreateServiceFunction = realCreateServiceFunction;
		pluginLink->CreateTransientServiceFunction = realCreateTransientServiceFunction;
		
		if (sbFindServices > 1) //dynamic
		{
			Log("Unhooking real CallService(). Replacing 0x%p with 0x%p", pluginLink->CallService, realCallServiceFunction);
			
			pluginLink->CallService = realCallServiceFunction;
		}
		
		LeaveCriticalSection(&csServices);
	}
}

HANDLE HackCreateServiceFunction(const char *name, MIRANDASERVICE service)
{
	EnterCriticalSection(&csServices);
	AddServiceToList(name, service, SERVICE_NORMAL);
	HANDLE res = realCreateServiceFunction(name, service);
	LeaveCriticalSection(&csServices);
	
	return res;
}

HANDLE HackCreateTransientServiceFunction(const char *name, MIRANDASERVICE service)
{
	EnterCriticalSection(&csServices);
	AddServiceToList(name, service, SERVICE_TRANSIENT);
	HANDLE res = realCreateTransientServiceFunction(name, service);
	LeaveCriticalSection(&csServices);
	
	return res;
}

INT_PTR HackCallServiceFunction(const char *name, WPARAM wParam, LPARAM lParam)
{
	DWORD time_start;
	LARGE_INTEGER hr_time_start, hr_time;
	double time_taken;

	if(use_hi_res)
	{
		QueryPerformanceCounter(&hr_time_start);
	}
	else{
		time_start = GetTickCount();
	}
	
	INT_PTR res = realCallServiceFunction(name, wParam, lParam);
	
	if (res != CALLSERVICE_NOTFOUND)
	{
		if(use_hi_res)
		{
			QueryPerformanceCounter(&hr_time);
			hr_time.QuadPart -= hr_time_start.QuadPart;
			time_taken = (hr_time.QuadPart * 1000.0 / hr_freq.QuadPart);
		}
		else{
			time_taken = GetTickCount() - time_start;
		}

		int index = lstServices.Index(name);
		if(index >= 0)
		{
			lstServices[index]->call_count++;
			lstServices[index]->total_call_time_ms += time_taken;
			
			//RefreshServiceInList(index);
		}
		else{
		//EnterCriticalSection(&csServices);
		AddServiceToList(name, NULL, SLF_DYNAMIC);
		//LeaveCriticalSection(&csServices);
		}
	}

	return res;
}


PVOID CALLBACK TableAccess(HANDLE hProcess, DWORD64 addrBase)
{
	return NULL;
}

DWORD64 CALLBACK GetModuleBase(HANDLE hProcess, DWORD64 address)
{
	return 0;
}

#include "DbgHelp.h"

int AddServiceToList(const char *name, MIRANDASERVICE service, int type)
{
	//return Log("Service created: '%s' at address %p %s", name, service, (type == SERVICE_NORMAL) ? "" : "[transient]");
	//STACKFRAME64 stack;
	//IMAGEHLP_MODULE64 info;
	//
	//while (StackWalk64(IMAGE_FILE_MACHINE_I386, //machine
	//									 GetCurrentProcess(), //proces
	//									 GetCurrentThread(), //thread
	//									 &stack, //stack frame
	//									 NULL, //context
	//									 NULL, //read memory routine
	//									 TableAccess, //table access routine
	//									 GetModuleBase, //get module base routine
	//									 NULL))
	//{
	//	if (SymGetModuleInfo64(GetCurrentProcess(), 0, &info))
	//	{
	//		Log("%s", info.ModuleName);
	//	}
	//}
	MEMORY_BASIC_INFORMATION mbi;
	HINSTANCE hInstance;
	char mod[512];
	mod[0] = 0;
	if(VirtualQuery(service, &mbi, sizeof(mbi) )) {
		hInstance = (HINSTANCE)(mbi.AllocationBase);
		char mod_path[MAX_PATH];
		GetModuleFileName(hInstance, mod_path, MAX_PATH);
		// chop path
		char *p = strrchr(mod_path, '\\');
		if(p) {
			p++;
			strncpy(mod, p, 512);
		}

	}

	if ((!lstServices.Add(name, mod, service, type)) && (hOptDlg))
	{
		SendMessage(hOptDlg, SLM_ADDNEWSERVICES, 0, 0);
	}
	
	return 0;
}