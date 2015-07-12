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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BlendFunc_SectionShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Convert_ParameterisationType.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Boolean.hxx>
class Adaptor3d_HSurface;
class gp_Pnt2d;
class gp_Vec;
class BlendFunc_ConstRad;
class BlendFunc_ConstRadInv;
class BlendFunc_Ruled;
class BlendFunc_RuledInv;
class BlendFunc_EvolRad;
class BlendFunc_EvolRadInv;
class BlendFunc_CSConstRad;
class BlendFunc_CSCircular;
class BlendFunc_Corde;
class BlendFunc_Chamfer;
class BlendFunc_ChamfInv;
class BlendFunc_ChAsym;
class BlendFunc_ChAsymInv;
class BlendFunc_Tensor;


//! This package provides a set of generic functions, that can
//! instantiated to compute blendings between two surfaces
//! (Constant radius, Evolutive radius, Ruled surface).
class BlendFunc 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void GetShape (const BlendFunc_SectionShape SectShape, const Standard_Real MaxAng, Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree, Convert_ParameterisationType& TypeConv);
  
  Standard_EXPORT static void Knots (const BlendFunc_SectionShape SectShape, TColStd_Array1OfReal& TKnots);
  
  Standard_EXPORT static void Mults (const BlendFunc_SectionShape SectShape, TColStd_Array1OfInteger& TMults);
  
  Standard_EXPORT static void GetMinimalWeights (const BlendFunc_SectionShape SectShape, const Convert_ParameterisationType TConv, const Standard_Real AngleMin, const Standard_Real AngleMax, TColStd_Array1OfReal& Weigths);
  
  //! Used  to obtain the next level of continuity.
  Standard_EXPORT static GeomAbs_Shape NextShape (const GeomAbs_Shape S);
  
  Standard_EXPORT static Standard_Boolean ComputeNormal (const Handle(Adaptor3d_HSurface)& Surf, const gp_Pnt2d& p2d, gp_Vec& Normal);
  
  Standard_EXPORT static Standard_Boolean ComputeDNormal (const Handle(Adaptor3d_HSurface)& Surf, const gp_Pnt2d& p2d, gp_Vec& Normal, gp_Vec& DNu, gp_Vec& DNv);




protected:





private:




friend class BlendFunc_ConstRad;
friend class BlendFunc_ConstRadInv;
friend class BlendFunc_Ruled;
friend class BlendFunc_RuledInv;
friend class BlendFunc_EvolRad;
friend class BlendFunc_EvolRadInv;
friend class BlendFunc_CSConstRad;
friend class BlendFunc_CSCircular;
friend class BlendFunc_Corde;
friend class BlendFunc_Chamfer;
friend class BlendFunc_ChamfInv;
friend class BlendFunc_ChAsym;
friend class BlendFunc_ChAsymInv;
friend class BlendFunc_Tensor;

};







#endif // _BlendFunc_HeaderFile
