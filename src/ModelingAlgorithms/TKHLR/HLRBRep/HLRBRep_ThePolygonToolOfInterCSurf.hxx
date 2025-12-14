// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
#define _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>

#include "IntCurveSurface_PolygonToolUtils.pxx"

class HLRBRep_ThePolygonToolOfInterCSurf
{
public:
  DEFINE_STANDARD_ALLOC

  //! Give the bounding box of the polygon.
  static const Bnd_Box& Bounding(const HLRBRep_ThePolygonOfInterCSurf& thePolygon)
  {
    return IntCurveSurface_PolygonToolUtils::Bounding(thePolygon);
  }

  static Standard_Real DeflectionOverEstimation(const HLRBRep_ThePolygonOfInterCSurf& thePolygon)
  {
    return IntCurveSurface_PolygonToolUtils::DeflectionOverEstimation(thePolygon);
  }

  static Standard_Boolean Closed(const HLRBRep_ThePolygonOfInterCSurf& thePolygon)
  {
    return IntCurveSurface_PolygonToolUtils::Closed(thePolygon);
  }

  static Standard_Integer NbSegments(const HLRBRep_ThePolygonOfInterCSurf& thePolygon)
  {
    return IntCurveSurface_PolygonToolUtils::NbSegments(thePolygon);
  }

  //! Give the point of range Index in the Polygon.
  static const gp_Pnt& BeginOfSeg(const HLRBRep_ThePolygonOfInterCSurf& thePolygon,
                                  const Standard_Integer                Index)
  {
    return IntCurveSurface_PolygonToolUtils::BeginOfSeg(thePolygon, Index);
  }

  //! Give the point of range Index in the Polygon.
  static const gp_Pnt& EndOfSeg(const HLRBRep_ThePolygonOfInterCSurf& thePolygon,
                                const Standard_Integer                Index)
  {
    return IntCurveSurface_PolygonToolUtils::EndOfSeg(thePolygon, Index);
  }

  Standard_EXPORT static void Dump(const HLRBRep_ThePolygonOfInterCSurf& thePolygon);
};

#endif // _HLRBRep_ThePolygonToolOfInterCSurf_HeaderFile
