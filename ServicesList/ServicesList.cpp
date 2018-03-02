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

#include "commonheaders.h"

char ModuleName[] = "ServicesList";
HINSTANCE hInstance;
HBITMAP hiRefresh = NULL;

int bAddTTBBLoaded;

PLUGINLINK *pluginLink;

PLUGININFOEX pluginInfo = {
	sizeof(PLUGININFOEX),
	__PLUGIN_DISPLAY_NAME,
	VERSION,
	__DESC,
	__AUTHOR,
	__AUTHOREMAIL,
	__COPYRIGHT,
	__AUTHORWEB,
	0,
	0,
	{0x2f30f130, 0x9749, 0x4523, {0x91, 0xc9, 0x28, 0xb7, 0xe9, 0x77, 0x9d, 0xcd}} //{2f30f130-9749-4523-91c9-28b7e9779dcd}
}; //not used

OLD_MIRANDAPLUGININFO_SUPPORT;

extern "C" __declspec(dllexport) PLUGININFOEX *MirandaPluginInfoEx(DWORD mirandaVersion) 
{
//	Log("%s", "Entering function " __FUNCTION__);
//	Log("%s", "Leaving function " __FUNCTION__);
	if (mirandaVersion < PLUGIN_MAKE_VERSION(0, 6, 0, 0))
	{
		return NULL;
	}
	
	return &pluginInfo;
}

static const MUUID interfaces[] = {MIID_SERVICESLIST, MIID_LAST};

extern "C" __declspec(dllexport) const MUUID *MirandaPluginInterfaces()
{
	return interfaces;
}

extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
	LogInit();
	
	pluginLink = link;
	
	hiRefresh = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_REFRESH), IMAGE_BITMAP, 0, 0, 0);
	
	InitServices();
	
	HookEvents();
	
	InitializeMirandaMemFunctions();
	
	return 0;
}

extern "C" int __declspec(dllexport) Unload()
{
	DestroyServices();

	UnhookEvents();
	
	DeleteObject(hiRefresh);
	
	return 0;
}

bool WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
		{
			DisableThreadLibraryCalls(hinstDLL);
		}
		
	return TRUE;
}

