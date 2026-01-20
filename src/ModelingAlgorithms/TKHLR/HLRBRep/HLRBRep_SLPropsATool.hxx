// Created on: 1993-04-23
// Created by: Modelistation
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

#ifndef _HLRBRep_SLPropsATool_HeaderFile
#define _HLRBRep_SLPropsATool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRBRep_TypeDef.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Vec;

class HLRBRep_SLPropsATool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the point <P> of parameter <U> and <V>
  //! on the Surface <A>.
  static void Value(const HLRBRep_SurfacePtr A,
                    const double      U,
                    const double      V,
                    gp_Pnt&                  P);

  //! Computes the point <P> and first derivative <D1*>
  //! of parameter <U> and <V> on the Surface <A>.
  static void D1(const HLRBRep_SurfacePtr A,
                 const double      U,
                 const double      V,
                 gp_Pnt&                  P,
                 gp_Vec&                  D1U,
                 gp_Vec&                  D1V);

  //! Computes the point <P>, the first derivative <D1*>
  //! and second derivative <D2*> of parameter <U> and
  //! <V> on the Surface <A>.
  static void D2(const HLRBRep_SurfacePtr A,
                 const double      U,
                 const double      V,
                 gp_Pnt&                  P,
                 gp_Vec&                  D1U,
                 gp_Vec&                  D1V,
                 gp_Vec&                  D2U,
                 gp_Vec&                  D2V,
                 gp_Vec&                  DUV);

  static gp_Vec DN(const HLRBRep_SurfacePtr A,
                   const double      U,
                   const double      V,
                   const int   Nu,
                   const int   Nv);

  //! returns the order of continuity of the Surface <A>.
  //! returns 1 : first derivative only is computable
  //! returns 2 : first and second derivative only are
  //! computable.
  static int Continuity(const HLRBRep_SurfacePtr A);

  //! returns the bounds of the Surface.
  static void Bounds(const HLRBRep_SurfacePtr A,
                     double&           U1,
                     double&           V1,
                     double&           U2,
                     double&           V2);

};

#include <HLRBRep_SLPropsATool.lxx>

#endif // _HLRBRep_SLPropsATool_HeaderFile
