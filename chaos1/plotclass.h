#ifndef _PLOT_CLASS
#define _PLOT_CLASS

#include <windows.h>

LRESULT CALLBACK PlotProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK myParameterProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK myStaticProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK myEditOrgXProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK myEditOrgYProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

struct MYPARAM
{
	HWND *hwndPlot,*hwndParam, *hTab, *hwndStatic,*hwndEditOrgX,*hwndEditOrgY, *hokButton, *hcaButton, *hapButton;
	//WNDPROC orgProc;
	RECT *pos,*parentpos;
	HWND hwndParent;
	//WNDPROC* orgStaticProc;
	//HMENU hMenu;
	POINT *origin;
	
};

class myplot
{
	private:
		HINSTANCE hInst;
		HWND hwndPlot, hwndParent, hwndParamDlg, hTab, hwndStatic,hwndEditOrgX,hwndEditOrgY, hokButton, hcaButton, hapButton;
		//WNDPROC orgStaticProc;
		RECT pos,parentpos,clientplot,clientparam;
		MYPARAM param;
		//HMENU hMenu;
		
		POINT origin;
		BOOL bGridLine;
		double scale;
		
	public:
		myplot();
		myplot(HINSTANCE hInst, HWND hwnd);
		HWND InitializePlot(RECT* rc);
		HWND create(RECT* rc);
		void destroy();
		void drawaxis( POINT org, int width, COLORREF col);
		POINT getOrigin();
		void setOrigin(POINT pt);
		void setParameterDialog();
		
};
#endif

		