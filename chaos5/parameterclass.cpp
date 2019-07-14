#include <windows.h>
#include "parameterclass.h"


LRESULT CALLBACK parameterProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{

	switch(msg)
	{
	case WM_CLOSE:
		RECT pr,cpr;
		int hp;
		
		if(*mParentWnd)
		{
			GetWindowRect(*mParentWnd, &pr);
			GetClientRect(*mParentWnd, &cpr);
		}
		if(mParamWnd.hwnd)
			GetWindowRect(mParamWnd.hwnd, &mPos);
			
		hp = pr.bottom - pr.top;
		
		mPos.right -= mPos.left;
		mPos.bottom -= mPos.top;
		mPos.left -= pr.left + 8;
		mPos.top -= mPos.top - pr.top - (hp - cpr.bottom - 10);
		
		destroyHwnd(&mParamWnd);
		
		break;
	
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
		
}

parameterclass::parameterclass(HINSTANCE hInst,HWND *parentWnd, RECT& pos)
{
	mParentWnd = parentWnd;
	mPos = pos;
	mHinst = hInst;
	
	mParamWnd.hwnd = NULL;
	mParamWnd.func = NULL;
	
	
}

paramterclass::~parameterclass()
{
	destroyHwnd(&mParamWnd);
}

void parameterclass::destroyHwnd(MYHWND *myhwnd)
{
	

	for(int i=0;i < myhwnd->childWnd.size(); ++i)
	{
		if(*(myhwnd->childWnd(i)->hwnd) != NULL)
		{
			releaseHwnd(myhwnd->childWnd(i), myhwnd);
		}
	}
	
	if(myhwnd->func())
		myhwnd->func();
	DestroyWindow(myhwnd->hwnd);
	
	myhwnd->hwnd = NULL;
	
	return;
}

void parameterclass::initParameterWindow()
{
	WNDCLASSEX wc;
	
	wc.cbSize 		= sizeof(WNDCLASSEX);
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= parameterProc;
	wc.cbClsExtra 	= 0;
	wc.cbWndExtra 	= 0;
	wc.hInstance 	= mHinst;
	wc.hIcon		= LoadIcon(NULL, IDI_APPLICATION);
	wc.HCursor		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName	= NULL;
	wc.lpszClassName = "parameterclass";
	wc.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	
	if(!RegisterClassEx(&rc))
	{
		MessageBox(NULL, "parameterclass RegisterClassEx() failed", "", MB_OK);
		return ;
	}
	
	createParameterWindow();
}

void parameterclass::createParameterWindow()
{
	if(!(mParamWnd.hwnd))
	{
		mParamWnd.hwnd = CreateWindow("parameterclass", "parameter",
			WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
			mPos.left, mPos.top, mPos.right, mPos.left,
			mParentWnd, NULL, mHinst, NULL);
		
		if(!(mParamWnd.hwnd))
		{
			MessageBox(NULL, "parameterclass createParameterWindow() CreateWindow() failed", "", MB_OK);
		}
	}
	MessageBox(NULL, "‚·‚Å‚Éì‚ç‚ê‚Ä‚¢‚Ü‚·", "", MB_OK);
	return;
}



/*
void parameterclass::cleanHwnd(MYHWND *myhwnd);
{
	myhwnd->hwnd = NULL;
	
	for(int i=0;i < myhwnd->childWnd.size(); ++i)
	{
		cleanHwnd(myhwnd->childWnd(i));
	}

	
	return;
}
*/
	