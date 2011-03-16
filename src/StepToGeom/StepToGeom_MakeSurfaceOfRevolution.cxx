// File:	StepToGeom_MakeSurfaceOfRevolution.cxx
// Created:	Mon Jul  5 10:37:59 1993
// Author:	Martine LANGLOIS

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
            C = new Geom_TrimmedCurve(conic, 0., PI);
          }
        }
      }
      CS = new Geom_SurfaceOfRevolution(C, A);
      return Standard_True;
    }
  }
  return Standard_False;
}
