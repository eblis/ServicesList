/*
ServicesList plugin for Miranda IM

Copyright � 2006 Cristian Libotean

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

#ifndef M_SERVICESLIST_SERVICES_H
#define M_SERVICESLIST_SERVICES_H

#include "commonheaders.h"

extern bool use_hi_res;

typedef HANDLE (*CREATESERVICEFUNCTION) (const char *, MIRANDASERVICE service);
typedef HANDLE (*CREATETRANSIENTSERVICEFUNCTION) (const char *, MIRANDASERVICE service);
typedef INT_PTR (*CALLSERVICEFUNCTION) (const char *, WPARAM wParam, LPARAM lParam);

extern CREATESERVICEFUNCTION realCreateServiceFunction;
extern CREATETRANSIENTSERVICEFUNCTION realCreateTransientServiceFunction;
extern CALLSERVICEFUNCTION realCallServiceFunction;

int InitServices();
int DestroyServices();

void HookRealServices(int bDynamic);
void UnhookRealServices();

HANDLE HackCreateServiceFunction(const char *name, MIRANDASERVICE service);
HANDLE HackCreateTransientServiceFunction(const char *name, MIRANDASERVICE service);
INT_PTR HackCallServiceFunction(const char *name, WPARAM wParam, LPARAM lParam);

int AddServiceToList(const char *name, MIRANDASERVICE service, int type);

#endif //M_SERVICESLIST_SERVICES_H
