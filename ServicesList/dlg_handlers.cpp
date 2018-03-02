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

#include "dlg_handlers.h"
#include "commctrl.h"

int nVisibleColumns = 0xFF;

#define CMP(a, b) ( ((a == b) ? 0 : ((a < b) ? -1 : 1)) )

const char *szDiscoveryMode[] = {"None, do not discover services", "Static, on service create", "Dynamic, on service create or call"};
const int cDiscoveryMode = sizeof(szDiscoveryMode) / sizeof(szDiscoveryMode[0]);

const char *szColumns[] = {"Owner", "Service", "Address", "Calls", "Total time", "Flags"};
const int cxColumns[] = {100, 220, 100, 90, 90, 90};
const int cColumns = sizeof(szColumns) / sizeof(szColumns[0]);



HWND hOptDlg = NULL;
WNDPROC oldServicesListProc = NULL;

#define POPUPMENU_COPY              1
#define POPUPMENU_HOTKEYSPLUS       2
#define POPUPMENU_TOPTOOLBAR        3

#define POPUPMENU_COLUMNS         100

#define COLUMN(col) (1 << col)

int IsColumnVisible(int column)
{
	if (column == 0) //owner column needs to be visible.
	{
		return 1;
	}
	int res = (nVisibleColumns & COLUMN(column)) != 0;
	
	return res;
}

int ShowColumn(int column, int bShow)
{
	if (bShow)
	{
		nVisibleColumns |= COLUMN(column);
	}
	else{
		nVisibleColumns &= ~COLUMN(column);
	}
	
	return 0;
}

void RefreshServiceInfo(HWND hList, int listIndex, PService service)
{
	char buffer[4096];
	int invisible = 0; //count how many invisible columns there are

	if (IsColumnVisible(1))
	{
		ListView_SetItemText(hList, listIndex, 1, service->name);
	}
	else{
		invisible++;
	}
	
	if (IsColumnVisible(2))
	{
		_snprintf(buffer, sizeof(buffer), "0x%08p", service->service);
		ListView_SetItemText(hList, listIndex, 2 - invisible, buffer);
	}
	else{
		invisible++;
	}

	if (IsColumnVisible(3))
	{
		_snprintf(buffer, sizeof(buffer), "%u", service->call_count);
		ListView_SetItemText(hList, listIndex, 3 - invisible, buffer);
	}
	else{
		invisible++;
	}

	if (IsColumnVisible(4))
	{
		_snprintf(buffer, sizeof(buffer), "%.3lf", service->total_call_time_ms / 1000.0);
		ListView_SetItemText(hList, listIndex, 4 - invisible, buffer);
	}
	else{
		invisible++;
	}
	
	if (IsColumnVisible(5))
	{
		_snprintf(buffer, sizeof(buffer), "%s", (service->flags & SERVICE_TRANSIENT) ? Translate("transient") : "");
		if (strlen(buffer) > 0)
		{
			strcat(buffer, " ");
		}
		strcat(buffer, (service->flags & SLF_DYNAMIC) ? Translate("dynamic discovery") : "");
		ListView_SetItemText(hList, listIndex, 5 - invisible, buffer);
	}
}

int UpdateService(HWND hList, int i, int bAdd, int listIndex = -1)
{
	PService service = lstServices[i];
	if (service)
	{
		LVITEM item = {0};
		
		item.mask = LVIF_TEXT | LVIF_PARAM;

		item.iItem = (listIndex >= 0) ? listIndex : i;
		
		//item.pszText = "N/A"; //service->module;
		item.pszText = service->module;
		item.lParam = (LPARAM) service;
		
		if (bAdd)
		{
			ListView_InsertItem(hList, &item);
		}
		else{
			ListView_SetItemText(hList, i, 0, service->module);
		}
		RefreshServiceInfo(hList, i, service);
	}
	
	return 0;
}

void LoadServices(HWND hWnd, int bAll = TRUE)
{
	char buffer[2048];
	HWND hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
	
	if (bAll)
	{
		ListView_DeleteAllItems(hList);
	}
	
	int i;
	int init;
	PService service = NULL;
	
	LVITEM item = {0};
	item.mask = LVIF_TEXT;
	
	init = (bAll) ? 0 : SendMessage(hList, LVM_GETITEMCOUNT, 0, 0);
	
	for (i = init; i < lstServices.Count(); i++)
	{
		UpdateService(hList, i, TRUE);
	}
	
	_snprintf(buffer, sizeof(buffer), Translate("Available services (%d) :"), lstServices.Count());
	SetWindowText(GetDlgItem(hWnd, IDC_AVAILABLE_SERVICES), buffer);
}

struct SortParams{
	HWND hList;
	int fullColumn;
	int visibleColumn;
};

static int lastColumn = -1;

int CALLBACK ServicesCompare(LPARAM lParam1, LPARAM lParam2, LPARAM myParam)
{
	SortParams params = *(SortParams *) myParam;
	int res;
	const int MAX_SIZE = 1024;
	int value1, value2;
	float real1, real2;
	
	char text1[MAX_SIZE];
	char text2[MAX_SIZE];
	
	ListView_GetItemText(params.hList, (int) lParam1, params.visibleColumn, text1, MAX_SIZE);
	ListView_GetItemText(params.hList, (int) lParam2, params.visibleColumn, text2, MAX_SIZE);
	
	switch (params.fullColumn)
	{
		case 0:
		case 1: 
		case 5:
		default: //compare strings with strcmp
		{
			res = _stricmp(text1, text2);
			
			break;
		}
		
		case 2:
		{
			sscanf(text1, "0x%x", &value1);
			sscanf(text2, "0x%x", &value2);
			
			res = CMP(value1, value2);
			
			break;
		}
		
		case 3:
		case 4:
		{
			sscanf(text1, "%f", &real1);
			sscanf(text2, "%f", &real2);
			
			res = CMP(real1, real2);
			
			break;
		}
	}
	
	
	res = (params.visibleColumn == lastColumn) ? -res : res;
	
	return res;
}

int AddInfoToCombobox(HWND hWnd, int nDiscoveryComboBox)
{
	int i;
	for (i = 0; i < cDiscoveryMode; i++)
		{
			SendDlgItemMessage(hWnd, nDiscoveryComboBox, CB_ADDSTRING, 0, (LPARAM) Translate(szDiscoveryMode[i]));
		}
		
	return 0;
}

int LoadColumns(HWND hList)
{
	int i;
	char buffer[512];
	int start = 0;
	LVCOLUMN col;
	col.mask = LVCF_TEXT | LVCF_WIDTH;
	
	while (ListView_DeleteColumn(hList, 1)) //delete all columns
	{
		start = 1;
	}
	
	for (i = start; i < cColumns; i++)
	{
		if (IsColumnVisible(i))
		{
			col.pszText = TranslateTS(szColumns[i]);
			_snprintf(buffer, sizeof(buffer), "cx%s", szColumns[i]);
		
			col.cx = DBGetContactSettingWord(NULL, ModuleName, buffer, cxColumns[i]);
		
			ListView_InsertColumn(hList, i, &col);
		}
	}
	
	return 0;
}

int SaveColumnSizes(HWND hList)
{
	char buffer[512];
	int i;
	int cx;
	int invisible = 0;
	
	for (i = 0; i < cColumns; i++)
	{
		if (IsColumnVisible(i))
		{
			_snprintf(buffer, sizeof(buffer), "cx%s", szColumns[i]);
			cx = ListView_GetColumnWidth(hList, i - invisible);
			DBWriteContactSettingWord(NULL, ModuleName, buffer, cx);
		}
		else{
			invisible++;
		}
	}
	
	return 0;
}

char *GetSelectedItemService(HWND hWnd, char *buffer, int size)
{
	int i;
	HWND hList = hWnd;//GetDlgItem(hWnd, IDC_LIST_SERVICES);
	int count = ListView_GetItemCount(hList);
	
	buffer[0] = '\0';
	
	for (i = 0; i < count; i++)
	{
		if (ListView_GetItemState(hList, i, LVIS_SELECTED))
		{
			LVITEM item = {0};
			PService service;
			item.mask = LVIF_PARAM;
			item.iItem = i;
			
			ListView_GetItem(hList, &item);
			service = (PService) item.lParam;
			if (service)
			{
				strncpy(buffer, service->name, size);
			}
			
			break; //found it
		}	
	}
	
	return buffer;
}

void HandleListViewPopupMenu(HWND hWnd, HMENU hMenu, int selection)
{
	char service[1024];
	switch (selection)
	{
		case POPUPMENU_COPY:
		{
			GetSelectedItemService(hWnd, service, sizeof(service));
			if (GetOpenClipboardWindow())
			{
				MyPUShowMessage(Translate("The clipboard is not available, retry"), SM_WARNING);
				
				break;
			}
			
			if (OpenClipboard(hWnd))
			{
				EmptyClipboard();
				char *clipboard;
				size_t len = strlen(service);
				HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, len + 2);
				clipboard = (char *) GlobalLock(hData);
				memcpy(clipboard, service, len + 1);
				clipboard[len] = '\0'; //make sure it's null terminated
				GlobalUnlock(hData);
				if (!SetClipboardData(CF_TEXT, hData))
				{
					int err = GetLastError();
					MyPUShowMessage(Translate("Could not set clipboard data"), SM_WARNING);
				}
				CloseClipboard();
			}
			else{
				MyPUShowMessage(Translate("Could not open clipboard"), SM_WARNING);
			}
			
			break;
		}
		
		case POPUPMENU_HOTKEYSPLUS:
		{
			GetSelectedItemService(hWnd, service, sizeof(service));
			int res = CallService(MS_HOTKEYSPLUS_ADDKEY, (WPARAM) service, (LPARAM) Translate("Services List autogenerated description"));
			char message[1024];
			int type = SM_NOTIFY;
			switch (res)
			{
				case 0:
				{
					_snprintf(message, sizeof(message), Translate("Hotkey for service '%s' added successfully"), service);
					
					break;
				}
				
				case 1:
				{
					_snprintf(message, sizeof(message), Translate("There's already a hotkey configured for service '%s'"), service);
					type = SM_WARNING;
				
					break;
				}
				
				default:
				{
					_snprintf(message, sizeof(message), Translate("Service '%s'does not exist ???"), service);
					type = SM_WARNING;
					
					break;
				}
			}
			MyPUShowMessage(message, type);
			
			break;
		}
		
		case POPUPMENU_TOPTOOLBAR:
		{
			GetSelectedItemService(hWnd, service, sizeof(service));
			int index = 0;
			char buffer[1024];
			char name[64];
			for(;; index++) //find the first unused slot
			{
				_itoa(index, name, 10);
				if (GetStringFromDatabase(NULL, "AddTTBB", name, "<N/A>", buffer, sizeof(buffer)))
				{
					break;
				}
			}
			
			_itoa(index, name, 10);
			DBWriteContactSettingString(NULL, "AddTTBB", name, service);
			_snprintf(buffer, sizeof(buffer), Translate("Service %s was added to module AddTTBB as entry %s"), service, name);
			MyPUShowMessage(buffer, SM_NOTIFY);
		
			break;
		}
	}
}

void HandleHeaderPopupMenu(HWND hList, HMENU hMenu, int column)
{
	if (column)
	{
		column -= POPUPMENU_COLUMNS;
		ShowColumn(column, IsColumnVisible(column) ? 0 : 1);
		
		LoadColumns(hList);
		LoadServices(hOptDlg, 1);
	}
}

INT_PTR CALLBACK ServicesListSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case SLM_SUBCLASSED:
		{
			HWND hHeader = ListView_GetHeader(hWnd);
			
			break;
		}
		
		case WM_DESTROY: //doesn't get called
		{
			HWND hHeader = ListView_GetHeader(hWnd);
			
			break;
		}
		
		case WM_CONTEXTMENU:
		{
			HMENU hMenu = CreatePopupMenu();
			int x = LOWORD(lParam); 
			int y = HIWORD(lParam); 
			int res;
			
			if (hMenu)
			{
				if ((HWND) wParam == ListView_GetHeader(hWnd))
				{
					int i;
					AppendMenu(hMenu, MF_STRING | MF_GRAYED, POPUPMENU_COLUMNS + 0, TranslateTS(szColumns[0]));
					for (i = 1; i < cColumns; i++)
					{
						AppendMenu(hMenu, MF_STRING | (IsColumnVisible(i) ? MF_CHECKED : MF_UNCHECKED), POPUPMENU_COLUMNS + i, TranslateTS(szColumns[i]));
					}
					
					res = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, 0, hWnd, NULL);
					DestroyMenu(hMenu);
					HandleHeaderPopupMenu(hWnd, hMenu, res);
				}
				else{
					res = AppendMenu(hMenu, MF_STRING, POPUPMENU_COPY, Translate("Copy to clipboard"));
					res = AppendMenu(hMenu, MF_STRING | ((ServiceExists(MS_HOTKEYSPLUS_ADDKEY) ? 0 : MF_DISABLED | MF_GRAYED)), POPUPMENU_HOTKEYSPLUS, Translate("Add to Hotkeys+"));
					res = AppendMenu(hMenu, MF_STRING | ((bAddTTBBLoaded) ? 0 : MF_DISABLED | MF_GRAYED), POPUPMENU_TOPTOOLBAR, Translate("Add TopToolBar button"));
					res = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, 0, hWnd, NULL);
					
					DestroyMenu(hMenu);
					HandleListViewPopupMenu(hWnd, hMenu, res);
				}
			}
			else{
				MyPUShowMessage("Could not create popup menu", SM_WARNING);
			}
		
			break;
		}
	}
	
	return CallWindowProc(oldServicesListProc, hWnd, msg, wParam, lParam);
}

INT_PTR CALLBACK DlgProcOptions(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			hOptDlg = hWnd;
			HWND hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
			
			AddInfoToCombobox(hWnd, IDC_DISCOVERY_MODE);
			
			SendMessage(GetDlgItem(hWnd, IDC_REFRESH), BM_SETIMAGE, IMAGE_BITMAP,	(LPARAM) hiRefresh);
			
			TranslateDialogDefault(hWnd);

			SendDlgItemMessage(hWnd, IDC_DISCOVERY_MODE, CB_SETCURSEL, DBGetContactSettingByte(NULL, ModuleName, "DiscoveryMode", 2), 0);
			
			ListView_SetExtendedListViewStyleEx(hList, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
			
			LoadColumns(hList);
			
			oldServicesListProc = (WNDPROC) SetWindowLongPtr(hList, GWLP_WNDPROC, (LONG_PTR) ServicesListSubclassProc); //subclass the list
			SendMessage(GetDlgItem(hWnd, IDC_LIST_SERVICES), SLM_SUBCLASSED, 0, 0);	 //tell the list it's been subclassed
			
			LoadServices(hWnd);
			
			SortParams params = {0};
			params.hList = hList;
			params.fullColumn = params.visibleColumn = DBGetContactSettingByte(NULL, ModuleName, "LastSortColumn", -1);
			ListView_SortItemsEx(hList, ServicesCompare, &params);
			
			return TRUE;
			break;
		}
		
		case WM_DESTROY:
		{
			hOptDlg = NULL;
			HWND hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
			DBWriteContactSettingByte(NULL, ModuleName, "LastSortColumn", lastColumn);
			SaveColumnSizes(hList);
			
			lastColumn = -1;
			SetWindowLongPtr(hList, GWLP_WNDPROC, (LONG_PTR) oldServicesListProc); //remove the subclass proc
		
			break;
		}
		
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_DISCOVERY_MODE:
				{
					ShowWindow(GetDlgItem(hWnd, IDC_RESTARTREQUIRED), SW_SHOW);
					SendMessage(GetParent(hWnd), PSM_CHANGED, 0, 0);
					
					break;
				}
				
				case IDC_REFRESH:
				{
					SendMessage(hWnd, SLM_REFRESHSERVICES, 0, 0);
					
					break;
				}
			}
			
			break;
		}
		
		case SLM_UPDATESERVICES:
		{
			LoadServices(hWnd);
		
			break;
		}
		
		case SLM_UPDATESERVICE:
		{
			PService service = lstServices[wParam];
			if (service)
			{
				HWND hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
				int i;
				int count = ListView_GetItemCount(hList);
				char buffer[1024];
				LVITEM item = {0};
				
				item.mask = LVIF_TEXT;
				
				item.pszText = buffer;
				item.cchTextMax = sizeof(buffer);
				
				for (i = 0; i < count; i++)
				{
					item.iItem = i;
					ListView_GetItem(hList, &item);
					if (_stricmp(service->module, buffer) == 0)
					{
						UpdateService(hList, wParam, FALSE, i);
						break;
					}
				}
			}
		
			break;
		}
		
		case SLM_REFRESHSERVICES:
		{
			HWND hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
			int count = ListView_GetItemCount(hList);
			int i;
			LVITEM item = {0};
			item.mask = LVIF_PARAM;
			
			for (i = 0; i < count; i++)
			{
				item.iItem = i;
				ListView_GetItem(hList, &item);
				RefreshServiceInfo(hList, i, (PService) item.lParam);
			}		
		
			break;
		}
		
		case SLM_ADDNEWSERVICES:
		{
			LoadServices(hWnd, 0);
		
			break;
		}
		
		case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->idFrom)
			{
				case 0:
				{
					switch (((LPNMHDR)lParam)->code)
					{
						case PSN_APPLY:
						{
							int mode = SendDlgItemMessage(hWnd, IDC_DISCOVERY_MODE, CB_GETCURSEL, 0, 0);
							DBWriteContactSettingByte(NULL, ModuleName, "DiscoveryMode", mode);
						
							break;
						}
					}
					
					break;
				}
				
				case IDC_LIST_SERVICES:
				{
					switch (((LPNMHDR)lParam)->code)
					{
						case LVN_COLUMNCLICK:
						{
							LPNMLISTVIEW lv = (LPNMLISTVIEW) lParam;
							int column = lv->iSubItem;
							int i;
							SortParams params = {0};
							params.hList = GetDlgItem(hWnd, IDC_LIST_SERVICES);
							params.visibleColumn = params.fullColumn = column;
							for (i = 0; i <= column; i++)
							{
								if (!IsColumnVisible(i))
								{
									params.fullColumn++;
								}
							}
							ListView_SortItemsEx(params.hList, ServicesCompare, (LPARAM) &params);
							lastColumn = (params.visibleColumn == lastColumn) ? -1 : params.visibleColumn;

							break;
						}
					}
				
					break;
				}
			}
			
			break;
		}
	}
	
	return 0;
}