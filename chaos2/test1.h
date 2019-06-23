#include <windows.h>


#define WM_MYPARAM (WM_APP+1)
#define WM_PAINTPLOT (WM_APP+2)
#define WM_D3DRESET (WM_APP+3)
#define WM_D3DRELEASE (WM_APP+4)
#define WM_D3DCLASSOBJ (WM_APP+5)

#define IDM_END 1000
#define IDM_MINIMIZE 1001
#define IDM_CREATEPLOT 1002
#define IDM_DESTROYPLOT 1003
#define IDM_SETPARAMETER 1004
#define IDM_D3DCREATE 1005
#define IDM_D3DDESTROY 1006
#define IDM_D3DPARAMETERDLG 1007

#define ID_TAB 2000
#define ID_STATIC 2001
#define ID_EDIT1 2002
#define ID_EDIT2 2003
#define ID_BUTTON1 2004
#define ID_BUTTON2 2005
#define ID_BUTTON3 2006

#define testes MessageBox(NULL, "testes", "", MB_OK)
#define testes2 MessageBox(NULL, "testes2", "", MB_OK)
#define testesf(f) 	\
{					\
	float x = (float)f;								\
	char buf[32],buf2[16] = "%f";	\
	sprintf(buf, buf2, x);			\
	MessageBox(NULL, buf, "", MB_OK);	\
}


	


