#ifndef DIRECT3DCLASS_H
#define DIRECT3DCLASS_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <vector>
#include <string>
#include <tchar.h>

#include "test1.h"


#pragma comment(lib, "d3d9.lib")


float randomFloat(float a, float b);
void randomVec(D3DXVECTOR3& v);
LRESULT CALLBACK MyD3DWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

struct MTRL
{
	MTRL()
		:ambient(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)), diffuse(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)), spec(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)), specPower(8.0f){}
	MTRL(const D3DXCOLOR& a, const D3DXCOLOR& d, 
		 const D3DXCOLOR& s, float power)
		:ambient(a), diffuse(d), spec(s), specPower(power){}

	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	float specPower;
};


struct D3DPARAM
{
	HWND *hD3DWnd,*hParent;
	BOOL *pbPaused, *pbMinMax;
	RECT *pclientrc;
	IDirect3DDevice9 **ppd3dDevice;
	IDirect3D9 **ppd3d9;
	D3DPRESENT_PARAMETERS *pd3dpp;
};

struct D3DXGRIDFXHANDLE
{
	D3DXHANDLE		mhTech;
	D3DXHANDLE		mhWVP;
	D3DXHANDLE		mhTime;
};

struct D3DXPOINTLIGHTFXHANDLE
{
	D3DXHANDLE		mhTech,mhW,mhWIT,mhWVP, mhCamPos, mhAmbMtrl, mhDifMtrl, mhSpecMtrl, mhPower, mhLightPos, mhAmbLight, mhDifLight, mhSpecLight, mhAttenuation, mhTime;

};

struct D3DXLIGHTTEXHANDLE
{
	D3DXHANDLE	mhTech,mhW,mhWIT,mhWVP,mhAmbMtrl,mhAmbLight,mhDifMtrl,mhDifLight,mhSpecMtrl,mhSpecLight,mhVLight,mhCamPos,mhPower,mhTex,mhTime;
	MTRL		mtrl;
};


struct VPos
{
	VPos():pos(0.0f, 0.0f, 0.0f){}
	VPos(float x, float y, float z):pos(x,y,z){}
	VPos(const D3DXVECTOR3& v):pos(v){}

	D3DXVECTOR3 pos;
	static IDirect3DVertexDeclaration9* Decl;
};

struct VPosCol
{
	VPosCol():pos(0.0f,0.0f,0.0f),col(0x00000000){}
	VPosCol(float x, float y, float z, D3DCOLOR c):pos(x,y,z),col(c){}
	VPosCol(const D3DXVECTOR3& v, D3DCOLOR c):pos(v),col(c){}
	
	D3DXVECTOR3	pos;
	D3DCOLOR	col;
	static IDirect3DVertexDeclaration9* Decl;
};



struct VPosNormal
{
	VPosNormal():pos(0.0f, 0.0f, 0.0f),n(0.0f,0.0f,0.0f){}
	VPosNormal(float x, float y, float z, float nx, float ny, float nz):pos(x,y,z),n(nx,ny,nz){}
	VPosNormal(const D3DXVECTOR3& v, const D3DXVECTOR3& n):pos(v),n(n){}
	
	D3DXVECTOR3 pos;
	D3DXVECTOR3	n;
	static IDirect3DVertexDeclaration9* Decl;
};

struct VPosNormalTex
{
	VPosNormalTex():pos(0.0f,0.0f,0.0f),n(0.0f,0.0f,0.0f),tex(0.0f,0.0f){}
	VPosNormalTex(float x, float y, float z, float nx, float ny, float nz, float u, float v):pos(x,y,z),n(nx,ny,nz),tex(u,v){}
	VPosNormalTex(const D3DXVECTOR3& v, const D3DXVECTOR3& n, const D3DXVECTOR2& t):pos(v),n(n),tex(t){}
	
	D3DXVECTOR3 pos;
	D3DXVECTOR3 n;
	D3DXVECTOR2 tex;
	
	static IDirect3DVertexDeclaration9*	Decl;
};
	

struct MYBOX
{
	MYBOX():minP(FLT_MAX, FLT_MAX, FLT_MAX),maxP(-FLT_MAX,-FLT_MAX,-FLT_MAX){}
	
	D3DXVECTOR3 center()const
	{
		return (minP+maxP)*0.5f;
	}
	
	D3DXVECTOR3 halfMinToMax()const
	{
		return (maxP-minP)*0.5f;
	}
	
	void transformBox(const D3DXMATRIX& W, MYBOX& box)
	{
		D3DXVECTOR3 p = center();
		D3DXVECTOR3 v = halfMinToMax();
		D3DXMATRIX	W2;
		D3DXMatrixIdentity(&W2);
		
		D3DXVec3TransformCoord(&p, &p, &W);
		
		W2(0,0) = fabsf(W(0,0));
		W2(0,1) = fabsf(W(0,1));
		W2(0,2) = fabsf(W(0,2));
		W2(0,3) = fabsf(W(0,3));
		W2(1,0) = fabsf(W(1,0));
		W2(1,1) = fabsf(W(1,1));
		W2(1,2) = fabsf(W(1,2));
		W2(1,3) = fabsf(W(1,3));
		W2(2,0) = fabsf(W(2,0));
		W2(2,1) = fabsf(W(2,1));
		W2(2,2) = fabsf(W(2,2));
		W2(2,3) = fabsf(W(2,3));
		W2(3,0) = fabsf(W(3,0));
		W2(3,1) = fabsf(W(3,1));
		W2(3,2) = fabsf(W(3,2));
		W2(3,3) = fabsf(W(3,3));
		
		D3DXVec3TransformNormal(&v, &v, &W2);
		
		box.minP = p - v;
		box.maxP = p + v;
	}
	
	D3DXVECTOR3 minP, maxP;
};

struct MYTEXT3D
{
	MYTEXT3D():text(""),pos(0.0f,0.0f,0.0f),col(0xff000000),size(12),numChar(0)
	{
	Font = NULL;
	bShow = TRUE;
	}
	MYTEXT3D(LPCSTR t,D3DXVECTOR3& p,D3DXCOLOR& c,int s,int n,IDirect3DDevice9* md3dDevice)
	{
		text = t;
		pos = p;
		col = c;
		size = s;
		numChar = n;
		Font = NULL;
		bShow = TRUE;
	
		D3DXFONT_DESC fontDesc;
	
		fontDesc.Height          = size;
		fontDesc.Width           = 0;
		fontDesc.Weight          = 0;
		fontDesc.MipLevels       = 1;
		fontDesc.Italic          = false;
		fontDesc.CharSet         = DEFAULT_CHARSET;
		fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
		fontDesc.Quality         = DEFAULT_QUALITY;
		fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy(fontDesc.FaceName, _T("Times New Roman"));
	
	
		if(FAILED(D3DXCreateFontIndirect(md3dDevice, &fontDesc, &Font)))
		{
		MessageBox(NULL, "CreateFont failed", "", MB_OK);
		}
	}
	
	BOOL bShow;
	std::string text;
	D3DXVECTOR3 pos;
	D3DXCOLOR col;
	int size,numChar;
	ID3DXFont *Font;
};

class d3dclass
{
protected:
	HWND hD3DWnd, hParent;
	HINSTANCE hInst;
	D3DDEVTYPE mDevType;
	DWORD mVP;
	RECT wndPos,clientrc;
	LPSTR lpCaption;
	D3DPARAM d3dParam;
	IDirect3D9*	md3d9;
	IDirect3DDevice9* md3dDevice;
	D3DPRESENT_PARAMETERS md3dpp;
	BOOL mbPaused,mbMinMax;
	float mfps,mmpf,mtime;
	ID3DXFont *mFont;
	IDirect3DVertexBuffer9	*mVBGrid, *mVBPLGrid, *mVBLTGrid, *mVBLTBox, *mVBLTSpring;
	IDirect3DIndexBuffer9	*mIBGrid, *mIBPLGrid, *mIBLTGrid, *mIBLTBox, *mIBLTSpring;
	ID3DXEffect			*mCurFX,*mGridFX,*mPointLightFX,*mLightTexFX;
	D3DXGRIDFXHANDLE			mhGrid;
	D3DXPOINTLIGHTFXHANDLE		mhPL;
	D3DXLIGHTTEXHANDLE			mhLT;
	D3DXMATRIX mView, mProj, mWorld;
	int mNVR, mNVC, mNVRLT, mNVCLT;
	IDirect3DTexture9	*mSphereTex, *mLTGridTex, *mLTBoxTex, *mLTSpringTex;
	ID3DXMesh	*mSphere,*mArrowMesh;
	
	D3DXCOLOR	mAmbLight, mDifLight, mSpecLight, mSphereAmbLight, mSphereDifLight, mSphereSpecLight;
	D3DXVECTOR3	mLightPos,mLightVec;
	D3DXVECTOR3 mCamPos,mCamLook,mCamUp, mCamRight, mSpherePos, mTarget, mLTGridPos, mLTBoxPos , mLTSpringPos, mLTXYZaxisPos;
	D3DXVECTOR3 mAttenuation;
	D3DXVECTOR3 mSpringScale;
	float mCamSpeed;
	
	MTRL	mGridMtrl, mLTGridMtrl, mLTSphereMtrl, mLTBoxMtrl;
	D3DXMATRIX	mGridWorld;
	D3DXPLANE	mPlFrustum[6];
	
	std::vector<MYTEXT3D> mText;
	std::vector<MTRL> mArrowMtrl;
	std::vector<IDirect3DTexture9*> mArrowTex;
	TCHAR crDir[MAX_PATH+1];
	
	
	
public:
	d3dclass(HINSTANCE hInst, HWND hwndParent, LPSTR lpstrCaption, D3DDEVTYPE devtype, DWORD VP, RECT *rc);
	virtual ~d3dclass();

	HWND getD3DHwnd();
	IDirect3DDevice9* getD3DDevice();
	BOOL isPaused();
	void setPaused(BOOL paused);
	void setMinMax(BOOL minmax);
	
	virtual void initWindow();
	virtual void initDirect3D();
	virtual void lostDevice();
	virtual void resetDevice();
	virtual void update(float dt)	{}
	virtual void draw()				{}
	virtual void setProjMtx();
	virtual void setViewMtx();
	void setTarget(D3DXVECTOR3& t);
	void setCamPos(D3DXVECTOR3& camPos);
	bool deviceCap();
	void drawFPS();
	void initFont();
	void createMeshGrid(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center, std::vector<D3DXVECTOR3>& v, std::vector<DWORD>& i);
	void createMeshGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center);
	virtual void initVertexDeclarations();
	virtual void destroyVertexDeclarations();
	void createMeshGridFX();
	void drawMeshGrid();
	void createPointLightFX();
	void createPointLightGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center);
	void setFX(ID3DXEffect* fx);
	void drawPLGrid();
	void createLightTexFX();
	void createSphere(LPCSTR texname, float r, D3DXVECTOR3& pos);
	void setSphereTex(LPCSTR texname);
	void drawSphere();
	void createLightTexGridBuffer(int nVR, int nVC, float dx, float dz, const D3DXVECTOR3& center, LPCSTR texname);
	void drawLTGrid();
	void createLightTexBoxBuffer(LPCSTR texname);
	void drawLTBox();
	void setLTBoxPos(D3DXVECTOR3& p);
	void createLightTexSpringBuffer(LPCSTR texname);
	void drawLTSpring();
	void d3dDrawLine(VPosCol *v,int numPoint,std::vector<VPosCol>* vec, D3DXMATRIX* lineW);
	void d3dDrawText3D(std::vector<MYTEXT3D>& text3d);
	void createArrow(LPCSTR filename);
	void drawLTXYZaxis(D3DXVECTOR3& org,float size);
	D3DXVECTOR3& getCamPos();
	D3DXVECTOR3& getCamLook();
	D3DXVECTOR3& getCamUp();
	D3DXVECTOR3& getCamRight();
	void setCamLook(D3DXVECTOR3& v);
	void setCamUp(D3DXVECTOR3& v);
	void setCamRight(D3DXVECTOR3& v);
	float getCamSpeed();
	float getTime();
	void LoadXfile(LPCSTR filename, std::vector<MTRL>& mtrl, std::vector<IDirect3DTexture9*>& tex, ID3DXMesh** mesh);
	D3DXMATRIX& getViewMtx();
	D3DXMATRIX& getProjMtx();
	
	virtual void d3drelease();
	
	void reset();
	bool isDeviceLost();
	void calcFrustumPlane();
	bool insideFrustumBox(const MYBOX& box);
	
	//parameterclass* mParamClass;
};

#endif
