// Created on: 1995-10-25
// Created by: Bruno DUMORTIER
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


#include <BRepOffset.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) BRepOffset::Surface(const Handle(Geom_Surface)& Surface,
					 const Standard_Real Offset,
					       BRepOffset_Status& Status)
{
  Standard_Real Tol = Precision::Confusion();

  Status = BRepOffset_Good;
  Handle(Geom_Surface) Result;
  
  Handle(Standard_Type) TheType = Surface->DynamicType();

  if (TheType == STANDARD_TYPE(Geom_Plane)) {
    Handle(Geom_Plane) P =
      Handle(Geom_Plane)::DownCast(Surface);
    gp_Vec T = P->Position().XDirection()^P->Position().YDirection();
    T *= Offset;
    Result = Handle(Geom_Plane)::DownCast(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface)) {
    Handle(Geom_CylindricalSurface) C =
      Handle(Geom_CylindricalSurface)::DownCast(Surface);
    Standard_Real Radius = C->Radius();
    gp_Ax3 Axis = C->Position();
    if (Axis.Direct()) 
      Radius += Offset;
    else 
      Radius -= Offset;
    if ( Radius >= Tol ) {
      Result = new Geom_CylindricalSurface( Axis, Radius);
    }
    else if ( Radius <= -Tol ){
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Result = new Geom_CylindricalSurface( Axis, Abs(Radius));
      Status = BRepOffset_Reversed;
    }
    else {
      Status = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface)) {
    Handle(Geom_ConicalSurface) C =
      Handle(Geom_ConicalSurface)::DownCast(Surface);
    Standard_Real Alpha = C->SemiAngle();
    Standard_Real Radius = C->RefRadius() + Offset * Cos(Alpha);
    gp_Ax3 Axis = C->Position();
    if ( Radius >= 0.) {
      gp_Vec Z( Axis.Direction());
      Z *= - Offset * Sin(Alpha);
      Axis.Translate(Z);
    }
    else {
      Radius = -Radius; 
      gp_Vec Z( Axis.Direction());
      Z *= - Offset * Sin(Alpha);
      Axis.Translate(Z);
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Alpha = -Alpha; 
    }
    Result = new Geom_ConicalSurface(Axis, Alpha, Radius);
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface)) {
    Handle(Geom_SphericalSurface) S = 
      Handle(Geom_SphericalSurface)::DownCast(Surface);
    Standard_Real Radius = S->Radius();
    gp_Ax3 Axis = S->Position();
    if (Axis.Direct()) 
      Radius += Offset;
    else 
      Radius -= Offset;
    if ( Radius >= Tol) {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if ( Radius <= -Tol ) {
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),M_PI);
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
      Status = BRepOffset_Reversed;
    }
    else {
      Status = BRepOffset_Degenerated;
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface)) {
    Handle(Geom_ToroidalSurface) S = 
      Handle(Geom_ToroidalSurface)::DownCast(Surface);
    Standard_Real MajorRadius = S->MajorRadius();
    Standard_Real MinorRadius = S->MinorRadius();
    gp_Ax3 Axis = S->Position();
    if (MinorRadius < MajorRadius) {  // A FINIR
      if (Axis.Direct())
	MinorRadius += Offset;
      else 
	MinorRadius -= Offset;
      if (MinorRadius >= Tol) {
	Result = new Geom_ToroidalSurface(Axis,MajorRadius,MinorRadius);
      }
      else if (MinorRadius <= -Tol) {
	Status = BRepOffset_Reversed;
      }
      else {
	Status = BRepOffset_Degenerated;
      }
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_BSplineSurface)) {
  }
  else if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) S = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(Surface);
    Standard_Real U1,U2,V1,V2;
    S->Bounds(U1,U2,V1,V2);
    Handle(Geom_Surface) Off = 
      BRepOffset::Surface(S->BasisSurface(),Offset,Status);
    Result = new Geom_RectangularTrimmedSurface (Off,U1,U2,V1,V2);
  }
  else if (TheType == STANDARD_TYPE(Geom_OffsetSurface)) {
  }

  if ( Result.IsNull()) {
    Result = new Geom_OffsetSurface( Surface, Offset);
  }
  
  return Result;
}


