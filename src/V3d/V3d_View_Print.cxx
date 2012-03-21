// Created by: THA
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

Standard_Boolean V3d_View::Print (const Aspect_Handle    hPrnDC,
                                  const Standard_Boolean showDialog,
                                  const Standard_Boolean showBackground,
                                  const Standard_CString filename,
                                  const Aspect_PrintAlgo printAlgorithm) const
{
#ifdef WNT
	if( MyView->IsDefined() ) 
	{
		if (hPrnDC != NULL)
		{
			return MyView->Print(hPrnDC, showBackground,
			                     filename, printAlgorithm) ;
			
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
				return Standard_False;
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
				return Standard_False;
			}
		}

    // process scale factor accordingly to the new printing approach
    DEVMODE* aMode = (LPDEVMODE)GlobalLock(device._pd.hDevMode);

    // convert percents to multiplication factor, 100% = 1.0
    Standard_Real aScaleFactor = (Standard_Real) aMode->dmScale / 100.0;
    GlobalUnlock (device._pd.hDevMode);
   return MyView->Print(device._pd.hDC, showBackground,
                        filename, printAlgorithm, aScaleFactor) ;
	}
#else
	Standard_NotImplemented::Raise ("V3d_View::Print is implemented only on Windows");
#endif
  return Standard_False;
}
