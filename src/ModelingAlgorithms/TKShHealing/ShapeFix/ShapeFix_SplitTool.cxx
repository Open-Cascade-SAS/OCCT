// Created on: 2004-07-14
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_SplitTool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================

ShapeFix_SplitTool::ShapeFix_SplitTool() {}

//=================================================================================================

bool ShapeFix_SplitTool::SplitEdge(const TopoDS_Edge&   edge,
                                   const double         param,
                                   const TopoDS_Vertex& vert,
                                   const TopoDS_Face&   face,
                                   TopoDS_Edge&         newE1,
                                   TopoDS_Edge&         newE2,
                                   const double         tol3d,
                                   const double         tol2d) const
{
  double                    a, b;
  ShapeAnalysis_Edge        sae;
  occ::handle<Geom2d_Curve> c2d;
  sae.PCurve(edge, face, c2d, a, b, true);
  if (std::abs(a - param) < tol2d || std::abs(b - param) < tol2d)
    return false;
  // check distance between edge and new vertex
  gp_Pnt          P1;
  TopLoc_Location L;
  if (BRep_Tool::SameParameter(edge))
  {
    double                        f, l;
    const occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(edge, L, f, l);
    if (c3d.IsNull())
      return false;
    P1 = c3d->Value(param);
    if (!L.IsIdentity())
      P1 = P1.Transformed(L.Transformation());
  }
  else
  {
    occ::handle<Geom_Surface>          surf = BRep_Tool::Surface(face, L);
    occ::handle<ShapeAnalysis_Surface> sas  = new ShapeAnalysis_Surface(surf);
    P1                                      = sas->Value(c2d->Value(param));
    if (!L.IsIdentity())
      P1 = P1.Transformed(L.Transformation());
  }
  gp_Pnt P2 = BRep_Tool::Pnt(vert);
  if (P1.Distance(P2) > tol3d)
  {
    // return false;
    BRep_Builder B;
    B.UpdateVertex(vert, P1.Distance(P2));
  }

  occ::handle<ShapeAnalysis_TransferParametersProj> transferParameters =
    new ShapeAnalysis_TransferParametersProj;
  transferParameters->SetMaxTolerance(tol3d);
  transferParameters->Init(edge, face);
  double first, last;
  if (a < b)
  {
    first = a;
    last  = b;
  }
  else
  {
    first = b;
    last  = a;
  }

  ShapeBuild_Edge            sbe;
  occ::handle<ShapeFix_Edge> sfe    = new ShapeFix_Edge;
  TopAbs_Orientation         orient = edge.Orientation();
  BRep_Builder               B;
  TopoDS_Edge                wE = edge;
  wE.Orientation(TopAbs_FORWARD);
  TopoDS_Shape aTmpShape = vert.Oriented(TopAbs_REVERSED); // for porting
  newE1 = sbe.CopyReplaceVertices(wE, sae.FirstVertex(wE), TopoDS::Vertex(aTmpShape));
  sbe.CopyPCurves(newE1, wE);
  transferParameters->TransferRange(newE1, first, param, true);
  B.SameRange(newE1, false);
  sfe->FixSameParameter(newE1);
  // B.SameParameter(newE1,false);
  aTmpShape = vert.Oriented(TopAbs_FORWARD);
  newE2     = sbe.CopyReplaceVertices(wE, TopoDS::Vertex(aTmpShape), sae.LastVertex(wE));
  sbe.CopyPCurves(newE2, wE);
  transferParameters->TransferRange(newE2, param, last, true);
  B.SameRange(newE2, false);
  sfe->FixSameParameter(newE2);
  // B.SameParameter(newE2,false);

  newE1.Orientation(orient);
  newE2.Orientation(orient);
  if (orient == TopAbs_REVERSED)
  {
    TopoDS_Edge tmp = newE2;
    newE2           = newE1;
    newE1           = tmp;
  }

  return true;
}

//=================================================================================================

bool ShapeFix_SplitTool::SplitEdge(const TopoDS_Edge&   edge,
                                   const double         param1,
                                   const double         param2,
                                   const TopoDS_Vertex& vert,
                                   const TopoDS_Face&   face,
                                   TopoDS_Edge&         newE1,
                                   TopoDS_Edge&         newE2,
                                   const double         tol3d,
                                   const double         tol2d) const
{
  double param = (param1 + param2) / 2;
  if (SplitEdge(edge, param, vert, face, newE1, newE2, tol3d, tol2d))
  {
    // cut new edges by param1 and param2
    bool                      IsCutLine;
    occ::handle<Geom2d_Curve> Crv1, Crv2;
    double                    fp1, lp1, fp2, lp2;
    ShapeAnalysis_Edge        sae;
    if (sae.PCurve(newE1, face, Crv1, fp1, lp1, false))
    {
      if (sae.PCurve(newE2, face, Crv2, fp2, lp2, false))
      {
        if (lp1 == param)
        {
          if ((lp1 - fp1) * (lp1 - param1) > 0)
          {
            CutEdge(newE1, fp1, param1, face, IsCutLine);
            CutEdge(newE2, lp2, param2, face, IsCutLine);
          }
          else
          {
            CutEdge(newE1, fp1, param2, face, IsCutLine);
            CutEdge(newE2, lp2, param1, face, IsCutLine);
          }
        }
        else
        {
          if ((fp1 - lp1) * (fp1 - param1) > 0)
          {
            CutEdge(newE1, lp1, param1, face, IsCutLine);
            CutEdge(newE2, fp2, param2, face, IsCutLine);
          }
          else
          {
            CutEdge(newE1, lp1, param2, face, IsCutLine);
            CutEdge(newE2, fp2, param1, face, IsCutLine);
          }
        }
      }
    }
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeFix_SplitTool::CutEdge(const TopoDS_Edge& edge,
                                 const double       pend,
                                 const double       cut,
                                 const TopoDS_Face& face,
                                 bool&              iscutline) const
{
  if (std::abs(cut - pend) < 10. * Precision::PConfusion())
    return false;
  double aRange = std::abs(cut - pend);
  double a, b;
  BRep_Tool::Range(edge, a, b);
  iscutline = false;
  if (aRange < 10. * Precision::PConfusion())
    return false;

  // case pcurve is trimm of line
  if (!BRep_Tool::SameParameter(edge))
  {
    ShapeAnalysis_Edge        sae;
    occ::handle<Geom2d_Curve> Crv;
    double                    fp, lp;
    if (sae.PCurve(edge, face, Crv, fp, lp, false))
    {
      if (Crv->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
      {
        occ::handle<Geom2d_TrimmedCurve> tc = occ::down_cast<Geom2d_TrimmedCurve>(Crv);
        if (tc->BasisCurve()->IsKind(STANDARD_TYPE(Geom2d_Line)))
        {
          BRep_Builder B;
          B.Range(edge, std::min(pend, cut), std::max(pend, cut));
          if (std::abs(pend - lp) < Precision::PConfusion())
          { // cut from the beginning
            double cut3d = (cut - fp) * (b - a) / (lp - fp);
            if (cut3d <= Precision::PConfusion())
              return false;
            B.Range(edge, a + cut3d, b, true);
            iscutline = true;
          }
          else if (std::abs(pend - fp) < Precision::PConfusion())
          { // cut from the end
            double cut3d = (lp - cut) * (b - a) / (lp - fp);
            if (cut3d <= Precision::PConfusion())
              return false;
            B.Range(edge, a, b - cut3d, true);
            iscutline = true;
          }
        }
      }
    }
    return true;
  }

  // det-study on 03/12/01 checking the old and new ranges
  if (std::abs(std::abs(a - b) - aRange) < Precision::PConfusion())
    return false;
  if (aRange < 10. * Precision::PConfusion())
    return false;

  occ::handle<Geom_Curve> c = BRep_Tool::Curve(edge, a, b);
  ShapeAnalysis_Curve     sac;
  a         = std::min(pend, cut);
  b         = std::max(pend, cut);
  double na = a, nb = b;

  BRep_Builder B;
  if (!BRep_Tool::Degenerated(edge) && !c.IsNull()
      && sac.ValidateRange(c, na, nb, Precision::PConfusion()) && (na != a || nb != b))
  {
    B.Range(edge, na, nb, true);
    ShapeAnalysis_Edge sae;
    if (sae.HasPCurve(edge, face))
    {
      B.SameRange(edge, false);
    }

    ShapeFix_Edge sfe;
    sfe.FixSameParameter(edge);
  }
  else
  {
    B.Range(edge, a, b, false);
  }

  return true;
}

//=================================================================================================

bool ShapeFix_SplitTool::SplitEdge(const TopoDS_Edge&                     edge,
                                   const double                           fp,
                                   const TopoDS_Vertex&                   V1,
                                   const double                           lp,
                                   const TopoDS_Vertex&                   V2,
                                   const TopoDS_Face&                     face,
                                   NCollection_Sequence<TopoDS_Shape>&    SeqE,
                                   int&                                   aNum,
                                   const occ::handle<ShapeBuild_ReShape>& context,
                                   const double                           tol3d,
                                   const double                           tol2d) const
{
  if (fabs(lp - fp) < tol2d)
    return false;
  aNum = 0;
  SeqE.Clear();
  BRep_Builder              B;
  double                    a, b;
  ShapeAnalysis_Edge        sae;
  occ::handle<Geom2d_Curve> c2d;
  sae.PCurve(edge, face, c2d, a, b, true);
  TopoDS_Vertex VF    = sae.FirstVertex(edge);
  TopoDS_Vertex VL    = sae.LastVertex(edge);
  double        tolVF = BRep_Tool::Tolerance(VF);
  double        tolVL = BRep_Tool::Tolerance(VL);
  double        tolV1 = BRep_Tool::Tolerance(V1);
  double        tolV2 = BRep_Tool::Tolerance(V2);
  gp_Pnt        PVF   = BRep_Tool::Pnt(VF);
  gp_Pnt        PVL   = BRep_Tool::Pnt(VL);
  gp_Pnt        PV1   = BRep_Tool::Pnt(V1);
  gp_Pnt        PV2   = BRep_Tool::Pnt(V2);

  double par1, par2;
  bool   IsReverse = false;
  if ((b - a) * (lp - fp) > 0)
  {
    par1 = fp;
    par2 = lp;
  }
  else
  {
    par1      = lp;
    par2      = fp;
    IsReverse = true;
  }

  if (fabs(a - par1) <= tol2d && fabs(b - par2) <= tol2d)
  {
    if (IsReverse)
    {
      double newtol = tolVF + PVF.Distance(PV2);
      if (tolV2 < newtol)
        B.UpdateVertex(V2, newtol);
      if (VF.Orientation() == V2.Orientation())
      {
        context->Replace(VF, V2);
        VF = V2;
      }
      else
      {
        context->Replace(VF, V2.Reversed());
        VF = TopoDS::Vertex(V2.Reversed());
      }
      newtol = tolVL + PVL.Distance(PV1);
      if (tolV1 < newtol)
        B.UpdateVertex(V1, newtol);
      if (VL.Orientation() == V1.Orientation())
      {
        context->Replace(VL, V1);
        VL = V1;
      }
      else
      {
        context->Replace(VL, V1.Reversed());
        VL = TopoDS::Vertex(V1.Reversed());
      }
    }
    else
    {
      double newtol = tolVF + PVF.Distance(PV1);
      if (tolV1 < newtol)
        B.UpdateVertex(V1, newtol);
      if (VF.Orientation() == V1.Orientation())
      {
        context->Replace(VF, V1);
        VF = V1;
      }
      else
      {
        context->Replace(VF, V1.Reversed());
        VF = TopoDS::Vertex(V1.Reversed());
      }
      newtol = tolVL + PVL.Distance(PV2);
      if (tolV2 < newtol)
        B.UpdateVertex(V2, newtol);
      if (VL.Orientation() == V2.Orientation())
      {
        context->Replace(VL, V2);
        VL = V2;
      }
      else
      {
        context->Replace(VL, V2.Reversed());
        VL = TopoDS::Vertex(V2.Reversed());
      }
    }
    SeqE.Append(edge);
    aNum = 1;
  }

  if (fabs(a - par1) <= tol2d && fabs(b - par2) > tol2d)
  {
    TopoDS_Edge newE1, newE2;
    if (IsReverse)
    {
      if (!SplitEdge(edge, par2, V1, face, newE1, newE2, tol3d, tol2d))
        return false;
      double newtol = tolVF + PVF.Distance(PV2);
      if (tolV2 < newtol)
        B.UpdateVertex(V2, newtol);
      if (VF.Orientation() == V2.Orientation())
      {
        context->Replace(VF, V2);
        VF = V2;
      }
      else
      {
        context->Replace(VF, V2.Reversed());
        VF = TopoDS::Vertex(V2.Reversed());
      }
    }
    else
    {
      if (!SplitEdge(edge, par2, V2, face, newE1, newE2, tol3d, tol2d))
        return false;
      double newtol = tolVF + PVF.Distance(PV1);
      if (tolV1 < newtol)
        B.UpdateVertex(V1, newtol);
      if (VF.Orientation() == V1.Orientation())
      {
        context->Replace(VF, V1);
        VF = V1;
      }
      else
      {
        context->Replace(VF, V1.Reversed());
        VF = TopoDS::Vertex(V1.Reversed());
      }
    }
    SeqE.Append(newE1);
    SeqE.Append(newE2);
    aNum = 1;
  }

  if (fabs(a - par1) > tol2d && fabs(b - par2) <= tol2d)
  {
    TopoDS_Edge newE1, newE2;
    if (IsReverse)
    {
      if (!SplitEdge(edge, par1, V2, face, newE1, newE2, tol3d, tol2d))
        return false;
      double newtol = tolVL + PVL.Distance(PV1);
      if (tolV1 < newtol)
        B.UpdateVertex(V1, newtol);
      if (VL.Orientation() == V1.Orientation())
      {
        context->Replace(VL, V1);
        VL = V1;
      }
      else
      {
        context->Replace(VL, V1.Reversed());
        VL = TopoDS::Vertex(V1.Reversed());
      }
    }
    else
    {
      if (!SplitEdge(edge, par1, V1, face, newE1, newE2, tol3d, tol2d))
        return false;
      double newtol = tolVL + PVL.Distance(PV2);
      if (tolV2 < newtol)
        B.UpdateVertex(V2, newtol);
      if (VL.Orientation() == V2.Orientation())
      {
        context->Replace(VL, V2);
        VL = V2;
      }
      else
      {
        context->Replace(VL, V2.Reversed());
        VL = TopoDS::Vertex(V2.Reversed());
      }
    }
    SeqE.Append(newE1);
    SeqE.Append(newE2);
    aNum = 2;
  }

  if (fabs(a - par1) > tol2d && fabs(b - par2) > tol2d)
  {
    TopoDS_Edge newE1, newE2, newE3, newE4;
    if (IsReverse)
    {
      if (!SplitEdge(edge, par1, V2, face, newE1, newE2, tol3d, tol2d))
        return false;
      if (!SplitEdge(newE2, par2, V1, face, newE3, newE4, tol3d, tol2d))
        return false;
    }
    else
    {
      if (!SplitEdge(edge, par1, V1, face, newE1, newE2, tol3d, tol2d))
        return false;
      if (!SplitEdge(newE2, par2, V2, face, newE3, newE4, tol3d, tol2d))
        return false;
    }
    SeqE.Append(newE1);
    SeqE.Append(newE3);
    SeqE.Append(newE4);
    aNum = 2;
  }

  if (aNum == 0)
    return false;

  occ::handle<ShapeExtend_WireData> sewd = new ShapeExtend_WireData;
  for (int i = 1; i <= SeqE.Length(); i++)
  {
    sewd->Add(SeqE.Value(i));
  }
  context->Replace(edge, sewd->Wire());
  for (TopExp_Explorer exp(sewd->Wire(), TopAbs_EDGE); exp.More(); exp.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(exp.Current());
    BRepTools::Update(E);
  }

  return true;
}
