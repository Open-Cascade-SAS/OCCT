// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _CSLib_HeaderFile
#define _CSLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <Standard_Boolean.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array2OfVec.hxx>
class gp_Vec;
class gp_Dir;


//! This package implements functions for basis geometric
//! computation on curves and surfaces.
//! The tolerance criterions used in this package are
//! Resolution from package gp and RealEpsilon from class
//! Real of package Standard.
class CSLib 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The following functions computes the normal to a surface
  //! inherits FunctionWithDerivative from math
  //!
  //! Computes the normal direction of a surface as the cross product
  //! between D1U and D1V.
  //! If D1U has null length or D1V has null length or D1U and D1V are
  //! parallel the normal is undefined.
  //! To check that D1U and D1V are colinear the sinus of the angle
  //! between D1U and D1V is computed and compared with SinTol.
  //! The normal is computed if theStatus == Done else the theStatus gives the
  //! reason why the computation has failed.
  Standard_EXPORT static void Normal (const gp_Vec& D1U, const gp_Vec& D1V, const Standard_Real SinTol, CSLib_DerivativeStatus& theStatus, gp_Dir& Normal);
  

  //! If there is a singularity on the surface  the previous method
  //! cannot compute the local normal.
  //! This method computes an approached normal direction of a surface.
  //! It does a limited development and needs the second derivatives
  //! on the surface as input data.
  //! It computes the normal as follow :
  //! N(u, v) = D1U ^ D1V
  //! N(u0+du,v0+dv) = N0 + DN/du(u0,v0) * du + DN/dv(u0,v0) * dv + Eps
  //! with Eps->0 so we can have the equivalence N ~ dN/du + dN/dv.
  //! DNu = ||DN/du|| and DNv = ||DN/dv||
  //!
  //! . if DNu IsNull (DNu <= Resolution from gp) the answer Done = True
  //! the normal direction is given by DN/dv
  //! . if DNv IsNull (DNv <= Resolution from gp) the answer Done = True
  //! the normal direction is given by DN/du
  //! . if the two directions DN/du and DN/dv are parallel Done = True
  //! the normal direction is given either by DN/du or DN/dv.
  //! To check that the two directions are colinear the sinus of the
  //! angle between these directions is computed and compared with
  //! SinTol.
  //! . if DNu/DNv or DNv/DNu is lower or equal than Real Epsilon
  //! Done = False, the normal is undefined
  //! . if DNu IsNull and DNv is Null Done = False, there is an
  //! indetermination and we should do a limited development at
  //! order 2 (it means that we cannot omit Eps).
  //! . if DNu Is not Null and DNv Is not Null Done = False, there are
  //! an infinity of normals at the considered point on the surface.
  Standard_EXPORT static void Normal (const gp_Vec& D1U, const gp_Vec& D1V, const gp_Vec& D2U, const gp_Vec& D2V, const gp_Vec& D2UV, const Standard_Real SinTol, Standard_Boolean& Done, CSLib_NormalStatus& theStatus, gp_Dir& Normal);
  

  //! Computes the normal direction of a surface as the cross product
  //! between D1U and D1V.
  Standard_EXPORT static void Normal (const gp_Vec& D1U, const gp_Vec& D1V, const Standard_Real MagTol, CSLib_NormalStatus& theStatus, gp_Dir& Normal);
  
  //! find the first  order k0  of deriviative of NUV
  //! where: foreach order < k0  all the derivatives of NUV  are
  //! null all the derivatives of NUV corresponding to the order
  //! k0 are collinear and have the same sens.
  //! In this case, normal at U,V is unique.
  Standard_EXPORT static void Normal (const Standard_Integer MaxOrder, const TColgp_Array2OfVec& DerNUV, const Standard_Real MagTol, const Standard_Real U, const Standard_Real V, const Standard_Real Umin, const Standard_Real Umax, const Standard_Real Vmin, const Standard_Real Vmax, CSLib_NormalStatus& theStatus, gp_Dir& Normal, Standard_Integer& OrderU, Standard_Integer& OrderV);
  
  //! -- Computes the derivative  of order Nu in the --
  //! direction U and Nv in the direction V of the not --
  //! normalized  normal vector at  the point  P(U,V) The
  //! array DerSurf contain the derivative (i,j) of the surface
  //! for i=0,Nu+1 ; j=0,Nv+1
  Standard_EXPORT static gp_Vec DNNUV (const Standard_Integer Nu, const Standard_Integer Nv, const TColgp_Array2OfVec& DerSurf);
  
  //! Computes the derivatives of order Nu in the direction Nu
  //! and Nv in the direction Nv of the not normalized vector
  //! N(u,v) = dS1/du * dS2/dv (cases where we use an osculating surface)
  //! DerSurf1 are the derivatives of S1
  Standard_EXPORT static gp_Vec DNNUV (const Standard_Integer Nu, const Standard_Integer Nv, const TColgp_Array2OfVec& DerSurf1, const TColgp_Array2OfVec& DerSurf2);
  
  //! -- Computes the derivative  of order Nu in the --
  //! direction   U and  Nv in the  direction  V  of the
  //! normalized normal vector  at the point P(U,V) array
  //! DerNUV contain the  derivative  (i+Iduref,j+Idvref)
  //! of D1U ^ D1V for i=0,Nu  ; j=0,Nv Iduref and Idvref
  //! correspond to a derivative  of D1U ^ D1V  which can
  //! be used to compute the normalized normal vector.
  //! In the regular cases , Iduref=Idvref=0.
  Standard_EXPORT static gp_Vec DNNormal (const Standard_Integer Nu, const Standard_Integer Nv, const TColgp_Array2OfVec& DerNUV, const Standard_Integer Iduref = 0, const Standard_Integer Idvref = 0);




protected:





private:




friend class CSLib_Class2d;
friend class CSLib_NormalPolyDef;

};







#endif // _CSLib_HeaderFile
