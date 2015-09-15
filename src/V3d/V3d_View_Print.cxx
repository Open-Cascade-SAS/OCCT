// Created by: THA
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// e-mail	    t-hartl@muenchen.matra-dtv.fr

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifdef _WIN32
#include <windows.h>
#pragma comment( lib, "comdlg32.lib"  )
#endif


#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_TypeMismatch.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#ifdef _WIN32
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

//=============================================================================
//function : SetGrid
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::Print (const Aspect_Handle    thePrintDC,
                                  const Standard_Boolean theShowDialog,
                                  const Standard_Boolean theShowBackground,
                                  const Standard_CString theFilename,
                                  const Aspect_PrintAlgo thePrintAlgorithm) const
{
#ifdef _WIN32
  if (myView->IsDefined())
  {
    if (thePrintDC != NULL)
    {
      return myView->Print (thePrintDC, theShowBackground, theFilename, thePrintAlgorithm);
    }

    if (device._pd.hDC == NULL || theShowDialog)
    {
      if (device._pd.hDC)
        DeleteDC (device._pd.hDC);
      if (!theShowDialog)
      {
        device._pd.Flags = PD_RETURNDC | PD_NOSELECTION | PD_RETURNDEFAULT;
      }
      else
      {
        device._pd.Flags = PD_RETURNDC | PD_NOSELECTION;
      }

      BOOL ispd;
      ispd = PrintDlg((LPPRINTDLG)(&(device._pd)));

      if (!ispd)
      {
        return Standard_False;
      }

      if (!(device._pd.hDC)) 
      {
        if (device._pd.hDevNames) 
        {
          GlobalFree (device._pd.hDevNames);
          device._pd.hDevNames = NULL;
        }
        if (device._pd.hDevMode)
        {
          GlobalFree (device._pd.hDevMode);
          device._pd.hDevMode = NULL;
        }
        MessageBox (0, "Couldn't create Printer Device Context", "Error", MB_OK | MB_ICONSTOP);
        return Standard_False;
      }
    }

    // process scale factor accordingly to the new printing approach
    DEVMODE* aMode = (LPDEVMODE)GlobalLock(device._pd.hDevMode);

    // convert percents to multiplication factor, 100% = 1.0
    Standard_Real aScaleFactor = (Standard_Real) aMode->dmScale / 100.0;
    GlobalUnlock (device._pd.hDevMode);
    return myView->Print (device._pd.hDC, theShowBackground, theFilename, thePrintAlgorithm, aScaleFactor);
  }
#else
  Standard_NotImplemented::Raise ("V3d_View::Print is implemented only on Windows");
#endif
  return Standard_False;
}
