// Created on: 1993-07-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
          if( dirline.IsNormal(norm,Precision::Angular()) ) { //A2 lies on plane of circle
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
