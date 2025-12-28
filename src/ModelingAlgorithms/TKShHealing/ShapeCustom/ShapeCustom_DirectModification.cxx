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

//: l8 abv 15.01.99: CTS22022: writing correct pcurves for indirect tori
//: p5 abv 26.02.99: PRO18207: force copying of edge if any its pcurve is to be replaced
// szv#4 S4163
// S4181 pdn 20.04.99 Modification of indirect rectangular trimming surfaces and taking
//  locations into account
// szv 03.01.01 PositiveCones merged in

#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <ShapeCustom_DirectModification.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_DirectModification, ShapeCustom_Modification)

//=================================================================================================

ShapeCustom_DirectModification::ShapeCustom_DirectModification() = default;

// S4181 returns 0 - none, 1 - indirect, 2 - negative cone, 3 - indirect negative cone
static int IsIndirectSurface(occ::handle<Geom_Surface>& S, TopLoc_Location& L)
{
  int result = 0;

  occ::handle<Geom_Surface> TS = S;
  while (TS->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    TS = occ::down_cast<Geom_RectangularTrimmedSurface>(TS)->BasisSurface();

  occ::handle<Geom_ElementarySurface> ES = occ::down_cast<Geom_ElementarySurface>(TS);
  if (!ES.IsNull())
  {
    // is the surface indirect ?
    gp_Trsf t   = L.Transformation();
    bool    neg = t.IsNegative();
    bool    det = (t.VectorialPart().Determinant() < 0.0);
    bool    dir = ES->Position().Direct();
    if ((neg != det) == dir)
      result = 1;
    occ::handle<Geom_ConicalSurface> CS = occ::down_cast<Geom_ConicalSurface>(ES);
    if (!CS.IsNull())
    {
      // does the cone have negative semiangle ?
      if (CS->SemiAngle() < 0.0)
        result += 2;
    }
    if (result)
      S = TS;
  }

  return result;
}

//=================================================================================================

bool ShapeCustom_DirectModification::NewSurface(const TopoDS_Face&         F,
                                                occ::handle<Geom_Surface>& S,
                                                TopLoc_Location&           L,
                                                double&                    Tol,
                                                bool&                      RevWires,
                                                bool&                      RevFace)
{
  S = BRep_Tool::Surface(F, L);

  switch (IsIndirectSurface(S, L))
  {
    case 1: { // Indirect surface
      // UReverse a copy of S
      S        = S->UReversed();
      RevWires = true;
      RevFace  = true;
      break;
    }
    case 2: { // Negative cone
      // U- and VReverse a copy of S
      S = S->VReversed();
      S->UReverse();
      RevWires = false;
      RevFace  = false;
      break;
    }
    case 3: { // Indirect negative cone
      // VReverse a copy of S
      S        = S->VReversed();
      RevWires = true;
      RevFace  = true;
      break;
    }
    default:
      return false;
  }

  SendMsg(F, Message_Msg("DirectModification.NewSurface.MSG0"));

  Tol = BRep_Tool::Tolerance(F);

  return true;
}

//=================================================================================================

bool ShapeCustom_DirectModification::NewCurve(const TopoDS_Edge&       E,
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
    occ::handle<Geom_Surface> S   = GC->Surface();
    TopLoc_Location           Loc = GC->Location();
    if (!IsIndirectSurface(S, Loc))
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

bool ShapeCustom_DirectModification::NewPoint(const TopoDS_Vertex& /*V*/,
                                              gp_Pnt& /*P*/,
                                              double& /*Tol*/)
{
  // 3d points are never modified
  return false;
}

//=================================================================================================

bool ShapeCustom_DirectModification::NewCurve2d(const TopoDS_Edge&         E,
                                                const TopoDS_Face&         F,
                                                const TopoDS_Edge&         NewE,
                                                const TopoDS_Face&         NewF,
                                                occ::handle<Geom2d_Curve>& C,
                                                double&                    Tol)
{
  TopLoc_Location           L;
  occ::handle<Geom_Surface> S = BRep_Tool::Surface(F, L);

  int result = IsIndirectSurface(S, L);
  if (!result && E.IsSame(NewE))
    return false;

  double f, l;
  C   = BRep_Tool::CurveOnSurface(E, F, f, l);
  Tol = BRep_Tool::Tolerance(E);

  if (result)
  {

    gp_Trsf2d T;

    switch (result)
    {
      case 1: { // Indirect surface
        // mirror the PCurve about the V axis
        T.SetMirror(gp::OY2d());
        C = occ::down_cast<Geom2d_Curve>(C->Transformed(T));
        break;
      }
      case 2: { // Negative cone
        // mirror the PCurve about the U and V axis
        T.SetMirror(gp::OX2d());
        C = occ::down_cast<Geom2d_Curve>(C->Transformed(T));
        T.SetMirror(gp::OY2d());
        C->Transform(T);
        break;
      }
      case 3: { // Indirect negative cone
        // mirror the PCurve about the U axis
        T.SetMirror(gp::OX2d());
        C = occ::down_cast<Geom2d_Curve>(C->Transformed(T));
        break;
      }
    }

    // #26 rln When seam edge contains triangulations trimming is lost by BRep_Builder::UpdateEdge
    if (BRepTools::IsReallyClosed(E, F))
    {
      // szv#4:S4163:12Mar99 SGI warning
      TopoDS_Shape              sh  = NewE.Reversed();
      occ::handle<Geom2d_Curve> tmp = BRep_Tool::CurveOnSurface(TopoDS::Edge(sh), NewF, f, l);
      if (tmp.IsNull())
      {
        tmp = BRep_Tool::CurveOnSurface(E, F, f, l);
        BRep_Builder B;
        B.UpdateEdge(NewE, tmp, C, NewF, Tol);
        B.Range(NewE, NewF, f, l);
        // anyway, tmp will be removed later by BRepTools_Modifier
      }
    }
  }
  else
  {
    //: p5 abv 26 Feb 99: force copying of pcurves if edge was copied
    if (!C.IsNull())
      C = occ::down_cast<Geom2d_Curve>(C->Copy());
  }

  return true;
}

//=================================================================================================

bool ShapeCustom_DirectModification::NewParameter(const TopoDS_Vertex& /*V*/,
                                                  const TopoDS_Edge& /*E*/,
                                                  double& /*P*/,
                                                  double& /*Tol*/)
{
  return false;
}

//=================================================================================================

GeomAbs_Shape ShapeCustom_DirectModification::Continuity(const TopoDS_Edge& E,
                                                         const TopoDS_Face& F1,
                                                         const TopoDS_Face& F2,
                                                         const TopoDS_Edge& /*NewE*/,
                                                         const TopoDS_Face& /*NewF1*/,
                                                         const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E, F1, F2);
}
