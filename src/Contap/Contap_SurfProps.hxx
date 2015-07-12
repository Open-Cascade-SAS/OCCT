// Created on: 1995-02-24
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Contap_SurfProps_HeaderFile
#define _Contap_SurfProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Adaptor3d_HSurface;
class gp_Pnt;
class gp_Vec;


//! Internal tool used  to compute the  normal and its
//! derivatives.
class Contap_SurfProps 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Computes  the point <P>, and  normal vector <N> on
  //! <S> at parameters U,V.
  Standard_EXPORT static void Normale (const Handle(Adaptor3d_HSurface)& S, const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& N);
  
  //! Computes  the point <P>, and  normal vector <N> on
  //! <S> at parameters U,V.
  Standard_EXPORT static void DerivAndNorm (const Handle(Adaptor3d_HSurface)& S, const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& d1u, gp_Vec& d1v, gp_Vec& N);
  
  //! Computes the point <P>, normal vector <N>, and its
  //! derivatives <Dnu> and <Dnv> on <S> at parameters U,V.
  Standard_EXPORT static void NormAndDn (const Handle(Adaptor3d_HSurface)& S, const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& N, gp_Vec& Dnu, gp_Vec& Dnv);




protected:





private:





};







#endif // _Contap_SurfProps_HeaderFile
