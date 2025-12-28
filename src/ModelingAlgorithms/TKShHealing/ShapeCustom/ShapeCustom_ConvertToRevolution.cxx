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

#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <ShapeCustom_ConvertToRevolution.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_ConvertToRevolution, ShapeCustom_Modification)

//=================================================================================================

ShapeCustom_ConvertToRevolution::ShapeCustom_ConvertToRevolution() = default;

// Analyze surface: is it to be converted?
static bool IsToConvert(const occ::handle<Geom_Surface>& S, occ::handle<Geom_ElementarySurface>& ES)
{
  ES = occ::down_cast<Geom_ElementarySurface>(S);
  if (ES.IsNull())
  {
    if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      occ::handle<Geom_RectangularTrimmedSurface> RTS =
        occ::down_cast<Geom_RectangularTrimmedSurface>(S);
      ES = occ::down_cast<Geom_ElementarySurface>(RTS->BasisSurface());
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      occ::handle<Geom_OffsetSurface> OS = occ::down_cast<Geom_OffsetSurface>(S);
      ES = occ::down_cast<Geom_ElementarySurface>(OS->BasisSurface());
    }
    if (ES.IsNull())
      return false;
  }

  return ES->IsKind(STANDARD_TYPE(Geom_SphericalSurface))
         || ES->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))
         || ES->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
         || ES->IsKind(STANDARD_TYPE(Geom_ConicalSurface));
}

//=================================================================================================

bool ShapeCustom_ConvertToRevolution::NewSurface(const TopoDS_Face&         F,
                                                 occ::handle<Geom_Surface>& S,
                                                 TopLoc_Location&           L,
                                                 double&                    Tol,
                                                 bool&                      RevWires,
                                                 bool&                      RevFace)
{
  S = BRep_Tool::Surface(F, L);

  occ::handle<Geom_ElementarySurface> ES;
  if (!IsToConvert(S, ES))
    return false;

  // remove location if it contains inversion
  /*
    gp_Trsf t = L.Transformation();
    gp_Mat m = t.VectorialPart();
    bool neg = t.IsNegative();
    bool det = ( m.Determinant() <0 ? true : false );
    if ( neg != det ) {
      ES = Handle(Geom_ElementarySurface)::DownCast ( ES->Transformed(t) );
      L.Identity();
    }
  */

  gp_Ax3 Ax3 = ES->Position();
  gp_Pnt pos = Ax3.Location();
  gp_Dir dir = Ax3.Direction();
  gp_Dir X   = Ax3.XDirection();

  // create basis line to rotate
  occ::handle<Geom_Curve> BasisCurve;
  if (ES->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    occ::handle<Geom_SphericalSurface> SS = occ::down_cast<Geom_SphericalSurface>(ES);
    gp_Ax2                             Ax2(pos, X ^ dir, X);
    occ::handle<Geom_Circle>           Circ = new Geom_Circle(Ax2, SS->Radius());
    BasisCurve                              = new Geom_TrimmedCurve(Circ, -M_PI / 2., M_PI / 2.);
  }
  else if (ES->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
  {
    occ::handle<Geom_ToroidalSurface> TS = occ::down_cast<Geom_ToroidalSurface>(ES);
    gp_Ax2                            Ax2(pos.XYZ() + X.XYZ() * TS->MajorRadius(), X ^ dir, X);
    BasisCurve = new Geom_Circle(Ax2, TS->MinorRadius());
  }
  else if (ES->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    occ::handle<Geom_CylindricalSurface> CS = occ::down_cast<Geom_CylindricalSurface>(ES);
    gp_Ax1                               Ax1(pos.XYZ() + X.XYZ() * CS->Radius(), dir);
    BasisCurve = new Geom_Line(Ax1);
  }
  else if (ES->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> CS = occ::down_cast<Geom_ConicalSurface>(ES);
    gp_Dir                           N  = dir.XYZ() + X.XYZ() * std::tan(CS->SemiAngle());
    gp_Ax1                           Ax1(pos.XYZ() + X.XYZ() * CS->RefRadius(), N);
    BasisCurve = new Geom_Line(Ax1);
  }

  // create revolution with proper U parametrization
  gp_Ax1 Axis = Ax3.Axis();

  // if the surface is indirect (taking into account locations), reverse dir

  /*
    gp_Trsf t = L.Transformation();
    gp_Mat m = t.VectorialPart();
    bool neg = t.IsNegative();
    bool det = ( m.Determinant() <0 ? true : false );
    bool isdir = Ax3.Direct();
    if ( ( neg != det ) == isdir ) Axis.Reverse();
  */
  if (!Ax3.Direct())
    Axis.Reverse();

  occ::handle<Geom_SurfaceOfRevolution> Rev = new Geom_SurfaceOfRevolution(BasisCurve, Axis);

  // set resulting surface and restore trimming or offsetting if necessary
  if (ES == S)
    S = Rev;
  else
  {
    if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      occ::handle<Geom_RectangularTrimmedSurface> RTS =
        occ::down_cast<Geom_RectangularTrimmedSurface>(S);
      double U1, U2, V1, V2;
      RTS->Bounds(U1, U2, V1, V2);
      S = new Geom_RectangularTrimmedSurface(Rev, U1, U2, V1, V2);
    }
    else if (S->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      occ::handle<Geom_OffsetSurface> OS = occ::down_cast<Geom_OffsetSurface>(S);
      S                                  = new Geom_OffsetSurface(Rev, OS->Offset());
    }
    else
      S = Rev;
  }
  SendMsg(F, Message_Msg("ConvertToRevolution.NewSurface.MSG0"));

  Tol      = BRep_Tool::Tolerance(F);
  RevWires = false;
  RevFace  = false;
  return true;
}

//=================================================================================================

bool ShapeCustom_ConvertToRevolution::NewCurve(const TopoDS_Edge&       E,
                                               occ::handle<Geom_Curve>& C,
                                               TopLoc_Location&         L,
                                               double&                  Tol)
{
  //: p5 abv 26 Feb 99: force copying of edge if any its pcurve will be replaced
  occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());

  // iterate on pcurves
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
  for (; itcr.More(); itcr.Next())
  {
    occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itcr.Value());
    if (GC.IsNull() || !GC->IsCurveOnSurface())
      continue;
    occ::handle<Geom_Surface>           S = GC->Surface();
    occ::handle<Geom_ElementarySurface> ES;
    if (!IsToConvert(S, ES))
      continue;
    double f, l;
    C = BRep_Tool::Curve(E, L, f, l);
    if (!C.IsNull())
      C = occ::down_cast<Geom_Curve>(C->Copy());
    Tol = BRep_Tool::Tolerance(E);
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeCustom_ConvertToRevolution::NewPoint(const TopoDS_Vertex& /*V*/,
                                               gp_Pnt& /*P*/,
                                               double& /*Tol*/)
{
  // 3d points are never modified
  return false;
}

//=================================================================================================

bool ShapeCustom_ConvertToRevolution::NewCurve2d(const TopoDS_Edge& E,
                                                 const TopoDS_Face& F,
                                                 const TopoDS_Edge& NewE,
                                                 const TopoDS_Face& /*NewF*/,
                                                 occ::handle<Geom2d_Curve>& C,
                                                 double&                    Tol)
{
  TopLoc_Location                     L;
  occ::handle<Geom_Surface>           S = BRep_Tool::Surface(F, L);
  occ::handle<Geom_ElementarySurface> ES;

  // just copy pcurve if either its surface is changing or edge was copied
  if (!IsToConvert(S, ES) && E.IsSame(NewE))
    return false;

  double f, l;
  C = BRep_Tool::CurveOnSurface(E, F, f, l);
  if (!C.IsNull())
  {
    C = occ::down_cast<Geom2d_Curve>(C->Copy());

    // for spherical surface, surface of revolution since based on TrimmedCurve
    // has V parametrisation shifted on 2PI; translate pcurve accordingly
    if (!ES.IsNull() && ES->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    {
      gp_Vec2d shift(0., 2 * M_PI);
      C->Translate(shift);
    }
  }

  Tol = BRep_Tool::Tolerance(E);
  return true;
}

//=================================================================================================

bool ShapeCustom_ConvertToRevolution::NewParameter(const TopoDS_Vertex& /*V*/,
                                                   const TopoDS_Edge& /*E*/,
                                                   double& /*P*/,
                                                   double& /*Tol*/)
{
  return false;
}

//=================================================================================================

GeomAbs_Shape ShapeCustom_ConvertToRevolution::Continuity(const TopoDS_Edge& E,
                                                          const TopoDS_Face& F1,
                                                          const TopoDS_Face& F2,
                                                          const TopoDS_Edge& /*NewE*/,
                                                          const TopoDS_Face& /*NewF1*/,
                                                          const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E, F1, F2);
}
