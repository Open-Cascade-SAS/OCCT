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
