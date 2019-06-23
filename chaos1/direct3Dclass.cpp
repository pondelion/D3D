
#include "direct3Dclass.h"
#include <stdio.h>
#include <tchar.h>
#include "parameterclass.h"


IDirect3DVertexDeclaration9* VPos::Decl = 0;
IDirect3DVertexDeclaration9* VPosCol::Decl = 0;
IDirect3DVertexDeclaration9* VPosNormal::Decl = 0;
IDirect3DVertexDeclaration9* VPosNormalTex::Decl = 0;

float randomFloat(float a,float b)
{
	if(a<b)
	{
	return ((rand()%10001)*0.0001f)*(b-a)+a;
	}
	if(a>b)
	{
	return ((rand()%10001)*0.0001f)*(a-b)+b;
	}
	return a;
}

void randomVec(D3DXVECTOR3& v)
{
	v.x = randomFloat(-1.0f, 1.0f);
	v.y = randomFloat(-1.0f, 1.0f);
	v.z = randomFloat(-1.0f, 1.0f);
	
	D3DXVec3Normalize(&v, &v);
}

LRESULT CALLBACK MyD3DWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static D3DPARAM* d3dParam = NULL; 
	static d3dclass* myd3d = NULL;
	static int n = 0;
	static float prevTK=0.0f, curTK=0.0f;
	static BOOL keyflag = false,bRClicked = false;
	static POINT prevP, curP;
	switch(msg)
	{
	
	case WM_MYPARAM:
		d3dParam = (D3DPARAM *)lp;
		break;
	case WM_D3DCLASSOBJ:
		myd3d = (d3dclass *)lp;
		break;
	case WM_CLOSE:
		if(MessageBox(hwnd, "ウィンドウを閉じますか?", "", MB_YESNO) == IDYES)
		{
			//SendMessage(*(d3dParam->hParent), WM_D3DRELEASE, 0, 0);
			/*
			if(*(d3dParam->ppd3d9))
			{
			(*(d3dParam->ppd3d9))->Release();
			*(d3dParam->ppd3d9) = NULL;
			}
			
			if(*(d3dParam->ppd3dDevice))
			{
			(*(d3dParam->ppd3dDevice))->Release();
			*(d3dParam->ppd3dDevice) = NULL;
			}
			*/
			myd3d->d3drelease();
			delete myd3d;
			//DestroyWindow(hwnd);
			//*(d3dParam->hD3DWnd) = NULL;
			
			
		}
		
		break;
	case WM_CHAR:
		
		if(myd3d)
		{
			if(wp == 119 || wp == 97 || wp == 115 || wp == 100)
			{
			
			if(!keyflag)
			{
				prevTK = myd3d->getTime();
				keyflag = true;
			}
			else
			{
				curTK = myd3d->getTime();
				D3DXVECTOR3 p,look,right;
				float speed;
				speed = myd3d->getCamSpeed();
				look = myd3d->getCamLook();
				right = myd3d->getCamRight();
				p = myd3d->getCamPos();
				if(wp == 119)
					p += look*(curTK-prevTK)*speed;
				else if(wp == 115)
					p -= look*(curTK-prevTK)*speed;
				else if(wp == 100)
					p += right*(curTK-prevTK)*speed;
				else if(wp == 97)
					p -= right*(curTK-prevTK)*speed;
				
				myd3d->setCamPos(p);
				prevTK = curTK;
			}
			}
		}
		break;
	case WM_KEYUP:
		
		keyflag = false;
		break;
	case WM_RBUTTONDOWN:
		
		SetFocus(hwnd);
		SetForegroundWindow(hwnd);
		
		GetCursorPos(&prevP);
			
		break;
	case WM_MOUSEMOVE:
		if(myd3d)
		{
			if(wp & MK_RBUTTON)
			{
			GetCursorPos(&curP);
			D3DXMATRIX rot,roty;
			D3DXVECTOR3 right,up,look;
			right = myd3d->getCamRight();
			look = myd3d->getCamLook();
			
			D3DXMatrixRotationAxis(&rot, &right, (curP.y-prevP.y)*0.003f);
			D3DXMatrixRotationY(&roty, (curP.x-prevP.x)*0.003f);
			D3DXVec3TransformCoord(&right, &right, &(roty*rot));
			D3DXVec3TransformCoord(&look, &look, &(roty*rot));
			right.y = 0.0f;
			D3DXVec3Cross(&up, &look, &right);
			D3DXVec3Cross(&look, &right, &up);
			D3DXVec3Normalize(&up, &up);
			D3DXVec3Normalize(&look, &look);
			D3DXVec3Normalize(&right, &right);
			myd3d->setCamUp(up);
			myd3d->setCamLook(look);
			myd3d->setCamRight(right);
			prevP = curP;
			}
		}
		break;
	
	case WM_ACTIVATE:
		if(LOWORD(wp) == WA_INACTIVE)
			*(d3dParam->pbPaused) = true;
		else
			*(d3dParam->pbPaused) = false;
		break;
		
	case WM_SIZE:
		if(d3dParam)
		{
			
			GetClientRect(*(d3dParam->hD3DWnd), d3dParam->pclientrc);
			if(*(d3dParam->ppd3dDevice))
			{
				d3dParam->pd3dpp->BackBufferWidth = LOWORD(lp);
				d3dParam->pd3dpp->BackBufferHeight = HIWORD(lp);
			
				if(wp == SIZE_MINIMIZED)
				{
					*(d3dParam->pbPaused) = true;
					*(d3dParam->pbMinMax) = true;
				}
				else if( wp == SIZE_MAXIMIZED)
				{
					*(d3dParam->pbPaused) = false;
					SendMessage(*(d3dParam->hParent), WM_D3DRESET, 0, 0);
				}
				else if(wp == SIZE_RESTORED)
				{
				*(d3dParam->pbPaused) = false;
				
					if(*(d3dParam->pbMinMax) && d3dParam->pd3dpp->Windowed)
					{
					SendMessage(*(d3dParam->hParent), WM_D3DRESET, 0, 0);
					}
				
					*(d3dParam->pbMinMax) = false;
				}
			}
		}
		break;
		
	case WM_EXITSIZEMOVE:
		GetClientRect(*(d3dParam->hD3DWnd), d3dParam->pclientrc);
		d3dParam->pd3dpp->BackBufferWidth = d3dParam->pclientrc->right;
		d3dParam->pd3dpp->BackBufferHeight = d3dParam->pclientrc->bottom;
		SendMessage(*(d3dParam->hParent), WM_D3DRESET, 0, 0);

		break;
		
	case WM_MOVE:
		
		SetFocus(hwnd);
		SetForegroundWindow(hwnd);
		//InvalidateRect(hwnd, NULL, TRUE);
		
		break;
		
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


d3dclass::d3dclass(HINSTANCE hInst, HWND hwndParent, LPSTR lpstrCaption, D3DDEVTYPE devtype, DWORD VP,RECT *rc)
{
	
	this->hInst = hInst;
	hParent = hwndParent;
	hD3DWnd = NULL;
	wndPos.left = rc->left;
	wndPos.right = rc->right;
	wndPos.top = rc->top;
	wndPos.bottom = rc->bottom;
	clientrc.left = 0;
	clientrc.right = 0;
	clientrc.top = 0;
	clientrc.bottom = 0;
	ZeroMemory(&md3dpp, sizeof(D3DPRESENT_PARAMETERS));
	lpCaption = lpCaption;
	d3dParam.hD3DWnd = &hD3DWnd;
	mDevType = devtype;
	mVP = VP;
	md3dDevice = NULL;
	md3d9 = NULL;
	mbPaused = false;
	mbMinMax = false;
	mfps = 0.0;
	mmpf = 0.0;
	mFont = NULL;
	
	mVBGrid = NULL;
	mIBGrid = NULL;
	mVBPLGrid = NULL;
	mIBPLGrid = NULL;
	mVBLTGrid = NULL;
	mIBLTGrid = NULL;
	mVBLTBox = NULL;
	mIBLTBox = NULL;
	mVBLTSpring = NULL;
	mIBLTSpring = NULL;
	mtime = 0.0;
	mPointLightFX = NULL;
	mGridFX = NULL;
	mCurFX = NULL;
	mLightTexFX = NULL;
	mSphere = NULL;
	mSphereTex = NULL;
	mLTGridTex = NULL;
	mLTBoxTex = NULL;
	mLTSpringTex = NULL;
	D3DXMatrixIdentity(&mGridWorld);
	mAmbLight = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mDifLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpecLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSphereAmbLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSphereDifLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSphereSpecLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLightPos = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	mLightVec = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	mAttenuation	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mGridMtrl	= MTRL(D3DXCOLOR(0.0f, 1.0f,0.0f, 1.0f),D3DXCOLOR(0.0f,1.0f, 0.0f, 1.0f), D3DXCOLOR(1.0f,1.0f, 1.0f, 1.0f), 16.0f);
	mCamPos = D3DXVECTOR3(5.0,8.0,5.0);
	mhLT.mtrl	= MTRL(D3DXCOLOR(0.6f, 0.6f,0.6f, 0.6f), D3DXCOLOR(1.0f, 1.0f,1.0f, 0.5f), D3DXCOLOR(0.8f, 0.8f,0.8f, 0.8f), 8.0f);
	mSpherePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mTarget = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	mLTGridPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mLTGridMtrl = MTRL(D3DXCOLOR(0.6f, 0.6f,0.6f, 0.6f), D3DXCOLOR(1.0f, 1.0f,1.0f, 1.0f), D3DXCOLOR(0.8f, 0.8f,0.8f, 0.8f), 8.0f);
	mLTSphereMtrl = MTRL(D3DXCOLOR(0.6f, 0.6f,0.6f, 0.6f), D3DXCOLOR(1.0f, 1.0f,1.0f, 0.5f), D3DXCOLOR(0.8f, 0.8f,0.8f, 0.8f), 8.0f);
	mLTBoxPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mLTBoxMtrl = MTRL(D3DXCOLOR(0.6f, 0.6f,0.6f, 0.6f), D3DXCOLOR(1.0f, 1.0f,1.0f, 0.4f), D3DXCOLOR(0.8f, 0.8f,0.8f, 0.8f), 8.0f);
	mLTSpringPos = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	mSpringScale = D3DXVECTOR3(4.0f, 4.0f, 4.0f);
	D3DXVec3Normalize(&mCamLook, &(mTarget-mCamPos)); 
	D3DXVec3Cross(&mCamRight, &(D3DXVECTOR3(0.0f, 1.0f, 0.0f)), &mCamLook);
	D3DXVec3Normalize(&mCamRight , &mCamRight);
	D3DXVec3Cross(&mCamUp, &mCamLook, &mCamRight);
	D3DXVec3Normalize(&mCamUp, &mCamUp);
	mCamSpeed = 4.0f;
	
	d3dParam.pbPaused = &mbPaused;
	d3dParam.pbMinMax = &mbMinMax;
	d3dParam.pclientrc = &clientrc;
	d3dParam.ppd3dDevice = &md3dDevice;
	d3dParam.pd3dpp = &md3dpp;
	d3dParam.hParent = &hParent;
	d3dParam.ppd3d9 = &md3d9;
	
	
}

d3dclass::~d3dclass()
{
	if(mParamClass)
	{
		delete mParamClass;
		mParamClass = NULL;
	}
	d3drelease();
	
}

void d3dclass::initWindow()
{
	WNDCLASSEX wc;
	
	if(!hD3DWnd)
	{
		wc.cbSize 		= sizeof(WNDCLASSEX);
		wc.style 		= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc 	= MyD3DWndProc;
		wc.cbClsExtra 	= 0;
		wc.cbWndExtra 	= 0;
		wc.hInstance 	= hInst;
		wc.hIcon 		= LoadIcon(hInst, "MYICON");
		wc.hCursor 		= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "myd3dclass";
		wc.hIconSm = LoadIcon(hInst, "MYICON");
	
		if(!RegisterClassEx(&wc))
		{
		
			MessageBox(NULL, "RegisterClassEx Failed", "", MB_OK);
			return;
		}
	
	
		
		hD3DWnd = CreateWindow("myd3dclass", "Direct3D",//lpCaption,
			WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_CAPTION |WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_OVERLAPPED,
			wndPos.left, wndPos.top, wndPos.right, wndPos.bottom,
			hParent,
			NULL,
			hInst,
			NULL);
		
		
	
		if(!hD3DWnd)
		{
			MessageBox(hParent, "D3Dウィンドウ作成失敗", "", MB_OK);
			return;
		}
		
		ShowWindow(hD3DWnd, SW_SHOWNORMAL);
		UpdateWindow(hD3DWnd);
		
		GetClientRect(hD3DWnd, &clientrc);
		
		SendMessage(hD3DWnd, WM_MYPARAM, 0, (LPARAM)&d3dParam);
		
	}
}

void d3dclass::initDirect3D()
{
	D3DDISPLAYMODE d3dmode;
	D3DCAPS9 d3dcaps;
	DWORD flag;
	flag = 0;
	
	if(!md3d9)
	{
	
	md3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!md3d9)
	{
	MessageBox(hParent, "Direct3DCreate9 failed", "", MB_OK);
	return;
	}
	}
	
	md3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dmode);
	if(FAILED(md3d9->CheckDeviceType(D3DADAPTER_DEFAULT, mDevType, d3dmode.Format, d3dmode.Format, true)))
		MessageBox(hParent, "CheckDeviceType Failed", "", MB_OK);
	if(FAILED(md3d9->CheckDeviceType(D3DADAPTER_DEFAULT, mDevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false)))
		MessageBox(hParent, "CheckDeviceType Failed", "", MB_OK);
		
		
	if(FAILED(md3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, mDevType, &d3dcaps)))
		MessageBox(hParent, "GetDeviceCaps failed", "", MB_OK);
		
	if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		flag |= mVP;
	else
		flag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		
	if(d3dcaps.DevCaps & D3DDEVCAPS_PUREDEVICE && flag & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		flag |= D3DCREATE_PUREDEVICE;
		

	GetClientRect(hD3DWnd, &clientrc);
	
	md3dpp.BackBufferWidth		= clientrc.right;
	md3dpp.BackBufferHeight		= clientrc.bottom;
	md3dpp.BackBufferFormat		= D3DFMT_UNKNOWN;
	md3dpp.BackBufferCount		= 1;
	md3dpp.MultiSampleType		= D3DMULTISAMPLE_NONE;
	md3dpp.MultiSampleQuality	= 0;
	md3dpp.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	md3dpp.hDeviceWindow		= hD3DWnd;
	md3dpp.Windowed				= true;
	md3dpp.EnableAutoDepthStencil	= true;
	md3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;
	md3dpp.Flags				= 0;
	md3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	md3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	
	if(!md3dDevice)
	{
	if(FAILED(md3d9->CreateDevice(D3DADAPTER_DEFAULT,
		mDevType,
		hD3DWnd,
		flag,
		&md3dpp,
		&md3dDevice)))
		MessageBox(hParent, "CreateDevice failed", "", MB_OK);
	}
	
	if(!deviceCap())
		MessageBox(NULL, "Shader Version Not Supported", NULL, MB_OK);
	
	initFont();
		
	
}

bool d3dclass::isDeviceLost()
{
	
	HRESULT h = md3dDevice->TestCooperativeLevel();
	
	if( h == D3DERR_DEVICELOST)
	{
	Sleep(50);
	return true;
	}
	
	else if(h == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(hParent, "DRIVERINTERNALERROR", "", MB_OK);
		PostQuitMessage(0);
		return true;
	}
	
	else if(h == D3DERR_DEVICENOTRESET)
	{
	lostDevice();
	if(FAILED(md3dDevice->Reset(&md3dpp)))
		MessageBox(hParent, "Reset Error", "", MB_OK);
	resetDevice();
	return false;
	}
	else
		return false;
}

HWND d3dclass::getD3DHwnd()
{
	return hD3DWnd;
}

IDirect3DDevice9* d3dclass::getD3DDevice()
{
	return md3dDevice;
}

void d3dclass::reset()
{
	if(FAILED(md3dDevice->Reset(&md3dpp)))
		MessageBox(NULL, "ResetFailed", "", MB_OK);
}

BOOL d3dclass::isPaused()
{
	return mbPaused;
}
	
void d3dclass::d3drelease()
{
	if(mFont)
	{
	mFont->Release();
	mFont = NULL;
	}
	if(mText.size() >0)
	{
		for(int i=0; i < mText.size(); i++)
		{
		if(mText[i].Font)
		{
		mText[i].Font->Release();
		mText[i].Font = NULL;
		}
		}
	}
	if(md3d9)
	{
	md3d9->Release();
	md3d9 = NULL;
	}
	
	if(mVBGrid)
	{
	mVBGrid->Release();
	mVBGrid = NULL;
	}
	if(mIBGrid)
	{
	mIBGrid->Release();
	mIBGrid = NULL;
	}
	if(mVBPLGrid)
	{
	mVBPLGrid->Release();
	mVBPLGrid = NULL;
	}
	if(mIBPLGrid)
	{
	mIBPLGrid->Release();
	mIBPLGrid = NULL;
	}
	if(mVBLTGrid)
	{
	mVBLTGrid->Release();
	mVBLTGrid = NULL;
	}
	if(mIBLTGrid)
	{
	mIBLTGrid->Release();
	mIBLTGrid = NULL;
	}
	if(mVBLTBox)
	{
	mVBLTBox->Release();
	mVBLTBox = NULL;
	}
	if(mIBLTBox)
	{
	mIBLTBox->Release();
	mIBLTBox = NULL;
	}
	if(mVBLTSpring)
	{
	mVBLTSpring->Release();
	mVBLTSpring = NULL;
	}
	if(mIBLTSpring)
	{
	mIBLTSpring->Release();
	mIBLTSpring = NULL;
	}
	if(mGridFX)
	{
	mGridFX->Release();
	mGridFX = NULL;
	}
	if(mPointLightFX)
	{
	mPointLightFX->Release();
	mPointLightFX = NULL;
	}
	if(mLightTexFX)
	{
	mLightTexFX->Release();
	mLightTexFX = NULL;
	}
	if(mSphereTex)
	{
	mSphereTex->Release();
	mSphereTex = NULL;
	}
	if(mLTGridTex)
	{
	mLTGridTex->Release();
	mLTGridTex = NULL;
	}
	if(mLTSpringTex)
	{
	mLTSpringTex->Release();
	mLTSpringTex = NULL;
	}
	if(md3d9)
	{
	md3d9->Release();
	md3d9 = NULL;
	}
	if(md3dDevice)
	{
	md3dDevice->Release();
	md3dDevice = NULL;
	}
	
	destroyVertexDeclarations();
}

void d3dclass::setPaused(BOOL paused)
{
	mbPaused = paused;
}

void d3dclass::setMinMax(BOOL minmax)
{
	mbMinMax = minmax;
}

void d3dclass::drawFPS()
{
	static char buf[256];
	RECT rc = {5,5,0,0};
	
	sprintf(buf, "Frame Per Secont = %.3f\nMilliSecont Per Frame = %.3f\nTime = %.3f\n",
		mfps, mmpf, mtime);
		
	if(mFont)
		mFont->DrawText(0, buf, -1, &rc, DT_NOCLIP, D3DCOLOR_XRGB(255, 0,0));
}

void d3dclass::initFont()
{
	D3DXFONT_DESC fontDesc;
	if(md3dDevice)
	{
	
	fontDesc.Height          = 18;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    _tcscpy(fontDesc.FaceName, _T("Times New Roman"));

	
	if(FAILED(D3DXCreateFontIndirect(md3dDevice, &fontDesc, &mFont)))
	{
	MessageBox(NULL, "CreateFont failed", "", MB_OK);
	}
	}
	return;
}

bool d3dclass::deviceCap()
{
	D3DCAPS9 cap;
	if(FAILED(md3dDevice->GetDeviceCaps(&cap)))
	{
	MessageBox(NULL, "GetDeviceCapsFailed", "", MB_OK);
	}
	
	if( cap.VertexShaderVersion < D3DVS_VERSION(2, 0) || cap.PixelShaderVersion < D3DPS_VERSION(2, 0) )
	{
		return false;
	}
	
	return true;
}

void d3dclass::createMeshGrid(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center, std::vector<D3DXVECTOR3>& vec, std::vector<DWORD>& index)
{
	int nV = nVR*nVC;
	int nTri = ((nVR-1)*(nVC-1))*2;
	float w = (float)(nVC-1)*dx;
	float h = (float)(nVR-1)*dz;
	int k = 0;
	float x = -w*0.5;
	float z = h*0.5;
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, center.x, center.y, center.z);
	
	vec.resize(nV);
	index.resize(nTri*3);
	
	for(float i=0; i < nVR; ++i)
	{
		for(float j=0; j<nVC; ++j)
		{
		
		vec[k].x = j*dx + x;
		vec[k].z = -i*dz + z;
		vec[k].y = 0.0;
		
		D3DXVec3TransformCoord(&vec[k], &vec[k], &T);
		
		++k;
		}
	}
	
	k=0;
	for(DWORD i=0; i<(DWORD)(nVR-1); ++i)
	{
		for(DWORD j=0; j < (DWORD)(nVC-1); ++j)
		{
		index[k]	= i*nVC+j;
		index[k+1]	= i*nVC+j+1;
		index[k+2]	= (i+1)*nVC+j;
		index[k+3]	= (i+1)*nVC+j;
		index[k+4]	= i*nVC+j+1;
		index[k+5]	= (i+1)*nVC+j+1;
		
		k += 6;
		}
	}
}

void d3dclass::createMeshGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center)
{
	std::vector<D3DXVECTOR3> vec;
	std::vector<DWORD> index;
	
	mNVR = nVR;
	mNVC = nVC;
	if(md3dDevice)
	{
	
	createMeshGrid(nVR, nVC, dx, dz, center, vec, index);

	if(FAILED(md3dDevice->CreateVertexBuffer(nVR*nVC*sizeof(VPos), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVBGrid, 0)))
		MessageBox(NULL, "CreateVertexBuffer() failed", "", MB_OK);
		
	VPos* vp = 0;
	if(FAILED(mVBGrid->Lock(0, 0, (void**)&vp, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
		
	for(DWORD i=0; i<nVR*nVC; ++i)
	{
	vp[i] = vec[i];
	}
	
	if(FAILED(mVBGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
		
	if(FAILED(md3dDevice->CreateIndexBuffer((nVR-1)*(nVC-1)*2*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIBGrid, 0)))
		MessageBox(NULL, "CreateIndexBuffer() failed", "", MB_OK);
	
	WORD* ind = 0;
	if(FAILED(mIBGrid->Lock(0 ,0 ,(void**)&ind, 0)))
		MessageBox(NULL, "IBGrid->Lock() failed", "", MB_OK);
	
	for(DWORD i=0; i<(nVR-1)*(nVC-1)*2*3; ++i)
	{
	ind[i] = (WORD)index[i];
	}
	
	if(FAILED(mIBGrid->Unlock()))
		MessageBox(NULL, "IndexBuffer Unlock() failed", "", MB_OK);
	}
}
	
void d3dclass::initVertexDeclarations()
{
	D3DVERTEXELEMENT9 VPosElement[] =
	{
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	D3DDECL_END()
	};
	if(FAILED(md3dDevice->CreateVertexDeclaration(VPosElement, &VPos::Decl)))
		MessageBox(NULL, "CreateVertexDeclaration() failed", "", MB_OK);
		
	D3DVERTEXELEMENT9 VPosColElement[] =
	{
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
	};
	if(FAILED(md3dDevice->CreateVertexDeclaration(VPosColElement, &VPosCol::Decl)))
		MessageBox(NULL, "CreateVertexDeclaration() failed", "", MB_OK);
		
	D3DVERTEXELEMENT9 VPosNormalElement[] =
	{
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	D3DDECL_END()
	};
	if(FAILED(md3dDevice->CreateVertexDeclaration(VPosNormalElement, &VPosNormal::Decl)))
		MessageBox(NULL, "CreateVertexDeclaration() failed", "", MB_OK);
	
	D3DVERTEXELEMENT9 VPosNormalTexElement[] =
	{
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
	};
	if(FAILED(md3dDevice->CreateVertexDeclaration(VPosNormalTexElement, &VPosNormalTex::Decl)))
		MessageBox(NULL, "CreateVertexDeclaration() failed", "", MB_OK);
		
}

void d3dclass::destroyVertexDeclarations()
{
	if(VPos::Decl)
	{
		(VPos::Decl)->Release();
		VPos::Decl = NULL;
	}
	if(VPosCol::Decl)
	{
		(VPosCol::Decl)->Release();
		VPosCol::Decl = NULL;
	}
	if(VPosNormal::Decl)
	{
		(VPosNormal::Decl)->Release();
		VPosNormal::Decl = NULL;
	}
	if(VPosNormalTex::Decl)
	{
		(VPosNormalTex::Decl)->Release();
		VPosNormalTex::Decl = NULL;
	}
}

void d3dclass::createMeshGridFX()
{
	if(md3dDevice)
	{
	ID3DXBuffer* error = 0;
	if(FAILED(D3DXCreateEffectFromFile(md3dDevice, "..\\resource\\fx\\gridfx.fx", 0, 0,
		D3DXSHADER_DEBUG, 0, &mGridFX, &error)))
		{
			MessageBox(NULL, "createMeshGridFX() CreateEffectFromFile() failed", "", MB_OK);
			if( error )
		MessageBox(0, (char*)error->GetBufferPointer(), 0, 0);
		}
			
	mhGrid.mhTech = mGridFX->GetTechniqueByName("gridTech");
	mhGrid.mhWVP  = mGridFX->GetParameterByName(0, "gWVP");
	mhGrid.mhTime = mGridFX->GetParameterByName(0, "time");
	}
}

void d3dclass::drawMeshGrid()
{
	if(FAILED(md3dDevice->SetStreamSource(0, mVBGrid, 0 , sizeof(VPos))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetIndices(mIBGrid)))
		MessageBox(NULL, "SetIndices() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetVertexDeclaration(VPos::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
		
	if(FAILED(mGridFX->SetTechnique(mhGrid.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
		
	UINT n = 0;
	if(FAILED(mGridFX->Begin(&n, 0)))
		MessageBox(NULL, "FX Begin() failed", "", MB_OK);
	for(UINT i=0; i<n; ++i)
	{
		if(FAILED(mGridFX->BeginPass(i)))
			MessageBox(NULL, "BeginPass() failed", "", MB_OK);
			
		if(FAILED(mGridFX->SetMatrix(mhGrid.mhWVP, &(mView*mProj))))
			MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
			
		if(FAILED(mGridFX->SetFloat(mhGrid.mhTime, mtime)))
			MessageBox(NULL, "SetFloat() failed", "", MB_OK);
			
		if(FAILED(md3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNVR*mNVC, 0, (mNVR-1)*(mNVC-1)*2 )))
			MessageBox(NULL, "BeginPass() failed", "", MB_OK);
			
		if(FAILED(mGridFX->EndPass()))
			MessageBox(NULL, "EndPass() failed", "", MB_OK);
			
	}
	if(FAILED(mGridFX->End()))
			MessageBox(NULL, "End() failed", "", MB_OK);
}
			
void d3dclass::drawPLGrid()
{

	if(!mVBPLGrid || !mIBPLGrid || !mPointLightFX)
		testes;
	
	if(FAILED(md3dDevice->SetStreamSource(0, mVBPLGrid, 0 , sizeof(VPosNormal))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetIndices(mIBPLGrid)))
		MessageBox(NULL, "SetIndices() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetVertexDeclaration(VPosNormal::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
		
	if(FAILED(mPointLightFX->SetTechnique(mhPL.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
		
	D3DXMATRIX WIT;
	D3DXMatrixInverse(&WIT, 0, &mGridWorld);
	D3DXMatrixTranspose(&WIT, &WIT);
	
	if(FAILED(mPointLightFX->SetMatrix(mhPL.mhW, &mGridWorld)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetMatrix(mhPL.mhWVP, &(mGridWorld*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetMatrix(mhPL.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
		
	
		
	if(FAILED(mPointLightFX->SetValue(mhPL.mhCamPos, &mCamPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhLightPos, &mLightPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
		
	//if(FAILED(mPointLightFX->CommitChanges()))
	//	MessageBox(NULL, "CommitChanges() failed", "", MB_OK);
	
	UINT n = 0;
	if(FAILED(mPointLightFX->Begin(&n, 0)))
		MessageBox(NULL, "FX Begin() failed", "", MB_OK);
	for(UINT i=0; i<n; ++i)
	{
	
	if(FAILED(mPointLightFX->BeginPass(i)))
			MessageBox(NULL, "BeginPass() failed", "", MB_OK);
	
	
	if(FAILED(md3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNVR*mNVC, 0, (mNVR-1)*(mNVC-1)*2)))
		MessageBox(NULL, "DrawIndexedPromitive() failed", "", MB_OK);
		
		if(FAILED(mPointLightFX->EndPass()))
			MessageBox(NULL, "EndPass() failed", "", MB_OK);
			
	}
	if(FAILED(mPointLightFX->End()))
			MessageBox(NULL, "End() failed", "", MB_OK);
		
	

}
		


void d3dclass::createPointLightFX()
{
	if(md3dDevice)
	{
	ID3DXBuffer* error = 0;
	if(FAILED(D3DXCreateEffectFromFile(md3dDevice, "..\\resource\\fx\\pointlightfx.fx", 0, 0,
		D3DXSHADER_DEBUG, 0, &mPointLightFX, &error)))
		{
			MessageBox(NULL, "PointLightFX CreateEffectFromFile() failed", "", MB_OK);
			if( error )
		MessageBox(0, (char*)error->GetBufferPointer(), 0, 0);
		}
			
	mhPL.mhTech	= mPointLightFX->GetTechniqueByName("PLTech");
	mhPL.mhW	= mPointLightFX->GetParameterByName(0, "gW");
	mhPL.mhWIT	= mPointLightFX->GetParameterByName(0, "gWIT");
	mhPL.mhWVP	= mPointLightFX->GetParameterByName(0, "gWVP");
	mhPL.mhCamPos	= mPointLightFX->GetParameterByName(0, "gCamPos");
	mhPL.mhAmbMtrl	= mPointLightFX->GetParameterByName(0, "gAmbMtrl");
	mhPL.mhDifMtrl	= mPointLightFX->GetParameterByName(0, "gDifMtrl");
	mhPL.mhSpecMtrl	= mPointLightFX->GetParameterByName(0, "gSpecMtrl");
	mhPL.mhPower	= mPointLightFX->GetParameterByName(0, "gPower");
	mhPL.mhLightPos	= mPointLightFX->GetParameterByName(0, "gLightPos");
	mhPL.mhAmbLight	= mPointLightFX->GetParameterByName(0, "gAmbLight");
	mhPL.mhDifLight	= mPointLightFX->GetParameterByName(0, "gDifLight");
	mhPL.mhSpecLight	= mPointLightFX->GetParameterByName(0, "gSpecLight");
	mhPL.mhAttenuation	= mPointLightFX->GetParameterByName(0, "gAttenuation");
	mhPL.mhTime	= mPointLightFX->GetParameterByName(0, "gTime");
	
	
	if(FAILED(mPointLightFX->SetValue(mhPL.mhAmbLight, &mAmbLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhDifLight, &mDifLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhSpecLight, &mSpecLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhAttenuation, &mAttenuation, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	
	if(FAILED(mPointLightFX->SetValue(mhPL.mhAmbMtrl, &mGridMtrl.ambient, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhDifMtrl, &mGridMtrl.diffuse, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetValue(mhPL.mhSpecMtrl, &mGridMtrl.spec, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetFloat(mhPL.mhPower, mGridMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mPointLightFX->SetFloat(mhPL.mhTime, mtime)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	}

}

void d3dclass::setFX(ID3DXEffect* fx)
{
	mCurFX = fx;
}

void d3dclass::lostDevice()
{
	if(mFont)
	{
	if(FAILED(mFont->OnLostDevice()))
		MessageBox(NULL, "Font OnLostDevice() failed", "", MB_OK);
	}
	
	if(mText.size() >0)
	{
		
		for(int i=0; i < mText.size(); i++)
		{
		
		if(mText[i].Font)
		{
		mText[i].Font->OnLostDevice();
		
		}
		}
	}
	if(mGridFX)
	{
	if(FAILED(mGridFX->OnLostDevice()))
		MessageBox(NULL, "FX OnLostDevice() failed", "", MB_OK);
	}
	
	if(mPointLightFX)
	{
	if(FAILED(mPointLightFX->OnLostDevice()))
		MessageBox(NULL, "FX OnLostDevice() failed", "", MB_OK);
	}
	
	if(mLightTexFX)
	{
		if(FAILED(mLightTexFX->OnLostDevice()))
		MessageBox(NULL, "FX OnLostDevice() failed", "", MB_OK);
	}
}

void d3dclass::resetDevice()
{
	if(mFont)
	{
	if(FAILED(mFont->OnResetDevice()))
		MessageBox(NULL, "Font OnResetDevice() failed", "", MB_OK);
	}
	if(mText.size() >0)
	{
		for(int i=0; i < mText.size(); i++)
		{
		if(mText[i].Font)
		{
		mText[i].Font->OnResetDevice();
		
		}
		}
	}
	if(mGridFX)
	{
	if(FAILED(mGridFX->OnResetDevice()))
		MessageBox(NULL, "FX OnResetDevice() failed", "", MB_OK);
	}
	
	if(mPointLightFX)
	{
	
	if(FAILED(mPointLightFX->OnResetDevice()))
		MessageBox(NULL, "FX OnResetDevice() failed", "", MB_OK);
	}
	
	if(mLightTexFX)
	{
		
		if(FAILED(mLightTexFX->OnResetDevice()))
		MessageBox(NULL, "FX OnLostDevice() failed", "", MB_OK);
	}
	
	setProjMtx();
}

void d3dclass::createPointLightGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center)
{
	std::vector<D3DXVECTOR3> vec;
	std::vector<DWORD> index;
	
	mNVR = nVR;
	mNVC = nVC;
	if(md3dDevice)
	{
	
	createMeshGrid(nVR, nVC, dx, dz, center, vec, index);

	if(FAILED(md3dDevice->CreateVertexBuffer(nVR*nVC*sizeof(VPosNormal), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVBPLGrid, 0)))
		MessageBox(NULL, "CreateVertexBuffer() failed", "", MB_OK);
		
	VPosNormal* vpn = 0;
	if(FAILED(mVBPLGrid->Lock(0, 0, (void**)&vpn, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
		
	for(DWORD i=0; i<nVR*nVC; ++i)
	{
	vpn[i].pos = vec[i];
	vpn[i].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	
	}
	
	if(FAILED(mVBPLGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
		
	if(FAILED(md3dDevice->CreateIndexBuffer((nVR-1)*(nVC-1)*2*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIBPLGrid, 0)))
		MessageBox(NULL, "CreateIndexBuffer() failed", "", MB_OK);
	
	WORD* ind = 0;
	if(FAILED(mIBPLGrid->Lock(0 ,0 ,(void**)&ind, 0)))
		MessageBox(NULL, "IBGrid->Lock() failed", "", MB_OK);
	
	for(DWORD i=0; i<(nVR-1)*(nVC-1)*2*3; ++i)
	{
	ind[i] = (WORD)index[i];
	}
	
	if(FAILED(mIBPLGrid->Unlock()))
		MessageBox(NULL, "IndexBuffer Unlock() failed", "", MB_OK);
	}
}

void d3dclass::createLightTexFX()
{
	if(!md3dDevice)
		return;
		
	ID3DXBuffer* error = 0;
	if(FAILED(D3DXCreateEffectFromFile(md3dDevice, "..\\resource\\fx\\lighttexfx.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &mLightTexFX, &error)))
	{
		MessageBox(NULL, "LightTexFX CreateEffectFromFile() failed", "", MB_OK);
			if( error )
			{
			testes;
		MessageBox(0, (char*)error->GetBufferPointer(), 0, 0);
		}
	}
	
	mhLT.mhTech		= mLightTexFX->GetTechniqueByName("LightTexTech");
	mhLT.mhW		= mLightTexFX->GetParameterByName(0, "gW");
	mhLT.mhWIT		= mLightTexFX->GetParameterByName(0, "gWIT");
	mhLT.mhAmbMtrl		= mLightTexFX->GetParameterByName(0, "gAmbMtrl");
	mhLT.mhAmbLight	= mLightTexFX->GetParameterByName(0, "gAmbLight");
	mhLT.mhWVP		= mLightTexFX->GetParameterByName(0, "gWVP");
	mhLT.mhDifMtrl	= mLightTexFX->GetParameterByName(0, "gDifMtrl");
	mhLT.mhDifLight	= mLightTexFX->GetParameterByName(0, "gDifLight");
	mhLT.mhSpecMtrl	= mLightTexFX->GetParameterByName(0, "gSpecMtrl");
	mhLT.mhSpecLight	= mLightTexFX->GetParameterByName(0, "gSpecLight");
	mhLT.mhVLight	= mLightTexFX->GetParameterByName(0, "gVLight");
	mhLT.mhCamPos	= mLightTexFX->GetParameterByName(0, "gCamPos");
	mhLT.mhPower	= mLightTexFX->GetParameterByName(0, "gPower");
	mhLT.mhTex		= mLightTexFX->GetParameterByName(0, "gTex");
	mhLT.mhTime		= mLightTexFX->GetParameterByName(0, "gTime");
	
	
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbLight, &mSphereAmbLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifLight, &mSphereDifLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecLight, &mSphereSpecLight, sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhVLight, &mLightVec, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	
}

void d3dclass::createSphere(LPCSTR texname, float r, D3DXVECTOR3& pos)
{

	if(pos)
	{
		mSpherePos.x = pos.x;
		mSpherePos.y = pos.y;
		mSpherePos.z = pos.z;
	}
	
	if(!md3dDevice)
	{
	MessageBox(NULL, "createSphere() failed", "", MB_OK);
	return;
	}
	
	if(FAILED(D3DXCreateSphere(md3dDevice, r, 20, 20, &mSphere,0)))
	{
		MessageBox(NULL, " D3DXCreateSphere() failed", "", MB_OK);
		return;
	}
	
	if(FAILED(D3DXCreateTextureFromFile(md3dDevice, texname, &mSphereTex)))
	{
		MessageBox(NULL, "createSphere() D3DXCreateTextureFromFile() failed", "", MB_OK);
		return;
	}
	
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mhLT.mtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mhLT.mtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mhLT.mtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mhLT.mtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mSphereTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
		
	
	D3DVERTEXELEMENT9 elem[64];
	UINT nElem = 0;
	ID3DXMesh*	t = 0;
	VPosNormalTex*	v = 0;
	VPosNormalTex::Decl->GetDeclaration(elem, &nElem);

	if(FAILED(mSphere->CloneMesh(D3DXMESH_SYSTEMMEM, elem, md3dDevice, &t)))
		MessageBox(NULL, "mSphere->CloneMesh() failed", "", MB_OK);
		
	
	
	mSphere->Release();
	mSphere = NULL;
	
	if(FAILED(t->LockVertexBuffer(0, (void **)&v)))
		MessageBox(NULL, "LockVertexBuffer() failed", NULL, MB_OK);
		
	for(UINT i=0; i<t->GetNumVertices(); ++i)
	{
	
		v[i].tex.x	= atan2f(v[i].pos.z, v[i].pos.x) / (2.0f*D3DX_PI);
		v[i].tex.y	= acosf(v[i].pos.y / sqrtf(v[i].pos.x*v[i].pos.x+v[i].pos.y*v[i].pos.y+v[i].pos.z*v[i].pos.z)) / D3DX_PI;
		
	}
	if(FAILED(t->UnlockVertexBuffer()))
		MessageBox(NULL, "UnlockVertexBuffer() failed", "", MB_OK);
		
	if(FAILED(t->CloneMesh(D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, elem, md3dDevice, &mSphere)))
		MessageBox(NULL, "CloneMesh() failed", "", MB_OK);
		
	t->Release();
	t=NULL;
}
		

	
void d3dclass::setSphereTex(LPCSTR texname)
{
	if(mSphereTex)
	{
	mSphereTex->Release();
	mSphereTex = NULL;
	}
	
	if(FAILED(D3DXCreateTextureFromFile(md3dDevice, texname, &mSphereTex)))
	{
		MessageBox(NULL, "setSphereTex() D3DXCreateTextureFromFile() failed", "", MB_OK);
		return;
	}
	
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mSphereTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
}

void d3dclass::drawSphere()
{
	if(FAILED(md3dDevice->SetRenderState(D3DRS_WRAP0, D3DWRAP_U)))
	{
	MessageBox(NULL, "SetRenderState() failed", "", MB_OK);
	}
	md3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	md3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	md3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	
	if(FAILED(mLightTexFX->SetTechnique(mhLT.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
	
	D3DXMATRIX	W,WIT;
	
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhTime, mtime)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);

	if(FAILED(mLightTexFX->SetValue(mhLT.mhCamPos, &mCamPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mLTSphereMtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mLTSphereMtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mLTSphereMtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mLTSphereMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mSphereTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
	
	D3DXMatrixTranslation(&W, mSpherePos.x, mSpherePos.y, mSpherePos.z);
	D3DXMatrixInverse(&WIT, 0, &W);
	D3DXMatrixTranspose(&WIT, &WIT);
	
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhW, &W)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWVP, &(W*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	
	
	
	UINT nPass;
	if(FAILED(mLightTexFX->Begin(&nPass, 0)))
	{
		MessageBox(NULL, "mLightTexFX->begin() failed", "", MB_OK);
		return;
	}
	for(UINT i=0; i<nPass; ++i)
	{
		if(FAILED(mLightTexFX->BeginPass(i)))
		{
		MessageBox(NULL, "mLightTexFX->BeginPass() failed", "", MB_OK);
		}
		
		if(FAILED(mSphere->DrawSubset(0)))
			MessageBox(NULL, "mSphere->DrawSybset(0) failed", "", MB_OK);
			
		if(FAILED(mLightTexFX->EndPass()))
		{
		MessageBox(NULL, "mLightTexFX->EndPass() failed", "", MB_OK);
		}
	}
	
	
	
	if(FAILED(mLightTexFX->End()))
	{
		MessageBox(NULL, "mLightTexFX->End() failed", "", MB_OK);
		return;
	}
	
	if(FAILED(md3dDevice->SetRenderState(D3DRS_WRAP0, 0)))
	{
	MessageBox(NULL, "SetRenderState() failed", "", MB_OK);
	}
	
		md3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}
	
void d3dclass::setProjMtx()
{
	float w = (float)md3dpp.BackBufferWidth;
	float h = (float)md3dpp.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI*0.25, w/h, 1.0f, 5000.0f);
	
}

void d3dclass::setViewMtx()
{
	//D3DXVECTOR3 pos(5.0f, 20.0f, 5.0f);
	//D3DXVECTOR3 up(0.0, 1.0, 0.0);
	//D3DXMatrixLookAtLH(&mView, &mCamPos, &mTarget, &up);
	D3DXMatrixLookAtLH(&mView, &mCamPos, &(mCamPos+mCamLook), &mCamUp);
	calcFrustumPlane();
}

void d3dclass::setTarget(D3DXVECTOR3& t)
{
	mTarget = t;
}

void d3dclass::setCamPos(D3DXVECTOR3& camPos)
{
	mCamPos = camPos;
}

void d3dclass::createLightTexGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center, LPCSTR texname)
{
	std::vector<D3DXVECTOR3> vec;
	std::vector<DWORD> index;
	mLTGridPos = center;
	
	mNVRLT = nVR;
	mNVCLT = nVC;
	if(md3dDevice)
	{
	float s = 0.2f;
	
	createMeshGrid(nVR, nVC, dx, dz, center, vec, index);

	if(FAILED(md3dDevice->CreateVertexBuffer(nVR*nVC*sizeof(VPosNormalTex), NULL, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, &mVBLTGrid, 0)))
		MessageBox(NULL, "CreateVertexBuffer() failed", "", MB_OK);
		
	VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTGrid->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
		
	for(int i = 0; i < nVR; ++i)
	{
		for(int j = 0; j < nVC; ++j)
		{
		DWORD k = i*nVC + j;
		vpnt[k].pos = vec[k];
		vpnt[k].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vpnt[k].tex = D3DXVECTOR2((float)j, (float)i)*0.2f;
		}
	}

	if(FAILED(mVBLTGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
		
	if(FAILED(md3dDevice->CreateIndexBuffer((nVR-1)*(nVC-1)*2*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIBLTGrid, 0)))
		MessageBox(NULL, "CreateIndexBuffer() failed", "", MB_OK);
	
	WORD* ind = 0;
	if(FAILED(mIBLTGrid->Lock(0 ,0 ,(void**)&ind, 0)))
		MessageBox(NULL, "IBGrid->Lock() failed", "", MB_OK);
	
	for(DWORD i=0; i<(nVR-1)*(nVC-1)*2*3; ++i)
	{
	ind[i] = (WORD)index[i];
	}
	
	if(FAILED(mIBLTGrid->Unlock()))
		MessageBox(NULL, "IndexBuffer Unlock() failed", "", MB_OK);
	}
	
	if(FAILED(D3DXCreateTextureFromFile(md3dDevice, texname, &mLTGridTex)))
	{
		MessageBox(NULL, "createLightTexGridBuffer() D3DXCreateTextureFromFile() failed", "", MB_OK);
		return;
	}
}

void d3dclass::drawLTGrid()
{
	if(FAILED(md3dDevice->SetStreamSource(0, mVBLTGrid, 0 , sizeof(VPosNormalTex))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetIndices(mIBLTGrid)))
		MessageBox(NULL, "SetIndices() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetVertexDeclaration(VPosNormalTex::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetTechnique(mhLT.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhTime, mtime)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);

	if(FAILED(mLightTexFX->SetValue(mhLT.mhCamPos, &mCamPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
		
		
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mLTGridMtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mLTGridMtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mLTGridMtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mLTGridMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mLTGridTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
	
	D3DXMATRIX	W,WIT;
	D3DXMatrixTranslation(&W, mLTGridPos.x, mLTGridPos.y, mLTGridPos.z);
	D3DXMatrixInverse(&WIT, 0, &W);
	D3DXMatrixTranspose(&WIT, &WIT);
	
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhW, &W)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWVP, &(W*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
		
	UINT nPass;
	if(FAILED(mLightTexFX->Begin(&nPass, 0)))
	{
		MessageBox(NULL, "mLightTexFX->begin() failed", "", MB_OK);
		return;
	}
	for(UINT i=0; i<nPass; ++i)
	{
		if(FAILED(mLightTexFX->BeginPass(i)))
		{
		MessageBox(NULL, "mLightTexFX->BeginPass() failed", "", MB_OK);
		}
		
		if(FAILED(md3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNVRLT*mNVCLT, 0, (mNVRLT-1)*(mNVCLT-1)*2)))
		MessageBox(NULL, "DrawIndexedPromitive() failed", "", MB_OK);
			
		if(FAILED(mLightTexFX->EndPass()))
		{
		MessageBox(NULL, "mLightTexFX->EndPass() failed", "", MB_OK);
		}
	}
	
	
	
	if(FAILED(mLightTexFX->End()))
	{
		MessageBox(NULL, "mLightTexFX->End() failed", "", MB_OK);
		return;
	}
}

D3DXVECTOR3& d3dclass::getCamPos()
{
	return mCamPos;
}

D3DXVECTOR3& d3dclass::getCamLook()
{
	return mCamLook;
}

D3DXVECTOR3& d3dclass::getCamUp()
{
	return mCamUp;
}

D3DXVECTOR3& d3dclass::getCamRight()
{
	return mCamRight;
}

void d3dclass::setCamUp(D3DXVECTOR3& v)
{
	mCamUp = v;
	return;
}

void d3dclass::setCamLook(D3DXVECTOR3& v)
{
	mCamLook = v;
	return;
}

void d3dclass::setCamRight(D3DXVECTOR3& v)
{
	mCamRight = v;
	return;
}

float d3dclass::getCamSpeed()
{
	return mCamSpeed;
}

float d3dclass::getTime()
{
	return mtime;
}

void d3dclass::createLightTexBoxBuffer(LPCSTR texname)
{
	if(FAILED(md3dDevice->CreateVertexBuffer(24*sizeof(VPosNormalTex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVBLTBox, 0)))
		MessageBox(NULL, "CreateVertexBuffer() failed", "", MB_OK);
		
	VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTBox->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "mVPLTBox->Lock() failed", "", MB_OK);
		
	vpnt[0] = VPosNormalTex(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vpnt[1] = VPosNormalTex(-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vpnt[2] = VPosNormalTex(0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vpnt[3] = VPosNormalTex(0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	
	vpnt[4] = VPosNormalTex(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vpnt[5] = VPosNormalTex(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vpnt[6] = VPosNormalTex(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vpnt[7] = VPosNormalTex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	
	vpnt[8] = VPosNormalTex(-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vpnt[9] = VPosNormalTex(-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vpnt[10] = VPosNormalTex(-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vpnt[11] = VPosNormalTex(-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	
	vpnt[12] = VPosNormalTex(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vpnt[13] = VPosNormalTex(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vpnt[14] = VPosNormalTex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vpnt[15] = VPosNormalTex(0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	
	vpnt[16] = VPosNormalTex(0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	vpnt[17] = VPosNormalTex(0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	vpnt[18] = VPosNormalTex(-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	vpnt[19] = VPosNormalTex(-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	
	vpnt[20] = VPosNormalTex(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vpnt[21] = VPosNormalTex(-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vpnt[22] = VPosNormalTex(0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vpnt[23] = VPosNormalTex(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	
	if(FAILED(mVBLTBox->Unlock()))
		MessageBox(NULL, "mVBLTBox->Unlock() failed", "", MB_OK);
	
	
	if(FAILED(md3dDevice->CreateIndexBuffer(36*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIBLTBox, 0)))
		MessageBox(NULL, "CreateIndexBuffer() failed", "", MB_OK);
		
	WORD* ind = 0;
	if(FAILED(mIBLTBox->Lock(0, 0, (void**)&ind, 0)))
		MessageBox(NULL, "mIBLTBox->Lock() failed", NULL, MB_OK);
		
	ind[0] = 0;
	ind[1] = 1;
	ind[2] = 2;
	ind[3] = 0;
	ind[4] = 2;
	ind[5] = 3;
	
	ind[6] = 4;
	ind[7] = 5;
	ind[8] = 6;
	ind[9] = 4;
	ind[10] = 6;
	ind[11] = 7;
	
	ind[12] = 8;
	ind[13] = 9;
	ind[14] = 10;
	ind[15] = 8;
	ind[16] = 10;
	ind[17] = 11;
	
	ind[18] = 12;
	ind[19] = 13;
	ind[20] = 14;
	ind[21] = 12;
	ind[22] = 14;
	ind[23] = 15;
	
	ind[24] = 16;
	ind[25] = 17;
	ind[26] = 18;
	ind[27] = 16;
	ind[28] = 18;
	ind[29] = 19;
	
	ind[30] = 20;
	ind[31] = 21;
	ind[32] = 22;
	ind[33] = 20;
	ind[34] = 22;
	ind[35] = 23;
	
	if(FAILED(mIBLTBox->Unlock()))
		MessageBox(NULL, "mIBLTBox->Unlock() failed", "", MB_OK);
		
	if(FAILED(D3DXCreateTextureFromFile(md3dDevice, texname, &mLTBoxTex)))
	{
		MessageBox(NULL, "createLightTexBoxBuffer() D3DXCreateTextureFromFile() failed", "", MB_OK);
		return;
	}
}
	
void d3dclass::drawLTBox()
{
	if(FAILED(md3dDevice->SetStreamSource(0, mVBLTBox, 0 , sizeof(VPosNormalTex))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetIndices(mIBLTBox)))
		MessageBox(NULL, "SetIndices() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetVertexDeclaration(VPosNormalTex::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetTechnique(mhLT.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhTime, mtime)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);

	if(FAILED(mLightTexFX->SetValue(mhLT.mhCamPos, &mCamPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
		
		
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mLTBoxMtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mLTBoxMtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mLTBoxMtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mLTBoxMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mLTBoxTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
	
	D3DXMATRIX	W,WIT;
	D3DXMatrixTranslation(&W, mLTBoxPos.x, mLTBoxPos.y, mLTBoxPos.z);
	D3DXMatrixInverse(&WIT, 0, &W);
	D3DXMatrixTranspose(&WIT, &WIT);
	
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhW, &W)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWVP, &(W*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
		
	UINT nPass;
	if(FAILED(mLightTexFX->Begin(&nPass, 0)))
	{
		MessageBox(NULL, "mLightTexFX->begin() failed", "", MB_OK);
		return;
	}
	for(UINT i=0; i<nPass; ++i)
	{
		if(FAILED(mLightTexFX->BeginPass(i)))
		{
		MessageBox(NULL, "mLightTexFX->BeginPass() failed", "", MB_OK);
		}
		
		if(FAILED(md3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12)))
		MessageBox(NULL, "DrawIndexedPromitive() failed", "", MB_OK);
			
		if(FAILED(mLightTexFX->EndPass()))
		{
		MessageBox(NULL, "mLightTexFX->EndPass() failed", "", MB_OK);
		}
	}
	
	
	
	if(FAILED(mLightTexFX->End()))
	{
		MessageBox(NULL, "mLightTexFX->End() failed", "", MB_OK);
		return;
	}
}

void d3dclass::setLTBoxPos(D3DXVECTOR3& p)
{
	mLTBoxPos = p;
}

void d3dclass::createLightTexSpringBuffer(LPCSTR texname)
{
	if(FAILED(md3dDevice->CreateVertexBuffer(2042*sizeof(VPosNormalTex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVBLTSpring, 0)))
		MessageBox(NULL, "CreateVertexBuffer() failed", "", MB_OK);
		
	VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTSpring->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "mVPLTSpring->Lock() failed", "", MB_OK);
		
	vpnt[0] = VPosNormalTex(-1,0,0,-1,0,0,0,1);
vpnt[1] = VPosNormalTex(-1,0,0.02,-1,0,0,0.25,1);
vpnt[2] = VPosNormalTex(-1,0.01,0.0173205,-1,0,0,0.166667,1);
vpnt[3] = VPosNormalTex(-1,0.0173205,0.01,-1,0,0,0.0833333,1);
vpnt[4] = VPosNormalTex(-1,0.02,0,-1,0,0,0,1);
vpnt[5] = VPosNormalTex(-1,0.0173205,-0.01,-1,0,0,-0.0833333,1);
vpnt[6] = VPosNormalTex(-1,0.01,-0.0173205,-1,0,0,-0.166667,1);
vpnt[7] = VPosNormalTex(-1,0,-0.02,-1,0,0,-0.25,1);
vpnt[8] = VPosNormalTex(-1,-0.01,-0.0173205,-1,0,0,-0.333333,1);
vpnt[9] = VPosNormalTex(-1,-0.0173205,-0.01,-1,0,0,-0.416667,1);
vpnt[10] = VPosNormalTex(-1,-0.02,0,-1,0,0,0.5,1);
vpnt[11] = VPosNormalTex(-1,-0.0173205,0.01,-1,0,0,0.416667,1);
vpnt[12] = VPosNormalTex(-1,-0.01,0.0173205,-1,0,0,0.333333,1);
vpnt[13] = VPosNormalTex(-1,0,0.02,-0,0.258819,0.965926,0.25,1);
vpnt[14] = VPosNormalTex(-1,0.01,0.0173205,-0,0.258819,0.965926,0.166667,1);
vpnt[15] = VPosNormalTex(-0.875,0.01,0.0173205,-0,0.258819,0.965926,0.166667,0.9375);
vpnt[16] = VPosNormalTex(-0.875,0,0.02,-0,0.258819,0.965926,0.25,0.9375);
vpnt[17] = VPosNormalTex(-1,0.01,0.0173205,-0,0.707107,0.707107,0.166667,1);
vpnt[18] = VPosNormalTex(-1,0.0173205,0.01,-0,0.707107,0.707107,0.0833333,1);
vpnt[19] = VPosNormalTex(-0.875,0.0173205,0.01,-0,0.707107,0.707107,0.0833333,0.9375);
vpnt[20] = VPosNormalTex(-0.875,0.01,0.0173205,-0,0.707107,0.707107,0.166667,0.9375);
vpnt[21] = VPosNormalTex(-1,0.0173205,0.01,-0,0.965926,0.258819,0.0833333,1);
vpnt[22] = VPosNormalTex(-1,0.02,0,-0,0.965926,0.258819,0,1);
vpnt[23] = VPosNormalTex(-0.875,0.02,0,-0,0.965926,0.258819,0,0.9375);
vpnt[24] = VPosNormalTex(-0.875,0.0173205,0.01,-0,0.965926,0.258819,0.0833333,0.9375);
vpnt[25] = VPosNormalTex(-1,0.02,0,0,0.965926,-0.258819,0,1);
vpnt[26] = VPosNormalTex(-1,0.0173205,-0.01,0,0.965926,-0.258819,-0.0833333,1);
vpnt[27] = VPosNormalTex(-0.875,0.0173205,-0.01,0,0.965926,-0.258819,-0.0833333,0.9375);
vpnt[28] = VPosNormalTex(-0.875,0.02,0,0,0.965926,-0.258819,0,0.9375);
vpnt[29] = VPosNormalTex(-1,0.0173205,-0.01,0,0.707107,-0.707107,-0.0833333,1);
vpnt[30] = VPosNormalTex(-1,0.01,-0.0173205,0,0.707107,-0.707107,-0.166667,1);
vpnt[31] = VPosNormalTex(-0.875,0.01,-0.0173205,0,0.707107,-0.707107,-0.166667,0.9375);
vpnt[32] = VPosNormalTex(-0.875,0.0173205,-0.01,0,0.707107,-0.707107,-0.0833333,0.9375);
vpnt[33] = VPosNormalTex(-1,0.01,-0.0173205,0,0.258819,-0.965926,-0.166667,1);
vpnt[34] = VPosNormalTex(-1,0,-0.02,0,0.258819,-0.965926,-0.25,1);
vpnt[35] = VPosNormalTex(-0.875,0,-0.02,0,0.258819,-0.965926,-0.25,0.9375);
vpnt[36] = VPosNormalTex(-0.875,0.01,-0.0173205,0,0.258819,-0.965926,-0.166667,0.9375);
vpnt[37] = VPosNormalTex(-1,0,-0.02,0,-0.258819,-0.965926,-0.25,1);
vpnt[38] = VPosNormalTex(-1,-0.01,-0.0173205,0,-0.258819,-0.965926,-0.333333,1);
vpnt[39] = VPosNormalTex(-0.875,-0.01,-0.0173205,0,-0.258819,-0.965926,-0.333333,0.9375);
vpnt[40] = VPosNormalTex(-0.875,0,-0.02,0,-0.258819,-0.965926,-0.25,0.9375);
vpnt[41] = VPosNormalTex(-1,-0.01,-0.0173205,0,-0.707107,-0.707107,-0.333333,1);
vpnt[42] = VPosNormalTex(-1,-0.0173205,-0.01,0,-0.707107,-0.707107,-0.416667,1);
vpnt[43] = VPosNormalTex(-0.875,-0.0173205,-0.01,0,-0.707107,-0.707107,-0.416667,0.9375);
vpnt[44] = VPosNormalTex(-0.875,-0.01,-0.0173205,0,-0.707107,-0.707107,-0.333333,0.9375);
vpnt[45] = VPosNormalTex(-1,-0.0173205,-0.01,0,-0.965926,-0.258819,-0.416667,1);
vpnt[46] = VPosNormalTex(-1,-0.02,0,0,-0.965926,-0.258819,0.5,1);
vpnt[47] = VPosNormalTex(-0.875,-0.02,0,0,-0.965926,-0.258819,0.5,0.9375);
vpnt[48] = VPosNormalTex(-0.875,-0.0173205,-0.01,0,-0.965926,-0.258819,-0.416667,0.9375);
vpnt[49] = VPosNormalTex(-1,-0.02,0,0,-0.965926,0.258819,0.5,1);
vpnt[50] = VPosNormalTex(-1,-0.0173205,0.01,0,-0.965926,0.258819,0.416667,1);
vpnt[51] = VPosNormalTex(-0.875,-0.0173205,0.01,0,-0.965926,0.258819,0.416667,0.9375);
vpnt[52] = VPosNormalTex(-0.875,-0.02,0,0,-0.965926,0.258819,0.5,0.9375);
vpnt[53] = VPosNormalTex(-1,-0.0173205,0.01,0,-0.707107,0.707107,0.416667,1);
vpnt[54] = VPosNormalTex(-1,-0.01,0.0173205,0,-0.707107,0.707107,0.333333,1);
vpnt[55] = VPosNormalTex(-0.875,-0.01,0.0173205,0,-0.707107,0.707107,0.333333,0.9375);
vpnt[56] = VPosNormalTex(-0.875,-0.0173205,0.01,0,-0.707107,0.707107,0.416667,0.9375);
vpnt[57] = VPosNormalTex(-1,-0.01,0.0173205,0,-0.258819,0.965926,0.333333,1);
vpnt[58] = VPosNormalTex(-1,0,0.02,0,-0.258819,0.965926,0.25,1);
vpnt[59] = VPosNormalTex(-0.875,0,0.02,0,-0.258819,0.965926,0.25,0.9375);
vpnt[60] = VPosNormalTex(-0.875,-0.01,0.0173205,0,-0.258819,0.965926,0.333333,0.9375);
vpnt[61] = VPosNormalTex(-0.875,0,0.02,-0.200059,0.253587,0.946398,0.25,0.9375);
vpnt[62] = VPosNormalTex(-0.875,0.01,0.0173205,-0.200059,0.253587,0.946398,0.166667,0.9375);
vpnt[63] = VPosNormalTex(-0.83125,0.0085387,0.0269604,-0.200059,0.253587,0.946398,0.201184,0.915625);
vpnt[64] = VPosNormalTex(-0.83125,-0.0014613,0.0296399,-0.200059,0.253587,0.946398,0.25784,0.915625);
vpnt[65] = VPosNormalTex(-0.875,0.01,0.0173205,-0.131046,0.701009,0.701009,0.166667,0.9375);
vpnt[66] = VPosNormalTex(-0.875,0.0173205,0.01,-0.131046,0.701009,0.701009,0.0833333,0.9375);
vpnt[67] = VPosNormalTex(-0.83125,0.0158592,0.0196399,-0.131046,0.701009,0.701009,0.141886,0.915625);
vpnt[68] = VPosNormalTex(-0.83125,0.0085387,0.0269604,-0.131046,0.701009,0.701009,0.201184,0.915625);
vpnt[69] = VPosNormalTex(-0.875,0.0173205,0.01,-0.0247575,0.96563,0.25874,0.0833333,0.9375);
vpnt[70] = VPosNormalTex(-0.875,0.02,0,-0.0247575,0.96563,0.25874,0,0.9375);
vpnt[71] = VPosNormalTex(-0.83125,0.0185387,0.00963987,-0.0247575,0.96563,0.25874,0.0763162,0.915625);
vpnt[72] = VPosNormalTex(-0.83125,0.0158592,0.0196399,-0.0247575,0.96563,0.25874,0.141886,0.915625);
vpnt[73] = VPosNormalTex(-0.875,0.02,0,0.0889374,0.962098,-0.257793,0,0.9375);
vpnt[74] = VPosNormalTex(-0.875,0.0173205,-0.01,0.0889374,0.962098,-0.257793,-0.0833333,0.9375);
vpnt[75] = VPosNormalTex(-0.83125,0.0158592,-0.000360128,0.0889374,0.962098,-0.257793,-0.00361344,0.915625);
vpnt[76] = VPosNormalTex(-0.83125,0.0185387,0.00963987,0.0889374,0.962098,-0.257793,0.0763162,0.915625);
vpnt[77] = VPosNormalTex(-0.875,0.0173205,-0.01,0.176602,0.695993,-0.695993,-0.0833333,0.9375);
vpnt[78] = VPosNormalTex(-0.875,0.01,-0.0173205,0.176602,0.695993,-0.695993,-0.166667,0.9375);
vpnt[79] = VPosNormalTex(-0.83125,0.0085387,-0.00768064,0.176602,0.695993,-0.695993,-0.116588,0.915625);
vpnt[80] = VPosNormalTex(-0.83125,0.0158592,-0.000360128,0.176602,0.695993,-0.695993,-0.00361344,0.915625);
vpnt[81] = VPosNormalTex(-0.875,0.01,-0.0173205,0.216237,0.252696,-0.943073,-0.166667,0.9375);
vpnt[82] = VPosNormalTex(-0.875,0,-0.02,0.216237,0.252696,-0.943073,-0.25,0.9375);
vpnt[83] = VPosNormalTex(-0.83125,-0.0014613,-0.0103601,0.216237,0.252696,-0.943073,-0.272302,0.915625);
vpnt[84] = VPosNormalTex(-0.83125,0.0085387,-0.00768064,0.216237,0.252696,-0.943073,-0.116588,0.915625);
vpnt[85] = VPosNormalTex(-0.875,0,-0.02,0.200059,-0.253587,-0.946398,-0.25,0.9375);
vpnt[86] = VPosNormalTex(-0.875,-0.01,-0.0173205,0.200059,-0.253587,-0.946398,-0.333333,0.9375);
vpnt[87] = VPosNormalTex(-0.83125,-0.0114613,-0.00768064,0.200059,-0.253587,-0.946398,-0.406035,0.915625);
vpnt[88] = VPosNormalTex(-0.83125,-0.0014613,-0.0103601,0.200059,-0.253587,-0.946398,-0.272302,0.915625);
vpnt[89] = VPosNormalTex(-0.875,-0.01,-0.0173205,0.131046,-0.701009,-0.701009,-0.333333,0.9375);
vpnt[90] = VPosNormalTex(-0.875,-0.0173205,-0.01,0.131046,-0.701009,-0.701009,-0.416667,0.9375);
vpnt[91] = VPosNormalTex(-0.83125,-0.0187818,-0.000360128,0.131046,-0.701009,-0.701009,-0.496949,0.915625);
vpnt[92] = VPosNormalTex(-0.83125,-0.0114613,-0.00768064,0.131046,-0.701009,-0.701009,-0.406035,0.915625);
vpnt[93] = VPosNormalTex(-0.875,-0.0173205,-0.01,0.0247575,-0.96563,-0.25874,-0.416667,0.9375);
vpnt[94] = VPosNormalTex(-0.875,-0.02,0,0.0247575,-0.96563,-0.25874,0.5,0.9375);
vpnt[95] = VPosNormalTex(-0.83125,-0.0214613,0.00963987,0.0247575,-0.96563,-0.25874,0.43281,0.915625);
vpnt[96] = VPosNormalTex(-0.83125,-0.0187818,-0.000360128,0.0247575,-0.96563,-0.25874,-0.496949,0.915625);
vpnt[97] = VPosNormalTex(-0.875,-0.02,0,-0.0889374,-0.962098,0.257793,0.5,0.9375);
vpnt[98] = VPosNormalTex(-0.875,-0.0173205,0.01,-0.0889374,-0.962098,0.257793,0.416667,0.9375);
vpnt[99] = VPosNormalTex(-0.83125,-0.0187818,0.0196399,-0.0889374,-0.962098,0.257793,0.371446,0.915625);
vpnt[100] = VPosNormalTex(-0.83125,-0.0214613,0.00963987,-0.0889374,-0.962098,0.257793,0.43281,0.915625);
vpnt[101] = VPosNormalTex(-0.875,-0.0173205,0.01,-0.176602,-0.695993,0.695993,0.416667,0.9375);
vpnt[102] = VPosNormalTex(-0.875,-0.01,0.0173205,-0.176602,-0.695993,0.695993,0.333333,0.9375);
vpnt[103] = VPosNormalTex(-0.83125,-0.0114613,0.0269604,-0.176602,-0.695993,0.695993,0.313975,0.915625);
vpnt[104] = VPosNormalTex(-0.83125,-0.0187818,0.0196399,-0.176602,-0.695993,0.695993,0.371446,0.915625);
vpnt[105] = VPosNormalTex(-0.875,-0.01,0.0173205,-0.216237,-0.252696,0.943073,0.333333,0.9375);
vpnt[106] = VPosNormalTex(-0.875,0,0.02,-0.216237,-0.252696,0.943073,0.25,0.9375);
vpnt[107] = VPosNormalTex(-0.83125,-0.0014613,0.0296399,-0.216237,-0.252696,0.943073,0.25784,0.915625);
vpnt[108] = VPosNormalTex(-0.83125,-0.0114613,0.0269604,-0.216237,-0.252696,0.943073,0.313975,0.915625);
vpnt[109] = VPosNormalTex(-0.83125,-0.0014613,0.0296399,-0.861667,0.131344,0.490183,0.25784,0.915625);
vpnt[110] = VPosNormalTex(-0.83125,0.0085387,0.0269604,-0.861667,0.131344,0.490183,0.201184,0.915625);
vpnt[111] = VPosNormalTex(-0.7875,0.0850988,0.0833521,-0.861667,0.131344,0.490183,0.12335,0.89375);
vpnt[112] = VPosNormalTex(-0.7875,0.0750988,0.0860316,-0.861667,0.131344,0.490183,0.135782,0.89375);
vpnt[113] = VPosNormalTex(-0.83125,0.0085387,0.0269604,-0.906633,0.298343,0.298343,0.201184,0.915625);
vpnt[114] = VPosNormalTex(-0.83125,0.0158592,0.0196399,-0.906633,0.298343,0.298343,0.141886,0.915625);
vpnt[115] = VPosNormalTex(-0.7875,0.0924193,0.0760316,-0.906633,0.298343,0.298343,0.109565,0.89375);
vpnt[116] = VPosNormalTex(-0.7875,0.0850988,0.0833521,-0.906633,0.298343,0.298343,0.12335,0.89375);
vpnt[117] = VPosNormalTex(-0.83125,0.0158592,0.0196399,-0.896536,0.427876,0.114649,0.141886,0.915625);
vpnt[118] = VPosNormalTex(-0.83125,0.0185387,0.00963987,-0.896536,0.427876,0.114649,0.0763162,0.915625);
vpnt[119] = VPosNormalTex(-0.7875,0.0950987,0.0660316,-0.896536,0.427876,0.114649,0.0965947,0.89375);
vpnt[120] = VPosNormalTex(-0.7875,0.0924193,0.0760316,-0.896536,0.427876,0.114649,0.109565,0.89375);
vpnt[121] = VPosNormalTex(-0.83125,0.0185387,0.00963987,-0.804965,0.573105,-0.153563,0.0763162,0.915625);
vpnt[122] = VPosNormalTex(-0.83125,0.0158592,-0.000360128,-0.804965,0.573105,-0.153563,-0.00361344,0.915625);
vpnt[123] = VPosNormalTex(-0.7875,0.0924193,0.0560316,-0.804965,0.573105,-0.153563,0.0867429,0.89375);
vpnt[124] = VPosNormalTex(-0.7875,0.0950987,0.0660316,-0.804965,0.573105,-0.153563,0.0965947,0.89375);
vpnt[125] = VPosNormalTex(-0.83125,0.0158592,-0.000360128,-0.309919,0.672291,-0.672291,-0.00361344,0.915625);
vpnt[126] = VPosNormalTex(-0.83125,0.0085387,-0.00768064,-0.309919,0.672291,-0.672291,-0.116588,0.915625);
vpnt[127] = VPosNormalTex(-0.7875,0.0850988,0.0487111,-0.309919,0.672291,-0.672291,0.082742,0.89375);
vpnt[128] = VPosNormalTex(-0.7875,0.0924193,0.0560316,-0.309919,0.672291,-0.672291,0.0867429,0.89375);
vpnt[129] = VPosNormalTex(-0.83125,0.0085387,-0.00768064,0.620919,0.202882,-0.757165,-0.116588,0.915625);
vpnt[130] = VPosNormalTex(-0.83125,-0.0014613,-0.0103601,0.620919,0.202882,-0.757165,-0.272302,0.915625);
vpnt[131] = VPosNormalTex(-0.7875,0.0750988,0.0460316,0.620919,0.202882,-0.757165,0.087517,0.89375);
vpnt[132] = VPosNormalTex(-0.7875,0.0850988,0.0487111,0.620919,0.202882,-0.757165,0.082742,0.89375);
vpnt[133] = VPosNormalTex(-0.83125,-0.0014613,-0.0103601,0.861667,-0.131344,-0.490183,-0.272302,0.915625);
vpnt[134] = VPosNormalTex(-0.83125,-0.0114613,-0.00768064,0.861667,-0.131344,-0.490183,-0.406035,0.915625);
vpnt[135] = VPosNormalTex(-0.7875,0.0650988,0.0487111,0.861667,-0.131344,-0.490183,0.10224,0.89375);
vpnt[136] = VPosNormalTex(-0.7875,0.0750988,0.0460316,0.861667,-0.131344,-0.490183,0.087517,0.89375);
vpnt[137] = VPosNormalTex(-0.83125,-0.0114613,-0.00768064,0.906633,-0.298343,-0.298343,-0.406035,0.915625);
vpnt[138] = VPosNormalTex(-0.83125,-0.0187818,-0.000360128,0.906633,-0.298343,-0.298343,-0.496949,0.915625);
vpnt[139] = VPosNormalTex(-0.7875,0.0577782,0.0560316,0.906633,-0.298343,-0.298343,0.122558,0.89375);
vpnt[140] = VPosNormalTex(-0.7875,0.0650988,0.0487111,0.906633,-0.298343,-0.298343,0.10224,0.89375);
vpnt[141] = VPosNormalTex(-0.83125,-0.0187818,-0.000360128,0.896536,-0.427876,-0.114649,-0.496949,0.915625);
vpnt[142] = VPosNormalTex(-0.83125,-0.0214613,0.00963987,0.896536,-0.427876,-0.114649,0.43281,0.915625);
vpnt[143] = VPosNormalTex(-0.7875,0.0550988,0.0660316,0.896536,-0.427876,-0.114649,0.139326,0.89375);
vpnt[144] = VPosNormalTex(-0.7875,0.0577782,0.0560316,0.896536,-0.427876,-0.114649,0.122558,0.89375);
vpnt[145] = VPosNormalTex(-0.83125,-0.0214613,0.00963987,0.804965,-0.573105,0.153563,0.43281,0.915625);
vpnt[146] = VPosNormalTex(-0.83125,-0.0187818,0.0196399,0.804965,-0.573105,0.153563,0.371446,0.915625);
vpnt[147] = VPosNormalTex(-0.7875,0.0577782,0.0760316,0.804965,-0.573105,0.153563,0.146578,0.89375);
vpnt[148] = VPosNormalTex(-0.7875,0.0550988,0.0660316,0.804965,-0.573105,0.153563,0.139326,0.89375);
vpnt[149] = VPosNormalTex(-0.83125,-0.0187818,0.0196399,0.309919,-0.672291,0.672291,0.371446,0.915625);
vpnt[150] = VPosNormalTex(-0.83125,-0.0114613,0.0269604,0.309919,-0.672291,0.672291,0.313975,0.915625);
vpnt[151] = VPosNormalTex(-0.7875,0.0650988,0.0833521,0.309919,-0.672291,0.672291,0.144472,0.89375);
vpnt[152] = VPosNormalTex(-0.7875,0.0577782,0.0760316,0.309919,-0.672291,0.672291,0.146578,0.89375);
vpnt[153] = VPosNormalTex(-0.83125,-0.0114613,0.0269604,-0.620919,-0.202882,0.757165,0.313975,0.915625);
vpnt[154] = VPosNormalTex(-0.83125,-0.0014613,0.0296399,-0.620919,-0.202882,0.757165,0.25784,0.915625);
vpnt[155] = VPosNormalTex(-0.7875,0.0750988,0.0860316,-0.620919,-0.202882,0.757165,0.135782,0.89375);
vpnt[156] = VPosNormalTex(-0.7875,0.0650988,0.0833521,-0.620919,-0.202882,0.757165,0.144472,0.89375);
vpnt[157] = VPosNormalTex(-0.7875,0.0750988,0.0860316,0.888949,0.118541,0.442401,0.135782,0.89375);
vpnt[158] = VPosNormalTex(-0.7875,0.0850988,0.0833521,0.888949,0.118541,0.442401,0.12335,0.89375);
vpnt[159] = VPosNormalTex(-0.74375,0.10614,-0.0101958,0.888949,0.118541,0.442401,-0.0152417,0.871875);
vpnt[160] = VPosNormalTex(-0.74375,0.0961398,-0.00751629,0.888949,0.118541,0.442401,-0.0124176,0.871875);
vpnt[161] = VPosNormalTex(-0.7875,0.0850988,0.0833521,0.760691,0.458993,0.458993,0.12335,0.89375);
vpnt[162] = VPosNormalTex(-0.7875,0.0924193,0.0760316,0.760691,0.458993,0.458993,0.109565,0.89375);
vpnt[163] = VPosNormalTex(-0.74375,0.11346,-0.0175163,0.760691,0.458993,0.458993,-0.0243783,0.871875);
vpnt[164] = VPosNormalTex(-0.74375,0.10614,-0.0101958,0.760691,0.458993,0.458993,-0.0152417,0.871875);
vpnt[165] = VPosNormalTex(-0.7875,0.0924193,0.0760316,0.088518,0.962134,0.257803,0.109565,0.89375);
vpnt[166] = VPosNormalTex(-0.7875,0.0950987,0.0660316,0.088518,0.962134,0.257803,0.0965947,0.89375);
vpnt[167] = VPosNormalTex(-0.74375,0.11614,-0.0275163,0.088518,0.962134,0.257803,-0.0370249,0.871875);
vpnt[168] = VPosNormalTex(-0.74375,0.11346,-0.0175163,0.088518,0.962134,0.257803,-0.0243783,0.871875);
vpnt[169] = VPosNormalTex(-0.7875,0.0950987,0.0660316,-0.713375,0.676904,-0.181376,0.0965947,0.89375);
vpnt[170] = VPosNormalTex(-0.7875,0.0924193,0.0560316,-0.713375,0.676904,-0.181376,0.0867429,0.89375);
vpnt[171] = VPosNormalTex(-0.74375,0.11346,-0.0375163,-0.713375,0.676904,-0.181376,-0.0508243,0.871875);
vpnt[172] = VPosNormalTex(-0.74375,0.11614,-0.0275163,-0.713375,0.676904,-0.181376,-0.0370249,0.871875);
vpnt[173] = VPosNormalTex(-0.7875,0.0924193,0.0560316,-0.879926,0.335955,-0.335955,0.0867429,0.89375);
vpnt[174] = VPosNormalTex(-0.7875,0.0850988,0.0487111,-0.879926,0.335955,-0.335955,0.082742,0.89375);
vpnt[175] = VPosNormalTex(-0.74375,0.10614,-0.0448368,-0.879926,0.335955,-0.335955,-0.0636132,0.871875);
vpnt[176] = VPosNormalTex(-0.74375,0.11346,-0.0375163,-0.879926,0.335955,-0.335955,-0.0508243,0.871875);
vpnt[177] = VPosNormalTex(-0.7875,0.0850988,0.0487111,-0.909644,0.107511,-0.401235,0.082742,0.89375);
vpnt[178] = VPosNormalTex(-0.7875,0.0750988,0.0460316,-0.909644,0.107511,-0.401235,0.087517,0.89375);
vpnt[179] = VPosNormalTex(-0.74375,0.0961398,-0.0475163,-0.909644,0.107511,-0.401235,-0.073057,0.871875);
vpnt[180] = VPosNormalTex(-0.74375,0.10614,-0.0448368,-0.909644,0.107511,-0.401235,-0.0636132,0.871875);
vpnt[181] = VPosNormalTex(-0.7875,0.0750988,0.0460316,-0.888949,-0.118541,-0.442401,0.087517,0.89375);
vpnt[182] = VPosNormalTex(-0.7875,0.0650988,0.0487111,-0.888949,-0.118541,-0.442401,0.10224,0.89375);
vpnt[183] = VPosNormalTex(-0.74375,0.0861398,-0.0448368,-0.888949,-0.118541,-0.442401,-0.076382,0.871875);
vpnt[184] = VPosNormalTex(-0.74375,0.0961398,-0.0475163,-0.888949,-0.118541,-0.442401,-0.073057,0.871875);
vpnt[185] = VPosNormalTex(-0.7875,0.0650988,0.0487111,-0.760691,-0.458993,-0.458993,0.10224,0.89375);
vpnt[186] = VPosNormalTex(-0.7875,0.0577782,0.0560316,-0.760691,-0.458993,-0.458993,0.122558,0.89375);
vpnt[187] = VPosNormalTex(-0.74375,0.0788193,-0.0375163,-0.760691,-0.458993,-0.458993,-0.070704,0.871875);
vpnt[188] = VPosNormalTex(-0.74375,0.0861398,-0.0448368,-0.760691,-0.458993,-0.458993,-0.076382,0.871875);
vpnt[189] = VPosNormalTex(-0.7875,0.0577782,0.0560316,-0.0885188,-0.962134,-0.257803,0.122558,0.89375);
vpnt[190] = VPosNormalTex(-0.7875,0.0550988,0.0660316,-0.0885188,-0.962134,-0.257803,0.139326,0.89375);
vpnt[191] = VPosNormalTex(-0.74375,0.0761398,-0.0275163,-0.0885188,-0.962134,-0.257803,-0.0551929,0.871875);
vpnt[192] = VPosNormalTex(-0.74375,0.0788193,-0.0375163,-0.0885188,-0.962134,-0.257803,-0.070704,0.871875);
vpnt[193] = VPosNormalTex(-0.7875,0.0550988,0.0660316,0.713375,-0.676904,0.181376,0.139326,0.89375);
vpnt[194] = VPosNormalTex(-0.7875,0.0577782,0.0760316,0.713375,-0.676904,0.181376,0.146578,0.89375);
vpnt[195] = VPosNormalTex(-0.74375,0.0788193,-0.0175163,0.713375,-0.676904,0.181376,-0.034804,0.871875);
vpnt[196] = VPosNormalTex(-0.74375,0.0761398,-0.0275163,0.713375,-0.676904,0.181376,-0.0551929,0.871875);
vpnt[197] = VPosNormalTex(-0.7875,0.0577782,0.0760316,0.879926,-0.335955,0.335955,0.146578,0.89375);
vpnt[198] = VPosNormalTex(-0.7875,0.0650988,0.0833521,0.879926,-0.335955,0.335955,0.144472,0.89375);
vpnt[199] = VPosNormalTex(-0.74375,0.0861398,-0.0101958,0.879926,-0.335955,0.335955,-0.0187509,0.871875);
vpnt[200] = VPosNormalTex(-0.74375,0.0788193,-0.0175163,0.879926,-0.335955,0.335955,-0.034804,0.871875);
vpnt[201] = VPosNormalTex(-0.7875,0.0650988,0.0833521,0.909644,-0.107511,0.401235,0.144472,0.89375);
vpnt[202] = VPosNormalTex(-0.7875,0.0750988,0.0860316,0.909644,-0.107511,0.401235,0.135782,0.89375);
vpnt[203] = VPosNormalTex(-0.74375,0.0961398,-0.00751629,0.909644,-0.107511,0.401235,-0.0124176,0.871875);
vpnt[204] = VPosNormalTex(-0.74375,0.0861398,-0.0101958,0.909644,-0.107511,0.401235,-0.0187509,0.871875);
vpnt[205] = VPosNormalTex(-0.74375,0.0961398,-0.00751629,0.885448,0.120283,0.448902,-0.0124176,0.871875);
vpnt[206] = VPosNormalTex(-0.74375,0.10614,-0.0101958,0.885448,0.120283,0.448902,-0.0152417,0.871875);
vpnt[207] = VPosNormalTex(-0.7,0.0387904,-0.0784454,0.885448,0.120283,0.448902,-0.176911,0.85);
vpnt[208] = VPosNormalTex(-0.7,0.0287904,-0.0757659,0.885448,0.120283,0.448902,-0.192205,0.85);
vpnt[209] = VPosNormalTex(-0.74375,0.10614,-0.0101958,0.909769,0.29353,0.29353,-0.0152417,0.871875);
vpnt[210] = VPosNormalTex(-0.74375,0.11346,-0.0175163,0.909769,0.29353,0.29353,-0.0243783,0.871875);
vpnt[211] = VPosNormalTex(-0.7,0.0461109,-0.0857659,0.909769,0.29353,0.29353,-0.171489,0.85);
vpnt[212] = VPosNormalTex(-0.7,0.0387904,-0.0784454,0.909769,0.29353,0.29353,-0.176911,0.85);
vpnt[213] = VPosNormalTex(-0.74375,0.11346,-0.0175163,0.883975,0.451604,0.121007,-0.0243783,0.871875);
vpnt[214] = VPosNormalTex(-0.74375,0.11614,-0.0275163,0.883975,0.451604,0.121007,-0.0370249,0.871875);
vpnt[215] = VPosNormalTex(-0.7,0.0487904,-0.0957659,0.883975,0.451604,0.121007,-0.175006,0.85);
vpnt[216] = VPosNormalTex(-0.7,0.0461109,-0.0857659,0.883975,0.451604,0.121007,-0.171489,0.85);
vpnt[217] = VPosNormalTex(-0.74375,0.11614,-0.0275163,0.734763,0.65521,-0.175563,-0.0370249,0.871875);
vpnt[218] = VPosNormalTex(-0.74375,0.11346,-0.0375163,0.734763,0.65521,-0.175563,-0.0508243,0.871875);
vpnt[219] = VPosNormalTex(-0.7,0.0461109,-0.105766,0.734763,0.65521,-0.175563,-0.184567,0.85);
vpnt[220] = VPosNormalTex(-0.7,0.0487904,-0.0957659,0.734763,0.65521,-0.175563,-0.175006,0.85);
vpnt[221] = VPosNormalTex(-0.74375,0.11346,-0.0375163,-0.0145486,0.707032,-0.707032,-0.0508243,0.871875);
vpnt[222] = VPosNormalTex(-0.74375,0.10614,-0.0448368,-0.0145486,0.707032,-0.707032,-0.0636132,0.871875);
vpnt[223] = VPosNormalTex(-0.7,0.0387904,-0.113086,-0.0145486,0.707032,-0.707032,-0.197409,0.85);
vpnt[224] = VPosNormalTex(-0.7,0.0461109,-0.105766,-0.0145486,0.707032,-0.707032,-0.184567,0.85);
vpnt[225] = VPosNormalTex(-0.74375,0.10614,-0.0448368,-0.742483,0.173374,-0.64704,-0.0636132,0.871875);
vpnt[226] = VPosNormalTex(-0.74375,0.0961398,-0.0475163,-0.742483,0.173374,-0.64704,-0.073057,0.871875);
vpnt[227] = VPosNormalTex(-0.7,0.0287904,-0.115766,-0.742483,0.173374,-0.64704,-0.211206,0.85);
vpnt[228] = VPosNormalTex(-0.7,0.0387904,-0.113086,-0.742483,0.173374,-0.64704,-0.197409,0.85);
vpnt[229] = VPosNormalTex(-0.74375,0.0961398,-0.0475163,-0.885448,-0.120283,-0.448902,-0.073057,0.871875);
vpnt[230] = VPosNormalTex(-0.74375,0.0861398,-0.0448368,-0.885448,-0.120283,-0.448902,-0.076382,0.871875);
vpnt[231] = VPosNormalTex(-0.7,0.0187904,-0.113086,-0.885448,-0.120283,-0.448902,-0.223794,0.85);
vpnt[232] = VPosNormalTex(-0.7,0.0287904,-0.115766,-0.885448,-0.120283,-0.448902,-0.211206,0.85);
vpnt[233] = VPosNormalTex(-0.74375,0.0861398,-0.0448368,-0.909769,-0.29353,-0.29353,-0.076382,0.871875);
vpnt[234] = VPosNormalTex(-0.74375,0.0788193,-0.0375163,-0.909769,-0.29353,-0.29353,-0.070704,0.871875);
vpnt[235] = VPosNormalTex(-0.7,0.0114698,-0.105766,-0.909769,-0.29353,-0.29353,-0.232808,0.85);
vpnt[236] = VPosNormalTex(-0.7,0.0187904,-0.113086,-0.909769,-0.29353,-0.29353,-0.223794,0.85);
vpnt[237] = VPosNormalTex(-0.74375,0.0788193,-0.0375163,-0.883975,-0.451604,-0.121007,-0.070704,0.871875);
vpnt[238] = VPosNormalTex(-0.74375,0.0761398,-0.0275163,-0.883975,-0.451604,-0.121007,-0.0551929,0.871875);
vpnt[239] = VPosNormalTex(-0.7,0.00879036,-0.0957659,-0.883975,-0.451604,-0.121007,-0.235432,0.85);
vpnt[240] = VPosNormalTex(-0.7,0.0114698,-0.105766,-0.883975,-0.451604,-0.121007,-0.232808,0.85);
vpnt[241] = VPosNormalTex(-0.74375,0.0761398,-0.0275163,-0.734763,-0.65521,0.175563,-0.0551929,0.871875);
vpnt[242] = VPosNormalTex(-0.74375,0.0788193,-0.0175163,-0.734763,-0.65521,0.175563,-0.034804,0.871875);
vpnt[243] = VPosNormalTex(-0.7,0.0114698,-0.0857659,-0.734763,-0.65521,0.175563,-0.228841,0.85);
vpnt[244] = VPosNormalTex(-0.7,0.00879036,-0.0957659,-0.734763,-0.65521,0.175563,-0.235432,0.85);
vpnt[245] = VPosNormalTex(-0.74375,0.0788193,-0.0175163,0.0145487,-0.707032,0.707032,-0.034804,0.871875);
vpnt[246] = VPosNormalTex(-0.74375,0.0861398,-0.0101958,0.0145487,-0.707032,0.707032,-0.0187509,0.871875);
vpnt[247] = VPosNormalTex(-0.7,0.0187904,-0.0784454,0.0145487,-0.707032,0.707032,-0.212582,0.85);
vpnt[248] = VPosNormalTex(-0.7,0.0114698,-0.0857659,0.0145487,-0.707032,0.707032,-0.228841,0.85);
vpnt[249] = VPosNormalTex(-0.74375,0.0861398,-0.0101958,0.742483,-0.173374,0.64704,-0.0187509,0.871875);
vpnt[250] = VPosNormalTex(-0.74375,0.0961398,-0.00751629,0.742483,-0.173374,0.64704,-0.0124176,0.871875);
vpnt[251] = VPosNormalTex(-0.7,0.0287904,-0.0757659,0.742483,-0.173374,0.64704,-0.192205,0.85);
vpnt[252] = VPosNormalTex(-0.7,0.0187904,-0.0784454,0.742483,-0.173374,0.64704,-0.212582,0.85);
vpnt[253] = VPosNormalTex(-0.7,0.0287904,-0.0757659,0.117123,0.257038,0.959278,-0.192205,0.85);
vpnt[254] = VPosNormalTex(-0.7,0.0387904,-0.0784454,0.117123,0.257038,0.959278,-0.176911,0.85);
vpnt[255] = VPosNormalTex(-0.65625,-0.0550288,-0.0586483,0.117123,0.257038,0.959278,-0.369934,0.828125);
vpnt[256] = VPosNormalTex(-0.65625,-0.0650288,-0.0559688,0.117123,0.257038,0.959278,-0.386895,0.828125);
vpnt[257] = VPosNormalTex(-0.7,0.0387904,-0.0784454,0.767268,0.453486,0.453486,-0.176911,0.85);
vpnt[258] = VPosNormalTex(-0.7,0.0461109,-0.0857659,0.767268,0.453486,0.453486,-0.171489,0.85);
vpnt[259] = VPosNormalTex(-0.65625,-0.0477083,-0.0659688,0.767268,0.453486,0.453486,-0.349651,0.828125);
vpnt[260] = VPosNormalTex(-0.65625,-0.0550288,-0.0586483,0.767268,0.453486,0.453486,-0.369934,0.828125);
vpnt[261] = VPosNormalTex(-0.7,0.0461109,-0.0857659,0.89022,0.440008,0.1179,-0.171489,0.85);
vpnt[262] = VPosNormalTex(-0.7,0.0487904,-0.0957659,0.89022,0.440008,0.1179,-0.175006,0.85);
vpnt[263] = VPosNormalTex(-0.65625,-0.0450288,-0.0759688,0.89022,0.440008,0.1179,-0.335157,0.828125);
vpnt[264] = VPosNormalTex(-0.65625,-0.0477083,-0.0659688,0.89022,0.440008,0.1179,-0.349651,0.828125);
vpnt[265] = VPosNormalTex(-0.7,0.0487904,-0.0957659,0.909545,0.401443,-0.107567,-0.175006,0.85);
vpnt[266] = VPosNormalTex(-0.7,0.0461109,-0.105766,0.909545,0.401443,-0.107567,-0.184567,0.85);
vpnt[267] = VPosNormalTex(-0.65625,-0.0477083,-0.0859688,0.909545,0.401443,-0.107567,-0.330633,0.828125);
vpnt[268] = VPosNormalTex(-0.65625,-0.0450288,-0.0759688,0.909545,0.401443,-0.107567,-0.335157,0.828125);
vpnt[269] = VPosNormalTex(-0.7,0.0461109,-0.105766,0.878223,0.338175,-0.338175,-0.184567,0.85);
vpnt[270] = VPosNormalTex(-0.7,0.0387904,-0.113086,0.878223,0.338175,-0.338175,-0.197409,0.85);
vpnt[271] = VPosNormalTex(-0.65625,-0.0550288,-0.0932893,0.878223,0.338175,-0.338175,-0.33482,0.828125);
vpnt[272] = VPosNormalTex(-0.65625,-0.0477083,-0.0859688,0.878223,0.338175,-0.338175,-0.330633,0.828125);
vpnt[273] = VPosNormalTex(-0.7,0.0387904,-0.113086,0.704301,0.183736,-0.685712,-0.197409,0.85);
vpnt[274] = VPosNormalTex(-0.7,0.0287904,-0.115766,0.704301,0.183736,-0.685712,-0.211206,0.85);
vpnt[275] = VPosNormalTex(-0.65625,-0.0650288,-0.0959688,0.704301,0.183736,-0.685712,-0.344783,0.828125);
vpnt[276] = VPosNormalTex(-0.65625,-0.0550288,-0.0932893,0.704301,0.183736,-0.685712,-0.33482,0.828125);
vpnt[277] = VPosNormalTex(-0.7,0.0287904,-0.115766,-0.117123,-0.257038,-0.959278,-0.211206,0.85);
vpnt[278] = VPosNormalTex(-0.7,0.0187904,-0.113086,-0.117123,-0.257038,-0.959278,-0.223794,0.85);
vpnt[279] = VPosNormalTex(-0.65625,-0.0750288,-0.0932893,-0.117123,-0.257038,-0.959278,-0.357801,0.828125);
vpnt[280] = VPosNormalTex(-0.65625,-0.0650288,-0.0959688,-0.117123,-0.257038,-0.959278,-0.344783,0.828125);
vpnt[281] = VPosNormalTex(-0.7,0.0187904,-0.113086,-0.767268,-0.453486,-0.453486,-0.223794,0.85);
vpnt[282] = VPosNormalTex(-0.7,0.0114698,-0.105766,-0.767268,-0.453486,-0.453486,-0.232808,0.85);
vpnt[283] = VPosNormalTex(-0.65625,-0.0823493,-0.0859688,-0.767268,-0.453486,-0.453486,-0.371578,0.828125);
vpnt[284] = VPosNormalTex(-0.65625,-0.0750288,-0.0932893,-0.767268,-0.453486,-0.453486,-0.357801,0.828125);
vpnt[285] = VPosNormalTex(-0.7,0.0114698,-0.105766,-0.89022,-0.440008,-0.1179,-0.232808,0.85);
vpnt[286] = VPosNormalTex(-0.7,0.00879036,-0.0957659,-0.89022,-0.440008,-0.1179,-0.235432,0.85);
vpnt[287] = VPosNormalTex(-0.65625,-0.0850288,-0.0759688,-0.89022,-0.440008,-0.1179,-0.383947,0.828125);
vpnt[288] = VPosNormalTex(-0.65625,-0.0823493,-0.0859688,-0.89022,-0.440008,-0.1179,-0.371578,0.828125);
vpnt[289] = VPosNormalTex(-0.7,0.00879036,-0.0957659,-0.909545,-0.401444,0.107566,-0.235432,0.85);
vpnt[290] = VPosNormalTex(-0.7,0.0114698,-0.0857659,-0.909545,-0.401444,0.107566,-0.228841,0.85);
vpnt[291] = VPosNormalTex(-0.65625,-0.0823493,-0.0659688,-0.909545,-0.401444,0.107566,-0.392506,0.828125);
vpnt[292] = VPosNormalTex(-0.65625,-0.0850288,-0.0759688,-0.909545,-0.401444,0.107566,-0.383947,0.828125);
vpnt[293] = VPosNormalTex(-0.7,0.0114698,-0.0857659,-0.878223,-0.338175,0.338175,-0.228841,0.85);
vpnt[294] = VPosNormalTex(-0.7,0.0187904,-0.0784454,-0.878223,-0.338175,0.338175,-0.212582,0.85);
vpnt[295] = VPosNormalTex(-0.65625,-0.0750288,-0.0586483,-0.878223,-0.338175,0.338175,-0.394406,0.828125);
vpnt[296] = VPosNormalTex(-0.65625,-0.0823493,-0.0659688,-0.878223,-0.338175,0.338175,-0.392506,0.828125);
vpnt[297] = VPosNormalTex(-0.7,0.0187904,-0.0784454,-0.704301,-0.183736,0.685712,-0.212582,0.85);
vpnt[298] = VPosNormalTex(-0.7,0.0287904,-0.0757659,-0.704301,-0.183736,0.685712,-0.192205,0.85);
vpnt[299] = VPosNormalTex(-0.65625,-0.0650288,-0.0559688,-0.704301,-0.183736,0.685712,-0.386895,0.828125);
vpnt[300] = VPosNormalTex(-0.65625,-0.0750288,-0.0586483,-0.704301,-0.183736,0.685712,-0.394406,0.828125);
vpnt[301] = VPosNormalTex(-0.65625,-0.0650288,-0.0559688,-0.871581,0.126886,0.473546,-0.386895,0.828125);
vpnt[302] = VPosNormalTex(-0.65625,-0.0550288,-0.0586483,-0.871581,0.126886,0.473546,-0.369934,0.828125);
vpnt[303] = VPosNormalTex(-0.6125,-0.0890607,0.0309943,-0.871581,0.126886,0.473546,0.446698,0.80625);
vpnt[304] = VPosNormalTex(-0.6125,-0.0990607,0.0336737,-0.871581,0.126886,0.473546,0.447849,0.80625);
vpnt[305] = VPosNormalTex(-0.65625,-0.0550288,-0.0586483,-0.668473,0.525901,0.525901,-0.369934,0.828125);
vpnt[306] = VPosNormalTex(-0.65625,-0.0477083,-0.0659688,-0.668473,0.525901,0.525901,-0.349651,0.828125);
vpnt[307] = VPosNormalTex(-0.6125,-0.0817402,0.0236737,-0.668473,0.525901,0.525901,0.455133,0.80625);
vpnt[308] = VPosNormalTex(-0.6125,-0.0890607,0.0309943,-0.668473,0.525901,0.525901,0.446698,0.80625);
vpnt[309] = VPosNormalTex(-0.65625,-0.0477083,-0.0659688,0.215843,0.943157,0.252718,-0.349651,0.828125);
vpnt[310] = VPosNormalTex(-0.65625,-0.0450288,-0.0759688,0.215843,0.943157,0.252718,-0.335157,0.828125);
vpnt[311] = VPosNormalTex(-0.6125,-0.0790607,0.0136737,0.215843,0.943157,0.252718,0.472743,0.80625);
vpnt[312] = VPosNormalTex(-0.6125,-0.0817402,0.0236737,0.215843,0.943157,0.252718,0.455133,0.80625);
vpnt[313] = VPosNormalTex(-0.65625,-0.0450288,-0.0759688,0.788416,0.594182,-0.159211,-0.335157,0.828125);
vpnt[314] = VPosNormalTex(-0.65625,-0.0477083,-0.0859688,0.788416,0.594182,-0.159211,-0.330633,0.828125);
vpnt[315] = VPosNormalTex(-0.6125,-0.0817402,0.00367375,0.788416,0.594182,-0.159211,0.492852,0.80625);
vpnt[316] = VPosNormalTex(-0.6125,-0.0790607,0.0136737,0.788416,0.594182,-0.159211,0.472743,0.80625);
vpnt[317] = VPosNormalTex(-0.65625,-0.0477083,-0.0859688,0.894327,0.316369,-0.316369,-0.330633,0.828125);
vpnt[318] = VPosNormalTex(-0.65625,-0.0550288,-0.0932893,0.894327,0.316369,-0.316369,-0.33482,0.828125);
vpnt[319] = VPosNormalTex(-0.6125,-0.0890607,-0.00364676,0.894327,0.316369,-0.316369,-0.493487,0.80625);
vpnt[320] = VPosNormalTex(-0.6125,-0.0817402,0.00367375,0.894327,0.316369,-0.316369,0.492852,0.80625);
vpnt[321] = VPosNormalTex(-0.65625,-0.0550288,-0.0932893,0.908968,0.107893,-0.402661,-0.33482,0.828125);
vpnt[322] = VPosNormalTex(-0.65625,-0.0650288,-0.0959688,0.908968,0.107893,-0.402661,-0.344783,0.828125);
vpnt[323] = VPosNormalTex(-0.6125,-0.0990607,-0.00632625,0.908968,0.107893,-0.402661,-0.48985,0.80625);
vpnt[324] = VPosNormalTex(-0.6125,-0.0890607,-0.00364676,0.908968,0.107893,-0.402661,-0.493487,0.80625);
vpnt[325] = VPosNormalTex(-0.65625,-0.0650288,-0.0959688,0.871581,-0.126887,-0.473546,-0.344783,0.828125);
vpnt[326] = VPosNormalTex(-0.65625,-0.0750288,-0.0932893,0.871581,-0.126887,-0.473546,-0.357801,0.828125);
vpnt[327] = VPosNormalTex(-0.6125,-0.109061,-0.00364676,0.871581,-0.126887,-0.473546,-0.49468,0.80625);
vpnt[328] = VPosNormalTex(-0.6125,-0.0990607,-0.00632625,0.871581,-0.126887,-0.473546,-0.48985,0.80625);
vpnt[329] = VPosNormalTex(-0.65625,-0.0750288,-0.0932893,0.668473,-0.525901,-0.525901,-0.357801,0.828125);
vpnt[330] = VPosNormalTex(-0.65625,-0.0823493,-0.0859688,0.668473,-0.525901,-0.525901,-0.371578,0.828125);
vpnt[331] = VPosNormalTex(-0.6125,-0.116381,0.00367375,0.668473,-0.525901,-0.525901,0.494978,0.80625);
vpnt[332] = VPosNormalTex(-0.6125,-0.109061,-0.00364676,0.668473,-0.525901,-0.525901,-0.49468,0.80625);
vpnt[333] = VPosNormalTex(-0.65625,-0.0823493,-0.0859688,-0.215844,-0.943157,-0.252718,-0.371578,0.828125);
vpnt[334] = VPosNormalTex(-0.65625,-0.0850288,-0.0759688,-0.215844,-0.943157,-0.252718,-0.383947,0.828125);
vpnt[335] = VPosNormalTex(-0.6125,-0.119061,0.0136737,-0.215844,-0.943157,-0.252718,0.481801,0.80625);
vpnt[336] = VPosNormalTex(-0.6125,-0.116381,0.00367375,-0.215844,-0.943157,-0.252718,0.494978,0.80625);
vpnt[337] = VPosNormalTex(-0.65625,-0.0850288,-0.0759688,-0.788416,-0.594182,0.15921,-0.383947,0.828125);
vpnt[338] = VPosNormalTex(-0.65625,-0.0823493,-0.0659688,-0.788416,-0.594182,0.15921,-0.392506,0.828125);
vpnt[339] = VPosNormalTex(-0.6125,-0.116381,0.0236737,-0.788416,-0.594182,0.15921,0.468061,0.80625);
vpnt[340] = VPosNormalTex(-0.6125,-0.119061,0.0136737,-0.788416,-0.594182,0.15921,0.481801,0.80625);
vpnt[341] = VPosNormalTex(-0.65625,-0.0823493,-0.0659688,-0.894327,-0.316369,0.316369,-0.392506,0.828125);
vpnt[342] = VPosNormalTex(-0.65625,-0.0750288,-0.0586483,-0.894327,-0.316369,0.316369,-0.394406,0.828125);
vpnt[343] = VPosNormalTex(-0.6125,-0.109061,0.0309943,-0.894327,-0.316369,0.316369,0.455931,0.80625);
vpnt[344] = VPosNormalTex(-0.6125,-0.116381,0.0236737,-0.894327,-0.316369,0.316369,0.468061,0.80625);
vpnt[345] = VPosNormalTex(-0.65625,-0.0750288,-0.0586483,-0.908968,-0.107893,0.402661,-0.394406,0.828125);
vpnt[346] = VPosNormalTex(-0.65625,-0.0650288,-0.0559688,-0.908968,-0.107893,0.402661,-0.386895,0.828125);
vpnt[347] = VPosNormalTex(-0.6125,-0.0990607,0.0336737,-0.908968,-0.107893,0.402661,0.447849,0.80625);
vpnt[348] = VPosNormalTex(-0.6125,-0.109061,0.0309943,-0.908968,-0.107893,0.402661,0.455931,0.80625);
vpnt[349] = VPosNormalTex(-0.6125,-0.0990607,0.0336737,-0.89784,0.113963,0.425317,0.447849,0.80625);
vpnt[350] = VPosNormalTex(-0.6125,-0.0890607,0.0309943,-0.89784,0.113963,0.425317,0.446698,0.80625);
vpnt[351] = VPosNormalTex(-0.56875,-0.0320167,0.108065,-0.89784,0.113963,0.425317,0.295842,0.784375);
vpnt[352] = VPosNormalTex(-0.56875,-0.0420167,0.110745,-0.89784,0.113963,0.425317,0.307713,0.784375);
vpnt[353] = VPosNormalTex(-0.6125,-0.0890607,0.0309943,-0.90803,0.296211,0.296211,0.446698,0.80625);
vpnt[354] = VPosNormalTex(-0.6125,-0.0817402,0.0236737,-0.90803,0.296211,0.296211,0.455133,0.80625);
vpnt[355] = VPosNormalTex(-0.56875,-0.0246962,0.100745,-0.90803,0.296211,0.296211,0.28826,0.784375);
vpnt[356] = VPosNormalTex(-0.56875,-0.0320167,0.108065,-0.90803,0.296211,0.296211,0.295842,0.784375);
vpnt[357] = VPosNormalTex(-0.6125,-0.0817402,0.0236737,-0.863918,0.486471,0.130349,0.455133,0.80625);
vpnt[358] = VPosNormalTex(-0.6125,-0.0790607,0.0136737,-0.863918,0.486471,0.130349,0.472743,0.80625);
vpnt[359] = VPosNormalTex(-0.56875,-0.0220167,0.0907447,-0.863918,0.486471,0.130349,0.287882,0.784375);
vpnt[360] = VPosNormalTex(-0.56875,-0.0246962,0.100745,-0.863918,0.486471,0.130349,0.28826,0.784375);
vpnt[361] = VPosNormalTex(-0.6125,-0.0790607,0.0136737,-0.626355,0.752976,-0.201759,0.472743,0.80625);
vpnt[362] = VPosNormalTex(-0.6125,-0.0817402,0.00367375,-0.626355,0.752976,-0.201759,0.492852,0.80625);
vpnt[363] = VPosNormalTex(-0.56875,-0.0246962,0.0807447,-0.626355,0.752976,-0.201759,0.29724,0.784375);
vpnt[364] = VPosNormalTex(-0.56875,-0.0220167,0.0907447,-0.626355,0.752976,-0.201759,0.287882,0.784375);
vpnt[365] = VPosNormalTex(-0.6125,-0.0817402,0.00367375,0.307952,0.672743,-0.672743,0.492852,0.80625);
vpnt[366] = VPosNormalTex(-0.6125,-0.0890607,-0.00364676,0.307952,0.672743,-0.672743,-0.493487,0.80625);
vpnt[367] = VPosNormalTex(-0.56875,-0.0320167,0.0734242,0.307952,0.672743,-0.672743,0.315443,0.784375);
vpnt[368] = VPosNormalTex(-0.56875,-0.0246962,0.0807447,0.307952,0.672743,-0.672743,0.29724,0.784375);
vpnt[369] = VPosNormalTex(-0.6125,-0.0890607,-0.00364676,0.806507,0.15302,-0.571079,-0.493487,0.80625);
vpnt[370] = VPosNormalTex(-0.6125,-0.0990607,-0.00632625,0.806507,0.15302,-0.571079,-0.48985,0.80625);
vpnt[371] = VPosNormalTex(-0.56875,-0.0420167,0.0707447,0.806507,0.15302,-0.571079,0.335297,0.784375);
vpnt[372] = VPosNormalTex(-0.56875,-0.0320167,0.0734242,0.806507,0.15302,-0.571079,0.315443,0.784375);
vpnt[373] = VPosNormalTex(-0.6125,-0.0990607,-0.00632625,0.89784,-0.113963,-0.425317,-0.48985,0.80625);
vpnt[374] = VPosNormalTex(-0.6125,-0.109061,-0.00364676,0.89784,-0.113963,-0.425317,-0.49468,0.80625);
vpnt[375] = VPosNormalTex(-0.56875,-0.0520167,0.0734242,0.89784,-0.113963,-0.425317,0.348098,0.784375);
vpnt[376] = VPosNormalTex(-0.56875,-0.0420167,0.0707447,0.89784,-0.113963,-0.425317,0.335297,0.784375);
vpnt[377] = VPosNormalTex(-0.6125,-0.109061,-0.00364676,0.90803,-0.296211,-0.296211,-0.49468,0.80625);
vpnt[378] = VPosNormalTex(-0.6125,-0.116381,0.00367375,0.90803,-0.296211,-0.296211,0.494978,0.80625);
vpnt[379] = VPosNormalTex(-0.56875,-0.0593372,0.0807447,0.90803,-0.296211,-0.296211,0.350864,0.784375);
vpnt[380] = VPosNormalTex(-0.56875,-0.0520167,0.0734242,0.90803,-0.296211,-0.296211,0.348098,0.784375);
vpnt[381] = VPosNormalTex(-0.6125,-0.116381,0.00367375,0.863918,-0.486471,-0.13035,0.494978,0.80625);
vpnt[382] = VPosNormalTex(-0.6125,-0.119061,0.0136737,0.863918,-0.486471,-0.13035,0.481801,0.80625);
vpnt[383] = VPosNormalTex(-0.56875,-0.0620167,0.0907447,0.863918,-0.486471,-0.13035,0.345415,0.784375);
vpnt[384] = VPosNormalTex(-0.56875,-0.0593372,0.0807447,0.863918,-0.486471,-0.13035,0.350864,0.784375);
vpnt[385] = VPosNormalTex(-0.6125,-0.119061,0.0136737,0.626355,-0.752976,0.201759,0.481801,0.80625);
vpnt[386] = VPosNormalTex(-0.6125,-0.116381,0.0236737,0.626355,-0.752976,0.201759,0.468061,0.80625);
vpnt[387] = VPosNormalTex(-0.56875,-0.0593372,0.100745,0.626355,-0.752976,0.201759,0.334715,0.784375);
vpnt[388] = VPosNormalTex(-0.56875,-0.0620167,0.0907447,0.626355,-0.752976,0.201759,0.345415,0.784375);
vpnt[389] = VPosNormalTex(-0.6125,-0.116381,0.0236737,-0.307953,-0.672743,0.672743,0.468061,0.80625);
vpnt[390] = VPosNormalTex(-0.6125,-0.109061,0.0309943,-0.307953,-0.672743,0.672743,0.455931,0.80625);
vpnt[391] = VPosNormalTex(-0.56875,-0.0520167,0.108065,-0.307953,-0.672743,0.672743,0.321399,0.784375);
vpnt[392] = VPosNormalTex(-0.56875,-0.0593372,0.100745,-0.307953,-0.672743,0.672743,0.334715,0.784375);
vpnt[393] = VPosNormalTex(-0.6125,-0.109061,0.0309943,-0.806507,-0.15302,0.571079,0.455931,0.80625);
vpnt[394] = VPosNormalTex(-0.6125,-0.0990607,0.0336737,-0.806507,-0.15302,0.571079,0.447849,0.80625);
vpnt[395] = VPosNormalTex(-0.56875,-0.0420167,0.110745,-0.806507,-0.15302,0.571079,0.307713,0.784375);
vpnt[396] = VPosNormalTex(-0.56875,-0.0520167,0.108065,-0.806507,-0.15302,0.571079,0.321399,0.784375);
vpnt[397] = VPosNormalTex(-0.56875,-0.0420167,0.110745,-0.391603,0.238149,0.888781,0.307713,0.784375);
vpnt[398] = VPosNormalTex(-0.56875,-0.0320167,0.108065,-0.391603,0.238149,0.888781,0.295842,0.784375);
vpnt[399] = VPosNormalTex(-0.525,0.0636573,0.101706,-0.391603,0.238149,0.888781,0.160994,0.7625);
vpnt[400] = VPosNormalTex(-0.525,0.0536573,0.104385,-0.391603,0.238149,0.888781,0.174432,0.7625);
vpnt[401] = VPosNormalTex(-0.56875,-0.0320167,0.108065,-0.822026,0.402662,0.402662,0.295842,0.784375);
vpnt[402] = VPosNormalTex(-0.56875,-0.0246962,0.100745,-0.822026,0.402662,0.402662,0.28826,0.784375);
vpnt[403] = VPosNormalTex(-0.525,0.0709778,0.0943854,-0.822026,0.402662,0.402662,0.14738,0.7625);
vpnt[404] = VPosNormalTex(-0.525,0.0636573,0.101706,-0.822026,0.402662,0.402662,0.160994,0.7625);
vpnt[405] = VPosNormalTex(-0.56875,-0.0246962,0.100745,-0.90082,0.419398,0.112377,0.28826,0.784375);
vpnt[406] = VPosNormalTex(-0.56875,-0.0220167,0.0907447,-0.90082,0.419398,0.112377,0.287882,0.784375);
vpnt[407] = VPosNormalTex(-0.525,0.0736573,0.0843854,-0.90082,0.419398,0.112377,0.135787,0.7625);
vpnt[408] = VPosNormalTex(-0.525,0.0709778,0.0943854,-0.90082,0.419398,0.112377,0.14738,0.7625);
vpnt[409] = VPosNormalTex(-0.56875,-0.0220167,0.0907447,-0.906717,0.40737,-0.109154,0.287882,0.784375);
vpnt[410] = VPosNormalTex(-0.56875,-0.0246962,0.0807447,-0.906717,0.40737,-0.109154,0.29724,0.784375);
vpnt[411] = VPosNormalTex(-0.525,0.0709778,0.0743854,-0.906717,0.40737,-0.109154,0.12873,0.7625);
vpnt[412] = VPosNormalTex(-0.525,0.0736573,0.0843854,-0.906717,0.40737,-0.109154,0.135787,0.7625);
vpnt[413] = VPosNormalTex(-0.56875,-0.0246962,0.0807447,-0.855074,0.36664,-0.36664,0.29724,0.784375);
vpnt[414] = VPosNormalTex(-0.56875,-0.0320167,0.0734242,-0.855074,0.36664,-0.36664,0.315443,0.784375);
vpnt[415] = VPosNormalTex(-0.525,0.0636573,0.0670649,-0.855074,0.36664,-0.36664,0.129148,0.7625);
vpnt[416] = VPosNormalTex(-0.525,0.0709778,0.0743854,-0.855074,0.36664,-0.36664,0.12873,0.7625);
vpnt[417] = VPosNormalTex(-0.56875,-0.0320167,0.0734242,-0.576964,0.211396,-0.788939,0.315443,0.784375);
vpnt[418] = VPosNormalTex(-0.56875,-0.0420167,0.0707447,-0.576964,0.211396,-0.788939,0.335297,0.784375);
vpnt[419] = VPosNormalTex(-0.525,0.0536573,0.0643854,-0.576964,0.211396,-0.788939,0.139425,0.7625);
vpnt[420] = VPosNormalTex(-0.525,0.0636573,0.0670649,-0.576964,0.211396,-0.788939,0.129148,0.7625);
vpnt[421] = VPosNormalTex(-0.56875,-0.0420167,0.0707447,0.391603,-0.238149,-0.888781,0.335297,0.784375);
vpnt[422] = VPosNormalTex(-0.56875,-0.0520167,0.0734242,0.391603,-0.238149,-0.888781,0.348098,0.784375);
vpnt[423] = VPosNormalTex(-0.525,0.0436573,0.0670649,0.391603,-0.238149,-0.888781,0.158159,0.7625);
vpnt[424] = VPosNormalTex(-0.525,0.0536573,0.0643854,0.391603,-0.238149,-0.888781,0.139425,0.7625);
vpnt[425] = VPosNormalTex(-0.56875,-0.0520167,0.0734242,0.822026,-0.402662,-0.402662,0.348098,0.784375);
vpnt[426] = VPosNormalTex(-0.56875,-0.0593372,0.0807447,0.822026,-0.402662,-0.402662,0.350864,0.784375);
vpnt[427] = VPosNormalTex(-0.525,0.0363367,0.0743854,0.822026,-0.402662,-0.402662,0.17768,0.7625);
vpnt[428] = VPosNormalTex(-0.525,0.0436573,0.0670649,0.822026,-0.402662,-0.402662,0.158159,0.7625);
vpnt[429] = VPosNormalTex(-0.56875,-0.0593372,0.0807447,0.90082,-0.419398,-0.112377,0.350864,0.784375);
vpnt[430] = VPosNormalTex(-0.56875,-0.0620167,0.0907447,0.90082,-0.419398,-0.112377,0.345415,0.784375);
vpnt[431] = VPosNormalTex(-0.525,0.0336573,0.0843854,0.90082,-0.419398,-0.112377,0.189598,0.7625);
vpnt[432] = VPosNormalTex(-0.525,0.0363367,0.0743854,0.90082,-0.419398,-0.112377,0.17768,0.7625);
vpnt[433] = VPosNormalTex(-0.56875,-0.0620167,0.0907447,0.906717,-0.40737,0.109154,0.345415,0.784375);
vpnt[434] = VPosNormalTex(-0.56875,-0.0593372,0.100745,0.906717,-0.40737,0.109154,0.334715,0.784375);
vpnt[435] = VPosNormalTex(-0.525,0.0363367,0.0943854,0.906717,-0.40737,0.109154,0.191512,0.7625);
vpnt[436] = VPosNormalTex(-0.525,0.0336573,0.0843854,0.906717,-0.40737,0.109154,0.189598,0.7625);
vpnt[437] = VPosNormalTex(-0.56875,-0.0593372,0.100745,0.855074,-0.36664,0.36664,0.334715,0.784375);
vpnt[438] = VPosNormalTex(-0.56875,-0.0520167,0.108065,0.855074,-0.36664,0.36664,0.321399,0.784375);
vpnt[439] = VPosNormalTex(-0.525,0.0436573,0.101706,0.855074,-0.36664,0.36664,0.185468,0.7625);
vpnt[440] = VPosNormalTex(-0.525,0.0363367,0.0943854,0.855074,-0.36664,0.36664,0.191512,0.7625);
vpnt[441] = VPosNormalTex(-0.56875,-0.0520167,0.108065,0.576964,-0.211396,0.788939,0.321399,0.784375);
vpnt[442] = VPosNormalTex(-0.56875,-0.0420167,0.110745,0.576964,-0.211396,0.788939,0.307713,0.784375);
vpnt[443] = VPosNormalTex(-0.525,0.0536573,0.104385,0.576964,-0.211396,0.788939,0.174432,0.7625);
vpnt[444] = VPosNormalTex(-0.525,0.0436573,0.101706,0.576964,-0.211396,0.788939,0.185468,0.7625);
vpnt[445] = VPosNormalTex(-0.525,0.0536573,0.104385,0.844852,0.138468,0.51677,0.174432,0.7625);
vpnt[446] = VPosNormalTex(-0.525,0.0636573,0.101706,0.844852,0.138468,0.51677,0.160994,0.7625);
vpnt[447] = VPosNormalTex(-0.48125,0.109999,0.0177631,0.844852,0.138468,0.51677,0.025481,0.740625);
vpnt[448] = VPosNormalTex(-0.48125,0.099999,0.0204426,0.844852,0.138468,0.51677,0.0320935,0.740625);
vpnt[449] = VPosNormalTex(-0.525,0.0636573,0.101706,0.519341,0.60427,0.60427,0.160994,0.7625);
vpnt[450] = VPosNormalTex(-0.525,0.0709778,0.0943854,0.519341,0.60427,0.60427,0.14738,0.7625);
vpnt[451] = VPosNormalTex(-0.48125,0.11732,0.0104426,0.519341,0.60427,0.60427,0.0141291,0.740625);
vpnt[452] = VPosNormalTex(-0.48125,0.109999,0.0177631,0.519341,0.60427,0.60427,0.025481,0.740625);
vpnt[453] = VPosNormalTex(-0.525,0.0709778,0.0943854,-0.465911,0.854682,0.229011,0.14738,0.7625);
vpnt[454] = VPosNormalTex(-0.525,0.0736573,0.0843854,-0.465911,0.854682,0.229011,0.135787,0.7625);
vpnt[455] = VPosNormalTex(-0.48125,0.119999,0.000442594,-0.465911,0.854682,0.229011,0.00058701,0.740625);
vpnt[456] = VPosNormalTex(-0.48125,0.11732,0.0104426,-0.465911,0.854682,0.229011,0.0141291,0.740625);
vpnt[457] = VPosNormalTex(-0.525,0.0736573,0.0843854,-0.835374,0.530952,-0.142268,0.135787,0.7625);
vpnt[458] = VPosNormalTex(-0.525,0.0709778,0.0743854,-0.835374,0.530952,-0.142268,0.12873,0.7625);
vpnt[459] = VPosNormalTex(-0.48125,0.11732,-0.00955741,-0.835374,0.530952,-0.142268,-0.0129369,0.740625);
vpnt[460] = VPosNormalTex(-0.48125,0.119999,0.000442594,-0.835374,0.530952,-0.142268,0.00058701,0.740625);
vpnt[461] = VPosNormalTex(-0.525,0.0709778,0.0743854,-0.903313,0.303336,-0.303336,0.12873,0.7625);
vpnt[462] = VPosNormalTex(-0.525,0.0636573,0.0670649,-0.903313,0.303336,-0.303336,0.129148,0.7625);
vpnt[463] = VPosNormalTex(-0.48125,0.109999,-0.0168779,-0.903313,0.303336,-0.303336,-0.0242313,0.740625);
vpnt[464] = VPosNormalTex(-0.48125,0.11732,-0.00955741,-0.903313,0.303336,-0.303336,-0.0129369,0.740625);
vpnt[465] = VPosNormalTex(-0.525,0.0636573,0.0670649,-0.905009,0.1101,-0.410898,0.129148,0.7625);
vpnt[466] = VPosNormalTex(-0.525,0.0536573,0.0643854,-0.905009,0.1101,-0.410898,0.139425,0.7625);
vpnt[467] = VPosNormalTex(-0.48125,0.099999,-0.0195574,-0.905009,0.1101,-0.410898,-0.0307389,0.740625);
vpnt[468] = VPosNormalTex(-0.48125,0.109999,-0.0168779,-0.905009,0.1101,-0.410898,-0.0242313,0.740625);
vpnt[469] = VPosNormalTex(-0.525,0.0536573,0.0643854,-0.844852,-0.138468,-0.51677,0.139425,0.7625);
vpnt[470] = VPosNormalTex(-0.525,0.0436573,0.0670649,-0.844852,-0.138468,-0.51677,0.158159,0.7625);
vpnt[471] = VPosNormalTex(-0.48125,0.089999,-0.0168779,-0.844852,-0.138468,-0.51677,-0.0295043,0.740625);
vpnt[472] = VPosNormalTex(-0.48125,0.099999,-0.0195574,-0.844852,-0.138468,-0.51677,-0.0307389,0.740625);
vpnt[473] = VPosNormalTex(-0.525,0.0436573,0.0670649,-0.519341,-0.60427,-0.60427,0.158159,0.7625);
vpnt[474] = VPosNormalTex(-0.525,0.0363367,0.0743854,-0.519341,-0.60427,-0.60427,0.17768,0.7625);
vpnt[475] = VPosNormalTex(-0.48125,0.0826785,-0.00955741,-0.519341,-0.60427,-0.60427,-0.0183166,0.740625);
vpnt[476] = VPosNormalTex(-0.48125,0.089999,-0.0168779,-0.519341,-0.60427,-0.60427,-0.0295043,0.740625);
vpnt[477] = VPosNormalTex(-0.525,0.0363367,0.0743854,0.465911,-0.854682,-0.229011,0.17768,0.7625);
vpnt[478] = VPosNormalTex(-0.525,0.0336573,0.0843854,0.465911,-0.854682,-0.229011,0.189598,0.7625);
vpnt[479] = VPosNormalTex(-0.48125,0.079999,0.000442594,0.465911,-0.854682,-0.229011,0.000880514,0.740625);
vpnt[480] = VPosNormalTex(-0.48125,0.0826785,-0.00955741,0.465911,-0.854682,-0.229011,-0.0183166,0.740625);
vpnt[481] = VPosNormalTex(-0.525,0.0336573,0.0843854,0.835374,-0.530952,0.142268,0.189598,0.7625);
vpnt[482] = VPosNormalTex(-0.525,0.0363367,0.0943854,0.835374,-0.530952,0.142268,0.191512,0.7625);
vpnt[483] = VPosNormalTex(-0.48125,0.0826785,0.0104426,0.835374,-0.530952,0.142268,0.019996,0.740625);
vpnt[484] = VPosNormalTex(-0.48125,0.079999,0.000442594,0.835374,-0.530952,0.142268,0.000880514,0.740625);
vpnt[485] = VPosNormalTex(-0.525,0.0363367,0.0943854,0.903313,-0.303336,0.303336,0.191512,0.7625);
vpnt[486] = VPosNormalTex(-0.525,0.0436573,0.101706,0.903313,-0.303336,0.303336,0.185468,0.7625);
vpnt[487] = VPosNormalTex(-0.48125,0.089999,0.0177631,0.903313,-0.303336,0.303336,0.0310138,0.740625);
vpnt[488] = VPosNormalTex(-0.48125,0.0826785,0.0104426,0.903313,-0.303336,0.303336,0.019996,0.740625);
vpnt[489] = VPosNormalTex(-0.525,0.0436573,0.101706,0.905009,-0.1101,0.410898,0.185468,0.7625);
vpnt[490] = VPosNormalTex(-0.525,0.0536573,0.104385,0.905009,-0.1101,0.410898,0.174432,0.7625);
vpnt[491] = VPosNormalTex(-0.48125,0.099999,0.0204426,0.905009,-0.1101,0.410898,0.0320935,0.740625);
vpnt[492] = VPosNormalTex(-0.48125,0.089999,0.0177631,0.905009,-0.1101,0.410898,0.0310138,0.740625);
vpnt[493] = VPosNormalTex(-0.48125,0.099999,0.0204426,0.90536,0.109906,0.410175,0.0320935,0.740625);
vpnt[494] = VPosNormalTex(-0.48125,0.109999,0.0177631,0.90536,0.109906,0.410175,0.025481,0.740625);
vpnt[495] = VPosNormalTex(-0.4375,0.0644021,-0.0665866,0.90536,0.109906,0.410175,-0.127654,0.71875);
vpnt[496] = VPosNormalTex(-0.4375,0.0544021,-0.0639071,0.90536,0.109906,0.410175,-0.137759,0.71875);
vpnt[497] = VPosNormalTex(-0.48125,0.109999,0.0177631,0.902881,0.303979,0.303979,0.025481,0.740625);
vpnt[498] = VPosNormalTex(-0.48125,0.11732,0.0104426,0.902881,0.303979,0.303979,0.0141291,0.740625);
vpnt[499] = VPosNormalTex(-0.4375,0.0717226,-0.0739072,0.902881,0.303979,0.303979,-0.127387,0.71875);
vpnt[500] = VPosNormalTex(-0.4375,0.0644021,-0.0665866,0.902881,0.303979,0.303979,-0.127654,0.71875);
vpnt[501] = VPosNormalTex(-0.48125,0.11732,0.0104426,0.83302,0.534391,0.14319,0.0141291,0.740625);
vpnt[502] = VPosNormalTex(-0.48125,0.119999,0.000442594,0.83302,0.534391,0.14319,0.00058701,0.740625);
vpnt[503] = VPosNormalTex(-0.4375,0.0744021,-0.0839071,0.83302,0.534391,0.14319,-0.134544,0.71875);
vpnt[504] = VPosNormalTex(-0.4375,0.0717226,-0.0739072,0.83302,0.534391,0.14319,-0.127387,0.71875);
vpnt[505] = VPosNormalTex(-0.48125,0.119999,0.000442594,0.452698,0.861281,-0.23078,0.00058701,0.740625);
vpnt[506] = VPosNormalTex(-0.48125,0.11732,-0.00955741,0.452698,0.861281,-0.23078,-0.0129369,0.740625);
vpnt[507] = VPosNormalTex(-0.4375,0.0717226,-0.0939071,0.452698,0.861281,-0.23078,-0.146191,0.71875);
vpnt[508] = VPosNormalTex(-0.4375,0.0744021,-0.0839071,0.452698,0.861281,-0.23078,-0.134544,0.71875);
vpnt[509] = VPosNormalTex(-0.48125,0.11732,-0.00955741,-0.530816,0.599264,-0.599264,-0.0129369,0.740625);
vpnt[510] = VPosNormalTex(-0.48125,0.109999,-0.0168779,-0.530816,0.599264,-0.599264,-0.0242313,0.740625);
vpnt[511] = VPosNormalTex(-0.4375,0.0644021,-0.101228,-0.530816,0.599264,-0.599264,-0.15982,0.71875);
vpnt[512] = VPosNormalTex(-0.4375,0.0717226,-0.0939071,-0.530816,0.599264,-0.599264,-0.146191,0.71875);
vpnt[513] = VPosNormalTex(-0.48125,0.109999,-0.0168779,-0.846884,0.137634,-0.513658,-0.0242313,0.740625);
vpnt[514] = VPosNormalTex(-0.48125,0.099999,-0.0195574,-0.846884,0.137634,-0.513658,-0.0307389,0.740625);
vpnt[515] = VPosNormalTex(-0.4375,0.0544021,-0.103907,-0.846884,0.137634,-0.513658,-0.173236,0.71875);
vpnt[516] = VPosNormalTex(-0.4375,0.0644021,-0.101228,-0.846884,0.137634,-0.513658,-0.15982,0.71875);
vpnt[517] = VPosNormalTex(-0.48125,0.099999,-0.0195574,-0.90536,-0.109906,-0.410175,-0.0307389,0.740625);
vpnt[518] = VPosNormalTex(-0.48125,0.089999,-0.0168779,-0.90536,-0.109906,-0.410175,-0.0295043,0.740625);
vpnt[519] = VPosNormalTex(-0.4375,0.0444021,-0.101228,-0.90536,-0.109906,-0.410175,-0.184211,0.71875);
vpnt[520] = VPosNormalTex(-0.4375,0.0544021,-0.103907,-0.90536,-0.109906,-0.410175,-0.173236,0.71875);
vpnt[521] = VPosNormalTex(-0.48125,0.089999,-0.0168779,-0.902881,-0.303979,-0.303979,-0.0295043,0.740625);
vpnt[522] = VPosNormalTex(-0.48125,0.0826785,-0.00955741,-0.902881,-0.303979,-0.303979,-0.0183166,0.740625);
vpnt[523] = VPosNormalTex(-0.4375,0.0370816,-0.0939071,-0.902881,-0.303979,-0.303979,-0.190145,0.71875);
vpnt[524] = VPosNormalTex(-0.4375,0.0444021,-0.101228,-0.902881,-0.303979,-0.303979,-0.184211,0.71875);
vpnt[525] = VPosNormalTex(-0.48125,0.0826785,-0.00955741,-0.83302,-0.534391,-0.14319,-0.0183166,0.740625);
vpnt[526] = VPosNormalTex(-0.48125,0.079999,0.000442594,-0.83302,-0.534391,-0.14319,0.000880514,0.740625);
vpnt[527] = VPosNormalTex(-0.4375,0.0344021,-0.0839071,-0.83302,-0.534391,-0.14319,-0.188073,0.71875);
vpnt[528] = VPosNormalTex(-0.4375,0.0370816,-0.0939071,-0.83302,-0.534391,-0.14319,-0.190145,0.71875);
vpnt[529] = VPosNormalTex(-0.48125,0.079999,0.000442594,-0.452697,-0.861281,0.23078,0.000880514,0.740625);
vpnt[530] = VPosNormalTex(-0.48125,0.0826785,0.0104426,-0.452697,-0.861281,0.23078,0.019996,0.740625);
vpnt[531] = VPosNormalTex(-0.4375,0.0370816,-0.0739072,-0.452697,-0.861281,0.23078,-0.175988,0.71875);
vpnt[532] = VPosNormalTex(-0.4375,0.0344021,-0.0839071,-0.452697,-0.861281,0.23078,-0.188073,0.71875);
vpnt[533] = VPosNormalTex(-0.48125,0.0826785,0.0104426,0.530815,-0.599264,0.599264,0.019996,0.740625);
vpnt[534] = VPosNormalTex(-0.48125,0.089999,0.0177631,0.530815,-0.599264,0.599264,0.0310138,0.740625);
vpnt[535] = VPosNormalTex(-0.4375,0.0444021,-0.0665866,0.530815,-0.599264,0.599264,-0.156398,0.71875);
vpnt[536] = VPosNormalTex(-0.4375,0.0370816,-0.0739072,0.530815,-0.599264,0.599264,-0.175988,0.71875);
vpnt[537] = VPosNormalTex(-0.48125,0.089999,0.0177631,0.846884,-0.137634,0.513658,0.0310138,0.740625);
vpnt[538] = VPosNormalTex(-0.48125,0.099999,0.0204426,0.846884,-0.137634,0.513658,0.0320935,0.740625);
vpnt[539] = VPosNormalTex(-0.4375,0.0544021,-0.0639071,0.846884,-0.137634,0.513658,-0.137759,0.71875);
vpnt[540] = VPosNormalTex(-0.4375,0.0444021,-0.0665866,0.846884,-0.137634,0.513658,-0.156398,0.71875);
vpnt[541] = VPosNormalTex(-0.4375,0.0544021,-0.0639071,0.586826,0.209569,0.782123,-0.137759,0.71875);
vpnt[542] = VPosNormalTex(-0.4375,0.0644021,-0.0665866,0.586826,0.209569,0.782123,-0.127654,0.71875);
vpnt[543] = VPosNormalTex(-0.39375,-0.0312119,-0.0737925,0.586826,0.209569,0.782123,-0.313686,0.696875);
vpnt[544] = VPosNormalTex(-0.39375,-0.0412119,-0.071113,0.586826,0.209569,0.782123,-0.333593,0.696875);
vpnt[545] = VPosNormalTex(-0.4375,0.0644021,-0.0665866,0.856831,0.364583,0.364583,-0.127654,0.71875);
vpnt[546] = VPosNormalTex(-0.4375,0.0717226,-0.0739072,0.856831,0.364583,0.364583,-0.127387,0.71875);
vpnt[547] = VPosNormalTex(-0.39375,-0.0238914,-0.081113,0.856831,0.364583,0.364583,-0.295589,0.696875);
vpnt[548] = VPosNormalTex(-0.39375,-0.0312119,-0.0737925,0.856831,0.364583,0.364583,-0.313686,0.696875);
vpnt[549] = VPosNormalTex(-0.4375,0.0717226,-0.0739072,0.906992,0.406797,0.109001,-0.127387,0.71875);
vpnt[550] = VPosNormalTex(-0.4375,0.0744021,-0.0839071,0.906992,0.406797,0.109001,-0.134544,0.71875);
vpnt[551] = VPosNormalTex(-0.39375,-0.0212119,-0.091113,0.906992,0.406797,0.109001,-0.286404,0.696875);
vpnt[552] = VPosNormalTex(-0.39375,-0.0238914,-0.081113,0.906992,0.406797,0.109001,-0.295589,0.696875);
vpnt[553] = VPosNormalTex(-0.4375,0.0744021,-0.0839071,0.900299,0.42044,-0.112656,-0.134544,0.71875);
vpnt[554] = VPosNormalTex(-0.4375,0.0717226,-0.0939071,0.900299,0.42044,-0.112656,-0.146191,0.71875);
vpnt[555] = VPosNormalTex(-0.39375,-0.0238914,-0.101113,0.900299,0.42044,-0.112656,-0.286928,0.696875);
vpnt[556] = VPosNormalTex(-0.39375,-0.0212119,-0.091113,0.900299,0.42044,-0.112656,-0.286404,0.696875);
vpnt[557] = VPosNormalTex(-0.4375,0.0717226,-0.0939071,0.819292,0.405438,-0.405438,-0.146191,0.71875);
vpnt[558] = VPosNormalTex(-0.4375,0.0644021,-0.101228,0.819292,0.405438,-0.405438,-0.15982,0.71875);
vpnt[559] = VPosNormalTex(-0.39375,-0.0312119,-0.108434,0.819292,0.405438,-0.405438,-0.294606,0.696875);
vpnt[560] = VPosNormalTex(-0.39375,-0.0238914,-0.101113,0.819292,0.405438,-0.405438,-0.286928,0.696875);
vpnt[561] = VPosNormalTex(-0.4375,0.0644021,-0.101228,0.376613,0.239763,-0.894805,-0.15982,0.71875);
vpnt[562] = VPosNormalTex(-0.4375,0.0544021,-0.103907,0.376613,0.239763,-0.894805,-0.173236,0.71875);
vpnt[563] = VPosNormalTex(-0.39375,-0.0412119,-0.111113,0.376613,0.239763,-0.894805,-0.306527,0.696875);
vpnt[564] = VPosNormalTex(-0.39375,-0.0312119,-0.108434,0.376613,0.239763,-0.894805,-0.294606,0.696875);
vpnt[565] = VPosNormalTex(-0.4375,0.0544021,-0.103907,-0.586826,-0.20957,-0.782123,-0.173236,0.71875);
vpnt[566] = VPosNormalTex(-0.4375,0.0444021,-0.101228,-0.586826,-0.20957,-0.782123,-0.184211,0.71875);
vpnt[567] = VPosNormalTex(-0.39375,-0.0512119,-0.108434,-0.586826,-0.20957,-0.782123,-0.320224,0.696875);
vpnt[568] = VPosNormalTex(-0.39375,-0.0412119,-0.111113,-0.586826,-0.20957,-0.782123,-0.306527,0.696875);
vpnt[569] = VPosNormalTex(-0.4375,0.0444021,-0.101228,-0.856831,-0.364583,-0.364583,-0.184211,0.71875);
vpnt[570] = VPosNormalTex(-0.4375,0.0370816,-0.0939071,-0.856831,-0.364583,-0.364583,-0.190145,0.71875);
vpnt[571] = VPosNormalTex(-0.39375,-0.0585324,-0.101113,-0.856831,-0.364583,-0.364583,-0.333516,0.696875);
vpnt[572] = VPosNormalTex(-0.39375,-0.0512119,-0.108434,-0.856831,-0.364583,-0.364583,-0.320224,0.696875);
vpnt[573] = VPosNormalTex(-0.4375,0.0370816,-0.0939071,-0.906992,-0.406797,-0.109001,-0.190145,0.71875);
vpnt[574] = VPosNormalTex(-0.4375,0.0344021,-0.0839071,-0.906992,-0.406797,-0.109001,-0.188073,0.71875);
vpnt[575] = VPosNormalTex(-0.39375,-0.0612119,-0.091113,-0.906992,-0.406797,-0.109001,-0.34415,0.696875);
vpnt[576] = VPosNormalTex(-0.39375,-0.0585324,-0.101113,-0.906992,-0.406797,-0.109001,-0.333516,0.696875);
vpnt[577] = VPosNormalTex(-0.4375,0.0344021,-0.0839071,-0.900299,-0.42044,0.112656,-0.188073,0.71875);
vpnt[578] = VPosNormalTex(-0.4375,0.0370816,-0.0739072,-0.900299,-0.42044,0.112656,-0.175988,0.71875);
vpnt[579] = VPosNormalTex(-0.39375,-0.0585324,-0.081113,-0.900299,-0.42044,0.112656,-0.349486,0.696875);
vpnt[580] = VPosNormalTex(-0.39375,-0.0612119,-0.091113,-0.900299,-0.42044,0.112656,-0.34415,0.696875);
vpnt[581] = VPosNormalTex(-0.4375,0.0370816,-0.0739072,-0.819292,-0.405438,0.405438,-0.175988,0.71875);
vpnt[582] = VPosNormalTex(-0.4375,0.0444021,-0.0665866,-0.819292,-0.405438,0.405438,-0.156398,0.71875);
vpnt[583] = VPosNormalTex(-0.39375,-0.0512119,-0.0737925,-0.819292,-0.405438,0.405438,-0.346557,0.696875);
vpnt[584] = VPosNormalTex(-0.39375,-0.0585324,-0.081113,-0.819292,-0.405438,0.405438,-0.349486,0.696875);
vpnt[585] = VPosNormalTex(-0.4375,0.0444021,-0.0665866,-0.376613,-0.239763,0.894805,-0.156398,0.71875);
vpnt[586] = VPosNormalTex(-0.4375,0.0544021,-0.0639071,-0.376613,-0.239763,0.894805,-0.137759,0.71875);
vpnt[587] = VPosNormalTex(-0.39375,-0.0412119,-0.071113,-0.376613,-0.239763,0.894805,-0.333593,0.696875);
vpnt[588] = VPosNormalTex(-0.39375,-0.0512119,-0.0737925,-0.376613,-0.239763,0.894805,-0.346557,0.696875);
vpnt[589] = VPosNormalTex(-0.39375,-0.0412119,-0.071113,-0.803324,0.154138,0.57525,-0.333593,0.696875);
vpnt[590] = VPosNormalTex(-0.39375,-0.0312119,-0.0737925,-0.803324,0.154138,0.57525,-0.313686,0.696875);
vpnt[591] = VPosNormalTex(-0.35,-0.0889358,0.00277049,-0.803324,0.154138,0.57525,0.495044,0.675);
vpnt[592] = VPosNormalTex(-0.35,-0.0989358,0.00544998,-0.803324,0.154138,0.57525,0.491242,0.675);
vpnt[593] = VPosNormalTex(-0.39375,-0.0312119,-0.0737925,-0.291282,0.676445,0.676445,-0.313686,0.696875);
vpnt[594] = VPosNormalTex(-0.39375,-0.0238914,-0.081113,-0.291282,0.676445,0.676445,-0.295589,0.696875);
vpnt[595] = VPosNormalTex(-0.35,-0.0816153,-0.00455002,-0.291282,0.676445,0.676445,-0.491136,0.675);
vpnt[596] = VPosNormalTex(-0.35,-0.0889358,0.00277049,-0.291282,0.676445,0.676445,0.495044,0.675);
vpnt[597] = VPosNormalTex(-0.39375,-0.0238914,-0.081113,0.634777,0.746366,0.199988,-0.295589,0.696875);
vpnt[598] = VPosNormalTex(-0.39375,-0.0212119,-0.091113,0.634777,0.746366,0.199988,-0.286404,0.696875);
vpnt[599] = VPosNormalTex(-0.35,-0.0789358,-0.01455,0.634777,0.746366,0.199988,-0.470989,0.675);
vpnt[600] = VPosNormalTex(-0.35,-0.0816153,-0.00455002,0.634777,0.746366,0.199988,-0.491136,0.675);
vpnt[601] = VPosNormalTex(-0.39375,-0.0212119,-0.091113,0.86544,0.483941,-0.129671,-0.286404,0.696875);
vpnt[602] = VPosNormalTex(-0.39375,-0.0238914,-0.101113,0.86544,0.483941,-0.129671,-0.286928,0.696875);
vpnt[603] = VPosNormalTex(-0.35,-0.0816153,-0.02455,0.86544,0.483941,-0.129671,-0.453496,0.675);
vpnt[604] = VPosNormalTex(-0.35,-0.0789358,-0.01455,0.86544,0.483941,-0.129671,-0.470989,0.675);
vpnt[605] = VPosNormalTex(-0.39375,-0.0238914,-0.101113,0.908234,0.295898,-0.295898,-0.286928,0.696875);
vpnt[606] = VPosNormalTex(-0.39375,-0.0312119,-0.108434,0.908234,0.295898,-0.295898,-0.294606,0.696875);
vpnt[607] = VPosNormalTex(-0.35,-0.0889358,-0.0318705,0.908234,0.295898,-0.295898,-0.445235,0.675);
vpnt[608] = VPosNormalTex(-0.35,-0.0816153,-0.02455,0.908234,0.295898,-0.295898,-0.453496,0.675);
vpnt[609] = VPosNormalTex(-0.39375,-0.0312119,-0.108434,0.897224,0.114288,-0.426529,-0.294606,0.696875);
vpnt[610] = VPosNormalTex(-0.39375,-0.0412119,-0.111113,0.897224,0.114288,-0.426529,-0.306527,0.696875);
vpnt[611] = VPosNormalTex(-0.35,-0.0989358,-0.03455,0.897224,0.114288,-0.426529,-0.446528,0.675);
vpnt[612] = VPosNormalTex(-0.35,-0.0889358,-0.0318705,0.897224,0.114288,-0.426529,-0.445235,0.675);
vpnt[613] = VPosNormalTex(-0.39375,-0.0412119,-0.111113,0.803324,-0.154138,-0.57525,-0.306527,0.696875);
vpnt[614] = VPosNormalTex(-0.39375,-0.0512119,-0.108434,0.803324,-0.154138,-0.57525,-0.320224,0.696875);
vpnt[615] = VPosNormalTex(-0.35,-0.108936,-0.0318705,0.803324,-0.154138,-0.57525,-0.454701,0.675);
vpnt[616] = VPosNormalTex(-0.35,-0.0989358,-0.03455,0.803324,-0.154138,-0.57525,-0.446528,0.675);
vpnt[617] = VPosNormalTex(-0.39375,-0.0512119,-0.108434,0.291282,-0.676445,-0.676445,-0.320224,0.696875);
vpnt[618] = VPosNormalTex(-0.39375,-0.0585324,-0.101113,0.291282,-0.676445,-0.676445,-0.333516,0.696875);
vpnt[619] = VPosNormalTex(-0.35,-0.116256,-0.02455,0.291282,-0.676445,-0.676445,-0.466878,0.675);
vpnt[620] = VPosNormalTex(-0.35,-0.108936,-0.0318705,0.291282,-0.676445,-0.676445,-0.454701,0.675);
vpnt[621] = VPosNormalTex(-0.39375,-0.0585324,-0.101113,-0.634778,-0.746366,-0.199988,-0.333516,0.696875);
vpnt[622] = VPosNormalTex(-0.39375,-0.0612119,-0.091113,-0.634778,-0.746366,-0.199988,-0.34415,0.696875);
vpnt[623] = VPosNormalTex(-0.35,-0.118936,-0.01455,-0.634778,-0.746366,-0.199988,-0.480626,0.675);
vpnt[624] = VPosNormalTex(-0.35,-0.116256,-0.02455,-0.634778,-0.746366,-0.199988,-0.466878,0.675);
vpnt[625] = VPosNormalTex(-0.39375,-0.0612119,-0.091113,-0.86544,-0.483941,0.129671,-0.34415,0.696875);
vpnt[626] = VPosNormalTex(-0.39375,-0.0585324,-0.081113,-0.86544,-0.483941,0.129671,-0.349486,0.696875);
vpnt[627] = VPosNormalTex(-0.35,-0.116256,-0.00455002,-0.86544,-0.483941,0.129671,-0.493774,0.675);
vpnt[628] = VPosNormalTex(-0.35,-0.118936,-0.01455,-0.86544,-0.483941,0.129671,-0.480626,0.675);
vpnt[629] = VPosNormalTex(-0.39375,-0.0585324,-0.081113,-0.908234,-0.295898,0.295898,-0.349486,0.696875);
vpnt[630] = VPosNormalTex(-0.39375,-0.0512119,-0.0737925,-0.908234,-0.295898,0.295898,-0.346557,0.696875);
vpnt[631] = VPosNormalTex(-0.35,-0.108936,0.00277049,-0.908234,-0.295898,0.295898,0.495953,0.675);
vpnt[632] = VPosNormalTex(-0.35,-0.116256,-0.00455002,-0.908234,-0.295898,0.295898,-0.493774,0.675);
vpnt[633] = VPosNormalTex(-0.39375,-0.0512119,-0.0737925,-0.897224,-0.114288,0.426529,-0.346557,0.696875);
vpnt[634] = VPosNormalTex(-0.39375,-0.0412119,-0.071113,-0.897224,-0.114288,0.426529,-0.333593,0.696875);
vpnt[635] = VPosNormalTex(-0.35,-0.0989358,0.00544998,-0.897224,-0.114288,0.426529,0.491242,0.675);
vpnt[636] = VPosNormalTex(-0.35,-0.108936,0.00277049,-0.897224,-0.114288,0.426529,0.495953,0.675);
vpnt[637] = VPosNormalTex(-0.35,-0.0989358,0.00544998,-0.909104,0.107816,0.402376,0.491242,0.675);
vpnt[638] = VPosNormalTex(-0.35,-0.0889358,0.00277049,-0.909104,0.107816,0.402376,0.495044,0.675);
vpnt[639] = VPosNormalTex(-0.30625,-0.0556987,0.0927107,-0.909104,0.107816,0.402376,0.336102,0.653125);
vpnt[640] = VPosNormalTex(-0.30625,-0.0656987,0.0953902,-0.909104,0.107816,0.402376,0.34599,0.653125);
vpnt[641] = VPosNormalTex(-0.35,-0.0889358,0.00277049,-0.893604,0.317389,0.317389,0.495044,0.675);
vpnt[642] = VPosNormalTex(-0.35,-0.0816153,-0.00455002,-0.893604,0.317389,0.317389,-0.491136,0.675);
vpnt[643] = VPosNormalTex(-0.30625,-0.0483781,0.0853902,-0.893604,0.317389,0.317389,0.332039,0.653125);
vpnt[644] = VPosNormalTex(-0.30625,-0.0556987,0.0927107,-0.893604,0.317389,0.317389,0.336102,0.653125);
vpnt[645] = VPosNormalTex(-0.35,-0.0816153,-0.00455002,-0.784699,0.598755,0.160436,-0.491136,0.675);
vpnt[646] = VPosNormalTex(-0.35,-0.0789358,-0.01455,-0.784699,0.598755,0.160436,-0.470989,0.675);
vpnt[647] = VPosNormalTex(-0.30625,-0.0456987,0.0753902,-0.784699,0.598755,0.160436,0.33673,0.653125);
vpnt[648] = VPosNormalTex(-0.30625,-0.0483781,0.0853902,-0.784699,0.598755,0.160436,0.332039,0.653125);
vpnt[649] = VPosNormalTex(-0.35,-0.0789358,-0.01455,-0.197761,0.946849,-0.253708,-0.470989,0.675);
vpnt[650] = VPosNormalTex(-0.35,-0.0816153,-0.02455,-0.197761,0.946849,-0.253708,-0.453496,0.675);
vpnt[651] = VPosNormalTex(-0.30625,-0.0483781,0.0653902,-0.197761,0.946849,-0.253708,0.351376,0.653125);
vpnt[652] = VPosNormalTex(-0.30625,-0.0456987,0.0753902,-0.197761,0.946849,-0.253708,0.33673,0.653125);
vpnt[653] = VPosNormalTex(-0.35,-0.0816153,-0.02455,0.675642,0.5213,-0.5213,-0.453496,0.675);
vpnt[654] = VPosNormalTex(-0.35,-0.0889358,-0.0318705,0.675642,0.5213,-0.5213,-0.445235,0.675);
vpnt[655] = VPosNormalTex(-0.30625,-0.0556987,0.0580697,0.675642,0.5213,-0.5213,0.371683,0.653125);
vpnt[656] = VPosNormalTex(-0.30625,-0.0483781,0.0653902,0.675642,0.5213,-0.5213,0.351376,0.653125);
vpnt[657] = VPosNormalTex(-0.35,-0.0889358,-0.0318705,0.8729,0.126278,-0.471274,-0.445235,0.675);
vpnt[658] = VPosNormalTex(-0.35,-0.0989358,-0.03455,0.8729,0.126278,-0.471274,-0.446528,0.675);
vpnt[659] = VPosNormalTex(-0.30625,-0.0656987,0.0553902,0.8729,0.126278,-0.471274,0.388516,0.653125);
vpnt[660] = VPosNormalTex(-0.30625,-0.0556987,0.0580697,0.8729,0.126278,-0.471274,0.371683,0.653125);
vpnt[661] = VPosNormalTex(-0.35,-0.0989358,-0.03455,0.909104,-0.107816,-0.402376,-0.446528,0.675);
vpnt[662] = VPosNormalTex(-0.35,-0.108936,-0.0318705,0.909104,-0.107816,-0.402376,-0.454701,0.675);
vpnt[663] = VPosNormalTex(-0.30625,-0.0756987,0.0580697,0.909104,-0.107816,-0.402376,0.395854,0.653125);
vpnt[664] = VPosNormalTex(-0.30625,-0.0656987,0.0553902,0.909104,-0.107816,-0.402376,0.388516,0.653125);
vpnt[665] = VPosNormalTex(-0.35,-0.108936,-0.0318705,0.893604,-0.317389,-0.317389,-0.454701,0.675);
vpnt[666] = VPosNormalTex(-0.35,-0.116256,-0.02455,0.893604,-0.317389,-0.317389,-0.466878,0.675);
vpnt[667] = VPosNormalTex(-0.30625,-0.0830192,0.0653902,0.893604,-0.317389,-0.317389,0.393817,0.653125);
vpnt[668] = VPosNormalTex(-0.30625,-0.0756987,0.0580697,0.893604,-0.317389,-0.317389,0.395854,0.653125);
vpnt[669] = VPosNormalTex(-0.35,-0.116256,-0.02455,0.784699,-0.598755,-0.160436,-0.466878,0.675);
vpnt[670] = VPosNormalTex(-0.35,-0.118936,-0.01455,0.784699,-0.598755,-0.160436,-0.480626,0.675);
vpnt[671] = VPosNormalTex(-0.30625,-0.0856986,0.0753902,0.784699,-0.598755,-0.160436,0.385171,0.653125);
vpnt[672] = VPosNormalTex(-0.30625,-0.0830192,0.0653902,0.784699,-0.598755,-0.160436,0.393817,0.653125);
vpnt[673] = VPosNormalTex(-0.35,-0.118936,-0.01455,0.197761,-0.946849,0.253708,-0.480626,0.675);
vpnt[674] = VPosNormalTex(-0.35,-0.116256,-0.00455002,0.197761,-0.946849,0.253708,-0.493774,0.675);
vpnt[675] = VPosNormalTex(-0.30625,-0.0830192,0.0853902,0.197761,-0.946849,0.253708,0.372759,0.653125);
vpnt[676] = VPosNormalTex(-0.30625,-0.0856986,0.0753902,0.197761,-0.946849,0.253708,0.385171,0.653125);
vpnt[677] = VPosNormalTex(-0.35,-0.116256,-0.00455002,-0.675642,-0.5213,0.5213,-0.493774,0.675);
vpnt[678] = VPosNormalTex(-0.35,-0.108936,0.00277049,-0.675642,-0.5213,0.5213,0.495953,0.675);
vpnt[679] = VPosNormalTex(-0.30625,-0.0756987,0.0927107,-0.675642,-0.5213,0.5213,0.358977,0.653125);
vpnt[680] = VPosNormalTex(-0.30625,-0.0830192,0.0853902,-0.675642,-0.5213,0.5213,0.372759,0.653125);
vpnt[681] = VPosNormalTex(-0.35,-0.108936,0.00277049,-0.8729,-0.126277,0.471274,0.495953,0.675);
vpnt[682] = VPosNormalTex(-0.35,-0.0989358,0.00544998,-0.8729,-0.126277,0.471274,0.491242,0.675);
vpnt[683] = VPosNormalTex(-0.30625,-0.0656987,0.0953902,-0.8729,-0.126277,0.471274,0.34599,0.653125);
vpnt[684] = VPosNormalTex(-0.30625,-0.0756987,0.0927107,-0.8729,-0.126277,0.471274,0.358977,0.653125);
vpnt[685] = VPosNormalTex(-0.30625,-0.0656987,0.0953902,-0.710396,0.182157,0.679821,0.34599,0.653125);
vpnt[686] = VPosNormalTex(-0.30625,-0.0556987,0.0927107,-0.710396,0.182157,0.679821,0.336102,0.653125);
vpnt[687] = VPosNormalTex(-0.2625,0.0379415,0.113338,-0.710396,0.182157,0.679821,0.198587,0.63125);
vpnt[688] = VPosNormalTex(-0.2625,0.0279415,0.116017,-0.710396,0.182157,0.679821,0.212385,0.63125);
vpnt[689] = VPosNormalTex(-0.30625,-0.0556987,0.0927107,-0.879365,0.336687,0.336687,0.336102,0.653125);
vpnt[690] = VPosNormalTex(-0.30625,-0.0483781,0.0853902,-0.879365,0.336687,0.336687,0.332039,0.653125);
vpnt[691] = VPosNormalTex(-0.2625,0.045262,0.106017,-0.879365,0.336687,0.336687,0.18578,0.63125);
vpnt[692] = VPosNormalTex(-0.2625,0.0379415,0.113338,-0.879365,0.336687,0.336687,0.198587,0.63125);
vpnt[693] = VPosNormalTex(-0.30625,-0.0483781,0.0853902,-0.909614,0.401298,0.107528,0.332039,0.653125);
vpnt[694] = VPosNormalTex(-0.30625,-0.0456987,0.0753902,-0.909614,0.401298,0.107528,0.33673,0.653125);
vpnt[695] = VPosNormalTex(-0.2625,0.0479415,0.096017,-0.909614,0.401298,0.107528,0.176297,0.63125);
vpnt[696] = VPosNormalTex(-0.2625,0.045262,0.106017,-0.909614,0.401298,0.107528,0.18578,0.63125);
vpnt[697] = VPosNormalTex(-0.30625,-0.0456987,0.0753902,-0.889378,0.441594,-0.118325,0.33673,0.653125);
vpnt[698] = VPosNormalTex(-0.30625,-0.0483781,0.0653902,-0.889378,0.441594,-0.118325,0.351376,0.653125);
vpnt[699] = VPosNormalTex(-0.2625,0.045262,0.086017,-0.889378,0.441594,-0.118325,0.172908,0.63125);
vpnt[700] = VPosNormalTex(-0.2625,0.0479415,0.096017,-0.889378,0.441594,-0.118325,0.176297,0.63125);
vpnt[701] = VPosNormalTex(-0.30625,-0.0483781,0.0653902,-0.762916,0.457143,-0.457143,0.351376,0.653125);
vpnt[702] = VPosNormalTex(-0.30625,-0.0556987,0.0580697,-0.762916,0.457143,-0.457143,0.371683,0.653125);
vpnt[703] = VPosNormalTex(-0.2625,0.0379415,0.0786965,-0.762916,0.457143,-0.457143,0.1785,0.63125);
vpnt[704] = VPosNormalTex(-0.2625,0.045262,0.086017,-0.762916,0.457143,-0.457143,0.172908,0.63125);
vpnt[705] = VPosNormalTex(-0.30625,-0.0556987,0.0580697,-0.098083,0.257571,-0.961268,0.371683,0.653125);
vpnt[706] = VPosNormalTex(-0.30625,-0.0656987,0.0553902,-0.098083,0.257571,-0.961268,0.388516,0.653125);
vpnt[707] = VPosNormalTex(-0.2625,0.0279415,0.076017,-0.098083,0.257571,-0.961268,0.193939,0.63125);
vpnt[708] = VPosNormalTex(-0.2625,0.0379415,0.0786965,-0.098083,0.257571,-0.961268,0.1785,0.63125);
vpnt[709] = VPosNormalTex(-0.30625,-0.0656987,0.0553902,0.710396,-0.182158,-0.679821,0.388516,0.653125);
vpnt[710] = VPosNormalTex(-0.30625,-0.0756987,0.0580697,0.710396,-0.182158,-0.679821,0.395854,0.653125);
vpnt[711] = VPosNormalTex(-0.2625,0.0179415,0.0786965,0.710396,-0.182158,-0.679821,0.214325,0.63125);
vpnt[712] = VPosNormalTex(-0.2625,0.0279415,0.076017,0.710396,-0.182158,-0.679821,0.193939,0.63125);
vpnt[713] = VPosNormalTex(-0.30625,-0.0756987,0.0580697,0.879365,-0.336687,-0.336687,0.395854,0.653125);
vpnt[714] = VPosNormalTex(-0.30625,-0.0830192,0.0653902,0.879365,-0.336687,-0.336687,0.393817,0.653125);
vpnt[715] = VPosNormalTex(-0.2625,0.010621,0.086017,0.879365,-0.336687,-0.336687,0.230447,0.63125);
vpnt[716] = VPosNormalTex(-0.2625,0.0179415,0.0786965,0.879365,-0.336687,-0.336687,0.214325,0.63125);
vpnt[717] = VPosNormalTex(-0.30625,-0.0830192,0.0653902,0.909614,-0.401298,-0.107528,0.393817,0.653125);
vpnt[718] = VPosNormalTex(-0.30625,-0.0856986,0.0753902,0.909614,-0.401298,-0.107528,0.385171,0.653125);
vpnt[719] = VPosNormalTex(-0.2625,0.00794153,0.096017,0.909614,-0.401298,-0.107528,0.236866,0.63125);
vpnt[720] = VPosNormalTex(-0.2625,0.010621,0.086017,0.909614,-0.401298,-0.107528,0.230447,0.63125);
vpnt[721] = VPosNormalTex(-0.30625,-0.0856986,0.0753902,0.889378,-0.441594,0.118325,0.385171,0.653125);
vpnt[722] = VPosNormalTex(-0.30625,-0.0830192,0.0853902,0.889378,-0.441594,0.118325,0.372759,0.653125);
vpnt[723] = VPosNormalTex(-0.2625,0.010621,0.106017,0.889378,-0.441594,0.118325,0.234109,0.63125);
vpnt[724] = VPosNormalTex(-0.2625,0.00794153,0.096017,0.889378,-0.441594,0.118325,0.236866,0.63125);
vpnt[725] = VPosNormalTex(-0.30625,-0.0830192,0.0853902,0.762916,-0.457143,0.457143,0.372759,0.653125);
vpnt[726] = VPosNormalTex(-0.30625,-0.0756987,0.0927107,0.762916,-0.457143,0.457143,0.358977,0.653125);
vpnt[727] = VPosNormalTex(-0.2625,0.0179415,0.113338,0.762916,-0.457143,0.457143,0.225013,0.63125);
vpnt[728] = VPosNormalTex(-0.2625,0.010621,0.106017,0.762916,-0.457143,0.457143,0.234109,0.63125);
vpnt[729] = VPosNormalTex(-0.30625,-0.0756987,0.0927107,0.0980823,-0.257571,0.961268,0.358977,0.653125);
vpnt[730] = VPosNormalTex(-0.30625,-0.0656987,0.0953902,0.0980823,-0.257571,0.961268,0.34599,0.653125);
vpnt[731] = VPosNormalTex(-0.2625,0.0279415,0.116017,0.0980823,-0.257571,0.961268,0.212385,0.63125);
vpnt[732] = VPosNormalTex(-0.2625,0.0179415,0.113338,0.0980823,-0.257571,0.961268,0.225013,0.63125);
vpnt[733] = VPosNormalTex(-0.2625,0.0279415,0.116017,0.737375,0.174828,0.652467,0.212385,0.63125);
vpnt[734] = VPosNormalTex(-0.2625,0.0379415,0.113338,0.737375,0.174828,0.652467,0.198587,0.63125);
vpnt[735] = VPosNormalTex(-0.21875,0.105892,0.0456867,0.737375,0.174828,0.652467,0.0648265,0.609375);
vpnt[736] = VPosNormalTex(-0.21875,0.0958924,0.0483662,0.737375,0.174828,0.652467,0.0743485,0.609375);
vpnt[737] = VPosNormalTex(-0.2625,0.0379415,0.113338,-0.0048502,0.707098,0.707098,0.198587,0.63125);
vpnt[738] = VPosNormalTex(-0.2625,0.045262,0.106017,-0.0048502,0.707098,0.707098,0.18578,0.63125);
vpnt[739] = VPosNormalTex(-0.21875,0.113213,0.0383662,-0.0048502,0.707098,0.707098,0.0520021,0.609375);
vpnt[740] = VPosNormalTex(-0.21875,0.105892,0.0456867,-0.0048502,0.707098,0.707098,0.0648265,0.609375);
vpnt[741] = VPosNormalTex(-0.2625,0.045262,0.106017,-0.739948,0.649743,0.174098,0.18578,0.63125);
vpnt[742] = VPosNormalTex(-0.2625,0.0479415,0.096017,-0.739948,0.649743,0.174098,0.176297,0.63125);
vpnt[743] = VPosNormalTex(-0.21875,0.115892,0.0283662,-0.739948,0.649743,0.174098,0.0382042,0.609375);
vpnt[744] = VPosNormalTex(-0.21875,0.113213,0.0383662,-0.739948,0.649743,0.174098,0.0520021,0.609375);
vpnt[745] = VPosNormalTex(-0.2625,0.0479415,0.096017,-0.884964,0.449793,-0.120522,0.176297,0.63125);
vpnt[746] = VPosNormalTex(-0.2625,0.045262,0.086017,-0.884964,0.449793,-0.120522,0.172908,0.63125);
vpnt[747] = VPosNormalTex(-0.21875,0.113213,0.0183662,-0.884964,0.449793,-0.120522,0.0255963,0.609375);
vpnt[748] = VPosNormalTex(-0.21875,0.115892,0.0283662,-0.884964,0.449793,-0.120522,0.0382042,0.609375);
vpnt[749] = VPosNormalTex(-0.2625,0.045262,0.086017,-0.909772,0.293525,-0.293525,0.172908,0.63125);
vpnt[750] = VPosNormalTex(-0.2625,0.0379415,0.0786965,-0.909772,0.293525,-0.293525,0.1785,0.63125);
vpnt[751] = VPosNormalTex(-0.21875,0.105892,0.0110457,-0.909772,0.293525,-0.293525,0.0165417,0.609375);
vpnt[752] = VPosNormalTex(-0.21875,0.113213,0.0183662,-0.909772,0.293525,-0.293525,0.0255963,0.609375);
vpnt[753] = VPosNormalTex(-0.2625,0.0379415,0.0786965,-0.884473,0.120763,-0.450693,0.1785,0.63125);
vpnt[754] = VPosNormalTex(-0.2625,0.0279415,0.076017,-0.884473,0.120763,-0.450693,0.193939,0.63125);
vpnt[755] = VPosNormalTex(-0.21875,0.0958924,0.00836622,-0.884473,0.120763,-0.450693,0.0138505,0.609375);
vpnt[756] = VPosNormalTex(-0.21875,0.105892,0.0110457,-0.884473,0.120763,-0.450693,0.0165417,0.609375);
vpnt[757] = VPosNormalTex(-0.2625,0.0279415,0.076017,-0.737375,-0.174828,-0.652466,0.193939,0.63125);
vpnt[758] = VPosNormalTex(-0.2625,0.0179415,0.0786965,-0.737375,-0.174828,-0.652466,0.214325,0.63125);
vpnt[759] = VPosNormalTex(-0.21875,0.0858924,0.0110457,-0.737375,-0.174828,-0.652466,0.0203555,0.609375);
vpnt[760] = VPosNormalTex(-0.21875,0.0958924,0.00836622,-0.737375,-0.174828,-0.652466,0.0138505,0.609375);
vpnt[761] = VPosNormalTex(-0.2625,0.0179415,0.0786965,0.00485008,-0.707098,-0.707098,0.214325,0.63125);
vpnt[762] = VPosNormalTex(-0.2625,0.010621,0.086017,0.00485008,-0.707098,-0.707098,0.230447,0.63125);
vpnt[763] = VPosNormalTex(-0.21875,0.0785719,0.0183662,0.00485008,-0.707098,-0.707098,0.0365463,0.609375);
vpnt[764] = VPosNormalTex(-0.21875,0.0858924,0.0110457,0.00485008,-0.707098,-0.707098,0.0203555,0.609375);
vpnt[765] = VPosNormalTex(-0.2625,0.010621,0.086017,0.739948,-0.649743,-0.174098,0.230447,0.63125);
vpnt[766] = VPosNormalTex(-0.2625,0.00794153,0.096017,0.739948,-0.649743,-0.174098,0.236866,0.63125);
vpnt[767] = VPosNormalTex(-0.21875,0.0758924,0.0283662,0.739948,-0.649743,-0.174098,0.0569283,0.609375);
vpnt[768] = VPosNormalTex(-0.21875,0.0785719,0.0183662,0.739948,-0.649743,-0.174098,0.0365463,0.609375);
vpnt[769] = VPosNormalTex(-0.2625,0.00794153,0.096017,0.884964,-0.449793,0.120522,0.236866,0.63125);
vpnt[770] = VPosNormalTex(-0.2625,0.010621,0.106017,0.884964,-0.449793,0.120522,0.234109,0.63125);
vpnt[771] = VPosNormalTex(-0.21875,0.0785719,0.0383662,0.884964,-0.449793,0.120522,0.0722944,0.609375);
vpnt[772] = VPosNormalTex(-0.21875,0.0758924,0.0283662,0.884964,-0.449793,0.120522,0.0569283,0.609375);
vpnt[773] = VPosNormalTex(-0.2625,0.010621,0.106017,0.909772,-0.293525,0.293525,0.234109,0.63125);
vpnt[774] = VPosNormalTex(-0.2625,0.0179415,0.113338,0.909772,-0.293525,0.293525,0.225013,0.63125);
vpnt[775] = VPosNormalTex(-0.21875,0.0858924,0.0456867,0.909772,-0.293525,0.293525,0.0778022,0.609375);
vpnt[776] = VPosNormalTex(-0.21875,0.0785719,0.0383662,0.909772,-0.293525,0.293525,0.0722944,0.609375);
vpnt[777] = VPosNormalTex(-0.2625,0.0179415,0.113338,0.884473,-0.120763,0.450693,0.225013,0.63125);
vpnt[778] = VPosNormalTex(-0.2625,0.0279415,0.116017,0.884473,-0.120763,0.450693,0.212385,0.63125);
vpnt[779] = VPosNormalTex(-0.21875,0.0958924,0.0483662,0.884473,-0.120763,0.450693,0.0743485,0.609375);
vpnt[780] = VPosNormalTex(-0.21875,0.0858924,0.0456867,0.884473,-0.120763,0.450693,0.0778022,0.609375);
vpnt[781] = VPosNormalTex(-0.21875,0.0958924,0.0483662,0.909581,0.107546,0.401368,0.0743485,0.609375);
vpnt[782] = VPosNormalTex(-0.21875,0.105892,0.0456867,0.909581,0.107546,0.401368,0.0648265,0.609375);
vpnt[783] = VPosNormalTex(-0.175,0.0856803,-0.0480439,0.909581,0.107546,0.401368,-0.0813357,0.5875);
vpnt[784] = VPosNormalTex(-0.175,0.0756803,-0.0453644,0.909581,0.107546,0.401368,-0.0859427,0.5875);
vpnt[785] = VPosNormalTex(-0.21875,0.105892,0.0456867,0.878798,0.337427,0.337427,0.0648265,0.609375);
vpnt[786] = VPosNormalTex(-0.21875,0.113213,0.0383662,0.878798,0.337427,0.337427,0.0520021,0.609375);
vpnt[787] = VPosNormalTex(-0.175,0.0930008,-0.0553644,0.878798,0.337427,0.337427,-0.0854606,0.5875);
vpnt[788] = VPosNormalTex(-0.175,0.0856803,-0.0480439,0.878798,0.337427,0.337427,-0.0813357,0.5875);
vpnt[789] = VPosNormalTex(-0.21875,0.113213,0.0383662,0.707371,0.682758,0.182944,0.0520021,0.609375);
vpnt[790] = VPosNormalTex(-0.21875,0.115892,0.0283662,0.707371,0.682758,0.182944,0.0382042,0.609375);
vpnt[791] = VPosNormalTex(-0.175,0.0956803,-0.0653644,0.707371,0.682758,0.182944,-0.0953864,0.5875);
vpnt[792] = VPosNormalTex(-0.175,0.0930008,-0.0553644,0.707371,0.682758,0.182944,-0.0854606,0.5875);
vpnt[793] = VPosNormalTex(-0.21875,0.115892,0.0283662,-0.107618,0.960316,-0.257316,0.0382042,0.609375);
vpnt[794] = VPosNormalTex(-0.21875,0.113213,0.0183662,-0.107618,0.960316,-0.257316,0.0255963,0.609375);
vpnt[795] = VPosNormalTex(-0.175,0.0930008,-0.0753644,-0.107618,0.960316,-0.257316,-0.108389,0.5875);
vpnt[796] = VPosNormalTex(-0.175,0.0956803,-0.0653644,-0.107618,0.960316,-0.257316,-0.0953864,0.5875);
vpnt[797] = VPosNormalTex(-0.21875,0.113213,0.0183662,-0.765108,0.455307,-0.455307,0.0255963,0.609375);
vpnt[798] = VPosNormalTex(-0.21875,0.105892,0.0110457,-0.765108,0.455307,-0.455307,0.0165417,0.609375);
vpnt[799] = VPosNormalTex(-0.175,0.0856803,-0.0826849,-0.765108,0.455307,-0.455307,-0.122169,0.5875);
vpnt[800] = VPosNormalTex(-0.175,0.0930008,-0.0753644,-0.765108,0.455307,-0.455307,-0.108389,0.5875);
vpnt[801] = VPosNormalTex(-0.21875,0.105892,0.0110457,-0.889802,0.118111,-0.440797,0.0165417,0.609375);
vpnt[802] = VPosNormalTex(-0.21875,0.0958924,0.00836622,-0.889802,0.118111,-0.440797,0.0138505,0.609375);
vpnt[803] = VPosNormalTex(-0.175,0.0756803,-0.0853644,-0.889802,0.118111,-0.440797,-0.134559,0.5875);
vpnt[804] = VPosNormalTex(-0.175,0.0856803,-0.0826849,-0.889802,0.118111,-0.440797,-0.122169,0.5875);
vpnt[805] = VPosNormalTex(-0.21875,0.0958924,0.00836622,-0.909581,-0.107546,-0.401368,0.0138505,0.609375);
vpnt[806] = VPosNormalTex(-0.21875,0.0858924,0.0110457,-0.909581,-0.107546,-0.401368,0.0203555,0.609375);
vpnt[807] = VPosNormalTex(-0.175,0.0656803,-0.0826849,-0.909581,-0.107546,-0.401368,-0.143162,0.5875);
vpnt[808] = VPosNormalTex(-0.175,0.0756803,-0.0853644,-0.909581,-0.107546,-0.401368,-0.134559,0.5875);
vpnt[809] = VPosNormalTex(-0.21875,0.0858924,0.0110457,-0.878798,-0.337427,-0.337427,0.0203555,0.609375);
vpnt[810] = VPosNormalTex(-0.21875,0.0785719,0.0183662,-0.878798,-0.337427,-0.337427,0.0365463,0.609375);
vpnt[811] = VPosNormalTex(-0.175,0.0583597,-0.0753644,-0.878798,-0.337427,-0.337427,-0.14513,0.5875);
vpnt[812] = VPosNormalTex(-0.175,0.0656803,-0.0826849,-0.878798,-0.337427,-0.337427,-0.143162,0.5875);
vpnt[813] = VPosNormalTex(-0.21875,0.0785719,0.0183662,-0.707371,-0.682758,-0.182944,0.0365463,0.609375);
vpnt[814] = VPosNormalTex(-0.21875,0.0758924,0.0283662,-0.707371,-0.682758,-0.182944,0.0569283,0.609375);
vpnt[815] = VPosNormalTex(-0.175,0.0556803,-0.0653644,-0.707371,-0.682758,-0.182944,-0.137706,0.5875);
vpnt[816] = VPosNormalTex(-0.175,0.0583597,-0.0753644,-0.707371,-0.682758,-0.182944,-0.14513,0.5875);
vpnt[817] = VPosNormalTex(-0.21875,0.0758924,0.0283662,0.107618,-0.960316,0.257316,0.0569283,0.609375);
vpnt[818] = VPosNormalTex(-0.21875,0.0785719,0.0383662,0.107618,-0.960316,0.257316,0.0722944,0.609375);
vpnt[819] = VPosNormalTex(-0.175,0.0583597,-0.0553644,0.107618,-0.960316,0.257316,-0.120809,0.5875);
vpnt[820] = VPosNormalTex(-0.175,0.0556803,-0.0653644,0.107618,-0.960316,0.257316,-0.137706,0.5875);
vpnt[821] = VPosNormalTex(-0.21875,0.0785719,0.0383662,0.765108,-0.455307,0.455307,0.0722944,0.609375);
vpnt[822] = VPosNormalTex(-0.21875,0.0858924,0.0456867,0.765108,-0.455307,0.455307,0.0778022,0.609375);
vpnt[823] = VPosNormalTex(-0.175,0.0656803,-0.0480439,0.765108,-0.455307,0.455307,-0.100513,0.5875);
vpnt[824] = VPosNormalTex(-0.175,0.0583597,-0.0553644,0.765108,-0.455307,0.455307,-0.120809,0.5875);
vpnt[825] = VPosNormalTex(-0.21875,0.0858924,0.0456867,0.889802,-0.118111,0.440797,0.0778022,0.609375);
vpnt[826] = VPosNormalTex(-0.21875,0.0958924,0.0483662,0.889802,-0.118111,0.440797,0.0743485,0.609375);
vpnt[827] = VPosNormalTex(-0.175,0.0756803,-0.0453644,0.889802,-0.118111,0.440797,-0.0859427,0.5875);
vpnt[828] = VPosNormalTex(-0.175,0.0656803,-0.0480439,0.889802,-0.118111,0.440797,-0.100513,0.5875);
vpnt[829] = VPosNormalTex(-0.175,0.0756803,-0.0453644,0.786571,0.159821,0.596459,-0.0859427,0.5875);
vpnt[830] = VPosNormalTex(-0.175,0.0856803,-0.0480439,0.786571,0.159821,0.596459,-0.0813357,0.5875);
vpnt[831] = VPosNormalTex(-0.13125,-0.00411199,-0.0816787,0.786571,0.159821,0.596459,-0.258006,0.565625);
vpnt[832] = VPosNormalTex(-0.13125,-0.014112,-0.0789993,0.786571,0.159821,0.596459,-0.278134,0.565625);
vpnt[833] = VPosNormalTex(-0.175,0.0856803,-0.0480439,0.893968,0.316876,0.316876,-0.0813357,0.5875);
vpnt[834] = VPosNormalTex(-0.175,0.0930008,-0.0553644,0.893968,0.316876,0.316876,-0.0854606,0.5875);
vpnt[835] = VPosNormalTex(-0.13125,0.00320852,-0.0889993,0.893968,0.316876,0.316876,-0.244265,0.565625);
vpnt[836] = VPosNormalTex(-0.13125,-0.00411199,-0.0816787,0.893968,0.316876,0.316876,-0.258006,0.565625);
vpnt[837] = VPosNormalTex(-0.175,0.0930008,-0.0553644,0.909038,0.402515,0.107854,-0.0854606,0.5875);
vpnt[838] = VPosNormalTex(-0.175,0.0956803,-0.0653644,0.909038,0.402515,0.107854,-0.0953864,0.5875);
vpnt[839] = VPosNormalTex(-0.13125,0.00588801,-0.0989993,0.909038,0.402515,0.107854,-0.240545,0.565625);
vpnt[840] = VPosNormalTex(-0.13125,0.00320852,-0.0889993,0.909038,0.402515,0.107854,-0.244265,0.565625);
vpnt[841] = VPosNormalTex(-0.175,0.0956803,-0.0653644,0.872245,0.472404,-0.12658,-0.0953864,0.5875);
vpnt[842] = VPosNormalTex(-0.175,0.0930008,-0.0753644,0.872245,0.472404,-0.12658,-0.108389,0.5875);
vpnt[843] = VPosNormalTex(-0.13125,0.00320852,-0.108999,0.872245,0.472404,-0.12658,-0.245316,0.565625);
vpnt[844] = VPosNormalTex(-0.13125,0.00588801,-0.0989993,0.872245,0.472404,-0.12658,-0.240545,0.565625);
vpnt[845] = VPosNormalTex(-0.175,0.0930008,-0.0753644,0.672084,0.523595,-0.523595,-0.108389,0.5875);
vpnt[846] = VPosNormalTex(-0.175,0.0856803,-0.0826849,0.672084,0.523595,-0.523595,-0.122169,0.5875);
vpnt[847] = VPosNormalTex(-0.13125,-0.00411199,-0.11632,0.672084,0.523595,-0.523595,-0.255624,0.565625);
vpnt[848] = VPosNormalTex(-0.13125,0.00320852,-0.108999,0.672084,0.523595,-0.523595,-0.245316,0.565625);
vpnt[849] = VPosNormalTex(-0.175,0.0856803,-0.0826849,-0.206832,0.253222,-0.945039,-0.122169,0.5875);
vpnt[850] = VPosNormalTex(-0.175,0.0756803,-0.0853644,-0.206832,0.253222,-0.945039,-0.134559,0.5875);
vpnt[851] = VPosNormalTex(-0.13125,-0.014112,-0.118999,-0.206832,0.253222,-0.945039,-0.268786,0.565625);
vpnt[852] = VPosNormalTex(-0.13125,-0.00411199,-0.11632,-0.206832,0.253222,-0.945039,-0.255624,0.565625);
vpnt[853] = VPosNormalTex(-0.175,0.0756803,-0.0853644,-0.786571,-0.159821,-0.596459,-0.134559,0.5875);
vpnt[854] = VPosNormalTex(-0.175,0.0656803,-0.0826849,-0.786571,-0.159821,-0.596459,-0.143162,0.5875);
vpnt[855] = VPosNormalTex(-0.13125,-0.024112,-0.11632,-0.786571,-0.159821,-0.596459,-0.282531,0.565625);
vpnt[856] = VPosNormalTex(-0.13125,-0.014112,-0.118999,-0.786571,-0.159821,-0.596459,-0.268786,0.565625);
vpnt[857] = VPosNormalTex(-0.175,0.0656803,-0.0826849,-0.893968,-0.316876,-0.316876,-0.143162,0.5875);
vpnt[858] = VPosNormalTex(-0.175,0.0583597,-0.0753644,-0.893968,-0.316876,-0.316876,-0.14513,0.5875);
vpnt[859] = VPosNormalTex(-0.13125,-0.0314325,-0.108999,-0.893968,-0.316876,-0.316876,-0.294684,0.565625);
vpnt[860] = VPosNormalTex(-0.13125,-0.024112,-0.11632,-0.893968,-0.316876,-0.316876,-0.282531,0.565625);
vpnt[861] = VPosNormalTex(-0.175,0.0583597,-0.0753644,-0.909038,-0.402515,-0.107854,-0.14513,0.5875);
vpnt[862] = VPosNormalTex(-0.175,0.0556803,-0.0653644,-0.909038,-0.402515,-0.107854,-0.137706,0.5875);
vpnt[863] = VPosNormalTex(-0.13125,-0.034112,-0.0989993,-0.909038,-0.402515,-0.107854,-0.302812,0.565625);
vpnt[864] = VPosNormalTex(-0.13125,-0.0314325,-0.108999,-0.909038,-0.402515,-0.107854,-0.294684,0.565625);
vpnt[865] = VPosNormalTex(-0.175,0.0556803,-0.0653644,-0.872245,-0.472404,0.12658,-0.137706,0.5875);
vpnt[866] = VPosNormalTex(-0.175,0.0583597,-0.0553644,-0.872245,-0.472404,0.12658,-0.120809,0.5875);
vpnt[867] = VPosNormalTex(-0.13125,-0.0314325,-0.0889993,-0.872245,-0.472404,0.12658,-0.304033,0.565625);
vpnt[868] = VPosNormalTex(-0.13125,-0.034112,-0.0989993,-0.872245,-0.472404,0.12658,-0.302812,0.565625);
vpnt[869] = VPosNormalTex(-0.175,0.0583597,-0.0553644,-0.672084,-0.523595,0.523595,-0.120809,0.5875);
vpnt[870] = VPosNormalTex(-0.175,0.0656803,-0.0480439,-0.672084,-0.523595,0.523595,-0.100513,0.5875);
vpnt[871] = VPosNormalTex(-0.13125,-0.024112,-0.0816787,-0.672084,-0.523595,0.523595,-0.295686,0.565625);
vpnt[872] = VPosNormalTex(-0.13125,-0.0314325,-0.0889993,-0.672084,-0.523595,0.523595,-0.304033,0.565625);
vpnt[873] = VPosNormalTex(-0.175,0.0656803,-0.0480439,0.206831,-0.253223,0.945039,-0.100513,0.5875);
vpnt[874] = VPosNormalTex(-0.175,0.0756803,-0.0453644,0.206831,-0.253223,0.945039,-0.0859427,0.5875);
vpnt[875] = VPosNormalTex(-0.13125,-0.014112,-0.0789993,0.206831,-0.253223,0.945039,-0.278134,0.565625);
vpnt[876] = VPosNormalTex(-0.13125,-0.024112,-0.0816787,0.206831,-0.253223,0.945039,-0.295686,0.565625);
vpnt[877] = VPosNormalTex(-0.13125,-0.014112,-0.0789993,-0.630597,0.200873,0.749665,-0.278134,0.565625);
vpnt[878] = VPosNormalTex(-0.13125,-0.00411199,-0.0816787,-0.630597,0.200873,0.749665,-0.258006,0.565625);
vpnt[879] = VPosNormalTex(-0.0875,-0.0809297,-0.0242942,-0.630597,0.200873,0.749665,-0.453586,0.54375);
vpnt[880] = VPosNormalTex(-0.0875,-0.0909297,-0.0216147,-0.630597,0.200873,0.749665,-0.462857,0.54375);
vpnt[881] = VPosNormalTex(-0.13125,-0.00411199,-0.0816787,0.299655,0.674614,0.674614,-0.258006,0.565625);
vpnt[882] = VPosNormalTex(-0.13125,0.00320852,-0.0889993,0.299655,0.674614,0.674614,-0.244265,0.565625);
vpnt[883] = VPosNormalTex(-0.0875,-0.0736092,-0.0316147,0.299655,0.674614,0.674614,-0.435436,0.54375);
vpnt[884] = VPosNormalTex(-0.0875,-0.0809297,-0.0242942,0.299655,0.674614,0.674614,-0.453586,0.54375);
vpnt[885] = VPosNormalTex(-0.13125,0.00320852,-0.0889993,0.804927,0.573155,0.153576,-0.244265,0.565625);
vpnt[886] = VPosNormalTex(-0.13125,0.00588801,-0.0989993,0.804927,0.573155,0.153576,-0.240545,0.565625);
vpnt[887] = VPosNormalTex(-0.0875,-0.0709297,-0.0416147,0.804927,0.573155,0.153576,-0.415555,0.54375);
vpnt[888] = VPosNormalTex(-0.0875,-0.0736092,-0.0316147,0.804927,0.573155,0.153576,-0.435436,0.54375);
vpnt[889] = VPosNormalTex(-0.13125,0.00588801,-0.0989993,0.897535,0.425919,-0.114125,-0.240545,0.565625);
vpnt[890] = VPosNormalTex(-0.13125,0.00320852,-0.108999,0.897535,0.425919,-0.114125,-0.245316,0.565625);
vpnt[891] = VPosNormalTex(-0.0875,-0.0736092,-0.0516147,0.897535,0.425919,-0.114125,-0.402672,0.54375);
vpnt[892] = VPosNormalTex(-0.0875,-0.0709297,-0.0416147,0.897535,0.425919,-0.114125,-0.415555,0.54375);
vpnt[893] = VPosNormalTex(-0.13125,0.00320852,-0.108999,0.908133,0.296052,-0.296052,-0.245316,0.565625);
vpnt[894] = VPosNormalTex(-0.13125,-0.00411199,-0.11632,0.908133,0.296052,-0.296052,-0.255624,0.565625);
vpnt[895] = VPosNormalTex(-0.0875,-0.0809297,-0.0589352,0.908133,0.296052,-0.296052,-0.399825,0.54375);
vpnt[896] = VPosNormalTex(-0.0875,-0.0736092,-0.0516147,0.908133,0.296052,-0.296052,-0.402672,0.54375);
vpnt[897] = VPosNormalTex(-0.13125,-0.00411199,-0.11632,0.864684,0.130009,-0.485199,-0.255624,0.565625);
vpnt[898] = VPosNormalTex(-0.13125,-0.014112,-0.118999,0.864684,0.130009,-0.485199,-0.268786,0.565625);
vpnt[899] = VPosNormalTex(-0.0875,-0.0909297,-0.0616147,0.864684,0.130009,-0.485199,-0.405217,0.54375);
vpnt[900] = VPosNormalTex(-0.0875,-0.0809297,-0.0589352,0.864684,0.130009,-0.485199,-0.399825,0.54375);
vpnt[901] = VPosNormalTex(-0.13125,-0.014112,-0.118999,0.630597,-0.200872,-0.749665,-0.268786,0.565625);
vpnt[902] = VPosNormalTex(-0.13125,-0.024112,-0.11632,0.630597,-0.200872,-0.749665,-0.282531,0.565625);
vpnt[903] = VPosNormalTex(-0.0875,-0.10093,-0.0589352,0.630597,-0.200872,-0.749665,-0.415884,0.54375);
vpnt[904] = VPosNormalTex(-0.0875,-0.0909297,-0.0616147,0.630597,-0.200872,-0.749665,-0.405217,0.54375);
vpnt[905] = VPosNormalTex(-0.13125,-0.024112,-0.11632,-0.299655,-0.674614,-0.674614,-0.282531,0.565625);
vpnt[906] = VPosNormalTex(-0.13125,-0.0314325,-0.108999,-0.299655,-0.674614,-0.674614,-0.294684,0.565625);
vpnt[907] = VPosNormalTex(-0.0875,-0.10825,-0.0516147,-0.299655,-0.674614,-0.674614,-0.429188,0.54375);
vpnt[908] = VPosNormalTex(-0.0875,-0.10093,-0.0589352,-0.299655,-0.674614,-0.674614,-0.415884,0.54375);
vpnt[909] = VPosNormalTex(-0.13125,-0.0314325,-0.108999,-0.804927,-0.573155,-0.153576,-0.294684,0.565625);
vpnt[910] = VPosNormalTex(-0.13125,-0.034112,-0.0989993,-0.804927,-0.573155,-0.153576,-0.302812,0.565625);
vpnt[911] = VPosNormalTex(-0.0875,-0.11093,-0.0416147,-0.804927,-0.573155,-0.153576,-0.44288,0.54375);
vpnt[912] = VPosNormalTex(-0.0875,-0.10825,-0.0516147,-0.804927,-0.573155,-0.153576,-0.429188,0.54375);
vpnt[913] = VPosNormalTex(-0.13125,-0.034112,-0.0989993,-0.897535,-0.425919,0.114125,-0.302812,0.565625);
vpnt[914] = VPosNormalTex(-0.13125,-0.0314325,-0.0889993,-0.897535,-0.425919,0.114125,-0.304033,0.565625);
vpnt[915] = VPosNormalTex(-0.0875,-0.10825,-0.0316147,-0.897535,-0.425919,0.114125,-0.454776,0.54375);
vpnt[916] = VPosNormalTex(-0.0875,-0.11093,-0.0416147,-0.897535,-0.425919,0.114125,-0.44288,0.54375);
vpnt[917] = VPosNormalTex(-0.13125,-0.0314325,-0.0889993,-0.908134,-0.296052,0.296052,-0.304033,0.565625);
vpnt[918] = VPosNormalTex(-0.13125,-0.024112,-0.0816787,-0.908134,-0.296052,0.296052,-0.295686,0.565625);
vpnt[919] = VPosNormalTex(-0.0875,-0.10093,-0.0242942,-0.908134,-0.296052,0.296052,-0.462406,0.54375);
vpnt[920] = VPosNormalTex(-0.0875,-0.10825,-0.0316147,-0.908134,-0.296052,0.296052,-0.454776,0.54375);
vpnt[921] = VPosNormalTex(-0.13125,-0.024112,-0.0816787,-0.864685,-0.130009,0.485199,-0.295686,0.565625);
vpnt[922] = VPosNormalTex(-0.13125,-0.014112,-0.0789993,-0.864685,-0.130009,0.485199,-0.278134,0.565625);
vpnt[923] = VPosNormalTex(-0.0875,-0.0909297,-0.0216147,-0.864685,-0.130009,0.485199,-0.462857,0.54375);
vpnt[924] = VPosNormalTex(-0.0875,-0.10093,-0.0242942,-0.864685,-0.130009,0.485199,-0.462406,0.54375);
vpnt[925] = VPosNormalTex(-0.0875,-0.0909297,-0.0216147,-0.906856,0.109077,0.40708,-0.462857,0.54375);
vpnt[926] = VPosNormalTex(-0.0875,-0.0809297,-0.0242942,-0.906856,0.109077,0.40708,-0.453586,0.54375);
vpnt[927] = VPosNormalTex(-0.04375,-0.0741471,0.0713507,-0.906856,0.109077,0.40708,0.378058,0.521875);
vpnt[928] = VPosNormalTex(-0.04375,-0.0841471,0.0740302,-0.906856,0.109077,0.40708,0.385166,0.521875);
vpnt[929] = VPosNormalTex(-0.0875,-0.0809297,-0.0242942,-0.855958,0.365606,0.365606,-0.453586,0.54375);
vpnt[930] = VPosNormalTex(-0.0875,-0.0736092,-0.0316147,-0.855958,0.365606,0.365606,-0.435436,0.54375);
vpnt[931] = VPosNormalTex(-0.04375,-0.0668266,0.0640302,-0.855958,0.365606,0.365606,0.378401,0.521875);
vpnt[932] = VPosNormalTex(-0.04375,-0.0741471,0.0713507,-0.855958,0.365606,0.365606,0.378058,0.521875);
vpnt[933] = VPosNormalTex(-0.0875,-0.0736092,-0.0316147,-0.581931,0.785528,0.210482,-0.435436,0.54375);
vpnt[934] = VPosNormalTex(-0.0875,-0.0709297,-0.0416147,-0.581931,0.785528,0.210482,-0.415555,0.54375);
vpnt[935] = VPosNormalTex(-0.04375,-0.0641471,0.0540302,-0.581931,0.785528,0.210482,0.388592,0.521875);
vpnt[936] = VPosNormalTex(-0.04375,-0.0668266,0.0640302,-0.581931,0.785528,0.210482,0.378401,0.521875);
vpnt[937] = VPosNormalTex(-0.0875,-0.0709297,-0.0416147,0.384149,0.891811,-0.238961,-0.415555,0.54375);
vpnt[938] = VPosNormalTex(-0.0875,-0.0736092,-0.0516147,0.384149,0.891811,-0.238961,-0.402672,0.54375);
vpnt[939] = VPosNormalTex(-0.04375,-0.0668266,0.0440302,0.384149,0.891811,-0.238961,0.407278,0.521875);
vpnt[940] = VPosNormalTex(-0.04375,-0.0641471,0.0540302,0.384149,0.891811,-0.238961,0.388592,0.521875);
vpnt[941] = VPosNormalTex(-0.0875,-0.0736092,-0.0516147,0.820669,0.404044,-0.404044,-0.402672,0.54375);
vpnt[942] = VPosNormalTex(-0.0875,-0.0809297,-0.0589352,0.820669,0.404044,-0.404044,-0.399825,0.54375);
vpnt[943] = VPosNormalTex(-0.04375,-0.0741471,0.0367097,0.820669,0.404044,-0.404044,0.426834,0.521875);
vpnt[944] = VPosNormalTex(-0.04375,-0.0668266,0.0440302,0.820669,0.404044,-0.404044,0.407278,0.521875);
vpnt[945] = VPosNormalTex(-0.0875,-0.0809297,-0.0589352,0.900562,0.112516,-0.419915,-0.399825,0.54375);
vpnt[946] = VPosNormalTex(-0.0875,-0.0909297,-0.0616147,0.900562,0.112516,-0.419915,-0.405217,0.54375);
vpnt[947] = VPosNormalTex(-0.04375,-0.0841471,0.0340302,0.900562,0.112516,-0.419915,0.438836,0.521875);
vpnt[948] = VPosNormalTex(-0.04375,-0.0741471,0.0367097,0.900562,0.112516,-0.419915,0.426834,0.521875);
vpnt[949] = VPosNormalTex(-0.0875,-0.0909297,-0.0616147,0.906856,-0.109077,-0.40708,-0.405217,0.54375);
vpnt[950] = VPosNormalTex(-0.0875,-0.10093,-0.0589352,0.906856,-0.109077,-0.40708,-0.415884,0.54375);
vpnt[951] = VPosNormalTex(-0.04375,-0.0941471,0.0367097,0.906856,-0.109077,-0.40708,0.440828,0.521875);
vpnt[952] = VPosNormalTex(-0.04375,-0.0841471,0.0340302,0.906856,-0.109077,-0.40708,0.438836,0.521875);
vpnt[953] = VPosNormalTex(-0.0875,-0.10093,-0.0589352,0.855958,-0.365606,-0.365606,-0.415884,0.54375);
vpnt[954] = VPosNormalTex(-0.0875,-0.10825,-0.0516147,0.855958,-0.365606,-0.365606,-0.429188,0.54375);
vpnt[955] = VPosNormalTex(-0.04375,-0.101468,0.0440302,0.855958,-0.365606,-0.365606,0.43484,0.521875);
vpnt[956] = VPosNormalTex(-0.04375,-0.0941471,0.0367097,0.855958,-0.365606,-0.365606,0.440828,0.521875);
vpnt[957] = VPosNormalTex(-0.0875,-0.10825,-0.0516147,0.581931,-0.785528,-0.210482,-0.429188,0.54375);
vpnt[958] = VPosNormalTex(-0.0875,-0.11093,-0.0416147,0.581931,-0.785528,-0.210482,-0.44288,0.54375);
vpnt[959] = VPosNormalTex(-0.04375,-0.104147,0.0540302,0.581931,-0.785528,-0.210482,0.423834,0.521875);
vpnt[960] = VPosNormalTex(-0.04375,-0.101468,0.0440302,0.581931,-0.785528,-0.210482,0.43484,0.521875);
vpnt[961] = VPosNormalTex(-0.0875,-0.11093,-0.0416147,-0.384149,-0.891811,0.238961,-0.44288,0.54375);
vpnt[962] = VPosNormalTex(-0.0875,-0.10825,-0.0316147,-0.384149,-0.891811,0.238961,-0.454776,0.54375);
vpnt[963] = VPosNormalTex(-0.04375,-0.101468,0.0640302,-0.384149,-0.891811,0.238961,0.410407,0.521875);
vpnt[964] = VPosNormalTex(-0.04375,-0.104147,0.0540302,-0.384149,-0.891811,0.238961,0.423834,0.521875);
vpnt[965] = VPosNormalTex(-0.0875,-0.10825,-0.0316147,-0.820669,-0.404044,0.404044,-0.454776,0.54375);
vpnt[966] = VPosNormalTex(-0.0875,-0.10093,-0.0242942,-0.820669,-0.404044,0.404044,-0.462406,0.54375);
vpnt[967] = VPosNormalTex(-0.04375,-0.0941471,0.0713507,-0.820669,-0.404044,0.404044,0.396786,0.521875);
vpnt[968] = VPosNormalTex(-0.04375,-0.101468,0.0640302,-0.820669,-0.404044,0.404044,0.410407,0.521875);
vpnt[969] = VPosNormalTex(-0.0875,-0.10093,-0.0242942,-0.900562,-0.112516,0.419915,-0.462406,0.54375);
vpnt[970] = VPosNormalTex(-0.0875,-0.0909297,-0.0216147,-0.900562,-0.112516,0.419915,-0.462857,0.54375);
vpnt[971] = VPosNormalTex(-0.04375,-0.0841471,0.0740302,-0.900562,-0.112516,0.419915,0.385166,0.521875);
vpnt[972] = VPosNormalTex(-0.04375,-0.0941471,0.0713507,-0.900562,-0.112516,0.419915,0.396786,0.521875);
vpnt[973] = VPosNormalTex(-0.04375,-0.0841471,0.0740302,-0.834205,0.142727,0.532664,0.385166,0.521875);
vpnt[974] = VPosNormalTex(-0.04375,-0.0741471,0.0713507,-0.834205,0.142727,0.532664,0.378058,0.521875);
vpnt[975] = VPosNormalTex(0,0.01,0.117321,-0.834205,0.142727,0.532664,0.236467,0.5);
vpnt[976] = VPosNormalTex(0,0,0.12,-0.834205,0.142727,0.532664,0.25,0.5);
vpnt[977] = VPosNormalTex(-0.04375,-0.0741471,0.0713507,-0.903099,0.303655,0.303655,0.378058,0.521875);
vpnt[978] = VPosNormalTex(-0.04375,-0.0668266,0.0640302,-0.903099,0.303655,0.303655,0.378401,0.521875);
vpnt[979] = VPosNormalTex(0,0.0173205,0.11,-0.903099,0.303655,0.303655,0.225144,0.5);
vpnt[980] = VPosNormalTex(0,0.01,0.117321,-0.903099,0.303655,0.303655,0.236467,0.5);
vpnt[981] = VPosNormalTex(-0.04375,-0.0668266,0.0640302,-0.905186,0.410533,0.110002,0.378401,0.521875);
vpnt[982] = VPosNormalTex(-0.04375,-0.0641471,0.0540302,-0.905186,0.410533,0.110002,0.388592,0.521875);
vpnt[983] = VPosNormalTex(0,0.02,0.1,-0.905186,0.410533,0.110002,0.218584,0.5);
vpnt[984] = VPosNormalTex(0,0.0173205,0.11,-0.905186,0.410533,0.110002,0.225144,0.5);
vpnt[985] = VPosNormalTex(-0.04375,-0.0641471,0.0540302,-0.845875,0.515206,-0.138049,0.388592,0.521875);
vpnt[986] = VPosNormalTex(-0.04375,-0.0668266,0.0440302,-0.845875,0.515206,-0.138049,0.407278,0.521875);
vpnt[987] = VPosNormalTex(0,0.0173205,0.09,-0.845875,0.515206,-0.138049,0.219741,0.5);
vpnt[988] = VPosNormalTex(0,0.02,0.1,-0.845875,0.515206,-0.138049,0.218584,0.5);
vpnt[989] = VPosNormalTex(-0.04375,-0.0668266,0.0440302,-0.525118,0.601769,-0.601769,0.407278,0.521875);
vpnt[990] = VPosNormalTex(-0.04375,-0.0741471,0.0367097,-0.525118,0.601769,-0.601769,0.426834,0.521875);
vpnt[991] = VPosNormalTex(0,0.01,0.0826795,-0.525118,0.601769,-0.601769,0.230843,0.5);
vpnt[992] = VPosNormalTex(0,0.0173205,0.09,-0.525118,0.601769,-0.601769,0.219741,0.5);
vpnt[993] = VPosNormalTex(-0.04375,-0.0741471,0.0367097,0.459347,0.229898,-0.857991,0.426834,0.521875);
vpnt[994] = VPosNormalTex(-0.04375,-0.0841471,0.0340302,0.459347,0.229898,-0.857991,0.438836,0.521875);
vpnt[995] = VPosNormalTex(0,0,0.08,0.459347,0.229898,-0.857991,0.25,0.5);
vpnt[996] = VPosNormalTex(0,0.01,0.0826795,0.459347,0.229898,-0.857991,0.230843,0.5);
vpnt[997] = VPosNormalTex(-0.04375,-0.0841471,0.0340302,0.834205,-0.142727,-0.532664,0.438836,0.521875);
vpnt[998] = VPosNormalTex(-0.04375,-0.0941471,0.0367097,0.834205,-0.142727,-0.532664,0.440828,0.521875);
vpnt[999] = VPosNormalTex(0,-0.01,0.0826795,0.834205,-0.142727,-0.532664,0.269157,0.5);
vpnt[1000] = VPosNormalTex(0,0,0.08,0.834205,-0.142727,-0.532664,0.25,0.5);
vpnt[1001] = VPosNormalTex(-0.04375,-0.0941471,0.0367097,0.903099,-0.303655,-0.303655,0.440828,0.521875);
vpnt[1002] = VPosNormalTex(-0.04375,-0.101468,0.0440302,0.903099,-0.303655,-0.303655,0.43484,0.521875);
vpnt[1003] = VPosNormalTex(0,-0.0173205,0.09,0.903099,-0.303655,-0.303655,0.280259,0.5);
vpnt[1004] = VPosNormalTex(0,-0.01,0.0826795,0.903099,-0.303655,-0.303655,0.269157,0.5);
vpnt[1005] = VPosNormalTex(-0.04375,-0.101468,0.0440302,0.905186,-0.410533,-0.110002,0.43484,0.521875);
vpnt[1006] = VPosNormalTex(-0.04375,-0.104147,0.0540302,0.905186,-0.410533,-0.110002,0.423834,0.521875);
vpnt[1007] = VPosNormalTex(0,-0.02,0.1,0.905186,-0.410533,-0.110002,0.281416,0.5);
vpnt[1008] = VPosNormalTex(0,-0.0173205,0.09,0.905186,-0.410533,-0.110002,0.280259,0.5);
vpnt[1009] = VPosNormalTex(-0.04375,-0.104147,0.0540302,0.845875,-0.515206,0.138049,0.423834,0.521875);
vpnt[1010] = VPosNormalTex(-0.04375,-0.101468,0.0640302,0.845875,-0.515206,0.138049,0.410407,0.521875);
vpnt[1011] = VPosNormalTex(0,-0.0173205,0.11,0.845875,-0.515206,0.138049,0.274856,0.5);
vpnt[1012] = VPosNormalTex(0,-0.02,0.1,0.845875,-0.515206,0.138049,0.281416,0.5);
vpnt[1013] = VPosNormalTex(-0.04375,-0.101468,0.0640302,0.525118,-0.601769,0.601769,0.410407,0.521875);
vpnt[1014] = VPosNormalTex(-0.04375,-0.0941471,0.0713507,0.525118,-0.601769,0.601769,0.396786,0.521875);
vpnt[1015] = VPosNormalTex(0,-0.01,0.117321,0.525118,-0.601769,0.601769,0.263533,0.5);
vpnt[1016] = VPosNormalTex(0,-0.0173205,0.11,0.525118,-0.601769,0.601769,0.274856,0.5);
vpnt[1017] = VPosNormalTex(-0.04375,-0.0941471,0.0713507,-0.459346,-0.229898,0.857991,0.396786,0.521875);
vpnt[1018] = VPosNormalTex(-0.04375,-0.0841471,0.0740302,-0.459346,-0.229898,0.857991,0.385166,0.521875);
vpnt[1019] = VPosNormalTex(0,0,0.12,-0.459346,-0.229898,0.857991,0.25,0.5);
vpnt[1020] = VPosNormalTex(0,-0.01,0.117321,-0.459346,-0.229898,0.857991,0.263533,0.5);
vpnt[1021] = VPosNormalTex(0,0,0.12,0.459346,0.229898,0.857991,0.25,0.5);
vpnt[1022] = VPosNormalTex(0,0.01,0.117321,0.459346,0.229898,0.857991,0.236467,0.5);
vpnt[1023] = VPosNormalTex(0.04375,0.0941471,0.0713507,0.459346,0.229898,0.857991,0.103214,0.478125);
vpnt[1024] = VPosNormalTex(0.04375,0.0841471,0.0740302,0.459346,0.229898,0.857991,0.114834,0.478125);
vpnt[1025] = VPosNormalTex(0,0.01,0.117321,-0.525118,0.601769,0.601769,0.236467,0.5);
vpnt[1026] = VPosNormalTex(0,0.0173205,0.11,-0.525118,0.601769,0.601769,0.225144,0.5);
vpnt[1027] = VPosNormalTex(0.04375,0.101468,0.0640302,-0.525118,0.601769,0.601769,0.0895934,0.478125);
vpnt[1028] = VPosNormalTex(0.04375,0.0941471,0.0713507,-0.525118,0.601769,0.601769,0.103214,0.478125);
vpnt[1029] = VPosNormalTex(0,0.0173205,0.11,-0.845875,0.515206,0.138049,0.225144,0.5);
vpnt[1030] = VPosNormalTex(0,0.02,0.1,-0.845875,0.515206,0.138049,0.218584,0.5);
vpnt[1031] = VPosNormalTex(0.04375,0.104147,0.0540302,-0.845875,0.515206,0.138049,0.0761659,0.478125);
vpnt[1032] = VPosNormalTex(0.04375,0.101468,0.0640302,-0.845875,0.515206,0.138049,0.0895934,0.478125);
vpnt[1033] = VPosNormalTex(0,0.02,0.1,-0.905186,0.410533,-0.110002,0.218584,0.5);
vpnt[1034] = VPosNormalTex(0,0.0173205,0.09,-0.905186,0.410533,-0.110002,0.219741,0.5);
vpnt[1035] = VPosNormalTex(0.04375,0.101468,0.0440302,-0.905186,0.410533,-0.110002,0.0651601,0.478125);
vpnt[1036] = VPosNormalTex(0.04375,0.104147,0.0540302,-0.905186,0.410533,-0.110002,0.0761659,0.478125);
vpnt[1037] = VPosNormalTex(0,0.0173205,0.09,-0.903099,0.303655,-0.303655,0.219741,0.5);
vpnt[1038] = VPosNormalTex(0,0.01,0.0826795,-0.903099,0.303655,-0.303655,0.230843,0.5);
vpnt[1039] = VPosNormalTex(0.04375,0.0941471,0.0367097,-0.903099,0.303655,-0.303655,0.0591715,0.478125);
vpnt[1040] = VPosNormalTex(0.04375,0.101468,0.0440302,-0.903099,0.303655,-0.303655,0.0651601,0.478125);
vpnt[1041] = VPosNormalTex(0,0.01,0.0826795,-0.834205,0.142727,-0.532663,0.230843,0.5);
vpnt[1042] = VPosNormalTex(0,0,0.08,-0.834205,0.142727,-0.532663,0.25,0.5);
vpnt[1043] = VPosNormalTex(0.04375,0.0841471,0.0340302,-0.834205,0.142727,-0.532663,0.0611641,0.478125);
vpnt[1044] = VPosNormalTex(0.04375,0.0941471,0.0367097,-0.834205,0.142727,-0.532663,0.0591715,0.478125);
vpnt[1045] = VPosNormalTex(0,0,0.08,-0.459345,-0.229898,-0.857991,0.25,0.5);
vpnt[1046] = VPosNormalTex(0,-0.01,0.0826795,-0.459345,-0.229898,-0.857991,0.269157,0.5);
vpnt[1047] = VPosNormalTex(0.04375,0.0741471,0.0367097,-0.459345,-0.229898,-0.857991,0.0731658,0.478125);
vpnt[1048] = VPosNormalTex(0.04375,0.0841471,0.0340302,-0.459345,-0.229898,-0.857991,0.0611641,0.478125);
vpnt[1049] = VPosNormalTex(0,-0.01,0.0826795,0.525118,-0.601769,-0.601769,0.269157,0.5);
vpnt[1050] = VPosNormalTex(0,-0.0173205,0.09,0.525118,-0.601769,-0.601769,0.280259,0.5);
vpnt[1051] = VPosNormalTex(0.04375,0.0668266,0.0440302,0.525118,-0.601769,-0.601769,0.0927217,0.478125);
vpnt[1052] = VPosNormalTex(0.04375,0.0741471,0.0367097,0.525118,-0.601769,-0.601769,0.0731658,0.478125);
vpnt[1053] = VPosNormalTex(0,-0.0173205,0.09,0.845875,-0.515206,-0.138049,0.280259,0.5);
vpnt[1054] = VPosNormalTex(0,-0.02,0.1,0.845875,-0.515206,-0.138049,0.281416,0.5);
vpnt[1055] = VPosNormalTex(0.04375,0.0641471,0.0540302,0.845875,-0.515206,-0.138049,0.111408,0.478125);
vpnt[1056] = VPosNormalTex(0.04375,0.0668266,0.0440302,0.845875,-0.515206,-0.138049,0.0927217,0.478125);
vpnt[1057] = VPosNormalTex(0,-0.02,0.1,0.905186,-0.410533,0.110002,0.281416,0.5);
vpnt[1058] = VPosNormalTex(0,-0.0173205,0.11,0.905186,-0.410533,0.110002,0.274856,0.5);
vpnt[1059] = VPosNormalTex(0.04375,0.0668266,0.0640302,0.905186,-0.410533,0.110002,0.121599,0.478125);
vpnt[1060] = VPosNormalTex(0.04375,0.0641471,0.0540302,0.905186,-0.410533,0.110002,0.111408,0.478125);
vpnt[1061] = VPosNormalTex(0,-0.0173205,0.11,0.903099,-0.303655,0.303655,0.274856,0.5);
vpnt[1062] = VPosNormalTex(0,-0.01,0.117321,0.903099,-0.303655,0.303655,0.263533,0.5);
vpnt[1063] = VPosNormalTex(0.04375,0.0741471,0.0713507,0.903099,-0.303655,0.303655,0.121942,0.478125);
vpnt[1064] = VPosNormalTex(0.04375,0.0668266,0.0640302,0.903099,-0.303655,0.303655,0.121599,0.478125);
vpnt[1065] = VPosNormalTex(0,-0.01,0.117321,0.834205,-0.142727,0.532663,0.263533,0.5);
vpnt[1066] = VPosNormalTex(0,0,0.12,0.834205,-0.142727,0.532663,0.25,0.5);
vpnt[1067] = VPosNormalTex(0.04375,0.0841471,0.0740302,0.834205,-0.142727,0.532663,0.114834,0.478125);
vpnt[1068] = VPosNormalTex(0.04375,0.0741471,0.0713507,0.834205,-0.142727,0.532663,0.121942,0.478125);
vpnt[1069] = VPosNormalTex(0.04375,0.0841471,0.0740302,0.900562,0.112516,0.419915,0.114834,0.478125);
vpnt[1070] = VPosNormalTex(0.04375,0.0941471,0.0713507,0.900562,0.112516,0.419915,0.103214,0.478125);
vpnt[1071] = VPosNormalTex(0.0875,0.10093,-0.0242942,0.900562,0.112516,0.419915,-0.0375941,0.45625);
vpnt[1072] = VPosNormalTex(0.0875,0.0909297,-0.0216147,0.900562,0.112516,0.419915,-0.037143,0.45625);
vpnt[1073] = VPosNormalTex(0.04375,0.0941471,0.0713507,0.820669,0.404044,0.404044,0.103214,0.478125);
vpnt[1074] = VPosNormalTex(0.04375,0.101468,0.0640302,0.820669,0.404044,0.404044,0.0895934,0.478125);
vpnt[1075] = VPosNormalTex(0.0875,0.10825,-0.0316147,0.820669,0.404044,0.404044,-0.0452237,0.45625);
vpnt[1076] = VPosNormalTex(0.0875,0.10093,-0.0242942,0.820669,0.404044,0.404044,-0.0375941,0.45625);
vpnt[1077] = VPosNormalTex(0.04375,0.101468,0.0640302,0.384148,0.891812,0.23896,0.0895934,0.478125);
vpnt[1078] = VPosNormalTex(0.04375,0.104147,0.0540302,0.384148,0.891812,0.23896,0.0761659,0.478125);
vpnt[1079] = VPosNormalTex(0.0875,0.11093,-0.0416147,0.384148,0.891812,0.23896,-0.0571203,0.45625);
vpnt[1080] = VPosNormalTex(0.0875,0.10825,-0.0316147,0.384148,0.891812,0.23896,-0.0452237,0.45625);
vpnt[1081] = VPosNormalTex(0.04375,0.104147,0.0540302,-0.58193,0.785528,-0.210481,0.0761659,0.478125);
vpnt[1082] = VPosNormalTex(0.04375,0.101468,0.0440302,-0.58193,0.785528,-0.210481,0.0651601,0.478125);
vpnt[1083] = VPosNormalTex(0.0875,0.10825,-0.0516147,-0.58193,0.785528,-0.210481,-0.0708117,0.45625);
vpnt[1084] = VPosNormalTex(0.0875,0.11093,-0.0416147,-0.58193,0.785528,-0.210481,-0.0571203,0.45625);
vpnt[1085] = VPosNormalTex(0.04375,0.101468,0.0440302,-0.855958,0.365606,-0.365606,0.0651601,0.478125);
vpnt[1086] = VPosNormalTex(0.04375,0.0941471,0.0367097,-0.855958,0.365606,-0.365606,0.0591715,0.478125);
vpnt[1087] = VPosNormalTex(0.0875,0.10093,-0.0589352,-0.855958,0.365606,-0.365606,-0.0841157,0.45625);
vpnt[1088] = VPosNormalTex(0.0875,0.10825,-0.0516147,-0.855958,0.365606,-0.365606,-0.0708117,0.45625);
vpnt[1089] = VPosNormalTex(0.04375,0.0941471,0.0367097,-0.906856,0.109077,-0.40708,0.0591715,0.478125);
vpnt[1090] = VPosNormalTex(0.04375,0.0841471,0.0340302,-0.906856,0.109077,-0.40708,0.0611641,0.478125);
vpnt[1091] = VPosNormalTex(0.0875,0.0909297,-0.0616147,-0.906856,0.109077,-0.40708,-0.094783,0.45625);
vpnt[1092] = VPosNormalTex(0.0875,0.10093,-0.0589352,-0.906856,0.109077,-0.40708,-0.0841157,0.45625);
vpnt[1093] = VPosNormalTex(0.04375,0.0841471,0.0340302,-0.900562,-0.112516,-0.419915,0.0611641,0.478125);
vpnt[1094] = VPosNormalTex(0.04375,0.0741471,0.0367097,-0.900562,-0.112516,-0.419915,0.0731658,0.478125);
vpnt[1095] = VPosNormalTex(0.0875,0.0809297,-0.0589352,-0.900562,-0.112516,-0.419915,-0.100175,0.45625);
vpnt[1096] = VPosNormalTex(0.0875,0.0909297,-0.0616147,-0.900562,-0.112516,-0.419915,-0.094783,0.45625);
vpnt[1097] = VPosNormalTex(0.04375,0.0741471,0.0367097,-0.820669,-0.404044,-0.404044,0.0731658,0.478125);
vpnt[1098] = VPosNormalTex(0.04375,0.0668266,0.0440302,-0.820669,-0.404044,-0.404044,0.0927217,0.478125);
vpnt[1099] = VPosNormalTex(0.0875,0.0736092,-0.0516147,-0.820669,-0.404044,-0.404044,-0.097328,0.45625);
vpnt[1100] = VPosNormalTex(0.0875,0.0809297,-0.0589352,-0.820669,-0.404044,-0.404044,-0.100175,0.45625);
vpnt[1101] = VPosNormalTex(0.04375,0.0668266,0.0440302,-0.384148,-0.891812,-0.23896,0.0927217,0.478125);
vpnt[1102] = VPosNormalTex(0.04375,0.0641471,0.0540302,-0.384148,-0.891812,-0.23896,0.111408,0.478125);
vpnt[1103] = VPosNormalTex(0.0875,0.0709297,-0.0416147,-0.384148,-0.891812,-0.23896,-0.0844452,0.45625);
vpnt[1104] = VPosNormalTex(0.0875,0.0736092,-0.0516147,-0.384148,-0.891812,-0.23896,-0.097328,0.45625);
vpnt[1105] = VPosNormalTex(0.04375,0.0641471,0.0540302,0.58193,-0.785528,0.210482,0.111408,0.478125);
vpnt[1106] = VPosNormalTex(0.04375,0.0668266,0.0640302,0.58193,-0.785528,0.210482,0.121599,0.478125);
vpnt[1107] = VPosNormalTex(0.0875,0.0736092,-0.0316147,0.58193,-0.785528,0.210482,-0.0645645,0.45625);
vpnt[1108] = VPosNormalTex(0.0875,0.0709297,-0.0416147,0.58193,-0.785528,0.210482,-0.0844452,0.45625);
vpnt[1109] = VPosNormalTex(0.04375,0.0668266,0.0640302,0.855958,-0.365606,0.365606,0.121599,0.478125);
vpnt[1110] = VPosNormalTex(0.04375,0.0741471,0.0713507,0.855958,-0.365606,0.365606,0.121942,0.478125);
vpnt[1111] = VPosNormalTex(0.0875,0.0809297,-0.0242942,0.855958,-0.365606,0.365606,-0.0464143,0.45625);
vpnt[1112] = VPosNormalTex(0.0875,0.0736092,-0.0316147,0.855958,-0.365606,0.365606,-0.0645645,0.45625);
vpnt[1113] = VPosNormalTex(0.04375,0.0741471,0.0713507,0.906856,-0.109077,0.40708,0.121942,0.478125);
vpnt[1114] = VPosNormalTex(0.04375,0.0841471,0.0740302,0.906856,-0.109077,0.40708,0.114834,0.478125);
vpnt[1115] = VPosNormalTex(0.0875,0.0909297,-0.0216147,0.906856,-0.109077,0.40708,-0.037143,0.45625);
vpnt[1116] = VPosNormalTex(0.0875,0.0809297,-0.0242942,0.906856,-0.109077,0.40708,-0.0464143,0.45625);
vpnt[1117] = VPosNormalTex(0.0875,0.0909297,-0.0216147,0.864684,0.130009,0.4852,-0.037143,0.45625);
vpnt[1118] = VPosNormalTex(0.0875,0.10093,-0.0242942,0.864684,0.130009,0.4852,-0.0375941,0.45625);
vpnt[1119] = VPosNormalTex(0.13125,0.024112,-0.0816787,0.864684,0.130009,0.4852,-0.204314,0.434375);
vpnt[1120] = VPosNormalTex(0.13125,0.014112,-0.0789993,0.864684,0.130009,0.4852,-0.221866,0.434375);
vpnt[1121] = VPosNormalTex(0.0875,0.10093,-0.0242942,0.908134,0.296052,0.296052,-0.0375941,0.45625);
vpnt[1122] = VPosNormalTex(0.0875,0.10825,-0.0316147,0.908134,0.296052,0.296052,-0.0452237,0.45625);
vpnt[1123] = VPosNormalTex(0.13125,0.0314325,-0.0889993,0.908134,0.296052,0.296052,-0.195967,0.434375);
vpnt[1124] = VPosNormalTex(0.13125,0.024112,-0.0816787,0.908134,0.296052,0.296052,-0.204314,0.434375);
vpnt[1125] = VPosNormalTex(0.0875,0.10825,-0.0316147,0.897535,0.425919,0.114125,-0.0452237,0.45625);
vpnt[1126] = VPosNormalTex(0.0875,0.11093,-0.0416147,0.897535,0.425919,0.114125,-0.0571203,0.45625);
vpnt[1127] = VPosNormalTex(0.13125,0.034112,-0.0989993,0.897535,0.425919,0.114125,-0.197188,0.434375);
vpnt[1128] = VPosNormalTex(0.13125,0.0314325,-0.0889993,0.897535,0.425919,0.114125,-0.195967,0.434375);
vpnt[1129] = VPosNormalTex(0.0875,0.11093,-0.0416147,0.804927,0.573155,-0.153577,-0.0571203,0.45625);
vpnt[1130] = VPosNormalTex(0.0875,0.10825,-0.0516147,0.804927,0.573155,-0.153577,-0.0708117,0.45625);
vpnt[1131] = VPosNormalTex(0.13125,0.0314325,-0.108999,0.804927,0.573155,-0.153577,-0.205316,0.434375);
vpnt[1132] = VPosNormalTex(0.13125,0.034112,-0.0989993,0.804927,0.573155,-0.153577,-0.197188,0.434375);
vpnt[1133] = VPosNormalTex(0.0875,0.10825,-0.0516147,0.299655,0.674614,-0.674614,-0.0708117,0.45625);
vpnt[1134] = VPosNormalTex(0.0875,0.10093,-0.0589352,0.299655,0.674614,-0.674614,-0.0841157,0.45625);
vpnt[1135] = VPosNormalTex(0.13125,0.024112,-0.11632,0.299655,0.674614,-0.674614,-0.217469,0.434375);
vpnt[1136] = VPosNormalTex(0.13125,0.0314325,-0.108999,0.299655,0.674614,-0.674614,-0.205316,0.434375);
vpnt[1137] = VPosNormalTex(0.0875,0.10093,-0.0589352,-0.630598,0.200872,-0.749665,-0.0841157,0.45625);
vpnt[1138] = VPosNormalTex(0.0875,0.0909297,-0.0616147,-0.630598,0.200872,-0.749665,-0.094783,0.45625);
vpnt[1139] = VPosNormalTex(0.13125,0.014112,-0.118999,-0.630598,0.200872,-0.749665,-0.231214,0.434375);
vpnt[1140] = VPosNormalTex(0.13125,0.024112,-0.11632,-0.630598,0.200872,-0.749665,-0.217469,0.434375);
vpnt[1141] = VPosNormalTex(0.0875,0.0909297,-0.0616147,-0.864684,-0.130009,-0.4852,-0.094783,0.45625);
vpnt[1142] = VPosNormalTex(0.0875,0.0809297,-0.0589352,-0.864684,-0.130009,-0.4852,-0.100175,0.45625);
vpnt[1143] = VPosNormalTex(0.13125,0.00411199,-0.11632,-0.864684,-0.130009,-0.4852,-0.244376,0.434375);
vpnt[1144] = VPosNormalTex(0.13125,0.014112,-0.118999,-0.864684,-0.130009,-0.4852,-0.231214,0.434375);
vpnt[1145] = VPosNormalTex(0.0875,0.0809297,-0.0589352,-0.908133,-0.296052,-0.296052,-0.100175,0.45625);
vpnt[1146] = VPosNormalTex(0.0875,0.0736092,-0.0516147,-0.908133,-0.296052,-0.296052,-0.097328,0.45625);
vpnt[1147] = VPosNormalTex(0.13125,-0.00320852,-0.108999,-0.908133,-0.296052,-0.296052,-0.254684,0.434375);
vpnt[1148] = VPosNormalTex(0.13125,0.00411199,-0.11632,-0.908133,-0.296052,-0.296052,-0.244376,0.434375);
vpnt[1149] = VPosNormalTex(0.0875,0.0736092,-0.0516147,-0.897535,-0.425919,-0.114125,-0.097328,0.45625);
vpnt[1150] = VPosNormalTex(0.0875,0.0709297,-0.0416147,-0.897535,-0.425919,-0.114125,-0.0844452,0.45625);
vpnt[1151] = VPosNormalTex(0.13125,-0.00588801,-0.0989993,-0.897535,-0.425919,-0.114125,-0.259455,0.434375);
vpnt[1152] = VPosNormalTex(0.13125,-0.00320852,-0.108999,-0.897535,-0.425919,-0.114125,-0.254684,0.434375);
vpnt[1153] = VPosNormalTex(0.0875,0.0709297,-0.0416147,-0.804927,-0.573155,0.153577,-0.0844452,0.45625);
vpnt[1154] = VPosNormalTex(0.0875,0.0736092,-0.0316147,-0.804927,-0.573155,0.153577,-0.0645645,0.45625);
vpnt[1155] = VPosNormalTex(0.13125,-0.00320852,-0.0889993,-0.804927,-0.573155,0.153577,-0.255735,0.434375);
vpnt[1156] = VPosNormalTex(0.13125,-0.00588801,-0.0989993,-0.804927,-0.573155,0.153577,-0.259455,0.434375);
vpnt[1157] = VPosNormalTex(0.0875,0.0736092,-0.0316147,-0.299654,-0.674614,0.674614,-0.0645645,0.45625);
vpnt[1158] = VPosNormalTex(0.0875,0.0809297,-0.0242942,-0.299654,-0.674614,0.674614,-0.0464143,0.45625);
vpnt[1159] = VPosNormalTex(0.13125,0.00411199,-0.0816787,-0.299654,-0.674614,0.674614,-0.241994,0.434375);
vpnt[1160] = VPosNormalTex(0.13125,-0.00320852,-0.0889993,-0.299654,-0.674614,0.674614,-0.255735,0.434375);
vpnt[1161] = VPosNormalTex(0.0875,0.0809297,-0.0242942,0.630598,-0.200872,0.749665,-0.0464143,0.45625);
vpnt[1162] = VPosNormalTex(0.0875,0.0909297,-0.0216147,0.630598,-0.200872,0.749665,-0.037143,0.45625);
vpnt[1163] = VPosNormalTex(0.13125,0.014112,-0.0789993,0.630598,-0.200872,0.749665,-0.221866,0.434375);
vpnt[1164] = VPosNormalTex(0.13125,0.00411199,-0.0816787,0.630598,-0.200872,0.749665,-0.241994,0.434375);
vpnt[1165] = VPosNormalTex(0.13125,0.014112,-0.0789993,-0.206831,0.253223,0.945039,-0.221866,0.434375);
vpnt[1166] = VPosNormalTex(0.13125,0.024112,-0.0816787,-0.206831,0.253223,0.945039,-0.204314,0.434375);
vpnt[1167] = VPosNormalTex(0.175,-0.0656803,-0.0480439,-0.206831,0.253223,0.945039,-0.399487,0.4125);
vpnt[1168] = VPosNormalTex(0.175,-0.0756803,-0.0453644,-0.206831,0.253223,0.945039,-0.414057,0.4125);
vpnt[1169] = VPosNormalTex(0.13125,0.024112,-0.0816787,0.672084,0.523595,0.523595,-0.204314,0.434375);
vpnt[1170] = VPosNormalTex(0.13125,0.0314325,-0.0889993,0.672084,0.523595,0.523595,-0.195967,0.434375);
vpnt[1171] = VPosNormalTex(0.175,-0.0583597,-0.0553644,0.672084,0.523595,0.523595,-0.379191,0.4125);
vpnt[1172] = VPosNormalTex(0.175,-0.0656803,-0.0480439,0.672084,0.523595,0.523595,-0.399487,0.4125);
vpnt[1173] = VPosNormalTex(0.13125,0.0314325,-0.0889993,0.872245,0.472404,0.12658,-0.195967,0.434375);
vpnt[1174] = VPosNormalTex(0.13125,0.034112,-0.0989993,0.872245,0.472404,0.12658,-0.197188,0.434375);
vpnt[1175] = VPosNormalTex(0.175,-0.0556803,-0.0653644,0.872245,0.472404,0.12658,-0.362294,0.4125);
vpnt[1176] = VPosNormalTex(0.175,-0.0583597,-0.0553644,0.872245,0.472404,0.12658,-0.379191,0.4125);
vpnt[1177] = VPosNormalTex(0.13125,0.034112,-0.0989993,0.909038,0.402515,-0.107854,-0.197188,0.434375);
vpnt[1178] = VPosNormalTex(0.13125,0.0314325,-0.108999,0.909038,0.402515,-0.107854,-0.205316,0.434375);
vpnt[1179] = VPosNormalTex(0.175,-0.0583597,-0.0753644,0.909038,0.402515,-0.107854,-0.35487,0.4125);
vpnt[1180] = VPosNormalTex(0.175,-0.0556803,-0.0653644,0.909038,0.402515,-0.107854,-0.362294,0.4125);
vpnt[1181] = VPosNormalTex(0.13125,0.0314325,-0.108999,0.893968,0.316876,-0.316876,-0.205316,0.434375);
vpnt[1182] = VPosNormalTex(0.13125,0.024112,-0.11632,0.893968,0.316876,-0.316876,-0.217469,0.434375);
vpnt[1183] = VPosNormalTex(0.175,-0.0656803,-0.0826849,0.893968,0.316876,-0.316876,-0.356838,0.4125);
vpnt[1184] = VPosNormalTex(0.175,-0.0583597,-0.0753644,0.893968,0.316876,-0.316876,-0.35487,0.4125);
vpnt[1185] = VPosNormalTex(0.13125,0.024112,-0.11632,0.786571,0.159821,-0.596458,-0.217469,0.434375);
vpnt[1186] = VPosNormalTex(0.13125,0.014112,-0.118999,0.786571,0.159821,-0.596458,-0.231214,0.434375);
vpnt[1187] = VPosNormalTex(0.175,-0.0756803,-0.0853644,0.786571,0.159821,-0.596458,-0.365441,0.4125);
vpnt[1188] = VPosNormalTex(0.175,-0.0656803,-0.0826849,0.786571,0.159821,-0.596458,-0.356838,0.4125);
vpnt[1189] = VPosNormalTex(0.13125,0.014112,-0.118999,0.206831,-0.253223,-0.945039,-0.231214,0.434375);
vpnt[1190] = VPosNormalTex(0.13125,0.00411199,-0.11632,0.206831,-0.253223,-0.945039,-0.244376,0.434375);
vpnt[1191] = VPosNormalTex(0.175,-0.0856803,-0.0826849,0.206831,-0.253223,-0.945039,-0.377831,0.4125);
vpnt[1192] = VPosNormalTex(0.175,-0.0756803,-0.0853644,0.206831,-0.253223,-0.945039,-0.365441,0.4125);
vpnt[1193] = VPosNormalTex(0.13125,0.00411199,-0.11632,-0.672084,-0.523595,-0.523595,-0.244376,0.434375);
vpnt[1194] = VPosNormalTex(0.13125,-0.00320852,-0.108999,-0.672084,-0.523595,-0.523595,-0.254684,0.434375);
vpnt[1195] = VPosNormalTex(0.175,-0.0930008,-0.0753644,-0.672084,-0.523595,-0.523595,-0.391611,0.4125);
vpnt[1196] = VPosNormalTex(0.175,-0.0856803,-0.0826849,-0.672084,-0.523595,-0.523595,-0.377831,0.4125);
vpnt[1197] = VPosNormalTex(0.13125,-0.00320852,-0.108999,-0.872245,-0.472404,-0.12658,-0.254684,0.434375);
vpnt[1198] = VPosNormalTex(0.13125,-0.00588801,-0.0989993,-0.872245,-0.472404,-0.12658,-0.259455,0.434375);
vpnt[1199] = VPosNormalTex(0.175,-0.0956803,-0.0653644,-0.872245,-0.472404,-0.12658,-0.404614,0.4125);
vpnt[1200] = VPosNormalTex(0.175,-0.0930008,-0.0753644,-0.872245,-0.472404,-0.12658,-0.391611,0.4125);
vpnt[1201] = VPosNormalTex(0.13125,-0.00588801,-0.0989993,-0.909038,-0.402515,0.107854,-0.259455,0.434375);
vpnt[1202] = VPosNormalTex(0.13125,-0.00320852,-0.0889993,-0.909038,-0.402515,0.107854,-0.255735,0.434375);
vpnt[1203] = VPosNormalTex(0.175,-0.0930008,-0.0553644,-0.909038,-0.402515,0.107854,-0.414539,0.4125);
vpnt[1204] = VPosNormalTex(0.175,-0.0956803,-0.0653644,-0.909038,-0.402515,0.107854,-0.404614,0.4125);
vpnt[1205] = VPosNormalTex(0.13125,-0.00320852,-0.0889993,-0.893968,-0.316876,0.316876,-0.255735,0.434375);
vpnt[1206] = VPosNormalTex(0.13125,0.00411199,-0.0816787,-0.893968,-0.316876,0.316876,-0.241994,0.434375);
vpnt[1207] = VPosNormalTex(0.175,-0.0856803,-0.0480439,-0.893968,-0.316876,0.316876,-0.418664,0.4125);
vpnt[1208] = VPosNormalTex(0.175,-0.0930008,-0.0553644,-0.893968,-0.316876,0.316876,-0.414539,0.4125);
vpnt[1209] = VPosNormalTex(0.13125,0.00411199,-0.0816787,-0.786571,-0.159821,0.596458,-0.241994,0.434375);
vpnt[1210] = VPosNormalTex(0.13125,0.014112,-0.0789993,-0.786571,-0.159821,0.596458,-0.221866,0.434375);
vpnt[1211] = VPosNormalTex(0.175,-0.0756803,-0.0453644,-0.786571,-0.159821,0.596458,-0.414057,0.4125);
vpnt[1212] = VPosNormalTex(0.175,-0.0856803,-0.0480439,-0.786571,-0.159821,0.596458,-0.418664,0.4125);
vpnt[1213] = VPosNormalTex(0.175,-0.0756803,-0.0453644,-0.889802,0.118111,0.440797,-0.414057,0.4125);
vpnt[1214] = VPosNormalTex(0.175,-0.0656803,-0.0480439,-0.889802,0.118111,0.440797,-0.399487,0.4125);
vpnt[1215] = VPosNormalTex(0.21875,-0.0858924,0.0456867,-0.889802,0.118111,0.440797,0.422198,0.390625);
vpnt[1216] = VPosNormalTex(0.21875,-0.0958924,0.0483662,-0.889802,0.118111,0.440797,0.425651,0.390625);
vpnt[1217] = VPosNormalTex(0.175,-0.0656803,-0.0480439,-0.765108,0.455307,0.455307,-0.399487,0.4125);
vpnt[1218] = VPosNormalTex(0.175,-0.0583597,-0.0553644,-0.765108,0.455307,0.455307,-0.379191,0.4125);
vpnt[1219] = VPosNormalTex(0.21875,-0.0785719,0.0383662,-0.765108,0.455307,0.455307,0.427706,0.390625);
vpnt[1220] = VPosNormalTex(0.21875,-0.0858924,0.0456867,-0.765108,0.455307,0.455307,0.422198,0.390625);
vpnt[1221] = VPosNormalTex(0.175,-0.0583597,-0.0553644,-0.107619,0.960316,0.257316,-0.379191,0.4125);
vpnt[1222] = VPosNormalTex(0.175,-0.0556803,-0.0653644,-0.107619,0.960316,0.257316,-0.362294,0.4125);
vpnt[1223] = VPosNormalTex(0.21875,-0.0758924,0.0283662,-0.107619,0.960316,0.257316,0.443072,0.390625);
vpnt[1224] = VPosNormalTex(0.21875,-0.0785719,0.0383662,-0.107619,0.960316,0.257316,0.427706,0.390625);
vpnt[1225] = VPosNormalTex(0.175,-0.0556803,-0.0653644,0.707371,0.682757,-0.182944,-0.362294,0.4125);
vpnt[1226] = VPosNormalTex(0.175,-0.0583597,-0.0753644,0.707371,0.682757,-0.182944,-0.35487,0.4125);
vpnt[1227] = VPosNormalTex(0.21875,-0.0785719,0.0183662,0.707371,0.682757,-0.182944,0.463454,0.390625);
vpnt[1228] = VPosNormalTex(0.21875,-0.0758924,0.0283662,0.707371,0.682757,-0.182944,0.443072,0.390625);
vpnt[1229] = VPosNormalTex(0.175,-0.0583597,-0.0753644,0.878798,0.337427,-0.337427,-0.35487,0.4125);
vpnt[1230] = VPosNormalTex(0.175,-0.0656803,-0.0826849,0.878798,0.337427,-0.337427,-0.356838,0.4125);
vpnt[1231] = VPosNormalTex(0.21875,-0.0858924,0.0110457,0.878798,0.337427,-0.337427,0.479644,0.390625);
vpnt[1232] = VPosNormalTex(0.21875,-0.0785719,0.0183662,0.878798,0.337427,-0.337427,0.463454,0.390625);
vpnt[1233] = VPosNormalTex(0.175,-0.0656803,-0.0826849,0.909581,0.107546,-0.401368,-0.356838,0.4125);
vpnt[1234] = VPosNormalTex(0.175,-0.0756803,-0.0853644,0.909581,0.107546,-0.401368,-0.365441,0.4125);
vpnt[1235] = VPosNormalTex(0.21875,-0.0958924,0.00836622,0.909581,0.107546,-0.401368,0.486149,0.390625);
vpnt[1236] = VPosNormalTex(0.21875,-0.0858924,0.0110457,0.909581,0.107546,-0.401368,0.479644,0.390625);
vpnt[1237] = VPosNormalTex(0.175,-0.0756803,-0.0853644,0.889802,-0.118111,-0.440797,-0.365441,0.4125);
vpnt[1238] = VPosNormalTex(0.175,-0.0856803,-0.0826849,0.889802,-0.118111,-0.440797,-0.377831,0.4125);
vpnt[1239] = VPosNormalTex(0.21875,-0.105892,0.0110457,0.889802,-0.118111,-0.440797,0.483458,0.390625);
vpnt[1240] = VPosNormalTex(0.21875,-0.0958924,0.00836622,0.889802,-0.118111,-0.440797,0.486149,0.390625);
vpnt[1241] = VPosNormalTex(0.175,-0.0856803,-0.0826849,0.765108,-0.455307,-0.455307,-0.377831,0.4125);
vpnt[1242] = VPosNormalTex(0.175,-0.0930008,-0.0753644,0.765108,-0.455307,-0.455307,-0.391611,0.4125);
vpnt[1243] = VPosNormalTex(0.21875,-0.113213,0.0183662,0.765108,-0.455307,-0.455307,0.474404,0.390625);
vpnt[1244] = VPosNormalTex(0.21875,-0.105892,0.0110457,0.765108,-0.455307,-0.455307,0.483458,0.390625);
vpnt[1245] = VPosNormalTex(0.175,-0.0930008,-0.0753644,0.107618,-0.960316,-0.257316,-0.391611,0.4125);
vpnt[1246] = VPosNormalTex(0.175,-0.0956803,-0.0653644,0.107618,-0.960316,-0.257316,-0.404614,0.4125);
vpnt[1247] = VPosNormalTex(0.21875,-0.115892,0.0283662,0.107618,-0.960316,-0.257316,0.461796,0.390625);
vpnt[1248] = VPosNormalTex(0.21875,-0.113213,0.0183662,0.107618,-0.960316,-0.257316,0.474404,0.390625);
vpnt[1249] = VPosNormalTex(0.175,-0.0956803,-0.0653644,-0.707371,-0.682757,0.182944,-0.404614,0.4125);
vpnt[1250] = VPosNormalTex(0.175,-0.0930008,-0.0553644,-0.707371,-0.682757,0.182944,-0.414539,0.4125);
vpnt[1251] = VPosNormalTex(0.21875,-0.113213,0.0383662,-0.707371,-0.682757,0.182944,0.447998,0.390625);
vpnt[1252] = VPosNormalTex(0.21875,-0.115892,0.0283662,-0.707371,-0.682757,0.182944,0.461796,0.390625);
vpnt[1253] = VPosNormalTex(0.175,-0.0930008,-0.0553644,-0.878798,-0.337427,0.337427,-0.414539,0.4125);
vpnt[1254] = VPosNormalTex(0.175,-0.0856803,-0.0480439,-0.878798,-0.337427,0.337427,-0.418664,0.4125);
vpnt[1255] = VPosNormalTex(0.21875,-0.105892,0.0456867,-0.878798,-0.337427,0.337427,0.435174,0.390625);
vpnt[1256] = VPosNormalTex(0.21875,-0.113213,0.0383662,-0.878798,-0.337427,0.337427,0.447998,0.390625);
vpnt[1257] = VPosNormalTex(0.175,-0.0856803,-0.0480439,-0.909581,-0.107546,0.401368,-0.418664,0.4125);
vpnt[1258] = VPosNormalTex(0.175,-0.0756803,-0.0453644,-0.909581,-0.107546,0.401368,-0.414057,0.4125);
vpnt[1259] = VPosNormalTex(0.21875,-0.0958924,0.0483662,-0.909581,-0.107546,0.401368,0.425651,0.390625);
vpnt[1260] = VPosNormalTex(0.21875,-0.105892,0.0456867,-0.909581,-0.107546,0.401368,0.435174,0.390625);
vpnt[1261] = VPosNormalTex(0.21875,-0.0958924,0.0483662,-0.884473,0.120763,0.450693,0.425651,0.390625);
vpnt[1262] = VPosNormalTex(0.21875,-0.0858924,0.0456867,-0.884473,0.120763,0.450693,0.422198,0.390625);
vpnt[1263] = VPosNormalTex(0.2625,-0.0179415,0.113338,-0.884473,0.120763,0.450693,0.274987,0.36875);
vpnt[1264] = VPosNormalTex(0.2625,-0.0279415,0.116017,-0.884473,0.120763,0.450693,0.287615,0.36875);
vpnt[1265] = VPosNormalTex(0.21875,-0.0858924,0.0456867,-0.909772,0.293525,0.293525,0.422198,0.390625);
vpnt[1266] = VPosNormalTex(0.21875,-0.0785719,0.0383662,-0.909772,0.293525,0.293525,0.427706,0.390625);
vpnt[1267] = VPosNormalTex(0.2625,-0.010621,0.106017,-0.909772,0.293525,0.293525,0.265891,0.36875);
vpnt[1268] = VPosNormalTex(0.2625,-0.0179415,0.113338,-0.909772,0.293525,0.293525,0.274987,0.36875);
vpnt[1269] = VPosNormalTex(0.21875,-0.0785719,0.0383662,-0.884964,0.449793,0.120521,0.427706,0.390625);
vpnt[1270] = VPosNormalTex(0.21875,-0.0758924,0.0283662,-0.884964,0.449793,0.120521,0.443072,0.390625);
vpnt[1271] = VPosNormalTex(0.2625,-0.00794153,0.096017,-0.884964,0.449793,0.120521,0.263134,0.36875);
vpnt[1272] = VPosNormalTex(0.2625,-0.010621,0.106017,-0.884964,0.449793,0.120521,0.265891,0.36875);
vpnt[1273] = VPosNormalTex(0.21875,-0.0758924,0.0283662,-0.739949,0.649743,-0.174098,0.443072,0.390625);
vpnt[1274] = VPosNormalTex(0.21875,-0.0785719,0.0183662,-0.739949,0.649743,-0.174098,0.463454,0.390625);
vpnt[1275] = VPosNormalTex(0.2625,-0.010621,0.086017,-0.739949,0.649743,-0.174098,0.269553,0.36875);
vpnt[1276] = VPosNormalTex(0.2625,-0.00794153,0.096017,-0.739949,0.649743,-0.174098,0.263134,0.36875);
vpnt[1277] = VPosNormalTex(0.21875,-0.0785719,0.0183662,-0.00485008,0.707098,-0.707098,0.463454,0.390625);
vpnt[1278] = VPosNormalTex(0.21875,-0.0858924,0.0110457,-0.00485008,0.707098,-0.707098,0.479644,0.390625);
vpnt[1279] = VPosNormalTex(0.2625,-0.0179415,0.0786965,-0.00485008,0.707098,-0.707098,0.285675,0.36875);
vpnt[1280] = VPosNormalTex(0.2625,-0.010621,0.086017,-0.00485008,0.707098,-0.707098,0.269553,0.36875);
vpnt[1281] = VPosNormalTex(0.21875,-0.0858924,0.0110457,0.737375,0.174828,-0.652467,0.479644,0.390625);
vpnt[1282] = VPosNormalTex(0.21875,-0.0958924,0.00836622,0.737375,0.174828,-0.652467,0.486149,0.390625);
vpnt[1283] = VPosNormalTex(0.2625,-0.0279415,0.076017,0.737375,0.174828,-0.652467,0.306061,0.36875);
vpnt[1284] = VPosNormalTex(0.2625,-0.0179415,0.0786965,0.737375,0.174828,-0.652467,0.285675,0.36875);
vpnt[1285] = VPosNormalTex(0.21875,-0.0958924,0.00836622,0.884473,-0.120763,-0.450693,0.486149,0.390625);
vpnt[1286] = VPosNormalTex(0.21875,-0.105892,0.0110457,0.884473,-0.120763,-0.450693,0.483458,0.390625);
vpnt[1287] = VPosNormalTex(0.2625,-0.0379415,0.0786965,0.884473,-0.120763,-0.450693,0.3215,0.36875);
vpnt[1288] = VPosNormalTex(0.2625,-0.0279415,0.076017,0.884473,-0.120763,-0.450693,0.306061,0.36875);
vpnt[1289] = VPosNormalTex(0.21875,-0.105892,0.0110457,0.909772,-0.293525,-0.293525,0.483458,0.390625);
vpnt[1290] = VPosNormalTex(0.21875,-0.113213,0.0183662,0.909772,-0.293525,-0.293525,0.474404,0.390625);
vpnt[1291] = VPosNormalTex(0.2625,-0.045262,0.086017,0.909772,-0.293525,-0.293525,0.327092,0.36875);
vpnt[1292] = VPosNormalTex(0.2625,-0.0379415,0.0786965,0.909772,-0.293525,-0.293525,0.3215,0.36875);
vpnt[1293] = VPosNormalTex(0.21875,-0.113213,0.0183662,0.884964,-0.449793,-0.120521,0.474404,0.390625);
vpnt[1294] = VPosNormalTex(0.21875,-0.115892,0.0283662,0.884964,-0.449793,-0.120521,0.461796,0.390625);
vpnt[1295] = VPosNormalTex(0.2625,-0.0479415,0.096017,0.884964,-0.449793,-0.120521,0.323703,0.36875);
vpnt[1296] = VPosNormalTex(0.2625,-0.045262,0.086017,0.884964,-0.449793,-0.120521,0.327092,0.36875);
vpnt[1297] = VPosNormalTex(0.21875,-0.115892,0.0283662,0.739949,-0.649743,0.174098,0.461796,0.390625);
vpnt[1298] = VPosNormalTex(0.21875,-0.113213,0.0383662,0.739949,-0.649743,0.174098,0.447998,0.390625);
vpnt[1299] = VPosNormalTex(0.2625,-0.045262,0.106017,0.739949,-0.649743,0.174098,0.31422,0.36875);
vpnt[1300] = VPosNormalTex(0.2625,-0.0479415,0.096017,0.739949,-0.649743,0.174098,0.323703,0.36875);
vpnt[1301] = VPosNormalTex(0.21875,-0.113213,0.0383662,0.0048502,-0.707098,0.707098,0.447998,0.390625);
vpnt[1302] = VPosNormalTex(0.21875,-0.105892,0.0456867,0.0048502,-0.707098,0.707098,0.435174,0.390625);
vpnt[1303] = VPosNormalTex(0.2625,-0.0379415,0.113338,0.0048502,-0.707098,0.707098,0.301413,0.36875);
vpnt[1304] = VPosNormalTex(0.2625,-0.045262,0.106017,0.0048502,-0.707098,0.707098,0.31422,0.36875);
vpnt[1305] = VPosNormalTex(0.21875,-0.105892,0.0456867,-0.737375,-0.174828,0.652467,0.435174,0.390625);
vpnt[1306] = VPosNormalTex(0.21875,-0.0958924,0.0483662,-0.737375,-0.174828,0.652467,0.425651,0.390625);
vpnt[1307] = VPosNormalTex(0.2625,-0.0279415,0.116017,-0.737375,-0.174828,0.652467,0.287615,0.36875);
vpnt[1308] = VPosNormalTex(0.2625,-0.0379415,0.113338,-0.737375,-0.174828,0.652467,0.301413,0.36875);
vpnt[1309] = VPosNormalTex(0.2625,-0.0279415,0.116017,-0.0980822,0.257571,0.961268,0.287615,0.36875);
vpnt[1310] = VPosNormalTex(0.2625,-0.0179415,0.113338,-0.0980822,0.257571,0.961268,0.274987,0.36875);
vpnt[1311] = VPosNormalTex(0.30625,0.0756987,0.0927107,-0.0980822,0.257571,0.961268,0.141023,0.346875);
vpnt[1312] = VPosNormalTex(0.30625,0.0656987,0.0953902,-0.0980822,0.257571,0.961268,0.15401,0.346875);
vpnt[1313] = VPosNormalTex(0.2625,-0.0179415,0.113338,-0.762916,0.457143,0.457143,0.274987,0.36875);
vpnt[1314] = VPosNormalTex(0.2625,-0.010621,0.106017,-0.762916,0.457143,0.457143,0.265891,0.36875);
vpnt[1315] = VPosNormalTex(0.30625,0.0830192,0.0853902,-0.762916,0.457143,0.457143,0.127241,0.346875);
vpnt[1316] = VPosNormalTex(0.30625,0.0756987,0.0927107,-0.762916,0.457143,0.457143,0.141023,0.346875);
vpnt[1317] = VPosNormalTex(0.2625,-0.010621,0.106017,-0.889378,0.441594,0.118325,0.265891,0.36875);
vpnt[1318] = VPosNormalTex(0.2625,-0.00794153,0.096017,-0.889378,0.441594,0.118325,0.263134,0.36875);
vpnt[1319] = VPosNormalTex(0.30625,0.0856986,0.0753902,-0.889378,0.441594,0.118325,0.114829,0.346875);
vpnt[1320] = VPosNormalTex(0.30625,0.0830192,0.0853902,-0.889378,0.441594,0.118325,0.127241,0.346875);
vpnt[1321] = VPosNormalTex(0.2625,-0.00794153,0.096017,-0.909614,0.401298,-0.107528,0.263134,0.36875);
vpnt[1322] = VPosNormalTex(0.2625,-0.010621,0.086017,-0.909614,0.401298,-0.107528,0.269553,0.36875);
vpnt[1323] = VPosNormalTex(0.30625,0.0830192,0.0653902,-0.909614,0.401298,-0.107528,0.106183,0.346875);
vpnt[1324] = VPosNormalTex(0.30625,0.0856986,0.0753902,-0.909614,0.401298,-0.107528,0.114829,0.346875);
vpnt[1325] = VPosNormalTex(0.2625,-0.010621,0.086017,-0.879365,0.336687,-0.336687,0.269553,0.36875);
vpnt[1326] = VPosNormalTex(0.2625,-0.0179415,0.0786965,-0.879365,0.336687,-0.336687,0.285675,0.36875);
vpnt[1327] = VPosNormalTex(0.30625,0.0756987,0.0580697,-0.879365,0.336687,-0.336687,0.104146,0.346875);
vpnt[1328] = VPosNormalTex(0.30625,0.0830192,0.0653902,-0.879365,0.336687,-0.336687,0.106183,0.346875);
vpnt[1329] = VPosNormalTex(0.2625,-0.0179415,0.0786965,-0.710395,0.182157,-0.679821,0.285675,0.36875);
vpnt[1330] = VPosNormalTex(0.2625,-0.0279415,0.076017,-0.710395,0.182157,-0.679821,0.306061,0.36875);
vpnt[1331] = VPosNormalTex(0.30625,0.0656987,0.0553902,-0.710395,0.182157,-0.679821,0.111484,0.346875);
vpnt[1332] = VPosNormalTex(0.30625,0.0756987,0.0580697,-0.710395,0.182157,-0.679821,0.104146,0.346875);
vpnt[1333] = VPosNormalTex(0.2625,-0.0279415,0.076017,0.0980821,-0.257571,-0.961268,0.306061,0.36875);
vpnt[1334] = VPosNormalTex(0.2625,-0.0379415,0.0786965,0.0980821,-0.257571,-0.961268,0.3215,0.36875);
vpnt[1335] = VPosNormalTex(0.30625,0.0556987,0.0580697,0.0980821,-0.257571,-0.961268,0.128317,0.346875);
vpnt[1336] = VPosNormalTex(0.30625,0.0656987,0.0553902,0.0980821,-0.257571,-0.961268,0.111484,0.346875);
vpnt[1337] = VPosNormalTex(0.2625,-0.0379415,0.0786965,0.762916,-0.457143,-0.457143,0.3215,0.36875);
vpnt[1338] = VPosNormalTex(0.2625,-0.045262,0.086017,0.762916,-0.457143,-0.457143,0.327092,0.36875);
vpnt[1339] = VPosNormalTex(0.30625,0.0483781,0.0653902,0.762916,-0.457143,-0.457143,0.148624,0.346875);
vpnt[1340] = VPosNormalTex(0.30625,0.0556987,0.0580697,0.762916,-0.457143,-0.457143,0.128317,0.346875);
vpnt[1341] = VPosNormalTex(0.2625,-0.045262,0.086017,0.889378,-0.441594,-0.118325,0.327092,0.36875);
vpnt[1342] = VPosNormalTex(0.2625,-0.0479415,0.096017,0.889378,-0.441594,-0.118325,0.323703,0.36875);
vpnt[1343] = VPosNormalTex(0.30625,0.0456987,0.0753902,0.889378,-0.441594,-0.118325,0.16327,0.346875);
vpnt[1344] = VPosNormalTex(0.30625,0.0483781,0.0653902,0.889378,-0.441594,-0.118325,0.148624,0.346875);
vpnt[1345] = VPosNormalTex(0.2625,-0.0479415,0.096017,0.909614,-0.401298,0.107528,0.323703,0.36875);
vpnt[1346] = VPosNormalTex(0.2625,-0.045262,0.106017,0.909614,-0.401298,0.107528,0.31422,0.36875);
vpnt[1347] = VPosNormalTex(0.30625,0.0483781,0.0853902,0.909614,-0.401298,0.107528,0.167961,0.346875);
vpnt[1348] = VPosNormalTex(0.30625,0.0456987,0.0753902,0.909614,-0.401298,0.107528,0.16327,0.346875);
vpnt[1349] = VPosNormalTex(0.2625,-0.045262,0.106017,0.879365,-0.336687,0.336687,0.31422,0.36875);
vpnt[1350] = VPosNormalTex(0.2625,-0.0379415,0.113338,0.879365,-0.336687,0.336687,0.301413,0.36875);
vpnt[1351] = VPosNormalTex(0.30625,0.0556987,0.0927107,0.879365,-0.336687,0.336687,0.163898,0.346875);
vpnt[1352] = VPosNormalTex(0.30625,0.0483781,0.0853902,0.879365,-0.336687,0.336687,0.167961,0.346875);
vpnt[1353] = VPosNormalTex(0.2625,-0.0379415,0.113338,0.710395,-0.182157,0.679821,0.301413,0.36875);
vpnt[1354] = VPosNormalTex(0.2625,-0.0279415,0.116017,0.710395,-0.182157,0.679821,0.287615,0.36875);
vpnt[1355] = VPosNormalTex(0.30625,0.0656987,0.0953902,0.710395,-0.182157,0.679821,0.15401,0.346875);
vpnt[1356] = VPosNormalTex(0.30625,0.0556987,0.0927107,0.710395,-0.182157,0.679821,0.163898,0.346875);
vpnt[1357] = VPosNormalTex(0.30625,0.0656987,0.0953902,0.8729,0.126277,0.471274,0.15401,0.346875);
vpnt[1358] = VPosNormalTex(0.30625,0.0756987,0.0927107,0.8729,0.126277,0.471274,0.141023,0.346875);
vpnt[1359] = VPosNormalTex(0.35,0.108936,0.00277049,0.8729,0.126277,0.471274,0.00404681,0.325);
vpnt[1360] = VPosNormalTex(0.35,0.0989358,0.00544998,0.8729,0.126277,0.471274,0.00875837,0.325);
vpnt[1361] = VPosNormalTex(0.30625,0.0756987,0.0927107,0.675642,0.5213,0.5213,0.141023,0.346875);
vpnt[1362] = VPosNormalTex(0.30625,0.0830192,0.0853902,0.675642,0.5213,0.5213,0.127241,0.346875);
vpnt[1363] = VPosNormalTex(0.35,0.116256,-0.00455002,0.675642,0.5213,0.5213,-0.0062258,0.325);
vpnt[1364] = VPosNormalTex(0.35,0.108936,0.00277049,0.675642,0.5213,0.5213,0.00404681,0.325);
vpnt[1365] = VPosNormalTex(0.30625,0.0830192,0.0853902,-0.197762,0.946849,0.253707,0.127241,0.346875);
vpnt[1366] = VPosNormalTex(0.30625,0.0856986,0.0753902,-0.197762,0.946849,0.253707,0.114829,0.346875);
vpnt[1367] = VPosNormalTex(0.35,0.118936,-0.01455,-0.197762,0.946849,0.253707,-0.019374,0.325);
vpnt[1368] = VPosNormalTex(0.35,0.116256,-0.00455002,-0.197762,0.946849,0.253707,-0.0062258,0.325);
vpnt[1369] = VPosNormalTex(0.30625,0.0856986,0.0753902,-0.784699,0.598755,-0.160436,0.114829,0.346875);
vpnt[1370] = VPosNormalTex(0.30625,0.0830192,0.0653902,-0.784699,0.598755,-0.160436,0.106183,0.346875);
vpnt[1371] = VPosNormalTex(0.35,0.116256,-0.02455,-0.784699,0.598755,-0.160436,-0.0331224,0.325);
vpnt[1372] = VPosNormalTex(0.35,0.118936,-0.01455,-0.784699,0.598755,-0.160436,-0.019374,0.325);
vpnt[1373] = VPosNormalTex(0.30625,0.0830192,0.0653902,-0.893604,0.317389,-0.317389,0.106183,0.346875);
vpnt[1374] = VPosNormalTex(0.30625,0.0756987,0.0580697,-0.893604,0.317389,-0.317389,0.104146,0.346875);
vpnt[1375] = VPosNormalTex(0.35,0.108936,-0.0318705,-0.893604,0.317389,-0.317389,-0.0452986,0.325);
vpnt[1376] = VPosNormalTex(0.35,0.116256,-0.02455,-0.893604,0.317389,-0.317389,-0.0331224,0.325);
vpnt[1377] = VPosNormalTex(0.30625,0.0756987,0.0580697,-0.909104,0.107816,-0.402376,0.104146,0.346875);
vpnt[1378] = VPosNormalTex(0.30625,0.0656987,0.0553902,-0.909104,0.107816,-0.402376,0.111484,0.346875);
vpnt[1379] = VPosNormalTex(0.35,0.0989358,-0.03455,-0.909104,0.107816,-0.402376,-0.0534723,0.325);
vpnt[1380] = VPosNormalTex(0.35,0.108936,-0.0318705,-0.909104,0.107816,-0.402376,-0.0452986,0.325);
vpnt[1381] = VPosNormalTex(0.30625,0.0656987,0.0553902,-0.8729,-0.126278,-0.471274,0.111484,0.346875);
vpnt[1382] = VPosNormalTex(0.30625,0.0556987,0.0580697,-0.8729,-0.126278,-0.471274,0.128317,0.346875);
vpnt[1383] = VPosNormalTex(0.35,0.0889358,-0.0318705,-0.8729,-0.126278,-0.471274,-0.0547649,0.325);
vpnt[1384] = VPosNormalTex(0.35,0.0989358,-0.03455,-0.8729,-0.126278,-0.471274,-0.0534723,0.325);
vpnt[1385] = VPosNormalTex(0.30625,0.0556987,0.0580697,-0.675642,-0.5213,-0.5213,0.128317,0.346875);
vpnt[1386] = VPosNormalTex(0.30625,0.0483781,0.0653902,-0.675642,-0.5213,-0.5213,0.148624,0.346875);
vpnt[1387] = VPosNormalTex(0.35,0.0816153,-0.02455,-0.675642,-0.5213,-0.5213,-0.0465038,0.325);
vpnt[1388] = VPosNormalTex(0.35,0.0889358,-0.0318705,-0.675642,-0.5213,-0.5213,-0.0547649,0.325);
vpnt[1389] = VPosNormalTex(0.30625,0.0483781,0.0653902,0.197761,-0.946849,-0.253708,0.148624,0.346875);
vpnt[1390] = VPosNormalTex(0.30625,0.0456987,0.0753902,0.197761,-0.946849,-0.253708,0.16327,0.346875);
vpnt[1391] = VPosNormalTex(0.35,0.0789358,-0.01455,0.197761,-0.946849,-0.253708,-0.0290109,0.325);
vpnt[1392] = VPosNormalTex(0.35,0.0816153,-0.02455,0.197761,-0.946849,-0.253708,-0.0465038,0.325);
vpnt[1393] = VPosNormalTex(0.30625,0.0456987,0.0753902,0.784699,-0.598755,0.160436,0.16327,0.346875);
vpnt[1394] = VPosNormalTex(0.30625,0.0483781,0.0853902,0.784699,-0.598755,0.160436,0.167961,0.346875);
vpnt[1395] = VPosNormalTex(0.35,0.0816153,-0.00455002,0.784699,-0.598755,0.160436,-0.00886364,0.325);
vpnt[1396] = VPosNormalTex(0.35,0.0789358,-0.01455,0.784699,-0.598755,0.160436,-0.0290109,0.325);
vpnt[1397] = VPosNormalTex(0.30625,0.0483781,0.0853902,0.893604,-0.317389,0.317389,0.167961,0.346875);
vpnt[1398] = VPosNormalTex(0.30625,0.0556987,0.0927107,0.893604,-0.317389,0.317389,0.163898,0.346875);
vpnt[1399] = VPosNormalTex(0.35,0.0889358,0.00277049,0.893604,-0.317389,0.317389,0.00495633,0.325);
vpnt[1400] = VPosNormalTex(0.35,0.0816153,-0.00455002,0.893604,-0.317389,0.317389,-0.00886364,0.325);
vpnt[1401] = VPosNormalTex(0.30625,0.0556987,0.0927107,0.909104,-0.107816,0.402376,0.163898,0.346875);
vpnt[1402] = VPosNormalTex(0.30625,0.0656987,0.0953902,0.909104,-0.107816,0.402376,0.15401,0.346875);
vpnt[1403] = VPosNormalTex(0.35,0.0989358,0.00544998,0.909104,-0.107816,0.402376,0.00875837,0.325);
vpnt[1404] = VPosNormalTex(0.35,0.0889358,0.00277049,0.909104,-0.107816,0.402376,0.00495633,0.325);
vpnt[1405] = VPosNormalTex(0.35,0.0989358,0.00544998,0.897224,0.114288,0.426529,0.00875837,0.325);
vpnt[1406] = VPosNormalTex(0.35,0.108936,0.00277049,0.897224,0.114288,0.426529,0.00404681,0.325);
vpnt[1407] = VPosNormalTex(0.39375,0.0512119,-0.0737925,0.897224,0.114288,0.426529,-0.153443,0.303125);
vpnt[1408] = VPosNormalTex(0.39375,0.0412119,-0.071113,0.897224,0.114288,0.426529,-0.166407,0.303125);
vpnt[1409] = VPosNormalTex(0.35,0.108936,0.00277049,0.908234,0.295898,0.295898,0.00404681,0.325);
vpnt[1410] = VPosNormalTex(0.35,0.116256,-0.00455002,0.908234,0.295898,0.295898,-0.0062258,0.325);
vpnt[1411] = VPosNormalTex(0.39375,0.0585324,-0.081113,0.908234,0.295898,0.295898,-0.150514,0.303125);
vpnt[1412] = VPosNormalTex(0.39375,0.0512119,-0.0737925,0.908234,0.295898,0.295898,-0.153443,0.303125);
vpnt[1413] = VPosNormalTex(0.35,0.116256,-0.00455002,0.86544,0.48394,0.129672,-0.0062258,0.325);
vpnt[1414] = VPosNormalTex(0.35,0.118936,-0.01455,0.86544,0.48394,0.129672,-0.019374,0.325);
vpnt[1415] = VPosNormalTex(0.39375,0.0612119,-0.091113,0.86544,0.48394,0.129672,-0.15585,0.303125);
vpnt[1416] = VPosNormalTex(0.39375,0.0585324,-0.081113,0.86544,0.48394,0.129672,-0.150514,0.303125);
vpnt[1417] = VPosNormalTex(0.35,0.118936,-0.01455,0.634777,0.746366,-0.199989,-0.019374,0.325);
vpnt[1418] = VPosNormalTex(0.35,0.116256,-0.02455,0.634777,0.746366,-0.199989,-0.0331224,0.325);
vpnt[1419] = VPosNormalTex(0.39375,0.0585324,-0.101113,0.634777,0.746366,-0.199989,-0.166484,0.303125);
vpnt[1420] = VPosNormalTex(0.39375,0.0612119,-0.091113,0.634777,0.746366,-0.199989,-0.15585,0.303125);
vpnt[1421] = VPosNormalTex(0.35,0.116256,-0.02455,-0.291282,0.676445,-0.676445,-0.0331224,0.325);
vpnt[1422] = VPosNormalTex(0.35,0.108936,-0.0318705,-0.291282,0.676445,-0.676445,-0.0452986,0.325);
vpnt[1423] = VPosNormalTex(0.39375,0.0512119,-0.108434,-0.291282,0.676445,-0.676445,-0.179776,0.303125);
vpnt[1424] = VPosNormalTex(0.39375,0.0585324,-0.101113,-0.291282,0.676445,-0.676445,-0.166484,0.303125);
vpnt[1425] = VPosNormalTex(0.35,0.108936,-0.0318705,-0.803324,0.154138,-0.57525,-0.0452986,0.325);
vpnt[1426] = VPosNormalTex(0.35,0.0989358,-0.03455,-0.803324,0.154138,-0.57525,-0.0534723,0.325);
vpnt[1427] = VPosNormalTex(0.39375,0.0412119,-0.111113,-0.803324,0.154138,-0.57525,-0.193473,0.303125);
vpnt[1428] = VPosNormalTex(0.39375,0.0512119,-0.108434,-0.803324,0.154138,-0.57525,-0.179776,0.303125);
vpnt[1429] = VPosNormalTex(0.35,0.0989358,-0.03455,-0.897224,-0.114288,-0.426529,-0.0534723,0.325);
vpnt[1430] = VPosNormalTex(0.35,0.0889358,-0.0318705,-0.897224,-0.114288,-0.426529,-0.0547649,0.325);
vpnt[1431] = VPosNormalTex(0.39375,0.0312119,-0.108434,-0.897224,-0.114288,-0.426529,-0.205394,0.303125);
vpnt[1432] = VPosNormalTex(0.39375,0.0412119,-0.111113,-0.897224,-0.114288,-0.426529,-0.193473,0.303125);
vpnt[1433] = VPosNormalTex(0.35,0.0889358,-0.0318705,-0.908234,-0.295898,-0.295898,-0.0547649,0.325);
vpnt[1434] = VPosNormalTex(0.35,0.0816153,-0.02455,-0.908234,-0.295898,-0.295898,-0.0465038,0.325);
vpnt[1435] = VPosNormalTex(0.39375,0.0238914,-0.101113,-0.908234,-0.295898,-0.295898,-0.213072,0.303125);
vpnt[1436] = VPosNormalTex(0.39375,0.0312119,-0.108434,-0.908234,-0.295898,-0.295898,-0.205394,0.303125);
vpnt[1437] = VPosNormalTex(0.35,0.0816153,-0.02455,-0.86544,-0.48394,-0.129672,-0.0465038,0.325);
vpnt[1438] = VPosNormalTex(0.35,0.0789358,-0.01455,-0.86544,-0.48394,-0.129672,-0.0290109,0.325);
vpnt[1439] = VPosNormalTex(0.39375,0.0212119,-0.091113,-0.86544,-0.48394,-0.129672,-0.213596,0.303125);
vpnt[1440] = VPosNormalTex(0.39375,0.0238914,-0.101113,-0.86544,-0.48394,-0.129672,-0.213072,0.303125);
vpnt[1441] = VPosNormalTex(0.35,0.0789358,-0.01455,-0.634777,-0.746366,0.199989,-0.0290109,0.325);
vpnt[1442] = VPosNormalTex(0.35,0.0816153,-0.00455002,-0.634777,-0.746366,0.199989,-0.00886364,0.325);
vpnt[1443] = VPosNormalTex(0.39375,0.0238914,-0.081113,-0.634777,-0.746366,0.199989,-0.204411,0.303125);
vpnt[1444] = VPosNormalTex(0.39375,0.0212119,-0.091113,-0.634777,-0.746366,0.199989,-0.213596,0.303125);
vpnt[1445] = VPosNormalTex(0.35,0.0816153,-0.00455002,0.291282,-0.676445,0.676445,-0.00886364,0.325);
vpnt[1446] = VPosNormalTex(0.35,0.0889358,0.00277049,0.291282,-0.676445,0.676445,0.00495633,0.325);
vpnt[1447] = VPosNormalTex(0.39375,0.0312119,-0.0737925,0.291282,-0.676445,0.676445,-0.186314,0.303125);
vpnt[1448] = VPosNormalTex(0.39375,0.0238914,-0.081113,0.291282,-0.676445,0.676445,-0.204411,0.303125);
vpnt[1449] = VPosNormalTex(0.35,0.0889358,0.00277049,0.803324,-0.154138,0.57525,0.00495633,0.325);
vpnt[1450] = VPosNormalTex(0.35,0.0989358,0.00544998,0.803324,-0.154138,0.57525,0.00875837,0.325);
vpnt[1451] = VPosNormalTex(0.39375,0.0412119,-0.071113,0.803324,-0.154138,0.57525,-0.166407,0.303125);
vpnt[1452] = VPosNormalTex(0.39375,0.0312119,-0.0737925,0.803324,-0.154138,0.57525,-0.186314,0.303125);
vpnt[1453] = VPosNormalTex(0.39375,0.0412119,-0.071113,0.376612,0.239762,0.894806,-0.166407,0.303125);
vpnt[1454] = VPosNormalTex(0.39375,0.0512119,-0.0737925,0.376612,0.239762,0.894806,-0.153443,0.303125);
vpnt[1455] = VPosNormalTex(0.4375,-0.0444021,-0.0665866,0.376612,0.239762,0.894806,-0.343602,0.28125);
vpnt[1456] = VPosNormalTex(0.4375,-0.0544021,-0.0639071,0.376612,0.239762,0.894806,-0.362241,0.28125);
vpnt[1457] = VPosNormalTex(0.39375,0.0512119,-0.0737925,0.819292,0.405438,0.405438,-0.153443,0.303125);
vpnt[1458] = VPosNormalTex(0.39375,0.0585324,-0.081113,0.819292,0.405438,0.405438,-0.150514,0.303125);
vpnt[1459] = VPosNormalTex(0.4375,-0.0370816,-0.0739072,0.819292,0.405438,0.405438,-0.324012,0.28125);
vpnt[1460] = VPosNormalTex(0.4375,-0.0444021,-0.0665866,0.819292,0.405438,0.405438,-0.343602,0.28125);
vpnt[1461] = VPosNormalTex(0.39375,0.0585324,-0.081113,0.900299,0.42044,0.112656,-0.150514,0.303125);
vpnt[1462] = VPosNormalTex(0.39375,0.0612119,-0.091113,0.900299,0.42044,0.112656,-0.15585,0.303125);
vpnt[1463] = VPosNormalTex(0.4375,-0.0344021,-0.0839071,0.900299,0.42044,0.112656,-0.311927,0.28125);
vpnt[1464] = VPosNormalTex(0.4375,-0.0370816,-0.0739072,0.900299,0.42044,0.112656,-0.324012,0.28125);
vpnt[1465] = VPosNormalTex(0.39375,0.0612119,-0.091113,0.906992,0.406797,-0.109001,-0.15585,0.303125);
vpnt[1466] = VPosNormalTex(0.39375,0.0585324,-0.101113,0.906992,0.406797,-0.109001,-0.166484,0.303125);
vpnt[1467] = VPosNormalTex(0.4375,-0.0370816,-0.0939071,0.906992,0.406797,-0.109001,-0.309855,0.28125);
vpnt[1468] = VPosNormalTex(0.4375,-0.0344021,-0.0839071,0.906992,0.406797,-0.109001,-0.311927,0.28125);
vpnt[1469] = VPosNormalTex(0.39375,0.0585324,-0.101113,0.856831,0.364583,-0.364583,-0.166484,0.303125);
vpnt[1470] = VPosNormalTex(0.39375,0.0512119,-0.108434,0.856831,0.364583,-0.364583,-0.179776,0.303125);
vpnt[1471] = VPosNormalTex(0.4375,-0.0444021,-0.101228,0.856831,0.364583,-0.364583,-0.315789,0.28125);
vpnt[1472] = VPosNormalTex(0.4375,-0.0370816,-0.0939071,0.856831,0.364583,-0.364583,-0.309855,0.28125);
vpnt[1473] = VPosNormalTex(0.39375,0.0512119,-0.108434,0.586825,0.209569,-0.782123,-0.179776,0.303125);
vpnt[1474] = VPosNormalTex(0.39375,0.0412119,-0.111113,0.586825,0.209569,-0.782123,-0.193473,0.303125);
vpnt[1475] = VPosNormalTex(0.4375,-0.0544021,-0.103907,0.586825,0.209569,-0.782123,-0.326764,0.28125);
vpnt[1476] = VPosNormalTex(0.4375,-0.0444021,-0.101228,0.586825,0.209569,-0.782123,-0.315789,0.28125);
vpnt[1477] = VPosNormalTex(0.39375,0.0412119,-0.111113,-0.376612,-0.239762,-0.894806,-0.193473,0.303125);
vpnt[1478] = VPosNormalTex(0.39375,0.0312119,-0.108434,-0.376612,-0.239762,-0.894806,-0.205394,0.303125);
vpnt[1479] = VPosNormalTex(0.4375,-0.0644021,-0.101228,-0.376612,-0.239762,-0.894806,-0.34018,0.28125);
vpnt[1480] = VPosNormalTex(0.4375,-0.0544021,-0.103907,-0.376612,-0.239762,-0.894806,-0.326764,0.28125);
vpnt[1481] = VPosNormalTex(0.39375,0.0312119,-0.108434,-0.819292,-0.405438,-0.405438,-0.205394,0.303125);
vpnt[1482] = VPosNormalTex(0.39375,0.0238914,-0.101113,-0.819292,-0.405438,-0.405438,-0.213072,0.303125);
vpnt[1483] = VPosNormalTex(0.4375,-0.0717226,-0.0939071,-0.819292,-0.405438,-0.405438,-0.353809,0.28125);
vpnt[1484] = VPosNormalTex(0.4375,-0.0644021,-0.101228,-0.819292,-0.405438,-0.405438,-0.34018,0.28125);
vpnt[1485] = VPosNormalTex(0.39375,0.0238914,-0.101113,-0.900299,-0.42044,-0.112656,-0.213072,0.303125);
vpnt[1486] = VPosNormalTex(0.39375,0.0212119,-0.091113,-0.900299,-0.42044,-0.112656,-0.213596,0.303125);
vpnt[1487] = VPosNormalTex(0.4375,-0.0744021,-0.0839071,-0.900299,-0.42044,-0.112656,-0.365456,0.28125);
vpnt[1488] = VPosNormalTex(0.4375,-0.0717226,-0.0939071,-0.900299,-0.42044,-0.112656,-0.353809,0.28125);
vpnt[1489] = VPosNormalTex(0.39375,0.0212119,-0.091113,-0.906992,-0.406797,0.109001,-0.213596,0.303125);
vpnt[1490] = VPosNormalTex(0.39375,0.0238914,-0.081113,-0.906992,-0.406797,0.109001,-0.204411,0.303125);
vpnt[1491] = VPosNormalTex(0.4375,-0.0717226,-0.0739072,-0.906992,-0.406797,0.109001,-0.372613,0.28125);
vpnt[1492] = VPosNormalTex(0.4375,-0.0744021,-0.0839071,-0.906992,-0.406797,0.109001,-0.365456,0.28125);
vpnt[1493] = VPosNormalTex(0.39375,0.0238914,-0.081113,-0.856831,-0.364583,0.364583,-0.204411,0.303125);
vpnt[1494] = VPosNormalTex(0.39375,0.0312119,-0.0737925,-0.856831,-0.364583,0.364583,-0.186314,0.303125);
vpnt[1495] = VPosNormalTex(0.4375,-0.0644021,-0.0665866,-0.856831,-0.364583,0.364583,-0.372346,0.28125);
vpnt[1496] = VPosNormalTex(0.4375,-0.0717226,-0.0739072,-0.856831,-0.364583,0.364583,-0.372613,0.28125);
vpnt[1497] = VPosNormalTex(0.39375,0.0312119,-0.0737925,-0.586825,-0.209569,0.782123,-0.186314,0.303125);
vpnt[1498] = VPosNormalTex(0.39375,0.0412119,-0.071113,-0.586825,-0.209569,0.782123,-0.166407,0.303125);
vpnt[1499] = VPosNormalTex(0.4375,-0.0544021,-0.0639071,-0.586825,-0.209569,0.782123,-0.362241,0.28125);
vpnt[1500] = VPosNormalTex(0.4375,-0.0644021,-0.0665866,-0.586825,-0.209569,0.782123,-0.372346,0.28125);
vpnt[1501] = VPosNormalTex(0.4375,-0.0544021,-0.0639071,-0.846884,0.137634,0.513658,-0.362241,0.28125);
vpnt[1502] = VPosNormalTex(0.4375,-0.0444021,-0.0665866,-0.846884,0.137634,0.513658,-0.343602,0.28125);
vpnt[1503] = VPosNormalTex(0.48125,-0.089999,0.0177631,-0.846884,0.137634,0.513658,0.468986,0.259375);
vpnt[1504] = VPosNormalTex(0.48125,-0.099999,0.0204426,-0.846884,0.137634,0.513658,0.467906,0.259375);
vpnt[1505] = VPosNormalTex(0.4375,-0.0444021,-0.0665866,-0.530815,0.599264,0.599264,-0.343602,0.28125);
vpnt[1506] = VPosNormalTex(0.4375,-0.0370816,-0.0739072,-0.530815,0.599264,0.599264,-0.324012,0.28125);
vpnt[1507] = VPosNormalTex(0.48125,-0.0826785,0.0104426,-0.530815,0.599264,0.599264,0.480004,0.259375);
vpnt[1508] = VPosNormalTex(0.48125,-0.089999,0.0177631,-0.530815,0.599264,0.599264,0.468986,0.259375);
vpnt[1509] = VPosNormalTex(0.4375,-0.0370816,-0.0739072,0.452699,0.861281,0.230779,-0.324012,0.28125);
vpnt[1510] = VPosNormalTex(0.4375,-0.0344021,-0.0839071,0.452699,0.861281,0.230779,-0.311927,0.28125);
vpnt[1511] = VPosNormalTex(0.48125,-0.079999,0.000442594,0.452699,0.861281,0.230779,0.499119,0.259375);
vpnt[1512] = VPosNormalTex(0.48125,-0.0826785,0.0104426,0.452699,0.861281,0.230779,0.480004,0.259375);
vpnt[1513] = VPosNormalTex(0.4375,-0.0344021,-0.0839071,0.83302,0.534392,-0.14319,-0.311927,0.28125);
vpnt[1514] = VPosNormalTex(0.4375,-0.0370816,-0.0939071,0.83302,0.534392,-0.14319,-0.309855,0.28125);
vpnt[1515] = VPosNormalTex(0.48125,-0.0826785,-0.00955741,0.83302,0.534392,-0.14319,-0.481683,0.259375);
vpnt[1516] = VPosNormalTex(0.48125,-0.079999,0.000442594,0.83302,0.534392,-0.14319,0.499119,0.259375);
vpnt[1517] = VPosNormalTex(0.4375,-0.0370816,-0.0939071,0.902881,0.303979,-0.303979,-0.309855,0.28125);
vpnt[1518] = VPosNormalTex(0.4375,-0.0444021,-0.101228,0.902881,0.303979,-0.303979,-0.315789,0.28125);
vpnt[1519] = VPosNormalTex(0.48125,-0.089999,-0.0168779,0.902881,0.303979,-0.303979,-0.470496,0.259375);
vpnt[1520] = VPosNormalTex(0.48125,-0.0826785,-0.00955741,0.902881,0.303979,-0.303979,-0.481683,0.259375);
vpnt[1521] = VPosNormalTex(0.4375,-0.0444021,-0.101228,0.90536,0.109906,-0.410175,-0.315789,0.28125);
vpnt[1522] = VPosNormalTex(0.4375,-0.0544021,-0.103907,0.90536,0.109906,-0.410175,-0.326764,0.28125);
vpnt[1523] = VPosNormalTex(0.48125,-0.099999,-0.0195574,0.90536,0.109906,-0.410175,-0.469261,0.259375);
vpnt[1524] = VPosNormalTex(0.48125,-0.089999,-0.0168779,0.90536,0.109906,-0.410175,-0.470496,0.259375);
vpnt[1525] = VPosNormalTex(0.4375,-0.0544021,-0.103907,0.846884,-0.137634,-0.513658,-0.326764,0.28125);
vpnt[1526] = VPosNormalTex(0.4375,-0.0644021,-0.101228,0.846884,-0.137634,-0.513658,-0.34018,0.28125);
vpnt[1527] = VPosNormalTex(0.48125,-0.109999,-0.0168779,0.846884,-0.137634,-0.513658,-0.475769,0.259375);
vpnt[1528] = VPosNormalTex(0.48125,-0.099999,-0.0195574,0.846884,-0.137634,-0.513658,-0.469261,0.259375);
vpnt[1529] = VPosNormalTex(0.4375,-0.0644021,-0.101228,0.530815,-0.599264,-0.599264,-0.34018,0.28125);
vpnt[1530] = VPosNormalTex(0.4375,-0.0717226,-0.0939071,0.530815,-0.599264,-0.599264,-0.353809,0.28125);
vpnt[1531] = VPosNormalTex(0.48125,-0.11732,-0.00955741,0.530815,-0.599264,-0.599264,-0.487063,0.259375);
vpnt[1532] = VPosNormalTex(0.48125,-0.109999,-0.0168779,0.530815,-0.599264,-0.599264,-0.475769,0.259375);
vpnt[1533] = VPosNormalTex(0.4375,-0.0717226,-0.0939071,-0.452698,-0.861281,-0.230779,-0.353809,0.28125);
vpnt[1534] = VPosNormalTex(0.4375,-0.0744021,-0.0839071,-0.452698,-0.861281,-0.230779,-0.365456,0.28125);
vpnt[1535] = VPosNormalTex(0.48125,-0.119999,0.000442594,-0.452698,-0.861281,-0.230779,0.499413,0.259375);
vpnt[1536] = VPosNormalTex(0.48125,-0.11732,-0.00955741,-0.452698,-0.861281,-0.230779,-0.487063,0.259375);
vpnt[1537] = VPosNormalTex(0.4375,-0.0744021,-0.0839071,-0.83302,-0.534392,0.14319,-0.365456,0.28125);
vpnt[1538] = VPosNormalTex(0.4375,-0.0717226,-0.0739072,-0.83302,-0.534392,0.14319,-0.372613,0.28125);
vpnt[1539] = VPosNormalTex(0.48125,-0.11732,0.0104426,-0.83302,-0.534392,0.14319,0.485871,0.259375);
vpnt[1540] = VPosNormalTex(0.48125,-0.119999,0.000442594,-0.83302,-0.534392,0.14319,0.499413,0.259375);
vpnt[1541] = VPosNormalTex(0.4375,-0.0717226,-0.0739072,-0.902881,-0.303979,0.303979,-0.372613,0.28125);
vpnt[1542] = VPosNormalTex(0.4375,-0.0644021,-0.0665866,-0.902881,-0.303979,0.303979,-0.372346,0.28125);
vpnt[1543] = VPosNormalTex(0.48125,-0.109999,0.0177631,-0.902881,-0.303979,0.303979,0.474519,0.259375);
vpnt[1544] = VPosNormalTex(0.48125,-0.11732,0.0104426,-0.902881,-0.303979,0.303979,0.485871,0.259375);
vpnt[1545] = VPosNormalTex(0.4375,-0.0644021,-0.0665866,-0.90536,-0.109906,0.410175,-0.372346,0.28125);
vpnt[1546] = VPosNormalTex(0.4375,-0.0544021,-0.0639071,-0.90536,-0.109906,0.410175,-0.362241,0.28125);
vpnt[1547] = VPosNormalTex(0.48125,-0.099999,0.0204426,-0.90536,-0.109906,0.410175,0.467906,0.259375);
vpnt[1548] = VPosNormalTex(0.48125,-0.109999,0.0177631,-0.90536,-0.109906,0.410175,0.474519,0.259375);
vpnt[1549] = VPosNormalTex(0.48125,-0.099999,0.0204426,-0.905009,0.1101,0.410898,0.467906,0.259375);
vpnt[1550] = VPosNormalTex(0.48125,-0.089999,0.0177631,-0.905009,0.1101,0.410898,0.468986,0.259375);
vpnt[1551] = VPosNormalTex(0.525,-0.0436573,0.101706,-0.905009,0.1101,0.410898,0.314532,0.2375);
vpnt[1552] = VPosNormalTex(0.525,-0.0536573,0.104385,-0.905009,0.1101,0.410898,0.325568,0.2375);
vpnt[1553] = VPosNormalTex(0.48125,-0.089999,0.0177631,-0.903313,0.303336,0.303336,0.468986,0.259375);
vpnt[1554] = VPosNormalTex(0.48125,-0.0826785,0.0104426,-0.903313,0.303336,0.303336,0.480004,0.259375);
vpnt[1555] = VPosNormalTex(0.525,-0.0363367,0.0943854,-0.903313,0.303336,0.303336,0.308488,0.2375);
vpnt[1556] = VPosNormalTex(0.525,-0.0436573,0.101706,-0.903313,0.303336,0.303336,0.314532,0.2375);
vpnt[1557] = VPosNormalTex(0.48125,-0.0826785,0.0104426,-0.835374,0.530951,0.142268,0.480004,0.259375);
vpnt[1558] = VPosNormalTex(0.48125,-0.079999,0.000442594,-0.835374,0.530951,0.142268,0.499119,0.259375);
vpnt[1559] = VPosNormalTex(0.525,-0.0336573,0.0843854,-0.835374,0.530951,0.142268,0.310402,0.2375);
vpnt[1560] = VPosNormalTex(0.525,-0.0363367,0.0943854,-0.835374,0.530951,0.142268,0.308488,0.2375);
vpnt[1561] = VPosNormalTex(0.48125,-0.079999,0.000442594,-0.46591,0.854682,-0.229012,0.499119,0.259375);
vpnt[1562] = VPosNormalTex(0.48125,-0.0826785,-0.00955741,-0.46591,0.854682,-0.229012,-0.481683,0.259375);
vpnt[1563] = VPosNormalTex(0.525,-0.0363367,0.0743854,-0.46591,0.854682,-0.229012,0.32232,0.2375);
vpnt[1564] = VPosNormalTex(0.525,-0.0336573,0.0843854,-0.46591,0.854682,-0.229012,0.310402,0.2375);
vpnt[1565] = VPosNormalTex(0.48125,-0.0826785,-0.00955741,0.519342,0.60427,-0.60427,-0.481683,0.259375);
vpnt[1566] = VPosNormalTex(0.48125,-0.089999,-0.0168779,0.519342,0.60427,-0.60427,-0.470496,0.259375);
vpnt[1567] = VPosNormalTex(0.525,-0.0436573,0.0670649,0.519342,0.60427,-0.60427,0.341841,0.2375);
vpnt[1568] = VPosNormalTex(0.525,-0.0363367,0.0743854,0.519342,0.60427,-0.60427,0.32232,0.2375);
vpnt[1569] = VPosNormalTex(0.48125,-0.089999,-0.0168779,0.844852,0.138468,-0.51677,-0.470496,0.259375);
vpnt[1570] = VPosNormalTex(0.48125,-0.099999,-0.0195574,0.844852,0.138468,-0.51677,-0.469261,0.259375);
vpnt[1571] = VPosNormalTex(0.525,-0.0536573,0.0643854,0.844852,0.138468,-0.51677,0.360575,0.2375);
vpnt[1572] = VPosNormalTex(0.525,-0.0436573,0.0670649,0.844852,0.138468,-0.51677,0.341841,0.2375);
vpnt[1573] = VPosNormalTex(0.48125,-0.099999,-0.0195574,0.905009,-0.1101,-0.410898,-0.469261,0.259375);
vpnt[1574] = VPosNormalTex(0.48125,-0.109999,-0.0168779,0.905009,-0.1101,-0.410898,-0.475769,0.259375);
vpnt[1575] = VPosNormalTex(0.525,-0.0636573,0.0670649,0.905009,-0.1101,-0.410898,0.370852,0.2375);
vpnt[1576] = VPosNormalTex(0.525,-0.0536573,0.0643854,0.905009,-0.1101,-0.410898,0.360575,0.2375);
vpnt[1577] = VPosNormalTex(0.48125,-0.109999,-0.0168779,0.903313,-0.303336,-0.303336,-0.475769,0.259375);
vpnt[1578] = VPosNormalTex(0.48125,-0.11732,-0.00955741,0.903313,-0.303336,-0.303336,-0.487063,0.259375);
vpnt[1579] = VPosNormalTex(0.525,-0.0709778,0.0743854,0.903313,-0.303336,-0.303336,0.37127,0.2375);
vpnt[1580] = VPosNormalTex(0.525,-0.0636573,0.0670649,0.903313,-0.303336,-0.303336,0.370852,0.2375);
vpnt[1581] = VPosNormalTex(0.48125,-0.11732,-0.00955741,0.835374,-0.530952,-0.142268,-0.487063,0.259375);
vpnt[1582] = VPosNormalTex(0.48125,-0.119999,0.000442594,0.835374,-0.530952,-0.142268,0.499413,0.259375);
vpnt[1583] = VPosNormalTex(0.525,-0.0736573,0.0843854,0.835374,-0.530952,-0.142268,0.364213,0.2375);
vpnt[1584] = VPosNormalTex(0.525,-0.0709778,0.0743854,0.835374,-0.530952,-0.142268,0.37127,0.2375);
vpnt[1585] = VPosNormalTex(0.48125,-0.119999,0.000442594,0.46591,-0.854682,0.229012,0.499413,0.259375);
vpnt[1586] = VPosNormalTex(0.48125,-0.11732,0.0104426,0.46591,-0.854682,0.229012,0.485871,0.259375);
vpnt[1587] = VPosNormalTex(0.525,-0.0709778,0.0943854,0.46591,-0.854682,0.229012,0.35262,0.2375);
vpnt[1588] = VPosNormalTex(0.525,-0.0736573,0.0843854,0.46591,-0.854682,0.229012,0.364213,0.2375);
vpnt[1589] = VPosNormalTex(0.48125,-0.11732,0.0104426,-0.519341,-0.60427,0.60427,0.485871,0.259375);
vpnt[1590] = VPosNormalTex(0.48125,-0.109999,0.0177631,-0.519341,-0.60427,0.60427,0.474519,0.259375);
vpnt[1591] = VPosNormalTex(0.525,-0.0636573,0.101706,-0.519341,-0.60427,0.60427,0.339006,0.2375);
vpnt[1592] = VPosNormalTex(0.525,-0.0709778,0.0943854,-0.519341,-0.60427,0.60427,0.35262,0.2375);
vpnt[1593] = VPosNormalTex(0.48125,-0.109999,0.0177631,-0.844852,-0.138468,0.51677,0.474519,0.259375);
vpnt[1594] = VPosNormalTex(0.48125,-0.099999,0.0204426,-0.844852,-0.138468,0.51677,0.467906,0.259375);
vpnt[1595] = VPosNormalTex(0.525,-0.0536573,0.104385,-0.844852,-0.138468,0.51677,0.325568,0.2375);
vpnt[1596] = VPosNormalTex(0.525,-0.0636573,0.101706,-0.844852,-0.138468,0.51677,0.339006,0.2375);
vpnt[1597] = VPosNormalTex(0.525,-0.0536573,0.104385,-0.576963,0.211395,0.788939,0.325568,0.2375);
vpnt[1598] = VPosNormalTex(0.525,-0.0436573,0.101706,-0.576963,0.211395,0.788939,0.314532,0.2375);
vpnt[1599] = VPosNormalTex(0.56875,0.0520167,0.108065,-0.576963,0.211395,0.788939,0.178601,0.215625);
vpnt[1600] = VPosNormalTex(0.56875,0.0420167,0.110745,-0.576963,0.211395,0.788939,0.192287,0.215625);
vpnt[1601] = VPosNormalTex(0.525,-0.0436573,0.101706,-0.855074,0.36664,0.36664,0.314532,0.2375);
vpnt[1602] = VPosNormalTex(0.525,-0.0363367,0.0943854,-0.855074,0.36664,0.36664,0.308488,0.2375);
vpnt[1603] = VPosNormalTex(0.56875,0.0593372,0.100745,-0.855074,0.36664,0.36664,0.165285,0.215625);
vpnt[1604] = VPosNormalTex(0.56875,0.0520167,0.108065,-0.855074,0.36664,0.36664,0.178601,0.215625);
vpnt[1605] = VPosNormalTex(0.525,-0.0363367,0.0943854,-0.906717,0.40737,0.109154,0.308488,0.2375);
vpnt[1606] = VPosNormalTex(0.525,-0.0336573,0.0843854,-0.906717,0.40737,0.109154,0.310402,0.2375);
vpnt[1607] = VPosNormalTex(0.56875,0.0620167,0.0907447,-0.906717,0.40737,0.109154,0.154585,0.215625);
vpnt[1608] = VPosNormalTex(0.56875,0.0593372,0.100745,-0.906717,0.40737,0.109154,0.165285,0.215625);
vpnt[1609] = VPosNormalTex(0.525,-0.0336573,0.0843854,-0.90082,0.419398,-0.112377,0.310402,0.2375);
vpnt[1610] = VPosNormalTex(0.525,-0.0363367,0.0743854,-0.90082,0.419398,-0.112377,0.32232,0.2375);
vpnt[1611] = VPosNormalTex(0.56875,0.0593372,0.0807447,-0.90082,0.419398,-0.112377,0.149136,0.215625);
vpnt[1612] = VPosNormalTex(0.56875,0.0620167,0.0907447,-0.90082,0.419398,-0.112377,0.154585,0.215625);
vpnt[1613] = VPosNormalTex(0.525,-0.0363367,0.0743854,-0.822026,0.402662,-0.402662,0.32232,0.2375);
vpnt[1614] = VPosNormalTex(0.525,-0.0436573,0.0670649,-0.822026,0.402662,-0.402662,0.341841,0.2375);
vpnt[1615] = VPosNormalTex(0.56875,0.0520167,0.0734242,-0.822026,0.402662,-0.402662,0.151902,0.215625);
vpnt[1616] = VPosNormalTex(0.56875,0.0593372,0.0807447,-0.822026,0.402662,-0.402662,0.149136,0.215625);
vpnt[1617] = VPosNormalTex(0.525,-0.0436573,0.0670649,-0.391602,0.238148,-0.888782,0.341841,0.2375);
vpnt[1618] = VPosNormalTex(0.525,-0.0536573,0.0643854,-0.391602,0.238148,-0.888782,0.360575,0.2375);
vpnt[1619] = VPosNormalTex(0.56875,0.0420167,0.0707447,-0.391602,0.238148,-0.888782,0.164703,0.215625);
vpnt[1620] = VPosNormalTex(0.56875,0.0520167,0.0734242,-0.391602,0.238148,-0.888782,0.151902,0.215625);
vpnt[1621] = VPosNormalTex(0.525,-0.0536573,0.0643854,0.576963,-0.211395,-0.788939,0.360575,0.2375);
vpnt[1622] = VPosNormalTex(0.525,-0.0636573,0.0670649,0.576963,-0.211395,-0.788939,0.370852,0.2375);
vpnt[1623] = VPosNormalTex(0.56875,0.0320167,0.0734242,0.576963,-0.211395,-0.788939,0.184557,0.215625);
vpnt[1624] = VPosNormalTex(0.56875,0.0420167,0.0707447,0.576963,-0.211395,-0.788939,0.164703,0.215625);
vpnt[1625] = VPosNormalTex(0.525,-0.0636573,0.0670649,0.855074,-0.36664,-0.36664,0.370852,0.2375);
vpnt[1626] = VPosNormalTex(0.525,-0.0709778,0.0743854,0.855074,-0.36664,-0.36664,0.37127,0.2375);
vpnt[1627] = VPosNormalTex(0.56875,0.0246962,0.0807447,0.855074,-0.36664,-0.36664,0.20276,0.215625);
vpnt[1628] = VPosNormalTex(0.56875,0.0320167,0.0734242,0.855074,-0.36664,-0.36664,0.184557,0.215625);
vpnt[1629] = VPosNormalTex(0.525,-0.0709778,0.0743854,0.906717,-0.40737,-0.109154,0.37127,0.2375);
vpnt[1630] = VPosNormalTex(0.525,-0.0736573,0.0843854,0.906717,-0.40737,-0.109154,0.364213,0.2375);
vpnt[1631] = VPosNormalTex(0.56875,0.0220167,0.0907447,0.906717,-0.40737,-0.109154,0.212118,0.215625);
vpnt[1632] = VPosNormalTex(0.56875,0.0246962,0.0807447,0.906717,-0.40737,-0.109154,0.20276,0.215625);
vpnt[1633] = VPosNormalTex(0.525,-0.0736573,0.0843854,0.90082,-0.419398,0.112377,0.364213,0.2375);
vpnt[1634] = VPosNormalTex(0.525,-0.0709778,0.0943854,0.90082,-0.419398,0.112377,0.35262,0.2375);
vpnt[1635] = VPosNormalTex(0.56875,0.0246962,0.100745,0.90082,-0.419398,0.112377,0.21174,0.215625);
vpnt[1636] = VPosNormalTex(0.56875,0.0220167,0.0907447,0.90082,-0.419398,0.112377,0.212118,0.215625);
vpnt[1637] = VPosNormalTex(0.525,-0.0709778,0.0943854,0.822026,-0.402662,0.402662,0.35262,0.2375);
vpnt[1638] = VPosNormalTex(0.525,-0.0636573,0.101706,0.822026,-0.402662,0.402662,0.339006,0.2375);
vpnt[1639] = VPosNormalTex(0.56875,0.0320167,0.108065,0.822026,-0.402662,0.402662,0.204158,0.215625);
vpnt[1640] = VPosNormalTex(0.56875,0.0246962,0.100745,0.822026,-0.402662,0.402662,0.21174,0.215625);
vpnt[1641] = VPosNormalTex(0.525,-0.0636573,0.101706,0.391602,-0.238148,0.888782,0.339006,0.2375);
vpnt[1642] = VPosNormalTex(0.525,-0.0536573,0.104385,0.391602,-0.238148,0.888782,0.325568,0.2375);
vpnt[1643] = VPosNormalTex(0.56875,0.0420167,0.110745,0.391602,-0.238148,0.888782,0.192287,0.215625);
vpnt[1644] = VPosNormalTex(0.56875,0.0320167,0.108065,0.391602,-0.238148,0.888782,0.204158,0.215625);
vpnt[1645] = VPosNormalTex(0.56875,0.0420167,0.110745,0.806507,0.15302,0.571079,0.192287,0.215625);
vpnt[1646] = VPosNormalTex(0.56875,0.0520167,0.108065,0.806507,0.15302,0.571079,0.178601,0.215625);
vpnt[1647] = VPosNormalTex(0.6125,0.109061,0.0309943,0.806507,0.15302,0.571079,0.0440688,0.19375);
vpnt[1648] = VPosNormalTex(0.6125,0.0990607,0.0336737,0.806507,0.15302,0.571079,0.0521513,0.19375);
vpnt[1649] = VPosNormalTex(0.56875,0.0520167,0.108065,0.307953,0.672743,0.672743,0.178601,0.215625);
vpnt[1650] = VPosNormalTex(0.56875,0.0593372,0.100745,0.307953,0.672743,0.672743,0.165285,0.215625);
vpnt[1651] = VPosNormalTex(0.6125,0.116381,0.0236737,0.307953,0.672743,0.672743,0.0319388,0.19375);
vpnt[1652] = VPosNormalTex(0.6125,0.109061,0.0309943,0.307953,0.672743,0.672743,0.0440688,0.19375);
vpnt[1653] = VPosNormalTex(0.56875,0.0593372,0.100745,-0.626355,0.752976,0.201759,0.165285,0.215625);
vpnt[1654] = VPosNormalTex(0.56875,0.0620167,0.0907447,-0.626355,0.752976,0.201759,0.154585,0.215625);
vpnt[1655] = VPosNormalTex(0.6125,0.119061,0.0136737,-0.626355,0.752976,0.201759,0.0181987,0.19375);
vpnt[1656] = VPosNormalTex(0.6125,0.116381,0.0236737,-0.626355,0.752976,0.201759,0.0319388,0.19375);
vpnt[1657] = VPosNormalTex(0.56875,0.0620167,0.0907447,-0.863918,0.486471,-0.13035,0.154585,0.215625);
vpnt[1658] = VPosNormalTex(0.56875,0.0593372,0.0807447,-0.863918,0.486471,-0.13035,0.149136,0.215625);
vpnt[1659] = VPosNormalTex(0.6125,0.116381,0.00367375,-0.863918,0.486471,-0.13035,0.00502229,0.19375);
vpnt[1660] = VPosNormalTex(0.6125,0.119061,0.0136737,-0.863918,0.486471,-0.13035,0.0181987,0.19375);
vpnt[1661] = VPosNormalTex(0.56875,0.0593372,0.0807447,-0.90803,0.296211,-0.296211,0.149136,0.215625);
vpnt[1662] = VPosNormalTex(0.56875,0.0520167,0.0734242,-0.90803,0.296211,-0.296211,0.151902,0.215625);
vpnt[1663] = VPosNormalTex(0.6125,0.109061,-0.00364676,-0.90803,0.296211,-0.296211,-0.00531983,0.19375);
vpnt[1664] = VPosNormalTex(0.6125,0.116381,0.00367375,-0.90803,0.296211,-0.296211,0.00502229,0.19375);
vpnt[1665] = VPosNormalTex(0.56875,0.0520167,0.0734242,-0.89784,0.113964,-0.425317,0.151902,0.215625);
vpnt[1666] = VPosNormalTex(0.56875,0.0420167,0.0707447,-0.89784,0.113964,-0.425317,0.164703,0.215625);
vpnt[1667] = VPosNormalTex(0.6125,0.0990607,-0.00632625,-0.89784,0.113964,-0.425317,-0.0101502,0.19375);
vpnt[1668] = VPosNormalTex(0.6125,0.109061,-0.00364676,-0.89784,0.113964,-0.425317,-0.00531983,0.19375);
vpnt[1669] = VPosNormalTex(0.56875,0.0420167,0.0707447,-0.806507,-0.15302,-0.571079,0.164703,0.215625);
vpnt[1670] = VPosNormalTex(0.56875,0.0320167,0.0734242,-0.806507,-0.15302,-0.571079,0.184557,0.215625);
vpnt[1671] = VPosNormalTex(0.6125,0.0890607,-0.00364676,-0.806507,-0.15302,-0.571079,-0.00651327,0.19375);
vpnt[1672] = VPosNormalTex(0.6125,0.0990607,-0.00632625,-0.806507,-0.15302,-0.571079,-0.0101502,0.19375);
vpnt[1673] = VPosNormalTex(0.56875,0.0320167,0.0734242,-0.307952,-0.672743,-0.672743,0.184557,0.215625);
vpnt[1674] = VPosNormalTex(0.56875,0.0246962,0.0807447,-0.307952,-0.672743,-0.672743,0.20276,0.215625);
vpnt[1675] = VPosNormalTex(0.6125,0.0817402,0.00367375,-0.307952,-0.672743,-0.672743,0.00714827,0.19375);
vpnt[1676] = VPosNormalTex(0.6125,0.0890607,-0.00364676,-0.307952,-0.672743,-0.672743,-0.00651327,0.19375);
vpnt[1677] = VPosNormalTex(0.56875,0.0246962,0.0807447,0.626355,-0.752976,-0.201759,0.20276,0.215625);
vpnt[1678] = VPosNormalTex(0.56875,0.0220167,0.0907447,0.626355,-0.752976,-0.201759,0.212118,0.215625);
vpnt[1679] = VPosNormalTex(0.6125,0.0790607,0.0136737,0.626355,-0.752976,-0.201759,0.0272566,0.19375);
vpnt[1680] = VPosNormalTex(0.6125,0.0817402,0.00367375,0.626355,-0.752976,-0.201759,0.00714827,0.19375);
vpnt[1681] = VPosNormalTex(0.56875,0.0220167,0.0907447,0.863918,-0.486471,0.13035,0.212118,0.215625);
vpnt[1682] = VPosNormalTex(0.56875,0.0246962,0.100745,0.863918,-0.486471,0.13035,0.21174,0.215625);
vpnt[1683] = VPosNormalTex(0.6125,0.0817402,0.0236737,0.863918,-0.486471,0.13035,0.0448671,0.19375);
vpnt[1684] = VPosNormalTex(0.6125,0.0790607,0.0136737,0.863918,-0.486471,0.13035,0.0272566,0.19375);
vpnt[1685] = VPosNormalTex(0.56875,0.0246962,0.100745,0.90803,-0.296211,0.296211,0.21174,0.215625);
vpnt[1686] = VPosNormalTex(0.56875,0.0320167,0.108065,0.90803,-0.296211,0.296211,0.204158,0.215625);
vpnt[1687] = VPosNormalTex(0.6125,0.0890607,0.0309943,0.90803,-0.296211,0.296211,0.0533015,0.19375);
vpnt[1688] = VPosNormalTex(0.6125,0.0817402,0.0236737,0.90803,-0.296211,0.296211,0.0448671,0.19375);
vpnt[1689] = VPosNormalTex(0.56875,0.0320167,0.108065,0.89784,-0.113964,0.425317,0.204158,0.215625);
vpnt[1690] = VPosNormalTex(0.56875,0.0420167,0.110745,0.89784,-0.113964,0.425317,0.192287,0.215625);
vpnt[1691] = VPosNormalTex(0.6125,0.0990607,0.0336737,0.89784,-0.113964,0.425317,0.0521513,0.19375);
vpnt[1692] = VPosNormalTex(0.6125,0.0890607,0.0309943,0.89784,-0.113964,0.425317,0.0533015,0.19375);
vpnt[1693] = VPosNormalTex(0.6125,0.0990607,0.0336737,0.908968,0.107893,0.402661,0.0521513,0.19375);
vpnt[1694] = VPosNormalTex(0.6125,0.109061,0.0309943,0.908968,0.107893,0.402661,0.0440688,0.19375);
vpnt[1695] = VPosNormalTex(0.65625,0.0750288,-0.0586483,0.908968,0.107893,0.402661,-0.105594,0.171875);
vpnt[1696] = VPosNormalTex(0.65625,0.0650288,-0.0559688,0.908968,0.107893,0.402661,-0.113105,0.171875);
vpnt[1697] = VPosNormalTex(0.6125,0.109061,0.0309943,0.894327,0.316369,0.316369,0.0440688,0.19375);
vpnt[1698] = VPosNormalTex(0.6125,0.116381,0.0236737,0.894327,0.316369,0.316369,0.0319388,0.19375);
vpnt[1699] = VPosNormalTex(0.65625,0.0823493,-0.0659688,0.894327,0.316369,0.316369,-0.107494,0.171875);
vpnt[1700] = VPosNormalTex(0.65625,0.0750288,-0.0586483,0.894327,0.316369,0.316369,-0.105594,0.171875);
vpnt[1701] = VPosNormalTex(0.6125,0.116381,0.0236737,0.788416,0.594182,0.15921,0.0319388,0.19375);
vpnt[1702] = VPosNormalTex(0.6125,0.119061,0.0136737,0.788416,0.594182,0.15921,0.0181987,0.19375);
vpnt[1703] = VPosNormalTex(0.65625,0.0850288,-0.0759688,0.788416,0.594182,0.15921,-0.116053,0.171875);
vpnt[1704] = VPosNormalTex(0.65625,0.0823493,-0.0659688,0.788416,0.594182,0.15921,-0.107494,0.171875);
vpnt[1705] = VPosNormalTex(0.6125,0.119061,0.0136737,0.215844,0.943157,-0.252718,0.0181987,0.19375);
vpnt[1706] = VPosNormalTex(0.6125,0.116381,0.00367375,0.215844,0.943157,-0.252718,0.00502229,0.19375);
vpnt[1707] = VPosNormalTex(0.65625,0.0823493,-0.0859688,0.215844,0.943157,-0.252718,-0.128422,0.171875);
vpnt[1708] = VPosNormalTex(0.65625,0.0850288,-0.0759688,0.215844,0.943157,-0.252718,-0.116053,0.171875);
vpnt[1709] = VPosNormalTex(0.6125,0.116381,0.00367375,-0.668473,0.525901,-0.525901,0.00502229,0.19375);
vpnt[1710] = VPosNormalTex(0.6125,0.109061,-0.00364676,-0.668473,0.525901,-0.525901,-0.00531983,0.19375);
vpnt[1711] = VPosNormalTex(0.65625,0.0750288,-0.0932893,-0.668473,0.525901,-0.525901,-0.142199,0.171875);
vpnt[1712] = VPosNormalTex(0.65625,0.0823493,-0.0859688,-0.668473,0.525901,-0.525901,-0.128422,0.171875);
vpnt[1713] = VPosNormalTex(0.6125,0.109061,-0.00364676,-0.871581,0.126886,-0.473546,-0.00531983,0.19375);
vpnt[1714] = VPosNormalTex(0.6125,0.0990607,-0.00632625,-0.871581,0.126886,-0.473546,-0.0101502,0.19375);
vpnt[1715] = VPosNormalTex(0.65625,0.0650288,-0.0959688,-0.871581,0.126886,-0.473546,-0.155217,0.171875);
vpnt[1716] = VPosNormalTex(0.65625,0.0750288,-0.0932893,-0.871581,0.126886,-0.473546,-0.142199,0.171875);
vpnt[1717] = VPosNormalTex(0.6125,0.0990607,-0.00632625,-0.908968,-0.107893,-0.402661,-0.0101502,0.19375);
vpnt[1718] = VPosNormalTex(0.6125,0.0890607,-0.00364676,-0.908968,-0.107893,-0.402661,-0.00651327,0.19375);
vpnt[1719] = VPosNormalTex(0.65625,0.0550288,-0.0932893,-0.908968,-0.107893,-0.402661,-0.16518,0.171875);
vpnt[1720] = VPosNormalTex(0.65625,0.0650288,-0.0959688,-0.908968,-0.107893,-0.402661,-0.155217,0.171875);
vpnt[1721] = VPosNormalTex(0.6125,0.0890607,-0.00364676,-0.894327,-0.316369,-0.316369,-0.00651327,0.19375);
vpnt[1722] = VPosNormalTex(0.6125,0.0817402,0.00367375,-0.894327,-0.316369,-0.316369,0.00714827,0.19375);
vpnt[1723] = VPosNormalTex(0.65625,0.0477083,-0.0859688,-0.894327,-0.316369,-0.316369,-0.169367,0.171875);
vpnt[1724] = VPosNormalTex(0.65625,0.0550288,-0.0932893,-0.894327,-0.316369,-0.316369,-0.16518,0.171875);
vpnt[1725] = VPosNormalTex(0.6125,0.0817402,0.00367375,-0.788416,-0.594182,-0.15921,0.00714827,0.19375);
vpnt[1726] = VPosNormalTex(0.6125,0.0790607,0.0136737,-0.788416,-0.594182,-0.15921,0.0272566,0.19375);
vpnt[1727] = VPosNormalTex(0.65625,0.0450288,-0.0759688,-0.788416,-0.594182,-0.15921,-0.164843,0.171875);
vpnt[1728] = VPosNormalTex(0.65625,0.0477083,-0.0859688,-0.788416,-0.594182,-0.15921,-0.169367,0.171875);
vpnt[1729] = VPosNormalTex(0.6125,0.0790607,0.0136737,-0.215844,-0.943157,0.252718,0.0272566,0.19375);
vpnt[1730] = VPosNormalTex(0.6125,0.0817402,0.0236737,-0.215844,-0.943157,0.252718,0.0448671,0.19375);
vpnt[1731] = VPosNormalTex(0.65625,0.0477083,-0.0659688,-0.215844,-0.943157,0.252718,-0.150349,0.171875);
vpnt[1732] = VPosNormalTex(0.65625,0.0450288,-0.0759688,-0.215844,-0.943157,0.252718,-0.164843,0.171875);
vpnt[1733] = VPosNormalTex(0.6125,0.0817402,0.0236737,0.668473,-0.525901,0.525901,0.0448671,0.19375);
vpnt[1734] = VPosNormalTex(0.6125,0.0890607,0.0309943,0.668473,-0.525901,0.525901,0.0533015,0.19375);
vpnt[1735] = VPosNormalTex(0.65625,0.0550288,-0.0586483,0.668473,-0.525901,0.525901,-0.130066,0.171875);
vpnt[1736] = VPosNormalTex(0.65625,0.0477083,-0.0659688,0.668473,-0.525901,0.525901,-0.150349,0.171875);
vpnt[1737] = VPosNormalTex(0.6125,0.0890607,0.0309943,0.871581,-0.126886,0.473546,0.0533015,0.19375);
vpnt[1738] = VPosNormalTex(0.6125,0.0990607,0.0336737,0.871581,-0.126886,0.473546,0.0521513,0.19375);
vpnt[1739] = VPosNormalTex(0.65625,0.0650288,-0.0559688,0.871581,-0.126886,0.473546,-0.113105,0.171875);
vpnt[1740] = VPosNormalTex(0.65625,0.0550288,-0.0586483,0.871581,-0.126886,0.473546,-0.130066,0.171875);
vpnt[1741] = VPosNormalTex(0.65625,0.0650288,-0.0559688,0.7043,0.183736,0.685713,-0.113105,0.171875);
vpnt[1742] = VPosNormalTex(0.65625,0.0750288,-0.0586483,0.7043,0.183736,0.685713,-0.105594,0.171875);
vpnt[1743] = VPosNormalTex(0.7,-0.0187904,-0.0784454,0.7043,0.183736,0.685713,-0.287418,0.15);
vpnt[1744] = VPosNormalTex(0.7,-0.0287904,-0.0757659,0.7043,0.183736,0.685713,-0.307795,0.15);
vpnt[1745] = VPosNormalTex(0.65625,0.0750288,-0.0586483,0.878223,0.338175,0.338175,-0.105594,0.171875);
vpnt[1746] = VPosNormalTex(0.65625,0.0823493,-0.0659688,0.878223,0.338175,0.338175,-0.107494,0.171875);
vpnt[1747] = VPosNormalTex(0.7,-0.0114698,-0.0857659,0.878223,0.338175,0.338175,-0.271159,0.15);
vpnt[1748] = VPosNormalTex(0.7,-0.0187904,-0.0784454,0.878223,0.338175,0.338175,-0.287418,0.15);
vpnt[1749] = VPosNormalTex(0.65625,0.0823493,-0.0659688,0.909545,0.401444,0.107566,-0.107494,0.171875);
vpnt[1750] = VPosNormalTex(0.65625,0.0850288,-0.0759688,0.909545,0.401444,0.107566,-0.116053,0.171875);
vpnt[1751] = VPosNormalTex(0.7,-0.00879036,-0.0957659,0.909545,0.401444,0.107566,-0.264568,0.15);
vpnt[1752] = VPosNormalTex(0.7,-0.0114698,-0.0857659,0.909545,0.401444,0.107566,-0.271159,0.15);
vpnt[1753] = VPosNormalTex(0.65625,0.0850288,-0.0759688,0.89022,0.440008,-0.1179,-0.116053,0.171875);
vpnt[1754] = VPosNormalTex(0.65625,0.0823493,-0.0859688,0.89022,0.440008,-0.1179,-0.128422,0.171875);
vpnt[1755] = VPosNormalTex(0.7,-0.0114698,-0.105766,0.89022,0.440008,-0.1179,-0.267192,0.15);
vpnt[1756] = VPosNormalTex(0.7,-0.00879036,-0.0957659,0.89022,0.440008,-0.1179,-0.264568,0.15);
vpnt[1757] = VPosNormalTex(0.65625,0.0823493,-0.0859688,0.767268,0.453486,-0.453486,-0.128422,0.171875);
vpnt[1758] = VPosNormalTex(0.65625,0.0750288,-0.0932893,0.767268,0.453486,-0.453486,-0.142199,0.171875);
vpnt[1759] = VPosNormalTex(0.7,-0.0187904,-0.113086,0.767268,0.453486,-0.453486,-0.276206,0.15);
vpnt[1760] = VPosNormalTex(0.7,-0.0114698,-0.105766,0.767268,0.453486,-0.453486,-0.267192,0.15);
vpnt[1761] = VPosNormalTex(0.65625,0.0750288,-0.0932893,0.117123,0.257038,-0.959278,-0.142199,0.171875);
vpnt[1762] = VPosNormalTex(0.65625,0.0650288,-0.0959688,0.117123,0.257038,-0.959278,-0.155217,0.171875);
vpnt[1763] = VPosNormalTex(0.7,-0.0287904,-0.115766,0.117123,0.257038,-0.959278,-0.288794,0.15);
vpnt[1764] = VPosNormalTex(0.7,-0.0187904,-0.113086,0.117123,0.257038,-0.959278,-0.276206,0.15);
vpnt[1765] = VPosNormalTex(0.65625,0.0650288,-0.0959688,-0.704301,-0.183736,-0.685712,-0.155217,0.171875);
vpnt[1766] = VPosNormalTex(0.65625,0.0550288,-0.0932893,-0.704301,-0.183736,-0.685712,-0.16518,0.171875);
vpnt[1767] = VPosNormalTex(0.7,-0.0387904,-0.113086,-0.704301,-0.183736,-0.685712,-0.302591,0.15);
vpnt[1768] = VPosNormalTex(0.7,-0.0287904,-0.115766,-0.704301,-0.183736,-0.685712,-0.288794,0.15);
vpnt[1769] = VPosNormalTex(0.65625,0.0550288,-0.0932893,-0.878223,-0.338175,-0.338175,-0.16518,0.171875);
vpnt[1770] = VPosNormalTex(0.65625,0.0477083,-0.0859688,-0.878223,-0.338175,-0.338175,-0.169367,0.171875);
vpnt[1771] = VPosNormalTex(0.7,-0.0461109,-0.105766,-0.878223,-0.338175,-0.338175,-0.315433,0.15);
vpnt[1772] = VPosNormalTex(0.7,-0.0387904,-0.113086,-0.878223,-0.338175,-0.338175,-0.302591,0.15);
vpnt[1773] = VPosNormalTex(0.65625,0.0477083,-0.0859688,-0.909545,-0.401443,-0.107567,-0.169367,0.171875);
vpnt[1774] = VPosNormalTex(0.65625,0.0450288,-0.0759688,-0.909545,-0.401443,-0.107567,-0.164843,0.171875);
vpnt[1775] = VPosNormalTex(0.7,-0.0487904,-0.0957659,-0.909545,-0.401443,-0.107567,-0.324994,0.15);
vpnt[1776] = VPosNormalTex(0.7,-0.0461109,-0.105766,-0.909545,-0.401443,-0.107567,-0.315433,0.15);
vpnt[1777] = VPosNormalTex(0.65625,0.0450288,-0.0759688,-0.89022,-0.440008,0.1179,-0.164843,0.171875);
vpnt[1778] = VPosNormalTex(0.65625,0.0477083,-0.0659688,-0.89022,-0.440008,0.1179,-0.150349,0.171875);
vpnt[1779] = VPosNormalTex(0.7,-0.0461109,-0.0857659,-0.89022,-0.440008,0.1179,-0.328511,0.15);
vpnt[1780] = VPosNormalTex(0.7,-0.0487904,-0.0957659,-0.89022,-0.440008,0.1179,-0.324994,0.15);
vpnt[1781] = VPosNormalTex(0.65625,0.0477083,-0.0659688,-0.767268,-0.453486,0.453486,-0.150349,0.171875);
vpnt[1782] = VPosNormalTex(0.65625,0.0550288,-0.0586483,-0.767268,-0.453486,0.453486,-0.130066,0.171875);
vpnt[1783] = VPosNormalTex(0.7,-0.0387904,-0.0784454,-0.767268,-0.453486,0.453486,-0.323088,0.15);
vpnt[1784] = VPosNormalTex(0.7,-0.0461109,-0.0857659,-0.767268,-0.453486,0.453486,-0.328511,0.15);
vpnt[1785] = VPosNormalTex(0.65625,0.0550288,-0.0586483,-0.117123,-0.257038,0.959278,-0.130066,0.171875);
vpnt[1786] = VPosNormalTex(0.65625,0.0650288,-0.0559688,-0.117123,-0.257038,0.959278,-0.113105,0.171875);
vpnt[1787] = VPosNormalTex(0.7,-0.0287904,-0.0757659,-0.117123,-0.257038,0.959278,-0.307795,0.15);
vpnt[1788] = VPosNormalTex(0.7,-0.0387904,-0.0784454,-0.117123,-0.257038,0.959278,-0.323088,0.15);
vpnt[1789] = VPosNormalTex(0.7,-0.0287904,-0.0757659,-0.742482,0.173374,0.647041,-0.307795,0.15);
vpnt[1790] = VPosNormalTex(0.7,-0.0187904,-0.0784454,-0.742482,0.173374,0.647041,-0.287418,0.15);
vpnt[1791] = VPosNormalTex(0.74375,-0.0861398,-0.0101958,-0.742482,0.173374,0.647041,-0.481249,0.128125);
vpnt[1792] = VPosNormalTex(0.74375,-0.0961398,-0.00751629,-0.742482,0.173374,0.647041,-0.487582,0.128125);
vpnt[1793] = VPosNormalTex(0.7,-0.0187904,-0.0784454,-0.0145487,0.707032,0.707032,-0.287418,0.15);
vpnt[1794] = VPosNormalTex(0.7,-0.0114698,-0.0857659,-0.0145487,0.707032,0.707032,-0.271159,0.15);
vpnt[1795] = VPosNormalTex(0.74375,-0.0788193,-0.0175163,-0.0145487,0.707032,0.707032,-0.465196,0.128125);
vpnt[1796] = VPosNormalTex(0.74375,-0.0861398,-0.0101958,-0.0145487,0.707032,0.707032,-0.481249,0.128125);
vpnt[1797] = VPosNormalTex(0.7,-0.0114698,-0.0857659,0.734763,0.655211,0.175563,-0.271159,0.15);
vpnt[1798] = VPosNormalTex(0.7,-0.00879036,-0.0957659,0.734763,0.655211,0.175563,-0.264568,0.15);
vpnt[1799] = VPosNormalTex(0.74375,-0.0761398,-0.0275163,0.734763,0.655211,0.175563,-0.444807,0.128125);
vpnt[1800] = VPosNormalTex(0.74375,-0.0788193,-0.0175163,0.734763,0.655211,0.175563,-0.465196,0.128125);
vpnt[1801] = VPosNormalTex(0.7,-0.00879036,-0.0957659,0.883975,0.451604,-0.121007,-0.264568,0.15);
vpnt[1802] = VPosNormalTex(0.7,-0.0114698,-0.105766,0.883975,0.451604,-0.121007,-0.267192,0.15);
vpnt[1803] = VPosNormalTex(0.74375,-0.0788193,-0.0375163,0.883975,0.451604,-0.121007,-0.429296,0.128125);
vpnt[1804] = VPosNormalTex(0.74375,-0.0761398,-0.0275163,0.883975,0.451604,-0.121007,-0.444807,0.128125);
vpnt[1805] = VPosNormalTex(0.7,-0.0114698,-0.105766,0.909769,0.29353,-0.29353,-0.267192,0.15);
vpnt[1806] = VPosNormalTex(0.7,-0.0187904,-0.113086,0.909769,0.29353,-0.29353,-0.276206,0.15);
vpnt[1807] = VPosNormalTex(0.74375,-0.0861398,-0.0448368,0.909769,0.29353,-0.29353,-0.423618,0.128125);
vpnt[1808] = VPosNormalTex(0.74375,-0.0788193,-0.0375163,0.909769,0.29353,-0.29353,-0.429296,0.128125);
vpnt[1809] = VPosNormalTex(0.7,-0.0187904,-0.113086,0.885448,0.120283,-0.448902,-0.276206,0.15);
vpnt[1810] = VPosNormalTex(0.7,-0.0287904,-0.115766,0.885448,0.120283,-0.448902,-0.288794,0.15);
vpnt[1811] = VPosNormalTex(0.74375,-0.0961398,-0.0475163,0.885448,0.120283,-0.448902,-0.426943,0.128125);
vpnt[1812] = VPosNormalTex(0.74375,-0.0861398,-0.0448368,0.885448,0.120283,-0.448902,-0.423618,0.128125);
vpnt[1813] = VPosNormalTex(0.7,-0.0287904,-0.115766,0.742482,-0.173374,-0.64704,-0.288794,0.15);
vpnt[1814] = VPosNormalTex(0.7,-0.0387904,-0.113086,0.742482,-0.173374,-0.64704,-0.302591,0.15);
vpnt[1815] = VPosNormalTex(0.74375,-0.10614,-0.0448368,0.742482,-0.173374,-0.64704,-0.436387,0.128125);
vpnt[1816] = VPosNormalTex(0.74375,-0.0961398,-0.0475163,0.742482,-0.173374,-0.64704,-0.426943,0.128125);
vpnt[1817] = VPosNormalTex(0.7,-0.0387904,-0.113086,0.0145486,-0.707032,-0.707032,-0.302591,0.15);
vpnt[1818] = VPosNormalTex(0.7,-0.0461109,-0.105766,0.0145486,-0.707032,-0.707032,-0.315433,0.15);
vpnt[1819] = VPosNormalTex(0.74375,-0.11346,-0.0375163,0.0145486,-0.707032,-0.707032,-0.449176,0.128125);
vpnt[1820] = VPosNormalTex(0.74375,-0.10614,-0.0448368,0.0145486,-0.707032,-0.707032,-0.436387,0.128125);
vpnt[1821] = VPosNormalTex(0.7,-0.0461109,-0.105766,-0.734763,-0.655211,-0.175563,-0.315433,0.15);
vpnt[1822] = VPosNormalTex(0.7,-0.0487904,-0.0957659,-0.734763,-0.655211,-0.175563,-0.324994,0.15);
vpnt[1823] = VPosNormalTex(0.74375,-0.11614,-0.0275163,-0.734763,-0.655211,-0.175563,-0.462975,0.128125);
vpnt[1824] = VPosNormalTex(0.74375,-0.11346,-0.0375163,-0.734763,-0.655211,-0.175563,-0.449176,0.128125);
vpnt[1825] = VPosNormalTex(0.7,-0.0487904,-0.0957659,-0.883975,-0.451604,0.121007,-0.324994,0.15);
vpnt[1826] = VPosNormalTex(0.7,-0.0461109,-0.0857659,-0.883975,-0.451604,0.121007,-0.328511,0.15);
vpnt[1827] = VPosNormalTex(0.74375,-0.11346,-0.0175163,-0.883975,-0.451604,0.121007,-0.475622,0.128125);
vpnt[1828] = VPosNormalTex(0.74375,-0.11614,-0.0275163,-0.883975,-0.451604,0.121007,-0.462975,0.128125);
vpnt[1829] = VPosNormalTex(0.7,-0.0461109,-0.0857659,-0.909769,-0.29353,0.29353,-0.328511,0.15);
vpnt[1830] = VPosNormalTex(0.7,-0.0387904,-0.0784454,-0.909769,-0.29353,0.29353,-0.323088,0.15);
vpnt[1831] = VPosNormalTex(0.74375,-0.10614,-0.0101958,-0.909769,-0.29353,0.29353,-0.484758,0.128125);
vpnt[1832] = VPosNormalTex(0.74375,-0.11346,-0.0175163,-0.909769,-0.29353,0.29353,-0.475622,0.128125);
vpnt[1833] = VPosNormalTex(0.7,-0.0387904,-0.0784454,-0.885448,-0.120283,0.448902,-0.323088,0.15);
vpnt[1834] = VPosNormalTex(0.7,-0.0287904,-0.0757659,-0.885448,-0.120283,0.448902,-0.307795,0.15);
vpnt[1835] = VPosNormalTex(0.74375,-0.0961398,-0.00751629,-0.885448,-0.120283,0.448902,-0.487582,0.128125);
vpnt[1836] = VPosNormalTex(0.74375,-0.10614,-0.0101958,-0.885448,-0.120283,0.448902,-0.484758,0.128125);
vpnt[1837] = VPosNormalTex(0.74375,-0.0961398,-0.00751629,-0.909644,0.107511,0.401235,-0.487582,0.128125);
vpnt[1838] = VPosNormalTex(0.74375,-0.0861398,-0.0101958,-0.909644,0.107511,0.401235,-0.481249,0.128125);
vpnt[1839] = VPosNormalTex(0.7875,-0.0650988,0.0833521,-0.909644,0.107511,0.401235,0.355528,0.10625);
vpnt[1840] = VPosNormalTex(0.7875,-0.0750988,0.0860316,-0.909644,0.107511,0.401235,0.364218,0.10625);
vpnt[1841] = VPosNormalTex(0.74375,-0.0861398,-0.0101958,-0.879926,0.335955,0.335955,-0.481249,0.128125);
vpnt[1842] = VPosNormalTex(0.74375,-0.0788193,-0.0175163,-0.879926,0.335955,0.335955,-0.465196,0.128125);
vpnt[1843] = VPosNormalTex(0.7875,-0.0577782,0.0760316,-0.879926,0.335955,0.335955,0.353422,0.10625);
vpnt[1844] = VPosNormalTex(0.7875,-0.0650988,0.0833521,-0.879926,0.335955,0.335955,0.355528,0.10625);
vpnt[1845] = VPosNormalTex(0.74375,-0.0788193,-0.0175163,-0.713375,0.676904,0.181376,-0.465196,0.128125);
vpnt[1846] = VPosNormalTex(0.74375,-0.0761398,-0.0275163,-0.713375,0.676904,0.181376,-0.444807,0.128125);
vpnt[1847] = VPosNormalTex(0.7875,-0.0550988,0.0660316,-0.713375,0.676904,0.181376,0.360674,0.10625);
vpnt[1848] = VPosNormalTex(0.7875,-0.0577782,0.0760316,-0.713375,0.676904,0.181376,0.353422,0.10625);
vpnt[1849] = VPosNormalTex(0.74375,-0.0761398,-0.0275163,0.0885178,0.962134,-0.257803,-0.444807,0.128125);
vpnt[1850] = VPosNormalTex(0.74375,-0.0788193,-0.0375163,0.0885178,0.962134,-0.257803,-0.429296,0.128125);
vpnt[1851] = VPosNormalTex(0.7875,-0.0577782,0.0560316,0.0885178,0.962134,-0.257803,0.377442,0.10625);
vpnt[1852] = VPosNormalTex(0.7875,-0.0550988,0.0660316,0.0885178,0.962134,-0.257803,0.360674,0.10625);
vpnt[1853] = VPosNormalTex(0.74375,-0.0788193,-0.0375163,0.760691,0.458993,-0.458993,-0.429296,0.128125);
vpnt[1854] = VPosNormalTex(0.74375,-0.0861398,-0.0448368,0.760691,0.458993,-0.458993,-0.423618,0.128125);
vpnt[1855] = VPosNormalTex(0.7875,-0.0650988,0.0487111,0.760691,0.458993,-0.458993,0.39776,0.10625);
vpnt[1856] = VPosNormalTex(0.7875,-0.0577782,0.0560316,0.760691,0.458993,-0.458993,0.377442,0.10625);
vpnt[1857] = VPosNormalTex(0.74375,-0.0861398,-0.0448368,0.888949,0.118541,-0.442401,-0.423618,0.128125);
vpnt[1858] = VPosNormalTex(0.74375,-0.0961398,-0.0475163,0.888949,0.118541,-0.442401,-0.426943,0.128125);
vpnt[1859] = VPosNormalTex(0.7875,-0.0750988,0.0460316,0.888949,0.118541,-0.442401,0.412483,0.10625);
vpnt[1860] = VPosNormalTex(0.7875,-0.0650988,0.0487111,0.888949,0.118541,-0.442401,0.39776,0.10625);
vpnt[1861] = VPosNormalTex(0.74375,-0.0961398,-0.0475163,0.909644,-0.107511,-0.401235,-0.426943,0.128125);
vpnt[1862] = VPosNormalTex(0.74375,-0.10614,-0.0448368,0.909644,-0.107511,-0.401235,-0.436387,0.128125);
vpnt[1863] = VPosNormalTex(0.7875,-0.0850988,0.0487111,0.909644,-0.107511,-0.401235,0.417258,0.10625);
vpnt[1864] = VPosNormalTex(0.7875,-0.0750988,0.0460316,0.909644,-0.107511,-0.401235,0.412483,0.10625);
vpnt[1865] = VPosNormalTex(0.74375,-0.10614,-0.0448368,0.879926,-0.335955,-0.335955,-0.436387,0.128125);
vpnt[1866] = VPosNormalTex(0.74375,-0.11346,-0.0375163,0.879926,-0.335955,-0.335955,-0.449176,0.128125);
vpnt[1867] = VPosNormalTex(0.7875,-0.0924193,0.0560316,0.879926,-0.335955,-0.335955,0.413257,0.10625);
vpnt[1868] = VPosNormalTex(0.7875,-0.0850988,0.0487111,0.879926,-0.335955,-0.335955,0.417258,0.10625);
vpnt[1869] = VPosNormalTex(0.74375,-0.11346,-0.0375163,0.713375,-0.676904,-0.181376,-0.449176,0.128125);
vpnt[1870] = VPosNormalTex(0.74375,-0.11614,-0.0275163,0.713375,-0.676904,-0.181376,-0.462975,0.128125);
vpnt[1871] = VPosNormalTex(0.7875,-0.0950987,0.0660316,0.713375,-0.676904,-0.181376,0.403405,0.10625);
vpnt[1872] = VPosNormalTex(0.7875,-0.0924193,0.0560316,0.713375,-0.676904,-0.181376,0.413257,0.10625);
vpnt[1873] = VPosNormalTex(0.74375,-0.11614,-0.0275163,-0.0885179,-0.962134,0.257803,-0.462975,0.128125);
vpnt[1874] = VPosNormalTex(0.74375,-0.11346,-0.0175163,-0.0885179,-0.962134,0.257803,-0.475622,0.128125);
vpnt[1875] = VPosNormalTex(0.7875,-0.0924193,0.0760316,-0.0885179,-0.962134,0.257803,0.390435,0.10625);
vpnt[1876] = VPosNormalTex(0.7875,-0.0950987,0.0660316,-0.0885179,-0.962134,0.257803,0.403405,0.10625);
vpnt[1877] = VPosNormalTex(0.74375,-0.11346,-0.0175163,-0.760691,-0.458993,0.458993,-0.475622,0.128125);
vpnt[1878] = VPosNormalTex(0.74375,-0.10614,-0.0101958,-0.760691,-0.458993,0.458993,-0.484758,0.128125);
vpnt[1879] = VPosNormalTex(0.7875,-0.0850988,0.0833521,-0.760691,-0.458993,0.458993,0.37665,0.10625);
vpnt[1880] = VPosNormalTex(0.7875,-0.0924193,0.0760316,-0.760691,-0.458993,0.458993,0.390435,0.10625);
vpnt[1881] = VPosNormalTex(0.74375,-0.10614,-0.0101958,-0.888949,-0.118541,0.442401,-0.484758,0.128125);
vpnt[1882] = VPosNormalTex(0.74375,-0.0961398,-0.00751629,-0.888949,-0.118541,0.442401,-0.487582,0.128125);
vpnt[1883] = VPosNormalTex(0.7875,-0.0750988,0.0860316,-0.888949,-0.118541,0.442401,0.364218,0.10625);
vpnt[1884] = VPosNormalTex(0.7875,-0.0850988,0.0833521,-0.888949,-0.118541,0.442401,0.37665,0.10625);
vpnt[1885] = VPosNormalTex(0.7875,-0.0750988,0.0860316,0.620919,0.202882,0.757165,0.364218,0.10625);
vpnt[1886] = VPosNormalTex(0.7875,-0.0650988,0.0833521,0.620919,0.202882,0.757165,0.355528,0.10625);
vpnt[1887] = VPosNormalTex(0.83125,0.0114613,0.0269604,0.620919,0.202882,0.757165,0.186025,0.084375);
vpnt[1888] = VPosNormalTex(0.83125,0.0014613,0.0296399,0.620919,0.202882,0.757165,0.24216,0.084375);
vpnt[1889] = VPosNormalTex(0.7875,-0.0650988,0.0833521,-0.309919,0.672291,0.672291,0.355528,0.10625);
vpnt[1890] = VPosNormalTex(0.7875,-0.0577782,0.0760316,-0.309919,0.672291,0.672291,0.353422,0.10625);
vpnt[1891] = VPosNormalTex(0.83125,0.0187818,0.0196399,-0.309919,0.672291,0.672291,0.128554,0.084375);
vpnt[1892] = VPosNormalTex(0.83125,0.0114613,0.0269604,-0.309919,0.672291,0.672291,0.186025,0.084375);
vpnt[1893] = VPosNormalTex(0.7875,-0.0577782,0.0760316,-0.804965,0.573105,0.153563,0.353422,0.10625);
vpnt[1894] = VPosNormalTex(0.7875,-0.0550988,0.0660316,-0.804965,0.573105,0.153563,0.360674,0.10625);
vpnt[1895] = VPosNormalTex(0.83125,0.0214613,0.00963987,-0.804965,0.573105,0.153563,0.06719,0.084375);
vpnt[1896] = VPosNormalTex(0.83125,0.0187818,0.0196399,-0.804965,0.573105,0.153563,0.128554,0.084375);
vpnt[1897] = VPosNormalTex(0.7875,-0.0550988,0.0660316,-0.896536,0.427876,-0.114649,0.360674,0.10625);
vpnt[1898] = VPosNormalTex(0.7875,-0.0577782,0.0560316,-0.896536,0.427876,-0.114649,0.377442,0.10625);
vpnt[1899] = VPosNormalTex(0.83125,0.0187818,-0.000360128,-0.896536,0.427876,-0.114649,-0.00305131,0.084375);
vpnt[1900] = VPosNormalTex(0.83125,0.0214613,0.00963987,-0.896536,0.427876,-0.114649,0.06719,0.084375);
vpnt[1901] = VPosNormalTex(0.7875,-0.0577782,0.0560316,-0.906633,0.298343,-0.298343,0.377442,0.10625);
vpnt[1902] = VPosNormalTex(0.7875,-0.0650988,0.0487111,-0.906633,0.298343,-0.298343,0.39776,0.10625);
vpnt[1903] = VPosNormalTex(0.83125,0.0114613,-0.00768064,-0.906633,0.298343,-0.298343,-0.0939652,0.084375);
vpnt[1904] = VPosNormalTex(0.83125,0.0187818,-0.000360128,-0.906633,0.298343,-0.298343,-0.00305131,0.084375);
vpnt[1905] = VPosNormalTex(0.7875,-0.0650988,0.0487111,-0.861667,0.131344,-0.490182,0.39776,0.10625);
vpnt[1906] = VPosNormalTex(0.7875,-0.0750988,0.0460316,-0.861667,0.131344,-0.490182,0.412483,0.10625);
vpnt[1907] = VPosNormalTex(0.83125,0.0014613,-0.0103601,-0.861667,0.131344,-0.490182,-0.227698,0.084375);
vpnt[1908] = VPosNormalTex(0.83125,0.0114613,-0.00768064,-0.861667,0.131344,-0.490182,-0.0939652,0.084375);
vpnt[1909] = VPosNormalTex(0.7875,-0.0750988,0.0460316,-0.620919,-0.202882,-0.757165,0.412483,0.10625);
vpnt[1910] = VPosNormalTex(0.7875,-0.0850988,0.0487111,-0.620919,-0.202882,-0.757165,0.417258,0.10625);
vpnt[1911] = VPosNormalTex(0.83125,-0.0085387,-0.00768064,-0.620919,-0.202882,-0.757165,-0.383412,0.084375);
vpnt[1912] = VPosNormalTex(0.83125,0.0014613,-0.0103601,-0.620919,-0.202882,-0.757165,-0.227698,0.084375);
vpnt[1913] = VPosNormalTex(0.7875,-0.0850988,0.0487111,0.309919,-0.672291,-0.672291,0.417258,0.10625);
vpnt[1914] = VPosNormalTex(0.7875,-0.0924193,0.0560316,0.309919,-0.672291,-0.672291,0.413257,0.10625);
vpnt[1915] = VPosNormalTex(0.83125,-0.0158592,-0.000360128,0.309919,-0.672291,-0.672291,-0.496387,0.084375);
vpnt[1916] = VPosNormalTex(0.83125,-0.0085387,-0.00768064,0.309919,-0.672291,-0.672291,-0.383412,0.084375);
vpnt[1917] = VPosNormalTex(0.7875,-0.0924193,0.0560316,0.804965,-0.573105,-0.153563,0.413257,0.10625);
vpnt[1918] = VPosNormalTex(0.7875,-0.0950987,0.0660316,0.804965,-0.573105,-0.153563,0.403405,0.10625);
vpnt[1919] = VPosNormalTex(0.83125,-0.0185387,0.00963987,0.804965,-0.573105,-0.153563,0.423684,0.084375);
vpnt[1920] = VPosNormalTex(0.83125,-0.0158592,-0.000360128,0.804965,-0.573105,-0.153563,-0.496387,0.084375);
vpnt[1921] = VPosNormalTex(0.7875,-0.0950987,0.0660316,0.896536,-0.427876,0.114649,0.403405,0.10625);
vpnt[1922] = VPosNormalTex(0.7875,-0.0924193,0.0760316,0.896536,-0.427876,0.114649,0.390435,0.10625);
vpnt[1923] = VPosNormalTex(0.83125,-0.0158592,0.0196399,0.896536,-0.427876,0.114649,0.358114,0.084375);
vpnt[1924] = VPosNormalTex(0.83125,-0.0185387,0.00963987,0.896536,-0.427876,0.114649,0.423684,0.084375);
vpnt[1925] = VPosNormalTex(0.7875,-0.0924193,0.0760316,0.906633,-0.298343,0.298343,0.390435,0.10625);
vpnt[1926] = VPosNormalTex(0.7875,-0.0850988,0.0833521,0.906633,-0.298343,0.298343,0.37665,0.10625);
vpnt[1927] = VPosNormalTex(0.83125,-0.0085387,0.0269604,0.906633,-0.298343,0.298343,0.298816,0.084375);
vpnt[1928] = VPosNormalTex(0.83125,-0.0158592,0.0196399,0.906633,-0.298343,0.298343,0.358114,0.084375);
vpnt[1929] = VPosNormalTex(0.7875,-0.0850988,0.0833521,0.861667,-0.131344,0.490183,0.37665,0.10625);
vpnt[1930] = VPosNormalTex(0.7875,-0.0750988,0.0860316,0.861667,-0.131344,0.490183,0.364218,0.10625);
vpnt[1931] = VPosNormalTex(0.83125,0.0014613,0.0296399,0.861667,-0.131344,0.490183,0.24216,0.084375);
vpnt[1932] = VPosNormalTex(0.83125,-0.0085387,0.0269604,0.861667,-0.131344,0.490183,0.298816,0.084375);
vpnt[1933] = VPosNormalTex(0.83125,0.0014613,0.0296399,0.216237,0.252696,0.943073,0.24216,0.084375);
vpnt[1934] = VPosNormalTex(0.83125,0.0114613,0.0269604,0.216237,0.252696,0.943073,0.186025,0.084375);
vpnt[1935] = VPosNormalTex(0.875,0.01,0.0173205,0.216237,0.252696,0.943073,0.166667,0.0625);
vpnt[1936] = VPosNormalTex(0.875,0,0.02,0.216237,0.252696,0.943073,0.25,0.0625);
vpnt[1937] = VPosNormalTex(0.83125,0.0114613,0.0269604,0.176602,0.695993,0.695993,0.186025,0.084375);
vpnt[1938] = VPosNormalTex(0.83125,0.0187818,0.0196399,0.176602,0.695993,0.695993,0.128554,0.084375);
vpnt[1939] = VPosNormalTex(0.875,0.0173205,0.01,0.176602,0.695993,0.695993,0.0833333,0.0625);
vpnt[1940] = VPosNormalTex(0.875,0.01,0.0173205,0.176602,0.695993,0.695993,0.166667,0.0625);
vpnt[1941] = VPosNormalTex(0.83125,0.0187818,0.0196399,0.0889374,0.962098,0.257793,0.128554,0.084375);
vpnt[1942] = VPosNormalTex(0.83125,0.0214613,0.00963987,0.0889374,0.962098,0.257793,0.06719,0.084375);
vpnt[1943] = VPosNormalTex(0.875,0.02,0,0.0889374,0.962098,0.257793,0,0.0625);
vpnt[1944] = VPosNormalTex(0.875,0.0173205,0.01,0.0889374,0.962098,0.257793,0.0833333,0.0625);
vpnt[1945] = VPosNormalTex(0.83125,0.0214613,0.00963987,-0.0247575,0.96563,-0.25874,0.06719,0.084375);
vpnt[1946] = VPosNormalTex(0.83125,0.0187818,-0.000360128,-0.0247575,0.96563,-0.25874,-0.00305131,0.084375);
vpnt[1947] = VPosNormalTex(0.875,0.0173205,-0.01,-0.0247575,0.96563,-0.25874,-0.0833333,0.0625);
vpnt[1948] = VPosNormalTex(0.875,0.02,0,-0.0247575,0.96563,-0.25874,0,0.0625);
vpnt[1949] = VPosNormalTex(0.83125,0.0187818,-0.000360128,-0.131046,0.701009,-0.701009,-0.00305131,0.084375);
vpnt[1950] = VPosNormalTex(0.83125,0.0114613,-0.00768064,-0.131046,0.701009,-0.701009,-0.0939652,0.084375);
vpnt[1951] = VPosNormalTex(0.875,0.01,-0.0173205,-0.131046,0.701009,-0.701009,-0.166667,0.0625);
vpnt[1952] = VPosNormalTex(0.875,0.0173205,-0.01,-0.131046,0.701009,-0.701009,-0.0833333,0.0625);
vpnt[1953] = VPosNormalTex(0.83125,0.0114613,-0.00768064,-0.200059,0.253587,-0.946398,-0.0939652,0.084375);
vpnt[1954] = VPosNormalTex(0.83125,0.0014613,-0.0103601,-0.200059,0.253587,-0.946398,-0.227698,0.084375);
vpnt[1955] = VPosNormalTex(0.875,0,-0.02,-0.200059,0.253587,-0.946398,-0.25,0.0625);
vpnt[1956] = VPosNormalTex(0.875,0.01,-0.0173205,-0.200059,0.253587,-0.946398,-0.166667,0.0625);
vpnt[1957] = VPosNormalTex(0.83125,0.0014613,-0.0103601,-0.216237,-0.252696,-0.943073,-0.227698,0.084375);
vpnt[1958] = VPosNormalTex(0.83125,-0.0085387,-0.00768064,-0.216237,-0.252696,-0.943073,-0.383412,0.084375);
vpnt[1959] = VPosNormalTex(0.875,-0.01,-0.0173205,-0.216237,-0.252696,-0.943073,-0.333333,0.0625);
vpnt[1960] = VPosNormalTex(0.875,0,-0.02,-0.216237,-0.252696,-0.943073,-0.25,0.0625);
vpnt[1961] = VPosNormalTex(0.83125,-0.0085387,-0.00768064,-0.176602,-0.695993,-0.695993,-0.383412,0.084375);
vpnt[1962] = VPosNormalTex(0.83125,-0.0158592,-0.000360128,-0.176602,-0.695993,-0.695993,-0.496387,0.084375);
vpnt[1963] = VPosNormalTex(0.875,-0.0173205,-0.01,-0.176602,-0.695993,-0.695993,-0.416667,0.0625);
vpnt[1964] = VPosNormalTex(0.875,-0.01,-0.0173205,-0.176602,-0.695993,-0.695993,-0.333333,0.0625);
vpnt[1965] = VPosNormalTex(0.83125,-0.0158592,-0.000360128,-0.0889374,-0.962098,-0.257793,-0.496387,0.084375);
vpnt[1966] = VPosNormalTex(0.83125,-0.0185387,0.00963987,-0.0889374,-0.962098,-0.257793,0.423684,0.084375);
vpnt[1967] = VPosNormalTex(0.875,-0.02,0,-0.0889374,-0.962098,-0.257793,0.5,0.0625);
vpnt[1968] = VPosNormalTex(0.875,-0.0173205,-0.01,-0.0889374,-0.962098,-0.257793,-0.416667,0.0625);
vpnt[1969] = VPosNormalTex(0.83125,-0.0185387,0.00963987,0.0247575,-0.96563,0.25874,0.423684,0.084375);
vpnt[1970] = VPosNormalTex(0.83125,-0.0158592,0.0196399,0.0247575,-0.96563,0.25874,0.358114,0.084375);
vpnt[1971] = VPosNormalTex(0.875,-0.0173205,0.01,0.0247575,-0.96563,0.25874,0.416667,0.0625);
vpnt[1972] = VPosNormalTex(0.875,-0.02,0,0.0247575,-0.96563,0.25874,0.5,0.0625);
vpnt[1973] = VPosNormalTex(0.83125,-0.0158592,0.0196399,0.131046,-0.701009,0.701009,0.358114,0.084375);
vpnt[1974] = VPosNormalTex(0.83125,-0.0085387,0.0269604,0.131046,-0.701009,0.701009,0.298816,0.084375);
vpnt[1975] = VPosNormalTex(0.875,-0.01,0.0173205,0.131046,-0.701009,0.701009,0.333333,0.0625);
vpnt[1976] = VPosNormalTex(0.875,-0.0173205,0.01,0.131046,-0.701009,0.701009,0.416667,0.0625);
vpnt[1977] = VPosNormalTex(0.83125,-0.0085387,0.0269604,0.200059,-0.253587,0.946398,0.298816,0.084375);
vpnt[1978] = VPosNormalTex(0.83125,0.0014613,0.0296399,0.200059,-0.253587,0.946398,0.24216,0.084375);
vpnt[1979] = VPosNormalTex(0.875,0,0.02,0.200059,-0.253587,0.946398,0.25,0.0625);
vpnt[1980] = VPosNormalTex(0.875,-0.01,0.0173205,0.200059,-0.253587,0.946398,0.333333,0.0625);
vpnt[1981] = VPosNormalTex(0.875,0,0.02,-0,0.258819,0.965926,0.25,0.0625);
vpnt[1982] = VPosNormalTex(0.875,0.01,0.0173205,-0,0.258819,0.965926,0.166667,0.0625);
vpnt[1983] = VPosNormalTex(1,0.01,0.0173205,-0,0.258819,0.965926,0.166667,-0);
vpnt[1984] = VPosNormalTex(1,0,0.02,-0,0.258819,0.965926,0.25,-0);
vpnt[1985] = VPosNormalTex(0.875,0.01,0.0173205,-0,0.707107,0.707107,0.166667,0.0625);
vpnt[1986] = VPosNormalTex(0.875,0.0173205,0.01,-0,0.707107,0.707107,0.0833333,0.0625);
vpnt[1987] = VPosNormalTex(1,0.0173205,0.01,-0,0.707107,0.707107,0.0833333,-0);
vpnt[1988] = VPosNormalTex(1,0.01,0.0173205,-0,0.707107,0.707107,0.166667,-0);
vpnt[1989] = VPosNormalTex(0.875,0.0173205,0.01,-0,0.965926,0.258819,0.0833333,0.0625);
vpnt[1990] = VPosNormalTex(0.875,0.02,0,-0,0.965926,0.258819,0,0.0625);
vpnt[1991] = VPosNormalTex(1,0.02,0,-0,0.965926,0.258819,0,-0);
vpnt[1992] = VPosNormalTex(1,0.0173205,0.01,-0,0.965926,0.258819,0.0833333,-0);
vpnt[1993] = VPosNormalTex(0.875,0.02,0,0,0.965926,-0.258819,0,0.0625);
vpnt[1994] = VPosNormalTex(0.875,0.0173205,-0.01,0,0.965926,-0.258819,-0.0833333,0.0625);
vpnt[1995] = VPosNormalTex(1,0.0173205,-0.01,0,0.965926,-0.258819,-0.0833333,-0);
vpnt[1996] = VPosNormalTex(1,0.02,0,0,0.965926,-0.258819,0,-0);
vpnt[1997] = VPosNormalTex(0.875,0.0173205,-0.01,0,0.707107,-0.707107,-0.0833333,0.0625);
vpnt[1998] = VPosNormalTex(0.875,0.01,-0.0173205,0,0.707107,-0.707107,-0.166667,0.0625);
vpnt[1999] = VPosNormalTex(1,0.01,-0.0173205,0,0.707107,-0.707107,-0.166667,-0);
vpnt[2000] = VPosNormalTex(1,0.0173205,-0.01,0,0.707107,-0.707107,-0.0833333,-0);
vpnt[2001] = VPosNormalTex(0.875,0.01,-0.0173205,0,0.258819,-0.965926,-0.166667,0.0625);
vpnt[2002] = VPosNormalTex(0.875,0,-0.02,0,0.258819,-0.965926,-0.25,0.0625);
vpnt[2003] = VPosNormalTex(1,0,-0.02,0,0.258819,-0.965926,-0.25,-0);
vpnt[2004] = VPosNormalTex(1,0.01,-0.0173205,0,0.258819,-0.965926,-0.166667,-0);
vpnt[2005] = VPosNormalTex(0.875,0,-0.02,0,-0.258819,-0.965926,-0.25,0.0625);
vpnt[2006] = VPosNormalTex(0.875,-0.01,-0.0173205,0,-0.258819,-0.965926,-0.333333,0.0625);
vpnt[2007] = VPosNormalTex(1,-0.01,-0.0173205,0,-0.258819,-0.965926,-0.333333,-0);
vpnt[2008] = VPosNormalTex(1,0,-0.02,0,-0.258819,-0.965926,-0.25,-0);
vpnt[2009] = VPosNormalTex(0.875,-0.01,-0.0173205,0,-0.707107,-0.707107,-0.333333,0.0625);
vpnt[2010] = VPosNormalTex(0.875,-0.0173205,-0.01,0,-0.707107,-0.707107,-0.416667,0.0625);
vpnt[2011] = VPosNormalTex(1,-0.0173205,-0.01,0,-0.707107,-0.707107,-0.416667,-0);
vpnt[2012] = VPosNormalTex(1,-0.01,-0.0173205,0,-0.707107,-0.707107,-0.333333,-0);
vpnt[2013] = VPosNormalTex(0.875,-0.0173205,-0.01,0,-0.965926,-0.258819,-0.416667,0.0625);
vpnt[2014] = VPosNormalTex(0.875,-0.02,0,0,-0.965926,-0.258819,0.5,0.0625);
vpnt[2015] = VPosNormalTex(1,-0.02,0,0,-0.965926,-0.258819,0.5,-0);
vpnt[2016] = VPosNormalTex(1,-0.0173205,-0.01,0,-0.965926,-0.258819,-0.416667,-0);
vpnt[2017] = VPosNormalTex(0.875,-0.02,0,0,-0.965926,0.258819,0.5,0.0625);
vpnt[2018] = VPosNormalTex(0.875,-0.0173205,0.01,0,-0.965926,0.258819,0.416667,0.0625);
vpnt[2019] = VPosNormalTex(1,-0.0173205,0.01,0,-0.965926,0.258819,0.416667,-0);
vpnt[2020] = VPosNormalTex(1,-0.02,0,0,-0.965926,0.258819,0.5,-0);
vpnt[2021] = VPosNormalTex(0.875,-0.0173205,0.01,0,-0.707107,0.707107,0.416667,0.0625);
vpnt[2022] = VPosNormalTex(0.875,-0.01,0.0173205,0,-0.707107,0.707107,0.333333,0.0625);
vpnt[2023] = VPosNormalTex(1,-0.01,0.0173205,0,-0.707107,0.707107,0.333333,-0);
vpnt[2024] = VPosNormalTex(1,-0.0173205,0.01,0,-0.707107,0.707107,0.416667,-0);
vpnt[2025] = VPosNormalTex(0.875,-0.01,0.0173205,0,-0.258819,0.965926,0.333333,0.0625);
vpnt[2026] = VPosNormalTex(0.875,0,0.02,0,-0.258819,0.965926,0.25,0.0625);
vpnt[2027] = VPosNormalTex(1,0,0.02,0,-0.258819,0.965926,0.25,-0);
vpnt[2028] = VPosNormalTex(1,-0.01,0.0173205,0,-0.258819,0.965926,0.333333,-0);
vpnt[2029] = VPosNormalTex(1,0,0.02,1,0,0,0.25,-0);
vpnt[2030] = VPosNormalTex(1,0.01,0.0173205,1,0,0,0.166667,-0);
vpnt[2031] = VPosNormalTex(1,0.0173205,0.01,1,0,0,0.0833333,-0);
vpnt[2032] = VPosNormalTex(1,0.02,0,1,0,0,0,-0);
vpnt[2033] = VPosNormalTex(1,0.0173205,-0.01,1,0,0,-0.0833333,-0);
vpnt[2034] = VPosNormalTex(1,0.01,-0.0173205,1,0,0,-0.166667,-0);
vpnt[2035] = VPosNormalTex(1,0,-0.02,1,0,0,-0.25,-0);
vpnt[2036] = VPosNormalTex(1,-0.01,-0.0173205,1,0,0,-0.333333,-0);
vpnt[2037] = VPosNormalTex(1,-0.0173205,-0.01,1,0,0,-0.416667,-0);
vpnt[2038] = VPosNormalTex(1,-0.02,0,1,0,0,0.5,-0);
vpnt[2039] = VPosNormalTex(1,-0.0173205,0.01,1,0,0,0.416667,-0);
vpnt[2040] = VPosNormalTex(1,-0.01,0.0173205,1,0,0,0.333333,-0);
vpnt[2041] = VPosNormalTex(1,0,0,1,0,0,0,-0);


	if(FAILED(mVBLTSpring->Unlock()))
		MessageBox(NULL, "mVBLTBox->Unlock() failed", "", MB_OK);
	
	
	if(FAILED(md3dDevice->CreateIndexBuffer(3096*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIBLTSpring, 0)))
		MessageBox(NULL, "CreateIndexBuffer() failed", "", MB_OK);
		
	WORD* indice = 0;
	if(FAILED(mIBLTSpring->Lock(0, 0, (void**)&indice, 0)))
		MessageBox(NULL, "mIBLTBox->Lock() failed", NULL, MB_OK);
		
	
indice[0] = 0;
indice[1] = 2;
indice[2] = 1;
indice[3] = 0;
indice[4] = 3;
indice[5] = 2;
indice[6] = 0;
indice[7] = 4;
indice[8] = 3;
indice[9] = 0;
indice[10] = 5;
indice[11] = 4;
indice[12] = 0;
indice[13] = 6;
indice[14] = 5;
indice[15] = 0;
indice[16] = 7;
indice[17] = 6;
indice[18] = 0;
indice[19] = 8;
indice[20] = 7;
indice[21] = 0;
indice[22] = 9;
indice[23] = 8;
indice[24] = 0;
indice[25] = 10;
indice[26] = 9;
indice[27] = 0;
indice[28] = 11;
indice[29] = 10;
indice[30] = 0;
indice[31] = 12;
indice[32] = 11;
indice[33] = 0;
indice[34] = 1;
indice[35] = 12;
indice[36] = 13;
indice[37] = 15;
indice[38] = 14;
indice[39] = 13;
indice[40] = 16;
indice[41] = 15;
indice[42] = 17;
indice[43] = 19;
indice[44] = 18;
indice[45] = 17;
indice[46] = 20;
indice[47] = 19;
indice[48] = 21;
indice[49] = 23;
indice[50] = 22;
indice[51] = 21;
indice[52] = 24;
indice[53] = 23;
indice[54] = 25;
indice[55] = 27;
indice[56] = 26;
indice[57] = 25;
indice[58] = 28;
indice[59] = 27;
indice[60] = 29;
indice[61] = 31;
indice[62] = 30;
indice[63] = 29;
indice[64] = 32;
indice[65] = 31;
indice[66] = 33;
indice[67] = 35;
indice[68] = 34;
indice[69] = 33;
indice[70] = 36;
indice[71] = 35;
indice[72] = 37;
indice[73] = 39;
indice[74] = 38;
indice[75] = 37;
indice[76] = 40;
indice[77] = 39;
indice[78] = 41;
indice[79] = 43;
indice[80] = 42;
indice[81] = 41;
indice[82] = 44;
indice[83] = 43;
indice[84] = 45;
indice[85] = 47;
indice[86] = 46;
indice[87] = 45;
indice[88] = 48;
indice[89] = 47;
indice[90] = 49;
indice[91] = 51;
indice[92] = 50;
indice[93] = 49;
indice[94] = 52;
indice[95] = 51;
indice[96] = 53;
indice[97] = 55;
indice[98] = 54;
indice[99] = 53;
indice[100] = 56;
indice[101] = 55;
indice[102] = 57;
indice[103] = 59;
indice[104] = 58;
indice[105] = 57;
indice[106] = 60;
indice[107] = 59;
indice[108] = 61;
indice[109] = 63;
indice[110] = 62;
indice[111] = 61;
indice[112] = 64;
indice[113] = 63;
indice[114] = 65;
indice[115] = 67;
indice[116] = 66;
indice[117] = 65;
indice[118] = 68;
indice[119] = 67;
indice[120] = 69;
indice[121] = 71;
indice[122] = 70;
indice[123] = 69;
indice[124] = 72;
indice[125] = 71;
indice[126] = 73;
indice[127] = 75;
indice[128] = 74;
indice[129] = 73;
indice[130] = 76;
indice[131] = 75;
indice[132] = 77;
indice[133] = 79;
indice[134] = 78;
indice[135] = 77;
indice[136] = 80;
indice[137] = 79;
indice[138] = 81;
indice[139] = 83;
indice[140] = 82;
indice[141] = 81;
indice[142] = 84;
indice[143] = 83;
indice[144] = 85;
indice[145] = 87;
indice[146] = 86;
indice[147] = 85;
indice[148] = 88;
indice[149] = 87;
indice[150] = 89;
indice[151] = 91;
indice[152] = 90;
indice[153] = 89;
indice[154] = 92;
indice[155] = 91;
indice[156] = 93;
indice[157] = 95;
indice[158] = 94;
indice[159] = 93;
indice[160] = 96;
indice[161] = 95;
indice[162] = 97;
indice[163] = 99;
indice[164] = 98;
indice[165] = 97;
indice[166] = 100;
indice[167] = 99;
indice[168] = 101;
indice[169] = 103;
indice[170] = 102;
indice[171] = 101;
indice[172] = 104;
indice[173] = 103;
indice[174] = 105;
indice[175] = 107;
indice[176] = 106;
indice[177] = 105;
indice[178] = 108;
indice[179] = 107;
indice[180] = 109;
indice[181] = 111;
indice[182] = 110;
indice[183] = 109;
indice[184] = 112;
indice[185] = 111;
indice[186] = 113;
indice[187] = 115;
indice[188] = 114;
indice[189] = 113;
indice[190] = 116;
indice[191] = 115;
indice[192] = 117;
indice[193] = 119;
indice[194] = 118;
indice[195] = 117;
indice[196] = 120;
indice[197] = 119;
indice[198] = 121;
indice[199] = 123;
indice[200] = 122;
indice[201] = 121;
indice[202] = 124;
indice[203] = 123;
indice[204] = 125;
indice[205] = 127;
indice[206] = 126;
indice[207] = 125;
indice[208] = 128;
indice[209] = 127;
indice[210] = 129;
indice[211] = 131;
indice[212] = 130;
indice[213] = 129;
indice[214] = 132;
indice[215] = 131;
indice[216] = 133;
indice[217] = 135;
indice[218] = 134;
indice[219] = 133;
indice[220] = 136;
indice[221] = 135;
indice[222] = 137;
indice[223] = 139;
indice[224] = 138;
indice[225] = 137;
indice[226] = 140;
indice[227] = 139;
indice[228] = 141;
indice[229] = 143;
indice[230] = 142;
indice[231] = 141;
indice[232] = 144;
indice[233] = 143;
indice[234] = 145;
indice[235] = 147;
indice[236] = 146;
indice[237] = 145;
indice[238] = 148;
indice[239] = 147;
indice[240] = 149;
indice[241] = 151;
indice[242] = 150;
indice[243] = 149;
indice[244] = 152;
indice[245] = 151;
indice[246] = 153;
indice[247] = 155;
indice[248] = 154;
indice[249] = 153;
indice[250] = 156;
indice[251] = 155;
indice[252] = 157;
indice[253] = 159;
indice[254] = 158;
indice[255] = 157;
indice[256] = 160;
indice[257] = 159;
indice[258] = 161;
indice[259] = 163;
indice[260] = 162;
indice[261] = 161;
indice[262] = 164;
indice[263] = 163;
indice[264] = 165;
indice[265] = 167;
indice[266] = 166;
indice[267] = 165;
indice[268] = 168;
indice[269] = 167;
indice[270] = 169;
indice[271] = 171;
indice[272] = 170;
indice[273] = 169;
indice[274] = 172;
indice[275] = 171;
indice[276] = 173;
indice[277] = 175;
indice[278] = 174;
indice[279] = 173;
indice[280] = 176;
indice[281] = 175;
indice[282] = 177;
indice[283] = 179;
indice[284] = 178;
indice[285] = 177;
indice[286] = 180;
indice[287] = 179;
indice[288] = 181;
indice[289] = 183;
indice[290] = 182;
indice[291] = 181;
indice[292] = 184;
indice[293] = 183;
indice[294] = 185;
indice[295] = 187;
indice[296] = 186;
indice[297] = 185;
indice[298] = 188;
indice[299] = 187;
indice[300] = 189;
indice[301] = 191;
indice[302] = 190;
indice[303] = 189;
indice[304] = 192;
indice[305] = 191;
indice[306] = 193;
indice[307] = 195;
indice[308] = 194;
indice[309] = 193;
indice[310] = 196;
indice[311] = 195;
indice[312] = 197;
indice[313] = 199;
indice[314] = 198;
indice[315] = 197;
indice[316] = 200;
indice[317] = 199;
indice[318] = 201;
indice[319] = 203;
indice[320] = 202;
indice[321] = 201;
indice[322] = 204;
indice[323] = 203;
indice[324] = 205;
indice[325] = 207;
indice[326] = 206;
indice[327] = 205;
indice[328] = 208;
indice[329] = 207;
indice[330] = 209;
indice[331] = 211;
indice[332] = 210;
indice[333] = 209;
indice[334] = 212;
indice[335] = 211;
indice[336] = 213;
indice[337] = 215;
indice[338] = 214;
indice[339] = 213;
indice[340] = 216;
indice[341] = 215;
indice[342] = 217;
indice[343] = 219;
indice[344] = 218;
indice[345] = 217;
indice[346] = 220;
indice[347] = 219;
indice[348] = 221;
indice[349] = 223;
indice[350] = 222;
indice[351] = 221;
indice[352] = 224;
indice[353] = 223;
indice[354] = 225;
indice[355] = 227;
indice[356] = 226;
indice[357] = 225;
indice[358] = 228;
indice[359] = 227;
indice[360] = 229;
indice[361] = 231;
indice[362] = 230;
indice[363] = 229;
indice[364] = 232;
indice[365] = 231;
indice[366] = 233;
indice[367] = 235;
indice[368] = 234;
indice[369] = 233;
indice[370] = 236;
indice[371] = 235;
indice[372] = 237;
indice[373] = 239;
indice[374] = 238;
indice[375] = 237;
indice[376] = 240;
indice[377] = 239;
indice[378] = 241;
indice[379] = 243;
indice[380] = 242;
indice[381] = 241;
indice[382] = 244;
indice[383] = 243;
indice[384] = 245;
indice[385] = 247;
indice[386] = 246;
indice[387] = 245;
indice[388] = 248;
indice[389] = 247;
indice[390] = 249;
indice[391] = 251;
indice[392] = 250;
indice[393] = 249;
indice[394] = 252;
indice[395] = 251;
indice[396] = 253;
indice[397] = 255;
indice[398] = 254;
indice[399] = 253;
indice[400] = 256;
indice[401] = 255;
indice[402] = 257;
indice[403] = 259;
indice[404] = 258;
indice[405] = 257;
indice[406] = 260;
indice[407] = 259;
indice[408] = 261;
indice[409] = 263;
indice[410] = 262;
indice[411] = 261;
indice[412] = 264;
indice[413] = 263;
indice[414] = 265;
indice[415] = 267;
indice[416] = 266;
indice[417] = 265;
indice[418] = 268;
indice[419] = 267;
indice[420] = 269;
indice[421] = 271;
indice[422] = 270;
indice[423] = 269;
indice[424] = 272;
indice[425] = 271;
indice[426] = 273;
indice[427] = 275;
indice[428] = 274;
indice[429] = 273;
indice[430] = 276;
indice[431] = 275;
indice[432] = 277;
indice[433] = 279;
indice[434] = 278;
indice[435] = 277;
indice[436] = 280;
indice[437] = 279;
indice[438] = 281;
indice[439] = 283;
indice[440] = 282;
indice[441] = 281;
indice[442] = 284;
indice[443] = 283;
indice[444] = 285;
indice[445] = 287;
indice[446] = 286;
indice[447] = 285;
indice[448] = 288;
indice[449] = 287;
indice[450] = 289;
indice[451] = 291;
indice[452] = 290;
indice[453] = 289;
indice[454] = 292;
indice[455] = 291;
indice[456] = 293;
indice[457] = 295;
indice[458] = 294;
indice[459] = 293;
indice[460] = 296;
indice[461] = 295;
indice[462] = 297;
indice[463] = 299;
indice[464] = 298;
indice[465] = 297;
indice[466] = 300;
indice[467] = 299;
indice[468] = 301;
indice[469] = 303;
indice[470] = 302;
indice[471] = 301;
indice[472] = 304;
indice[473] = 303;
indice[474] = 305;
indice[475] = 307;
indice[476] = 306;
indice[477] = 305;
indice[478] = 308;
indice[479] = 307;
indice[480] = 309;
indice[481] = 311;
indice[482] = 310;
indice[483] = 309;
indice[484] = 312;
indice[485] = 311;
indice[486] = 313;
indice[487] = 315;
indice[488] = 314;
indice[489] = 313;
indice[490] = 316;
indice[491] = 315;
indice[492] = 317;
indice[493] = 319;
indice[494] = 318;
indice[495] = 317;
indice[496] = 320;
indice[497] = 319;
indice[498] = 321;
indice[499] = 323;
indice[500] = 322;
indice[501] = 321;
indice[502] = 324;
indice[503] = 323;
indice[504] = 325;
indice[505] = 327;
indice[506] = 326;
indice[507] = 325;
indice[508] = 328;
indice[509] = 327;
indice[510] = 329;
indice[511] = 331;
indice[512] = 330;
indice[513] = 329;
indice[514] = 332;
indice[515] = 331;
indice[516] = 333;
indice[517] = 335;
indice[518] = 334;
indice[519] = 333;
indice[520] = 336;
indice[521] = 335;
indice[522] = 337;
indice[523] = 339;
indice[524] = 338;
indice[525] = 337;
indice[526] = 340;
indice[527] = 339;
indice[528] = 341;
indice[529] = 343;
indice[530] = 342;
indice[531] = 341;
indice[532] = 344;
indice[533] = 343;
indice[534] = 345;
indice[535] = 347;
indice[536] = 346;
indice[537] = 345;
indice[538] = 348;
indice[539] = 347;
indice[540] = 349;
indice[541] = 351;
indice[542] = 350;
indice[543] = 349;
indice[544] = 352;
indice[545] = 351;
indice[546] = 353;
indice[547] = 355;
indice[548] = 354;
indice[549] = 353;
indice[550] = 356;
indice[551] = 355;
indice[552] = 357;
indice[553] = 359;
indice[554] = 358;
indice[555] = 357;
indice[556] = 360;
indice[557] = 359;
indice[558] = 361;
indice[559] = 363;
indice[560] = 362;
indice[561] = 361;
indice[562] = 364;
indice[563] = 363;
indice[564] = 365;
indice[565] = 367;
indice[566] = 366;
indice[567] = 365;
indice[568] = 368;
indice[569] = 367;
indice[570] = 369;
indice[571] = 371;
indice[572] = 370;
indice[573] = 369;
indice[574] = 372;
indice[575] = 371;
indice[576] = 373;
indice[577] = 375;
indice[578] = 374;
indice[579] = 373;
indice[580] = 376;
indice[581] = 375;
indice[582] = 377;
indice[583] = 379;
indice[584] = 378;
indice[585] = 377;
indice[586] = 380;
indice[587] = 379;
indice[588] = 381;
indice[589] = 383;
indice[590] = 382;
indice[591] = 381;
indice[592] = 384;
indice[593] = 383;
indice[594] = 385;
indice[595] = 387;
indice[596] = 386;
indice[597] = 385;
indice[598] = 388;
indice[599] = 387;
indice[600] = 389;
indice[601] = 391;
indice[602] = 390;
indice[603] = 389;
indice[604] = 392;
indice[605] = 391;
indice[606] = 393;
indice[607] = 395;
indice[608] = 394;
indice[609] = 393;
indice[610] = 396;
indice[611] = 395;
indice[612] = 397;
indice[613] = 399;
indice[614] = 398;
indice[615] = 397;
indice[616] = 400;
indice[617] = 399;
indice[618] = 401;
indice[619] = 403;
indice[620] = 402;
indice[621] = 401;
indice[622] = 404;
indice[623] = 403;
indice[624] = 405;
indice[625] = 407;
indice[626] = 406;
indice[627] = 405;
indice[628] = 408;
indice[629] = 407;
indice[630] = 409;
indice[631] = 411;
indice[632] = 410;
indice[633] = 409;
indice[634] = 412;
indice[635] = 411;
indice[636] = 413;
indice[637] = 415;
indice[638] = 414;
indice[639] = 413;
indice[640] = 416;
indice[641] = 415;
indice[642] = 417;
indice[643] = 419;
indice[644] = 418;
indice[645] = 417;
indice[646] = 420;
indice[647] = 419;
indice[648] = 421;
indice[649] = 423;
indice[650] = 422;
indice[651] = 421;
indice[652] = 424;
indice[653] = 423;
indice[654] = 425;
indice[655] = 427;
indice[656] = 426;
indice[657] = 425;
indice[658] = 428;
indice[659] = 427;
indice[660] = 429;
indice[661] = 431;
indice[662] = 430;
indice[663] = 429;
indice[664] = 432;
indice[665] = 431;
indice[666] = 433;
indice[667] = 435;
indice[668] = 434;
indice[669] = 433;
indice[670] = 436;
indice[671] = 435;
indice[672] = 437;
indice[673] = 439;
indice[674] = 438;
indice[675] = 437;
indice[676] = 440;
indice[677] = 439;
indice[678] = 441;
indice[679] = 443;
indice[680] = 442;
indice[681] = 441;
indice[682] = 444;
indice[683] = 443;
indice[684] = 445;
indice[685] = 447;
indice[686] = 446;
indice[687] = 445;
indice[688] = 448;
indice[689] = 447;
indice[690] = 449;
indice[691] = 451;
indice[692] = 450;
indice[693] = 449;
indice[694] = 452;
indice[695] = 451;
indice[696] = 453;
indice[697] = 455;
indice[698] = 454;
indice[699] = 453;
indice[700] = 456;
indice[701] = 455;
indice[702] = 457;
indice[703] = 459;
indice[704] = 458;
indice[705] = 457;
indice[706] = 460;
indice[707] = 459;
indice[708] = 461;
indice[709] = 463;
indice[710] = 462;
indice[711] = 461;
indice[712] = 464;
indice[713] = 463;
indice[714] = 465;
indice[715] = 467;
indice[716] = 466;
indice[717] = 465;
indice[718] = 468;
indice[719] = 467;
indice[720] = 469;
indice[721] = 471;
indice[722] = 470;
indice[723] = 469;
indice[724] = 472;
indice[725] = 471;
indice[726] = 473;
indice[727] = 475;
indice[728] = 474;
indice[729] = 473;
indice[730] = 476;
indice[731] = 475;
indice[732] = 477;
indice[733] = 479;
indice[734] = 478;
indice[735] = 477;
indice[736] = 480;
indice[737] = 479;
indice[738] = 481;
indice[739] = 483;
indice[740] = 482;
indice[741] = 481;
indice[742] = 484;
indice[743] = 483;
indice[744] = 485;
indice[745] = 487;
indice[746] = 486;
indice[747] = 485;
indice[748] = 488;
indice[749] = 487;
indice[750] = 489;
indice[751] = 491;
indice[752] = 490;
indice[753] = 489;
indice[754] = 492;
indice[755] = 491;
indice[756] = 493;
indice[757] = 495;
indice[758] = 494;
indice[759] = 493;
indice[760] = 496;
indice[761] = 495;
indice[762] = 497;
indice[763] = 499;
indice[764] = 498;
indice[765] = 497;
indice[766] = 500;
indice[767] = 499;
indice[768] = 501;
indice[769] = 503;
indice[770] = 502;
indice[771] = 501;
indice[772] = 504;
indice[773] = 503;
indice[774] = 505;
indice[775] = 507;
indice[776] = 506;
indice[777] = 505;
indice[778] = 508;
indice[779] = 507;
indice[780] = 509;
indice[781] = 511;
indice[782] = 510;
indice[783] = 509;
indice[784] = 512;
indice[785] = 511;
indice[786] = 513;
indice[787] = 515;
indice[788] = 514;
indice[789] = 513;
indice[790] = 516;
indice[791] = 515;
indice[792] = 517;
indice[793] = 519;
indice[794] = 518;
indice[795] = 517;
indice[796] = 520;
indice[797] = 519;
indice[798] = 521;
indice[799] = 523;
indice[800] = 522;
indice[801] = 521;
indice[802] = 524;
indice[803] = 523;
indice[804] = 525;
indice[805] = 527;
indice[806] = 526;
indice[807] = 525;
indice[808] = 528;
indice[809] = 527;
indice[810] = 529;
indice[811] = 531;
indice[812] = 530;
indice[813] = 529;
indice[814] = 532;
indice[815] = 531;
indice[816] = 533;
indice[817] = 535;
indice[818] = 534;
indice[819] = 533;
indice[820] = 536;
indice[821] = 535;
indice[822] = 537;
indice[823] = 539;
indice[824] = 538;
indice[825] = 537;
indice[826] = 540;
indice[827] = 539;
indice[828] = 541;
indice[829] = 543;
indice[830] = 542;
indice[831] = 541;
indice[832] = 544;
indice[833] = 543;
indice[834] = 545;
indice[835] = 547;
indice[836] = 546;
indice[837] = 545;
indice[838] = 548;
indice[839] = 547;
indice[840] = 549;
indice[841] = 551;
indice[842] = 550;
indice[843] = 549;
indice[844] = 552;
indice[845] = 551;
indice[846] = 553;
indice[847] = 555;
indice[848] = 554;
indice[849] = 553;
indice[850] = 556;
indice[851] = 555;
indice[852] = 557;
indice[853] = 559;
indice[854] = 558;
indice[855] = 557;
indice[856] = 560;
indice[857] = 559;
indice[858] = 561;
indice[859] = 563;
indice[860] = 562;
indice[861] = 561;
indice[862] = 564;
indice[863] = 563;
indice[864] = 565;
indice[865] = 567;
indice[866] = 566;
indice[867] = 565;
indice[868] = 568;
indice[869] = 567;
indice[870] = 569;
indice[871] = 571;
indice[872] = 570;
indice[873] = 569;
indice[874] = 572;
indice[875] = 571;
indice[876] = 573;
indice[877] = 575;
indice[878] = 574;
indice[879] = 573;
indice[880] = 576;
indice[881] = 575;
indice[882] = 577;
indice[883] = 579;
indice[884] = 578;
indice[885] = 577;
indice[886] = 580;
indice[887] = 579;
indice[888] = 581;
indice[889] = 583;
indice[890] = 582;
indice[891] = 581;
indice[892] = 584;
indice[893] = 583;
indice[894] = 585;
indice[895] = 587;
indice[896] = 586;
indice[897] = 585;
indice[898] = 588;
indice[899] = 587;
indice[900] = 589;
indice[901] = 591;
indice[902] = 590;
indice[903] = 589;
indice[904] = 592;
indice[905] = 591;
indice[906] = 593;
indice[907] = 595;
indice[908] = 594;
indice[909] = 593;
indice[910] = 596;
indice[911] = 595;
indice[912] = 597;
indice[913] = 599;
indice[914] = 598;
indice[915] = 597;
indice[916] = 600;
indice[917] = 599;
indice[918] = 601;
indice[919] = 603;
indice[920] = 602;
indice[921] = 601;
indice[922] = 604;
indice[923] = 603;
indice[924] = 605;
indice[925] = 607;
indice[926] = 606;
indice[927] = 605;
indice[928] = 608;
indice[929] = 607;
indice[930] = 609;
indice[931] = 611;
indice[932] = 610;
indice[933] = 609;
indice[934] = 612;
indice[935] = 611;
indice[936] = 613;
indice[937] = 615;
indice[938] = 614;
indice[939] = 613;
indice[940] = 616;
indice[941] = 615;
indice[942] = 617;
indice[943] = 619;
indice[944] = 618;
indice[945] = 617;
indice[946] = 620;
indice[947] = 619;
indice[948] = 621;
indice[949] = 623;
indice[950] = 622;
indice[951] = 621;
indice[952] = 624;
indice[953] = 623;
indice[954] = 625;
indice[955] = 627;
indice[956] = 626;
indice[957] = 625;
indice[958] = 628;
indice[959] = 627;
indice[960] = 629;
indice[961] = 631;
indice[962] = 630;
indice[963] = 629;
indice[964] = 632;
indice[965] = 631;
indice[966] = 633;
indice[967] = 635;
indice[968] = 634;
indice[969] = 633;
indice[970] = 636;
indice[971] = 635;
indice[972] = 637;
indice[973] = 639;
indice[974] = 638;
indice[975] = 637;
indice[976] = 640;
indice[977] = 639;
indice[978] = 641;
indice[979] = 643;
indice[980] = 642;
indice[981] = 641;
indice[982] = 644;
indice[983] = 643;
indice[984] = 645;
indice[985] = 647;
indice[986] = 646;
indice[987] = 645;
indice[988] = 648;
indice[989] = 647;
indice[990] = 649;
indice[991] = 651;
indice[992] = 650;
indice[993] = 649;
indice[994] = 652;
indice[995] = 651;
indice[996] = 653;
indice[997] = 655;
indice[998] = 654;
indice[999] = 653;
indice[1000] = 656;
indice[1001] = 655;
indice[1002] = 657;
indice[1003] = 659;
indice[1004] = 658;
indice[1005] = 657;
indice[1006] = 660;
indice[1007] = 659;
indice[1008] = 661;
indice[1009] = 663;
indice[1010] = 662;
indice[1011] = 661;
indice[1012] = 664;
indice[1013] = 663;
indice[1014] = 665;
indice[1015] = 667;
indice[1016] = 666;
indice[1017] = 665;
indice[1018] = 668;
indice[1019] = 667;
indice[1020] = 669;
indice[1021] = 671;
indice[1022] = 670;
indice[1023] = 669;
indice[1024] = 672;
indice[1025] = 671;
indice[1026] = 673;
indice[1027] = 675;
indice[1028] = 674;
indice[1029] = 673;
indice[1030] = 676;
indice[1031] = 675;
indice[1032] = 677;
indice[1033] = 679;
indice[1034] = 678;
indice[1035] = 677;
indice[1036] = 680;
indice[1037] = 679;
indice[1038] = 681;
indice[1039] = 683;
indice[1040] = 682;
indice[1041] = 681;
indice[1042] = 684;
indice[1043] = 683;
indice[1044] = 685;
indice[1045] = 687;
indice[1046] = 686;
indice[1047] = 685;
indice[1048] = 688;
indice[1049] = 687;
indice[1050] = 689;
indice[1051] = 691;
indice[1052] = 690;
indice[1053] = 689;
indice[1054] = 692;
indice[1055] = 691;
indice[1056] = 693;
indice[1057] = 695;
indice[1058] = 694;
indice[1059] = 693;
indice[1060] = 696;
indice[1061] = 695;
indice[1062] = 697;
indice[1063] = 699;
indice[1064] = 698;
indice[1065] = 697;
indice[1066] = 700;
indice[1067] = 699;
indice[1068] = 701;
indice[1069] = 703;
indice[1070] = 702;
indice[1071] = 701;
indice[1072] = 704;
indice[1073] = 703;
indice[1074] = 705;
indice[1075] = 707;
indice[1076] = 706;
indice[1077] = 705;
indice[1078] = 708;
indice[1079] = 707;
indice[1080] = 709;
indice[1081] = 711;
indice[1082] = 710;
indice[1083] = 709;
indice[1084] = 712;
indice[1085] = 711;
indice[1086] = 713;
indice[1087] = 715;
indice[1088] = 714;
indice[1089] = 713;
indice[1090] = 716;
indice[1091] = 715;
indice[1092] = 717;
indice[1093] = 719;
indice[1094] = 718;
indice[1095] = 717;
indice[1096] = 720;
indice[1097] = 719;
indice[1098] = 721;
indice[1099] = 723;
indice[1100] = 722;
indice[1101] = 721;
indice[1102] = 724;
indice[1103] = 723;
indice[1104] = 725;
indice[1105] = 727;
indice[1106] = 726;
indice[1107] = 725;
indice[1108] = 728;
indice[1109] = 727;
indice[1110] = 729;
indice[1111] = 731;
indice[1112] = 730;
indice[1113] = 729;
indice[1114] = 732;
indice[1115] = 731;
indice[1116] = 733;
indice[1117] = 735;
indice[1118] = 734;
indice[1119] = 733;
indice[1120] = 736;
indice[1121] = 735;
indice[1122] = 737;
indice[1123] = 739;
indice[1124] = 738;
indice[1125] = 737;
indice[1126] = 740;
indice[1127] = 739;
indice[1128] = 741;
indice[1129] = 743;
indice[1130] = 742;
indice[1131] = 741;
indice[1132] = 744;
indice[1133] = 743;
indice[1134] = 745;
indice[1135] = 747;
indice[1136] = 746;
indice[1137] = 745;
indice[1138] = 748;
indice[1139] = 747;
indice[1140] = 749;
indice[1141] = 751;
indice[1142] = 750;
indice[1143] = 749;
indice[1144] = 752;
indice[1145] = 751;
indice[1146] = 753;
indice[1147] = 755;
indice[1148] = 754;
indice[1149] = 753;
indice[1150] = 756;
indice[1151] = 755;
indice[1152] = 757;
indice[1153] = 759;
indice[1154] = 758;
indice[1155] = 757;
indice[1156] = 760;
indice[1157] = 759;
indice[1158] = 761;
indice[1159] = 763;
indice[1160] = 762;
indice[1161] = 761;
indice[1162] = 764;
indice[1163] = 763;
indice[1164] = 765;
indice[1165] = 767;
indice[1166] = 766;
indice[1167] = 765;
indice[1168] = 768;
indice[1169] = 767;
indice[1170] = 769;
indice[1171] = 771;
indice[1172] = 770;
indice[1173] = 769;
indice[1174] = 772;
indice[1175] = 771;
indice[1176] = 773;
indice[1177] = 775;
indice[1178] = 774;
indice[1179] = 773;
indice[1180] = 776;
indice[1181] = 775;
indice[1182] = 777;
indice[1183] = 779;
indice[1184] = 778;
indice[1185] = 777;
indice[1186] = 780;
indice[1187] = 779;
indice[1188] = 781;
indice[1189] = 783;
indice[1190] = 782;
indice[1191] = 781;
indice[1192] = 784;
indice[1193] = 783;
indice[1194] = 785;
indice[1195] = 787;
indice[1196] = 786;
indice[1197] = 785;
indice[1198] = 788;
indice[1199] = 787;
indice[1200] = 789;
indice[1201] = 791;
indice[1202] = 790;
indice[1203] = 789;
indice[1204] = 792;
indice[1205] = 791;
indice[1206] = 793;
indice[1207] = 795;
indice[1208] = 794;
indice[1209] = 793;
indice[1210] = 796;
indice[1211] = 795;
indice[1212] = 797;
indice[1213] = 799;
indice[1214] = 798;
indice[1215] = 797;
indice[1216] = 800;
indice[1217] = 799;
indice[1218] = 801;
indice[1219] = 803;
indice[1220] = 802;
indice[1221] = 801;
indice[1222] = 804;
indice[1223] = 803;
indice[1224] = 805;
indice[1225] = 807;
indice[1226] = 806;
indice[1227] = 805;
indice[1228] = 808;
indice[1229] = 807;
indice[1230] = 809;
indice[1231] = 811;
indice[1232] = 810;
indice[1233] = 809;
indice[1234] = 812;
indice[1235] = 811;
indice[1236] = 813;
indice[1237] = 815;
indice[1238] = 814;
indice[1239] = 813;
indice[1240] = 816;
indice[1241] = 815;
indice[1242] = 817;
indice[1243] = 819;
indice[1244] = 818;
indice[1245] = 817;
indice[1246] = 820;
indice[1247] = 819;
indice[1248] = 821;
indice[1249] = 823;
indice[1250] = 822;
indice[1251] = 821;
indice[1252] = 824;
indice[1253] = 823;
indice[1254] = 825;
indice[1255] = 827;
indice[1256] = 826;
indice[1257] = 825;
indice[1258] = 828;
indice[1259] = 827;
indice[1260] = 829;
indice[1261] = 831;
indice[1262] = 830;
indice[1263] = 829;
indice[1264] = 832;
indice[1265] = 831;
indice[1266] = 833;
indice[1267] = 835;
indice[1268] = 834;
indice[1269] = 833;
indice[1270] = 836;
indice[1271] = 835;
indice[1272] = 837;
indice[1273] = 839;
indice[1274] = 838;
indice[1275] = 837;
indice[1276] = 840;
indice[1277] = 839;
indice[1278] = 841;
indice[1279] = 843;
indice[1280] = 842;
indice[1281] = 841;
indice[1282] = 844;
indice[1283] = 843;
indice[1284] = 845;
indice[1285] = 847;
indice[1286] = 846;
indice[1287] = 845;
indice[1288] = 848;
indice[1289] = 847;
indice[1290] = 849;
indice[1291] = 851;
indice[1292] = 850;
indice[1293] = 849;
indice[1294] = 852;
indice[1295] = 851;
indice[1296] = 853;
indice[1297] = 855;
indice[1298] = 854;
indice[1299] = 853;
indice[1300] = 856;
indice[1301] = 855;
indice[1302] = 857;
indice[1303] = 859;
indice[1304] = 858;
indice[1305] = 857;
indice[1306] = 860;
indice[1307] = 859;
indice[1308] = 861;
indice[1309] = 863;
indice[1310] = 862;
indice[1311] = 861;
indice[1312] = 864;
indice[1313] = 863;
indice[1314] = 865;
indice[1315] = 867;
indice[1316] = 866;
indice[1317] = 865;
indice[1318] = 868;
indice[1319] = 867;
indice[1320] = 869;
indice[1321] = 871;
indice[1322] = 870;
indice[1323] = 869;
indice[1324] = 872;
indice[1325] = 871;
indice[1326] = 873;
indice[1327] = 875;
indice[1328] = 874;
indice[1329] = 873;
indice[1330] = 876;
indice[1331] = 875;
indice[1332] = 877;
indice[1333] = 879;
indice[1334] = 878;
indice[1335] = 877;
indice[1336] = 880;
indice[1337] = 879;
indice[1338] = 881;
indice[1339] = 883;
indice[1340] = 882;
indice[1341] = 881;
indice[1342] = 884;
indice[1343] = 883;
indice[1344] = 885;
indice[1345] = 887;
indice[1346] = 886;
indice[1347] = 885;
indice[1348] = 888;
indice[1349] = 887;
indice[1350] = 889;
indice[1351] = 891;
indice[1352] = 890;
indice[1353] = 889;
indice[1354] = 892;
indice[1355] = 891;
indice[1356] = 893;
indice[1357] = 895;
indice[1358] = 894;
indice[1359] = 893;
indice[1360] = 896;
indice[1361] = 895;
indice[1362] = 897;
indice[1363] = 899;
indice[1364] = 898;
indice[1365] = 897;
indice[1366] = 900;
indice[1367] = 899;
indice[1368] = 901;
indice[1369] = 903;
indice[1370] = 902;
indice[1371] = 901;
indice[1372] = 904;
indice[1373] = 903;
indice[1374] = 905;
indice[1375] = 907;
indice[1376] = 906;
indice[1377] = 905;
indice[1378] = 908;
indice[1379] = 907;
indice[1380] = 909;
indice[1381] = 911;
indice[1382] = 910;
indice[1383] = 909;
indice[1384] = 912;
indice[1385] = 911;
indice[1386] = 913;
indice[1387] = 915;
indice[1388] = 914;
indice[1389] = 913;
indice[1390] = 916;
indice[1391] = 915;
indice[1392] = 917;
indice[1393] = 919;
indice[1394] = 918;
indice[1395] = 917;
indice[1396] = 920;
indice[1397] = 919;
indice[1398] = 921;
indice[1399] = 923;
indice[1400] = 922;
indice[1401] = 921;
indice[1402] = 924;
indice[1403] = 923;
indice[1404] = 925;
indice[1405] = 927;
indice[1406] = 926;
indice[1407] = 925;
indice[1408] = 928;
indice[1409] = 927;
indice[1410] = 929;
indice[1411] = 931;
indice[1412] = 930;
indice[1413] = 929;
indice[1414] = 932;
indice[1415] = 931;
indice[1416] = 933;
indice[1417] = 935;
indice[1418] = 934;
indice[1419] = 933;
indice[1420] = 936;
indice[1421] = 935;
indice[1422] = 937;
indice[1423] = 939;
indice[1424] = 938;
indice[1425] = 937;
indice[1426] = 940;
indice[1427] = 939;
indice[1428] = 941;
indice[1429] = 943;
indice[1430] = 942;
indice[1431] = 941;
indice[1432] = 944;
indice[1433] = 943;
indice[1434] = 945;
indice[1435] = 947;
indice[1436] = 946;
indice[1437] = 945;
indice[1438] = 948;
indice[1439] = 947;
indice[1440] = 949;
indice[1441] = 951;
indice[1442] = 950;
indice[1443] = 949;
indice[1444] = 952;
indice[1445] = 951;
indice[1446] = 953;
indice[1447] = 955;
indice[1448] = 954;
indice[1449] = 953;
indice[1450] = 956;
indice[1451] = 955;
indice[1452] = 957;
indice[1453] = 959;
indice[1454] = 958;
indice[1455] = 957;
indice[1456] = 960;
indice[1457] = 959;
indice[1458] = 961;
indice[1459] = 963;
indice[1460] = 962;
indice[1461] = 961;
indice[1462] = 964;
indice[1463] = 963;
indice[1464] = 965;
indice[1465] = 967;
indice[1466] = 966;
indice[1467] = 965;
indice[1468] = 968;
indice[1469] = 967;
indice[1470] = 969;
indice[1471] = 971;
indice[1472] = 970;
indice[1473] = 969;
indice[1474] = 972;
indice[1475] = 971;
indice[1476] = 973;
indice[1477] = 975;
indice[1478] = 974;
indice[1479] = 973;
indice[1480] = 976;
indice[1481] = 975;
indice[1482] = 977;
indice[1483] = 979;
indice[1484] = 978;
indice[1485] = 977;
indice[1486] = 980;
indice[1487] = 979;
indice[1488] = 981;
indice[1489] = 983;
indice[1490] = 982;
indice[1491] = 981;
indice[1492] = 984;
indice[1493] = 983;
indice[1494] = 985;
indice[1495] = 987;
indice[1496] = 986;
indice[1497] = 985;
indice[1498] = 988;
indice[1499] = 987;
indice[1500] = 989;
indice[1501] = 991;
indice[1502] = 990;
indice[1503] = 989;
indice[1504] = 992;
indice[1505] = 991;
indice[1506] = 993;
indice[1507] = 995;
indice[1508] = 994;
indice[1509] = 993;
indice[1510] = 996;
indice[1511] = 995;
indice[1512] = 997;
indice[1513] = 999;
indice[1514] = 998;
indice[1515] = 997;
indice[1516] = 1000;
indice[1517] = 999;
indice[1518] = 1001;
indice[1519] = 1003;
indice[1520] = 1002;
indice[1521] = 1001;
indice[1522] = 1004;
indice[1523] = 1003;
indice[1524] = 1005;
indice[1525] = 1007;
indice[1526] = 1006;
indice[1527] = 1005;
indice[1528] = 1008;
indice[1529] = 1007;
indice[1530] = 1009;
indice[1531] = 1011;
indice[1532] = 1010;
indice[1533] = 1009;
indice[1534] = 1012;
indice[1535] = 1011;
indice[1536] = 1013;
indice[1537] = 1015;
indice[1538] = 1014;
indice[1539] = 1013;
indice[1540] = 1016;
indice[1541] = 1015;
indice[1542] = 1017;
indice[1543] = 1019;
indice[1544] = 1018;
indice[1545] = 1017;
indice[1546] = 1020;
indice[1547] = 1019;
indice[1548] = 1021;
indice[1549] = 1023;
indice[1550] = 1022;
indice[1551] = 1021;
indice[1552] = 1024;
indice[1553] = 1023;
indice[1554] = 1025;
indice[1555] = 1027;
indice[1556] = 1026;
indice[1557] = 1025;
indice[1558] = 1028;
indice[1559] = 1027;
indice[1560] = 1029;
indice[1561] = 1031;
indice[1562] = 1030;
indice[1563] = 1029;
indice[1564] = 1032;
indice[1565] = 1031;
indice[1566] = 1033;
indice[1567] = 1035;
indice[1568] = 1034;
indice[1569] = 1033;
indice[1570] = 1036;
indice[1571] = 1035;
indice[1572] = 1037;
indice[1573] = 1039;
indice[1574] = 1038;
indice[1575] = 1037;
indice[1576] = 1040;
indice[1577] = 1039;
indice[1578] = 1041;
indice[1579] = 1043;
indice[1580] = 1042;
indice[1581] = 1041;
indice[1582] = 1044;
indice[1583] = 1043;
indice[1584] = 1045;
indice[1585] = 1047;
indice[1586] = 1046;
indice[1587] = 1045;
indice[1588] = 1048;
indice[1589] = 1047;
indice[1590] = 1049;
indice[1591] = 1051;
indice[1592] = 1050;
indice[1593] = 1049;
indice[1594] = 1052;
indice[1595] = 1051;
indice[1596] = 1053;
indice[1597] = 1055;
indice[1598] = 1054;
indice[1599] = 1053;
indice[1600] = 1056;
indice[1601] = 1055;
indice[1602] = 1057;
indice[1603] = 1059;
indice[1604] = 1058;
indice[1605] = 1057;
indice[1606] = 1060;
indice[1607] = 1059;
indice[1608] = 1061;
indice[1609] = 1063;
indice[1610] = 1062;
indice[1611] = 1061;
indice[1612] = 1064;
indice[1613] = 1063;
indice[1614] = 1065;
indice[1615] = 1067;
indice[1616] = 1066;
indice[1617] = 1065;
indice[1618] = 1068;
indice[1619] = 1067;
indice[1620] = 1069;
indice[1621] = 1071;
indice[1622] = 1070;
indice[1623] = 1069;
indice[1624] = 1072;
indice[1625] = 1071;
indice[1626] = 1073;
indice[1627] = 1075;
indice[1628] = 1074;
indice[1629] = 1073;
indice[1630] = 1076;
indice[1631] = 1075;
indice[1632] = 1077;
indice[1633] = 1079;
indice[1634] = 1078;
indice[1635] = 1077;
indice[1636] = 1080;
indice[1637] = 1079;
indice[1638] = 1081;
indice[1639] = 1083;
indice[1640] = 1082;
indice[1641] = 1081;
indice[1642] = 1084;
indice[1643] = 1083;
indice[1644] = 1085;
indice[1645] = 1087;
indice[1646] = 1086;
indice[1647] = 1085;
indice[1648] = 1088;
indice[1649] = 1087;
indice[1650] = 1089;
indice[1651] = 1091;
indice[1652] = 1090;
indice[1653] = 1089;
indice[1654] = 1092;
indice[1655] = 1091;
indice[1656] = 1093;
indice[1657] = 1095;
indice[1658] = 1094;
indice[1659] = 1093;
indice[1660] = 1096;
indice[1661] = 1095;
indice[1662] = 1097;
indice[1663] = 1099;
indice[1664] = 1098;
indice[1665] = 1097;
indice[1666] = 1100;
indice[1667] = 1099;
indice[1668] = 1101;
indice[1669] = 1103;
indice[1670] = 1102;
indice[1671] = 1101;
indice[1672] = 1104;
indice[1673] = 1103;
indice[1674] = 1105;
indice[1675] = 1107;
indice[1676] = 1106;
indice[1677] = 1105;
indice[1678] = 1108;
indice[1679] = 1107;
indice[1680] = 1109;
indice[1681] = 1111;
indice[1682] = 1110;
indice[1683] = 1109;
indice[1684] = 1112;
indice[1685] = 1111;
indice[1686] = 1113;
indice[1687] = 1115;
indice[1688] = 1114;
indice[1689] = 1113;
indice[1690] = 1116;
indice[1691] = 1115;
indice[1692] = 1117;
indice[1693] = 1119;
indice[1694] = 1118;
indice[1695] = 1117;
indice[1696] = 1120;
indice[1697] = 1119;
indice[1698] = 1121;
indice[1699] = 1123;
indice[1700] = 1122;
indice[1701] = 1121;
indice[1702] = 1124;
indice[1703] = 1123;
indice[1704] = 1125;
indice[1705] = 1127;
indice[1706] = 1126;
indice[1707] = 1125;
indice[1708] = 1128;
indice[1709] = 1127;
indice[1710] = 1129;
indice[1711] = 1131;
indice[1712] = 1130;
indice[1713] = 1129;
indice[1714] = 1132;
indice[1715] = 1131;
indice[1716] = 1133;
indice[1717] = 1135;
indice[1718] = 1134;
indice[1719] = 1133;
indice[1720] = 1136;
indice[1721] = 1135;
indice[1722] = 1137;
indice[1723] = 1139;
indice[1724] = 1138;
indice[1725] = 1137;
indice[1726] = 1140;
indice[1727] = 1139;
indice[1728] = 1141;
indice[1729] = 1143;
indice[1730] = 1142;
indice[1731] = 1141;
indice[1732] = 1144;
indice[1733] = 1143;
indice[1734] = 1145;
indice[1735] = 1147;
indice[1736] = 1146;
indice[1737] = 1145;
indice[1738] = 1148;
indice[1739] = 1147;
indice[1740] = 1149;
indice[1741] = 1151;
indice[1742] = 1150;
indice[1743] = 1149;
indice[1744] = 1152;
indice[1745] = 1151;
indice[1746] = 1153;
indice[1747] = 1155;
indice[1748] = 1154;
indice[1749] = 1153;
indice[1750] = 1156;
indice[1751] = 1155;
indice[1752] = 1157;
indice[1753] = 1159;
indice[1754] = 1158;
indice[1755] = 1157;
indice[1756] = 1160;
indice[1757] = 1159;
indice[1758] = 1161;
indice[1759] = 1163;
indice[1760] = 1162;
indice[1761] = 1161;
indice[1762] = 1164;
indice[1763] = 1163;
indice[1764] = 1165;
indice[1765] = 1167;
indice[1766] = 1166;
indice[1767] = 1165;
indice[1768] = 1168;
indice[1769] = 1167;
indice[1770] = 1169;
indice[1771] = 1171;
indice[1772] = 1170;
indice[1773] = 1169;
indice[1774] = 1172;
indice[1775] = 1171;
indice[1776] = 1173;
indice[1777] = 1175;
indice[1778] = 1174;
indice[1779] = 1173;
indice[1780] = 1176;
indice[1781] = 1175;
indice[1782] = 1177;
indice[1783] = 1179;
indice[1784] = 1178;
indice[1785] = 1177;
indice[1786] = 1180;
indice[1787] = 1179;
indice[1788] = 1181;
indice[1789] = 1183;
indice[1790] = 1182;
indice[1791] = 1181;
indice[1792] = 1184;
indice[1793] = 1183;
indice[1794] = 1185;
indice[1795] = 1187;
indice[1796] = 1186;
indice[1797] = 1185;
indice[1798] = 1188;
indice[1799] = 1187;
indice[1800] = 1189;
indice[1801] = 1191;
indice[1802] = 1190;
indice[1803] = 1189;
indice[1804] = 1192;
indice[1805] = 1191;
indice[1806] = 1193;
indice[1807] = 1195;
indice[1808] = 1194;
indice[1809] = 1193;
indice[1810] = 1196;
indice[1811] = 1195;
indice[1812] = 1197;
indice[1813] = 1199;
indice[1814] = 1198;
indice[1815] = 1197;
indice[1816] = 1200;
indice[1817] = 1199;
indice[1818] = 1201;
indice[1819] = 1203;
indice[1820] = 1202;
indice[1821] = 1201;
indice[1822] = 1204;
indice[1823] = 1203;
indice[1824] = 1205;
indice[1825] = 1207;
indice[1826] = 1206;
indice[1827] = 1205;
indice[1828] = 1208;
indice[1829] = 1207;
indice[1830] = 1209;
indice[1831] = 1211;
indice[1832] = 1210;
indice[1833] = 1209;
indice[1834] = 1212;
indice[1835] = 1211;
indice[1836] = 1213;
indice[1837] = 1215;
indice[1838] = 1214;
indice[1839] = 1213;
indice[1840] = 1216;
indice[1841] = 1215;
indice[1842] = 1217;
indice[1843] = 1219;
indice[1844] = 1218;
indice[1845] = 1217;
indice[1846] = 1220;
indice[1847] = 1219;
indice[1848] = 1221;
indice[1849] = 1223;
indice[1850] = 1222;
indice[1851] = 1221;
indice[1852] = 1224;
indice[1853] = 1223;
indice[1854] = 1225;
indice[1855] = 1227;
indice[1856] = 1226;
indice[1857] = 1225;
indice[1858] = 1228;
indice[1859] = 1227;
indice[1860] = 1229;
indice[1861] = 1231;
indice[1862] = 1230;
indice[1863] = 1229;
indice[1864] = 1232;
indice[1865] = 1231;
indice[1866] = 1233;
indice[1867] = 1235;
indice[1868] = 1234;
indice[1869] = 1233;
indice[1870] = 1236;
indice[1871] = 1235;
indice[1872] = 1237;
indice[1873] = 1239;
indice[1874] = 1238;
indice[1875] = 1237;
indice[1876] = 1240;
indice[1877] = 1239;
indice[1878] = 1241;
indice[1879] = 1243;
indice[1880] = 1242;
indice[1881] = 1241;
indice[1882] = 1244;
indice[1883] = 1243;
indice[1884] = 1245;
indice[1885] = 1247;
indice[1886] = 1246;
indice[1887] = 1245;
indice[1888] = 1248;
indice[1889] = 1247;
indice[1890] = 1249;
indice[1891] = 1251;
indice[1892] = 1250;
indice[1893] = 1249;
indice[1894] = 1252;
indice[1895] = 1251;
indice[1896] = 1253;
indice[1897] = 1255;
indice[1898] = 1254;
indice[1899] = 1253;
indice[1900] = 1256;
indice[1901] = 1255;
indice[1902] = 1257;
indice[1903] = 1259;
indice[1904] = 1258;
indice[1905] = 1257;
indice[1906] = 1260;
indice[1907] = 1259;
indice[1908] = 1261;
indice[1909] = 1263;
indice[1910] = 1262;
indice[1911] = 1261;
indice[1912] = 1264;
indice[1913] = 1263;
indice[1914] = 1265;
indice[1915] = 1267;
indice[1916] = 1266;
indice[1917] = 1265;
indice[1918] = 1268;
indice[1919] = 1267;
indice[1920] = 1269;
indice[1921] = 1271;
indice[1922] = 1270;
indice[1923] = 1269;
indice[1924] = 1272;
indice[1925] = 1271;
indice[1926] = 1273;
indice[1927] = 1275;
indice[1928] = 1274;
indice[1929] = 1273;
indice[1930] = 1276;
indice[1931] = 1275;
indice[1932] = 1277;
indice[1933] = 1279;
indice[1934] = 1278;
indice[1935] = 1277;
indice[1936] = 1280;
indice[1937] = 1279;
indice[1938] = 1281;
indice[1939] = 1283;
indice[1940] = 1282;
indice[1941] = 1281;
indice[1942] = 1284;
indice[1943] = 1283;
indice[1944] = 1285;
indice[1945] = 1287;
indice[1946] = 1286;
indice[1947] = 1285;
indice[1948] = 1288;
indice[1949] = 1287;
indice[1950] = 1289;
indice[1951] = 1291;
indice[1952] = 1290;
indice[1953] = 1289;
indice[1954] = 1292;
indice[1955] = 1291;
indice[1956] = 1293;
indice[1957] = 1295;
indice[1958] = 1294;
indice[1959] = 1293;
indice[1960] = 1296;
indice[1961] = 1295;
indice[1962] = 1297;
indice[1963] = 1299;
indice[1964] = 1298;
indice[1965] = 1297;
indice[1966] = 1300;
indice[1967] = 1299;
indice[1968] = 1301;
indice[1969] = 1303;
indice[1970] = 1302;
indice[1971] = 1301;
indice[1972] = 1304;
indice[1973] = 1303;
indice[1974] = 1305;
indice[1975] = 1307;
indice[1976] = 1306;
indice[1977] = 1305;
indice[1978] = 1308;
indice[1979] = 1307;
indice[1980] = 1309;
indice[1981] = 1311;
indice[1982] = 1310;
indice[1983] = 1309;
indice[1984] = 1312;
indice[1985] = 1311;
indice[1986] = 1313;
indice[1987] = 1315;
indice[1988] = 1314;
indice[1989] = 1313;
indice[1990] = 1316;
indice[1991] = 1315;
indice[1992] = 1317;
indice[1993] = 1319;
indice[1994] = 1318;
indice[1995] = 1317;
indice[1996] = 1320;
indice[1997] = 1319;
indice[1998] = 1321;
indice[1999] = 1323;
indice[2000] = 1322;
indice[2001] = 1321;
indice[2002] = 1324;
indice[2003] = 1323;
indice[2004] = 1325;
indice[2005] = 1327;
indice[2006] = 1326;
indice[2007] = 1325;
indice[2008] = 1328;
indice[2009] = 1327;
indice[2010] = 1329;
indice[2011] = 1331;
indice[2012] = 1330;
indice[2013] = 1329;
indice[2014] = 1332;
indice[2015] = 1331;
indice[2016] = 1333;
indice[2017] = 1335;
indice[2018] = 1334;
indice[2019] = 1333;
indice[2020] = 1336;
indice[2021] = 1335;
indice[2022] = 1337;
indice[2023] = 1339;
indice[2024] = 1338;
indice[2025] = 1337;
indice[2026] = 1340;
indice[2027] = 1339;
indice[2028] = 1341;
indice[2029] = 1343;
indice[2030] = 1342;
indice[2031] = 1341;
indice[2032] = 1344;
indice[2033] = 1343;
indice[2034] = 1345;
indice[2035] = 1347;
indice[2036] = 1346;
indice[2037] = 1345;
indice[2038] = 1348;
indice[2039] = 1347;
indice[2040] = 1349;
indice[2041] = 1351;
indice[2042] = 1350;
indice[2043] = 1349;
indice[2044] = 1352;
indice[2045] = 1351;
indice[2046] = 1353;
indice[2047] = 1355;
indice[2048] = 1354;
indice[2049] = 1353;
indice[2050] = 1356;
indice[2051] = 1355;
indice[2052] = 1357;
indice[2053] = 1359;
indice[2054] = 1358;
indice[2055] = 1357;
indice[2056] = 1360;
indice[2057] = 1359;
indice[2058] = 1361;
indice[2059] = 1363;
indice[2060] = 1362;
indice[2061] = 1361;
indice[2062] = 1364;
indice[2063] = 1363;
indice[2064] = 1365;
indice[2065] = 1367;
indice[2066] = 1366;
indice[2067] = 1365;
indice[2068] = 1368;
indice[2069] = 1367;
indice[2070] = 1369;
indice[2071] = 1371;
indice[2072] = 1370;
indice[2073] = 1369;
indice[2074] = 1372;
indice[2075] = 1371;
indice[2076] = 1373;
indice[2077] = 1375;
indice[2078] = 1374;
indice[2079] = 1373;
indice[2080] = 1376;
indice[2081] = 1375;
indice[2082] = 1377;
indice[2083] = 1379;
indice[2084] = 1378;
indice[2085] = 1377;
indice[2086] = 1380;
indice[2087] = 1379;
indice[2088] = 1381;
indice[2089] = 1383;
indice[2090] = 1382;
indice[2091] = 1381;
indice[2092] = 1384;
indice[2093] = 1383;
indice[2094] = 1385;
indice[2095] = 1387;
indice[2096] = 1386;
indice[2097] = 1385;
indice[2098] = 1388;
indice[2099] = 1387;
indice[2100] = 1389;
indice[2101] = 1391;
indice[2102] = 1390;
indice[2103] = 1389;
indice[2104] = 1392;
indice[2105] = 1391;
indice[2106] = 1393;
indice[2107] = 1395;
indice[2108] = 1394;
indice[2109] = 1393;
indice[2110] = 1396;
indice[2111] = 1395;
indice[2112] = 1397;
indice[2113] = 1399;
indice[2114] = 1398;
indice[2115] = 1397;
indice[2116] = 1400;
indice[2117] = 1399;
indice[2118] = 1401;
indice[2119] = 1403;
indice[2120] = 1402;
indice[2121] = 1401;
indice[2122] = 1404;
indice[2123] = 1403;
indice[2124] = 1405;
indice[2125] = 1407;
indice[2126] = 1406;
indice[2127] = 1405;
indice[2128] = 1408;
indice[2129] = 1407;
indice[2130] = 1409;
indice[2131] = 1411;
indice[2132] = 1410;
indice[2133] = 1409;
indice[2134] = 1412;
indice[2135] = 1411;
indice[2136] = 1413;
indice[2137] = 1415;
indice[2138] = 1414;
indice[2139] = 1413;
indice[2140] = 1416;
indice[2141] = 1415;
indice[2142] = 1417;
indice[2143] = 1419;
indice[2144] = 1418;
indice[2145] = 1417;
indice[2146] = 1420;
indice[2147] = 1419;
indice[2148] = 1421;
indice[2149] = 1423;
indice[2150] = 1422;
indice[2151] = 1421;
indice[2152] = 1424;
indice[2153] = 1423;
indice[2154] = 1425;
indice[2155] = 1427;
indice[2156] = 1426;
indice[2157] = 1425;
indice[2158] = 1428;
indice[2159] = 1427;
indice[2160] = 1429;
indice[2161] = 1431;
indice[2162] = 1430;
indice[2163] = 1429;
indice[2164] = 1432;
indice[2165] = 1431;
indice[2166] = 1433;
indice[2167] = 1435;
indice[2168] = 1434;
indice[2169] = 1433;
indice[2170] = 1436;
indice[2171] = 1435;
indice[2172] = 1437;
indice[2173] = 1439;
indice[2174] = 1438;
indice[2175] = 1437;
indice[2176] = 1440;
indice[2177] = 1439;
indice[2178] = 1441;
indice[2179] = 1443;
indice[2180] = 1442;
indice[2181] = 1441;
indice[2182] = 1444;
indice[2183] = 1443;
indice[2184] = 1445;
indice[2185] = 1447;
indice[2186] = 1446;
indice[2187] = 1445;
indice[2188] = 1448;
indice[2189] = 1447;
indice[2190] = 1449;
indice[2191] = 1451;
indice[2192] = 1450;
indice[2193] = 1449;
indice[2194] = 1452;
indice[2195] = 1451;
indice[2196] = 1453;
indice[2197] = 1455;
indice[2198] = 1454;
indice[2199] = 1453;
indice[2200] = 1456;
indice[2201] = 1455;
indice[2202] = 1457;
indice[2203] = 1459;
indice[2204] = 1458;
indice[2205] = 1457;
indice[2206] = 1460;
indice[2207] = 1459;
indice[2208] = 1461;
indice[2209] = 1463;
indice[2210] = 1462;
indice[2211] = 1461;
indice[2212] = 1464;
indice[2213] = 1463;
indice[2214] = 1465;
indice[2215] = 1467;
indice[2216] = 1466;
indice[2217] = 1465;
indice[2218] = 1468;
indice[2219] = 1467;
indice[2220] = 1469;
indice[2221] = 1471;
indice[2222] = 1470;
indice[2223] = 1469;
indice[2224] = 1472;
indice[2225] = 1471;
indice[2226] = 1473;
indice[2227] = 1475;
indice[2228] = 1474;
indice[2229] = 1473;
indice[2230] = 1476;
indice[2231] = 1475;
indice[2232] = 1477;
indice[2233] = 1479;
indice[2234] = 1478;
indice[2235] = 1477;
indice[2236] = 1480;
indice[2237] = 1479;
indice[2238] = 1481;
indice[2239] = 1483;
indice[2240] = 1482;
indice[2241] = 1481;
indice[2242] = 1484;
indice[2243] = 1483;
indice[2244] = 1485;
indice[2245] = 1487;
indice[2246] = 1486;
indice[2247] = 1485;
indice[2248] = 1488;
indice[2249] = 1487;
indice[2250] = 1489;
indice[2251] = 1491;
indice[2252] = 1490;
indice[2253] = 1489;
indice[2254] = 1492;
indice[2255] = 1491;
indice[2256] = 1493;
indice[2257] = 1495;
indice[2258] = 1494;
indice[2259] = 1493;
indice[2260] = 1496;
indice[2261] = 1495;
indice[2262] = 1497;
indice[2263] = 1499;
indice[2264] = 1498;
indice[2265] = 1497;
indice[2266] = 1500;
indice[2267] = 1499;
indice[2268] = 1501;
indice[2269] = 1503;
indice[2270] = 1502;
indice[2271] = 1501;
indice[2272] = 1504;
indice[2273] = 1503;
indice[2274] = 1505;
indice[2275] = 1507;
indice[2276] = 1506;
indice[2277] = 1505;
indice[2278] = 1508;
indice[2279] = 1507;
indice[2280] = 1509;
indice[2281] = 1511;
indice[2282] = 1510;
indice[2283] = 1509;
indice[2284] = 1512;
indice[2285] = 1511;
indice[2286] = 1513;
indice[2287] = 1515;
indice[2288] = 1514;
indice[2289] = 1513;
indice[2290] = 1516;
indice[2291] = 1515;
indice[2292] = 1517;
indice[2293] = 1519;
indice[2294] = 1518;
indice[2295] = 1517;
indice[2296] = 1520;
indice[2297] = 1519;
indice[2298] = 1521;
indice[2299] = 1523;
indice[2300] = 1522;
indice[2301] = 1521;
indice[2302] = 1524;
indice[2303] = 1523;
indice[2304] = 1525;
indice[2305] = 1527;
indice[2306] = 1526;
indice[2307] = 1525;
indice[2308] = 1528;
indice[2309] = 1527;
indice[2310] = 1529;
indice[2311] = 1531;
indice[2312] = 1530;
indice[2313] = 1529;
indice[2314] = 1532;
indice[2315] = 1531;
indice[2316] = 1533;
indice[2317] = 1535;
indice[2318] = 1534;
indice[2319] = 1533;
indice[2320] = 1536;
indice[2321] = 1535;
indice[2322] = 1537;
indice[2323] = 1539;
indice[2324] = 1538;
indice[2325] = 1537;
indice[2326] = 1540;
indice[2327] = 1539;
indice[2328] = 1541;
indice[2329] = 1543;
indice[2330] = 1542;
indice[2331] = 1541;
indice[2332] = 1544;
indice[2333] = 1543;
indice[2334] = 1545;
indice[2335] = 1547;
indice[2336] = 1546;
indice[2337] = 1545;
indice[2338] = 1548;
indice[2339] = 1547;
indice[2340] = 1549;
indice[2341] = 1551;
indice[2342] = 1550;
indice[2343] = 1549;
indice[2344] = 1552;
indice[2345] = 1551;
indice[2346] = 1553;
indice[2347] = 1555;
indice[2348] = 1554;
indice[2349] = 1553;
indice[2350] = 1556;
indice[2351] = 1555;
indice[2352] = 1557;
indice[2353] = 1559;
indice[2354] = 1558;
indice[2355] = 1557;
indice[2356] = 1560;
indice[2357] = 1559;
indice[2358] = 1561;
indice[2359] = 1563;
indice[2360] = 1562;
indice[2361] = 1561;
indice[2362] = 1564;
indice[2363] = 1563;
indice[2364] = 1565;
indice[2365] = 1567;
indice[2366] = 1566;
indice[2367] = 1565;
indice[2368] = 1568;
indice[2369] = 1567;
indice[2370] = 1569;
indice[2371] = 1571;
indice[2372] = 1570;
indice[2373] = 1569;
indice[2374] = 1572;
indice[2375] = 1571;
indice[2376] = 1573;
indice[2377] = 1575;
indice[2378] = 1574;
indice[2379] = 1573;
indice[2380] = 1576;
indice[2381] = 1575;
indice[2382] = 1577;
indice[2383] = 1579;
indice[2384] = 1578;
indice[2385] = 1577;
indice[2386] = 1580;
indice[2387] = 1579;
indice[2388] = 1581;
indice[2389] = 1583;
indice[2390] = 1582;
indice[2391] = 1581;
indice[2392] = 1584;
indice[2393] = 1583;
indice[2394] = 1585;
indice[2395] = 1587;
indice[2396] = 1586;
indice[2397] = 1585;
indice[2398] = 1588;
indice[2399] = 1587;
indice[2400] = 1589;
indice[2401] = 1591;
indice[2402] = 1590;
indice[2403] = 1589;
indice[2404] = 1592;
indice[2405] = 1591;
indice[2406] = 1593;
indice[2407] = 1595;
indice[2408] = 1594;
indice[2409] = 1593;
indice[2410] = 1596;
indice[2411] = 1595;
indice[2412] = 1597;
indice[2413] = 1599;
indice[2414] = 1598;
indice[2415] = 1597;
indice[2416] = 1600;
indice[2417] = 1599;
indice[2418] = 1601;
indice[2419] = 1603;
indice[2420] = 1602;
indice[2421] = 1601;
indice[2422] = 1604;
indice[2423] = 1603;
indice[2424] = 1605;
indice[2425] = 1607;
indice[2426] = 1606;
indice[2427] = 1605;
indice[2428] = 1608;
indice[2429] = 1607;
indice[2430] = 1609;
indice[2431] = 1611;
indice[2432] = 1610;
indice[2433] = 1609;
indice[2434] = 1612;
indice[2435] = 1611;
indice[2436] = 1613;
indice[2437] = 1615;
indice[2438] = 1614;
indice[2439] = 1613;
indice[2440] = 1616;
indice[2441] = 1615;
indice[2442] = 1617;
indice[2443] = 1619;
indice[2444] = 1618;
indice[2445] = 1617;
indice[2446] = 1620;
indice[2447] = 1619;
indice[2448] = 1621;
indice[2449] = 1623;
indice[2450] = 1622;
indice[2451] = 1621;
indice[2452] = 1624;
indice[2453] = 1623;
indice[2454] = 1625;
indice[2455] = 1627;
indice[2456] = 1626;
indice[2457] = 1625;
indice[2458] = 1628;
indice[2459] = 1627;
indice[2460] = 1629;
indice[2461] = 1631;
indice[2462] = 1630;
indice[2463] = 1629;
indice[2464] = 1632;
indice[2465] = 1631;
indice[2466] = 1633;
indice[2467] = 1635;
indice[2468] = 1634;
indice[2469] = 1633;
indice[2470] = 1636;
indice[2471] = 1635;
indice[2472] = 1637;
indice[2473] = 1639;
indice[2474] = 1638;
indice[2475] = 1637;
indice[2476] = 1640;
indice[2477] = 1639;
indice[2478] = 1641;
indice[2479] = 1643;
indice[2480] = 1642;
indice[2481] = 1641;
indice[2482] = 1644;
indice[2483] = 1643;
indice[2484] = 1645;
indice[2485] = 1647;
indice[2486] = 1646;
indice[2487] = 1645;
indice[2488] = 1648;
indice[2489] = 1647;
indice[2490] = 1649;
indice[2491] = 1651;
indice[2492] = 1650;
indice[2493] = 1649;
indice[2494] = 1652;
indice[2495] = 1651;
indice[2496] = 1653;
indice[2497] = 1655;
indice[2498] = 1654;
indice[2499] = 1653;
indice[2500] = 1656;
indice[2501] = 1655;
indice[2502] = 1657;
indice[2503] = 1659;
indice[2504] = 1658;
indice[2505] = 1657;
indice[2506] = 1660;
indice[2507] = 1659;
indice[2508] = 1661;
indice[2509] = 1663;
indice[2510] = 1662;
indice[2511] = 1661;
indice[2512] = 1664;
indice[2513] = 1663;
indice[2514] = 1665;
indice[2515] = 1667;
indice[2516] = 1666;
indice[2517] = 1665;
indice[2518] = 1668;
indice[2519] = 1667;
indice[2520] = 1669;
indice[2521] = 1671;
indice[2522] = 1670;
indice[2523] = 1669;
indice[2524] = 1672;
indice[2525] = 1671;
indice[2526] = 1673;
indice[2527] = 1675;
indice[2528] = 1674;
indice[2529] = 1673;
indice[2530] = 1676;
indice[2531] = 1675;
indice[2532] = 1677;
indice[2533] = 1679;
indice[2534] = 1678;
indice[2535] = 1677;
indice[2536] = 1680;
indice[2537] = 1679;
indice[2538] = 1681;
indice[2539] = 1683;
indice[2540] = 1682;
indice[2541] = 1681;
indice[2542] = 1684;
indice[2543] = 1683;
indice[2544] = 1685;
indice[2545] = 1687;
indice[2546] = 1686;
indice[2547] = 1685;
indice[2548] = 1688;
indice[2549] = 1687;
indice[2550] = 1689;
indice[2551] = 1691;
indice[2552] = 1690;
indice[2553] = 1689;
indice[2554] = 1692;
indice[2555] = 1691;
indice[2556] = 1693;
indice[2557] = 1695;
indice[2558] = 1694;
indice[2559] = 1693;
indice[2560] = 1696;
indice[2561] = 1695;
indice[2562] = 1697;
indice[2563] = 1699;
indice[2564] = 1698;
indice[2565] = 1697;
indice[2566] = 1700;
indice[2567] = 1699;
indice[2568] = 1701;
indice[2569] = 1703;
indice[2570] = 1702;
indice[2571] = 1701;
indice[2572] = 1704;
indice[2573] = 1703;
indice[2574] = 1705;
indice[2575] = 1707;
indice[2576] = 1706;
indice[2577] = 1705;
indice[2578] = 1708;
indice[2579] = 1707;
indice[2580] = 1709;
indice[2581] = 1711;
indice[2582] = 1710;
indice[2583] = 1709;
indice[2584] = 1712;
indice[2585] = 1711;
indice[2586] = 1713;
indice[2587] = 1715;
indice[2588] = 1714;
indice[2589] = 1713;
indice[2590] = 1716;
indice[2591] = 1715;
indice[2592] = 1717;
indice[2593] = 1719;
indice[2594] = 1718;
indice[2595] = 1717;
indice[2596] = 1720;
indice[2597] = 1719;
indice[2598] = 1721;
indice[2599] = 1723;
indice[2600] = 1722;
indice[2601] = 1721;
indice[2602] = 1724;
indice[2603] = 1723;
indice[2604] = 1725;
indice[2605] = 1727;
indice[2606] = 1726;
indice[2607] = 1725;
indice[2608] = 1728;
indice[2609] = 1727;
indice[2610] = 1729;
indice[2611] = 1731;
indice[2612] = 1730;
indice[2613] = 1729;
indice[2614] = 1732;
indice[2615] = 1731;
indice[2616] = 1733;
indice[2617] = 1735;
indice[2618] = 1734;
indice[2619] = 1733;
indice[2620] = 1736;
indice[2621] = 1735;
indice[2622] = 1737;
indice[2623] = 1739;
indice[2624] = 1738;
indice[2625] = 1737;
indice[2626] = 1740;
indice[2627] = 1739;
indice[2628] = 1741;
indice[2629] = 1743;
indice[2630] = 1742;
indice[2631] = 1741;
indice[2632] = 1744;
indice[2633] = 1743;
indice[2634] = 1745;
indice[2635] = 1747;
indice[2636] = 1746;
indice[2637] = 1745;
indice[2638] = 1748;
indice[2639] = 1747;
indice[2640] = 1749;
indice[2641] = 1751;
indice[2642] = 1750;
indice[2643] = 1749;
indice[2644] = 1752;
indice[2645] = 1751;
indice[2646] = 1753;
indice[2647] = 1755;
indice[2648] = 1754;
indice[2649] = 1753;
indice[2650] = 1756;
indice[2651] = 1755;
indice[2652] = 1757;
indice[2653] = 1759;
indice[2654] = 1758;
indice[2655] = 1757;
indice[2656] = 1760;
indice[2657] = 1759;
indice[2658] = 1761;
indice[2659] = 1763;
indice[2660] = 1762;
indice[2661] = 1761;
indice[2662] = 1764;
indice[2663] = 1763;
indice[2664] = 1765;
indice[2665] = 1767;
indice[2666] = 1766;
indice[2667] = 1765;
indice[2668] = 1768;
indice[2669] = 1767;
indice[2670] = 1769;
indice[2671] = 1771;
indice[2672] = 1770;
indice[2673] = 1769;
indice[2674] = 1772;
indice[2675] = 1771;
indice[2676] = 1773;
indice[2677] = 1775;
indice[2678] = 1774;
indice[2679] = 1773;
indice[2680] = 1776;
indice[2681] = 1775;
indice[2682] = 1777;
indice[2683] = 1779;
indice[2684] = 1778;
indice[2685] = 1777;
indice[2686] = 1780;
indice[2687] = 1779;
indice[2688] = 1781;
indice[2689] = 1783;
indice[2690] = 1782;
indice[2691] = 1781;
indice[2692] = 1784;
indice[2693] = 1783;
indice[2694] = 1785;
indice[2695] = 1787;
indice[2696] = 1786;
indice[2697] = 1785;
indice[2698] = 1788;
indice[2699] = 1787;
indice[2700] = 1789;
indice[2701] = 1791;
indice[2702] = 1790;
indice[2703] = 1789;
indice[2704] = 1792;
indice[2705] = 1791;
indice[2706] = 1793;
indice[2707] = 1795;
indice[2708] = 1794;
indice[2709] = 1793;
indice[2710] = 1796;
indice[2711] = 1795;
indice[2712] = 1797;
indice[2713] = 1799;
indice[2714] = 1798;
indice[2715] = 1797;
indice[2716] = 1800;
indice[2717] = 1799;
indice[2718] = 1801;
indice[2719] = 1803;
indice[2720] = 1802;
indice[2721] = 1801;
indice[2722] = 1804;
indice[2723] = 1803;
indice[2724] = 1805;
indice[2725] = 1807;
indice[2726] = 1806;
indice[2727] = 1805;
indice[2728] = 1808;
indice[2729] = 1807;
indice[2730] = 1809;
indice[2731] = 1811;
indice[2732] = 1810;
indice[2733] = 1809;
indice[2734] = 1812;
indice[2735] = 1811;
indice[2736] = 1813;
indice[2737] = 1815;
indice[2738] = 1814;
indice[2739] = 1813;
indice[2740] = 1816;
indice[2741] = 1815;
indice[2742] = 1817;
indice[2743] = 1819;
indice[2744] = 1818;
indice[2745] = 1817;
indice[2746] = 1820;
indice[2747] = 1819;
indice[2748] = 1821;
indice[2749] = 1823;
indice[2750] = 1822;
indice[2751] = 1821;
indice[2752] = 1824;
indice[2753] = 1823;
indice[2754] = 1825;
indice[2755] = 1827;
indice[2756] = 1826;
indice[2757] = 1825;
indice[2758] = 1828;
indice[2759] = 1827;
indice[2760] = 1829;
indice[2761] = 1831;
indice[2762] = 1830;
indice[2763] = 1829;
indice[2764] = 1832;
indice[2765] = 1831;
indice[2766] = 1833;
indice[2767] = 1835;
indice[2768] = 1834;
indice[2769] = 1833;
indice[2770] = 1836;
indice[2771] = 1835;
indice[2772] = 1837;
indice[2773] = 1839;
indice[2774] = 1838;
indice[2775] = 1837;
indice[2776] = 1840;
indice[2777] = 1839;
indice[2778] = 1841;
indice[2779] = 1843;
indice[2780] = 1842;
indice[2781] = 1841;
indice[2782] = 1844;
indice[2783] = 1843;
indice[2784] = 1845;
indice[2785] = 1847;
indice[2786] = 1846;
indice[2787] = 1845;
indice[2788] = 1848;
indice[2789] = 1847;
indice[2790] = 1849;
indice[2791] = 1851;
indice[2792] = 1850;
indice[2793] = 1849;
indice[2794] = 1852;
indice[2795] = 1851;
indice[2796] = 1853;
indice[2797] = 1855;
indice[2798] = 1854;
indice[2799] = 1853;
indice[2800] = 1856;
indice[2801] = 1855;
indice[2802] = 1857;
indice[2803] = 1859;
indice[2804] = 1858;
indice[2805] = 1857;
indice[2806] = 1860;
indice[2807] = 1859;
indice[2808] = 1861;
indice[2809] = 1863;
indice[2810] = 1862;
indice[2811] = 1861;
indice[2812] = 1864;
indice[2813] = 1863;
indice[2814] = 1865;
indice[2815] = 1867;
indice[2816] = 1866;
indice[2817] = 1865;
indice[2818] = 1868;
indice[2819] = 1867;
indice[2820] = 1869;
indice[2821] = 1871;
indice[2822] = 1870;
indice[2823] = 1869;
indice[2824] = 1872;
indice[2825] = 1871;
indice[2826] = 1873;
indice[2827] = 1875;
indice[2828] = 1874;
indice[2829] = 1873;
indice[2830] = 1876;
indice[2831] = 1875;
indice[2832] = 1877;
indice[2833] = 1879;
indice[2834] = 1878;
indice[2835] = 1877;
indice[2836] = 1880;
indice[2837] = 1879;
indice[2838] = 1881;
indice[2839] = 1883;
indice[2840] = 1882;
indice[2841] = 1881;
indice[2842] = 1884;
indice[2843] = 1883;
indice[2844] = 1885;
indice[2845] = 1887;
indice[2846] = 1886;
indice[2847] = 1885;
indice[2848] = 1888;
indice[2849] = 1887;
indice[2850] = 1889;
indice[2851] = 1891;
indice[2852] = 1890;
indice[2853] = 1889;
indice[2854] = 1892;
indice[2855] = 1891;
indice[2856] = 1893;
indice[2857] = 1895;
indice[2858] = 1894;
indice[2859] = 1893;
indice[2860] = 1896;
indice[2861] = 1895;
indice[2862] = 1897;
indice[2863] = 1899;
indice[2864] = 1898;
indice[2865] = 1897;
indice[2866] = 1900;
indice[2867] = 1899;
indice[2868] = 1901;
indice[2869] = 1903;
indice[2870] = 1902;
indice[2871] = 1901;
indice[2872] = 1904;
indice[2873] = 1903;
indice[2874] = 1905;
indice[2875] = 1907;
indice[2876] = 1906;
indice[2877] = 1905;
indice[2878] = 1908;
indice[2879] = 1907;
indice[2880] = 1909;
indice[2881] = 1911;
indice[2882] = 1910;
indice[2883] = 1909;
indice[2884] = 1912;
indice[2885] = 1911;
indice[2886] = 1913;
indice[2887] = 1915;
indice[2888] = 1914;
indice[2889] = 1913;
indice[2890] = 1916;
indice[2891] = 1915;
indice[2892] = 1917;
indice[2893] = 1919;
indice[2894] = 1918;
indice[2895] = 1917;
indice[2896] = 1920;
indice[2897] = 1919;
indice[2898] = 1921;
indice[2899] = 1923;
indice[2900] = 1922;
indice[2901] = 1921;
indice[2902] = 1924;
indice[2903] = 1923;
indice[2904] = 1925;
indice[2905] = 1927;
indice[2906] = 1926;
indice[2907] = 1925;
indice[2908] = 1928;
indice[2909] = 1927;
indice[2910] = 1929;
indice[2911] = 1931;
indice[2912] = 1930;
indice[2913] = 1929;
indice[2914] = 1932;
indice[2915] = 1931;
indice[2916] = 1933;
indice[2917] = 1935;
indice[2918] = 1934;
indice[2919] = 1933;
indice[2920] = 1936;
indice[2921] = 1935;
indice[2922] = 1937;
indice[2923] = 1939;
indice[2924] = 1938;
indice[2925] = 1937;
indice[2926] = 1940;
indice[2927] = 1939;
indice[2928] = 1941;
indice[2929] = 1943;
indice[2930] = 1942;
indice[2931] = 1941;
indice[2932] = 1944;
indice[2933] = 1943;
indice[2934] = 1945;
indice[2935] = 1947;
indice[2936] = 1946;
indice[2937] = 1945;
indice[2938] = 1948;
indice[2939] = 1947;
indice[2940] = 1949;
indice[2941] = 1951;
indice[2942] = 1950;
indice[2943] = 1949;
indice[2944] = 1952;
indice[2945] = 1951;
indice[2946] = 1953;
indice[2947] = 1955;
indice[2948] = 1954;
indice[2949] = 1953;
indice[2950] = 1956;
indice[2951] = 1955;
indice[2952] = 1957;
indice[2953] = 1959;
indice[2954] = 1958;
indice[2955] = 1957;
indice[2956] = 1960;
indice[2957] = 1959;
indice[2958] = 1961;
indice[2959] = 1963;
indice[2960] = 1962;
indice[2961] = 1961;
indice[2962] = 1964;
indice[2963] = 1963;
indice[2964] = 1965;
indice[2965] = 1967;
indice[2966] = 1966;
indice[2967] = 1965;
indice[2968] = 1968;
indice[2969] = 1967;
indice[2970] = 1969;
indice[2971] = 1971;
indice[2972] = 1970;
indice[2973] = 1969;
indice[2974] = 1972;
indice[2975] = 1971;
indice[2976] = 1973;
indice[2977] = 1975;
indice[2978] = 1974;
indice[2979] = 1973;
indice[2980] = 1976;
indice[2981] = 1975;
indice[2982] = 1977;
indice[2983] = 1979;
indice[2984] = 1978;
indice[2985] = 1977;
indice[2986] = 1980;
indice[2987] = 1979;
indice[2988] = 1981;
indice[2989] = 1983;
indice[2990] = 1982;
indice[2991] = 1981;
indice[2992] = 1984;
indice[2993] = 1983;
indice[2994] = 1985;
indice[2995] = 1987;
indice[2996] = 1986;
indice[2997] = 1985;
indice[2998] = 1988;
indice[2999] = 1987;
indice[3000] = 1989;
indice[3001] = 1991;
indice[3002] = 1990;
indice[3003] = 1989;
indice[3004] = 1992;
indice[3005] = 1991;
indice[3006] = 1993;
indice[3007] = 1995;
indice[3008] = 1994;
indice[3009] = 1993;
indice[3010] = 1996;
indice[3011] = 1995;
indice[3012] = 1997;
indice[3013] = 1999;
indice[3014] = 1998;
indice[3015] = 1997;
indice[3016] = 2000;
indice[3017] = 1999;
indice[3018] = 2001;
indice[3019] = 2003;
indice[3020] = 2002;
indice[3021] = 2001;
indice[3022] = 2004;
indice[3023] = 2003;
indice[3024] = 2005;
indice[3025] = 2007;
indice[3026] = 2006;
indice[3027] = 2005;
indice[3028] = 2008;
indice[3029] = 2007;
indice[3030] = 2009;
indice[3031] = 2011;
indice[3032] = 2010;
indice[3033] = 2009;
indice[3034] = 2012;
indice[3035] = 2011;
indice[3036] = 2013;
indice[3037] = 2015;
indice[3038] = 2014;
indice[3039] = 2013;
indice[3040] = 2016;
indice[3041] = 2015;
indice[3042] = 2017;
indice[3043] = 2019;
indice[3044] = 2018;
indice[3045] = 2017;
indice[3046] = 2020;
indice[3047] = 2019;
indice[3048] = 2021;
indice[3049] = 2023;
indice[3050] = 2022;
indice[3051] = 2021;
indice[3052] = 2024;
indice[3053] = 2023;
indice[3054] = 2025;
indice[3055] = 2027;
indice[3056] = 2026;
indice[3057] = 2025;
indice[3058] = 2028;
indice[3059] = 2027;
indice[3060] = 2041;
indice[3061] = 2030;
indice[3062] = 2029;
indice[3063] = 2041;
indice[3064] = 2031;
indice[3065] = 2030;
indice[3066] = 2041;
indice[3067] = 2032;
indice[3068] = 2031;
indice[3069] = 2041;
indice[3070] = 2033;
indice[3071] = 2032;
indice[3072] = 2041;
indice[3073] = 2034;
indice[3074] = 2033;
indice[3075] = 2041;
indice[3076] = 2035;
indice[3077] = 2034;
indice[3078] = 2041;
indice[3079] = 2036;
indice[3080] = 2035;
indice[3081] = 2041;
indice[3082] = 2037;
indice[3083] = 2036;
indice[3084] = 2041;
indice[3085] = 2038;
indice[3086] = 2037;
indice[3087] = 2041;
indice[3088] = 2039;
indice[3089] = 2038;
indice[3090] = 2041;
indice[3091] = 2040;
indice[3092] = 2039;
indice[3093] = 2041;
indice[3094] = 2029;
indice[3095] = 2040;

	if(FAILED(mIBLTSpring->Unlock()))
		MessageBox(NULL, "mIBLTBox->Unlock() failed", "", MB_OK);
		
	if(FAILED(D3DXCreateTextureFromFile(md3dDevice, texname, &mLTSpringTex)))
	{
		MessageBox(NULL, "createLightTexSpringBuffer() D3DXCreateTextureFromFile() failed", "", MB_OK);
		return;
	}
}

void d3dclass::drawLTSpring()
{
	if(FAILED(md3dDevice->SetStreamSource(0, mVBLTSpring, 0 , sizeof(VPosNormalTex))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetIndices(mIBLTSpring)))
		MessageBox(NULL, "SetIndices() failed", "", MB_OK);
	if(FAILED(md3dDevice->SetVertexDeclaration(VPosNormalTex::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetTechnique(mhLT.mhTech)))
		MessageBox(NULL, "SetTechnique() failed", "", MB_OK);
		
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhTime, mtime)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);

	if(FAILED(mLightTexFX->SetValue(mhLT.mhCamPos, &mCamPos, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
		
		
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mLTGridMtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mLTGridMtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mLTGridMtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mLTGridMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetTexture(mhLT.mhTex, mLTSpringTex)))
		MessageBox(NULL, "SetTexture() failed", "", MB_OK);
	
	D3DXMATRIX	W,WIT,S;
	D3DXMatrixScaling(&S, mSpringScale.x,  mSpringScale.y,  mSpringScale.z);
	D3DXMatrixTranslation(&W, mLTSpringPos.x, mLTSpringPos.y, mLTSpringPos.z);
	D3DXMatrixInverse(&WIT, 0, &(S*W));
	D3DXMatrixTranspose(&WIT, &WIT);
	
	
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhW, &(S*W))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWVP, &(S*W*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
		
	UINT nPass;
	if(FAILED(mLightTexFX->Begin(&nPass, 0)))
	{
		MessageBox(NULL, "mLightTexFX->begin() failed", "", MB_OK);
		return;
	}
	for(UINT i=0; i<nPass; ++i)
	{
		if(FAILED(mLightTexFX->BeginPass(i)))
		{
		MessageBox(NULL, "mLightTexFX->BeginPass() failed", "", MB_OK);
		}
		
		if(FAILED(md3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 2042, 0, 3096/3)))
		MessageBox(NULL, "DrawIndexedPromitive() failed", "", MB_OK);
			
		if(FAILED(mLightTexFX->EndPass()))
		{
		MessageBox(NULL, "mLightTexFX->EndPass() failed", "", MB_OK);
		}
	}
	
	
	
	if(FAILED(mLightTexFX->End()))
	{
		MessageBox(NULL, "mLightTexFX->End() failed", "", MB_OK);
		return;
	}
}

void d3dclass::LoadXfile(LPCSTR filename, std::vector<MTRL>& mtrl, std::vector<IDirect3DTexture9*>& tex, ID3DXMesh** mesh)
{
		
	ID3DXMesh *tempMesh1=0, *tempMesh2=0;
	ID3DXBuffer *adjbuf=0, *mtrlbuf;
	DWORD nmtrl=0;
		
	if(FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, md3dDevice, &adjbuf, &mtrlbuf, 0, &nmtrl, &tempMesh1)))
		MessageBox(NULL, "D3DXLoadMeshFromX() failed", "", MB_OK);
		
	D3DVERTEXELEMENT9 elem1[MAX_FVF_DECL_SIZE],elem2[64];
	bool bNormal = false;
	UINT nElem=0;
	
	if(FAILED(tempMesh1->GetDeclaration(elem1)))
		MessageBox(NULL, "GetDeclaration() failed ", "", MB_OK);
		
	for(int i=0; i<MAX_FVF_DECL_SIZE; ++i)
	{
		if(elem1[i].Stream == 0xff)
			break;
			
		if(elem1[i].Type == D3DDECLTYPE_FLOAT3 && elem1[i].Usage == D3DDECLUSAGE_NORMAL && elem1[i].UsageIndex == 0)
		{
			bNormal = true;
			break;
		}
	}
	
	VPosNormalTex::Decl->GetDeclaration(elem2, &nElem);
	if(FAILED(tempMesh1->CloneMesh(D3DXMESH_SYSTEMMEM, elem2,md3dDevice,&tempMesh2)))
		MessageBox(NULL, "CloneMesh() failed", "", MB_OK);
		
	tempMesh1->Release();
	tempMesh1 = tempMesh2;
	
	if(bNormal == false)
	{
		if(FAILED(D3DXComputeNormals(tempMesh1, 0)))
			MessageBox(NULL, "D3DXComputeNormals() failed", "", MB_OK);
	}
	
	if(FAILED(tempMesh2->Optimize(D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, (DWORD *)adjbuf->GetBufferPointer(), 0 ,0 ,0, mesh)))
		MessageBox(NULL, "Optimize() failed", "", MB_OK);
		
	tempMesh1->Release();
	adjbuf->Release();
	
	if( mtrlbuf != 0 && nmtrl != 0)
	{
		D3DXMATERIAL* m1 = (D3DXMATERIAL*)mtrlbuf->GetBufferPointer();
		
		for(DWORD i=0; i<nmtrl; ++i)
		{
		
		MTRL m2;
		m2.ambient	= m1[i].MatD3D.Diffuse;
		m2.diffuse	= m1[i].MatD3D.Diffuse;
		m2.spec		= m1[i].MatD3D.Specular;
		m2.specPower	= m1[i].MatD3D.Power;
		mtrl.push_back(m2);
		
		if( m1[i].pTextureFilename != 0)
		{
			IDirect3DTexture9* t = 0;
			LPCSTR tfilename = m1[i].pTextureFilename;
			if(FAILED(D3DXCreateTextureFromFile(md3dDevice, tfilename, &t)))
				MessageBox(NULL, "LoadXfile() D3DXCreateTextureFromFile() failed", "", MB_OK);
			tex.push_back(t);
		}
		else
		{
			tex.push_back(0);
		}
		}
	mtrlbuf->Release();
	}
}

void d3dclass::calcFrustumPlane()
{
	D3DXMATRIX M = mView*mProj;
	D3DXVECTOR4 v0 = D3DXVECTOR4(M(0,0),M(1,0),M(2,0),M(3,0));
	D3DXVECTOR4 v1 = D3DXVECTOR4(M(0,1),M(1,1),M(2,1),M(3,1));
	D3DXVECTOR4 v2 = D3DXVECTOR4(M(0,2),M(1,2),M(2,2),M(3,2));
	D3DXVECTOR4 v3 = D3DXVECTOR4(M(0,3),M(1,3),M(2,3),M(3,3));
	
	mPlFrustum[0] = (D3DXPLANE)v2;			// near
	mPlFrustum[1] = (D3DXPLANE)(v3-v2);		// far
	mPlFrustum[2] = (D3DXPLANE)(v3+v0);		// left
	mPlFrustum[3] = (D3DXPLANE)(v3-v0);		// right
	mPlFrustum[4] = (D3DXPLANE)(v3-v1);		// top
	mPlFrustum[5] = (D3DXPLANE)(v3-v1);		// bottom
	
	for(int i=0; i<6; i++)
		D3DXPlaneNormalize(&mPlFrustum[i], &mPlFrustum[i]);
}

bool d3dclass::insideFrustumBox(const MYBOX& box)
{
	D3DXVECTOR3 p;
	
	for(int i=0; i<6; ++i)
	{
		for(int j=0; j<3; ++j)
		{
		
		if( mPlFrustum[i][j] >= 0.0f)
			p[j] = box.maxP[j];
		else
			p[j] = box.minP[j];
			
		}
		
		if(D3DXPlaneDotCoord(&mPlFrustum[i], &p) < 0.0f)
			return false;
	}
	return true;
}

D3DXMATRIX& d3dclass::getViewMtx()
{
	return mView;
}

D3DXMATRIX& d3dclass::getProjMtx()
{
	return mProj;
}

void d3dclass::d3dDrawLine(VPosCol *v,int numPoint,std::vector<VPosCol>* vec, D3DXMATRIX* lineW)
{
	
	md3dDevice->SetTransform(D3DTS_WORLD,lineW);
	md3dDevice->SetTransform(D3DTS_VIEW, &mView);
	md3dDevice->SetTransform(D3DTS_PROJECTION, &mProj);
	
	md3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	
	md3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
	if(v)
	{
	md3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, numPoint-1, v, sizeof(VPosCol));
	}
	if(vec)
	{
	md3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, vec->size()-1, &(vec->front()), sizeof(VPosCol));
	}
	/*
	if(FAILED(md3dDevice->SetStreamSource(0, vb, 0 , sizeof(VPosCol))))
		MessageBox(NULL, "SetStreamSource() failed", "", MB_OK);
		if(FAILED(md3dDevice->SetVertexDeclaration(VPosCol::Decl)))
		MessageBox(NULL, "SetVertexDeclaration() failed", "", MB_OK);
	if(FAILED(md3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 4)))
		MessageBox(NULL, "DrawPromitive() failed", "", MB_OK);
		*/
	md3dDevice->SetRenderState(D3DRS_LIGHTING, true);
}

void d3dclass::d3dDrawText3D(std::vector<MYTEXT3D>& text3d)
{
	if(text3d.size() > 0)
	{
		D3DXVECTOR4 p;
		RECT rect;
		float xoffset;
		GetClientRect(hD3DWnd, &rect);
		for(int i=0; i < text3d.size(); ++i)
		{
		
		if(text3d[i].bShow)
		{
		//p = (text3d[i].pos)*mView*mProj;
		//testesf(text3d[i].pos.x);
		//D3DXMatrixTranslation(&W, text3d[i].pos.x, text3d[i].pos.y, text3d[i].pos.z);
		//D3DXMatrixInverse(&W, 0,&(W*mView*mProj));
		//D3DXMatrixTranspose(&W, &W);
		D3DXVec4Transform(&p, &(D3DXVECTOR4(text3d[i].pos,1.0f)), &(mView*mProj));
		//testesf(p.y);
		p.x /= p.w;
		p.y /= -p.w;
		
		p.z /= p.w;
		//testesf(mProj(1,1));
		//testesf(p.y);
		//testesf(p.z);
		if( p.x > -1.1f && p.x < 1.1f && p.y > -1.1f && p.y < 1.1f && p.z > 0.0f && p.z < 1.0f)
		{
			p.x += 1.0f;
			p.y += 1.0f;
			xoffset = -(float)text3d[i].numChar*0.33*text3d[i].size;
			RECT rc = {p.x*rect.right*0.5f+xoffset,p.y*rect.bottom*0.5f,0,0};
	
	
		
			if(text3d[i].Font)
				text3d[i].Font->DrawText(0, text3d[i].text.c_str(), -1, &rc, DT_NOCLIP, text3d[i].col);
			else if(mFont)
				mFont->DrawText(0, text3d[i].text.c_str(), -1, &rc, DT_NOCLIP, text3d[i].col);
			}
		}
		}
	}
}