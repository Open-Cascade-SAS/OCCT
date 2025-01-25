// Created on: 1993-12-03
// Created by: Jacques GOUSSARD
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

#ifndef _BlendFunc_HeaderFile
#define _BlendFunc_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <BlendFunc_SectionShape.hxx>
#include <Convert_ParameterisationType.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomAbs_Shape.hxx>

class gp_Pnt2d;
class gp_Vec;

//! This package provides a set of generic functions, that can
//! instantiated to compute blendings between two surfaces
//! (Constant radius, Evolutive radius, Ruled surface).
class BlendFunc
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void GetShape(const BlendFunc_SectionShape  SectShape,
                                       const Standard_Real           MaxAng,
                                       Standard_Integer&             NbPoles,
                                       Standard_Integer&             NbKnots,
                                       Standard_Integer&             Degree,
                                       Convert_ParameterisationType& TypeConv);

  Standard_EXPORT static void Knots(const BlendFunc_SectionShape SectShape,
                                    TColStd_Array1OfReal&        TKnots);

  Standard_EXPORT static void Mults(const BlendFunc_SectionShape SectShape,
                                    TColStd_Array1OfInteger&     TMults);

  Standard_EXPORT static void GetMinimalWeights(const BlendFunc_SectionShape       SectShape,
                                                const Convert_ParameterisationType TConv,
                                                const Standard_Real                AngleMin,
                                                const Standard_Real                AngleMax,
                                                TColStd_Array1OfReal&              Weigths);

  //! Used  to obtain the next level of continuity.
  Standard_EXPORT static GeomAbs_Shape NextShape(const GeomAbs_Shape S);

  Standard_EXPORT static Standard_Boolean ComputeNormal(const Handle(Adaptor3d_Surface)& Surf,
                                                        const gp_Pnt2d&                  p2d,
                                                        gp_Vec&                          Normal);

  Standard_EXPORT static Standard_Boolean ComputeDNormal(const Handle(Adaptor3d_Surface)& Surf,
                                                         const gp_Pnt2d&                  p2d,
                                                         gp_Vec&                          Normal,
                                                         gp_Vec&                          DNu,
                                                         gp_Vec&                          DNv);
};

#endif // _BlendFunc_HeaderFile
