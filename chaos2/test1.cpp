
#include <windows.h>
#include <commctrl.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "test1.h"
#include "plotclass.h"
#include "direct3Dclass.h"
#include "D3DParticleClass.h"
#include <fstream>
#define N 10
#define M 10

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "d3dx9.lib")

std::ofstream fout("log.txt");
HWND MyInitApp(HINSTANCE, int, COLORREF, LPCSTR, RECT );
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
float calcHeight(float x, float z, int n, int m);

struct MAINAPPPARAM
{
	d3dclass *myd3d;
};

class test1d3d : public d3dclass
{
public:
	test1d3d(HINSTANCE hInst, HWND hwndParent, LPSTR lpstrCaption, D3DDEVTYPE devtype, DWORD VP, RECT *rc);
	~test1d3d();
	
	void update(float dt);
	void draw();
	void initDirect3D();
	void lostDevice();
	void resetDevice();
	
	D3DParticleClass* mParticle;
	
private:
	ID3DXMesh  *mTestMesh;
	std::vector<MTRL>  mTestMtrl;
	std::vector<IDirect3DTexture9*> mTestTex;
	
	VPosCol pointLine[5];
	D3DXMATRIX lineW;
	std::vector<VPosCol> LineVec;
	
	//IDirect3DVertexBuffer9* mVBLine;
};

class test1particle : public D3DParticleClass
{
public:
	test1particle(d3dclass* d3dc,LPCSTR fxname, LPCSTR techname, LPCSTR texname, const MYBOX& box, int maxparticle, float emitionrate)
: D3DParticleClass(d3dc,fxname,techname, texname, box,maxparticle,emitionrate)
	{
	}
	//virtual ~test1particle();
	
	void addParticle()
	{
	
	if(mDParticle.size() > 0)
	{
	
	PARTICLE* p = mDParticle.back();
	
	p->initTime = mTime;
	p->lifetime = randomFloat(2.0f, 4.0f);
	p->initSize = randomFloat(12.0f, 17.0f);
	p->mass		= randomFloat(1.0f, 2.0f);
	p->initColor = randomFloat(0.5f, 1.0f)*D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	randomVec(p->initVel);
	
	float theta = randomFloat(0, 2.0f*D3DX_PI);
	float r = randomFloat(5.0f, 7.0f);
	p->initPos.x = cosf(theta)*r;
	p->initPos.y = sinf(theta)*r;
	p->initPos.z = randomFloat(-1.0f, 1.0f);
	
	mDParticle.pop_back();
	mAParticle.push_back(p);
	}
	}
	
};



test1d3d::test1d3d(HINSTANCE hInst, HWND hwndParent, LPSTR lpstrCaption, D3DDEVTYPE devtype, DWORD VP, RECT *rc)
: d3dclass(hInst, hwndParent, lpstrCaption, devtype, VP,rc)
{
	srand(time_t(0));
	mParticle = NULL;
	//if(FAILED(D3DXCreateSphere(md3dDevice, 1.0f, 20, 20, &mSphere, 0)))
	//	MessageBox(NULL, "CreateSphere() failed", "", NULL);
	
	pointLine[0] = VPosCol(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0xff0000ff);
	pointLine[1] = VPosCol(D3DXVECTOR3(0.0f, 5.2f, 0.0f),0xff0000ff);
	pointLine[2] = VPosCol(D3DXVECTOR3(5.2f, 5.2f, 0.0f), 0xff0000ff);
	pointLine[3] = VPosCol(D3DXVECTOR3(5.2f, 5.2f, 5.2f), 0xff0000ff);
	pointLine[4] = VPosCol(D3DXVECTOR3(0.2f, 0.2f, 0.2f), 0xff0000ff);
	
	LineVec.push_back(VPosCol(D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0xffff00ff));
	LineVec.push_back(VPosCol(D3DXVECTOR3(0.0f, 3.0f, 5.0f), 0xffff00ff));
	LineVec.push_back(VPosCol(D3DXVECTOR3(2.0f, 6.0f, 1.0f), 0xffff00ff));
	LineVec.push_back(VPosCol(D3DXVECTOR3(3.0f, 0.0f, 3.0f), 0xffff00ff));
	
	D3DXMatrixIdentity(&lineW);
	
	
}
	
test1d3d::~test1d3d()
{
	
	
	if(mTestMesh)
	{
	mTestMesh->Release();
	mTestMesh = NULL;
	}
	for(int i=0; i < mTestTex.size(); ++i)
	{
	if(mTestTex[i])
	{
	mTestTex[i]->Release();
	mTestTex[i] = NULL;
	}
	}
	
	if(mParticle)
	{
	delete mParticle;
	mParticle = NULL;
	}
	/*
	if(mVBLine)
	{
		mVBLine->Release();
		mVBLine = NULL;
	}
	*/
	
	
}

void test1d3d::initDirect3D()
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
	initVertexDeclarations();
	
	createMeshGridBuffer(101, 101, 0.1, 0.1, D3DXVECTOR3(0.0, 0.0, 0.0));
	createMeshGridFX();
	//createPointLightGridBuffer(100, 100, 0.5, 0.5, D3DXVECTOR3(0.0, 0.0, 0.0));

	//createPointLightFX();
	
	createLightTexFX();
	
	createSphere("..\\resource\\img\\wall21_c.jpg", 1.0f, D3DXVECTOR3(0.0f, 4.0f, 0.0f));

	createLightTexGridBuffer(101, 101, 0.1, 0.1, D3DXVECTOR3(0.0, 0.0, 0.0), "..\\resource\\img\\ground0.dds");
	
	createLightTexBoxBuffer("..\\resource\\img\\wall21_c.jpg");
	
	createLightTexSpringBuffer("..\\resource\\img\\sozai1.jpg");
	
	//mVectorMesh = NULL;
	//LoadXfile("..\\resource\\x\\vector.x", mVectorMtrl, mVectorTex, &mVectorMesh);
	createArrow("..\\resource\\x\\vector.x");
	
	GetCurrentDirectory(MAX_PATH+1, crDir);
	SetCurrentDirectory("..\\resource\\x");
	LoadXfile("medaka.x", mTestMtrl, mTestTex, &mTestMesh);
	SetCurrentDirectory(crDir);
	
	/*
		VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTGrid->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
	

	for(int i = 0; i < 101; ++i)
	{
		for(int j = 0; j < 101; ++j)
		{
		
		if(i == 0)
		{
		
		DWORD k = i*101 + j;
		vpnt[k].pos.y = 10.0*sin(vpnt[k].pos.x);
		
		}
		
		if(j == 0)
		{
		DWORD k = i*101 + j;
		vpnt[k].pos.y = -5.0*cos(vpnt[k].pos.z/3.0f);
		}
		if(j == 99)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = -8.0*cos(vpnt[k].pos.z/3.0f);
		}
		if(i == 99)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = sin(vpnt[k].pos.x);
		}
		if( i == 50 && j == 50)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = 5.0f;
		}
		}
	}
	
	if(FAILED(mVBLTGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
	*/
	
	VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTGrid->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
	
	//float siny = sin(mtime);
	D3DXVECTOR3 n;
	for(int i = 0; i <= 100; ++i)
	{
		for(int j = 0; j <= 100; ++j)
		{
		DWORD k = i*101 + j;
		
		vpnt[k].pos.y = 10.0f*calcHeight((float)i/10.0f, (float)j/10.0f, N, M);
		if(i != 100 && j != 100)
		{
		D3DXVec3Cross(&n, &(vpnt[k+101].pos-vpnt[k].pos), &(vpnt[k+1].pos-vpnt[k].pos));
		vpnt[k].n = n;
		}
		}
	}

	if(FAILED(mVBLTGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
	
	
	
	mText.push_back(MYTEXT3D("テストの文", D3DXVECTOR3(0.0f, 3.0f, 0.0f), D3DXCOLOR(0xff0000ff), 30, 5,md3dDevice));
	
	
	resetDevice();
	
}

void test1d3d::lostDevice()
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
	
	
	if(mParticle)
	{
		mParticle->onLostDevice();
	}
	
}

void test1d3d::resetDevice()
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
	
	if(mParticle)
	{
		mParticle->onResetDevice();
	}
	
	setProjMtx();
	
}

void test1d3d::update(float dt)
{
	static float frame = 0.0;
	static float time = 0.0f;
	static float y=6.0f, vy=0.0f;
	
	if(dt >= 1.0f)
		return;
	
	mtime += dt;
	time += dt;
	frame += 1.0;
	
	//mLightPos.x = 4.0*sin(mtime);
	//mLightPos.z = 4.0*cos(mtime);
	mLightPos.y = 1.5f + sin(mtime);
	
	//mCamPos.y = 8.0f * sin(mtime*0.5f);
	//mCamPos.x = 8.0f * cos(0.5f*mtime);
	//mCamPos.z = 8.0f * sin(0.5f*mtime);
	
	vy += -10.0f*dt;
	y += vy*dt;
	if(y <= 0.5f)
	{
	y = 0.5f;
	vy = -vy;
	}
	mSpherePos.y = y;
	
	//mTarget.y = 3.0f;
	//mTarget = mCamPos+mCamLook;
	
	mLTBoxPos.y = 0.5f;
	
	mSpringScale.x = 3.0f + (2.0f*sin(2.0f*mtime));
	
	if(time >= 1.0)
	{
	
	mfps = frame;
	mmpf = 1000.0 / mfps;
	
	time = 0.0;
	frame = 0.0;
	}
	
	
	if(mParticle)
	{
		
			mParticle->update(dt);
		
	}
	
	setViewMtx();
	
	
}

void test1d3d::draw()
{
	if(md3dDevice)
	{
		
		if(FAILED(md3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0)))
		{
		MessageBox(NULL, "Clear failed", "", MB_OK);
		}
	
		GetClientRect(hD3DWnd, &clientrc);
		
		if(FAILED(md3dDevice->BeginScene()))
		{
			MessageBox(NULL, "BeginScene() failed", "", MB_OK);
		}
		
		//drawPLGrid();
		drawMeshGrid();
		
		/*
		VPosNormalTex* vpnt = 0;
	if(FAILED(mVBLTGrid->Lock(0, 0, (void**)&vpnt, 0)))
		MessageBox(NULL, "VertexBuffer Lock() failed", "", MB_OK);
	
	float siny = sin(mtime);
	D3DXVECTOR3 n;
	for(int i = 1; i < 99; ++i)
	{
		for(int j = 1; j < 99; ++j)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = (vpnt[k+1].pos.y+vpnt[k-1].pos.y+vpnt[k+100].pos.y+vpnt[k-100].pos.y )/4.0f;
		if( j==50 && i == 50)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = 5.0f*sin(mtime);
		}
		if( j==45 && i == 45)
		{
		DWORD k = i*100 + j;
		vpnt[k].pos.y = 4.0f*cos(mtime/4.0f);
		}
		D3DXVec3Cross(&n, &(vpnt[k+100].pos-vpnt[k].pos), &(vpnt[k+1].pos-vpnt[k].pos));
		vpnt[k].n = n;
		}
	}

	if(FAILED(mVBLTGrid->Unlock()))
		MessageBox(NULL, "VertexBuffer Unlock() failed", "", MB_OK);
		*/
		drawLTGrid();
		
		/*
		
		drawLTBox();
		mSpherePos = D3DXVECTOR3(-(mSpringScale.x+1.0f), 2.0f, 0.0f);
		drawSphere();
		drawLTSpring();
		
		mSpherePos = D3DXVECTOR3(mSpringScale.x+1.0f, 2.0f, 0.0f);
		
		drawSphere();
		
		mText[0].pos.x = mSpringScale.x+1.0f;
		
		//++++++++++++++++++++++++++++++++++++++++++++++++++
		D3DXMATRIX	W,WIT,S;
		mLTSpringPos.y = 1.0f;
	//D3DXMatrixScaling(&S, mSpringScale.y,  mSpringScale.x,  mSpringScale.z);
	//D3DXMatrixScaling(&S, 10.0f,  mSpringScale.x, 10.0f);
	D3DXMatrixScaling(&S, 1.0f,  1.0f, 1.0f);
	D3DXMatrixTranslation(&W, mLTSpringPos.x, mLTSpringPos.y, mLTSpringPos.z);
	D3DXMatrixInverse(&WIT, 0, &(S*W));
	D3DXMatrixTranspose(&WIT, &WIT);
	
	if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(mLTGridMtrl.ambient), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(mLTGridMtrl.diffuse), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(mLTGridMtrl.spec), sizeof(D3DXCOLOR))))
		MessageBox(NULL, "SetValue() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, mLTGridMtrl.specPower)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhW, &(S*W))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWIT, &WIT)))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
	if(FAILED(mLightTexFX->SetMatrix(mhLT.mhWVP, &(S*W*mView*mProj))))
		MessageBox(NULL, "SetMatrix() failed", "", MB_OK);
		
		
		mLightTexFX->SetTechnique(mhLT.mhTech);
	UINT numPasses = 0;
	mLightTexFX->Begin(&numPasses, 0);
	mLightTexFX->BeginPass(0);

	
	if(FAILED(mLightTexFX->SetValue(mhLT.mhDifMtrl, &(D3DXCOLOR(1.0f, 1.0f,1.0f, 1.0f)), sizeof(D3DXCOLOR))))
			MessageBox(NULL, "SetValue() failed", "", MB_OK);
		if(FAILED(mLightTexFX->SetValue(mhLT.mhAmbMtrl, &(D3DXCOLOR(0.6f, 0.6f,0.6f, 0.6f)), sizeof(D3DXCOLOR))))
			MessageBox(NULL, "SetValue() failed", "", MB_OK);
			if(FAILED(mLightTexFX->SetValue(mhLT.mhSpecMtrl, &(D3DXCOLOR(1.0f, 1.0f,1.0f, 0.6f)), sizeof(D3DXCOLOR))))
			MessageBox(NULL, "SetValue() failed", "", MB_OK);
		if(FAILED(mLightTexFX->SetFloat(mhLT.mhPower, 16)))
		MessageBox(NULL, "SetFloat() failed", "", MB_OK);
	for(int j = 0; j < mVectorMtrl.size(); ++j)
	{
		
		
		// If there is a texture, then use.
		if(mVectorTex[j] != 0)
		{
			mLightTexFX->SetTexture(mhLT.mhTex, mVectorTex[j]);
		}

		
		mLightTexFX->CommitChanges();
		if(!mVectorMesh)
		testes;
		mVectorMesh->DrawSubset(j); // 矢印
		
	}
	
	for(int j = 0; j < mTestMtrl.size(); ++j)
	{
		
		
		// If there is a texture, then use.
		if(mTestTex[j] != 0)
		{
			mLightTexFX->SetTexture(mhLT.mhTex, mTestTex[j]);

		}
			
	

	
		mLightTexFX->CommitChanges();
		
		mTestMesh->DrawSubset(j); // fish
		
	}
	
	
	mLightTexFX->EndPass();
	mLightTexFX->End();
		
		//************************************************
		if(mParticle)
			mParticle->draw();
		
		*/
		
		drawLTXYZaxis(D3DXVECTOR3(0.0f,0.0f,0.0f),6.0f);
		
		d3dDrawLine(&pointLine[0],5, &LineVec, &lineW);
	
		
		drawFPS();
		
		d3dDrawText3D(mText);
		
		if(FAILED(md3dDevice->EndScene()))
		{
			MessageBox(NULL, "EndScene() failed", "", MB_OK);
		}
		
		if(FAILED(md3dDevice->Present(0, 0,0,0)))
		{
			MessageBox(NULL, "Present() failed", "", MB_OK);
		}
		
		
	}
}
	

	
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpsCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	RECT rc,d3drc;
	COLORREF bgCol = RGB(0,0,0);
	test1d3d *myd3d;
	MAINAPPPARAM myparam;
	__int64 curTime = 0, prevTime = 0, pf=0;
	D3DParticleClass* d3dparticle;
	
	rc.left = CW_USEDEFAULT;
	rc.top = CW_USEDEFAULT;
	rc.right = CW_USEDEFAULT;
	rc.bottom = CW_USEDEFAULT;
	
	
	msg.message = WM_NULL;
	
	
	hwnd = MyInitApp(hInstance, nCmdShow, bgCol, "test1", rc);
	if(!hwnd)
		return FALSE;
		
	GetClientRect(hwnd, &d3drc);
	d3drc.right *= 3.0/4.0;
	

	
	myd3d = new test1d3d(hInstance, hwnd, "Direct3D", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3drc);
	
	myd3d->initWindow();
	
	myd3d->initDirect3D();
	
	myparam.myd3d = myd3d;
	
	SendMessage(myd3d->getD3DHwnd(), WM_D3DCLASSOBJ, 0, (LPARAM)myd3d);
	SendMessage(hwnd, WM_MYPARAM, 0, (LPARAM)&myparam);
	
	MYBOX box;
	box.minP = D3DXVECTOR3(-15.0f, -15.0f, -15.0f);
	box.maxP = D3DXVECTOR3( 15.0f,  15.0f,  15.0f);
	myd3d->mParticle = new test1particle(myd3d, "..\\resource\\fx\\particle.fx", "particleTech", "..\\resource\\img\\torch.dds", box, 1500, 0.0025f);
		
	RECT paramrc;
	paramrc.left = d3drc.right;
	paramrc.right = d3drc.right/3.0f;
	paramrc.top = d3drc.bottom*0.5f;
	paramrc.bottom = d3drc.bottom*0.5f;
	//myd3d->mParamClass = new parameterclass(hInstance, &hwnd, paramrc);
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&pf);
	float spc = 1.0f / (float)pf;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);
	
	while(msg.message != WM_QUIT)
	{
		
		if(PeekMessage(&msg, 0, 0,0, PM_REMOVE))
		{
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
		
		myd3d->update((float)((curTime-prevTime)*spc));
		
		myd3d->draw();
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		prevTime = curTime;
		}
		
		else
		{
			if(myd3d->isPaused())
			{
			Sleep(20);
			continue;
			//QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
			//myd3d->update((float)((curTime-prevTime)*spc));
			//prevTime = curTime;
			}
			
			if(!(myd3d->isDeviceLost()))
			{
			QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
			
			myd3d->update((float)((curTime-prevTime)*spc));
			myd3d->draw();
			prevTime = curTime;
			}
		}
		
	}
	return msg.wParam;
}


HWND MyInitApp(HINSTANCE hInst, int nCmdShow,COLORREF bgCol, LPCSTR lpszCaption, RECT rc)
{
	HWND hwnd;
	WNDCLASSEX wc;
	

	wc.cbSize 		= sizeof(WNDCLASSEX);
	wc.style 		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc 	= WndProc;
	wc.cbClsExtra 	= 0;
	wc.cbWndExtra 	= 0;
	wc.hInstance 	= hInst;
	wc.hIcon 		= LoadIcon(hInst, "MYICON");
	wc.hCursor 		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(bgCol);
	wc.lpszMenuName = "MYMENU";
	wc.lpszClassName = "test1class";
	wc.hIconSm = LoadIcon(hInst, "MYICON");
	
	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "RegisterClassEx Failed", "", MB_OK);
		return 0;
	}
		
	hwnd = CreateWindow("test1class", 
		lpszCaption,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		rc.left, 
		rc.top, 
		rc.right, //幅
		rc.bottom, //高さ
		NULL,
		NULL,
		hInst,
		NULL);
		
	if(!hwnd)
	{
		MessageBox(NULL, "CreateWindow Failed", "", MB_OK);
		return 0;
	}
		
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	return hwnd;
}

		
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HINSTANCE hInst;
	static myplot* myplot1;
	static HWND hwndplot;
	static RECT rc,rcplot;
	static MAINAPPPARAM *myparam;

	

		
	switch(msg)
	{
	case WM_MYPARAM:
		myparam = (MAINAPPPARAM *)lp;
		break;
	case WM_CREATE:
		
		GetClientRect(hwnd, &rcplot);
		rcplot.left = rcplot.right*3.0/4.0;
		rcplot.right = rcplot.right-rcplot.right*3.0/4.0;
		rcplot.bottom = rcplot.right;
		hInst = ((LPCREATESTRUCT)lp)->hInstance;
		myplot1 = new myplot(hInst, hwnd);
		hwndplot = myplot1->InitializePlot(&rcplot);
		SetFocus(hwndplot);
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDM_CREATEPLOT:
			
			hwndplot = myplot1->create(NULL);
			break;
		case IDM_DESTROYPLOT:
			myplot1->destroy();
			hwndplot = NULL;
			break;
		case IDM_SETPARAMETER:
			myplot1->setParameterDialog();
			break;
		case IDM_END:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	
	case WM_PAINTPLOT:
		if(hwndplot)
		{
		
		
		myplot1->drawaxis(myplot1->getOrigin(), 3, RGB(255, 0, 0));
		}
		break;
		
	case WM_D3DRESET:
		
		if(myparam->myd3d->getD3DDevice())
		{
		myparam->myd3d->lostDevice();
		myparam->myd3d->reset();
		myparam->myd3d->resetDevice();
		}
		break;
	/*case WM_D3DRELEASE:
		myparam->myd3d->d3drelease();
		
		break;
	*/
	case WM_SIZE:
		if(myparam)
		{
		if(myparam->myd3d->getD3DDevice())
		{
			
			if(wp == SIZE_MINIMIZED)
			{
				myparam->myd3d->setPaused(true);
				myparam->myd3d->setMinMax(true);
			}
			else if(wp == SIZE_MAXIMIZED)
			{
				myparam->myd3d->setPaused(false);
				myparam->myd3d->lostDevice();
				myparam->myd3d->reset();
				myparam->myd3d->resetDevice();
			}
			else if(wp == SIZE_RESTORED)
			{
				myparam->myd3d->setPaused(false);
				myparam->myd3d->setMinMax(false);
			}
		}
		}
		break;
		
	case WM_CLOSE:
		if(MessageBox(hwnd, "終了します", "終了", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			
			myplot1->destroy();
			hwndplot = NULL;
			delete myplot1;
			delete myparam->myd3d;
			
			
			DestroyWindow(hwnd);
			
		}
		return 0;
		
	case WM_DESTROY:
		
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
	

float calcHeight(float x, float z, int n, int m)
{
	float y=0.0f;
	int i,j;
	DWORD t;
	t = GetTickCount();
	for(i=1;i<=n;i++)
	{
		for(j=1;j<=m;j++)
		{
		y += (1.0f-cos((float)i*D3DX_PI/2.0f))*(1.0f-cos((float)j*D3DX_PI/2.0f))*sin((float)i*D3DX_PI*x/10.0f)*sin((float)j*D3DX_PI*z/10.0f)/((float)i*(float)j*D3DX_PI*D3DX_PI);
		
		}
	}
	fout << t << std::endl;
	return y;
}
