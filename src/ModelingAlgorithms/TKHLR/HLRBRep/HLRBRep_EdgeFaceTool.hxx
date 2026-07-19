// Created on: 1993-10-18
// Created by: Christophe MARION
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

#ifndef _HLRBRep_EdgeFaceTool_HeaderFile
#define _HLRBRep_EdgeFaceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <HLRBRep_TypeDef.hxx>

class gp_Dir;

//! The EdgeFaceTool computes the UV coordinates at a
//! given parameter on a Curve and a Surface. It also
//! compute the signed curvature value in a direction
//! at a given u,v point on a surface.
class HLRBRep_EdgeFaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static double CurvatureValue(const HLRBRep_SurfacePtr F,
                                               const double             U,
                                               const double             V,
                                               const gp_Dir&            Tg);

  //! return True if U and V are found.
  Standard_EXPORT static bool UVPoint(const double             Par,
                                      const HLRBRep_CurvePtr   E,
                                      const HLRBRep_SurfacePtr F,
                                      double&                  U,
                                      double&                  V);
};

#endif // _HLRBRep_EdgeFaceTool_HeaderFile
