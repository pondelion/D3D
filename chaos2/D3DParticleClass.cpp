#include <windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include "D3DParticleClass.h"
#include "direct3Dclass.h"
#include "test1.h"

#pragma comment(lib, "d3d9.lib")

IDirect3DVertexDeclaration9*	PARTICLE::Decl = 0;

D3DParticleClass::D3DParticleClass(d3dclass* d3dc,LPCSTR fxname, LPCSTR techname, LPCSTR texname, const MYBOX& box, int maxparticle, float emitionrate)
{
	mBox = box;
	mTime = 0.0f;
	mMaxParticle = maxparticle;
	mTimePerEmition = emitionrate;
	D3DXMatrixIdentity(&mW);
	D3DXMatrixIdentity(&mInvW);
	mpD3DClass = d3dc;
	
	mParticle.resize(mMaxParticle);
	mAParticle.resize(mMaxParticle);
	mDParticle.resize(mMaxParticle);
	
	for(int i=0; i<mMaxParticle; ++i)
	{
		mParticle[i].lifetime	= -1.0f;
		mParticle[i].initTime	= 0.0f;
	}
	
	if(FAILED(D3DXCreateTextureFromFile(mpD3DClass->getD3DDevice(), texname, &mTex)))
		MessageBox(NULL, "particle CreateTextureFromFile() failed", "", MB_OK);
	
	ID3DXBuffer* err = 0;
	if(FAILED(D3DXCreateEffectFromFile(mpD3DClass->getD3DDevice(), fxname, 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &err)))
		MessageBox(NULL, "particle CreateEffectFromFile() failed", "", MB_OK);
		
	mhTech	= mFX->GetTechniqueByName(techname);
	mhWVP	= mFX->GetParameterByName(0, "gWVP");
	mhTex	= mFX->GetParameterByName(0, "gTex");
	mhTime	= mFX->GetParameterByName(0, "gTime");
	mhLCamPos = mFX->GetParameterByName(0, "gLCamPos");
	mhClientRHeight = mFX->GetParameterByName(0, "gClientRHeight");
	
	if(FAILED(mFX->SetTechnique(mhTech)))
		MessageBox(NULL, "particle SetTexhnique() failed", "", MB_OK);
	if(FAILED(mFX->SetTexture(mhTex, mTex)))
		MessageBox(NULL, "particle SetTexture() failed", "", MB_OK);
		
	if(FAILED((mpD3DClass->getD3DDevice())->CreateVertexBuffer(mMaxParticle*sizeof(PARTICLE), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 0, D3DPOOL_DEFAULT, &mVB, 0)))
		MessageBox(NULL, "particle CreateVertexBuffer() failed", "", MB_OK);
		
	D3DVERTEXELEMENT9 ParticleElem[] =
	{
	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		{0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
		{0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
		{0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	if(FAILED((mpD3DClass->getD3DDevice())->CreateVertexDeclaration(ParticleElem, &PARTICLE::Decl)))
		MessageBox(NULL, "particle CreateVertexDeclaration() failed", "", MB_OK);
	
	
}

D3DParticleClass::~D3DParticleClass()
{
	if(mFX)
	{
	mFX->Release();
	mFX = NULL;
	}
	if(mTex)
	{
	mTex->Release();
	mTex = NULL;
	}
	if(mVB)
	{
	mVB->Release();
	mVB = NULL;
	}
	if(PARTICLE::Decl)
	{
	(PARTICLE::Decl)->Release();
	PARTICLE::Decl = NULL;
	}
	
}

void D3DParticleClass::setWMtx(const D3DXMATRIX& w)
{
	mW = w;
	D3DXMatrixInverse(&mInvW,0, &mW);
}

void D3DParticleClass::setTimePerEmition(float tpe)
{
	mTimePerEmition = tpe;
}

void D3DParticleClass::setTime(float t)
{
	mTime = t;
}

void D3DParticleClass::onLostDevice()
{
	if(FAILED(mFX->OnLostDevice()))
		MessageBox(NULL, "particle mFX->OnLostDevice() failed", "", MB_OK);
		
	if(mVB)
	{
	mVB->Release();
	mVB = NULL;
	}
}

void D3DParticleClass::onResetDevice()
{
	if(FAILED(mFX->OnResetDevice()))
		MessageBox(NULL, "particle mFX->OnResetDevice() failed", "", MB_OK);
	
	if(mVB == NULL)
	{
		if(FAILED((mpD3DClass->getD3DDevice())->CreateVertexBuffer(mMaxParticle*sizeof(PARTICLE), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 0, D3DPOOL_DEFAULT, &mVB, 0)))
		MessageBox(NULL, "particle CreateVertexBuffer() failed", "", MB_OK);
	}
}

void D3DParticleClass::draw()
{
	D3DXVECTOR3	CamPosW	= mpD3DClass->getCamPos();
	D3DXVECTOR3 CamPosL;
	RECT	cr;
	
	D3DXVec3TransformCoord(&CamPosL, &CamPosW, &mInvW);
	GetClientRect(mpD3DClass->getD3DHwnd(), &cr);
	
	if(FAILED(mFX->SetFloat(mhTime, mTime)))
		MessageBox(NULL, "particle mFX->SetFloat() failed", "", MB_OK);
	if(FAILED(mFX->SetValue(mhLCamPos, &CamPosL, sizeof(D3DXVECTOR3))))
		MessageBox(NULL, "particle mFX->SetValue() failed", "", MB_OK);
	if(FAILED(mFX->SetMatrix(mhWVP, &(mW*(mpD3DClass->getViewMtx())*(mpD3DClass->getProjMtx())))))
		MessageBox(NULL, "particle mFX->SetMatrix() failed", "", MB_OK);
	if(FAILED(mFX->SetInt(mhClientRHeight, cr.bottom)))
		MessageBox(NULL, "particle mFX->SetInt() failed", "", MB_OK);
		
	UINT np = 0;
	if(FAILED(mFX->Begin(&np, 0)))
		MessageBox(NULL, "particle mFX->Begin() failed", "", MB_OK);
	if(FAILED(mFX->BeginPass(0)))
		MessageBox(NULL, "particle mFX->BeginPass() failed", "", MB_OK);
		
	if(FAILED((mpD3DClass->getD3DDevice())->SetStreamSource(0, mVB, 0, sizeof(PARTICLE))))
		MessageBox(NULL, "particle SetStreamSource() failed", "", MB_OK);
	if(FAILED((mpD3DClass->getD3DDevice())->SetVertexDeclaration(PARTICLE::Decl)))
		MessageBox(NULL, "particle SetVertexDeclaration() failed", "", MB_OK);
		
	MYBOX bW;
	mBox.transformBox(mW, bW);
	if(mpD3DClass->insideFrustumBox(bW))
	{
		PARTICLE *p = 0;
		if(FAILED(mVB->Lock(0,0, (void**)&p, D3DLOCK_DISCARD)))
			MessageBox(NULL, "particle mVB->Lock() failed", "", MB_OK);
	
		
		UINT i;
		int j=0;
		for(i=0; i<mAParticle.size(); ++i)
		{
		
		p[j] = *mAParticle[i];
		++j;
		}
		
		if(FAILED(mVB->Unlock()))
			MessageBox(NULL, "particle mVB->Unlock() failed", "", MB_OK);
		
		if(i > 0)
		{
			if(FAILED((mpD3DClass->getD3DDevice())->DrawPrimitive(D3DPT_POINTLIST, 0, i)))
				MessageBox(NULL, "particle DrawPrimitive() failed", "", MB_OK);
		}
		
	}
	
	if(FAILED(mFX->EndPass()))
		MessageBox(NULL, "particle mFX->EndPass() failed", "", MB_OK);
	if(FAILED(mFX->End()))
		MessageBox(NULL, "particle mFX->End() failed", "", MB_OK);
}

void D3DParticleClass::update(float dt)
{
	mTime += dt;
	
	mDParticle.resize(0);
	mAParticle.resize(0);
	
	static int j=0;
	j++;
	for(int i=0; i<mMaxParticle; ++i)
	{
		if((mTime-mParticle[i].initTime)>mParticle[i].lifetime)
		{
			
			mDParticle.push_back(&mParticle[i]);
		}
		else
		{
			
			mAParticle.push_back(&mParticle[i]);
		}
	}
	
	if(mTimePerEmition>0)
	{
		static int k = 0;
		static float t = 0.0f;
		t += dt;
		/*
		if(k > 5000)
		{
			k = 0;
			testes;
		}
		*/
		while(t > mTimePerEmition)
		{
		++k;
		
		//if(k<100)
		addParticle();
		t -= mTimePerEmition;
		}
		
		
	}
}
	
	