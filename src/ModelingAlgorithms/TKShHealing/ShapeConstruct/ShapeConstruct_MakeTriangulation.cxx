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
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeConstruct_MakeTriangulation.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_HSequence.hxx>

//=================================================================================================

bool IsRightContour(const NCollection_Sequence<gp_Pnt>& pts, const double prec)
{
  int len = pts.Length();
  if (len < 4)
    return true;

  NCollection_Array1<gp_Pnt> thePts(1, len);
  int                        i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= len; i++)
    thePts(i) = pts(i);

  gp_XYZ Norm(0, 0, 0);
  if (ShapeAnalysis_Curve::IsPlanar(thePts, Norm, prec))
  {

    // Make polygonal wire from points
    BRepBuilderAPI_MakePolygon mkPoly;
    for (i = 1; i <= len; i++)
      mkPoly.Add(thePts(i));
    mkPoly.Close();
    mkPoly.Build();
    if (mkPoly.IsDone())
    {

      // Create mean plane
      gp_XYZ center(0, 0, 0);
      for (i = 1; i <= len; i++)
        center += thePts(i).XYZ();
      center /= len;
      gp_Pnt                  pc(center);
      gp_Dir                  pd(Norm);
      gp_Pln                  pl(pc, pd);
      occ::handle<Geom_Plane> thePlane = new Geom_Plane(pl);

      BRep_Builder B;
      TopoDS_Face  theFace;
      B.MakeFace(theFace, thePlane, Precision::Confusion());
      TopoDS_Wire theWire = mkPoly.Wire();
      B.Add(theFace, theWire);
      occ::handle<ShapeAnalysis_Wire> saw = new ShapeAnalysis_Wire(theWire, theFace, prec);
      return !saw->CheckSelfIntersection();
    }
  }

  return false;
}

//=================================================================================================

gp_Vec MeanNormal(const NCollection_Array1<gp_Pnt>& pts)
{
  int len = pts.Length();
  if (len < 3)
    return gp_Vec(0, 0, 0);

  int    i;
  gp_XYZ theCenter(0, 0, 0);
  for (i = 1; i <= len; i++)
    theCenter += pts(i).XYZ();
  theCenter /= len;

  gp_XYZ theNorm(0, 0, 0);
  for (i = 1; i <= len; i++)
  {
    gp_Vec v1(pts(i).XYZ() - theCenter);
    gp_Vec v2(pts((i == len) ? 1 : i + 1).XYZ() - theCenter);
    theNorm += (v1 ^ v2).XYZ();
  }

  return gp_Vec(theNorm).Normalized();
}

//=================================================================================================

ShapeConstruct_MakeTriangulation::ShapeConstruct_MakeTriangulation(
  const NCollection_Array1<gp_Pnt>& pnts,
  const double                      prec)
{
  myPrecision = (prec > 0.0) ? prec : Precision::Confusion();
  // Make polygonal wire from points
  BRepBuilderAPI_MakePolygon mkPoly;
  for (int i = pnts.Lower(); i <= pnts.Upper(); i++)
    mkPoly.Add(pnts(i));
  mkPoly.Close();
  mkPoly.Build();
  if (mkPoly.IsDone())
  {
    myWire = mkPoly.Wire();
    Build();
  }
}

//=================================================================================================

ShapeConstruct_MakeTriangulation::ShapeConstruct_MakeTriangulation(const TopoDS_Wire& wire,
                                                                   const double       prec)
{
  myPrecision = (prec > 0.0) ? prec : Precision::Confusion();
  myWire      = wire;
  Build();
}

//=================================================================================================

void ShapeConstruct_MakeTriangulation::Build(const Message_ProgressRange& /*theRange*/)
{
  if (myShape.IsNull())
  {
    // Triangulate polygonal wire
    if (!myWire.IsNull())
      Triangulate(myWire);
  }
}

//=================================================================================================

bool ShapeConstruct_MakeTriangulation::IsDone() const
{
  return (!myShape.IsNull());
}

//=================================================================================================

void ShapeConstruct_MakeTriangulation::Triangulate(const TopoDS_Wire& wire)
{
  // Fill sequence of edges
  occ::handle<NCollection_HSequence<TopoDS_Shape>> theEdges =
    new NCollection_HSequence<TopoDS_Shape>;
  for (TopoDS_Iterator ite(wire); ite.More(); ite.Next())
    theEdges->Append(ite.Value());
  int NbEdges = theEdges->Length();

  if (NbEdges < 4)
    AddFacet(wire);
  else
  {

    // Fill array of end points
    ShapeAnalysis_Edge                       sae;
    occ::handle<NCollection_HArray1<gp_Pnt>> theAPnt = new NCollection_HArray1<gp_Pnt>(1, NbEdges);
    for (int i = 1; i <= NbEdges; i++)
      theAPnt->SetValue(i, BRep_Tool::Pnt(sae.FirstVertex(TopoDS::Edge(theEdges->Value(i)))));

    TopoDS_Wire theNewWire;

    // Check planarity on array of points
    gp_XYZ Norm(0, 0, 0);
    if (ShapeAnalysis_Curve::IsPlanar(theAPnt->Array1(), Norm, myPrecision))
      AddFacet(wire);
    else
    {

      // Current contour is not planar - triangulate
      NCollection_Sequence<int> theSegments;
      int                       cindex = 1, seqFlag = 1;
      NCollection_Array1<int>   llimits(1, 2), rlimits(1, 2);
      llimits.Init(NbEdges + 1);
      rlimits.Init(0);

      // Find all "good" planar segments
      while (seqFlag)
      {

        // Set up limits for current sequence
        int llimit = llimits(seqFlag);
        int rlimit = rlimits(seqFlag);
        if (rlimit <= llimit)
          rlimit += NbEdges;

        // Check stop condition
        if ((rlimit - llimit) > (NbEdges - 2))
          break;

        NCollection_Sequence<gp_Pnt> theSPnt;

        // Add 3 points of minimal facet
        int lindex = (cindex == 1 ? NbEdges : cindex - 1);
        int rindex = (cindex == NbEdges ? 1 : cindex + 1);
        theSPnt.Append(theAPnt->Value(lindex));
        theSPnt.Append(theAPnt->Value(cindex));
        theSPnt.Append(theAPnt->Value(rindex));

        // Try prepending points
        bool isGood = true;
        while (isGood)
        {
          cindex = (lindex == 1 ? NbEdges : lindex - 1);
          if (cindex == rindex)
          {
            AddFacet(wire);
            return;
          }
          int cid = cindex;
          if (rlimit > NbEdges && cid <= llimit)
            cid += NbEdges;
          if (cid > llimit && cid < rlimit)
            isGood = false;
          if (isGood)
          {
            theSPnt.Prepend(theAPnt->Value(cindex));
            isGood = IsRightContour(theSPnt, myPrecision);
            if (isGood)
              lindex = cindex;
            else
              theSPnt.Remove(1);
          }
        }

        // Try appending points
        isGood = true;
        while (isGood)
        {
          cindex = (rindex == NbEdges ? 1 : rindex + 1);
          if (cindex == lindex)
          {
            AddFacet(wire);
            return;
          }
          int cid = cindex;
          if (rlimit > NbEdges && cid <= llimit)
            cid += NbEdges;
          if (cid > llimit && cid < rlimit)
            isGood = false;
          if (isGood)
          {
            theSPnt.Append(theAPnt->Value(cindex));
            isGood = IsRightContour(theSPnt, myPrecision);
            if (isGood)
              rindex = cindex;
            else
              theSPnt.Remove(theSPnt.Length());
          }
        }

        // Record new planar segment
        theSegments.Append(lindex);
        theSegments.Append(rindex);

        // Set up new limits
        cindex           = rindex;
        rlimits(seqFlag) = rindex;
        if (llimit > NbEdges)
          llimits(seqFlag) = lindex;

        // Set up sequence index
        seqFlag = (seqFlag == 1) ? 2 : 1;
      }

      cindex = theSegments.Length();
      if (cindex % 4 == 0)
      {
        gp_Vec theBaseNorm = MeanNormal(theAPnt->Array1());
        // Identify sequence of matching facets
        int    len = cindex / 2, lindex, rindex, seqlen, j;
        double theC, theC1 = 0.0, theC2 = 0.0;
        int    ii; // svv Jan11 2000 : porting on DEC
                   // skl : change "i" to "ii"
        for (ii = 0; ii < len; ii++)
        {
          // Compute normal collinearity for facet
          lindex = theSegments(ii * 2 + 1);
          rindex = theSegments(ii * 2 + 2);
          seqlen = ((rindex > lindex) ? rindex - lindex + 1 : NbEdges - lindex + rindex + 1);
          NCollection_Array1<gp_Pnt> theArr(1, seqlen);
          for (j = 1; j <= seqlen; j++)
          {
            theArr(j) = theAPnt->Value(lindex);
            lindex++;
            if (lindex > NbEdges)
              lindex = 1;
          }
          theC = theBaseNorm * MeanNormal(theArr);
          if (ii % 2)
            theC2 += theC;
          else
            theC1 += theC;
        }
        int best1 = ((theC1 > theC2) ? 0 : 2);

        // Add "best" planar facets
        BRep_Builder B;
        TopoDS_Wire  theFacetWire;
        TopoDS_Edge  theLEdge, theSLEdge;
        len = cindex / 4;
        // Check for special case - 1 shared line
        bool special = (len == 2 && theSegments(best1 + 1) == theSegments(4 + best1 + 2)
                        && theSegments(best1 + 2) == theSegments(4 + best1 + 1));
        int  pindex  = theSegments((len - 1) * 4 + best1 + 2);
        for (ii = 0; ii < len; ii++)
        {
          lindex = theSegments(ii * 4 + best1 + 1);
          rindex = theSegments(ii * 4 + best1 + 2);
          if (special && !theSLEdge.IsNull())
          {
            TopoDS_Shape aLocalShape = theSLEdge.Reversed();
            theLEdge                 = TopoDS::Edge(aLocalShape);
            //	    theLEdge = TopoDS::Edge(theSLEdge.Reversed());
          }
          else
          {
            TopoDS_Vertex v1 = sae.FirstVertex(TopoDS::Edge(theEdges->Value(lindex)));
            TopoDS_Vertex v2 = sae.FirstVertex(TopoDS::Edge(theEdges->Value(rindex)));
            v1.Orientation(TopAbs_FORWARD);
            v2.Orientation(TopAbs_REVERSED);
            theLEdge  = BRepBuilderAPI_MakeEdge(v1, v2);
            theSLEdge = theLEdge;
          }
          // Create new facet wire
          B.MakeWire(theFacetWire);
          B.Add(theFacetWire, theLEdge.Reversed());
          int cur_edge = lindex;
          while (cur_edge != rindex)
          {
            TopoDS_Edge theNEdge = TopoDS::Edge(theEdges->Value(cur_edge));
            B.Add(theFacetWire, theNEdge);
            if (cur_edge == NbEdges)
              cur_edge = 1;
            else
              cur_edge++;
          }
          AddFacet(theFacetWire);
          if (!special)
          {
            // Add edges to the new wire
            if (theNewWire.IsNull())
              B.MakeWire(theNewWire);
            cur_edge = pindex;
            while (cur_edge != lindex)
            {
              TopoDS_Edge theNEdge = TopoDS::Edge(theEdges->Value(cur_edge));
              B.Add(theNewWire, theNEdge);
              if (cur_edge == NbEdges)
                cur_edge = 1;
              else
                cur_edge++;
            }
            B.Add(theNewWire, theLEdge);
            pindex = rindex;
          }
        }
      }

      // Clear storage variables
      theEdges.Nullify();
      theAPnt.Nullify();

      // Triangulate the rest of edges
      if (!theNewWire.IsNull())
        Triangulate(theNewWire);
    }
  }
}

//=================================================================================================

void ShapeConstruct_MakeTriangulation::AddFacet(const TopoDS_Wire& wire)
{
  if (wire.IsNull())
    return;

  // Fill sequence of points
  ShapeAnalysis_Edge           sae;
  NCollection_Sequence<gp_Pnt> pts;
  for (TopoDS_Iterator ite(wire); ite.More(); ite.Next())
    pts.Append(BRep_Tool::Pnt(sae.FirstVertex(TopoDS::Edge(ite.Value()))));
  int i, nbp = pts.Length();
  if (nbp < 3)
    return;

  // Create mean plane
  double cMod, maxMod = 0.0;
  gp_XYZ maxVec, Normal(0, 0, 0);
  for (i = 1; i <= nbp; i++)
  {
    gp_XYZ vb(pts(i).XYZ());
    gp_XYZ v1(pts(i == nbp ? 1 : i + 1).XYZ() - vb);
    cMod = v1.SquareModulus();
    if (cMod == 0.0)
      continue;
    else if (cMod > maxMod)
    {
      maxMod = cMod;
      maxVec = v1;
    }
    gp_XYZ v2(pts(i == 1 ? nbp : i - 1).XYZ() - vb);
    cMod = v2.SquareModulus();
    if (cMod == 0.0)
      continue;
    else if (cMod > maxMod)
    {
      maxMod = cMod;
      maxVec = v2;
    }
    Normal += gp_XYZ(v1 ^ v2);
  }
  if (Normal.SquareModulus() == 0.0)
  {
    if (maxMod == 0.0)
      Normal = gp_XYZ(0, 0, 1);
    else if (maxVec.X() != 0.0)
      Normal = gp_XYZ(-maxVec.Y() / maxVec.X(), 1, 0);
    else if (maxVec.Y() != 0.0)
      Normal = gp_XYZ(0, -maxVec.Z() / maxVec.Y(), 1);
    else
      Normal = gp_XYZ(1, 0, 0);
  }

  gp_Pln                  Pln(pts(1), gp_Dir(Normal));
  occ::handle<Geom_Plane> thePlane = new Geom_Plane(Pln);

  // Mean plane created - build face
  BRep_Builder B;
  TopoDS_Face  theFace;
  B.MakeFace(theFace, thePlane, Precision::Confusion());
  B.Add(theFace, wire);

  // Add new face to the shell
  if (myShape.IsNull())
    myShape = theFace;
  else
  {
    if (myShape.ShapeType() == TopAbs_FACE)
    {
      TopoDS_Face  firstFace = TopoDS::Face(myShape);
      TopoDS_Shell theShell;
      B.MakeShell(theShell);
      myShape = theShell;
      B.Add(myShape, firstFace);
    }
    B.Add(myShape, theFace);
  }
}
