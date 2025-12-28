// Created on: 1999-05-14
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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

// 15.06 2000 gka fix against small edges ; merging ends pcurves and 3d curves

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeUpgrade_ConvertCurve2dToBezier.hxx>
#include <ShapeUpgrade_ConvertCurve3dToBezier.hxx>
#include <ShapeUpgrade_ConvertSurfaceToBezierBasis.hxx>
#include <ShapeUpgrade_FaceDivide.hxx>
#include <ShapeUpgrade_FixSmallBezierCurves.hxx>
#include <ShapeUpgrade_ShapeConvertToBezier.hxx>
#include <ShapeUpgrade_WireDivide.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

ShapeUpgrade_ShapeConvertToBezier::ShapeUpgrade_ShapeConvertToBezier()
{
  myLevel       = 0;
  my2dMode      = false;
  my3dMode      = false;
  mySurfaceMode = false;
  // set spesial flags to true
  my3dLineMode     = true;
  my3dCircleMode   = true;
  my3dConicMode    = true;
  myPlaneMode      = true;
  myRevolutionMode = true;
  myExtrusionMode  = true;
  myBSplineMode    = true;
}

//=================================================================================================

ShapeUpgrade_ShapeConvertToBezier::ShapeUpgrade_ShapeConvertToBezier(const TopoDS_Shape& S)
    : ShapeUpgrade_ShapeDivide(S)
{
  myLevel       = 0;
  my2dMode      = false;
  my3dMode      = false;
  mySurfaceMode = false;
  // set spesial flags to true
  my3dLineMode     = true;
  my3dCircleMode   = true;
  my3dConicMode    = true;
  myPlaneMode      = true;
  myRevolutionMode = true;
  myExtrusionMode  = true;
  myBSplineMode    = true;
}

//=================================================================================================

bool ShapeUpgrade_ShapeConvertToBezier::Perform(const bool newContext)
{
  myLevel++;

  bool res = false;
  if (myLevel == 1)
  {
    bool isNewContext = newContext;
    if (mySurfaceMode)
    {
      bool mode2d = my2dMode, mode3d = my3dMode;
      my2dMode = my3dMode = false;
      res                 = ShapeUpgrade_ShapeDivide::Perform(isNewContext);
      isNewContext        = false;
      myShape             = myResult;
      my2dMode            = mode2d;
      my3dMode            = mode3d;
    }
    if (my2dMode || my3dMode)
    {
      bool modeS = mySurfaceMode;
      mySurfaceMode          = false;
      res                    = ShapeUpgrade_ShapeDivide::Perform(isNewContext);
      mySurfaceMode          = modeS;
    }
  }
  else
    res = ShapeUpgrade_ShapeDivide::Perform(newContext);
  // pdn Hereafter the fix on GeomLib:SameParameter.
  // In order to fix this bug all edges that are based on
  // bezier curves (2d or 3d) and have range not equal to [0,1]
  // are performed the following sequence:
  //  1. Segment on bezier curve
  //  2. Changing rande of edge to [0,1]
  if (myLevel == 1)
  {
    BRep_Builder            B;
    ShapeAnalysis_Edge      sae;
    ShapeBuild_Edge         sbe;
    constexpr double preci = Precision::PConfusion();
    for (TopExp_Explorer exp(myResult, TopAbs_FACE); exp.More(); exp.Next())
    {
      TopoDS_Face face = TopoDS::Face(exp.Current());
      face.Orientation(TopAbs_FORWARD);
      for (TopExp_Explorer exp1(face, TopAbs_WIRE); exp1.More(); exp1.Next())
      {
        TopoDS_Wire           wire = TopoDS::Wire(exp1.Current());
        occ::handle<ShapeFix_Wire> sfw  = new ShapeFix_Wire(wire, face, myPrecision);
        sfw->FixReorder();
        sfw->FixShifted(); // for cylinders.brep
        occ::handle<ShapeExtend_WireData> sewd = sfw->WireData();
        for (int i = 1; i <= sewd->NbEdges(); i++)
        {
          TopoDS_Edge edge = sewd->Edge(i);
          // TopoDS_Edge edge = TopoDS::Edge(exp1.Current());
          occ::handle<Geom_Curve> c3d;
          double      first, last;
          TopoDS_Vertex      V1, V2;
          TopExp::Vertices(edge, V1, V2);
          if (sae.Curve3d(edge, c3d, first, last, false))
          {
            if (c3d->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
            {
              // B.SameRange(edge, false);
              occ::handle<Geom_BezierCurve> bezier = occ::down_cast<Geom_BezierCurve>(c3d);
              if (first != 0 || last != 1)
              {
                if (first > preci || last < 1 - preci)
                {
                  bezier->Segment(first, last);
                  B.UpdateEdge(edge, bezier, 0.);
                }
                sbe.SetRange3d(edge, 0, 1);
              }
              if (!bezier.IsNull())
              { // gka fix against small edges ; merging ends of 3d curves
                gp_Pnt p1  = bezier->Value(first);
                gp_Pnt p2  = bezier->Value(last);
                gp_Pnt p1v = BRep_Tool::Pnt(V1);
                gp_Pnt p2v = BRep_Tool::Pnt(V2);
                if (p1.Distance(p1v) > Precision::Confusion())
                  bezier->SetPole(1, p1v);
                if (p2.Distance(p2v) > Precision::Confusion())
                  bezier->SetPole(bezier->NbPoles(), p2v);
                B.UpdateEdge(edge, bezier, 0.);
              }
            }
            else if (my2dMode)
              B.SameRange(edge, true);
          }
          occ::handle<Geom2d_Curve>       c2d, c2drev;
          occ::handle<Geom2d_BezierCurve> bezier, bezierR;
          bool           isSeam = BRep_Tool::IsClosed(edge, face);
          if (!sae.PCurve(edge, face, c2d, first, last, false))
            continue;
          if (!c2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
            continue;
          occ::handle<Geom2d_Curve> newRevCurve, newCurve;
          bezier = occ::down_cast<Geom2d_BezierCurve>(c2d);
          if (isSeam)
          {
            TopoDS_Shape aLocalShape = edge.Reversed();
            TopoDS_Edge  tmpedge     = TopoDS::Edge(aLocalShape);
            //	   TopoDS_Edge tmpedge = TopoDS::Edge(edge.Reversed());
            if (sae.PCurve(tmpedge, face, c2drev, first, last, false))
            {
              if (c2drev->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
              {
                bezierR = occ::down_cast<Geom2d_BezierCurve>(c2drev);
              }
            }
          }
          if (first != 0 || last != 1)
          {
            if (first > preci || last < 1 - preci)
            {
              if (!bezier.IsNull())
              {
                bezier->Segment(first, last);
                newCurve = bezier;
              }
              else
                GeomLib::SameRange(preci, c2d, first, last, 0, 1, newCurve);
              if (isSeam)
              {
                if (!bezierR.IsNull())
                {
                  bezierR->Segment(first, last);
                  newRevCurve = bezierR;
                }
                else if (!c2drev.IsNull())
                  GeomLib::SameRange(preci, c2drev, first, last, 0, 1, newRevCurve);
                if (edge.Orientation() == TopAbs_FORWARD)
                  B.UpdateEdge(edge, newCurve, newRevCurve, face, 0.);
                else
                  B.UpdateEdge(edge, newRevCurve, newCurve, face, 0.);
              }
            }
            B.Range(edge, face, 0, 1);
          }

          TopoDS_Edge                edgenext = sewd->Edge((i == sewd->NbEdges() ? 1 : i + 1));
          occ::handle<Geom2d_Curve>       c2dnext, c2drevnext, newnextCurve;
          double              first2, last2;
          occ::handle<Geom2d_BezierCurve> beziernext, bezierRnext;
          if (!sae.PCurve(edgenext, face, c2dnext, first2, last2, false))
            continue;
          if (!c2dnext->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
            continue;
          beziernext = occ::down_cast<Geom2d_BezierCurve>(c2dnext);
          // occ::handle<Geom2d_Curve> newRevCurve;
          if (isSeam)
          {
            TopoDS_Shape aLocalShape = edgenext.Reversed();
            TopoDS_Edge  tmpedge     = TopoDS::Edge(aLocalShape);
            //	    TopoDS_Edge tmpedge = TopoDS::Edge(edgenext.Reversed());
            if (sae.PCurve(tmpedge, face, c2drevnext, first, last, false))
            {
              if (c2drevnext->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
              {
                bezierRnext = occ::down_cast<Geom2d_BezierCurve>(c2drevnext);
              }
            }
          }
          if (first2 != 0 || last2 != 1)
          {
            if (first2 > preci || last2 < 1 - preci)
            {
              if (!beziernext.IsNull())
              {
                beziernext->Segment(first2, last2);
                newnextCurve = beziernext;
              }
              else
                GeomLib::SameRange(preci, c2dnext, first2, last2, 0, 1, newnextCurve);
              if (isSeam)
              {
                if (!bezierRnext.IsNull())
                {
                  bezierRnext->Segment(first2, last2);
                  newRevCurve = bezierRnext;
                }
                else if (!c2drevnext.IsNull())
                  GeomLib::SameRange(preci, c2drevnext, first2, last2, 0, 1, newRevCurve);
                if (edge.Orientation() == TopAbs_FORWARD)
                  B.UpdateEdge(edgenext, newnextCurve, newRevCurve, face, 0.);
                else
                  B.UpdateEdge(edgenext, newRevCurve, newnextCurve, face, 0.);
              }
            }
            B.Range(edgenext, face, 0, 1);
          }

          // clang-format off
	  if(bezier.IsNull()  || beziernext.IsNull() ) continue; //gka fix against small edges ; merging ends of pcurves
          // clang-format on
          double f1, l1, f2, l2;
          f1                = bezier->FirstParameter();
          l1                = bezier->LastParameter();
          f2                = beziernext->FirstParameter();
          l2                = beziernext->LastParameter();
          gp_Pnt2d p2d1     = bezier->Value(f1);
          gp_Pnt2d p2d2     = bezier->Value(l1);
          gp_Pnt2d p2d1next = beziernext->Value(f2);
          gp_Pnt2d p2d2next = beziernext->Value(l2);
          if (edge.Orientation() == TopAbs_FORWARD && edgenext.Orientation() == TopAbs_FORWARD)
          {
            if (p2d2.Distance(p2d1next) > Precision::PConfusion())
            {
              gp_Pnt2d pmid = 0.5 * (p2d2.XY() + p2d1next.XY());
              //	      gp_Pnt2d p1 =  bezier->Pole(bezier->NbPoles());
              //	      gp_Pnt2d p2 =  beziernext->Pole(1);
              bezier->SetPole(bezier->NbPoles(), pmid);
              beziernext->SetPole(1, pmid);
            }
          }
          if (edge.Orientation() == TopAbs_FORWARD && edgenext.Orientation() == TopAbs_REVERSED)
          {
            if (p2d2.Distance(p2d2next) > Precision::PConfusion())
            {
              gp_Pnt2d pmid = 0.5 * (p2d2.XY() + p2d2next.XY());
              //	      gp_Pnt2d p1 =  bezier->Pole(bezier->NbPoles());
              //	      gp_Pnt2d p2 =  beziernext->Pole(beziernext->NbPoles());
              bezier->SetPole(bezier->NbPoles(), pmid);
              beziernext->SetPole(beziernext->NbPoles(), pmid);
            }
          }
          if (edge.Orientation() == TopAbs_REVERSED && edgenext.Orientation() == TopAbs_FORWARD)
          {
            if (p2d1.Distance(p2d1next) > Precision::PConfusion())
            {
              gp_Pnt2d pmid = 0.5 * (p2d1.XY() + p2d1next.XY());
              //	      gp_Pnt2d p1 =  bezier->Pole(1);
              //	      gp_Pnt2d p2 =  beziernext->Pole(1);
              bezier->SetPole(1, pmid);
              beziernext->SetPole(1, pmid);
            }
          }
          if (edge.Orientation() == TopAbs_REVERSED && edgenext.Orientation() == TopAbs_REVERSED)
          {
            if (p2d1.Distance(p2d2next) > Precision::PConfusion())
            {
              gp_Pnt2d pmid = 0.5 * (p2d1.XY() + p2d2next.XY());
              //	      gp_Pnt2d p1 =  bezier->Pole(1);
              //	      gp_Pnt2d p2 =  beziernext->Pole(beziernext->NbPoles());
              bezier->SetPole(1, pmid);
              beziernext->SetPole(beziernext->NbPoles(), pmid);
            }
          }
        }
        BRepTools::Update(face);
      }
    }
  }

  myLevel--;

  return res;
}

//=================================================================================================

occ::handle<ShapeUpgrade_FaceDivide> ShapeUpgrade_ShapeConvertToBezier::GetSplitFaceTool() const
{
  occ::handle<ShapeUpgrade_FaceDivide> tool  = new ShapeUpgrade_FaceDivide;
  occ::handle<ShapeUpgrade_WireDivide> wtool = new ShapeUpgrade_WireDivide;
  if (my3dMode)
  {
    occ::handle<ShapeUpgrade_ConvertCurve3dToBezier> curve3dConverter =
      new ShapeUpgrade_ConvertCurve3dToBezier;
    curve3dConverter->SetLineMode(my3dLineMode);
    curve3dConverter->SetCircleMode(my3dCircleMode);
    curve3dConverter->SetConicMode(my3dConicMode);
    wtool->SetSplitCurve3dTool(curve3dConverter);
  }
  if (my2dMode)
    wtool->SetSplitCurve2dTool(new ShapeUpgrade_ConvertCurve2dToBezier);
  wtool->SetEdgeMode(myEdgeMode);
  occ::handle<ShapeUpgrade_FixSmallBezierCurves> FixSmallBezierCurvesTool =
    new ShapeUpgrade_FixSmallBezierCurves;
  wtool->SetFixSmallCurveTool(FixSmallBezierCurvesTool);
  tool->SetWireDivideTool(wtool);
  if (mySurfaceMode)
  {
    occ::handle<ShapeUpgrade_ConvertSurfaceToBezierBasis> stool =
      new ShapeUpgrade_ConvertSurfaceToBezierBasis;
    stool->SetPlaneMode(myPlaneMode);
    stool->SetRevolutionMode(myRevolutionMode);
    stool->SetExtrusionMode(myExtrusionMode);
    stool->SetBSplineMode(myBSplineMode);
    tool->SetSplitSurfaceTool(stool);
  }
  return tool;
}

Message_Msg ShapeUpgrade_ShapeConvertToBezier::GetFaceMsg() const
{
  return "ShapeDivide.FaceConvertToBezier.MSG0";
}

Message_Msg ShapeUpgrade_ShapeConvertToBezier::GetWireMsg() const
{
  return "ShapeDivide.WireConvertToBezier.MSG0";
}

Message_Msg ShapeUpgrade_ShapeConvertToBezier::GetEdgeMsg() const
{
  return "ShapeDivide.EdgeConvertToBezier.MSG0";
}
