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

#include "hooked_events.h"

HANDLE hModulesLoaded;
HANDLE hOptionsInitialize;
HANDLE hShutdown;

UINT_PTR hRefreshTimer = NULL;

#define HOST "http://eblis.tla.ro/projects"

#if defined(WIN64) || defined(_WIN64)
#define SERVICESLIST_VERSION_URL HOST "/miranda/ServicesList/updater/x64/ServicesList.html"
#define SERVICESLIST_UPDATE_URL HOST "/miranda/ServicesList/updater/x64/ServicesList.zip"
#else
#define SERVICESLIST_VERSION_URL HOST "/miranda/ServicesList/updater/ServicesList.html"
#define SERVICESLIST_UPDATE_URL HOST "/miranda/ServicesList/updater/ServicesList.zip"
#endif
#define SERVICESLIST_VERSION_PREFIX "Services List version "

int HookEvents()
{
	Log("%s", "Entering function " __FUNCTION__);
	hModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, OnModulesLoaded);
	hOptionsInitialize = HookEvent(ME_OPT_INITIALISE, OnOptionsInitialise);
	hShutdown = HookEvent(ME_SYSTEM_SHUTDOWN, OnShutdown);
	Log("%s", "Leaving function " __FUNCTION__);
	
	UpdateTimer();
	nVisibleColumns = DBGetContactSettingByte(NULL, ModuleName, "VisibleColumns", 0xFF);
	
	return 0;
}

int UnhookEvents()
{
	Log("%s", "Entering function " __FUNCTION__);
	UnhookEvent(hModulesLoaded);
	UnhookEvent(hOptionsInitialize);
	UnhookEvent(hShutdown);
	Log("%s", "Leaving function " __FUNCTION__);
	
	DestroyTimer();
	
	return 0;
}

int UpdateTimer()
{
	if (!hRefreshTimer)
		{
			hRefreshTimer = SetTimer(NULL, 0, 5 * 1000, OnRefreshTimer);
		}
	
	return 0;
};

int DestroyTimer()
{
	if (hRefreshTimer)
	{
		KillTimer(NULL, hRefreshTimer);
		hRefreshTimer = NULL;
	}
	
	return 0;
}

int OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	char buffer[1024];
	Update update = {0};
	update.cbSize = sizeof(Update);
	update.szComponentName = __PLUGIN_DISPLAY_NAME;
	update.pbVersion = (BYTE *) CreateVersionString(VERSION, buffer);
	update.cpbVersion = (int) strlen((char *) update.pbVersion);
	update.szUpdateURL = UPDATER_AUTOREGISTER;
	update.szBetaVersionURL = SERVICESLIST_VERSION_URL;
	update.szBetaUpdateURL = SERVICESLIST_UPDATE_URL;
	update.pbBetaVersionPrefix = (BYTE *) SERVICESLIST_VERSION_PREFIX;
	update.cpbBetaVersionPrefix = (int) strlen(SERVICESLIST_VERSION_PREFIX);
	CallService(MS_UPDATE_REGISTER, 0, (LPARAM) &update);
	
	bAddTTBBLoaded = IsPluginLoaded("Add TTB Buttons");
	
	return 0;
}

int OnOptionsInitialise(WPARAM wParam, LPARAM lParam)
{
	OPTIONSDIALOGPAGE odp = {0};
	
	odp.cbSize = sizeof(odp);
	odp.position = 100000000;
	odp.hInstance = hInstance;
	odp.pszTemplate = MAKEINTRESOURCEA(IDD_OPT_SERVICES);
	odp.ptszTitle = TranslateT("Services");
	odp.ptszGroup = NULL;
	odp.groupPosition = 810000000;
	odp.flags=ODPF_BOLDGROUPS;
	odp.pfnDlgProc = DlgProcOptions;
	CallService(MS_OPT_ADDPAGE, wParam, (LPARAM)&odp);
	
	return 0;
}

int OnShutdown(WPARAM wParam, LPARAM lParam)
{
	DBWriteContactSettingByte(NULL, ModuleName, "VisibleColumns", nVisibleColumns);
	
	return 0;
}

void CALLBACK OnRefreshTimer(HWND hWnd, UINT msg, UINT_PTR eventID, DWORD dwTime)
{
	if (hOptDlg)
	{
		SendMessage(hOptDlg, SLM_REFRESHSERVICES, 0, 0);
	}
}