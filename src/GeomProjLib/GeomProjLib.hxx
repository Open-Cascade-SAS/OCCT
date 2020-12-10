// Created on: 1993-08-25
// Created by: Bruno DUMORTIER
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

#ifndef _GeomProjLib_HeaderFile
#define _GeomProjLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom2d_Curve;
class Geom_Curve;
class Geom_Surface;
class Geom_Plane;
class gp_Dir;


//! Projection of a curve on a surface.
class GeomProjLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! gives  the 2d-curve   of  a 3d-curve  lying on   a
  //! surface (  uses GeomProjLib_ProjectedCurve   )
  //! The 3dCurve is taken between the parametrization
  //! range [First, Last]
  //! <Tolerance> is used as input if the projection needs
  //! an approximation. In this case, the reached
  //! tolerance is set in <Tolerance> as output.
  //! WARNING :  if   the projection has  failed,   this
  //! method returns a null Handle.
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Standard_Real First, const Standard_Real Last, const Handle(Geom_Surface)& S, const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real VFirst, const Standard_Real VLast, Standard_Real& Tolerance);
  
  //! gives  the 2d-curve   of  a 3d-curve  lying on   a
  //! surface (  uses GeomProjLib_ProjectedCurve   )
  //! The 3dCurve is taken between the parametrization
  //! range [First, Last]
  //! <Tolerance> is used as input if the projection needs
  //! an approximation. In this case, the reached
  //! tolerance is set in <Tolerance> as output.
  //! WARNING :  if   the projection has  failed,   this
  //! method returns a null Handle.
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Standard_Real First, const Standard_Real Last, const Handle(Geom_Surface)& S, Standard_Real& Tolerance);
  
  //! gives  the 2d-curve   of  a 3d-curve  lying on   a
  //! surface (  uses GeomProjLib_ProjectedCurve   )
  //! The 3dCurve is taken between the parametrization
  //! range [First, Last]
  //! If the projection needs an approximation,
  //! Precision::PApproximation() is used.
  //! WARNING :  if   the projection has  failed,   this
  //! method returns a null Handle.
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Standard_Real First, const Standard_Real Last, const Handle(Geom_Surface)& S);
  
  //! gives  the  2d-curve  of  a  3d-curve lying   on a
  //! surface   ( uses   GeomProjLib_ProjectedCurve ).
  //! If the projection needs an approximation,
  //! Precision::PApproximation() is used.
  //! WARNING  :  if the   projection has  failed,  this
  //! method returns a null Handle.
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S);
  
  //! gives  the  2d-curve  of  a  3d-curve lying   on a
  //! surface   ( uses   GeomProjLib_ProjectedCurve ).
  //! If the projection needs an approximation,
  //! Precision::PApproximation() is used.
  //! WARNING  :  if the   projection has  failed,  this
  //! method returns a null Handle.
  //! can expand a little the bounds of surface
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S, const Standard_Real UDeb, const Standard_Real UFin, const Standard_Real VDeb, const Standard_Real VFin);
  
  //! gives  the  2d-curve  of  a  3d-curve lying   on a
  //! surface   ( uses   GeomProjLib_ProjectedCurve ).
  //! If the projection needs an approximation,
  //! Precision::PApproximation() is used.
  //! WARNING  :  if the   projection has  failed,  this
  //! method returns a null Handle.
  //! can expand a little the bounds of surface
  Standard_EXPORT static Handle(Geom2d_Curve) Curve2d (const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S, const Standard_Real UDeb, const Standard_Real UFin, const Standard_Real VDeb, const Standard_Real VFin, Standard_Real& Tolerance);
  
  //! Constructs   the  3d-curve  from the normal
  //! projection  of the  Curve <C> on  the surface <S>.
  //! WARNING : if the  projection has failed, returns  a
  //! null Handle.
  Standard_EXPORT static Handle(Geom_Curve) Project (const Handle(Geom_Curve)& C, const Handle(Geom_Surface)& S);
  
  //! Constructs  the 3d-curves from the projection
  //! of the  curve  <Curve> on the  plane <Plane> along
  //! the direction <Dir>.
  //! If <KeepParametrization> is true, the parametrization
  //! of the Projected Curve <PC> will be  the same as  the
  //! parametrization of the initial curve <C>.
  //! It means: proj(C(u)) = PC(u) for each u.
  //! Otherwise, the parametrization may change.
  Standard_EXPORT static Handle(Geom_Curve) ProjectOnPlane (const Handle(Geom_Curve)& Curve, const Handle(Geom_Plane)& Plane, const gp_Dir& Dir, const Standard_Boolean KeepParametrization);




protected:





private:





};







#endif // _GeomProjLib_HeaderFile
