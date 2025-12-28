// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile
#define _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <IntCurveSurface_ThePolygonOfHInter.hxx>

class IntCurveSurface_ThePolygonToolOfHInter
{
public:
  DEFINE_STANDARD_ALLOC

  //! Give the bounding box of the polygon.
  static const Bnd_Box& Bounding(const IntCurveSurface_ThePolygonOfHInter& thePolygon)
  {
    return thePolygon.Bounding();
  }

  static double DeflectionOverEstimation(
    const IntCurveSurface_ThePolygonOfHInter& thePolygon)
  {
    return thePolygon.DeflectionOverEstimation();
  }

  static bool Closed(const IntCurveSurface_ThePolygonOfHInter& thePolygon)
  {
    return thePolygon.Closed();
  }

  static int NbSegments(const IntCurveSurface_ThePolygonOfHInter& thePolygon)
  {
    return thePolygon.NbSegments();
  }

  //! Give the point of range Index in the Polygon.
  static const gp_Pnt& BeginOfSeg(const IntCurveSurface_ThePolygonOfHInter& thePolygon,
                                  const int                    Index)
  {
    return thePolygon.BeginOfSeg(Index);
  }

  //! Give the point of range Index in the Polygon.
  static const gp_Pnt& EndOfSeg(const IntCurveSurface_ThePolygonOfHInter& thePolygon,
                                const int                    Index)
  {
    return thePolygon.EndOfSeg(Index);
  }

  Standard_EXPORT static void Dump(const IntCurveSurface_ThePolygonOfHInter& thePolygon);
};

#endif // _IntCurveSurface_ThePolygonToolOfHInter_HeaderFile
