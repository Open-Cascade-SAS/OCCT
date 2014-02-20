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

#ifndef _Aspect_Convert_HeaderFile
#define _Aspect_Convert_HeaderFile

#include <Standard.hxx>
#include <Quantity_Parameter.hxx>

//! Auxiliary functions for DCU <-> Pixels conversions.
namespace Aspect_Convert
{

  inline Standard_Integer Round (Standard_Real theValue)
  {
    return Standard_Integer(theValue + (theValue >= 0 ? 0.5 : -0.5 ));
  }

  inline void ConvertCoordinates (const Standard_Integer theParentPxSizeX, const Standard_Integer theParentPxSizeY,
                                  const Quantity_Parameter theQCenterX, const Quantity_Parameter theQCenterY,
                                  const Quantity_Parameter theQSizeX,   const Quantity_Parameter theQSizeY,
                                  Standard_Integer& thePxLeft,  Standard_Integer& thePxTop,
                                  Standard_Integer& thePxSizeX, Standard_Integer& thePxSizeY)
  {
    Quantity_Parameter theParentSizeMin = Min (theParentPxSizeX, theParentPxSizeY);
    thePxSizeX = Round (theQSizeX * theParentSizeMin);
    thePxSizeY = Round (theQSizeY * theParentSizeMin);
    Standard_Integer thePxCenterX = Round(theQCenterX * Quantity_Parameter (theParentPxSizeX));
    Standard_Integer thePxCenterY = Round((1.0 - theQCenterY) * Quantity_Parameter (theParentPxSizeY));
    thePxLeft = thePxCenterX - thePxSizeX / 2;
    thePxTop  = thePxCenterY - thePxSizeY / 2;
  }

  inline void ConvertCoordinates (const Standard_Integer theParentPxSizeX, const Standard_Integer theParentPxSizeY,
                                  const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                                  const Standard_Integer thePxSizeX, const Standard_Integer thePxSizeY,
                                  Quantity_Parameter& theQCenterX, Quantity_Parameter& theQCenterY,
                                  Quantity_Parameter& theQSizeX,   Quantity_Parameter& theQSizeY)
  {
    Quantity_Parameter theParentSizeMin = Min (theParentPxSizeX, theParentPxSizeY);
    theQSizeX = Quantity_Parameter(thePxSizeX) / theParentSizeMin;
    theQSizeY = Quantity_Parameter(thePxSizeY) / theParentSizeMin;
    Standard_Integer thePxCenterX = thePxLeft + thePxSizeX / 2;
    Standard_Integer thePxCenterY = thePxTop  + thePxSizeY / 2;
    theQCenterX =       Quantity_Parameter (thePxCenterX) / Quantity_Parameter (theParentPxSizeX);
    theQCenterY = 1.0 - Quantity_Parameter (thePxCenterY) / Quantity_Parameter (theParentPxSizeY);
  }

  inline void FitIn (const Standard_Integer theParentPxSizeX, const Standard_Integer theParentPxSizeY,
                     Standard_Integer& thePxLeft,  Standard_Integer& thePxTop,
                     Standard_Integer& thePxSizeX, Standard_Integer& thePxSizeY)
  {
    if (thePxLeft < 0)
    {
      //thePxSizeX -= 2 * thePxLeft;
      thePxLeft = 0;
    }
    if ((thePxLeft + thePxSizeX) > theParentPxSizeX)
    {
      thePxSizeX = theParentPxSizeX - thePxLeft;
    }

    if (thePxTop < 0)
    {
      //thePxSizeY -= 2 * thePxTop;
      thePxTop = 0;
    }
    if ((thePxTop + thePxSizeY) > theParentPxSizeY)
    {
      thePxSizeY = theParentPxSizeY - thePxTop;
    }
  }

};

#endif /* _Aspect_Convert_HeaderFile */
