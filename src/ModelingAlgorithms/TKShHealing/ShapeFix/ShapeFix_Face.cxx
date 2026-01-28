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

// pdn 10.12.98: tr9_r0501-ug
// pdn 28.12.98: PRO10366 shifting pcurve between two singularities
//: k7 abv 5.01.99: USA60022.igs ent 243: FixMissingSeam() improved
//: l2 abv 10.01.99: USA60022 7289: corrections for reversed face
// gka 11.01.99 file PRO7755.stp #2018: work-around error in BRepLib_MakeFace
//: p4 abv, pdn 23.02.99: PRO9234 #15720: call BRepTools::Update() for faces
//    rln 03.03.99 S4135: transmission of parameter precision to SA_Surface::NbSingularities
//: q5 abv 19.03.99 code improvement
//%14 pdn 15.03.99 adding function for fixing null area wires
//%15 pdn 20.03.99 code improvement
//    abv 09.04.99 S4136: improve tolerance management; remove unused flag Closed
// #4  szv          S4163: optimization
//    smh 31.01.01 BUC60810 : Case of small wire on face in solid
// sln 25.09.2001  checking order of 3d and 2d representation curves
// abv 19.10.2001  FixAddNaturalBound improved and extracted as separate fix
// skl,pdn 14.05.2002  OCC55 (correction precision for small faces)

#include <Bnd_Box.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Surface.hxx>
#include <GProp_GProps.hxx>
#include <Message_Msg.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_IntersectionTool.hxx>
#include <ShapeFix_SplitTool.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Face, ShapeFix_Root)

static bool IsSurfaceUVInfinite(const occ::handle<Geom_Surface>& theSurf)
{
  double UMin, UMax, VMin, VMax;
  theSurf->Bounds(UMin, UMax, VMin, VMax);

  return (Precision::IsInfinite(UMin) || Precision::IsInfinite(UMax) || Precision::IsInfinite(VMin)
          || Precision::IsInfinite(VMax));
}

static bool IsSurfaceUVPeriodic(const occ::handle<GeomAdaptor_Surface>& theSurf)
{
  return ((theSurf->IsUPeriodic() && theSurf->IsVPeriodic())
          || theSurf->GetType() == GeomAbs_Sphere);
}

//=================================================================================================

ShapeFix_Face::ShapeFix_Face()
{
  myFwd     = true;
  myStatus  = 0;
  myFixWire = new ShapeFix_Wire;
  ClearModes();
}

//=================================================================================================

ShapeFix_Face::ShapeFix_Face(const TopoDS_Face& face)
{
  myFwd     = true;
  myStatus  = 0;
  myFixWire = new ShapeFix_Wire;
  ClearModes();
  Init(face);
}

//=================================================================================================

void ShapeFix_Face::ClearModes()
{
  myFixWireMode              = -1;
  myFixOrientationMode       = -1;
  myFixAddNaturalBoundMode   = -1;
  myFixMissingSeamMode       = -1;
  myFixSmallAreaWireMode     = -1;
  myRemoveSmallAreaFaceMode  = -1;
  myFixIntersectingWiresMode = -1;
  myFixLoopWiresMode         = -1;
  myFixSplitFaceMode         = -1;
  myAutoCorrectPrecisionMode = 1;
  myFixPeriodicDegenerated   = -1;
}

//=================================================================================================

void ShapeFix_Face::SetMsgRegistrator(const occ::handle<ShapeExtend_BasicMsgRegistrator>& msgreg)
{
  ShapeFix_Root::SetMsgRegistrator(msgreg);
  myFixWire->SetMsgRegistrator(msgreg);
}

//=================================================================================================

void ShapeFix_Face::SetPrecision(const double preci)
{
  ShapeFix_Root::SetPrecision(preci);
  myFixWire->SetPrecision(preci);
}

//=================================================================================================

void ShapeFix_Face::SetMinTolerance(const double mintol)
{
  ShapeFix_Root::SetMinTolerance(mintol);
  myFixWire->SetMinTolerance(mintol);
}

//=================================================================================================

void ShapeFix_Face::SetMaxTolerance(const double maxtol)
{
  ShapeFix_Root::SetMaxTolerance(maxtol);
  myFixWire->SetMaxTolerance(maxtol);
}

//=================================================================================================

void ShapeFix_Face::Init(const occ::handle<Geom_Surface>& surf, const double preci, const bool fwd)
{
  myStatus                               = 0;
  occ::handle<ShapeAnalysis_Surface> sas = new ShapeAnalysis_Surface(surf);
  Init(sas, preci, fwd);
}

//=================================================================================================

void ShapeFix_Face::Init(const occ::handle<ShapeAnalysis_Surface>& surf,
                         const double                              preci,
                         const bool                                fwd)
{
  myStatus = 0;
  mySurf   = surf;
  SetPrecision(preci);
  BRep_Builder B;
  B.MakeFace(myFace, mySurf->Surface(), ::Precision::Confusion());
  myShape = myFace;
  myFwd   = fwd;
  if (!fwd)
    myFace.Orientation(TopAbs_REVERSED);
}

//=================================================================================================

void ShapeFix_Face::Init(const TopoDS_Face& theFace)
{
  myStatus = 0;
  // Check if surface is null. It doesn't make sense to create ShapeAnalysis_Surface in that case.
  const occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(theFace);
  if (!aSurface.IsNull())
  {
    mySurf = new ShapeAnalysis_Surface(aSurface);
  }
  myFwd   = (theFace.Orientation() != TopAbs_REVERSED);
  myFace  = theFace;
  myShape = myFace;
}

//=================================================================================================

void ShapeFix_Face::Add(const TopoDS_Wire& wire)
{
  if (wire.IsNull())
    return;
  BRep_Builder B;
  // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape fc = myFace.Oriented(TopAbs_FORWARD); //: l2 abv 10 Jan 99: Oriented()
  B.Add(fc, wire);
}

//=======================================================================
// function : SplitWire
// purpose  : auxiliary - try to split wire (it is needed if some segments
//           were removed in ShapeFix_Wire::FixSelfIntersection() )
//=======================================================================
static bool SplitWire(const TopoDS_Face&                  face,
                      const TopoDS_Wire&                  wire,
                      NCollection_Sequence<TopoDS_Shape>& aResWires)
{
  NCollection_Map<int>              UsedEdges;
  occ::handle<ShapeExtend_WireData> sewd = new ShapeExtend_WireData(wire);
  int                               i, j, k;
  ShapeAnalysis_Edge                sae;
  for (i = 1; i <= sewd->NbEdges(); i++)
  {
    if (UsedEdges.Contains(i))
      continue;
    TopoDS_Edge E1 = sewd->Edge(i);
    UsedEdges.Add(i);
    TopoDS_Vertex V0, V1, V2;
    V0                                      = sae.FirstVertex(E1);
    V1                                      = sae.LastVertex(E1);
    occ::handle<ShapeExtend_WireData> sewd1 = new ShapeExtend_WireData;
    sewd1->Add(E1);
    bool IsConnectedEdge = true;
    for (j = 2; j <= sewd->NbEdges() && IsConnectedEdge; j++)
    {
      TopoDS_Edge E2;
      for (k = 2; k <= sewd->NbEdges(); k++)
      {
        if (UsedEdges.Contains(k))
          continue;
        E2                = sewd->Edge(k);
        TopoDS_Vertex V21 = sae.FirstVertex(E2);
        TopoDS_Vertex V22 = sae.LastVertex(E2);
        if (sae.FirstVertex(E2).IsSame(V1))
        {
          sewd1->Add(E2);
          UsedEdges.Add(k);
          V1 = sae.LastVertex(E2);
          break;
        }
      }
      if (k > sewd->NbEdges())
      {
        IsConnectedEdge = false;
        break;
      }
      if (V1.IsSame(V0))
      {
        // check that V0 and V1 are same in 2d too
        double                    a1, b1, a2, b2;
        occ::handle<Geom2d_Curve> curve1 = BRep_Tool::CurveOnSurface(E1, face, a1, b1);
        occ::handle<Geom2d_Curve> curve2 = BRep_Tool::CurveOnSurface(E2, face, a2, b2);
        if (curve1.IsNull() || curve2.IsNull())
          continue;

        gp_Pnt2d v0, v1;
        if (E1.Orientation() == TopAbs_REVERSED)
          a1 = b1;
        if (E2.Orientation() == TopAbs_REVERSED)
          b2 = a2;
        curve1->D0(a1, v0);
        curve2->D0(b2, v1);
        GeomAdaptor_Surface anAdaptor(BRep_Tool::Surface(face));
        double              tol = std::max(BRep_Tool::Tolerance(V0), BRep_Tool::Tolerance(V1));
        double maxResolution = 2 * std::max(anAdaptor.UResolution(tol), anAdaptor.VResolution(tol));
        if (v0.SquareDistance(v1) < maxResolution)
        {
          // new wire is closed, put it into sequence
          aResWires.Append(sewd1->Wire());
          break;
        }
      }
    }
    if (!IsConnectedEdge)
    {
      // create new notclosed wire
      aResWires.Append(sewd1->Wire());
    }
    if (UsedEdges.Extent() == sewd->NbEdges())
      break;
  }

  return true;
}

//=================================================================================================

bool ShapeFix_Face::Perform(const Message_ProgressRange& theProgress)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myFixWire->SetContext(Context());
  occ::handle<ShapeFix_Wire> theAdvFixWire = myFixWire;
  if (theAdvFixWire.IsNull())
    return false;

  BRep_Builder B;
  TopoDS_Shape aInitFace = myFace;
  // perform first part of fixes on wires
  bool isfixReorder = false;
  bool isReplaced   = false;

  // gka fix in order to avoid lost messages (following OCC21771)
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aMapReorderedWires;

  double aSavPreci = Precision();
  if (NeedFix(myFixWireMode))
  {
    theAdvFixWire->SetFace(myFace, mySurf);

    int usFixLackingMode = theAdvFixWire->FixLackingMode();
    // int usFixNotchedEdgesMode = theAdvFixWire->FixNotchedEdgesMode(); // CR0024983
    int usFixSelfIntersectionMode   = theAdvFixWire->FixSelfIntersectionMode();
    theAdvFixWire->FixLackingMode() = false;
    // theAdvFixWire->FixNotchedEdgesMode() = false; // CR0024983
    theAdvFixWire->FixSelfIntersectionMode() = false;

    bool         fixed = false;
    TopoDS_Shape S     = myFace;
    if (!Context().IsNull())
      S = Context()->Apply(myFace);
    TopoDS_Shape emptyCopied = S.EmptyCopied();
    TopoDS_Face  tmpFace     = TopoDS::Face(emptyCopied);
    tmpFace.Orientation(TopAbs_FORWARD);

    // skl 29.03.2010 (OCC21623)
    if (myAutoCorrectPrecisionMode)
    {
      double size     = ShapeFix::LeastEdgeSize(S);
      double newpreci = std::min(aSavPreci, size / 2.);
      newpreci        = newpreci * 1.00001;
      if (aSavPreci > newpreci && newpreci > Precision::Confusion())
      {
        SetPrecision(newpreci);
        theAdvFixWire->SetPrecision(newpreci);
      }
    }

    isfixReorder = false;

    for (TopoDS_Iterator iter(S, false); iter.More(); iter.Next())
    {
      if (iter.Value().ShapeType() != TopAbs_WIRE)
      {
        B.Add(tmpFace, iter.Value());
        continue;
      }
      TopoDS_Wire wire = TopoDS::Wire(iter.Value());
      theAdvFixWire->Load(wire);
      if (theAdvFixWire->NbEdges() == 0)
      {
        if (theAdvFixWire->WireData()->NbNonManifoldEdges())
          B.Add(tmpFace, wire);
        else
        {
          fixed = true;
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
        }
        continue;
      }
      if (theAdvFixWire->Perform(theProgress))
      {
        // fixed = true;
        isfixReorder  = (theAdvFixWire->StatusReorder(ShapeExtend_DONE) || isfixReorder);
        fixed         = (theAdvFixWire->StatusSmall(ShapeExtend_DONE)
                 || theAdvFixWire->StatusConnected(ShapeExtend_DONE)
                 || theAdvFixWire->StatusEdgeCurves(ShapeExtend_DONE)
                 || theAdvFixWire->StatusNotches(ShapeExtend_DONE) || // CR0024983
                 theAdvFixWire->StatusFixTails(ShapeExtend_DONE)
                 || theAdvFixWire->StatusDegenerated(ShapeExtend_DONE)
                 || theAdvFixWire->StatusClosed(ShapeExtend_DONE));
        TopoDS_Wire w = theAdvFixWire->Wire();
        if (fixed)
        {
          if (!Context().IsNull())
            Context()->Replace(wire, w);
          if (theAdvFixWire->NbEdges() == 0)
          {
            myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
            continue;
          }
        }
        else if (!wire.IsSame(w))
          aMapReorderedWires.Bind(wire, w);

        wire = w;
      }
      B.Add(tmpFace, wire);
    }

    theAdvFixWire->FixLackingMode()          = usFixLackingMode;
    theAdvFixWire->FixSelfIntersectionMode() = usFixSelfIntersectionMode;
    if (!myFwd)
      tmpFace.Orientation(TopAbs_REVERSED);

    if (fixed)
    {
      if (!Context().IsNull())
        Context()->Replace(S, tmpFace);
      isReplaced = true;
    }
    if (fixed || isfixReorder)
    {
      myFace = tmpFace;
      if (!theAdvFixWire->StatusReorder(ShapeExtend_DONE5))
      {
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      }
    }
  }

  myResult              = myFace;
  TopoDS_Shape savShape = myFace; // gka BUG 6555

  // Specific case for conic surfaces
  if (NeedFix(myFixPeriodicDegenerated))
    this->FixPeriodicDegenerated();

  // fix missing seam
  if (NeedFix(myFixMissingSeamMode))
  {
    if (FixMissingSeam())
    {
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    }
  }

  // cycle by all possible faces coming from FixMissingSeam
  // each face is processed as if it was single
  TopExp_Explorer exp(myResult, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    myFace                  = TopoDS::Face(exp.Current());
    bool NeedCheckSplitWire = false;

    // perform second part of fixes on wires
    if (NeedFix(myFixWireMode))
    {
      theAdvFixWire->SetFace(myFace, mySurf);

      int usFixSmallMode                  = theAdvFixWire->FixSmallMode();
      int usFixConnectedMode              = theAdvFixWire->FixConnectedMode();
      int usFixEdgeCurvesMode             = theAdvFixWire->FixEdgeCurvesMode();
      int usFixDegeneratedMode            = theAdvFixWire->FixDegeneratedMode();
      theAdvFixWire->FixSmallMode()       = false;
      theAdvFixWire->FixConnectedMode()   = false;
      theAdvFixWire->FixEdgeCurvesMode()  = false;
      theAdvFixWire->FixDegeneratedMode() = false;

      bool         fixed = false;
      TopoDS_Shape S     = myFace;
      if (!Context().IsNull())
        S = Context()->Apply(myFace);
      TopoDS_Shape emptyCopied = S.EmptyCopied();
      TopoDS_Face  tmpFace     = TopoDS::Face(emptyCopied);
      tmpFace.Orientation(TopAbs_FORWARD);
      for (TopoDS_Iterator iter(S, false); iter.More(); iter.Next())
      {
        if (iter.Value().ShapeType() != TopAbs_WIRE)
        {
          B.Add(tmpFace, iter.Value());
          continue;
        }

        TopoDS_Wire wire = TopoDS::Wire(iter.Value());
        theAdvFixWire->Load(wire);
        if (theAdvFixWire->NbEdges() == 0)
        {
          if (theAdvFixWire->WireData()->NbNonManifoldEdges())
            B.Add(tmpFace, wire);
          else
          {
            fixed = true;
            myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
          }
          continue;
        }
        if (theAdvFixWire->Perform())
        {
          isfixReorder  = theAdvFixWire->StatusReorder(ShapeExtend_DONE);
          fixed         = (theAdvFixWire->StatusLacking(ShapeExtend_DONE)
                   || theAdvFixWire->StatusSelfIntersection(ShapeExtend_DONE)
                   || theAdvFixWire->StatusNotches(ShapeExtend_DONE)
                   || theAdvFixWire->StatusFixTails(ShapeExtend_DONE));
          TopoDS_Wire w = theAdvFixWire->Wire();
          if (fixed)
          {
            if (!Context().IsNull())
              Context()->Replace(wire, w);
          }
          else if (!wire.IsSame(w))
            aMapReorderedWires.Bind(wire, w);

          wire = w;
        }
        if (theAdvFixWire->StatusRemovedSegment())
          NeedCheckSplitWire = true;

        // fix for loop of wire
        NCollection_Sequence<TopoDS_Shape> aLoopWires;
        if (NeedFix(myFixLoopWiresMode) && FixLoopWire(aLoopWires))
        {
          if (aLoopWires.Length() > 1)
            // clang-format off
            SendWarning ( wire, Message_Msg ( "FixAdvFace.FixLoopWire.MSG0" ) );// Wire was split on several wires
          // clang-format on
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
          fixed = true;
          int k = 1;
          for (; k <= aLoopWires.Length(); k++)
            B.Add(tmpFace, aLoopWires.Value(k));
        }
        else
          B.Add(tmpFace, wire);
      }

      theAdvFixWire->FixSmallMode()       = usFixSmallMode;
      theAdvFixWire->FixConnectedMode()   = usFixConnectedMode;
      theAdvFixWire->FixEdgeCurvesMode()  = usFixEdgeCurvesMode;
      theAdvFixWire->FixDegeneratedMode() = usFixDegeneratedMode;

      if (fixed)
      {
        if (!myFwd)
          tmpFace.Orientation(TopAbs_REVERSED);
        if (!isReplaced && !aInitFace.IsSame(myResult)
            && !Context().IsNull()) // gka 06.09.04 BUG 6555
          Context()->Replace(aInitFace, savShape);
        if (!Context().IsNull())
          Context()->Replace(S, tmpFace);
        myFace = tmpFace;
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      }
    }

    if (NeedCheckSplitWire)
    {
      // try to split wire - it is needed if some segments were removed
      // in ShapeFix_Wire::FixSelfIntersection()
      TopoDS_Shape S = myFace;
      if (!Context().IsNull())
        S = Context()->Apply(myFace);
      TopoDS_Shape emptyCopied = S.EmptyCopied();
      TopoDS_Face  tmpFace     = TopoDS::Face(emptyCopied);
      tmpFace.Orientation(TopAbs_FORWARD);
      NCollection_Sequence<TopoDS_Shape> aWires;
      int                                nbw = 0;
      for (TopoDS_Iterator iter(S, false); iter.More(); iter.Next())
      {
        if (iter.Value().ShapeType() != TopAbs_WIRE)
        {
          B.Add(tmpFace, iter.Value());
          continue;
        }
        if (iter.Value().Orientation() != TopAbs_FORWARD
            && iter.Value().Orientation() != TopAbs_REVERSED)
        {
          B.Add(tmpFace, TopoDS::Wire(iter.Value()));
          continue;
        }
        nbw++;
        TopoDS_Wire wire = TopoDS::Wire(iter.Value());
        SplitWire(tmpFace, wire, aWires);
      }
      if (nbw < aWires.Length())
      {
        for (int iw = 1; iw <= aWires.Length(); iw++)
          B.Add(tmpFace, aWires.Value(iw));
        if (!Context().IsNull())
          Context()->Replace(S, tmpFace);
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE8);
        myFace = tmpFace;
      }
    }

    // fix intersecting wires
    if (FixWiresTwoCoincEdges())
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
    if (NeedFix(myFixIntersectingWiresMode))
    {
      if (FixIntersectingWires())
      {
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);
      }
    }

    // fix orientation
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      MapWires;
    MapWires.Clear();
    if (NeedFix(myFixOrientationMode))
    {
      if (FixOrientation(MapWires))
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    }

    BRepTools::Update(myFace);

    // fix natural bounds
    bool NeedSplit = true;
    if (FixAddNaturalBound())
    {
      NeedSplit = false;
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
    }

    // split face
    if (NeedFix(myFixSplitFaceMode) && NeedSplit && MapWires.Extent() > 1)
    {
      if (FixSplitFace(MapWires))
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE8);
    }
  }

  // return the original preci
  SetPrecision(aSavPreci);
  theAdvFixWire->SetPrecision(aSavPreci);

  // cycle by all possible faces coming from FixAddNaturalBound
  // each face is processed as if it was single
  for (exp.Init(myResult, TopAbs_FACE); exp.More(); exp.Next())
  {
    myFace = TopoDS::Face(exp.Current());

    // fix small-area wires
    if (NeedFix(myFixSmallAreaWireMode, false))
    {
      const bool isRemoveFace = NeedFix(myRemoveSmallAreaFaceMode, false);
      if (FixSmallAreaWire(isRemoveFace))
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
    }
  }

  if (!Context().IsNull())
  {
    if (Status(ShapeExtend_DONE) && !isReplaced && !aInitFace.IsSame(savShape))
    {
      // gka fix in order to avoid lost messages (following OCC21771)
      if (aMapReorderedWires.Extent())
      {
        TopoDS_Iterator aItW(aInitFace, false);
        for (; aItW.More(); aItW.Next())
        {
          TopoDS_Shape aCurW = aItW.Value();
          while (aMapReorderedWires.IsBound(aCurW))
          {
            const TopoDS_Shape& aFixW = aMapReorderedWires.Find(aCurW);
            Context()->Replace(aCurW, aFixW);
            aCurW = aFixW;
          }
        }
      }
      Context()->Replace(aInitFace, savShape);
    }
    myResult = Context()->Apply(aInitFace); // gka 06.09.04
  }
  else if (!Status(ShapeExtend_DONE))
    myResult = aInitFace;

  return Status(ShapeExtend_DONE);
}

//=================================================================================================

// Shift all pcurves of edges in the given wire on the given face
// to vector <vec>
static void Shift2dWire(const TopoDS_Wire&                        w,
                        const TopoDS_Face&                        f,
                        const gp_Vec2d                            vec,
                        const occ::handle<ShapeAnalysis_Surface>& theSurface,
                        bool                                      recompute3d = false)
{
  gp_Trsf2d tr2d;
  tr2d.SetTranslation(vec.XY());
  ShapeAnalysis_Edge sae;
  ShapeBuild_Edge    sbe;
  BRep_Builder       B;
  for (TopoDS_Iterator ei(w, false); ei.More(); ei.Next())
  {
    TopoDS_Edge               edge = TopoDS::Edge(ei.Value());
    occ::handle<Geom2d_Curve> C2d;
    double                    cf, cl;
    if (!sae.PCurve(edge, f, C2d, cf, cl, true))
      continue;
    C2d->Transform(tr2d);
    if (recompute3d)
    {
      // recompute 3d curve and vertex
      sbe.RemoveCurve3d(edge);
      sbe.BuildCurve3d(edge);
      B.UpdateVertex(sae.FirstVertex(edge), theSurface->Value(C2d->Value(cf)), 0.);
    }
  }
}

// Cut interval from the sequence of intervals
static bool CutInterval(NCollection_Sequence<gp_Pnt2d>& intervals,
                        const gp_Pnt2d&                 toAddI,
                        const double                    period)
{
  if (intervals.Length() <= 0)
    return false;
  for (int j = 0; j < 2; j++)
  { // try twice, align to bottom and to top
    for (int i = 1; i <= intervals.Length(); i++)
    {
      gp_Pnt2d interval = intervals(i);
      // ACIS907, OCC921 a054a.sat (face 124)
      double   shift = ShapeAnalysis::AdjustByPeriod((j ? toAddI.X() : toAddI.Y()),
                                                   0.5 * (interval.X() + interval.Y()),
                                                   period);
      gp_Pnt2d toAdd(toAddI.X() + shift, toAddI.Y() + shift);
      if (toAdd.Y() <= interval.X() || toAdd.X() >= interval.Y())
        continue;
      if (toAdd.X() > interval.X())
      {
        if (toAdd.Y() < interval.Y())
        {
          intervals.InsertBefore(i, interval);
          intervals.ChangeValue(i + 1).SetX(toAdd.Y()); // i++...
        }
        intervals.ChangeValue(i).SetY(toAdd.X());
      }
      else if (toAdd.Y() < interval.Y())
      {
        intervals.ChangeValue(i).SetX(toAdd.Y());
      }
      else
        intervals.Remove(i--);
    }
  }
  return true;
}

// Find middle of the biggest interval
static double FindBestInterval(NCollection_Sequence<gp_Pnt2d>& intervals)
{
  double shift = 0., max = -1.;
  for (int i = 1; i <= intervals.Length(); i++)
  {
    gp_Pnt2d interval = intervals(i);
    if (interval.Y() - interval.X() <= max)
      continue;
    max   = interval.Y() - interval.X();
    shift = interval.X() + 0.5 * max;
  }
  return shift;
}

//=================================================================================================

// Detect missing natural boundary on spherical surfaces and add it if
// necessary
// pdn 981202: add natural bounds if missing (on sphere only)
//: abv 28.08.01: rewritten and extended for toruses

bool ShapeFix_Face::FixAddNaturalBound()
{
  if (mySurf.IsNull())
  {
    return false;
  }

  if (!Context().IsNull())
  {
    TopoDS_Shape S = Context()->Apply(myFace);
    myFace         = TopoDS::Face(S);
  }

  // collect wires in sequence
  NCollection_Sequence<TopoDS_Shape> ws;
  NCollection_Sequence<TopoDS_Shape> vs;
  TopoDS_Iterator                    wi(myFace, false);
  for (; wi.More(); wi.Next())
  {
    if (wi.Value().ShapeType() == TopAbs_WIRE
        && (wi.Value().Orientation() == TopAbs_FORWARD
            || wi.Value().Orientation() == TopAbs_REVERSED))
      ws.Append(wi.Value());
    else
      vs.Append(wi.Value());
  }

  // deal with the case of an empty face: just create a new face by a standard tool
  if (ws.IsEmpty() && !IsSurfaceUVInfinite(mySurf->Surface()))
  {
    BRepBuilderAPI_MakeFace aFaceBuilder(mySurf->Surface(), Precision::Confusion());

    TopoDS_Face aNewFace = aFaceBuilder.Face();
    aNewFace.Orientation(myFace.Orientation());

    if (!Context().IsNull())
      Context()->Replace(myFace, aNewFace);

    // taking into account orientation
    myFace = aNewFace;

    // gka 11.01.99 file PRO7755.stp entity #2018 surface #1895: error BRepLib_MakeFace func
    // IsDegenerated
    occ::handle<ShapeFix_Edge> sfe = myFixWire->FixEdgeTool();
    for (TopExp_Explorer Eed(myFace, TopAbs_EDGE); Eed.More(); Eed.Next())
    {
      TopoDS_Edge edg = TopoDS::Edge(Eed.Current());
      sfe->FixVertexTolerance(edg, myFace);
    }

    //    B.UpdateFace (myFace,myPrecision);
    // clang-format off
    SendWarning ( myFace, Message_Msg ( "FixAdvFace.FixOrientation.MSG0" ) );// Face created with natural bounds
    // clang-format on
    BRepTools::Update(myFace);
    myResult = myFace;
    return true;
  }

  // check if surface doesn't need natural bounds
  if (!isNeedAddNaturalBound(ws))
  {
    return false;
  }

  // Collect information on free intervals in U and V
  NCollection_Sequence<gp_Pnt2d> intU, intV, centers;
  double                         SUF, SUL, SVF, SVL;
  mySurf->Bounds(SUF, SUL, SVF, SVL);
  intU.Append(gp_Pnt2d(SUF, SUL));
  intV.Append(gp_Pnt2d(SVF, SVL));
  int nb = ws.Length();
  int i;

  for (i = 1; i <= nb; i++)
  {
    double Umin, Vmin, Umax, Vmax;
    //     Bnd_Box2d B;
    TopoDS_Wire  aw        = TopoDS::Wire(ws.Value(i));
    TopoDS_Face  aWireFace = TopoDS::Face(myFace.EmptyCopied());
    BRep_Builder aB;
    aB.Add(aWireFace, aw);
    ShapeAnalysis::GetFaceUVBounds(aWireFace, Umin, Umax, Vmin, Vmax);

    // PTV 01.11.2002 ACIS907, OCC921 end
    if (mySurf->IsUClosed())
      CutInterval(intU, gp_Pnt2d(Umin, Umax), SUL - SUF);
    if (mySurf->IsVClosed())
      CutInterval(intV, gp_Pnt2d(Vmin, Vmax), SVL - SVF);
    centers.Append(gp_Pnt2d(0.5 * (Umin + Umax), 0.5 * (Vmin + Vmax)));
  }

  // find best interval and thus compute shift
  gp_Pnt2d shift(0., 0.);
  if (mySurf->IsUClosed())
    shift.SetX(FindBestInterval(intU));
  if (mySurf->IsVClosed())
    shift.SetY(FindBestInterval(intV));

  // Adjust all other wires to be inside outer one
  gp_Pnt2d center(shift.X() + 0.5 * (SUL - SUF), shift.Y() + 0.5 * (SVL - SVF));
  for (i = 1; i <= nb; i++)
  {
    TopoDS_Wire wire = TopoDS::Wire(ws.Value(i));
    gp_Pnt2d    sh(0., 0.);
    if (mySurf->IsUClosed())
      sh.SetX(ShapeAnalysis::AdjustByPeriod(centers(i).X(), center.X(), SUL - SUF));
    if (mySurf->IsVClosed())
      sh.SetY(ShapeAnalysis::AdjustByPeriod(centers(i).Y(), center.Y(), SVL - SVF));
    Shift2dWire(wire, myFace, sh.XY(), mySurf);
  }

  // Create naturally bounded surface and add that wire to sequence
  TopLoc_Location           L;
  occ::handle<Geom_Surface> surf = BRep_Tool::Surface(myFace, L);
  BRepBuilderAPI_MakeFace   mf(surf, Precision::Confusion());
  TopoDS_Face               ftmp = mf.Face();
  ftmp.Location(L);
  for (wi.Initialize(ftmp, false); wi.More(); wi.Next())
  {
    if (wi.Value().ShapeType() != TopAbs_WIRE)
      continue;
    TopoDS_Wire wire = TopoDS::Wire(wi.Value());
    ws.Append(wire);
    if (shift.XY().Modulus() < ::Precision::PConfusion())
      continue;
    Shift2dWire(wire, myFace, shift.XY(), mySurf, true);
  }

  // Fix possible case on sphere when gap contains degenerated edge
  // and thus has a common part with natural boundary
  // Such hole should be merged with boundary
  if (mySurf->Adaptor3d()->GetType() == GeomAbs_Sphere && ws.Length() == nb + 1)
  {
    occ::handle<ShapeExtend_WireData> bnd = new ShapeExtend_WireData(TopoDS::Wire(ws.Last()));
    // code to become separate method FixTouchingWires()
    for (i = 1; i <= nb; i++)
    {
      occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData(TopoDS::Wire(ws.Value(i)));
      for (int j = 1; j <= sbwd->NbEdges(); j++)
      {
        if (!BRep_Tool::Degenerated(sbwd->Edge(j)))
          continue;
        // find corresponding place in boundary
        ShapeAnalysis_Edge sae;
        TopoDS_Vertex      V = sae.FirstVertex(sbwd->Edge(j));
        int                k;
        for (k = 1; k <= bnd->NbEdges(); k++)
        {
          if (!BRep_Tool::Degenerated(bnd->Edge(k)))
            continue;
          if (BRepTools::Compare(V, sae.FirstVertex(bnd->Edge(k))))
            break;
        }
        if (k > bnd->NbEdges())
          continue;
        // and insert hole to that place
        BRep_Builder B;
        B.Degenerated(sbwd->Edge(j), false);
        B.Degenerated(bnd->Edge(k), false);
        sbwd->SetLast(j);
        bnd->Add(sbwd, k + 1);
        ws.Remove(i--);
        nb--;
        myFixWire->SetFace(myFace, mySurf);
        myFixWire->Load(bnd);
        myFixWire->FixConnected();
        myFixWire->FixDegenerated();
        ws.SetValue(ws.Length(), bnd->Wire());
        break;
      }
    }
  }

  // Create resulting face
  BRep_Builder B;
  TopoDS_Shape S = myFace.EmptyCopied();
  S.Orientation(TopAbs_FORWARD);
  for (i = 1; i <= ws.Length(); i++)
    B.Add(S, ws.Value(i));
  for (i = 1; i <= vs.Length(); i++)
    B.Add(S, vs.Value(i));
  if (!myFwd)
    S.Orientation(TopAbs_REVERSED);
  if (!Context().IsNull())
    Context()->Replace(myFace, S);
  myFace = TopoDS::Face(S);
  BRepTools::Update(myFace);

  // clang-format off
  SendWarning ( myFace, Message_Msg ( "FixAdvFace.FixOrientation.MSG0" ) );// Face created with natural bounds
  // clang-format on
  return true;
}

//=================================================================================================

bool ShapeFix_Face::FixOrientation()
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    MapWires;
  MapWires.Clear();
  return FixOrientation(MapWires);
}

//=================================================================================================

bool ShapeFix_Face::isNeedAddNaturalBound(
  const NCollection_Sequence<TopoDS_Shape>& theOrientedWires)
{
  // if fix is not needed
  if (!NeedFix(myFixAddNaturalBoundMode))
  {
    return false;
  }
  // if surface is not double-closed
  if (mySurf && !IsSurfaceUVPeriodic(mySurf->Adaptor3d()))
  {
    return false;
  }
  // if face has an OUTER bound
  if (ShapeAnalysis::IsOuterBound(myFace))
  {
    return false;
  }
  // check that not any wire has a seam edge and not any edge is degenerated.
  // because the presence of a seam or degenerated edge indicates that this wire should be an
  // external one, and in case of its incorrect orientation, this will be corrected.
  int aNbOriented = theOrientedWires.Length();
  for (int i = 1; i <= aNbOriented; i++)
  {
    TopoDS_Wire aWire = TopoDS::Wire(theOrientedWires.Value(i));
    for (TopoDS_Iterator anEdgeIt(aWire); anEdgeIt.More(); anEdgeIt.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdgeIt.Value());
      if (BRep_Tool::Degenerated(anEdge))
      {
        return false;
      }
      if (BRep_Tool::IsClosed(anEdge, myFace))
      {
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================

bool ShapeFix_Face::FixOrientation(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    MapWires)
{
  bool done = false;

  if (!Context().IsNull())
  {
    TopoDS_Shape S = Context()->Apply(myFace);
    myFace         = TopoDS::Face(S);
  }
  NCollection_Sequence<TopoDS_Shape> ws;
  NCollection_Sequence<TopoDS_Shape> allSubShapes;
  // smh: BUC60810 : protection against very small wires (one-edge, null-length)
  NCollection_Sequence<TopoDS_Shape> VerySmallWires;
  for (TopoDS_Iterator wi(myFace, false); wi.More(); wi.Next())
  {
    if (wi.Value().ShapeType() == TopAbs_VERTEX
        || (wi.Value().Orientation() != TopAbs_FORWARD
            && wi.Value().Orientation() != TopAbs_REVERSED))
    {
      allSubShapes.Append(wi.Value());
      // ws.Append (wi.Value());
      continue;
    }

    TopoDS_Iterator ei(wi.Value(), false);
    TopoDS_Edge     anEdge;
    double          length = RealLast();
    if (ei.More())
    {
      anEdge = TopoDS::Edge(ei.Value());
      ei.Next();
      if (!ei.More())
      {
        length = 0;
        double                  First, Last;
        occ::handle<Geom_Curve> c3d;
        ShapeAnalysis_Edge      sae;
        if (sae.Curve3d(anEdge, c3d, First, Last))
        {
          gp_Pnt pntIni = c3d->Value(First);
          gp_XYZ prev;
          prev          = pntIni.XYZ();
          int NbControl = 10;
          for (int j = 1; j < NbControl; j++)
          {
            double prm     = ((NbControl - 1 - j) * First + j * Last) / (NbControl - 1);
            gp_Pnt pntCurr = c3d->Value(prm);
            gp_XYZ curr    = pntCurr.XYZ();
            gp_XYZ delta   = curr - prev;
            length += delta.Modulus();
            prev = curr;
          }
        }
      }
    }
    else
      length = 0;
    if (length > ::Precision::Confusion())
    {
      ws.Append(wi.Value());
      allSubShapes.Append(wi.Value());
    }
    else
      VerySmallWires.Append(wi.Value());
  }
  if (VerySmallWires.Length() > 0)
    done = true;

  int          nb    = ws.Length();
  int          nbAll = allSubShapes.Length();
  BRep_Builder B;

  // if no wires, just do nothing
  if (nb <= 0)
    return false;

  bool                      isAddNaturalBounds = isNeedAddNaturalBound(ws);
  NCollection_Sequence<int> aSeqReversed;
  // if wire is only one, check its orientation
  if (nb == 1)
  {
    // skl 12.04.2002 for cases with nbwires>1 (VerySmallWires>1)
    // make face with only one wire (ws.Value(1))
    TopoDS_Shape dummy = myFace.EmptyCopied();
    TopoDS_Face  af    = TopoDS::Face(dummy);
    af.Orientation(TopAbs_FORWARD);
    B.Add(af, ws.Value(1));

    if (!isAddNaturalBounds && !ShapeAnalysis::IsOuterBound(af))
    {
      occ::handle<ShapeExtend_WireData> sbdw = new ShapeExtend_WireData(TopoDS::Wire(ws.Value(1)));
      sbdw->Reverse(myFace);
      ws.SetValue(1, sbdw->Wire());
      // clang-format off
      SendWarning(sbdw->Wire(), Message_Msg("FixAdvFace.FixOrientation.MSG5"));// Wire on face was reversed
      // clang-format on
      done = true;
    }
  }
  // in case of several wires, perform complex analysis
  // TEST CAUTION
  // Multiple wires: relative orientations
  // Each wire must "contain" all the others
  // Obviously, in the case of leopard skin, there may be a problem
  else if (!mySurf.IsNull())
  {
    // Take each wire (NB: curves present!)
    // In principle, we should reject unclosed wires (see missing seam?)
    // We classify it relative to the others, which must all be either IN or
    // OUT. Otherwise, there is nesting -> SDB. If IN, OK, if OUT, we reverse
    // (NB: not myClos here, so no stitching problem)
    // If there is at least one inversion, the face must be redone (see myRebil)
    bool   uclosed = mySurf->IsUClosed();
    bool   vclosed = mySurf->IsVClosed();
    double SUF, SUL, SVF, SVL;
    mySurf->Bounds(SUF, SUL, SVF, SVL);
    double uRange = SUL - SUF;
    double vRange = SVL - SVF;

    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> MW;
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>                            SI;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapIntWires;
    MW.Clear();
    SI.Clear();
    MapIntWires.Clear();
    int i;

    NCollection_Array1<Bnd_Box2d> aWireBoxes(1, nb);
    double                        uMiddle = 0, vMiddle = 0;
    bool                          isFirst = true;
    // create Bounding boxes for each wire
    for (i = 1; i <= nb; i++)
    {
      TopoDS_Shape    aShape = ws.Value(i);
      TopoDS_Wire     aWire  = TopoDS::Wire(aShape);
      Bnd_Box2d       aBox;
      double          cf, cl;
      TopoDS_Iterator ew(aWire);
      for (; ew.More(); ew.Next())
      {
        TopoDS_Edge               ed = TopoDS::Edge(ew.Value());
        occ::handle<Geom2d_Curve> cw = BRep_Tool::CurveOnSurface(ed, myFace, cf, cl);
        if (cw.IsNull())
        {
          continue;
        }
        Geom2dAdaptor_Curve gac;
        double              aFirst = cw->FirstParameter();
        double              aLast  = cw->LastParameter();
        if (cw->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) && (cf < aFirst || cl > aLast))
        {
          // avoiding problems with segment in Bnd_Box
          gac.Load(cw);
        }
        else
          gac.Load(cw, cf, cl);
        BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), aBox);
      }

      double aXMin, aXMax, aYMin, aYMax;
      aBox.Get(aXMin, aYMin, aXMax, aYMax);
      if (isFirst)
      {
        isFirst = false;
        uMiddle = (aXMin + aXMax) * 0.5;
        vMiddle = (aYMin + aYMax) * 0.5;
      }
      else
      {
        double xShift = 0, yShift = 0;
        if (mySurf->IsUClosed())
          xShift = ShapeAnalysis::AdjustByPeriod(0.5 * (aXMin + aXMax), uMiddle, uRange);
        if (mySurf->IsVClosed())
          yShift = ShapeAnalysis::AdjustByPeriod(0.5 * (aYMin + aYMax), vMiddle, vRange);
        aBox.Update(aXMin + xShift, aYMin + yShift, aXMax + xShift, aYMax + yShift);
      }
      aWireBoxes.ChangeValue(i) = aBox;
    }

    for (i = 1; i <= nb; i++)
    {
      TopoDS_Shape asw   = ws.Value(i);
      TopoDS_Wire  aw    = TopoDS::Wire(asw);
      Bnd_Box2d    aBox1 = aWireBoxes.Value(i);
      TopoDS_Shape dummy = myFace.EmptyCopied();
      TopoDS_Face  af    = TopoDS::Face(dummy);
      af.Orientation(TopAbs_FORWARD);
      B.Add(af, aw);
      // PTV OCC945 06.11.2002 files ie_exhaust-A.stp (entities 3782,  3787)
      // tolerance is too big. It is seems that to identify placement of 2d point
      // it is enough Precision::PConfusion(), cause wea re know that 2d point in TopAbs_ON
      // BRepTopAdaptor_FClass2d clas (af,toluv);
      bool                           CheckShift = true;
      BRepTopAdaptor_FClass2d        clas(af, ::Precision::PConfusion());
      TopAbs_State                   sta    = TopAbs_OUT;
      TopAbs_State                   staout = clas.PerformInfinitePoint();
      NCollection_List<TopoDS_Shape> IntWires;
      int                            aWireIt = 0;
      for (int j = 1; j <= nbAll; j++)
      {
        aWireIt++;
        // if(i==j) continue;
        TopoDS_Shape aSh2 = allSubShapes.Value(j);
        if (aw == aSh2)
          continue;
        TopAbs_State stb = TopAbs_UNKNOWN;
        if (aSh2.ShapeType() == TopAbs_VERTEX)
        {
          aWireIt--;
          gp_Pnt   aP  = BRep_Tool::Pnt(TopoDS::Vertex(aSh2));
          gp_Pnt2d p2d = mySurf->ValueOfUV(aP, Precision::Confusion());
          stb          = clas.Perform(p2d, false);
          if (stb == staout && (uclosed || vclosed))
          {
            gp_Pnt2d p2d1;
            if (uclosed)
            {
              p2d1.SetCoord(p2d.X() + uRange, p2d.Y());
              stb = clas.Perform(p2d1, false);
            }
            if (stb == staout && vclosed)
            {
              p2d1.SetCoord(p2d.X(), p2d.Y() + vRange);
              stb = clas.Perform(p2d1, false);
            }
          }
        }
        else if (aSh2.ShapeType() == TopAbs_WIRE)
        {
          CheckShift     = true;
          TopoDS_Wire bw = TopoDS::Wire(aSh2);
          // int numin =0;
          Bnd_Box2d aBox2 = aWireBoxes.Value(aWireIt);
          if (aBox2.IsOut(aBox1))
            continue;

          TopoDS_Iterator ew(bw);
          for (; ew.More(); ew.Next())
          {
            TopoDS_Edge               ed = TopoDS::Edge(ew.Value());
            double                    cf, cl;
            occ::handle<Geom2d_Curve> cw = BRep_Tool::CurveOnSurface(ed, myFace, cf, cl);
            if (cw.IsNull())
              continue;
            gp_Pnt2d     unp = cw->Value((cf + cl) / 2.);
            TopAbs_State ste = clas.Perform(unp, false);
            if (ste == TopAbs_OUT || ste == TopAbs_IN)
            {
              if (stb == TopAbs_UNKNOWN)
              {
                stb = ste;
              }
              else
              {
                if (!(stb == ste))
                {
                  sta = TopAbs_UNKNOWN;
                  SI.Bind(aw, 0);
                  j = nbAll;
                  break;
                }
              }
            }

            bool     found = false;
            gp_Pnt2d unp1;
            if (stb == staout && CheckShift)
            {
              CheckShift = false;
              if (uclosed)
              {
                unp1.SetCoord(unp.X() + uRange, unp.Y());
                found = (staout != clas.Perform(unp1, false));
                if (!found)
                {
                  unp1.SetX(unp.X() - uRange);
                  found = (staout != clas.Perform(unp1, false));
                }
              }
              if (vclosed && !found)
              {
                unp1.SetCoord(unp.X(), unp.Y() + vRange);
                found = (staout != clas.Perform(unp1, false));
                if (!found)
                {
                  unp1.SetY(unp.Y() - vRange);
                  found = (staout != clas.Perform(unp1, false));
                }
              }
              // Additional check of diagonal steps for toroidal surfaces
              if (!found && uclosed && vclosed)
              {
                for (double dX = -1.0; dX <= 1.0 && !found; dX += 2.0)
                  for (double dY = -1.0; dY <= 1.0 && !found; dY += 2.0)
                  {
                    unp1.SetCoord(unp.X() + uRange * dX, unp.Y() + vRange * dY);
                    found = (staout != clas.Perform(unp1, false));
                  }
              }
            }
            if (found)
            {
              if (stb == TopAbs_IN)
                stb = TopAbs_OUT;
              else
                stb = TopAbs_IN;
              Shift2dWire(bw, myFace, unp1.XY() - unp.XY(), mySurf);
            }
          }
        }
        if (stb == staout)
        {
          sta = TopAbs_IN;
        }
        else
        {
          IntWires.Append(aSh2);
          MapIntWires.Add(aSh2);
        }
      }

      if (sta == TopAbs_UNKNOWN)
      { // ERREUR
        // clang-format off
        SendWarning ( aw, Message_Msg ( "FixAdvFace.FixOrientation.MSG11" ) );// Cannot orient wire
        // clang-format on
      }
      else
      {
        MW.Bind(aw, IntWires);
        if (sta == TopAbs_OUT)
        {
          if (staout == TopAbs_IN)
          {
            // wire is OUT but InfinitePoint is IN => need to reverse
            ShapeExtend_WireData sewd(aw);
            sewd.Reverse(myFace);
            ws.SetValue(i, sewd.Wire());
            // clang-format off
            SendWarning ( sewd.Wire(), Message_Msg ( "FixAdvFace.FixOrientation.MSG5" ) );// Wire on face was reversed
            // clang-format on
            aSeqReversed.Append(i);
            done = true;
            SI.Bind(ws.Value(i), 1);
            MapWires.Bind(ws.Value(i), IntWires);
          }
          else
          {
            SI.Bind(aw, 1);
            MapWires.Bind(aw, IntWires);
          }
        }
        else
        {
          if (staout == TopAbs_OUT)
            SI.Bind(aw, 2);
          else
            SI.Bind(aw, 3);
        }
      }
    }

    for (i = 1; i <= nb; i++)
    {
      TopoDS_Wire aw   = TopoDS::Wire(ws.Value(i));
      int         tmpi = SI.Find(aw);
      if (tmpi > 1)
      {
        if (!MapIntWires.Contains(aw))
        {
          const NCollection_List<TopoDS_Shape>& IW = MW.Find(aw);
          if (tmpi == 3)
          {
            // wire is OUT but InfinitePoint is IN => need to reverse
            ShapeExtend_WireData sewd(aw);
            sewd.Reverse(myFace);
            ws.SetValue(i, sewd.Wire());
            // clang-format off
            SendWarning ( sewd.Wire(), Message_Msg ( "FixAdvFace.FixOrientation.MSG5" ) );// Wire on face was reversed
            // clang-format on
            aSeqReversed.Append(i);
            done = true;
            MapWires.Bind(ws.Value(i), IW);
          }
          else
            MapWires.Bind(aw, IW);
        }
        else
        {
          if (tmpi == 2)
          {
            // wire is IN but InfinitePoint is OUT => need to reverse
            ShapeExtend_WireData sewd(aw);
            sewd.Reverse(myFace);
            ws.SetValue(i, sewd.Wire());
            // clang-format off
            SendWarning ( sewd.Wire(), Message_Msg ( "FixAdvFace.FixOrientation.MSG5" ) );// Wire on face was reversed
            // clang-format on
            aSeqReversed.Append(i);
            done = true;
          }
        }
      }
    }
  }

  if (isAddNaturalBounds && nb == aSeqReversed.Length())
    done = false;

  // Should I rebuild? if myRebil is set
  if (done)
  {
    TopoDS_Shape S = myFace.EmptyCopied();
    S.Orientation(TopAbs_FORWARD);
    int i = 1;
    for (; i <= nb; i++)
      B.Add(S, ws.Value(i));

    if (nb < nbAll)
    {
      for (i = 1; i <= nbAll; i++)
      {
        const TopoDS_Shape& aS2 = allSubShapes.Value(i);
        if (aS2.ShapeType() != TopAbs_WIRE
            || (aS2.Orientation() != TopAbs_FORWARD && aS2.Orientation() != TopAbs_REVERSED))
          B.Add(S, aS2);
      }
    }

    if (!myFwd)
      S.Orientation(TopAbs_REVERSED);
    if (!Context().IsNull())
      Context()->Replace(myFace, S);
    myFace = TopoDS::Face(S);
    BRepTools::Update(myFace);
  }
  return done;
}

//=======================================================================
// function : CheckWire
// purpose  : auxiliary for FixMissingSeam
//=======================================================================
//: i7 abv 18 Sep 98: ProSTEP TR9 r0501-ug.stp: algorithm of fixing missing seam changed
// test whether the wire is opened on period of periodical surface
static bool CheckWire(const TopoDS_Wire& wire,
                      const TopoDS_Face& face,
                      const double       dU,
                      const double       dV,
                      int&               isuopen,
                      int&               isvopen,
                      bool&              isDeg)
{
  gp_XY vec;
  vec.SetX(0);
  vec.SetY(0);
  ShapeAnalysis_Edge sae;

  isuopen = isvopen = 0;
  isDeg             = true;
  for (TopoDS_Iterator ed(wire); ed.More(); ed.Next())
  {
    TopoDS_Edge edge = TopoDS::Edge(ed.Value());
    if (!BRep_Tool::Degenerated(edge))
      isDeg = false;
    occ::handle<Geom2d_Curve> c2d;
    double                    f, l;
    if (!sae.PCurve(edge, face, c2d, f, l, true))
      return false;
    vec += c2d->Value(l).XY() - c2d->Value(f).XY();
  }

  double aDelta = std::abs(vec.X()) - dU;
  if (std::abs(aDelta) < 0.1 * dU)
  {
    if (vec.X() > 0.0)
    {
      isuopen = 1;
    }
    else
    {
      isuopen = -1;
    }
  }
  else
  {
    isuopen = 0;
  }

  aDelta = std::abs(vec.Y()) - dV;
  if (std::abs(aDelta) < 0.1 * dV)
  {
    if (vec.Y() > 0.0)
    {
      isvopen = 1;
    }
    else
    {
      isvopen = -1;
    }
  }
  else
  {
    isvopen = 0;
  }

  return isuopen || isvopen;
}

//=================================================================================================

bool ShapeFix_Face::FixMissingSeam()
{
  if (mySurf.IsNull())
  {
    return false;
  }

  bool uclosed = mySurf->IsUClosed();
  bool vclosed = mySurf->IsVClosed();

  if (!uclosed && !vclosed)
    return false;

  if (!Context().IsNull())
  {
    TopoDS_Shape S = Context()->Apply(myFace);
    myFace         = TopoDS::Face(S);
  }

  //%pdn: surface should be made periodic before (see ShapeCustom_Surface)!
  if (mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    occ::handle<Geom_BSplineSurface> BSpl = occ::down_cast<Geom_BSplineSurface>(mySurf->Surface());
    if (!BSpl->IsUPeriodic() && !BSpl->IsVPeriodic())
      return false;
  }

  double URange, VRange, SUF, SUL, SVF, SVL;
  mySurf->Bounds(SUF, SUL, SVF, SVL);
  double fU1, fU2, fV1, fV2;
  BRepTools::UVBounds(myFace, fU1, fU2, fV1, fV2);

  // pdn OCC55 fix to faces without the wires to avoid identical first and last parameters
  if (::Precision::IsInfinite(SUF) || ::Precision::IsInfinite(SUL))
  {
    if (::Precision::IsInfinite(SUF))
      SUF = fU1;
    if (::Precision::IsInfinite(SUL))
      SUL = fU2;
    if (std::abs(SUL - SUF) < ::Precision::PConfusion())
    {
      if (::Precision::IsInfinite(SUF))
        SUF -= 1000.;
      else
        SUL += 1000.;
    }
  }
  if (::Precision::IsInfinite(SVF) || ::Precision::IsInfinite(SVL))
  {
    if (::Precision::IsInfinite(SVF))
      SVF = fV1;
    if (::Precision::IsInfinite(SVL))
      SVL = fV2;
    if (std::abs(SVL - SVF) < ::Precision::PConfusion())
    {
      if (::Precision::IsInfinite(SVF))
        SVF -= 1000.;
      else
        SVL += 1000.;
    }
  }

  URange = std::min(std::abs(SUL - SUF), Precision::Infinite());
  VRange = std::min(std::abs(SVL - SVF), Precision::Infinite());
  //  double UTol = 0.2 * URange, VTol = 0.2 * VRange;
  int ismodeu = 0, ismodev = 0; // szv#4:S4163:12Mar99 was Boolean
  int isdeg1 = 0, isdeg2 = 0;

  NCollection_Sequence<TopoDS_Shape> ws;
  NCollection_Sequence<TopoDS_Shape> aSeqNonManif;
  for (TopoDS_Iterator wi(myFace, false); wi.More(); wi.Next())
  {
    if (wi.Value().ShapeType() != TopAbs_WIRE
        || (wi.Value().Orientation() != TopAbs_FORWARD
            && wi.Value().Orientation() != TopAbs_REVERSED))
    {
      aSeqNonManif.Append(wi.Value());
      continue;
    }
    ws.Append(wi.Value());
  }

  TopoDS_Wire w1, w2;
  int         i;
  for (i = 1; i <= ws.Length(); i++)
  {
    TopoDS_Wire wire = TopoDS::Wire(ws.Value(i));
    int         isuopen, isvopen;
    bool        isdeg;
    if (!CheckWire(wire, myFace, URange, VRange, isuopen, isvopen, isdeg))
      continue;
    if (w1.IsNull())
    {
      w1      = wire;
      ismodeu = isuopen;
      ismodev = isvopen;
      isdeg1  = isdeg ? i : 0;
    }
    else if (w2.IsNull())
    {
      if (ismodeu == -isuopen && ismodev == -isvopen)
      {
        w2     = wire;
        isdeg2 = isdeg ? i : 0;
      }
      else if (ismodeu == isuopen && ismodev == isvopen)
      {
        w2     = wire;
        isdeg2 = isdeg;
        //: abv 29.08.01: If wires are contraversal, reverse one of them
        // If first one is single degenerated edge, reverse it; else second
        if (isdeg1)
        {
          w1.Reverse();
          ismodeu = -ismodeu;
          ismodev = -ismodev;
        }
        else
        {
          w2.Reverse();
        }
      }
    }
    //    else return false; //  abort
    else
    {
      //: abv 30.08.09: if more than one open wires and more than two of them are
      // completely degenerated, remove any of them
      if (isdeg || isdeg1 || isdeg2)
      {
        ws.Remove(isdeg ? i : isdeg2 ? isdeg2 : isdeg1);
        w1.Nullify();
        w2.Nullify();
        i = 0;
        continue;
      }
    }
  }

  BRep_Builder                      B;
  occ::handle<Geom_ToroidalSurface> aTorSurf =
    occ::down_cast<Geom_ToroidalSurface>(mySurf->Surface());
  bool anIsDegeneratedTor =
    (aTorSurf.IsNull() ? false : aTorSurf->MajorRadius() < aTorSurf->MinorRadius());
  // if the second wire is not null, we don't need mark the torus as degenerated
  // and should process it as a regular one.
  if (anIsDegeneratedTor && !w2.IsNull())
  {
    anIsDegeneratedTor = false;
  }

  if (w1.IsNull())
    return false;
  else if (w2.IsNull())
  {
    // For spheres and BSpline cone-like surfaces(bug 24055):
    // If only one of wires limiting face on surface is open in 2d,
    // this may means that degenerated edge should be added, and
    // then usual procedure applied
    gp_Pnt2d p;
    gp_Dir2d d;
    double   aRange;

    if (ismodeu && anIsDegeneratedTor)
    {
      double aRa  = aTorSurf->MajorRadius();
      double aRi  = aTorSurf->MinorRadius();
      double aPhi = std::acos(-aRa / aRi);
      p.SetCoord(0.0, (ismodeu > 0 ? M_PI + aPhi : aPhi));

      double aXCoord = -ismodeu;
      d.SetCoord(aXCoord, 0.);
      aRange = 2. * M_PI;
    }
    else if (ismodeu && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    {
      p.SetCoord((ismodeu < 0 ? 0. : 2. * M_PI), ismodeu * 0.5 * M_PI);
      double aXCoord = -ismodeu;
      d.SetCoord(aXCoord, 0.);
      aRange = 2. * M_PI;
    }
    else if (ismodev && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      double uCoord;
      if (mySurf->Value(SUF, SVF).Distance(mySurf->Value(SUF, (SVF + SVL) / 2))
          < ::Precision::Confusion())
        uCoord = SUF;
      else if (mySurf->Value(SUL, SVF).Distance(mySurf->Value(SUL, (SVF + SVL) / 2))
               < ::Precision::Confusion())
        uCoord = SUL;
      else
        return false;

      p.SetCoord(uCoord, (ismodev < 0 ? 0. : VRange));
      d.SetCoord(0., -ismodev);
      aRange = VRange;
    }
    else if (ismodeu && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      double vCoord;
      if (mySurf->Value(SUF, SVF).Distance(mySurf->Value((SUF + SUL) / 2, SVF))
          < ::Precision::Confusion())
        vCoord = SVF;
      else if (mySurf->Value(SUL, SVL).Distance(mySurf->Value((SUF + SUL) / 2, SVL))
               < ::Precision::Confusion())
        vCoord = SVL;
      else
        return false;

      p.SetCoord((ismodeu < 0 ? 0. : URange), vCoord);
      double aXCoord = -ismodeu;
      d.SetCoord(aXCoord, 0.);
      aRange = URange;
    }
    else
      return false;

    occ::handle<Geom2d_Line> line = new Geom2d_Line(p, d);
    TopoDS_Edge              edge;
    B.MakeEdge(edge);
    B.Degenerated(edge, true);
    B.UpdateEdge(edge, line, myFace, ::Precision::Confusion());
    B.Range(edge, myFace, 0., aRange);
    TopoDS_Vertex V;
    B.MakeVertex(V, mySurf->Value(p.X(), p.Y()), ::Precision::Confusion());
    V.Orientation(TopAbs_FORWARD);
    B.Add(edge, V);
    V.Orientation(TopAbs_REVERSED);
    B.Add(edge, V);
    B.MakeWire(w2);
    B.Add(w2, edge);
    ws.Append(w2);
  }

  // Check consistency of orientations of the two wires that need to be connected by a seam
  double       uf = SUF, vf = SVF;
  int          coord  = (ismodeu ? 1 : 0);
  int          isneg  = (ismodeu ? ismodeu : -ismodev);
  double       period = (ismodeu ? URange : VRange);
  TopoDS_Shape S;
  double       m1[2][2], m2[2][2];
  S = myFace.EmptyCopied();
  B.Add(S, w1);
  ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(S), m1[0][0], m1[0][1], m1[1][0], m1[1][1]);
  S = myFace.EmptyCopied();
  B.Add(S, w2);
  ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(S), m2[0][0], m2[0][1], m2[1][0], m2[1][1]);

  // For the case when surface is closed only in one direction it is necessary to check
  // validity of orientation of the open wires in parametric space.
  // In case of U closed surface wire with minimal V coordinate should be directed in positive
  // direction by U In case of V closed surface wire with minimal U coordinate should be directed in
  // negative direction by V
  if (!vclosed || !uclosed || anIsDegeneratedTor)
  {
    double deltaOther = 0.5 * (m2[coord][0] + m2[coord][1]) - 0.5 * (m1[coord][0] + m1[coord][1]);
    if (deltaOther * isneg < 0)
    {
      w1.Reverse();
      w2.Reverse();
    }
  }

  // sort original wires
  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire;
  sfw->SetFace(myFace, mySurf);
  sfw->SetPrecision(Precision());
  occ::handle<ShapeExtend_WireData> wd1 = new ShapeExtend_WireData(w1);
  occ::handle<ShapeExtend_WireData> wd2 = new ShapeExtend_WireData(w2);
  sfw->Load(wd1);
  sfw->FixReorder();
  sfw->Load(wd2);
  sfw->FixReorder();
  TopoDS_Wire w11 = wd1->Wire();
  TopoDS_Wire w21 = wd2->Wire();

  //: abv 29.08.01: reconstruct face taking into account reversing
  TopoDS_Shape dummy = myFace.EmptyCopied();
  TopoDS_Face  tmpF  = TopoDS::Face(dummy);
  tmpF.Orientation(TopAbs_FORWARD);
  for (i = 1; i <= ws.Length(); i++)
  {
    TopoDS_Wire wire = TopoDS::Wire(ws.Value(i));
    if (wire.IsSame(w1))
      wire = w11;
    else if (wire.IsSame(w2))
      wire = w21;
    else
    {
      // other wires (not boundary) are considered as holes; make sure to have them oriented
      // accordingly
      TopoDS_Shape curface = tmpF.EmptyCopied();
      B.Add(curface, wire);
      curface.Orientation(myFace.Orientation());
      if (ShapeAnalysis::IsOuterBound(TopoDS::Face(curface)))
        wire.Reverse();
    }
    B.Add(tmpF, wire);
  }

  tmpF.Orientation(myFace.Orientation());

  // A special kind of FixShifted is necessary for torus-like
  // surfaces to adjust wires by period ALONG the missing SEAM direction
  // tr9_r0501-ug.stp #187640
  if (uclosed && vclosed && !anIsDegeneratedTor)
  {
    double shiftw2 = ShapeAnalysis::AdjustByPeriod(
      0.5 * (m2[coord][0] + m2[coord][1]),
      0.5 * (m1[coord][0] + m1[coord][1] + isneg * (period + ::Precision::PConfusion())),
      period);
    m1[coord][0] = std::min(m1[coord][0], m2[coord][0] + shiftw2);
    m1[coord][1] = std::max(m1[coord][1], m2[coord][1] + shiftw2);
    for (TopoDS_Iterator it(tmpF, false); it.More(); it.Next())
    {
      if (it.Value().ShapeType() != TopAbs_WIRE)
        continue;
      TopoDS_Wire w = TopoDS::Wire(it.Value());
      if (w == w11)
        continue;
      double shift;
      if (w == w21)
        shift = shiftw2;

      else
      {
        S = tmpF.EmptyCopied();
        B.Add(S, w);
        ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(S), m2[0][0], m2[0][1], m2[1][0], m2[1][1]);
        shift = ShapeAnalysis::AdjustByPeriod(0.5 * (m2[coord][0] + m2[coord][1]),
                                              0.5 * (m1[coord][0] + m1[coord][1]),
                                              period);
      }
      if (shift != 0.)
      {
        gp_Vec2d V(0., 0.);
        V.SetCoord(coord + 1, shift);
        ShapeAnalysis_Edge sae;
        for (TopoDS_Iterator iw(w); iw.More(); iw.Next())
        {
          TopoDS_Edge               E = TopoDS::Edge(iw.Value());
          occ::handle<Geom2d_Curve> C;
          double                    a, b;
          if (!sae.PCurve(E, tmpF, C, a, b))
            continue;
          C->Translate(V);
        }
      }
    }
    // abv 05 Feb 02: OCC34
    // by the way, select proper split place by V to avoid extra intersections
    if (m1[coord][1] - m1[coord][0] <= period)
    {
      double other = 0.5 * (m1[coord][0] + m1[coord][1] - period);
      if (ismodeu)
        vf = other;
      else
        uf = other;
    }
  }

  // find the best place by u and v to insert a seam
  // (so as to minimize splitting edges as possible)
  ShapeAnalysis_Edge sae;
  int                foundU = 0, foundV = 0;
  int                nb1 = wd1->NbEdges();
  int                nb2 = wd2->NbEdges();
  for (int i1 = 1; i1 <= nb1 + nb2; i1++)
  {
    TopoDS_Edge               edge1 = (i1 <= nb1 ? wd1->Edge(i1) : wd2->Edge(i1 - nb1));
    occ::handle<Geom2d_Curve> c2d;
    double                    f, l;
    if (!sae.PCurve(edge1, tmpF, c2d, f, l, true))
      return false;
    gp_Pnt2d pos1 = c2d->Value(l).XY();
    // the best place is end of edge which is nearest to 0
    bool skipU = !uclosed;
    if (uclosed && ismodeu)
    {
      pos1.SetX(pos1.X() + ShapeAnalysis::AdjustByPeriod(pos1.X(), SUF, URange));
      if (foundU == 2 && std::abs(pos1.X()) > std::abs(uf))
        skipU = true;
      else if (!foundU || (foundU == 1 && std::abs(pos1.X()) < std::abs(uf)))
      {
        foundU = 1;
        uf     = pos1.X();
      }
    }
    bool skipV = !vclosed;
    if (vclosed && !ismodeu)
    {
      pos1.SetY(pos1.Y() + ShapeAnalysis::AdjustByPeriod(pos1.Y(), SVF, VRange));
      if (foundV == 2 && std::abs(pos1.Y()) > std::abs(vf))
        skipV = true;
      else if (!foundV || (foundV == 1 && std::abs(pos1.Y()) < std::abs(vf)))
      {
        foundV = 1;
        vf     = pos1.Y();
      }
    }
    if (skipU && skipV)
    {
      if (i1 <= nb1)
        continue;
      else
        break;
    }
    // or yet better - if it is end of some edges on both wires
    for (int i2 = 1; i1 <= nb1 && i2 <= nb2; i2++)
    {
      TopoDS_Edge edge2 = wd2->Edge(i2);
      if (!sae.PCurve(edge2, tmpF, c2d, f, l, true))
        return false;
      gp_Pnt2d pos2 = c2d->Value(f).XY();
      if (uclosed && ismodeu)
      {
        pos2.SetX(pos2.X() + ShapeAnalysis::AdjustByPeriod(pos2.X(), pos1.X(), URange));
        if (std::abs(pos2.X() - pos1.X()) < ::Precision::PConfusion()
            && (foundU != 2 || std::abs(pos1.X()) < std::abs(uf)))
        {
          foundU = 2;
          uf     = pos1.X();
        }
      }
      if (vclosed && !ismodeu)
      {
        pos2.SetY(pos2.Y() + ShapeAnalysis::AdjustByPeriod(pos2.Y(), pos1.Y(), VRange));
        if (std::abs(pos2.Y() - pos1.Y()) < ::Precision::PConfusion()
            && (foundV != 2 || std::abs(pos1.Y()) < std::abs(vf)))
        {
          foundV = 2;
          vf     = pos1.Y();
        }
      }
    }
  }

  // pdn fixing RTS on offsets
  if (uf < SUF || uf > SUL)
    uf += ShapeAnalysis::AdjustToPeriod(uf, SUF, SUF + URange);
  if (vf < SVF || vf > SVL)
    vf += ShapeAnalysis::AdjustToPeriod(vf, SVF, SVF + VRange);

  // Create fictive grid and call ComposeShell to insert a seam
  occ::handle<Geom_RectangularTrimmedSurface> RTS =
    new Geom_RectangularTrimmedSurface(mySurf->Surface(), uf, uf + URange, vf, vf + VRange);
  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> grid =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
  grid->SetValue(1, 1, RTS); // mySurf->Surface() );
  occ::handle<ShapeExtend_CompositeSurface> G = new ShapeExtend_CompositeSurface(grid);
  TopLoc_Location                           L;

  // addition non-manifold topology
  int j = 1;
  for (; j <= aSeqNonManif.Length(); j++)
    B.Add(tmpF, aSeqNonManif.Value(j));

  ShapeFix_ComposeShell CompShell;
  CompShell.Init(G, L, tmpF, ::Precision::Confusion()); // myPrecision
  if (Context().IsNull())
    SetContext(new ShapeBuild_ReShape);
  CompShell.ClosedMode() = true;
  CompShell.SetContext(Context());
  CompShell.SetMaxTolerance(MaxTolerance());
  CompShell.Perform();

  // abv 03.07.00: CAX-IF TRJ4: trj4_k1_goe-tu-214.stp: #785: reset mySurf
  mySurf = new ShapeAnalysis_Surface(RTS);

  myResult = CompShell.Result();

  Context()->Replace(myFace, myResult);

  // Remove small wires and / or faces that can be generated by ComposeShell
  // (see tests bugs step bug30052_4, de step_3 E6)
  int             nbFaces = 0;
  TopExp_Explorer expF(myResult, TopAbs_FACE);
  for (; expF.More(); expF.Next())
  {
    TopoDS_Face     aFace = TopoDS::Face(expF.Value());
    TopExp_Explorer aExpW(aFace, TopAbs_WIRE);
    int             nbWires = 0;
    for (; aExpW.More(); aExpW.Next())
    {
      ShapeFix_Wire aSfw(TopoDS::Wire(aExpW.Value()), aFace, Precision());
      aSfw.SetContext(Context());
      if (aSfw.NbEdges())
        aSfw.FixSmall(true, Precision());
      if (!aSfw.NbEdges())
      {
        Context()->Remove(aExpW.Value());
        continue;
      }
      nbWires++;
    }
    if (!nbWires)
    {
      Context()->Remove(aFace);
      continue;
    }
    nbFaces++;
  }

  myResult = Context()->Apply(myResult);
  for (TopExp_Explorer exp(myResult, TopAbs_FACE); exp.More(); exp.Next())
  {
    myFace = TopoDS::Face(Context()->Apply(exp.Current()));
    if (myFace.IsNull())
      continue;
    if (nbFaces > 1)
    {
      FixSmallAreaWire(true);
      TopoDS_Shape aShape = Context()->Apply(myFace);
      if (aShape.IsNull())
        continue;
      myFace = TopoDS::Face(aShape);
    }
    BRepTools::Update(myFace); //: p4
  }
  myResult = Context()->Apply(myResult);

  SendWarning(Message_Msg("FixAdvFace.FixMissingSeam.MSG0")); // Missing seam-edge added
  return true;
}

//=================================================================================================

//%14 pdn 24.02.99 PRO10109, USA60293 fix wire on face with small area.
bool ShapeFix_Face::FixSmallAreaWire(const bool theIsRemoveSmallFace)
{
  if (!Context().IsNull())
  {
    TopoDS_Shape aShape = Context()->Apply(myFace);
    myFace              = TopoDS::Face(aShape);
  }

  BRep_Builder aBuilder;
  int          nbRemoved = 0, nbWires = 0;

  TopoDS_Shape anEmptyCopy = myFace.EmptyCopied();
  TopoDS_Face  aFace       = TopoDS::Face(anEmptyCopy);
  aFace.Orientation(TopAbs_FORWARD);

  const double aTolerance3d = ShapeFix_Root::Precision();
  for (TopoDS_Iterator aWIt(myFace, false); aWIt.More(); aWIt.Next())
  {
    const TopoDS_Shape& aShape = aWIt.Value();
    if (aShape.ShapeType() != TopAbs_WIRE && aShape.Orientation() != TopAbs_FORWARD
        && aShape.Orientation() != TopAbs_REVERSED)
    {
      continue;
    }

    const TopoDS_Wire&              aWire = TopoDS::Wire(aShape);
    occ::handle<ShapeAnalysis_Wire> anAnalyzer =
      new ShapeAnalysis_Wire(aWire, myFace, aTolerance3d);
    if (anAnalyzer->CheckSmallArea(aWire))
    {
      // Null area wire detected, wire skipped
      SendWarning(aWire, Message_Msg("FixAdvFace.FixSmallAreaWire.MSG0"));
      ++nbRemoved;
    }
    else
    {
      aBuilder.Add(aFace, aWire);
      ++nbWires;
    }
  }

  if (nbRemoved <= 0)
    return false;

  if (nbWires <= 0)
  {
    if (theIsRemoveSmallFace && !Context().IsNull())
      Context()->Remove(myFace);

    return false;
  }
  aFace.Orientation(myFace.Orientation());
  if (!Context().IsNull())
    Context()->Replace(myFace, aFace);

  myFace = aFace;
  return true;
}

//=================================================================================================

static void FindNext(
  const TopoDS_Shape&                                            aVert,
  const TopoDS_Shape&                                            ainitEdge,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapVertices,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                aMapVertexEdges,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSmallEdges,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSeemEdges,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       aMapEdges,
  occ::handle<ShapeExtend_WireData>&                            aWireData)
{
  TopoDS_Iterator aItV(ainitEdge);
  TopoDS_Shape    anextVert = aVert;
  bool            isFind    = false;
  for (; aItV.More() && !isFind; aItV.Next())
  {
    if (!aItV.Value().IsSame(aVert))
    {
      isFind    = true;
      anextVert = aItV.Value();
    }
  }

  if (!isFind && !aMapSmallEdges.Contains(ainitEdge))
    return;
  if (isFind && aMapVertices.Contains(anextVert))
    return;

  const NCollection_List<TopoDS_Shape>&    aledges = aMapVertexEdges.Find(anextVert);
  NCollection_List<TopoDS_Shape>::Iterator liter(aledges);
  isFind = false;
  TopoDS_Shape anextEdge;
  for (; liter.More() && !isFind; liter.Next())
  {
    if (!aMapEdges.Contains(liter.Value()) && !liter.Value().IsSame(ainitEdge))
    {
      anextEdge = liter.Value();
      aWireData->Add(anextEdge);
      if (aMapSeemEdges.Contains(anextEdge))
        aWireData->Add(anextEdge.Reversed());
      isFind = true;
      aMapEdges.Add(anextEdge);
      FindNext(anextVert,
               anextEdge,
               aMapVertices,
               aMapVertexEdges,
               aMapSmallEdges,
               aMapSeemEdges,
               aMapEdges,
               aWireData);
    }
  }
  return;
}

static bool isClosed2D(const TopoDS_Face& aFace, const TopoDS_Wire& aWire)
{
  bool                            isClosed = true;
  occ::handle<ShapeAnalysis_Wire> asaw =
    new ShapeAnalysis_Wire(aWire, aFace, Precision::Confusion());
  for (int i = 1; i <= asaw->NbEdges() && isClosed; i++)
  {
    TopoDS_Edge edge1 = asaw->WireData()->Edge(i);
    // checking that wire is closed in 2D space with tolerance of vertex.
    ShapeAnalysis_Edge sae;
    TopoDS_Vertex      v1 = sae.FirstVertex(edge1);
    asaw->SetPrecision(BRep_Tool::Tolerance(v1));
    asaw->CheckGap2d(i);
    isClosed = (asaw->LastCheckStatus(ShapeExtend_OK));
  }
  return isClosed;
}

//=================================================================================================

bool ShapeFix_Face::FixLoopWire(NCollection_Sequence<TopoDS_Shape>& aResWires)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapVertices;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                         aMapVertexEdges;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapSmallEdges;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapSeemEdges;
  if (!FixWireTool()->Analyzer()->CheckLoop(aMapVertices,
                                            aMapVertexEdges,
                                            aMapSmallEdges,
                                            aMapSeemEdges))
    return false;

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapEdges;
  NCollection_Sequence<TopoDS_Shape>                     aSeqWires;

  // collecting wires from common vertex belonging more than 2 edges
  int i = 1;
  for (; i <= aMapVertices.Extent(); i++)
  {
    TopoDS_Shape                             aVert   = aMapVertices.FindKey(i);
    const NCollection_List<TopoDS_Shape>&    aledges = aMapVertexEdges.Find(aVert);
    NCollection_List<TopoDS_Shape>::Iterator liter(aledges);
    for (; liter.More(); liter.Next())
    {
      TopoDS_Edge Edge = TopoDS::Edge(liter.Value());
      if (aMapEdges.Contains(Edge))
        continue;

      occ::handle<ShapeExtend_WireData> aWireData = new ShapeExtend_WireData;
      aWireData->Add(Edge);
      if (aMapSeemEdges.Contains(Edge))
        aWireData->Add(Edge.Reversed());
      aMapEdges.Add(Edge);
      FindNext(aVert,
               Edge,
               aMapVertices,
               aMapVertexEdges,
               aMapSmallEdges,
               aMapSeemEdges,
               aMapEdges,
               aWireData);
      if (aWireData->NbEdges() == 1 && aMapSmallEdges.Contains(aWireData->Edge(1)))
        continue;
      TopoDS_Vertex aV1, aV2;
      TopoDS_Wire   aWire = aWireData->Wire();
      TopExp::Vertices(aWire, aV1, aV2);

      if (aV1.IsSame(aV2))
      {
        occ::handle<ShapeExtend_WireData> asewd = new ShapeExtend_WireData(aWire);
        occ::handle<ShapeFix_Wire>        asfw  = new ShapeFix_Wire;
        asfw->Load(asewd);
        asfw->FixReorder();
        TopoDS_Wire awire2 = asfw->Wire();
        aResWires.Append(awire2);
      }
      else
        aSeqWires.Append(aWireData->Wire());
    }
  }

  if (aSeqWires.Length() == 1)
  {
    aResWires.Append(aSeqWires.Value(1));
  }
  else
  {
    // collecting whole wire from two not closed wires having two common vertices.
    for (i = 1; i <= aSeqWires.Length(); i++)
    {
      TopoDS_Vertex aV1, aV2;
      TopoDS_Wire   aWire = TopoDS::Wire(aSeqWires.Value(i));
      TopExp::Vertices(aWire, aV1, aV2);
      int j = i + 1;
      for (; j <= aSeqWires.Length(); j++)
      {
        TopoDS_Vertex aV21, aV22;
        TopoDS_Wire   aWire2 = TopoDS::Wire(aSeqWires.Value(j));
        TopExp::Vertices(aWire2, aV21, aV22);
        if ((aV1.IsSame(aV21) || aV1.IsSame(aV22)) && (aV2.IsSame(aV21) || aV2.IsSame(aV22)))
        {
          occ::handle<ShapeExtend_WireData> asewd = new ShapeExtend_WireData(aWire);
          asewd->Add(aWire2);
          occ::handle<ShapeFix_Wire> asfw = new ShapeFix_Wire;
          asfw->Load(asewd);
          asfw->FixReorder();
          aResWires.Append(asfw->Wire());
          aSeqWires.Remove(j--);
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
          break;
        }
      }
      if (j <= aSeqWires.Length())
        aSeqWires.Remove(i--);
    }
    if (aSeqWires.Length() < 3)
    {
      for (i = 1; i <= aSeqWires.Length(); i++)
        aResWires.Append(aSeqWires.Value(i));
    }
    else
    {
      // collecting wires having one common vertex
      for (i = 1; i <= aSeqWires.Length(); i++)
      {
        TopoDS_Vertex aV1, aV2;
        TopoDS_Wire   aWire = TopoDS::Wire(aSeqWires.Value(i));
        TopExp::Vertices(aWire, aV1, aV2);
        int j = i + 1;
        for (; j <= aSeqWires.Length(); j++)
        {
          TopoDS_Vertex aV21, aV22;
          TopoDS_Wire   aWire2 = TopoDS::Wire(aSeqWires.Value(j));
          TopExp::Vertices(aWire2, aV21, aV22);
          if ((aV1.IsSame(aV21) || aV1.IsSame(aV22)) || (aV2.IsSame(aV21) || aV2.IsSame(aV22)))
          {
            occ::handle<ShapeExtend_WireData> asewd = new ShapeExtend_WireData(aWire);
            asewd->Add(aWire2);
            occ::handle<ShapeFix_Wire> asfw = new ShapeFix_Wire;
            asfw->Load(asewd);
            asfw->FixReorder();
            aWire = asfw->Wire();
            TopExp::Vertices(aWire, aV1, aV2);
            aSeqWires.Remove(j--);
            myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
          }
        }
        aResWires.Append(aWire);
      }
    }
  }
  bool isClosed = true;

  // checking that obtained wires is closed in 2D space
  if (mySurf && mySurf->Adaptor3d()->GetType() != GeomAbs_Plane)
  {

    TopoDS_Shape emptyCopied = myFace.EmptyCopied();
    TopoDS_Face  tmpFace     = TopoDS::Face(emptyCopied);
    tmpFace.Orientation(TopAbs_FORWARD);

    for (i = 1; i <= aResWires.Length() && isClosed; i++)
    {
      TopoDS_Wire awire = TopoDS::Wire(aResWires.Value(i));
      isClosed          = isClosed2D(tmpFace, awire);
    }
  }

  return !aResWires.IsEmpty() && isClosed;
}

//=================================================================================================

bool ShapeFix_Face::SplitEdge(
  const occ::handle<ShapeExtend_WireData>&                               sewd,
  const int                                                              num,
  const double                                                           param,
  const TopoDS_Vertex&                                                   vert,
  const double                                                           preci,
  NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>& boxes)
{
  TopoDS_Edge        edge = sewd->Edge(num);
  TopoDS_Edge        newE1, newE2;
  ShapeFix_SplitTool aTool;
  if (aTool.SplitEdge(edge, param, vert, myFace, newE1, newE2, preci, 0.01 * preci))
  {
    // change context
    occ::handle<ShapeExtend_WireData> wd = new ShapeExtend_WireData;
    wd->Add(newE1);
    wd->Add(newE2);
    if (!Context().IsNull())
      Context()->Replace(edge, wd->Wire());
    for (TopExp_Explorer exp(wd->Wire(), TopAbs_EDGE); exp.More(); exp.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(exp.Current());
      BRepTools::Update(E);
    }

    // change sewd and boxes
    sewd->Set(newE1, num);
    if (num == sewd->NbEdges())
      sewd->Add(newE2);
    else
      sewd->Add(newE2, num + 1);

    boxes.UnBind(edge);
    TopLoc_Location                  L;
    const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(myFace, L);
    occ::handle<Geom2d_Curve>        c2d;
    double                           cf, cl;
    ShapeAnalysis_Edge               sae;
    if (sae.PCurve(newE1, S, L, c2d, cf, cl, false))
    {
      Bnd_Box2d           box;
      Geom2dAdaptor_Curve gac;
      double              aFirst = c2d->FirstParameter();
      double              aLast  = c2d->LastParameter();
      if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) && (cf < aFirst || cl > aLast))
      {
        // pdn avoiding problems with segment in Bnd_Box
        gac.Load(c2d);
      }
      else
        gac.Load(c2d, cf, cl);
      BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), box);
      boxes.Bind(newE1, box);
    }
    if (sae.PCurve(newE2, S, L, c2d, cf, cl, false))
    {
      Bnd_Box2d           box;
      Geom2dAdaptor_Curve gac;
      double              aFirst = c2d->FirstParameter();
      double              aLast  = c2d->LastParameter();
      if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) && (cf < aFirst || cl > aLast))
      {
        // pdn avoiding problems with segment in Bnd_Box
        gac.Load(c2d);
      }
      else
        gac.Load(c2d, cf, cl);
      BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), box);
      boxes.Bind(newE2, box);
    }
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeFix_Face::SplitEdge(
  const occ::handle<ShapeExtend_WireData>&                               sewd,
  const int                                                              num,
  const double                                                           param1,
  const double                                                           param2,
  const TopoDS_Vertex&                                                   vert,
  const double                                                           preci,
  NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>& boxes)
{
  TopoDS_Edge        edge = sewd->Edge(num);
  TopoDS_Edge        newE1, newE2;
  ShapeFix_SplitTool aTool;
  if (aTool.SplitEdge(edge, param1, param2, vert, myFace, newE1, newE2, preci, 0.01 * preci))
  {
    // change context
    occ::handle<ShapeExtend_WireData> wd = new ShapeExtend_WireData;
    wd->Add(newE1);
    wd->Add(newE2);
    if (!Context().IsNull())
      Context()->Replace(edge, wd->Wire());
    for (TopExp_Explorer exp(wd->Wire(), TopAbs_EDGE); exp.More(); exp.Next())
    {
      TopoDS_Edge E = TopoDS::Edge(exp.Current());
      BRepTools::Update(E);
    }

    // change sewd and boxes
    sewd->Set(newE1, num);
    if (num == sewd->NbEdges())
      sewd->Add(newE2);
    else
      sewd->Add(newE2, num + 1);

    boxes.UnBind(edge);
    TopLoc_Location                  L;
    const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(myFace, L);
    occ::handle<Geom2d_Curve>        c2d;
    double                           cf, cl;
    ShapeAnalysis_Edge               sae;
    if (sae.PCurve(newE1, S, L, c2d, cf, cl, false))
    {
      Bnd_Box2d           box;
      Geom2dAdaptor_Curve gac;
      double              aFirst = c2d->FirstParameter();
      double              aLast  = c2d->LastParameter();
      if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) && (cf < aFirst || cl > aLast))
      {
        // pdn avoiding problems with segment in Bnd_Box
        gac.Load(c2d);
      }
      else
        gac.Load(c2d, cf, cl);
      BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), box);
      boxes.Bind(newE1, box);
    }
    if (sae.PCurve(newE2, S, L, c2d, cf, cl, false))
    {
      Bnd_Box2d           box;
      Geom2dAdaptor_Curve gac;
      double              aFirst = c2d->FirstParameter();
      double              aLast  = c2d->LastParameter();
      if (c2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) && (cf < aFirst || cl > aLast))
      {
        // pdn avoiding problems with segment in Bnd_Box
        gac.Load(c2d);
      }
      else
        gac.Load(c2d, cf, cl);
      BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), box);
      boxes.Bind(newE2, box);
    }
    return true;
  }
  return false;
}

//=================================================================================================

bool ShapeFix_Face::FixIntersectingWires()
{
  ShapeFix_IntersectionTool ITool(Context(), Precision(), MaxTolerance());
  return ITool.FixIntersectingWires(myFace);
}

//=================================================================================================

bool ShapeFix_Face::FixWiresTwoCoincEdges()
{
  if (!Context().IsNull())
  {
    TopoDS_Shape S = Context()->Apply(myFace);
    myFace         = TopoDS::Face(S);
  }

  TopAbs_Orientation ori         = myFace.Orientation();
  TopoDS_Shape       emptyCopied = myFace.EmptyCopied();
  TopoDS_Face        face        = TopoDS::Face(emptyCopied);
  face.Orientation(TopAbs_FORWARD);
  int          nbWires = 0;
  BRep_Builder B;

  for (TopoDS_Iterator it(myFace, false); it.More(); it.Next())
  {
    if (it.Value().ShapeType() != TopAbs_WIRE
        || (it.Value().Orientation() != TopAbs_FORWARD
            && it.Value().Orientation() != TopAbs_REVERSED))
    {
      continue;
    }
    nbWires++;
  }
  if (nbWires < 2)
    return false;
  bool isFixed = false;
  for (TopoDS_Iterator wi(myFace, false); wi.More(); wi.Next())
  {
    if (wi.Value().ShapeType() != TopAbs_WIRE
        || (wi.Value().Orientation() != TopAbs_FORWARD
            && wi.Value().Orientation() != TopAbs_REVERSED))
    {
      B.Add(face, wi.Value());
      continue;
    }
    TopoDS_Wire                       wire = TopoDS::Wire(wi.Value());
    occ::handle<ShapeExtend_WireData> sewd = new ShapeExtend_WireData(wire);
    if (sewd->NbEdges() == 2)
    {
      TopoDS_Edge E1 = sewd->Edge(1);
      TopoDS_Edge E2 = sewd->Edge(2);
      E1.Orientation(TopAbs_FORWARD);
      E2.Orientation(TopAbs_FORWARD);
      if (!(E1 == E2))
      {
        B.Add(face, wire);
      }
      else
        isFixed = true;
    }
    else
    {
      B.Add(face, wire);
    }
  }
  if (isFixed)
  {
    face.Orientation(ori);
    if (!Context().IsNull())
      Context()->Replace(myFace, face);
    myFace = face;
  }

  return isFixed;
}

//=================================================================================================

bool ShapeFix_Face::FixSplitFace(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    MapWires)
{
  BRep_Builder                       B;
  NCollection_Sequence<TopoDS_Shape> faces;
  TopoDS_Shape                       S = myFace;
  if (!Context().IsNull())
    S = Context()->Apply(myFace);
  int NbWires = 0, NbWiresNew = 0, NbEdges;
  for (TopoDS_Iterator iter(S, false); iter.More(); iter.Next())
  {
    const TopoDS_Shape& aShape = iter.Value();
    if (aShape.ShapeType() != TopAbs_WIRE
        || (aShape.Orientation() != TopAbs_FORWARD && aShape.Orientation() != TopAbs_REVERSED))
      continue;
    TopoDS_Wire wire = TopoDS::Wire(aShape);
    NbWires++;
    if (MapWires.IsBound(wire))
    {
      // if wire not closed --> stop split and return false
      occ::handle<ShapeExtend_WireData> sewd = new ShapeExtend_WireData(wire);
      NbEdges                                = sewd->NbEdges();
      if (NbEdges == 0)
      {
        continue;
      }
      //
      TopoDS_Edge        E1 = sewd->Edge(1);
      TopoDS_Edge        E2 = sewd->Edge(NbEdges);
      TopoDS_Vertex      V1, V2;
      ShapeAnalysis_Edge sae;
      V1 = sae.FirstVertex(E1);
      V2 = sae.LastVertex(E2);
      if (!V1.IsSame(V2))
      {
        return false;
      }
      // create face
      TopoDS_Shape emptyCopied = S.EmptyCopied();
      TopoDS_Face  tmpFace     = TopoDS::Face(emptyCopied);
      tmpFace.Orientation(TopAbs_FORWARD);
      B.Add(tmpFace, wire);
      NbWiresNew++;
      const NCollection_List<TopoDS_Shape>&    IntWires = MapWires.Find(wire);
      NCollection_List<TopoDS_Shape>::Iterator liter(IntWires);
      for (; liter.More(); liter.Next())
      {
        TopoDS_Shape aShapeEmptyCopied = tmpFace.EmptyCopied();
        TopoDS_Face  aFace             = TopoDS::Face(aShapeEmptyCopied);
        aFace.Orientation(TopAbs_FORWARD);
        B.Add(aFace, liter.Value());
        BRepTopAdaptor_FClass2d clas(aFace, ::Precision::PConfusion());
        TopAbs_State            staout = clas.PerformInfinitePoint();
        if (staout == TopAbs_IN)
          B.Add(tmpFace, liter.Value());
        else
          B.Add(tmpFace, liter.Value().Reversed());
        NbWiresNew++;
      }
      if (!myFwd)
        tmpFace.Orientation(TopAbs_REVERSED);
      faces.Append(tmpFace);
    }
  }

  if (NbWires != NbWiresNew)
    return false;

  if (faces.Length() > 1)
  {
    TopoDS_Compound Comp;
    B.MakeCompound(Comp);
    for (int i = 1; i <= faces.Length(); i++)
      B.Add(Comp, faces(i));
    myResult = Comp;

    if (!Context().IsNull())
    {
      Context()->Replace(myFace, myResult);
    }

    for (TopExp_Explorer exp(myResult, TopAbs_FACE); exp.More(); exp.Next())
    {
      myFace = TopoDS::Face(exp.Current());
      BRepTools::Update(myFace);
    }
    return true;
  }

  return false;
}

//=======================================================================
// function : IsPeriodicConicalLoop
// purpose  : Checks whether the passed wire makes up a periodic loop on
//           passed conical surface
//=======================================================================

static bool IsPeriodicConicalLoop(const occ::handle<Geom_ConicalSurface>& theSurf,
                                  const TopoDS_Wire&                      theWire,
                                  const double                            theTolerance,
                                  double&                                 theMinU,
                                  double&                                 theMaxU,
                                  double&                                 theMinV,
                                  double&                                 theMaxV,
                                  bool&                                   isUDecrease)
{
  if (theSurf.IsNull())
    return false;

  ShapeAnalysis_Edge aSAE;
  TopLoc_Location    aLoc;

  double aCumulDeltaU = 0.0, aCumulDeltaUAbs = 0.0;
  double aMinU = RealLast();
  double aMinV = aMinU;
  double aMaxU = -aMinU;
  double aMaxV = aMaxU;

  // Iterate over the edges to check whether the wire is periodic on conical surface
  TopoDS_Iterator aWireIter(theWire, false);
  for (; aWireIter.More(); aWireIter.Next())
  {
    const TopoDS_Edge&        aCurrentEdge = TopoDS::Edge(aWireIter.Value());
    occ::handle<Geom2d_Curve> aC2d;
    double                    aPFirst, aPLast;

    aSAE.PCurve(aCurrentEdge, theSurf, aLoc, aC2d, aPFirst, aPLast, true);

    if (aC2d.IsNull())
      return false;

    gp_Pnt2d aUVFirst = aC2d->Value(aPFirst), aUVLast = aC2d->Value(aPLast);

    double aUFirst = aUVFirst.X(), aULast = aUVLast.X();
    double aVFirst = aUVFirst.Y(), aVLast = aUVLast.Y();

    double aCurMaxU = std::max(aUFirst, aULast), aCurMinU = std::min(aUFirst, aULast);
    double aCurMaxV = std::max(aVFirst, aVLast), aCurMinV = std::min(aVFirst, aVLast);

    if (aCurMinU < aMinU)
      aMinU = aCurMinU;
    if (aCurMaxU > aMaxU)
      aMaxU = aCurMaxU;
    if (aCurMinV < aMinV)
      aMinV = aCurMinV;
    if (aCurMaxV > aMaxV)
      aMaxV = aCurMaxV;

    double aDeltaU = aULast - aUFirst;

    aCumulDeltaU += aDeltaU;
    aCumulDeltaUAbs += std::abs(aDeltaU);
  }

  theMinU     = aMinU;
  theMaxU     = aMaxU;
  theMinV     = aMinV;
  theMaxV     = aMaxV;
  isUDecrease = (aCumulDeltaU < 0);

  bool is2PIDelta   = std::abs(aCumulDeltaUAbs - 2 * M_PI) <= theTolerance;
  bool isAroundApex = std::abs(theMaxU - theMinU) > 2 * M_PI - theTolerance;

  return is2PIDelta && isAroundApex;
}

//=================================================================================================

bool ShapeFix_Face::FixPeriodicDegenerated()
{
  /* =====================
   *  Prepare fix routine
   * ===================== */

  if (!Context().IsNull())
  {
    TopoDS_Shape aSh = Context()->Apply(myFace);
    myFace           = TopoDS::Face(aSh);
  }

  /* ================================================
   *  Check if fix can be applied on the passed face
   * ================================================ */

  // Collect all wires owned by the face
  NCollection_Sequence<TopoDS_Shape> aWireSeq;
  for (TopoDS_Iterator aWireIt(myFace, false); aWireIt.More(); aWireIt.Next())
  {
    const TopoDS_Shape& aSubSh = aWireIt.Value();
    if (aSubSh.ShapeType() != TopAbs_WIRE
        || (aSubSh.Orientation() != TopAbs_FORWARD && aSubSh.Orientation() != TopAbs_REVERSED))
      continue;

    aWireSeq.Append(aWireIt.Value());
  }

  // Get number of wires and surface
  int                       aNbWires = aWireSeq.Length();
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(myFace);

  // Only single wires on conical surfaces are checked
  if (aNbWires != 1 || aSurface.IsNull()
      || aSurface->DynamicType() != STANDARD_TYPE(Geom_ConicalSurface))
    return false;

  // Get the single wire
  TopoDS_Wire aSoleWire = TopoDS::Wire(aWireSeq.Value(1));

  // Check whether this wire is belting the conical surface by period
  occ::handle<Geom_ConicalSurface> aConeSurf = occ::down_cast<Geom_ConicalSurface>(aSurface);
  double aMinLoopU = 0.0, aMaxLoopU = 0.0, aMinLoopV = 0.0, aMaxLoopV = 0.0;
  bool   isUDecrease = false;

  bool isConicLoop = IsPeriodicConicalLoop(aConeSurf,
                                           aSoleWire,
                                           Precision(),
                                           aMinLoopU,
                                           aMaxLoopU,
                                           aMinLoopV,
                                           aMaxLoopV,
                                           isUDecrease);

  if (!isConicLoop)
    return false;

  /* ===============
   *  Retrieve apex
   * =============== */

  // Get base circle of the conical surface (the circle it was built from)
  occ::handle<Geom_Curve>  aConeBaseCrv  = aConeSurf->VIso(0.0);
  occ::handle<Geom_Circle> aConeBaseCirc = occ::down_cast<Geom_Circle>(aConeBaseCrv);

  // Retrieve conical props
  double aConeBaseR = aConeBaseCirc->Radius();
  double aSemiAngle = aConeSurf->SemiAngle();

  if (fabs(aSemiAngle) <= Precision::Confusion())
    return false; // Bad surface

  // Find the V parameter of the apex
  double aConeBaseH = aConeBaseR / std::sin(aSemiAngle);
  double anApexV    = -aConeBaseH;

  // Get apex vertex
  TopoDS_Vertex anApex = BRepBuilderAPI_MakeVertex(aConeSurf->Apex());

  // ====================================
  //  Build degenerated edge in the apex
  // ====================================

  TopoDS_Edge  anApexEdge;
  BRep_Builder aBuilder;
  aBuilder.MakeEdge(anApexEdge);

  // Check if positional relationship between the initial wire and apex
  // line in 2D is going to be consistent
  if (fabs(anApexV - aMinLoopV) <= Precision() || fabs(anApexV - aMaxLoopV) <= Precision()
      || (anApexV < aMaxLoopV && anApexV > aMinLoopV))
    return false;

  occ::handle<Geom2d_Line> anApexCurve2d;

  // Apex curve below the wire
  if (anApexV < aMinLoopV)
  {
    anApexCurve2d = new Geom2d_Line(gp_Pnt2d(aMinLoopU, anApexV), gp_Dir2d(gp_Dir2d::D::X));
    if (!isUDecrease)
      aSoleWire.Reverse();
  }

  // Apex curve above the wire
  if (anApexV > aMaxLoopV)
  {
    anApexCurve2d = new Geom2d_Line(gp_Pnt2d(aMaxLoopU, anApexV), gp_Dir2d(gp_Dir2d::D::NX));
    if (isUDecrease)
      aSoleWire.Reverse();
  }

  // Create degenerated edge & wire for apex
  aBuilder.UpdateEdge(anApexEdge, anApexCurve2d, myFace, Precision());
  aBuilder.Add(anApexEdge, anApex);
  aBuilder.Add(anApexEdge, anApex.Reversed());
  aBuilder.Degenerated(anApexEdge, true);
  aBuilder.Range(anApexEdge, 0, fabs(aMaxLoopU - aMinLoopU));
  TopoDS_Wire anApexWire = BRepBuilderAPI_MakeWire(anApexEdge);

  // ===============================================================
  //  Finalize the fix building new face and setting up the results
  // ===============================================================

  // Collect the resulting set of wires
  NCollection_Sequence<TopoDS_Shape> aNewWireSeq;
  aNewWireSeq.Append(aSoleWire);
  aNewWireSeq.Append(anApexWire);

  // Assemble new face
  TopoDS_Face aNewFace = TopoDS::Face(myFace.EmptyCopied());
  aNewFace.Orientation(TopAbs_FORWARD);
  BRep_Builder aFaceBuilder;
  for (int i = 1; i <= aNewWireSeq.Length(); i++)
  {
    TopoDS_Wire aNewWire = TopoDS::Wire(aNewWireSeq.Value(i));
    aFaceBuilder.Add(aNewFace, aNewWire);
  }
  aNewFace.Orientation(myFace.Orientation());

  // Adjust the resulting state of the healing tool
  myResult = aNewFace;
  Context()->Replace(myFace, myResult);

  return true;
}
