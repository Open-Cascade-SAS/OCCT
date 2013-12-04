// Created on: 1993-07-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StepToGeom_MakeSurfaceOfRevolution.ixx>
#include <StepGeom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_Axis1Placement.hxx>
#include <StepToGeom_MakeAxis1Placement.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>

//=============================================================================
// Creation d' une SurfaceOfRevolution de Geom a partir d' une
// SurfaceOfRevolution de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSurfaceOfRevolution::Convert (const Handle(StepGeom_SurfaceOfRevolution)& SS, Handle(Geom_SurfaceOfRevolution)& CS)
{
  Handle(Geom_Curve) C;
  if (StepToGeom_MakeCurve::Convert(SS->SweptCurve(),C))
  {
    Handle(Geom_Axis1Placement) A1;
    if (StepToGeom_MakeAxis1Placement::Convert(SS->AxisPosition(),A1))
    {
      const gp_Ax1 A( A1->Ax1() );
      //skl for OCC952 (one bad case revolution of circle)
      if ( C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)) )
      {
        const Handle(Geom_Conic) conic = Handle(Geom_Conic)::DownCast(C);
        const gp_Pnt pc = conic->Location();
        const gp_Lin rl (A);
        if (rl.Distance(pc) < Precision::Confusion()) { //pc lies on A2
          const gp_Dir dirline = A.Direction();
          const gp_Dir norm = conic->Axis().Direction();
          const gp_Dir xAxis = conic->XAxis().Direction();
          //checking A2 lies on plane of circle
          if( dirline.IsNormal(norm,Precision::Angular()) && (dirline.IsParallel(xAxis,Precision::Angular()) || C->IsKind(STANDARD_TYPE(Geom_Circle)))) { 
            //change parametrization for trimming
            gp_Ax2 axnew(pc,norm,dirline.Reversed());
            conic->SetPosition(axnew);
            C = new Geom_TrimmedCurve(conic, 0., M_PI);
          }
        }
      }
      CS = new Geom_SurfaceOfRevolution(C, A);
      return Standard_True;
    }
  }
  return Standard_False;
}
