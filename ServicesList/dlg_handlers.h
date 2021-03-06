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

#ifndef M_SERVICESLIST_DLGHANDLERS_H
#define M_SERVICESLIST_DLGHANDLERS_H

#include "commonheaders.h"

#define SLM_UPDATESERVICES  WM_USER + 10
#define SLM_ADDNEWSERVICES  WM_USER + 11
#define SLM_UPDATESERVICE   WM_USER + 12
#define SLM_REFRESHSERVICES WM_USER + 13

#define SLM_SUBCLASSED      WM_USER + 100

extern HWND hOptDlg;
extern int nVisibleColumns;

INT_PTR CALLBACK DlgProcOptions(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif