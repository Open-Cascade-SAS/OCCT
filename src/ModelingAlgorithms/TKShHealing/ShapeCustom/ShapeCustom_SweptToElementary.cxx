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

#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_SurfaceOfRevolution.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeCustom_SweptToElementary.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_SweptToElementary, ShapeCustom_Modification)

//=================================================================================================

ShapeCustom_SweptToElementary::ShapeCustom_SweptToElementary() = default;

//=======================================================================
// function : IsToConvert
// purpose  : auxiliary (Analyze surface: is it to be converted?)
//=======================================================================
static bool IsToConvert(const occ::handle<Geom_Surface>& S, occ::handle<Geom_SweptSurface>& SS)
{
  occ::handle<Geom_Surface> Stmp;

  if (S->IsKind(STANDARD_TYPE(Geom_SweptSurface)))
  {
    SS = occ::down_cast<Geom_SweptSurface>(S);
    return true;
  }
  if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> RTS =
      occ::down_cast<Geom_RectangularTrimmedSurface>(S);
    Stmp = RTS->BasisSurface();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_OffsetSurface> OS = occ::down_cast<Geom_OffsetSurface>(S);
    Stmp                               = OS->BasisSurface();
  }
  if (Stmp.IsNull())
    return false;
  if (S->IsKind(STANDARD_TYPE(Geom_SweptSurface)))
  {
    SS = occ::down_cast<Geom_SweptSurface>(Stmp);
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeCustom_SweptToElementary::NewSurface(const TopoDS_Face&         F,
                                               occ::handle<Geom_Surface>& S,
                                               TopLoc_Location&           L,
                                               double&                    Tol,
                                               bool&                      RevWires,
                                               bool&                      RevFace)
{
  S = BRep_Tool::Surface(F, L);
  occ::handle<Geom_SweptSurface> SS;
  if (!IsToConvert(S, SS))
    return false;

  // case SurfaceOfRevolution
  if (SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> SR  = occ::down_cast<Geom_SurfaceOfRevolution>(SS);
    occ::handle<Geom_Curve>               bc  = SR->BasisCurve();
    gp_Ax1                                ax1 = SR->Axis();
    occ::handle<GeomAdaptor_Curve>        HC  = new GeomAdaptor_Curve();
    HC->Load(bc, bc->FirstParameter(), bc->LastParameter());
    GeomAdaptor_SurfaceOfRevolution AS(HC, ax1);
    switch (AS.GetType())
    {
      // skl 18.12.2003 - plane not used, problems in PRO14665.igs
      // case GeomAbs_Plane : {
      //  occ::handle<Geom_Plane> Pl = new Geom_Plane(AS.Plane());
      //  S = Pl;
      //} break;
      case GeomAbs_Cylinder: {
        occ::handle<Geom_CylindricalSurface> Cy = new Geom_CylindricalSurface(AS.Cylinder());
        S                                       = Cy;
      }
      break;
      case GeomAbs_Sphere: {
        occ::handle<Geom_SphericalSurface> Sp = new Geom_SphericalSurface(AS.Sphere());
        S                                     = Sp;
      }
      break;
      case GeomAbs_Cone: {
        occ::handle<Geom_ConicalSurface> Co = new Geom_ConicalSurface(AS.Cone());
        S                                   = Co;
      }
      break;
      case GeomAbs_Torus: {
        occ::handle<Geom_ToroidalSurface> To = new Geom_ToroidalSurface(AS.Torus());
        S                                    = To;
      }
      break;
      default:
        return false;
        break;
    }
  }
  // case SurfaceOfLinearExtrusion
  else if (SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> SLE =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(SS);
    occ::handle<Geom_Curve>        bc  = SLE->BasisCurve();
    gp_Dir                         dir = SLE->Direction();
    occ::handle<GeomAdaptor_Curve> HC  = new GeomAdaptor_Curve();
    HC->Load(bc, bc->FirstParameter(), bc->LastParameter());
    GeomAdaptor_SurfaceOfLinearExtrusion AS(HC, dir);
    switch (AS.GetType())
    {
      // skl 18.12.2003 - plane not used, problems in ims013.igs
      // case GeomAbs_Plane : {
      //  occ::handle<Geom_Plane> Pl = new Geom_Plane(AS.Plane());
      //  S = Pl;
      //} break;
      case GeomAbs_Cylinder: {
        occ::handle<Geom_CylindricalSurface> Cy = new Geom_CylindricalSurface(AS.Cylinder());
        S                                       = Cy;
      }
      break;
      default:
        return false;
        break;
    }
  }

  SendMsg(F, Message_Msg("SweptToElementary.NewSurface.MSG0"));

  Tol      = BRep_Tool::Tolerance(F);
  RevWires = false;
  RevFace  = false;
  return true;
}

//=================================================================================================

bool ShapeCustom_SweptToElementary::NewCurve(const TopoDS_Edge&       E,
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
    occ::handle<Geom_Surface>      S = GC->Surface();
    occ::handle<Geom_SweptSurface> SS;
    if (!IsToConvert(S, SS))
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

bool ShapeCustom_SweptToElementary::NewPoint(const TopoDS_Vertex& /*V*/,
                                             gp_Pnt& /*P*/,
                                             double& /*Tol*/)
{
  // 3d points are never modified
  return false;
}

//=================================================================================================

bool ShapeCustom_SweptToElementary::NewCurve2d(const TopoDS_Edge&         E,
                                               const TopoDS_Face&         F,
                                               const TopoDS_Edge&         NewE,
                                               const TopoDS_Face&         NewF,
                                               occ::handle<Geom2d_Curve>& C,
                                               double&                    Tol)
{
  TopLoc_Location                L;
  occ::handle<Geom_Surface>      S = BRep_Tool::Surface(F, L);
  occ::handle<Geom_SweptSurface> SS;

  // just copy pcurve if either its surface is changing or edge was copied
  if (!IsToConvert(S, SS) && E.IsSame(NewE))
    return false;

  double f, l;
  C = BRep_Tool::CurveOnSurface(E, F, f, l);
  if (!C.IsNull())
  {
    C                            = occ::down_cast<Geom2d_Curve>(C->Copy());
    occ::handle<Geom_Surface> NS = BRep_Tool::Surface(NewF, L);
    if (!NS.IsNull() && NS->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    {
      if (SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
      {
        occ::handle<Geom_SurfaceOfRevolution> SR = occ::down_cast<Geom_SurfaceOfRevolution>(SS);
        double                                U1, U2, V1, V2;
        SR->Bounds(U1, U2, V1, V2);
        gp_Pnt P0;
        SR->D0(U1, V1, P0);
        occ::handle<ShapeAnalysis_Surface> sas = new ShapeAnalysis_Surface(NS);
        gp_Pnt2d                           p2d = sas->ValueOfUV(P0, Precision::Confusion());
        gp_Vec2d                           shift(p2d.X() - U1, p2d.Y() - V1);
        C->Translate(shift);
      }
    }
    if (!NS.IsNull() && NS->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    {
      if (SS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
      {
        occ::handle<Geom_SurfaceOfRevolution> SR = occ::down_cast<Geom_SurfaceOfRevolution>(SS);
        gp_Pnt                                PR, PS;
        occ::handle<Geom_SphericalSurface>    SPH = occ::down_cast<Geom_SphericalSurface>(NS);
        double                                US1, US2, VS1, VS2;
        SPH->Bounds(US1, US2, VS1, VS2);
        SPH->D0(US1, VS1, PS);
        double UR1, UR2, VR1, VR2;
        SR->Bounds(UR1, UR2, VR1, VR2);
        SR->D0(UR1, VR1, PR);
        gp_Pnt   P0 = SPH->Location();
        gp_Vec   VS(P0, PS);
        gp_Vec   VR(P0, PR);
        double   angle = VS.Angle(VR);
        gp_Vec2d shift(0, VS1 - VR1 + angle);
        C->Translate(shift);
      }
    }
  }

  Tol = BRep_Tool::Tolerance(E);
  return true;
}

//=================================================================================================

bool ShapeCustom_SweptToElementary::NewParameter(const TopoDS_Vertex& /*V*/,
                                                 const TopoDS_Edge& /*E*/,
                                                 double& /*P*/,
                                                 double& /*Tol*/)
{
  return false;
}

//=================================================================================================

GeomAbs_Shape ShapeCustom_SweptToElementary::Continuity(const TopoDS_Edge& E,
                                                        const TopoDS_Face& F1,
                                                        const TopoDS_Face& F2,
                                                        const TopoDS_Edge& /*NewE*/,
                                                        const TopoDS_Face& /*NewF1*/,
                                                        const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E, F1, F2);
}
