// File         V3d_View_Print.cxx
// Created      March 2000
// Author       THA
// e-mail	    t-hartl@muenchen.matra-dtv.fr

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifdef WNT
#include <windows.h>
#pragma comment( lib, "comdlg32.lib"  )
#endif

#include <V3d_View.jxx>
#include <Standard_NotImplemented.hxx>

#ifdef WNT
struct Device
{
	Device();
	~Device();
	
	PRINTDLG _pd;
};

//**********************************************************************

static Device device;

//**********************************************************************

Device::Device()
{
	memset(&_pd, 0, sizeof(PRINTDLG));
	_pd.hDevNames = NULL;
	_pd.hDevMode = NULL;
	_pd.lStructSize = sizeof(PRINTDLG);
}

//**********************************************************************

Device::~Device()
{	
	// :TODO:
	if (_pd.hDevNames) GlobalFree(_pd.hDevNames);
	if (_pd.hDevMode) GlobalFree(_pd.hDevMode);
	if (_pd.hDC) DeleteDC(_pd.hDC);
}
#endif

/************************************************************************/
/* Print Method                                                        */
/************************************************************************/

void V3d_View::Print (const Aspect_Handle       hPrnDC,
                      const Standard_Boolean    showDialog,
                      const Standard_Boolean    showBackground,
                      const Standard_CString    filename) const 
{
#ifdef WNT
	if( MyView->IsDefined() ) 
	{
		if (hPrnDC != NULL)
		{
			MyView->Print(hPrnDC, showBackground, filename) ;
			return;
		}

		if (device._pd.hDC == NULL || showDialog )
		{
			if (device._pd.hDC)
				DeleteDC(device._pd.hDC);
			if ( !showDialog )
			{
				device._pd.Flags = PD_RETURNDC | PD_NOSELECTION | PD_RETURNDEFAULT;
			}
			else
			{
				device._pd.Flags = PD_RETURNDC | PD_NOSELECTION;
			}

			BOOL	ispd;
			ispd = PrintDlg((LPPRINTDLG)(&(device._pd)));
		
			if (!ispd)
			{
				return;
			}
			
			if (!(device._pd.hDC)) 
			{
				if (device._pd.hDevNames) 
				{
					GlobalFree(device._pd.hDevNames);
					device._pd.hDevNames = NULL;
				}
				if (device._pd.hDevMode)
				{
					GlobalFree(device._pd.hDevMode);
					device._pd.hDevMode = NULL;
				}
				MessageBox(0, "Couldn't create Printer Device Context", "Error", MB_OK | MB_ICONSTOP);
				return;
			}
		}
		MyView->Print(device._pd.hDC, showBackground, filename) ;
	}
#else
	Standard_NotImplemented::Raise ("V3d_View::Print is implemented only on Windows");
#endif
}
