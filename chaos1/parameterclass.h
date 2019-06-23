#ifndef PARAMETERCLASS_H
#define PARAMETERCLASS_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

struct MYHWND
{
	HWND hwnd;
	std::vector<MYHWND*> childWnd;
	void (*func)();
};

struct MYD3DPARAMDATA
{
	D3DXVECTOR3 camPos;
	bool bCamFixed;

};

class parameterclass
{
private:
	HWND *mParentWnd;
	HINSTANCE mHinst;
	MYHWND mParamWnd;
	RECT mPos;
	MYD3DPARAMDATA mParamData;
	
public:
	parameterclass(HINSTANCE hInst,HWND *parentWnd,RECT& pos);
	~parameterclass();
	void initParameterWindow();
	void createParameterWindow();
	void destroyHwnd(MYHWND *myhwnd);
	//void cleanHwnd(MYHWND *myhwnd);
	
};

#endif