// Created on: 1999-08-24
// Created by: Sergei ZERTCHANINOV
// Copyright (c) 1999 Matra Datavision
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
#include <BRep_Tool.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Wireframe, ShapeFix_Root)

// #include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
//=================================================================================================

ShapeFix_Wireframe::ShapeFix_Wireframe()
{
  ClearStatuses();
  myModeDrop   = false;
  myLimitAngle = -1;
}

//=================================================================================================

ShapeFix_Wireframe::ShapeFix_Wireframe(const TopoDS_Shape& shape)
{
  ClearStatuses();
  myShape      = shape;
  myModeDrop   = false;
  myLimitAngle = -1;
}

//=================================================================================================

void ShapeFix_Wireframe::ClearStatuses()
{
  int emptyStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  myStatusWireGaps   = emptyStatus;
  myStatusSmallEdges = emptyStatus;
}

//=================================================================================================

void ShapeFix_Wireframe::Load(const TopoDS_Shape& shape)
{
  ClearStatuses();
  myShape = shape;
}

//=================================================================================================

bool ShapeFix_Wireframe::FixWireGaps()
{
  myStatusWireGaps = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (myShape.IsNull())
    return false;

  if (Context().IsNull())
    SetContext(new ShapeBuild_ReShape);
  else
  {
    TopoDS_Shape shape = myShape;
    myShape.Nullify();
    myShape = Context()->Apply(shape);
  }

  double                prec = (Precision() > 0.) ? Precision() : Precision::Confusion();
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> cont;
  if (myShape.ShapeType() == TopAbs_COMPOUND)
  {
    bool locModified = false;
    TopoDS_Compound  C;
    BRep_Builder     B;
    B.MakeCompound(C);
    TopoDS_Shape savShape = myShape;
    for (TopoDS_Iterator it(savShape); it.More(); it.Next())
    {
      TopoDS_Shape    shape1 = it.Value();
      TopLoc_Location L      = shape1.Location(), nullLoc;
      shape1.Location(nullLoc);
      TopoDS_Shape res;
      if (cont.IsBound(shape1))
      {
        res = cont.Find(shape1).Oriented(shape1.Orientation());
      }
      else
      {
        myShape = shape1;
        FixWireGaps();
        res = Shape();
        cont.Bind(myShape, res);
      }
      if (!res.IsSame(shape1))
        locModified = true;
      res.Location(L);
      B.Add(C, res);
    }
    if (locModified)
    {
      C.Orientation(savShape.Orientation());
      Context()->Replace(savShape, C);
    }
    myShape = Context()->Apply(savShape);
    return StatusWireGaps(ShapeExtend_DONE);
  }
  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire;
  sfw->SetContext(Context());
  sfw->SetPrecision(prec);

  TopoDS_Face face;
  for (TopExp_Explorer anExpf1(myShape, TopAbs_FACE); anExpf1.More(); anExpf1.Next())
  {
    // smh#8
    TopoDS_Shape tmpF = Context()->Apply(anExpf1.Current());
    face              = TopoDS::Face(tmpF);
    if (face.Orientation() == TopAbs_REVERSED)
      face.Orientation(TopAbs_FORWARD);
    for (TopoDS_Iterator itw(face); itw.More(); itw.Next())
    {
      if (itw.Value().ShapeType() != TopAbs_WIRE)
        continue;
      // smh#8
      TopoDS_Shape tmpW = Context()->Apply(itw.Value());
      sfw->Init(TopoDS::Wire(tmpW), face, prec);
      sfw->FixReorder();
      sfw->FixGaps3d();
      if (sfw->StatusGaps3d(ShapeExtend_DONE))
        myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      if (sfw->StatusGaps3d(ShapeExtend_FAIL))
        myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      sfw->FixGaps2d();
      if (sfw->StatusGaps2d(ShapeExtend_DONE))
        myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      if (sfw->StatusGaps2d(ShapeExtend_FAIL))
        myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      if (sfw->StatusGaps3d(ShapeExtend_DONE) || sfw->StatusGaps2d(ShapeExtend_DONE))
        SendWarning(itw.Value(), Message_Msg("FixWireframe.FixFixWireGaps.MSG0"));
    }
  }

  //============================================================
  // Author :  enk
  // Purpose:  This block fixing a 3d wire which not lie on plane
  // Part 1
  //============================================================
  for (TopExp_Explorer expw(myShape, TopAbs_WIRE, TopAbs_FACE); expw.More(); expw.Next())
  {
    TopoDS_Shape tmpW = Context()->Apply(expw.Current());
    sfw->Load(TopoDS::Wire(tmpW));
    sfw->SetPrecision(prec);
    sfw->FixReorder();
    sfw->FixGaps3d();
    if (sfw->StatusGaps3d(ShapeExtend_DONE))
      myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    if (sfw->StatusGaps3d(ShapeExtend_FAIL))
      myStatusWireGaps |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    if (sfw->StatusGaps3d(ShapeExtend_DONE))
      SendWarning(expw.Current(), Message_Msg("FixWireframe.FixFixWireGaps.MSG0"));
  }
  // End Part1========================================================

  if (StatusWireGaps(ShapeExtend_DONE))
  {

    myShape = Context()->Apply(myShape);

    ShapeFix::SameParameter(myShape, false);

    TopoDS_Wire           wire;
    occ::handle<ShapeFix_Edge> sfe = new ShapeFix_Edge;
    for (TopExp_Explorer anExpf2(myShape, TopAbs_FACE); anExpf2.More(); anExpf2.Next())
    {
      face = TopoDS::Face(anExpf2.Current());
      if (face.Orientation() == TopAbs_REVERSED)
        face.Orientation(TopAbs_FORWARD);
      for (TopoDS_Iterator itw(face); itw.More(); itw.Next())
      {
        if (itw.Value().ShapeType() != TopAbs_WIRE)
          continue;
        wire = TopoDS::Wire(itw.Value());
        sfw->Init(wire, face, prec);
        sfw->FixReorder();
        sfw->FixSelfIntersection();
        for (TopoDS_Iterator ite(wire); ite.More(); ite.Next())
          sfe->FixVertexTolerance(TopoDS::Edge(ite.Value()));
      }
    }

    // enk Part 2
    for (TopExp_Explorer expw2(myShape, TopAbs_WIRE, TopAbs_FACE); expw2.More(); expw2.Next())
    {
      wire = TopoDS::Wire(expw2.Current());
      sfw->Load(wire);
      sfw->SetPrecision(prec);
      sfw->FixReorder();
      sfw->FixSelfIntersection();
      for (TopoDS_Iterator ite(wire); ite.More(); ite.Next())
        sfe->FixVertexTolerance(TopoDS::Edge(ite.Value()));
    }
    // End Part 2

    return true;
  }

  return false;
}

//=======================================================================
// function : JoinEdges (static)
// purpose  : used in FixSmallEdges
//=======================================================================

static bool JoinEdges(const TopoDS_Edge&          E1,
                                  const TopoDS_Edge&          E2,
                                  TopoDS_Edge&                E3,
                                  const NCollection_List<TopoDS_Shape>& faces)
{
  bool     ReplaceFirst = true;
  ShapeAnalysis_Edge   sae;
  occ::handle<Geom_Curve>   c3d1, c3d2;
  occ::handle<Geom2d_Curve> c2d1, c2d2; // TopTools
  TopoDS_Edge          newedge, newedge1;
  E3                = newedge1;
  TopoDS_Vertex V11 = sae.FirstVertex(E1);
  TopoDS_Vertex V12 = sae.LastVertex(E1);
  TopoDS_Vertex V21 = sae.FirstVertex(E2);
  TopoDS_Vertex V22 = sae.LastVertex(E2);

  bool isSame = (V11.IsSame(V12) || V22.IsSame(V21));
  BRep_Builder     B;
  B.MakeEdge(newedge);
  double cf1, cf2, cl1, cl2, first1, first2, last1, last2;
  newedge.Orientation(TopAbs_FORWARD);
  try
  {
    OCC_CATCH_SIGNALS
    if (!sae.Curve3d(E1, c3d1, cf1, cl1, false))
      return ReplaceFirst;
    if (!sae.Curve3d(E2, c3d2, cf2, cl2, false))
      return ReplaceFirst;

    B.Add(newedge, V11.Oriented(TopAbs_FORWARD));
    B.Add(newedge, V22.Oriented(TopAbs_REVERSED));

    occ::handle<Geom_Curve> CRes;
    bool   isRev1, isRev2;
    //     double newf,newl;
    if (!ShapeConstruct::JoinCurves(c3d1,
                                    c3d2,
                                    E1.Orientation(),
                                    E2.Orientation(),
                                    cf1,
                                    cl1,
                                    cf2,
                                    cl2,
                                    CRes,
                                    isRev1,
                                    isRev2))
      return ReplaceFirst;
    //    if(isRev1 || isRev2)
    if (!isSame && (isRev1 || isRev2))
      return ReplaceFirst;
    ReplaceFirst = (!isRev1);

    double      newf        = cf1;
    double      newl        = cl1 + cl2 - cf2;
    TopAbs_Orientation OrEdge1     = E1.Orientation();
    TopAbs_Orientation OrEdge2     = E2.Orientation();
    bool   ismanifold  = (OrEdge1 == TopAbs_FORWARD || OrEdge1 == TopAbs_REVERSED);
    bool   ismanifold2 = (OrEdge2 == TopAbs_FORWARD || OrEdge2 == TopAbs_REVERSED);
    if (ismanifold != ismanifold2)
      return ReplaceFirst;

    if (ismanifold)
    {

      OrEdge1 = ((!isRev1 && E1.Orientation() == TopAbs_FORWARD)
                     || (isRev1 && E1.Orientation() == TopAbs_REVERSED)
                   ? TopAbs_FORWARD
                   : TopAbs_REVERSED);
      OrEdge2 = ((!isRev2 && E2.Orientation() == TopAbs_FORWARD)
                     || (isRev2 && E2.Orientation() == TopAbs_REVERSED)
                   ? TopAbs_FORWARD
                   : TopAbs_REVERSED);
    }
    B.UpdateEdge(newedge, CRes, std::max(BRep_Tool::Tolerance(E1), BRep_Tool::Tolerance(E2)));
    double fp = CRes->FirstParameter();
    double lp = CRes->LastParameter();
    if (fp > newf)
      newf = fp;
    if (lp < newl)
      newl = lp;
    B.Range(newedge, newf, newl);

    // merging pcurves
    for (NCollection_List<TopoDS_Shape>::Iterator iter(faces); iter.More(); iter.Next())
    {
      TopoDS_Face face = TopoDS::Face(iter.Value());
      if (!sae.PCurve(E1, face, c2d1, first1, last1, false))
        return ReplaceFirst;
      if (!sae.PCurve(E2, face, c2d2, first2, last2, false))
        return ReplaceFirst;

      occ::handle<Geom2d_Curve> C2dRes;
      bool     isRev12, isRev22;
      if (!ShapeConstruct::JoinCurves(c2d1,
                                      c2d2,
                                      OrEdge1,
                                      OrEdge2,
                                      first1,
                                      last1,
                                      first2,
                                      last2,
                                      C2dRes,
                                      isRev12,
                                      isRev22,
                                      isSame))
        return ReplaceFirst;

      if (ismanifold && (!isSame && (isRev12 || isRev22)))
        return ReplaceFirst;

      if (isRev12)
        ReplaceFirst = false;
      double fp2d = C2dRes->FirstParameter();
      double lp2d = C2dRes->LastParameter();
      // B.UpdateEdge(newedge,C2dRes,face,0);
      double newf1 = first1;
      double newl1 = last1 + (last2 - first2);
      if (fp2d > newf1)
        newf1 = fp2d;
      if (lp2d < newl1)
        newl1 = lp2d;

      // dealing with seams: the same again
      if (sae.IsSeam(E1, face) && sae.IsSeam(E2, face))
      {
        occ::handle<Geom2d_Curve> c2d12, c2d22;
        // smh#8
        TopoDS_Shape tmpE1 = E1.Reversed(), tmpE2 = E2.Reversed();
        TopoDS_Edge  E1t = TopoDS::Edge(tmpE1);
        TopoDS_Edge  E2t = TopoDS::Edge(tmpE2);
        sae.PCurve(E1t, face, c2d12, first1, last1, false);
        sae.PCurve(E2t, face, c2d22, first2, last2, false);

        occ::handle<Geom2d_Curve> C2dRes2;
        if (!ShapeConstruct::JoinCurves(c2d12,
                                        c2d22,
                                        OrEdge1,
                                        OrEdge2,
                                        first1,
                                        last1,
                                        first2,
                                        last2,
                                        C2dRes2,
                                        isRev12,
                                        isRev22,
                                        isSame))
          return ReplaceFirst;
        if (!isSame && (isRev1 || isRev2))
          return ReplaceFirst;
        B.UpdateEdge(newedge, C2dRes, C2dRes2, face, 0);
      }
      else if (sae.IsSeam(E1, face) || sae.IsSeam(E2, face))
        return ReplaceFirst;
      else if (!sae.IsSeam(E1, face) && !sae.IsSeam(E2, face))
        B.UpdateEdge(newedge, C2dRes, face, 0);
      B.Range(newedge, face, newf1, newl1);
      if (!ismanifold)
        newedge.Orientation(ReplaceFirst ? OrEdge1 : OrEdge2);
    }
    B.SameRange(newedge, false);

    E3 = newedge;
    return ReplaceFirst;
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: ShapeFix_Wireframe::FixSmallEdges: JoinEdges: Exception in "
                 "GeomConvert_CompCurveToBSplineCurve: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    return ReplaceFirst;
  }
}

//=================================================================================================

bool ShapeFix_Wireframe::FixSmallEdges()
{
  myStatusSmallEdges = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (myShape.IsNull())
    return false;

  if (Context().IsNull())
    SetContext(new ShapeBuild_ReShape);
  else
  {
    TopoDS_Shape shape = myShape;
    myShape.Nullify();
    myShape = Context()->Apply(shape);
  }
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> cont;
  if (myShape.ShapeType() == TopAbs_COMPOUND)
  {
    bool locModified = false;
    TopoDS_Compound  C;
    BRep_Builder     B;
    B.MakeCompound(C);
    TopoDS_Shape savShape = myShape;
    for (TopoDS_Iterator it(savShape); it.More(); it.Next())
    {
      TopoDS_Shape    shape1 = it.Value();
      TopLoc_Location L      = shape1.Location(), nullLoc;
      shape1.Location(nullLoc);
      TopoDS_Shape res;
      if (cont.IsBound(shape1))
      {
        res = cont.Find(shape1).Oriented(shape1.Orientation());
      }
      else
      {
        myShape = shape1;
        FixSmallEdges();
        res = Shape();
        cont.Bind(myShape, res);
      }
      if (!res.IsSame(shape1))
        locModified = true;

      // check if resulting shape if not empty
      if (res.IsNull())
        continue;

      res.Location(L);
      B.Add(C, res);
    }
    if (locModified)
    {
      C.Orientation(savShape.Orientation());
      Context()->Replace(savShape, C);
    }
    myShape = Context()->Apply(savShape);
    return StatusSmallEdges(ShapeExtend_DONE);
  }
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                theSmallEdges, theMultyEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> theEdgeToFaces, theFaceWithSmall;
  CheckSmallEdges(theSmallEdges, theEdgeToFaces, theFaceWithSmall, theMultyEdges);
  MergeSmallEdges(theSmallEdges, theEdgeToFaces, theFaceWithSmall, theMultyEdges);
  return StatusSmallEdges(ShapeExtend_DONE);
}

//=================================================================================================

#include <BRepBuilderAPI_MakeFace.hxx>

bool ShapeFix_Wireframe::CheckSmallEdges(
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSmallEdges,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theEdgeToFaces,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theFaceWithSmall,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                theMultyEdges)
{
  TopoDS_Face        face;
  TopoDS_Edge        edge;
  ShapeAnalysis_Wire SAW;

  for (TopExp_Explorer anExpf1(myShape, TopAbs_FACE); anExpf1.More(); anExpf1.Next())
  {
    NCollection_List<TopoDS_Shape> theEdgeList;
    TopoDS_Face          facet = TopoDS::Face(anExpf1.Current());
    face                       = facet;
    if (facet.Orientation() == TopAbs_REVERSED)
      face = TopoDS::Face(facet.Oriented(TopAbs_FORWARD));
    for (TopoDS_Iterator itw(face); itw.More(); itw.Next())
    {
      if (itw.Value().ShapeType() != TopAbs_WIRE)
        continue;
      TopoDS_Wire                  aW = TopoDS::Wire(itw.Value());
      occ::handle<ShapeExtend_WireData> aswd =
        new ShapeExtend_WireData(aW, true, false);
      SAW.Init(aswd, face, Precision());
      // pnd protection on seam edges
      NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> EdgeMap;
      int               i;
      for (i = 1; i <= SAW.NbEdges(); i++)
      {
        edge = SAW.WireData()->Edge(i);
        if (EdgeMap.IsBound(edge))
          EdgeMap.ChangeFind(edge)++;
        else
          EdgeMap.Bind(edge, 1);
      }

      for (i = 1; i <= SAW.NbEdges(); i++)
      {
        edge = SAW.WireData()->Edge(i);
        if (EdgeMap.Find(edge) != 1)
        {
          if (!SAW.WireData()->IsSeam(i))
            theMultyEdges.Add(edge);
          continue;
        }
        // Append current face to the list
        if (theEdgeToFaces.IsBound(edge))
        {
          theEdgeToFaces(edge).Append(facet);
        }
        else
        {
          NCollection_List<TopoDS_Shape> theFaceList;
          theFaceList.Append(facet);
          theEdgeToFaces.Bind(edge, theFaceList);
        }
        // Check if current edge is small
        if (theSmallEdges.Contains(edge))
          theEdgeList.Append(edge);
        else if (SAW.CheckSmall(i, Precision()))
        {
          theSmallEdges.Add(edge);
          theEdgeList.Append(edge);
        }
      }
    }
    // Add current face to the map if has small edges
    if (theEdgeList.Extent())
      theFaceWithSmall.Bind(facet, theEdgeList);
  }

  //=========================================================================
  // Author : enk
  // Purpose: Analyzing of shape for small edges, if edge doesn't lie on face
  //=========================================================================
  for (TopExp_Explorer expw1(myShape, TopAbs_WIRE, TopAbs_FACE); expw1.More(); expw1.Next())
  {
    SAW.SetPrecision(Precision());
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> EdgeMap;
    int               i;
    TopoDS_Wire                    theWire = TopoDS::Wire(expw1.Current());
    NCollection_List<TopoDS_Shape>           theEdgeList;
    SAW.Load(theWire);
    if (!SAW.IsLoaded())
    {
      return false;
    }
    for (i = 1; i <= SAW.NbEdges(); i++)
    {
      edge = SAW.WireData()->Edge(i);
      if (EdgeMap.IsBound(edge))
        EdgeMap.ChangeFind(edge)++;
      else
        EdgeMap.Bind(edge, 1);
    }

    for (i = 1; i <= SAW.NbEdges(); i++)
    {
      edge = SAW.WireData()->Edge(i);
      if (EdgeMap.Find(edge) != 1)
      {
        if (!SAW.WireData()->IsSeam(i))
          theMultyEdges.Add(edge);
        continue;
      }

      // Check if current edge is small
      if (theSmallEdges.Contains(edge))
        theEdgeList.Append(edge);
      else if (SAW.CheckSmall(i, Precision()))
      {
        theSmallEdges.Add(edge);
        theEdgeList.Append(edge);
      }
    }
  }
  return (!theSmallEdges.IsEmpty());
}

//=================================================================================================

bool ShapeFix_Wireframe::MergeSmallEdges(
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                theSmallEdges,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theEdgeToFaces,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theFaceWithSmall,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                theMultyEdges,
  const bool              theModeDrop,
  const double                 theLimitAngle)
{
  bool aModLimitAngle = (theLimitAngle > -1.0 || myLimitAngle > -1.0);
  double    aLimitAngle    = std::max(theLimitAngle, myLimitAngle);

  bool             aModeDrop = theModeDrop || myModeDrop;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> theNewVertices;
  if (!theSmallEdges.IsEmpty())
  {

    occ::handle<ShapeFix_Wire> SFW = new ShapeFix_Wire;
    SFW->SetContext(Context());
    ShapeAnalysis_Edge SAE;
    TopoDS_Edge        edge1, edge2, edge3;
    // Iterate on map of faces with small edges
    TopExp_Explorer anExpf2(myShape, TopAbs_FACE);
    for (; anExpf2.More(); anExpf2.Next())
    {
      if (theFaceWithSmall.IsBound(anExpf2.Current()))
      {
        if (theFaceWithSmall(anExpf2.Current()).Extent())
        {
          // smh#8
          TopoDS_Shape tmpShape = Context()->Apply(anExpf2.Current());
          TopoDS_Face  facet    = TopoDS::Face(tmpShape);
          if (!facet.IsSame(anExpf2.Current()))
          { // gka
            TopExp_Explorer aExpEdge(anExpf2.Current(), TopAbs_EDGE);
            for (; aExpEdge.More(); aExpEdge.Next())
            {
              TopoDS_Shape     newEdge;
              int stat = Context()->Status(aExpEdge.Current(), newEdge, true);
              if (stat > 0)
              {
                if (theSmallEdges.Contains(aExpEdge.Current()))
                {
                  theSmallEdges.Remove(aExpEdge.Current());
                  theSmallEdges.Add(newEdge);
                }
                if (theEdgeToFaces.IsBound(aExpEdge.Current()))
                {
                  NCollection_List<TopoDS_Shape> aListFaces;
                  aListFaces = theEdgeToFaces.Find(aExpEdge.Current());
                  theEdgeToFaces.UnBind(aExpEdge.Current());
                  theEdgeToFaces.Bind(newEdge, aListFaces);
                }
              }
            }
          }
          TopoDS_Face face = facet;
          // if (face.Orientation()==TopAbs_REVERSED)
          //   face = TopoDS::Face(facet.Oriented(TopAbs_FORWARD));
          for (TopoDS_Iterator itw(face); itw.More(); itw.Next())
          {
            if (itw.Value().ShapeType() != TopAbs_WIRE)
              continue;
            TopoDS_Wire aWire = TopoDS::Wire(itw.Value());
            if (face.Orientation() == TopAbs_REVERSED)
              face = TopoDS::Face(facet.Oriented(TopAbs_FORWARD));
            occ::handle<ShapeExtend_WireData> aswd =
              new ShapeExtend_WireData(aWire, true, false);
            // SFW->Load(aWire);
            SFW->Load(aswd);
            SFW->FixReorder();
            int prev, next, index = 1;

            while (index <= SFW->WireData()->NbEdges() && SFW->NbEdges() > 1)
            {
              prev  = (index == 1) ? SFW->WireData()->NbEdges() : index - 1;
              next  = (index == SFW->WireData()->NbEdges()) ? 1 : index + 1;
              edge1 = SFW->WireData()->Edge(prev);
              edge2 = SFW->WireData()->Edge(index);
              edge3 = SFW->WireData()->Edge(next);

              // gka protection against joining seem edge
              if (edge2.IsSame(edge1) || edge2.IsSame(edge3))
              {
                // if(BRep_Tool::IsClosed(edge2,face)) {
                index++;
                continue;
              }

              bool isSeam  = SFW->WireData()->IsSeam(index);
              bool isSeam1 = SFW->WireData()->IsSeam(prev);
              bool isSeam2 = SFW->WireData()->IsSeam(next);
              if (theSmallEdges.Contains(edge2))
              {
                // Middle edge is small - choose a pair of edges to join
                bool   IsAnyJoin  = (edge1.IsSame(edge3));
                bool   take_next  = IsAnyJoin; // false;
                bool   isLimAngle = false;
                occ::handle<Geom_Curve> C1, C2, C3;
                double      aux, last1, first2, last2, first3;
                double      Ang1 = 0., Ang2 = 0.;
                if (SAE.Curve3d(edge1, C1, aux, last1) && SAE.Curve3d(edge2, C2, first2, last2)
                    && SAE.Curve3d(edge3, C3, first3, aux))
                {
                  // Compare angles between edges
                  // double Ang1, Ang2;
                  gp_Vec Vec1, Vec2;
                  gp_Pnt P;
                  C1->D1(last1, P, Vec1);
                  C2->D1(first2, P, Vec2);
                  if (edge1.Orientation() == TopAbs_REVERSED)
                    Vec1.Reverse();
                  if (edge2.Orientation() == TopAbs_REVERSED)
                    Vec2.Reverse();
                  constexpr double tol2 = Precision::SquareConfusion();
                  if (Vec1.SquareMagnitude() < tol2 || Vec2.SquareMagnitude() < tol2)
                    Ang1 = M_PI / 2.;
                  else
                    Ang1 = std::abs(Vec1.Angle(Vec2));
                  C2->D1(last2, P, Vec1);
                  C3->D1(first3, P, Vec2);
                  if (edge2.Orientation() == TopAbs_REVERSED)
                    Vec1.Reverse();
                  if (edge3.Orientation() == TopAbs_REVERSED)
                    Vec2.Reverse();
                  if (Vec1.SquareMagnitude() < tol2 || Vec2.SquareMagnitude() < tol2)
                    Ang2 = M_PI / 2.;
                  else
                    Ang2 = std::abs(Vec1.Angle(Vec2));
                  // isLimAngle = (theLimitAngle != -1 && std::min(Ang1,Ang2) > theLimitAngle);
                  // take_next = (Ang2<Ang1);
                  // if (take_next) { edge1 = edge2; edge2 = edge3; }
                }
                // if(theLimitAngle != -1 && Ang1 > theLimitAngle && Ang2 >theLimitAngle) {
                //   index++; continue;
                // }

                // Check if edges lay on the same faces
                if (theMultyEdges.Contains(edge1) || theMultyEdges.Contains(edge2))
                { //??????
                  index++;
                  continue;
                }
                NCollection_List<TopoDS_Shape> theList1, theList2, theList3;
                if (theEdgeToFaces.IsBound(edge1))
                  theList1 = theEdgeToFaces(edge1);
                if (theEdgeToFaces.IsBound(edge2))
                  theList2 = theEdgeToFaces(edge2);
                if (theEdgeToFaces.IsBound(edge3))
                  theList3 = theEdgeToFaces(edge3);
                bool same_set = false;

                // gka protection against joining seem edges with other edges
                bool same_set1 = (theList1.Extent() == theList2.Extent() &&
                                              // clang-format off
                                              ((!isSeam && !isSeam1)|| (isSeam && isSeam1))); //gka
                // clang-format on
                bool    same_set2 = (theList3.Extent() == theList2.Extent()
                                              && ((!isSeam && !isSeam2) || (isSeam && isSeam2)));
                NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theSetOfFaces;
                for (NCollection_List<TopoDS_Shape>::Iterator itf1(theList2); itf1.More(); itf1.Next())
                  theSetOfFaces.Add(itf1.Value());
                if (same_set1)
                {
                  // Add all faces of the first edge to the current set
                  for (NCollection_List<TopoDS_Shape>::Iterator itf2(theList1);
                       (itf2.More() && same_set1);
                       itf2.Next())
                    same_set1 = theSetOfFaces.Contains(itf2.Value());
                }
                if (same_set2)
                {
                  // Add all faces of the first edge to the current set
                  for (NCollection_List<TopoDS_Shape>::Iterator itf2(theList3);
                       (itf2.More() && same_set2);
                       itf2.Next())
                    same_set2 = theSetOfFaces.Contains(itf2.Value());
                }
                if (same_set1 && same_set2)
                {
                  same_set = true;
                  if (fabs(Ang2 - Ang1) > Precision::Angular())
                    take_next = (Ang2 < Ang1);
                  if (take_next)
                  {
                    edge1 = edge2;
                    edge2 = edge3;
                  }
                  isLimAngle = (aModLimitAngle && std::min(Ang1, Ang2) > aLimitAngle);
                }
                else if (same_set1 && !same_set2)
                {
                  isLimAngle = (aModLimitAngle && Ang1 > aLimitAngle);
                  same_set   = true;
                }
                else if (!same_set1 && same_set2)
                {
                  same_set   = true;
                  isLimAngle = (aModLimitAngle && Ang2 > aLimitAngle);
                  edge1      = edge2;
                  edge2      = edge3;
                  take_next  = true;
                }
                if (same_set && !isLimAngle)
                {
                  // Merge current pair of edges
                  // gka protection against crossing seem on second face
                  bool isNeedJoin = true; // false;
                  for (NCollection_List<TopoDS_Shape>::Iterator aItF(theList2); aItF.More() && isNeedJoin;
                       aItF.Next())
                  {
                    if (aItF.Value().IsSame(anExpf2.Current()))
                      continue;
                    TopoDS_Shape aF = Context()->Apply(aItF.Value());
                    // aF = aF.Oriented(TopAbs_FORWARD);
                    for (TopoDS_Iterator aIw(aF); aIw.More(); aIw.Next())
                    {
                      if (aIw.Value().ShapeType() != TopAbs_WIRE)
                        continue;
                      TopoDS_Wire           wt   = TopoDS::Wire(aIw.Value());
                      occ::handle<ShapeFix_Wire> SFW1 = new ShapeFix_Wire;
                      SFW1->Load(wt);
                      SFW1->FixReorder();
                      occ::handle<ShapeExtend_WireData> atmpswd = SFW1->WireData();
                      int             ind1    = atmpswd->Index(edge1);
                      int             ind2    = atmpswd->Index(edge2);
                      if (ind1 && ind2)
                      {
                        isNeedJoin =
                          ((ind1 - ind2) == 1 || (ind2 == atmpswd->NbEdges() && ind1 == 1));
                        break;
                      }
                    }
                  }
                  bool ReplaceFirst = true;
                  if (isNeedJoin)
                  {

                    NCollection_List<TopoDS_Shape> aListF;
                    for (NCollection_List<TopoDS_Shape>::Iterator aItlF(theList2); aItlF.More();
                         aItlF.Next())
                    {
                      TopoDS_Shape tmpF = Context()->Apply(aItlF.Value());
                      aListF.Append(tmpF);
                    }
                    ReplaceFirst = JoinEdges(edge1, edge2, edge3, aListF);
                  }
                  else
                    edge3 = TopoDS_Edge();
                  if (edge3.IsNull())
                  {
                    index++;
                    myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
                  }
                  else
                  {
                    // Record vertex replacements in the map
                    TopoDS_Vertex oldV1 = SAE.FirstVertex(edge3), oldV2 = SAE.LastVertex(edge3);
                    if (!theNewVertices.IsBound(oldV1))
                    // smh#8
                    {
                      TopoDS_Shape emptyCopiedV1 = oldV1.EmptyCopied();
                      theNewVertices.Bind(oldV1, TopoDS::Vertex(emptyCopiedV1));
                    }
                    if (!oldV1.IsSame(oldV2))
                      if (!theNewVertices.IsBound(oldV2))
                      // smh#8
                      {
                        TopoDS_Shape emptyCopiedV2 = oldV2.EmptyCopied();
                        theNewVertices.Bind(oldV2, TopoDS::Vertex(emptyCopiedV2));
                      }

                    // To keep NM vertices belonging initial edges
                    TopoDS_Iterator aItv(edge1, false);
                    for (; aItv.More(); aItv.Next())
                    {
                      if (aItv.Value().Orientation() == TopAbs_INTERNAL
                          || aItv.Value().Orientation() == TopAbs_EXTERNAL)
                      {
                        TopoDS_Vertex aOldV = TopoDS::Vertex(aItv.Value());
                        TopoDS_Vertex anewV =
                          ShapeAnalysis_TransferParametersProj::CopyNMVertex(aOldV, edge3, edge1);
                        BRep_Builder aB;
                        aB.Add(edge3, anewV);
                        Context()->Replace(aOldV, anewV);
                      }
                    }

                    for (aItv.Initialize(edge2, false); aItv.More(); aItv.Next())
                    {
                      if (aItv.Value().Orientation() == TopAbs_INTERNAL
                          || aItv.Value().Orientation() == TopAbs_EXTERNAL)
                      {
                        BRep_Builder  aB;
                        TopoDS_Vertex aOldV = TopoDS::Vertex(aItv.Value());
                        TopoDS_Vertex anewV =
                          ShapeAnalysis_TransferParametersProj::CopyNMVertex(aOldV, edge3, edge2);
                        aB.Add(edge3, anewV);
                        Context()->Replace(aOldV, anewV);
                      }
                    }

                    // Check for small resulting edge
                    bool   newsmall = false;
                    ShapeAnalysis_Wire SAW;
                    SAW.Init(SFW->WireData(), face, Precision());
                    // Make changes in WireData and Context
                    if (ReplaceFirst)
                    {
                      Context()->Replace(edge1, edge3);
                      Context()->Remove(edge2);
                      SendWarning(edge2, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                    }
                    else
                    {
                      Context()->Replace(edge2, edge3);
                      Context()->Remove(edge1);
                      SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                    }
                    if (take_next)
                    {
                      SFW->WireData()->Set(edge3, next);
                      newsmall = SAW.CheckSmall(next, Precision());
                    }
                    else
                    {
                      SFW->WireData()->Set(edge3, prev);
                      newsmall = SAW.CheckSmall(prev, Precision());
                    }
                    SFW->WireData()->Remove(index);
                    // Process changes in maps
                    NCollection_List<TopoDS_Shape> theList;
                    theList.Append(theList2);
                    theEdgeToFaces.UnBind(edge1);
                    theEdgeToFaces.UnBind(edge2);
                    theEdgeToFaces.Bind(edge3, theList);
                    if (theSmallEdges.Contains(edge1))
                      theSmallEdges.Remove(edge1);
                    if (theSmallEdges.Contains(edge2))
                      theSmallEdges.Remove(edge2);
                    if (newsmall)
                      theSmallEdges.Add(edge3);
                    for (NCollection_List<TopoDS_Shape>::Iterator itlf(theList); itlf.More(); itlf.Next())
                    {
                      TopoDS_Shape curface = itlf.Value();
                      if (theFaceWithSmall.IsBound(curface))
                      {
                        NCollection_List<TopoDS_Shape>& theEdges = theFaceWithSmall(curface);
                        if (newsmall)
                          theEdges.Append(edge3);
                        NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                        while (ite.More())
                        {
                          TopoDS_Shape iedge = ite.Value();
                          if (iedge.IsSame(edge1) || iedge.IsSame(edge2))
                            theEdges.Remove(ite);
                          else
                            ite.Next();
                        }
                        // Remove face without small edges from the map
                        if (!theEdges.Extent())
                          theFaceWithSmall.UnBind(curface);
                      }
                    }
                    myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
                  }
                }
                else if (aModeDrop)
                { // gka
                  occ::handle<ShapeExtend_WireData> tempWire = new ShapeExtend_WireData();
                  ShapeAnalysis_Wire           tempSaw;
                  tempWire->Add(SFW->Wire());
                  TopoDS_Edge remedge;
                  if (take_next)
                    remedge = edge1;
                  else
                    remedge = edge2;
                  tempWire->Remove(index);
                  tempSaw.Load(tempWire);
                  int newindex = (index <= tempSaw.NbEdges() ? index : 1);
                  tempSaw.CheckConnected(newindex, Precision());
                  if (!tempSaw.LastCheckStatus(ShapeExtend_FAIL))
                  {
                    SFW->WireData()->Remove(index);
                    TopoDS_Edge tmpedge1 = tempWire->Edge(newindex);
                    TopoDS_Edge tmpedge2 =
                      tempWire->Edge(newindex == 1 ? tempSaw.NbEdges() : (newindex - 1));
                    NCollection_List<TopoDS_Shape> aL1;
                    if (theEdgeToFaces.IsBound(tmpedge1))
                      aL1 = theEdgeToFaces.Find(tmpedge1);
                    NCollection_List<TopoDS_Shape> aL2;
                    if (theEdgeToFaces.IsBound(tmpedge2))
                      aL2 = theEdgeToFaces.Find(tmpedge2);
                    SFW->FixConnected(newindex <= SFW->NbEdges() ? newindex : 1, Precision());
                    SFW->FixDegenerated(newindex <= SFW->NbEdges() ? newindex : 1);
                    TopoDS_Shape aTmpShape = Context()->Apply(tmpedge1); // for porting
                    TopoDS_Edge  anewedge1 = TopoDS::Edge(aTmpShape);
                    aTmpShape              = Context()->Apply(tmpedge2);
                    TopoDS_Edge anewedge2  = TopoDS::Edge(aTmpShape);
                    Context()->Remove(remedge);
                    SendWarning(remedge, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                    if (theSmallEdges.Contains(remedge))
                      theSmallEdges.Remove(remedge);
                    theEdgeToFaces.UnBind(remedge);
                    theEdgeToFaces.UnBind(tmpedge1);
                    theEdgeToFaces.UnBind(tmpedge2);
                    theEdgeToFaces.Bind(anewedge1, aL1);
                    theEdgeToFaces.Bind(anewedge2, aL2);
                    if (theSmallEdges.Contains(tmpedge1))
                    {
                      theSmallEdges.Remove(tmpedge1);
                      theSmallEdges.Add(anewedge1);
                      for (NCollection_List<TopoDS_Shape>::Iterator itlf(aL1); itlf.More(); itlf.Next())
                      {
                        TopoDS_Shape                       curface  = itlf.Value();
                        NCollection_List<TopoDS_Shape>&              theEdges = theFaceWithSmall(curface);
                        NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                        while (ite.More())
                        {
                          TopoDS_Shape iedge = ite.Value();
                          if (iedge.IsSame(tmpedge1))
                          {
                            theEdges.Remove(ite);
                            theEdges.Append(anewedge1);
                          }
                          else
                            ite.Next();
                        }
                      }
                    }
                    if (theSmallEdges.Contains(tmpedge2))
                    {
                      theSmallEdges.Remove(tmpedge2);
                      theSmallEdges.Add(anewedge2);
                      for (NCollection_List<TopoDS_Shape>::Iterator itlf(aL2); itlf.More(); itlf.Next())
                      {
                        TopoDS_Shape                       curface  = itlf.Value();
                        NCollection_List<TopoDS_Shape>&              theEdges = theFaceWithSmall(curface);
                        NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                        while (ite.More())
                        {
                          TopoDS_Shape iedge = ite.Value();
                          if (iedge.IsSame(tmpedge2))
                          {
                            theEdges.Remove(ite);
                            theEdges.Append(anewedge2);
                          }
                          else
                            ite.Next();
                        }
                      }
                    }
                    myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
                  }
                  else
                    index++;
                }
                else
                {
                  // gka protection against removing circles
                  TopoDS_Edge        ed = (take_next ? edge1 : edge2);
                  ShapeAnalysis_Edge sae;
                  occ::handle<Geom_Curve> c3d;
                  double      f1, l1;
                  if (sae.Curve3d(ed, c3d, f1, l1, false))
                  {
                    gp_Pnt p1, p2, p3;
                    c3d->D0(f1, p1);
                    c3d->D0(l1, p2);
                    c3d->D0((f1 + l1) * 0.5, p3);

                    if (p1.Distance(p3) > p1.Distance(p2))
                    {
                      index++;
                      continue;
                    }
                  }
                  if (take_next && theList2.Extent() == 1)
                  { // gka
                    TopoDS_Vertex V1 = SAE.FirstVertex(edge1), V2 = SAE.LastVertex(edge1);
                    if (V1.IsSame(V2))
                    {
                      SFW->WireData()->Remove(index);
                      Context()->Remove(edge1);
                      SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                      if (theSmallEdges.Contains(edge1))
                        theSmallEdges.Remove(edge1);
                      theEdgeToFaces.UnBind(edge1);
                      myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
                    }
                    else
                      index++;
                  }
                  else if (!take_next && theList2.Extent() == 1)
                  {
                    TopoDS_Vertex V1 = SAE.FirstVertex(edge2), V2 = SAE.LastVertex(edge2);
                    if (V1.IsSame(V2))
                    {
                      SFW->WireData()->Remove(index);
                      Context()->Remove(edge2);
                      SendWarning(edge2, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                      if (theSmallEdges.Contains(edge2))
                        theSmallEdges.Remove(edge2);
                      theEdgeToFaces.UnBind(edge2);
                      myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
                    }
                    else
                      index++;
                  }
                  else
                    index++;
                }
              }
              else
                index++;
            }
            if (SFW->NbEdges() == 1 && aModeDrop)
            {
              edge1 = SFW->WireData()->Edge(1);
              if (theSmallEdges.Contains(edge1))
              {
                SFW->WireData()->Remove(1);
                Context()->Remove(edge1);
                SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                theSmallEdges.Remove(edge1);
                theEdgeToFaces.UnBind(edge1);
                Context()->Remove(aWire);
                SendWarning(aWire, Message_Msg("FixWireframe.FixSmallEdges.MSG1"));
                myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
              }
            }
            else
            {
              SFW->FixConnected();
              Context()->Replace(aWire, SFW->Wire());
            }
          }
          face.Orientation(facet.Orientation());
          TopoDS_Shape    anewShape = Context()->Apply(face);
          TopoDS_Iterator aIter(anewShape);
          if (!aIter.More())
          {
            Context()->Remove(anewShape);
            SendWarning(face, Message_Msg("FixWireframe.FixSmallEdges.MSG2"));
          }
        }
      }
    }

    // enk block
    // Iterate on map of wires which not lie on faces
    for (TopExp_Explorer expw1(myShape, TopAbs_WIRE, TopAbs_FACE); expw1.More(); expw1.Next())
    {
      TopoDS_Wire aWire = TopoDS::Wire(expw1.Current());
      SFW->Load(aWire);
      SFW->FixReorder();
      int prev, next, index = 1;
      while (index <= SFW->NbEdges() && SFW->NbEdges() > 1)
      {
        prev  = (index == 1) ? SFW->NbEdges() : index - 1;
        next  = (index == SFW->NbEdges()) ? 1 : index + 1;
        edge1 = SFW->WireData()->Edge(prev);
        edge2 = SFW->WireData()->Edge(index);
        edge3 = SFW->WireData()->Edge(next);

        // gka protection against joining seem edge
        if (edge2.IsSame(edge1) || edge2.IsSame(edge3))
        {
          // if(BRep_Tool::IsClosed(edge2,face)) {
          index++;
          continue;
        }

        bool isSeam  = SFW->WireData()->IsSeam(index);
        bool isSeam1 = SFW->WireData()->IsSeam(prev);
        bool isSeam2 = SFW->WireData()->IsSeam(next);
        if (theSmallEdges.Contains(edge2))
        {
          // Middle edge is small - choose a pair of edges to join
          bool   IsAnyJoin  = (edge1.IsSame(edge3));
          bool   take_next  = IsAnyJoin; // false;
          bool   isLimAngle = false;
          occ::handle<Geom_Curve> C1, C2, C3;
          double      aux, last1, first2, last2, first3;
          double      Ang1 = 0., Ang2 = 0.;
          if (SAE.Curve3d(edge1, C1, aux, last1) && SAE.Curve3d(edge2, C2, first2, last2)
              && SAE.Curve3d(edge3, C3, first3, aux))
          {
            // Compare angles between edges
            // double Ang1, Ang2;
            gp_Vec Vec1, Vec2;
            gp_Pnt P;
            C1->D1(last1, P, Vec1);
            C2->D1(first2, P, Vec2);
            if (edge1.Orientation() == TopAbs_REVERSED)
              Vec1.Reverse();
            if (edge2.Orientation() == TopAbs_REVERSED)
              Vec2.Reverse();
            constexpr double tol2 = Precision::SquareConfusion();
            if (Vec1.SquareMagnitude() < tol2 || Vec2.SquareMagnitude() < tol2)
              Ang1 = M_PI / 2.;
            else
              Ang1 = std::abs(Vec1.Angle(Vec2));
            C2->D1(last2, P, Vec1);
            C3->D1(first3, P, Vec2);
            if (edge2.Orientation() == TopAbs_REVERSED)
              Vec1.Reverse();
            if (edge3.Orientation() == TopAbs_REVERSED)
              Vec2.Reverse();
            if (Vec1.SquareMagnitude() < tol2 || Vec2.SquareMagnitude() < tol2)
              Ang2 = M_PI / 2.;
            else
              Ang2 = std::abs(Vec1.Angle(Vec2));
            // isLimAngle = (theLimitAngle != -1 && std::min(Ang1,Ang2) > theLimitAngle);
            // take_next = (Ang2<Ang1);
            // if (take_next) { edge1 = edge2; edge2 = edge3; }
          }
          // if(theLimitAngle != -1 && Ang1 > theLimitAngle && Ang2 >theLimitAngle) {
          //   index++; continue;
          // }

          // Check if edges lay on the same faces
          if (theMultyEdges.Contains(edge1) || theMultyEdges.Contains(edge2))
          { //??????
            index++;
            continue;
          }
          NCollection_List<TopoDS_Shape> theList1, theList2, theList3;
          if (theEdgeToFaces.IsBound(edge1))
            theList1 = theEdgeToFaces(edge1);
          if (theEdgeToFaces.IsBound(edge2))
            theList2 = theEdgeToFaces(edge2);
          if (theEdgeToFaces.IsBound(edge3))
            theList3 = theEdgeToFaces(edge3);
          bool same_set = false;

          // gka protection against joining seem edges with other edges
          bool same_set1 = (theList1.Extent() == theList2.Extent() &&
                                        // clang-format off
                                              ((!isSeam && !isSeam1)|| (isSeam && isSeam1))); //gka
          // clang-format on
          bool    same_set2 = (theList3.Extent() == theList2.Extent()
                                        && ((!isSeam && !isSeam2) || (isSeam && isSeam2)));
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theSetOfFaces;
          for (NCollection_List<TopoDS_Shape>::Iterator itf1(theList2); itf1.More(); itf1.Next())
            theSetOfFaces.Add(itf1.Value());
          if (same_set1)
          {
            // Add all faces of the first edge to the current set
            for (NCollection_List<TopoDS_Shape>::Iterator itf2(theList1); (itf2.More() && same_set1);
                 itf2.Next())
              same_set1 = theSetOfFaces.Contains(itf2.Value());
          }
          if (same_set2)
          {
            // Add all faces of the first edge to the current set
            for (NCollection_List<TopoDS_Shape>::Iterator itf2(theList3); (itf2.More() && same_set2);
                 itf2.Next())
              same_set2 = theSetOfFaces.Contains(itf2.Value());
          }
          if (same_set1 && same_set2)
          {
            same_set = true;
            if (fabs(Ang2 - Ang1) > Precision::Angular())
              take_next = (Ang2 < Ang1);
            if (take_next)
            {
              edge1 = edge2;
              edge2 = edge3;
            }
            isLimAngle = (aModLimitAngle && std::min(Ang1, Ang2) > aLimitAngle);
          }
          else if (same_set1 && !same_set2)
          {
            isLimAngle = (aModLimitAngle && Ang1 > aLimitAngle);
            same_set   = true;
          }
          else if (!same_set1 && same_set2)
          {
            same_set   = true;
            isLimAngle = (aModLimitAngle && Ang2 > aLimitAngle);
            edge1      = edge2;
            edge2      = edge3;
            take_next  = true;
          }
          if (same_set && !isLimAngle)
          {
            // Merge current pair of edges
            // gka protection against crossing seem on second face
            bool isNeedJoin = true; // false;
            for (NCollection_List<TopoDS_Shape>::Iterator aItF(theList2); aItF.More() && isNeedJoin;
                 aItF.Next())
            {
              if (aItF.Value().IsSame(anExpf2.Current()))
                continue;
              TopoDS_Shape aF = Context()->Apply(aItF.Value());
              // aF = aF.Oriented(TopAbs_FORWARD);
              for (TopoDS_Iterator aIw(aF); aIw.More(); aIw.Next())
              {
                if (aIw.Value().ShapeType() != TopAbs_WIRE)
                  continue;
                TopoDS_Wire           wt   = TopoDS::Wire(aIw.Value());
                occ::handle<ShapeFix_Wire> SFW1 = new ShapeFix_Wire;
                SFW1->Load(wt);
                SFW1->FixReorder();
                occ::handle<ShapeExtend_WireData> atmpswd = SFW1->WireData();
                int             ind1    = atmpswd->Index(edge1);
                int             ind2    = atmpswd->Index(edge2);
                if (ind1 && ind2)
                {
                  isNeedJoin = ((ind1 - ind2) == 1 || (ind2 == atmpswd->NbEdges() && ind1 == 1));
                  break;
                }
              }
            }
            bool ReplaceFirst = true;
            if (isNeedJoin)
            {

              NCollection_List<TopoDS_Shape> aListF;
              for (NCollection_List<TopoDS_Shape>::Iterator aItlF(theList2); aItlF.More(); aItlF.Next())
              {
                TopoDS_Shape tmpF = Context()->Apply(aItlF.Value());
                aListF.Append(tmpF);
              }
              ReplaceFirst = JoinEdges(edge1, edge2, edge3, aListF);
            }
            else
              edge3 = TopoDS_Edge();
            if (edge3.IsNull())
            {
              index++;
              myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
            }
            else
            {
              // Record vertex replacements in the map
              TopoDS_Vertex oldV1 = SAE.FirstVertex(edge3), oldV2 = SAE.LastVertex(edge3);
              if (!theNewVertices.IsBound(oldV1))
              // smh#8
              {
                TopoDS_Shape emptyCopiedV1 = oldV1.EmptyCopied();
                theNewVertices.Bind(oldV1, TopoDS::Vertex(emptyCopiedV1));
              }
              if (!oldV1.IsSame(oldV2))
                if (!theNewVertices.IsBound(oldV2))
                // smh#8
                {
                  TopoDS_Shape emptyCopiedV2 = oldV2.EmptyCopied();
                  theNewVertices.Bind(oldV2, TopoDS::Vertex(emptyCopiedV2));
                }
              // To keep NM vertices belonging initial edges
              TopoDS_Iterator aItv(edge1, false);
              for (; aItv.More(); aItv.Next())
              {
                if (aItv.Value().Orientation() == TopAbs_INTERNAL
                    || aItv.Value().Orientation() == TopAbs_EXTERNAL)
                {
                  BRep_Builder  aB;
                  TopoDS_Vertex aOldV = TopoDS::Vertex(aItv.Value());
                  TopoDS_Vertex anewV =
                    ShapeAnalysis_TransferParametersProj::CopyNMVertex(aOldV, edge3, edge1);
                  aB.Add(edge3, anewV);
                  Context()->Replace(aOldV, anewV);
                }
              }

              for (aItv.Initialize(edge2, false); aItv.More(); aItv.Next())
              {
                if (aItv.Value().Orientation() == TopAbs_INTERNAL
                    || aItv.Value().Orientation() == TopAbs_EXTERNAL)
                {
                  BRep_Builder  aB;
                  TopoDS_Vertex aOldV = TopoDS::Vertex(aItv.Value());
                  TopoDS_Vertex anewV =
                    ShapeAnalysis_TransferParametersProj::CopyNMVertex(aOldV, edge3, edge2);
                  aB.Add(edge3, anewV);
                  Context()->Replace(aOldV, anewV);
                }
              }
              // Check for small resulting edge
              bool   newsmall = false;
              ShapeAnalysis_Wire SAW;
              SAW.Load(SFW->WireData());
              SAW.SetPrecision(Precision());
              // Make changes in WireData and Context
              if (ReplaceFirst)
              {
                Context()->Replace(edge1, edge3);
                Context()->Remove(edge2);
                SendWarning(edge2, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
              }
              else
              {
                Context()->Replace(edge2, edge3);
                Context()->Remove(edge1);
                SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
              }
              if (take_next)
              {
                SFW->WireData()->Set(edge3, next);
                newsmall = SAW.CheckSmall(next, Precision());
              }
              else
              {
                SFW->WireData()->Set(edge3, prev);
                newsmall = SAW.CheckSmall(prev, Precision());
              }
              SFW->WireData()->Remove(index);
              // Process changes in maps
              NCollection_List<TopoDS_Shape> theList;
              theList.Append(theList2);
              theEdgeToFaces.UnBind(edge1);
              theEdgeToFaces.UnBind(edge2);
              theEdgeToFaces.Bind(edge3, theList);
              if (theSmallEdges.Contains(edge1))
                theSmallEdges.Remove(edge1);
              if (theSmallEdges.Contains(edge2))
                theSmallEdges.Remove(edge2);
              if (newsmall)
                theSmallEdges.Add(edge3);
              for (NCollection_List<TopoDS_Shape>::Iterator itlf(theList); itlf.More(); itlf.Next())
              {
                TopoDS_Shape curface = itlf.Value();
                if (theFaceWithSmall.IsBound(curface))
                {
                  NCollection_List<TopoDS_Shape>& theEdges = theFaceWithSmall(curface);
                  if (newsmall)
                    theEdges.Append(edge3);
                  NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                  while (ite.More())
                  {
                    TopoDS_Shape iedge = ite.Value();
                    if (iedge.IsSame(edge1) || iedge.IsSame(edge2))
                      theEdges.Remove(ite);
                    else
                      ite.Next();
                  }
                  // Remove face without small edges from the map
                  if (!theEdges.Extent())
                    theFaceWithSmall.UnBind(curface);
                }
              }
              myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
            }
          }
          else if (aModeDrop)
          { // gka
            occ::handle<ShapeExtend_WireData> tempWire = new ShapeExtend_WireData();
            ShapeAnalysis_Wire           tempSaw;
            tempWire->Add(SFW->Wire());
            TopoDS_Edge remedge;
            if (take_next)
              remedge = edge1;
            else
              remedge = edge2;
            tempWire->Remove(index);
            tempSaw.Load(tempWire);
            int newindex = (index <= tempSaw.NbEdges() ? index : 1);
            tempSaw.CheckConnected(newindex, Precision());
            if (!tempSaw.LastCheckStatus(ShapeExtend_FAIL))
            {
              SFW->WireData()->Remove(index);
              TopoDS_Edge tmpedge1 = tempWire->Edge(newindex);
              TopoDS_Edge tmpedge2 =
                tempWire->Edge(newindex == 1 ? tempSaw.NbEdges() : (newindex - 1));
              NCollection_List<TopoDS_Shape> aL1;
              if (theEdgeToFaces.IsBound(tmpedge1))
                aL1 = theEdgeToFaces.Find(tmpedge1);
              NCollection_List<TopoDS_Shape> aL2;
              if (theEdgeToFaces.IsBound(tmpedge2))
                aL2 = theEdgeToFaces.Find(tmpedge2);
              SFW->FixConnected(newindex <= SFW->NbEdges() ? newindex : 1, Precision());
              SFW->FixDegenerated(newindex <= SFW->NbEdges() ? newindex : 1);
              TopoDS_Shape aTmpShape = Context()->Apply(tmpedge1); // for porting
              TopoDS_Edge  anewedge1 = TopoDS::Edge(aTmpShape);
              aTmpShape              = Context()->Apply(tmpedge2);
              TopoDS_Edge anewedge2  = TopoDS::Edge(aTmpShape);
              Context()->Remove(remedge);
              SendWarning(remedge, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
              if (theSmallEdges.Contains(remedge))
                theSmallEdges.Remove(remedge);
              theEdgeToFaces.UnBind(remedge);
              theEdgeToFaces.UnBind(tmpedge1);
              theEdgeToFaces.UnBind(tmpedge2);
              theEdgeToFaces.Bind(anewedge1, aL1);
              theEdgeToFaces.Bind(anewedge2, aL2);
              if (theSmallEdges.Contains(tmpedge1))
              {
                theSmallEdges.Remove(tmpedge1);
                theSmallEdges.Add(anewedge1);
                for (NCollection_List<TopoDS_Shape>::Iterator itlf(aL1); itlf.More(); itlf.Next())
                {
                  TopoDS_Shape                       curface  = itlf.Value();
                  NCollection_List<TopoDS_Shape>&              theEdges = theFaceWithSmall(curface);
                  NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                  while (ite.More())
                  {
                    TopoDS_Shape iedge = ite.Value();
                    if (iedge.IsSame(tmpedge1))
                    {
                      theEdges.Remove(ite);
                      theEdges.Append(anewedge1);
                    }
                    else
                      ite.Next();
                  }
                }
              }
              if (theSmallEdges.Contains(tmpedge2))
              {
                theSmallEdges.Remove(tmpedge2);
                theSmallEdges.Add(anewedge2);
                for (NCollection_List<TopoDS_Shape>::Iterator itlf(aL2); itlf.More(); itlf.Next())
                {
                  TopoDS_Shape                       curface  = itlf.Value();
                  NCollection_List<TopoDS_Shape>&              theEdges = theFaceWithSmall(curface);
                  NCollection_List<TopoDS_Shape>::Iterator ite(theEdges);
                  while (ite.More())
                  {
                    TopoDS_Shape iedge = ite.Value();
                    if (iedge.IsSame(tmpedge2))
                    {
                      theEdges.Remove(ite);
                      theEdges.Append(anewedge2);
                    }
                    else
                      ite.Next();
                  }
                }
              }
              myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
            }
            else
              index++;
          }
          else
          {
            // gka protection against removing circles
            TopoDS_Edge        ed = (take_next ? edge1 : edge2);
            ShapeAnalysis_Edge sae;
            occ::handle<Geom_Curve> c3d;
            double      f1, l1;
            if (sae.Curve3d(ed, c3d, f1, l1, false))
            {
              gp_Pnt p1, p2, p3;
              c3d->D0(f1, p1);
              c3d->D0(l1, p2);
              c3d->D0((f1 + l1) * 0.5, p3);

              if (p1.Distance(p3) > p1.Distance(p2))
              {
                index++;
                continue;
              }
            }
            if (take_next && theList2.Extent() == 1)
            { // gka
              TopoDS_Vertex V1 = SAE.FirstVertex(edge1), V2 = SAE.LastVertex(edge1);
              if (V1.IsSame(V2))
              {
                SFW->WireData()->Remove(index);
                Context()->Remove(edge1);
                SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                if (theSmallEdges.Contains(edge1))
                  theSmallEdges.Remove(edge1);
                theEdgeToFaces.UnBind(edge1);
                myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
              }
              else
                index++;
            }
            else if (!take_next && theList2.Extent() == 1)
            {
              TopoDS_Vertex V1 = SAE.FirstVertex(edge2), V2 = SAE.LastVertex(edge2);
              if (V1.IsSame(V2))
              {
                SFW->WireData()->Remove(index);
                Context()->Remove(edge2);
                SendWarning(edge2, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
                if (theSmallEdges.Contains(edge2))
                  theSmallEdges.Remove(edge2);
                theEdgeToFaces.UnBind(edge2);
                myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
              }
              else
                index++;
            }
            else
              index++;
          }
        }
        else
          index++;
      }
      if (SFW->NbEdges() == 1 && aModeDrop)
      {
        edge1 = SFW->WireData()->Edge(1);
        if (theSmallEdges.Contains(edge1))
        {
          SFW->WireData()->Remove(1);
          Context()->Remove(edge1);
          SendWarning(edge1, Message_Msg("FixWireframe.FixSmallEdges.MSG0"));
          theSmallEdges.Remove(edge1);
          theEdgeToFaces.UnBind(edge1);
          Context()->Remove(aWire);
          SendWarning(aWire, Message_Msg("FixWireframe.FixSmallEdges.MSG1"));
          myStatusSmallEdges |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
        }
      }
      else
      {
        SFW->FixConnected();
        Context()->Replace(aWire, SFW->Wire());
      }
    }
    // end enk block
    // Record vertex replacements in context
    for (NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itv(theNewVertices); itv.More(); itv.Next())
      Context()->Replace(itv.Key(), itv.Value());

    TopoDS_Shape shape = myShape;
    myShape.Nullify();
    myShape = Context()->Apply(shape);

    ShapeFix::SameParameter(myShape, false);

    return StatusSmallEdges(ShapeExtend_DONE);
  }

  return false;
}
