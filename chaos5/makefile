TARGETNAME=test1
LINK32=link.exe

test1.exe : test1.obj plotclass.obj direct3Dclass.obj D3DParticleClass.obj test1.res
	link test1.obj plotclass.obj direct3Dclass.obj D3DParticleClass.obj test1.res

test1.obj : test1.cpp
	cl /c test1.cpp
	
plotclass.obj : plotclass.cpp
	cl /c plotclass.cpp
	
direct3Dclass.obj : direct3Dclass.cpp
	cl /c direct3Dclass.cpp
	
D3DParticleClass.obj : D3DParticleClass.cpp
	cl /c D3DParticleClass.cpp
	
test1.res : test1.rc
	rc test1.rc
