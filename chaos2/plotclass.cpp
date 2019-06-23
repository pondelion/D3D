#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "plotclass.h"
#include "test1.h"

#pragma comment(lib, "comctl32.lib")


WNDPROC orgEditOrgXProc,orgEditOrgYProc,orgStaticProc;

myplot::myplot()
{
	hInst = NULL;
	hwndPlot = NULL;
	hwndParent = NULL;
	hwndParamDlg = NULL;
	hwndStatic = NULL;
	hTab = NULL;
}

myplot::myplot(HINSTANCE hInst, HWND hwnd)
{
	this->hInst = hInst;
	hwndParent = hwnd;
	hwndPlot = NULL;
	hwndParamDlg = NULL;
	hwndEditOrgX = NULL;
	hwndEditOrgY = NULL;
	hTab = NULL;
	hwndStatic = NULL;
	hokButton = NULL;
	hcaButton = NULL;
	hapButton = NULL;
	scale = 1.0;
	bGridLine = true;
	GetWindowRect(hwndParent, &parentpos);
	param.parentpos = &parentpos;
	param.hwndPlot = &hwndPlot;
	param.hwndParent = hwndParent;
	param.hTab = &hTab;
	param.hwndEditOrgX = &hwndEditOrgX;
	param.hwndEditOrgY = &hwndEditOrgY;
	param.hwndStatic = &hwndStatic;
	param.hokButton = &hokButton;
	param.hcaButton = &hcaButton;
	param.hapButton = &hapButton;
	hTab = NULL;
	hwndStatic = NULL;
}

HWND myplot::InitializePlot(RECT* rc)
{
	WNDCLASSEX wc;
	
	
	pos.left = rc->left;
	pos.right = rc->right;
	pos.top = rc->top;
	pos.bottom = rc->bottom;

	origin.x = 20;
	origin.y = 20;
	
	param.origin = &origin;
	
	wc.cbSize 		= sizeof(WNDCLASSEX);
	wc.style 		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc 	= PlotProc;
	wc.cbClsExtra 	= 0;
	wc.cbWndExtra 	= 0;
	wc.hInstance 	= hInst;
	wc.hIcon 		= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor 		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "plotclass";
	wc.hIconSm = LoadIcon(hInst, "MYICON");
	
	if(!RegisterClassEx(&wc))
	{
		
		MessageBox(NULL, "RegisterClassEx Failed", "", MB_OK);
		return 0;
	}
	
	wc.lpfnWndProc 	= myParameterProc;
	wc.lpszClassName = "plotparameterclass";
	
	if(!RegisterClassEx(&wc))
	{
		
		MessageBox(NULL, "RegisterClassEx Failed", "", MB_OK);
		return 0;
	}
	
		
	/*
	hwndPlot = CreateWindow("EDIT", "",
		WS_CHILD | WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		rc->left, rc->top, rc->right, rc->bottom,
		hwndParent,
		(HMENU)1,
		hInst,
		NULL);
		*/
		
	
	
	param.hwndPlot = &hwndPlot;
	param.pos = &pos;
	param.hwndParent = hwndParent;
	param.hwndParam = &hwndParamDlg;
	
		
	//orgProc = (WNDPROC)GetWindowLong(hwndPlot, GWL_WNDPROC);
	
	//param.orgProc = orgProc;
	//orgpro = (WNDPROC)GetWindowLong(hwndPlot, GWL_WNDPROC);
	//SetWindowLong(hwndPlot, GWL_WNDPROC, (LONG)PlotProc);
	
	
	return create(rc);
	
}

LRESULT CALLBACK PlotProc(HWND hwnd, UINT msg, WPARAM wp , LPARAM lp)
{
	
	static MYPARAM* param;
	RECT clientparent,rmenuparent;
	int hpa,hpl;
	
	//HDC hdc;
	//PAINTSTRUCT ps;
	
	char szbuf1[32] = "%d,%d,%d,%d", szbuf2[32];
	
	switch(msg)
	{
	
	case WM_MYPARAM:
	
		param = (MYPARAM*)lp;
		
		
		break;
	
	/*case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDM_END:
			//SetWindowLong(hwnd, GWL_WNDPROC, (LONG)param->orgProc);
			//SetWindowLong(hwnd, GWL_WNDPROC, (LONG)orgpro);
			DestroyWindow(hwnd);
			*param->hwndPlot = NULL;
			break;
		case IDM_MINIMIZE:
			break;
		}
		break;
	*/
	case WM_PAINT:
		SendMessage(param->hwndParent, WM_PAINTPLOT, 0, 0);
		if(*param->hwndParam)
			InvalidateRect(*param->hwndParam, NULL, TRUE);
		break;
	case WM_CLOSE:
		
		GetWindowRect(param->hwndParent, param->parentpos);
		GetClientRect(param->hwndParent, &clientparent);
		hpa = (*(param->parentpos)).bottom - (*(param->parentpos)).top;
		GetWindowRect(hwnd, param->pos);
		
		//wsprintf(szbuf2, szbuf1, hmenupa,(*(param->pos)).top,(*(param->pos)).right, (*(param->pos)).bottom);
		//MessageBox(NULL, szbuf2, "", MB_OK);
		
		hpl = (*(param->pos)).bottom - (*(param->pos)).top;
		
		(*(param->pos)).right -= (*(param->pos)).left;
		(*(param->pos)).bottom -= (*(param->pos)).top;
		(*(param->pos)).left -= ((*(param->parentpos)).left + 8);
		(*(param->pos)).top = (*(param->pos)).top - (*(param->parentpos)).top - (hpa - clientparent.bottom - 10);//- hmenupa;
		
		if(*param->hokButton)
		{
			DestroyWindow(*param->hokButton);
			*param->hokButton = NULL;
		}
		if(*param->hcaButton)
		{
			DestroyWindow(*param->hcaButton);
			*param->hcaButton = NULL;
		}
		if(*param->hapButton)
		{
			DestroyWindow(*param->hapButton);
			*param->hapButton = NULL;
		}
		
		if(*param->hwndEditOrgX)
		{
			
			SetWindowLong(*param->hwndEditOrgX, GWL_WNDPROC, (LONG)orgEditOrgXProc);
			DestroyWindow(*param->hwndEditOrgX);
			*param->hwndEditOrgX = NULL;
		}
		
		if(*param->hwndEditOrgY)
		{
			
			SetWindowLong(*param->hwndEditOrgY, GWL_WNDPROC, (LONG)orgEditOrgYProc);
			DestroyWindow(*param->hwndEditOrgY);
			*param->hwndEditOrgY = NULL;
		}
		
		if(*param->hTab)
		{
			
			DestroyWindow(*param->hTab);
			*param->hTab = NULL;
			//testes;
		}
		
		
		if(*param->hwndStatic)
		{
			
			SetWindowLong(*param->hwndStatic, GWL_WNDPROC, (LONG)orgStaticProc);
			DestroyWindow(*param->hwndStatic);
			*param->hwndStatic = NULL;
		}
		
		if(*param->hwndParam)
		{
			
			DestroyWindow(*param->hwndParam);
			*param->hwndParam = NULL;
		}
		
		if(*param->hwndPlot)
		{
		testes;
		DestroyWindow(hwnd);
		*param->hwndPlot = NULL;
		}
		break;
	default:
		
		return DefWindowProc(hwnd, msg, wp, lp);
		break;
	}
	
	return DefWindowProc(hwnd, msg, wp, lp);
	
}

HWND myplot::create(RECT* rc)
{

	HMENU hMenu;
	
	if(!hwndPlot)
	{
		
	
		if(rc)
		{
			hwndPlot = CreateWindow("plotclass", "plot",
				WS_CHILD |WS_VISIBLE| WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS|WS_OVERLAPPED,
				rc->left, rc->top, rc->right, rc->bottom,
				hwndParent,
				NULL,
				hInst,
				NULL);
		}
		else
		{
			hwndPlot = CreateWindow("plotclass", "plot",
				WS_CHILD |WS_VISIBLE| WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX |WS_CLIPSIBLINGS|WS_OVERLAPPED,
				pos.left, pos.top, pos.right, pos.bottom,
				hwndParent,
				NULL,
				hInst,
				NULL);
		}
		
		
		
		if(!hwndPlot)
		{
		MessageBox(NULL, "PLOTWINDOW ì¬Ž¸”s", "", MB_OK);
		return 0;
		}
		
		GetClientRect(hwndPlot, &clientplot); 
		
		/*
		hMenu = LoadMenu(hInst, "MYMENU");
		SetMenu(hwndPlot, hMenu);
		
		param.hMenu = hMenu;
		*/
		
		SendMessage(hwndPlot, WM_MYPARAM, 0, (LPARAM)&param);
		
		return hwndPlot;
		
			
		
	}
	MessageBox(NULL, "‚·‚Å‚Éì‚ç‚ê‚Ä‚¢‚Ü‚·", "", MB_OK);
	return hwndPlot;
}

void myplot::destroy()
{

	RECT clientparent,rmenuparent;
	int hpa,hpl;
	if(hokButton)
	{
		DestroyWindow(hokButton);
		hokButton = NULL;
	}
	if(hcaButton)
	{
		DestroyWindow(hcaButton);
		hcaButton = NULL;
	}
	if(hcaButton)
	{
		DestroyWindow(hcaButton);
		hcaButton = NULL;
	}
	if(hwndEditOrgX)
	{
		SetWindowLong(hwndEditOrgX, GWL_WNDPROC, (LONG)orgEditOrgXProc);
		DestroyWindow(hwndEditOrgX);
		hwndEditOrgX = NULL;
	}
	if(hwndEditOrgY)
	{
		SetWindowLong(hwndEditOrgY, GWL_WNDPROC, (LONG)orgEditOrgYProc);
		DestroyWindow(hwndEditOrgY);
		hwndEditOrgY = NULL;
	}
	if(hTab)
	{
			DestroyWindow(hTab);
			hTab = NULL;
	}
	if(hwndStatic)
	{
		
		SetWindowLong(hwndStatic, GWL_WNDPROC, (LONG)orgStaticProc);
		DestroyWindow(hwndStatic);
		hwndStatic = NULL;
	}
	if(hwndParamDlg)
	{
	DestroyWindow(hwndParamDlg);
	hwndParamDlg = NULL;
	}
	if(hwndPlot)
	{
	
	GetWindowRect(hwndParent, &parentpos);
	GetClientRect(hwndParent, &clientparent);
	hpa = parentpos.bottom - parentpos.top;
	GetWindowRect(hwndPlot, &pos);
	
	hpl = pos.bottom - pos.top;
		
	pos.right -= pos.left;
	pos.bottom -= pos.top;
	pos.left -= (parentpos.left + 8);
	pos.top = pos.top - parentpos.top - (hpa - clientparent.bottom - 10);
	//SetWindowLong(hwndPlot, GWL_WNDPROC, (LONG)orgProc);
	/*
	if(hMenu)
	{
	SetMenu(hwndPlot, NULL);
	DestroyMenu(hMenu);
	hMenu = NULL;
	}
	*/
	DestroyWindow(hwndPlot);
	hwndPlot = NULL;
	//orgProc = NULL;
	}
}
		
void myplot::drawaxis( POINT org, int width, COLORREF col)
{
	
	HPEN hpen;
	HDC hdc;
	PAINTSTRUCT ps;
	
	
	hdc = BeginPaint(hwndPlot, &ps);
	
	hpen = CreatePen(PS_SOLID, width, col);
	SelectObject(hdc, hpen);
	
	GetClientRect(hwndPlot, &clientplot);
	
	MoveToEx(hdc, 0, clientplot.bottom - org.y, NULL);
	LineTo(hdc, clientplot.right*0.95, clientplot.bottom - org.y);
	LineTo(hdc, clientplot.right*0.9, clientplot.bottom*0.97 - org.y);
	MoveToEx(hdc, clientplot.right*0.95, clientplot.bottom - org.y, NULL);
	LineTo(hdc, clientplot.right*0.9, clientplot.bottom*1.03 - org.y);
	
	MoveToEx(hdc, org.x, clientplot.bottom, NULL);
	LineTo(hdc, org.x, clientplot.bottom*0.05);
	LineTo(hdc, org.x-(clientplot.right*0.03), clientplot.bottom*0.1);
	MoveToEx(hdc, org.x, clientplot.bottom*0.05,NULL);
	LineTo(hdc, org.x+(clientplot.right*0.03), clientplot.bottom*0.1);
	
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, clientplot.right*0.95, clientplot.bottom-org.y-20, "X", 1);
	TextOut(hdc, org.x+20, clientplot.bottom*0.05, "Y", 1);
	
	EndPaint(hwndPlot, &ps);
	DeleteObject(hpen);
	
}
	
POINT myplot::getOrigin()
{
	return origin;
}

void myplot::setOrigin(POINT pt)
{
	origin.x = pt.x;
	origin.y = pt.y;
}

void myplot::setParameterDialog()
{
	INITCOMMONCONTROLSEX ic;
	TCITEM ti;
	char orgx[32], orgy[32];
	
	if(!hwndParamDlg)
	{
		ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
		ic.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&ic);
	
		hwndParamDlg = CreateWindow("plotparameterclass", "parameter",
				WS_CHILD |WS_VISIBLE| WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX ,
				clientplot.right-250, 0, 250, 290,
				hwndPlot,
				NULL,
				hInst,
				NULL);
		param.hwndParam = &hwndParamDlg;
		GetClientRect(hwndParamDlg, &clientparam);
	
		if(!hTab)
		{
		hTab = CreateWindowEx(0, WC_TABCONTROL, "",
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE ,//| TCS_VERTICAL| TCS_RIGHT ,
			0, 0, clientparam.right, clientparam.bottom,
			hwndParamDlg, (HMENU)ID_TAB, hInst, NULL);
		
	
		param.hTab = &hTab;
		SendMessage(hTab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
	
		ti.mask = TCIF_TEXT;
		ti.pszText = "Œ´“_";
		TabCtrl_InsertItem(hTab, 0, &ti);
		ti.pszText = "Ž²";
		TabCtrl_InsertItem(hTab, 1, &ti);
		}
	
		if(!hwndStatic)
		{
		hwndStatic = CreateWindowEx(0, "Static", "", WS_CHILD | WS_VISIBLE , 
			0, 0, clientparam.right, clientparam.bottom, 
			hwndParamDlg, (HMENU)ID_STATIC, hInst, NULL);
		
	
		param.hwndStatic = &hwndStatic;
	
		orgStaticProc = (WNDPROC)GetWindowLong(hwndStatic, GWL_WNDPROC);
		//param.orgStaticProc = &orgStaticProc;
		SetWindowLong(hwndStatic, GWL_WNDPROC, (LONG)myStaticProc);
		SendMessage(hwndStatic, WM_MYPARAM, 0, (LPARAM)&param);
		}
		
		_itoa_s(origin.x, orgx, sizeof(orgx), 10);
		_itoa_s(origin.y, orgy, sizeof(orgy), 10);
		if(!hwndEditOrgX)
		{
		hwndEditOrgX = CreateWindowEx(0, "Edit", "", 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP  |WS_BORDER | ES_AUTOVSCROLL,
			100, 40, 60, 25, 
			hwndStatic/*hwndParamDlg*/, (HMENU)ID_EDIT1, hInst, NULL);
			
			Edit_SetText(hwndEditOrgX, orgx);
		param.hwndEditOrgX = &hwndEditOrgX;
		orgEditOrgXProc = (WNDPROC)GetWindowLong(hwndEditOrgX, GWL_WNDPROC);
		SetWindowLong(hwndEditOrgX, GWL_WNDPROC, (LONG)myEditOrgXProc);
		}
		
		if(!hwndEditOrgY)
		{
		hwndEditOrgY = CreateWindowEx(0, "Edit", "", 
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER ,
			100, 70, 60, 25, 
			hwndStatic/*hwndParamDlg*/, (HMENU)ID_EDIT2, hInst, NULL);
		
		Edit_SetText(hwndEditOrgY, orgy);
		param.hwndEditOrgY = &hwndEditOrgY;
		orgEditOrgYProc = (WNDPROC)GetWindowLong(hwndEditOrgY, GWL_WNDPROC);
		SetWindowLong(hwndEditOrgY, GWL_WNDPROC, (LONG)myEditOrgYProc);
		}
		
		if(!hokButton)
		{
		
		hokButton = CreateWindowEx(0, "BUTTON", "OK",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_PUSHBUTTON,
			10, clientparam.bottom - 30, 60, 20, 
			hwndStatic, (HMENU)ID_BUTTON1, hInst, NULL);
		}
		
		if(!hcaButton)
		{
		
		hcaButton = CreateWindowEx(0, "BUTTON", "CANCEL",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_PUSHBUTTON,
			80, clientparam.bottom - 30, 60, 20, 
			hwndStatic, (HMENU)ID_BUTTON2, hInst, NULL);
		}
		
		if(!hapButton)
		{
		
		hapButton = CreateWindowEx(0, "BUTTON", "APPLY",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_PUSHBUTTON,
			150, clientparam.bottom - 30, 60, 20, 
			hwndStatic, (HMENU)ID_BUTTON3, hInst, NULL);
		}
		
		
		SendMessage(hwndParamDlg, WM_MYPARAM, 0, (LPARAM)&param);
	
		return;
	}
	MessageBox(hwndParent, "‚·‚Å‚Éì‚ç‚ê‚Ä‚¢‚Ü‚·", "", MB_OK);
}

LRESULT CALLBACK myParameterProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static MYPARAM* param;
	RECT clientparam;
	int tab;
	HDC hdc;
	PAINTSTRUCT ps;
	char orgx[32],orgy[32];
	
	switch(msg)
	{
	case WM_MYPARAM:
	
		param = (MYPARAM*)lp;
		
		break;
	case WM_NOTIFY:
		switch(((NMHDR *)lp)->code)
		{
		case TCN_SELCHANGE:
			
			
			tab = TabCtrl_GetCurSel(*param->hTab);
			switch(tab)
			{
			case 0:
				
				if(*param->hwndEditOrgX && *param->hwndEditOrgY)
				{
				
				_itoa_s(param->origin->x, orgx, sizeof(orgx), 10);
				_itoa_s(param->origin->y, orgy, sizeof(orgy), 10);
				Edit_SetText(*param->hwndEditOrgX, orgx);
				Edit_SetText(*param->hwndEditOrgY, orgy);
				ShowWindow(*param->hwndEditOrgX, SW_SHOW);
				ShowWindow(*param->hwndEditOrgY, SW_SHOW);
				}
				
				break;
			case 1:
				break;
			}
			
			InvalidateRect(*param->hTab, NULL, TRUE);
			//InvalidateRect(hwnd, NULL, TRUE);
			InvalidateRect(*param->hwndStatic, NULL, TRUE);
			//SendMessage(*param->hwndStatic, WM_PAINT, 0, 0);
			
			SetFocus(*param->hwndEditOrgX);
			/*
			GetClientRect(hwnd, &clientparam);
			TabCtrl_AdjustRect(*param->hTab, FALSE, &clientparam);
			MoveWindow(*param->hTab, 1 ,1, clientparam.right, clientparam.bottom, TRUE);
				
			MoveWindow(*param->hwndStatic, 0 ,0, clientparam.right, clientparam.bottom, TRUE);
			*/	
			break;
		case TCN_SELCHANGING:
			tab = TabCtrl_GetCurSel(*param->hTab);
			switch(tab)
			{
			case 0:
				
				
				if(*param->hwndEditOrgX && *param->hwndEditOrgY)
				{
				
				Edit_GetText(*param->hwndEditOrgX, orgx, sizeof(orgx));
				Edit_GetText(*param->hwndEditOrgY, orgy, sizeof(orgy));
				param->origin->x = atoi(orgx);
				param->origin->y = atoi(orgy);
				}
				
				ShowWindow(*param->hwndEditOrgX, SW_HIDE);
				ShowWindow(*param->hwndEditOrgY, SW_HIDE);
				InvalidateRect(*param->hwndPlot, NULL, TRUE);
				
				break;
			case 1:
				break;
			}
			break;
		}
		break;
	case WM_SIZE:
		
		
		if(param)
		{
			GetClientRect(hwnd, &clientparam);
			if(*param->hTab)
			{
				
				
				TabCtrl_AdjustRect(*param->hTab, FALSE, &clientparam);
				MoveWindow(*param->hTab, 1 ,1, LOWORD(lp), HIWORD(lp), TRUE);
			}
			if(*param->hwndStatic)
				MoveWindow(*param->hwndStatic, 0 ,0, LOWORD(lp), HIWORD(lp), TRUE);
			if(*param->hokButton)
				MoveWindow(*param->hokButton, 10, clientparam.bottom - 30, 60, 20, TRUE);
			if(*param->hcaButton)
				MoveWindow(*param->hcaButton, 80, clientparam.bottom - 30, 60, 20, TRUE);
			if(*param->hapButton)
				MoveWindow(*param->hapButton, 150, clientparam.bottom - 30, 60, 20, TRUE);
			
		}
		
		
		break;
	
	case WM_CLOSE:
		if(*param->hokButton)
		{
			DestroyWindow(*param->hokButton);
			*param->hokButton = NULL;
		}
		if(*param->hcaButton)
		{
			DestroyWindow(*param->hcaButton);
			*param->hcaButton = NULL;
		}
		if(*param->hapButton)
		{
			DestroyWindow(*param->hapButton);
			*param->hapButton = NULL;
		}
		if(*param->hwndEditOrgX)
		{
			SetWindowLong(*param->hwndEditOrgX, GWL_WNDPROC, (LONG)orgEditOrgXProc);
			DestroyWindow(*param->hwndEditOrgX);
			*param->hwndEditOrgX = NULL;
		}
		if(*param->hwndEditOrgY)
		{
			SetWindowLong(*param->hwndEditOrgY, GWL_WNDPROC, (LONG)orgEditOrgYProc);
			DestroyWindow(*param->hwndEditOrgY);
			*param->hwndEditOrgY = NULL;
		}
		if(*param->hTab)
		{
			DestroyWindow(*param->hTab);
			*param->hTab = NULL;
		}
		if(*param->hwndStatic)
		{
			SetWindowLong(*param->hwndStatic, GWL_WNDPROC, (LONG)orgStaticProc);
			DestroyWindow(*param->hwndStatic);
			*param->hwndStatic = NULL;
		}
		DestroyWindow(hwnd);
		*param->hwndParam = NULL;
		break;
		
	default:
		return DefWindowProc(hwnd , msg, wp, lp);
		break;
	}
	return DefWindowProc(hwnd , msg, wp, lp);
}

LRESULT CALLBACK myStaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int tab;
	static MYPARAM* param;
	char orgx[32],orgy[32];

	
	switch(msg)
	{
	case WM_MYPARAM:
	
		param = (MYPARAM *)lp;
		
		break;
	case WM_PAINT:
		
		if(param)
		{
			
			if(*param->hTab)
			{
			
			hdc = BeginPaint(hwnd, &ps);
			tab = TabCtrl_GetCurSel(*param->hTab);
			SetBkMode(hdc, TRANSPARENT);
			switch(tab)
			{
			case 0:
					//testes;
					Rectangle(hdc, 5 ,35 , 175, 100);
					TextOut(hdc, 15, 40, "Origin X", 8);
					TextOut(hdc, 15, 70, "       Y", 8);
					
					break;
			case 1:
					
					TextOut(hdc, 50, 50, "Temp", 4);
					break;
			}
			EndPaint(hwnd, &ps);
			}
		}
		break;
	case WM_COMMAND:
		tab = TabCtrl_GetCurSel(*param->hTab);
		switch(LOWORD(wp))
		{
		case ID_BUTTON1:
			switch(tab)
			{
			case 0:
				if(*param->hwndEditOrgX && *param->hwndEditOrgY)
				{
				
				Edit_GetText(*param->hwndEditOrgX, orgx, sizeof(orgx));
				Edit_GetText(*param->hwndEditOrgY, orgy, sizeof(orgy));
				param->origin->x = atoi(orgx);
				param->origin->y = atoi(orgy);
				}
				break;
			case 1:
			
				break;
			}
			
				if(*param->hokButton)
				{
				DestroyWindow(*param->hokButton);
				*param->hokButton = NULL;
				}
				if(*param->hcaButton)
				{
				DestroyWindow(*param->hcaButton);
				*param->hcaButton = NULL;
				}
				if(*param->hapButton)
				{
				DestroyWindow(*param->hapButton);
				*param->hapButton = NULL;
				}
				if(*param->hwndEditOrgX)
				{
				SetWindowLong(*param->hwndEditOrgX, GWL_WNDPROC, (LONG)orgEditOrgXProc);
				DestroyWindow(*param->hwndEditOrgX);
				*param->hwndEditOrgX = NULL;
				}
				if(*param->hwndEditOrgY)
				{
				SetWindowLong(*param->hwndEditOrgY, GWL_WNDPROC, (LONG)orgEditOrgYProc);
				DestroyWindow(*param->hwndEditOrgY);
				*param->hwndEditOrgY = NULL;
				}
				if(*param->hTab)
				{
				DestroyWindow(*param->hTab);
				*param->hTab = NULL;
				}
				if(*param->hwndStatic)
				{
				SetWindowLong(*param->hwndStatic, GWL_WNDPROC, (LONG)orgStaticProc);
				DestroyWindow(*param->hwndStatic);
				*param->hwndStatic = NULL;
				}
				if(*param->hwndParam)
				{
				DestroyWindow(*param->hwndParam);
				*param->hwndParam = NULL;
				}
				InvalidateRect(*param->hwndPlot, NULL, TRUE);
			break;
		case ID_BUTTON2:
			if(*param->hokButton)
				{
				DestroyWindow(*param->hokButton);
				*param->hokButton = NULL;
				}
				if(*param->hcaButton)
				{
				DestroyWindow(*param->hcaButton);
				*param->hcaButton = NULL;
				}
				if(*param->hapButton)
				{
				DestroyWindow(*param->hapButton);
				*param->hapButton = NULL;
				}
				if(*param->hwndEditOrgX)
				{
				SetWindowLong(*param->hwndEditOrgX, GWL_WNDPROC, (LONG)orgEditOrgXProc);
				DestroyWindow(*param->hwndEditOrgX);
				*param->hwndEditOrgX = NULL;
				}
				if(*param->hwndEditOrgY)
				{
				SetWindowLong(*param->hwndEditOrgY, GWL_WNDPROC, (LONG)orgEditOrgYProc);
				DestroyWindow(*param->hwndEditOrgY);
				*param->hwndEditOrgY = NULL;
				}
				if(*param->hTab)
				{
				DestroyWindow(*param->hTab);
				*param->hTab = NULL;
				}
				if(*param->hwndStatic)
				{
				SetWindowLong(*param->hwndStatic, GWL_WNDPROC, (LONG)orgStaticProc);
				DestroyWindow(*param->hwndStatic);
				*param->hwndStatic = NULL;
				}
				if(*param->hwndParam)
				{
				DestroyWindow(*param->hwndParam);
				*param->hwndParam = NULL;
				}
			break;
		
		case ID_BUTTON3:
			switch(tab)
			{
			case 0:
				if(*param->hwndEditOrgX && *param->hwndEditOrgY)
				{
				
				Edit_GetText(*param->hwndEditOrgX, orgx, sizeof(orgx));
				Edit_GetText(*param->hwndEditOrgY, orgy, sizeof(orgy));
				param->origin->x = atoi(orgx);
				param->origin->y = atoi(orgy);
				}
				InvalidateRect(*param->hwndPlot, NULL, TRUE);
				break;
			case 1:
			
				break;
			}
			
			break;
		}
		break;
				
				
	default:
		return CallWindowProc(orgStaticProc, hwnd, msg, wp, lp);
	}
	return 0;
}
		

LRESULT CALLBACK myEditOrgXProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int l;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		l = GetWindowTextLength(hwnd);
		SendMessage(hwnd, EM_SETSEL, l,l);
		SetFocus(hwnd);
		break;
	}
	
	return CallWindowProc(orgEditOrgXProc, hwnd, msg, wp, lp);
}
	
LRESULT CALLBACK myEditOrgYProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int l;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		l = GetWindowTextLength(hwnd);
		SendMessage(hwnd, EM_SETSEL, l,l);
		SetFocus(hwnd);
		break;
	}
	
	return CallWindowProc(orgEditOrgYProc, hwnd, msg, wp, lp);
}