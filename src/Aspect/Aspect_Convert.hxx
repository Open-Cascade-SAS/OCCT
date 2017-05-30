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
#include <Standard_Real.hxx>

//! Auxiliary functions for DCU <-> Pixels conversions.
namespace Aspect_Convert
{

  inline Standard_Integer Round (Standard_Real theValue)
  {
    return Standard_Integer(theValue + (theValue >= 0 ? 0.5 : -0.5 ));
  }

  inline void ConvertCoordinates (const Standard_Integer theParentPxSizeX, const Standard_Integer theParentPxSizeY,
                                  const Standard_Real theQCenterX, const Standard_Real theQCenterY,
                                  const Standard_Real theQSizeX,   const Standard_Real theQSizeY,
                                  Standard_Integer& thePxLeft,  Standard_Integer& thePxTop,
                                  Standard_Integer& thePxSizeX, Standard_Integer& thePxSizeY)
  {
    Standard_Real theParentSizeMin = Min (theParentPxSizeX, theParentPxSizeY);
    thePxSizeX = Round (theQSizeX * theParentSizeMin);
    thePxSizeY = Round (theQSizeY * theParentSizeMin);
    Standard_Integer thePxCenterX = Round(theQCenterX * Standard_Real (theParentPxSizeX));
    Standard_Integer thePxCenterY = Round((1.0 - theQCenterY) * Standard_Real (theParentPxSizeY));
    thePxLeft = thePxCenterX - thePxSizeX / 2;
    thePxTop  = thePxCenterY - thePxSizeY / 2;
  }

  inline void ConvertCoordinates (const Standard_Integer theParentPxSizeX, const Standard_Integer theParentPxSizeY,
                                  const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                                  const Standard_Integer thePxSizeX, const Standard_Integer thePxSizeY,
                                  Standard_Real& theQCenterX, Standard_Real& theQCenterY,
                                  Standard_Real& theQSizeX,   Standard_Real& theQSizeY)
  {
    Standard_Real theParentSizeMin = Min (theParentPxSizeX, theParentPxSizeY);
    theQSizeX = Standard_Real(thePxSizeX) / theParentSizeMin;
    theQSizeY = Standard_Real(thePxSizeY) / theParentSizeMin;
    Standard_Integer thePxCenterX = thePxLeft + thePxSizeX / 2;
    Standard_Integer thePxCenterY = thePxTop  + thePxSizeY / 2;
    theQCenterX =       Standard_Real (thePxCenterX) / Standard_Real (theParentPxSizeX);
    theQCenterY = 1.0 - Standard_Real (thePxCenterY) / Standard_Real (theParentPxSizeY);
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

} // namespace Aspect_Convert

#endif /* _Aspect_Convert_HeaderFile */
