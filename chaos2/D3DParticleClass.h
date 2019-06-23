#ifndef D3DPARTICLECLASS_H
#define D3DPARTICLECLASS_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include "direct3Dclass.h"

#pragma comment(lib, "d3d9.lib")

struct PARTICLE
{
	D3DXVECTOR3 initPos;
	D3DXVECTOR3 initVel;
	float		initSize;
	float		initTime;
	float 		lifetime;
	float		mass;
	D3DXCOLOR	initColor;
	
	static IDirect3DVertexDeclaration9*	Decl;
};

class D3DParticleClass
{
protected:
	D3DXHANDLE mhTech, mhWVP, mhTex, mhTime, mhClientRHeight, mhLCamPos;

	IDirect3DVertexBuffer9	*mVB;
	IDirect3DTexture9*		mTex;
	MYBOX					mBox;
	D3DXMATRIX mW, mInvW;
	float mTime,mTimePerEmition;
	int mMaxParticle;
	std::vector<PARTICLE>	mParticle;
	std::vector<PARTICLE*>	mAParticle;
	std::vector<PARTICLE*>	mDParticle;
	d3dclass*			mpD3DClass;
	
public:
	D3DParticleClass(d3dclass* d3dc,LPCSTR fxname, LPCSTR techname, LPCSTR texname, const MYBOX& box, int maxparticle, float emitionrate);
	virtual ~D3DParticleClass();
	
	void setWMtx(const D3DXMATRIX& w);
	void setTimePerEmition(float tpe);
	void setTime(float t);
	
	
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void draw();
	virtual void update(float dt);
	virtual void addParticle() {}
	
	ID3DXEffect	*mFX;
	
};

#endif
	