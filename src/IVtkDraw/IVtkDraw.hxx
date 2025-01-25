// Created on: 2012-02-03
//
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _IVtkDraw_HeaderFile
#define _IVtkDraw_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>

class Draw_Interpretor;
class WNT_WClass;

class IVtkDraw
{
public:
  DEFINE_STANDARD_ALLOC

  //! VTK window creation parameters.
  struct IVtkWinParams
  {
    Graphic3d_Vec2i  TopLeft;
    Graphic3d_Vec2i  Size;
    Standard_Integer NbMsaaSample;
    Standard_Boolean UseSRGBColorSpace;

    IVtkWinParams()
        : NbMsaaSample(0),
          UseSRGBColorSpace(false)
    {
    }
  };

public:
  Standard_EXPORT static void ViewerInit(const IVtkWinParams& theParams);

  static void ViewerInit(Standard_Integer thePxLeft,
                         Standard_Integer thePxTop,
                         Standard_Integer thePxWidth,
                         Standard_Integer thePxHeight)
  {
    IVtkWinParams aParams;
    aParams.TopLeft.SetValues(thePxLeft, thePxTop);
    aParams.Size.SetValues(thePxWidth, thePxHeight);
    ViewerInit(aParams);
  }

  Standard_EXPORT static void Factory(Draw_Interpretor& theDI);
  Standard_EXPORT static void Commands(Draw_Interpretor& theCommands);

private:
  Standard_EXPORT static const Handle(WNT_WClass)& WClass();
};

#endif
