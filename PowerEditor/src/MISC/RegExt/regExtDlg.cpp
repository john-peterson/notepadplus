// This file is part of Notepad++ project
// Copyright (C)2003 Don HO <don.h@free.fr>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// Note that the GPL places important restrictions on "derived works", yet
// it does not provide a detailed definition of that term.  To avoid      
// misunderstandings, we consider an application to constitute a          
// "derivative work" for the purpose of this license if it does any of the
// following:                                                             
// 1. Integrates source code from Notepad++.
// 2. Integrates/includes/aggregates Notepad++ into a proprietary executable
//    installer, such as those produced by InstallShield.
// 3. Links to a library or executes a program that does any of the above.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "precompiledHeaders.h"
#include "regExtDlg.h"
#include "resource.h"
#include <resource.h>
using namespace std;

const TCHAR *nppEdit = TEXT("edit_npp");
const TCHAR *nppEditName = TEXT("Edit with Notepad++");
const TCHAR *nppExe = TEXT("notepad++.exe");
const TCHAR *nppName = TEXT("Notepad++_file");
const TCHAR *nppBackup = TEXT("Notepad++_backup");
const TCHAR *nppDoc = TEXT("Notepad++ Document");

const int nbSupportedLang = 9;
const int nbExtMax = 10;
const int extNameMax = 18;

TCHAR defExtArray[nbSupportedLang][nbExtMax][extNameMax] = {
	{TEXT("Notepad"),        			TEXT(".txt"), TEXT(".log"), TEXT(".ini")},
	{TEXT("c, c++, objc"),   	TEXT(".h"), TEXT(".hpp"), TEXT(".hxx"), TEXT(".c"), TEXT(".cpp"), TEXT(".cxx"), TEXT(".cc"), TEXT(".m")},
	{TEXT("java, c#, pascal"), 		TEXT(".java"), TEXT(".cs"), TEXT(".pas"), TEXT(".inc")},
	{TEXT("web(html) script"),   TEXT(".html"), TEXT(".htm"), TEXT(".php"), TEXT(".phtml"), TEXT(".js"), TEXT(".jsp"), TEXT(".asp"), TEXT(".css"), TEXT(".xml")},
	{TEXT("public script"),		TEXT(".sh"), TEXT(".bsh"), TEXT(".nsi"), TEXT(".nsh"), TEXT(".lua"), TEXT(".pl"), TEXT(".pm"), TEXT(".py")},
	{TEXT("property script"),	TEXT(".rc"), TEXT(".as"), TEXT(".mx"), TEXT(".vb"), TEXT(".vbs")},
	{TEXT("fortran, TeX, SQL"),			TEXT(".f"),  TEXT(".for"), TEXT(".f90"),  TEXT(".f95"), TEXT(".f2k"), TEXT(".tex"), TEXT(".sql")},
	{TEXT("misc"),								TEXT(".nfo"), TEXT(".mak")},
	{TEXT("customize")}
};

void RegExtDlg::doDialog(bool isRTL) 
{
	if (isRTL)
	{
		DLGTEMPLATE *pMyDlgTemplate = NULL;
		HGLOBAL hMyDlgTemplate = makeRTLResource(IDD_REGEXT_BOX, &pMyDlgTemplate);
		::DialogBoxIndirectParam(_hInst, pMyDlgTemplate, _hParent,  (DLGPROC)dlgProc, (LPARAM)this);
		::GlobalFree(hMyDlgTemplate);
	}
	else
		::DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_REGEXT_BOX), _hParent,  (DLGPROC)dlgProc, (LPARAM)this);
};

BOOL CALLBACK RegExtDlg::run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_INITDIALOG :
		{
			getRegisteredExts();
			getDefSupportedExts();
			::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGOPENEXT_BUTTON), false);
			::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEDITEXT_BUTTON), false);
			::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), false);
			::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, EM_SETLIMITTEXT, extNameMax-1, 0);

			return TRUE;
		}

		case WM_DRAWITEM :
		{
			TCHAR szExt[extNameMax] = L"";
			int isEdit;

			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
			COLORREF OldTxtClr = GetTextColor(lpdis->hDC);
			COLORREF OldBkClr = GetBkColor(lpdis->hDC);

			SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETTEXT, lpdis->itemID, (LPARAM)szExt);
			isEdit = SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETITEMDATA, lpdis->itemID, 0);					

			if (lpdis->itemState & ODS_SELECTED)
			{
				SetTextColor(lpdis->hDC, RGB(0xff,0xff,0xff));
				SetBkColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHT));			
			}
			else
			{
				if (isEdit)
					SetTextColor (lpdis->hDC, RGB(0,0,0xff));
				else
					SetTextColor (lpdis->hDC, RGB(0,0,0));
				FillRect (lpdis->hDC, &(lpdis->rcItem), (HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			TextOut(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, szExt, extNameMax);

			SetTextColor(lpdis->hDC, OldTxtClr);
			SetBkColor(lpdis->hDC, OldBkClr);

			return TRUE;
		}

		case WM_COMMAND : 
		{
			switch (wParam)
			{
				case IDC_ADDFROMLANGOPENEXT_BUTTON :
				case IDC_ADDFROMLANGEDITEXT_BUTTON :
				{
					writeNppPath();

					TCHAR ext2Add[extNameMax] = TEXT("");
					if (!_isCustomize)
					{
						int index2Add = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETCURSEL, 0, 0);
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETTEXT, index2Add, (LPARAM)ext2Add);
						addExt(ext2Add, wParam == IDC_ADDFROMLANGEDITEXT_BUTTON);
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_DELETESTRING, index2Add, 0);
					}
					else
					{
						::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_GETTEXT, extNameMax, (LPARAM)ext2Add);
						int i = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_FINDSTRINGEXACT, 0, (LPARAM)ext2Add);
						if (i != LB_ERR) 
							return TRUE;
						addExt(ext2Add, wParam == IDC_ADDFROMLANGEDITEXT_BUTTON);
						::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_SETTEXT, 0, (LPARAM)TEXT(""));
					}
					::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_INSERTSTRING, 0, (LPARAM)ext2Add);
					::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_SETITEMDATA, 0, wParam == IDC_ADDFROMLANGOPENEXT_BUTTON ? 0 : 1);
					::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGOPENEXT_BUTTON), false);
					::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEDITEXT_BUTTON), false);

					return TRUE;
				}

				case IDC_REMOVEEXT_BUTTON :
				{
					TCHAR ext2Sup[extNameMax] = TEXT("");
					int index2Sup = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETCURSEL, 0, 0);
					::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETTEXT, index2Sup, (LPARAM)ext2Sup);
					int edit = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETTEXT, LB_GETITEMDATA, (LPARAM)0);
					if (deleteExts(ext2Sup, edit != 0))
						::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_DELETESTRING, index2Sup, 0);
					int langIndex = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANG_LIST, LB_GETCURSEL, 0, 0);

					::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), false);

					if (langIndex != LB_ERR)
					{
						for (int i = 1 ; i < nbExtMax ; i++)
						{
							if (!generic_stricmp(ext2Sup, defExtArray[langIndex][i]))
							{
								::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_ADDSTRING, 0, (LPARAM)ext2Sup);
								return TRUE;
							}
						}
					}
					return TRUE;
				}

				case IDCANCEL :
					::EndDialog(_hSelf, 0);
					return TRUE;

			}

			if (HIWORD(wParam) == EN_CHANGE)
            {
				TCHAR text[extNameMax] = TEXT("");
				::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_GETTEXT, extNameMax, (LPARAM)text);
				if ((lstrlen(text) == 1) && (text[0] != '.'))
				{
					text[1] = text[0];
					text[0] = '.';
					text[2] = '\0';
					::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, WM_SETTEXT, 0, (LPARAM)text);
					::SendDlgItemMessage(_hSelf, IDC_CUSTOMEXT_EDIT, EM_SETSEL, 2, 2);
				}
				::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGOPENEXT_BUTTON), (lstrlen(text) > 1));
				::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEDITEXT_BUTTON), (lstrlen(text) > 1));
				return TRUE;
			}

			if (HIWORD(wParam) == LBN_SELCHANGE)
            {
				int i = ::SendDlgItemMessage(_hSelf, LOWORD(wParam), LB_GETCURSEL, 0, 0);
				if (LOWORD(wParam) == IDC_REGEXT_LANG_LIST)
				{
					if (i != LB_ERR)
					{
						TCHAR itemName[32];
						::SendDlgItemMessage(_hSelf, LOWORD(wParam), LB_GETTEXT, i, (LPARAM)itemName);

						if (!generic_stricmp(defExtArray[nbSupportedLang-1][0], itemName))
						{
							::ShowWindow(::GetDlgItem(_hSelf, IDC_REGEXT_LANGEXT_LIST), SW_HIDE);
							::ShowWindow(::GetDlgItem(_hSelf, IDC_CUSTOMEXT_EDIT), SW_SHOW);
							_isCustomize = true;
						}
						else
						{
							if (_isCustomize)
							{
								::ShowWindow(::GetDlgItem(_hSelf, IDC_REGEXT_LANGEXT_LIST), SW_SHOW);
								::ShowWindow(::GetDlgItem(_hSelf, IDC_CUSTOMEXT_EDIT), SW_HIDE);
								
								_isCustomize = false;
							}
							int count = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_GETCOUNT, 0, 0);
							for (count -= 1 ; count >= 0 ; count--)
								::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_DELETESTRING, count, 0);

							for (int j = 1 ; j < nbExtMax ; j++)
								if (lstrcmp(TEXT(""), defExtArray[i][j]))
								{
									int index = ::SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_FINDSTRINGEXACT, 0, (LPARAM)defExtArray[i][j]);
									if (index == -1)
										::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANGEXT_LIST, LB_ADDSTRING, 0, (LPARAM)defExtArray[i][j]);
								}
						}

						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGOPENEXT_BUTTON), false);
						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEDITEXT_BUTTON), false);
					}
				}
				
				else if (LOWORD(wParam) == IDC_REGEXT_LANGEXT_LIST)
				{
					if (i != LB_ERR)
					{
						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGOPENEXT_BUTTON), true);
						::EnableWindow(::GetDlgItem(_hSelf, IDC_ADDFROMLANGEDITEXT_BUTTON), true);
					}

				}

				else if (LOWORD(wParam) == IDC_REGEXT_REGISTEREDEXTS_LIST)
				{
					if (i != LB_ERR)
						::EnableWindow(::GetDlgItem(_hSelf, IDC_REMOVEEXT_BUTTON), true);
				}
			}
		}
		default :
			return FALSE;
	}
}

wstring RegExtDlg::GetOpenCommand()
{
	TCHAR nppPath[MAX_PATH];
	GetModuleFileName(_hInst, nppPath, MAX_PATH);
	return L"\"" + wstring(nppPath) + L"\" \"%1\"";
}

wstring RegExtDlg::GetIconPath()
{
	TCHAR nppPath[MAX_PATH];
	GetModuleFileName(_hInst, nppPath, MAX_PATH);
	return L"\"" + wstring(nppPath) + L"\",0";
}

void RegExtDlg::getRegisteredExts()
{
	int nbRegisteredKey = getNbSubKey(HKEY_CLASSES_ROOT);
	for (int i = 0 ; i < nbRegisteredKey ; i++)
	{
		TCHAR extName[extNameLen];
		int extNameActualLen = extNameLen;
		if (RegEnumKeyEx(HKEY_CLASSES_ROOT, i, extName, (LPDWORD)&extNameActualLen, NULL, NULL, NULL, NULL)) continue;
		if (extName[0] == '.')
		{
			wstring defValue, openValue, editValue;
			if (RegQueryValueEx2(HKEY_CLASSES_ROOT, extName, L"", defValue)) continue;
			RegQueryValueEx2(HKEY_CLASSES_ROOT, defValue + L"\\shell\\open\\command", L"", openValue);
			RegQueryValueEx2(HKEY_CLASSES_ROOT, defValue + L"\\shell\\" + nppEdit + L"\\command", L"", editValue);
			if (defValue.find(nppName) != string::npos || openValue.find(nppExe) != string::npos)
			{
				SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_ADDSTRING, (WPARAM)0, (LPARAM)extName);
				int iCount = SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_SETITEMDATA, (WPARAM)(iCount-1), (LPARAM)0);
				continue;
			}
			if (editValue.find(nppExe) != string::npos)
			{
				SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_ADDSTRING, (WPARAM)0, (LPARAM)extName);				
				int iCount = SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				SendDlgItemMessage(_hSelf, IDC_REGEXT_REGISTEREDEXTS_LIST, LB_SETITEMDATA, (WPARAM)(iCount-1), (LPARAM)1);
			}
		}
	}
}

void RegExtDlg::getDefSupportedExts()
{
	for (int i = 0 ; i < nbSupportedLang ; i++)
		::SendDlgItemMessage(_hSelf, IDC_REGEXT_LANG_LIST, LB_ADDSTRING, 0, (LPARAM)defExtArray[i][0]);
}

void RegExtDlg::addExt(TCHAR* ext, bool edit)
{
	wstring fileType, subKey, value;

	// if ext exist read fileType
	if (RegQueryValueEx2(HKEY_CLASSES_ROOT, ext, L"", fileType))
	// else create ext
		if (RegCreateKeyEx2(HKEY_CLASSES_ROOT, ext)) return;

	// write to ext instead of fileType
	if (fileType.empty())
	{
		RegSetValueEx2(HKEY_CLASSES_ROOT, ext, L"", nppName);
		return;
	}

	subKey = fileType + L"\\shell\\" + (edit ? nppEdit : L"open");
	// check for existing value
	if (RegQueryValueEx2(HKEY_CLASSES_ROOT, subKey.c_str(), L"", value))
		if (RegCreateKeyEx2(HKEY_CLASSES_ROOT, subKey.c_str())) return;
	// backup existing value
	if (!value.empty())
		RegSetValueEx2(HKEY_CLASSES_ROOT, subKey.c_str(), nppBackup, value);
	// create 'nppEditName\command'
	if (edit) {
		RegSetValueEx2(HKEY_CLASSES_ROOT, subKey.c_str(), L"", nppEditName);
		if (RegCreateKeyEx2(HKEY_CLASSES_ROOT, subKey + L"\\command")) return;
	}
	// write command
	RegSetValueEx2(HKEY_CLASSES_ROOT, subKey + L"\\command", L"", GetOpenCommand());
	return;
}

bool RegExtDlg::deleteExts(const TCHAR* ext, bool edit)
{
	wstring fileType, subKey, value;
	if (RegQueryValueEx2(HKEY_CLASSES_ROOT, ext, L"", fileType)) return false;
	if (fileType.find(nppName) != string::npos)
	{
		RegDeleteKey2(HKEY_CLASSES_ROOT, ext);
		return true;
	}
	subKey = fileType + L"\\shell\\" + (edit ? nppEdit : L"open") + L"\\command";
	RegQueryValueEx2(HKEY_CLASSES_ROOT, subKey.c_str(), nppBackup, value);
	if (RegSetValueEx2(HKEY_CLASSES_ROOT, subKey.c_str(), L"", value)) return false;
	RegDeleteValue2(HKEY_CLASSES_ROOT, subKey.c_str(), nppBackup);
	return true;
}

void RegExtDlg::writeNppPath()
{
	RegCreateKeyEx2(HKEY_CLASSES_ROOT, nppName);
	RegSetValueEx2(HKEY_CLASSES_ROOT, nppName, L"", nppDoc);

	wstring regStr = nppName + wstring(L"\\shell\\open\\command");
	RegCreateKeyEx2(HKEY_CLASSES_ROOT, regStr);
	RegSetValueEx2(HKEY_CLASSES_ROOT, regStr, L"", GetOpenCommand());

	regStr = nppName + wstring(L"\\DefaultIcon");
	RegCreateKeyEx2(HKEY_CLASSES_ROOT, regStr);
	RegSetValueEx2(HKEY_CLASSES_ROOT, regStr, L"", GetIconPath());
} 