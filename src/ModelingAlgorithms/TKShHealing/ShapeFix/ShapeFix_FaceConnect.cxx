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
#include <BRepBuilderAPI_Sewing.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_FaceConnect.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_List.hxx>

#include <utility>

namespace
{
//=================================================================================================

void appendUniqueShape(NCollection_List<TopoDS_Shape>& theShapes, const TopoDS_Shape& theShape)
{
  for (const TopoDS_Shape& aStoredShape : theShapes)
  {
    if (aStoredShape.IsSame(theShape))
    {
      return;
    }
  }
  theShapes.Append(theShape);
}
} // namespace

//=================================================================================================

ShapeFix_FaceConnect::ShapeFix_FaceConnect() = default;

//=================================================================================================

bool ShapeFix_FaceConnect::Add(const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace)
{
  if (theFirstFace.IsNull() || theSecondFace.IsNull())
  {
    return false;
  }

  NCollection_List<TopoDS_Shape>& aConnectedFaces =
    myConnected.TryBound(theFirstFace, NCollection_List<TopoDS_Shape>());
  for (const TopoDS_Shape& aConnectedFace : aConnectedFaces)
  {
    if (aConnectedFace.IsSame(theSecondFace))
    {
      return true;
    }
  }
  aConnectedFaces.Append(theSecondFace);

  if (!theFirstFace.IsSame(theSecondFace))
  {
    myConnected.TryBound(theSecondFace, NCollection_List<TopoDS_Shape>()).Append(theFirstFace);
  }
  return true;
}

//=================================================================================================

TopoDS_Shell ShapeFix_FaceConnect::Build(const TopoDS_Shell& theShell,
                                         const double        theSewingTolerance,
                                         const double        theFixingTolerance)
{
  collectFreeEdges(theShell);
  sewConnectedFaces(theSewingTolerance);
  return rebuildShell(theShell, theSewingTolerance, theFixingTolerance);
}

//=================================================================================================

void ShapeFix_FaceConnect::Clear()
{
  myConnected.Clear();
  myOriFreeEdges.Clear();
  myResFreeEdges.Clear();
  myResSharEdges.Clear();
}

//=================================================================================================

void ShapeFix_FaceConnect::collectFreeEdges(const TopoDS_Shell& theShell)
{
  // Clear maps of free and shared edges
  myOriFreeEdges.Clear();
  myResFreeEdges.Clear();
  myResSharEdges.Clear();

  NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> theFreeEdges;
  TopoDS_Shape                                                                 theEdge, theFace;

  // Fill map of free edges / faces
  for (TopoDS_Iterator itf(theShell); itf.More(); itf.Next())
  {
    theFace = itf.Value();
    for (TopExp_Explorer expe(theFace, TopAbs_EDGE); expe.More(); expe.Next())
    {
      theEdge = expe.Current();
      if (!theFreeEdges.TryBind(theEdge, theFace))
      {
        theFreeEdges.UnBind(theEdge);
      }
    }
  }

  // Fill maps of original and resulting edges
  for (const auto& [aFreeEdge, aFreeFace] : theFreeEdges.Items())
  {
    // Get pair (face / free edge)
    theEdge = aFreeEdge, theFace = aFreeFace;
    // Process faces with bad connectivities only
    if (myConnected.Seek(theFace) != nullptr && !BRep_Tool::Degenerated(TopoDS::Edge(theEdge)))
    {
      // Add to the map of original free edges
      myOriFreeEdges.TryBound(theFace, NCollection_List<TopoDS_Shape>()).Append(theEdge);
      // Add to the maps of intermediate free and resulting edges
      NCollection_List<TopoDS_Shape> theFree;
      theFree.Append(theEdge);
      if (myResFreeEdges.TryBind(theEdge, std::move(theFree)))
      {
        myResSharEdges.TryBind(theEdge, NCollection_List<TopoDS_Shape>());
      }
    }
  }

  // Clear the temporary map of free edges
  theFreeEdges.Clear();
}

//=================================================================================================

void ShapeFix_FaceConnect::sewConnectedFaces(const double theSewingTolerance)
{
  if (myOriFreeEdges.IsEmpty())
  {
    return;
  }

  // Allocate array of faces to be sewed
  TopoDS_Shape                     theFirstFace, theSecondFace;
  NCollection_Array1<TopoDS_Shape> theFacesToSew(1, 2);
  int                              theNumOfFacesToSew = 0;
  bool                             skip_pair          = false;

  NCollection_List<TopoDS_Shape>::Iterator theOriginalIter, theResultsIter;
  TopoDS_Shape                             theAuxE, theOrigE, theAuxF;

  BRep_Builder theBuilder;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theProcessed;

  for (const auto& [aConnectedFace, aConnectedList] : myConnected.Items())
  {
    // Process first face only if it is in the map of faces / free edges
    theFirstFace = aConnectedFace;
    if (myOriFreeEdges.Seek(theFirstFace) != nullptr)
    {

      // Place first face into the array
      theFacesToSew.SetValue(1, theFirstFace);
      theNumOfFacesToSew = 1;
      // Create the list of processed faces
      NCollection_List<TopoDS_Shape> theProcessedList;

      // Explore the list of connected faces
      for (const TopoDS_Shape& aConnectedFaceCandidate : aConnectedList)
      {
        // Process second face only if it is in the map of faces / free edges
        theSecondFace = aConnectedFaceCandidate;
        if (myOriFreeEdges.Seek(theSecondFace) != nullptr)
        {

          // Place second face into the array
          theFacesToSew.SetValue(2, theSecondFace);
          // Add second face to the list of processed faces
          theProcessedList.Append(theSecondFace);

          // Skip the pair if already processed
          skip_pair                                             = false;
          const NCollection_List<TopoDS_Shape>* aProcessedFaces = theProcessed.Seek(theSecondFace);
          if (aProcessedFaces != nullptr)
          {
            for (const TopoDS_Shape& aProcessedFace : *aProcessedFaces)
            {
              if (theFirstFace.IsSame(aProcessedFace))
              {
                skip_pair = true;
                break;
              }
            }
          }
          if (!skip_pair)
          {

            // Process second face for the pair of different faces only
            if (theFirstFace.IsSame(theSecondFace))
            {
            }
            else
            {
              theNumOfFacesToSew = 2;
            }

            NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
                                  theSewerWires;
            BRepBuilderAPI_Sewing theSewer(theSewingTolerance);

            // Prepare set of faces containing free edges
            int i = 1;
            for (i = 1; i <= theNumOfFacesToSew; i++)
            {
              // Prepare empty face to fill with free edges
              TopoDS_Shape theFaceToSew = theFacesToSew(i);
              theAuxF                   = theFaceToSew.EmptyCopied();
              // Fill empty face with free edges
              for (theOriginalIter.Initialize(myOriFreeEdges(theFaceToSew)); theOriginalIter.More();
                   theOriginalIter.Next())
              {
                for (theResultsIter.Initialize(myResFreeEdges(theOriginalIter.Value()));
                     theResultsIter.More();
                     theResultsIter.Next())
                {
                  // Bind free edge to wire to find results later
                  theAuxE = theResultsIter.Value();
                  TopoDS_Wire theAuxW;
                  theBuilder.MakeWire(theAuxW);
                  theBuilder.Add(theAuxW, theAuxE);
                  theBuilder.Add(theAuxF, theAuxW);
                  theSewerWires.Bound(theAuxE, theAuxW);
                  theSewer.Add(theAuxW);
                }
              }
              // Add constructed face to sewer
              theSewer.Add(theAuxF);
            }

            // Perform sewing on the list of free edges
            bool sewing_ok = true;
            {
              try
              {
                OCC_CATCH_SIGNALS
                theSewer.Perform();
              }
              catch (Standard_Failure const&)
              {
                sewing_ok = false;
              }
            }
            if (sewing_ok)
            {
              if (theSewer.SewedShape().IsNull())
              {
                sewing_ok = false;
              }
            }

            if (sewing_ok)
            {
              NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>
                theResultEdges;

              // Find modified edges for the faces
              for (i = 1; i <= theNumOfFacesToSew; i++)
              {
                for (theOriginalIter.Initialize(myOriFreeEdges(theFacesToSew(i)));
                     theOriginalIter.More();
                     theOriginalIter.Next())
                {
                  // Get original free edge
                  theOrigE                                       = theOriginalIter.Value();
                  NCollection_List<TopoDS_Shape>& theOldFreeList = myResFreeEdges(theOrigE);
                  theResultsIter.Initialize(theOldFreeList);
                  while (theResultsIter.More())
                  {
                    theAuxE = theSewerWires(theResultsIter.Value());
                    // Process modified edges
                    if (theSewer.IsModified(theAuxE))
                    {
                      // Fill map of result edges
                      for (TopExp_Explorer expe(theSewer.Modified(theAuxE), TopAbs_EDGE);
                           expe.More();
                           expe.Next())
                      {
                        theAuxE = expe.Current();
                        // Check edge for being shared
                        if (theResultEdges.TryBind(theAuxE, theOrigE))
                        {
                          continue;
                        }
                        // Edge was shared - move in results list
                        myResSharEdges(theResultEdges(theAuxE)).Append(theAuxE);
                        myResSharEdges(theOrigE).Append(theAuxE);
                        theResultEdges.UnBind(theAuxE);
                      }
                      // Remove modified free edge from the list
                      theOldFreeList.Remove(theResultsIter);
                    }
                    else
                    {
                      theResultsIter.Next();
                    }
                  }
                }
              }

              // Put free edges back to the lists of results
              for (const auto& [aResultEdge, anOriginalEdge] : theResultEdges.Items())
              {
                myResFreeEdges(anOriginalEdge).Append(aResultEdge);
              }
            }
          }
        }
      }

      // Bind the list of processed faces to the processed face
      theProcessed.TryBind(theFirstFace, std::move(theProcessedList));
    }
  }

  // Clear the temporary map of processed faces
  theProcessed.Clear();
}

//=================================================================================================

TopoDS_Shell ShapeFix_FaceConnect::rebuildShell(const TopoDS_Shell& theShell,
                                                const double        theSewingTolerance,
                                                const double        theFixingTolerance)
{
  if (myOriFreeEdges.IsEmpty())
  {
    return theShell;
  }

  TopoDS_Shell                             result = theShell;
  NCollection_List<TopoDS_Shape>::Iterator theResultsIter;
  TopoDS_Shape                             theAuxE;
  BRep_Builder                             theBuilder;

  NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> theRepEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                               theRepVertices;
  NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> theOldVertices;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theNewVertices;

  // Replace old edges by resulting ones
  TopoDS_Wire   theNewW;
  TopoDS_Vertex theOldV1, theOldV2, theNewV1, theNewV2, theNewV;
  gp_Pnt        theOldP1, theOldP2;
  double        dist1, dist2, curdist1, curdist2;
  for (const NCollection_List<TopoDS_Shape>& anOriginalEdges : myOriFreeEdges)
  {
    // Iterate on original free edges
    for (const TopoDS_Shape& anOriginalEdge : anOriginalEdges)
    {
      TopoDS_Edge theOldE = TopoDS::Edge(anOriginalEdge);

      // Prepare empty wire to add new edges for reshape
      theBuilder.MakeWire(theNewW);

      // Explore new edges and vertices
      bool emptywire = true;
      for (int i = 1; i <= 2; i++)
      {
        // Select list of free or shared edges
        if (i == 1)
        {
          theResultsIter.Initialize(myResFreeEdges(theOldE));
        }
        else
        {
          theResultsIter.Initialize(myResSharEdges(theOldE));
        }
        // Iterate on new edges
        for (; theResultsIter.More(); theResultsIter.Next())
        {
          theAuxE = theResultsIter.Value();
          if (!theAuxE.IsSame(theOldE))
          {
            // Add new edge to the wire
            theBuilder.Add(theNewW, theAuxE);
            emptywire = false;
          }
        }
      }

      if (!emptywire)
      {

        // Get vertices on old and new edges
        TopExp::Vertices(theOldE, theOldV1, theOldV2);
        theOldP1 = BRep_Tool::Pnt(theOldV1);
        theOldP2 = BRep_Tool::Pnt(theOldV2);

        // Process vertices for replacing
        dist1 = -1.;
        dist2 = -1.;
        for (TopExp_Explorer expv(theNewW, TopAbs_VERTEX); expv.More(); expv.Next())
        {
          TopoDS_Vertex theNewVtx = TopoDS::Vertex(expv.Current());
          gp_Pnt        theNewPt  = BRep_Tool::Pnt(theNewVtx);
          curdist1                = theOldP1.Distance(theNewPt);
          curdist2                = theOldP2.Distance(theNewPt);
          if (dist1 < 0 || curdist1 < dist1)
          {
            dist1    = curdist1;
            theNewV1 = theNewVtx;
          }
          if (dist2 < 0 || curdist2 < dist2)
          {
            dist2    = curdist2;
            theNewV2 = theNewVtx;
          }
        }

        // Place results in map for replacing
        if (!theOldV1.IsSame(theNewV1))
        {
          appendUniqueShape(theRepVertices.TryBound(theOldV1, NCollection_List<TopoDS_Shape>()),
                            theNewV1);
        }
        if (!theOldV2.IsSame(theNewV2))
        {
          appendUniqueShape(theRepVertices.TryBound(theOldV2, NCollection_List<TopoDS_Shape>()),
                            theNewV2);
        }

        // Bind edge to replace
        theRepEdges.TryBind(theOldE, theNewW);
      }
    }
  }

  if (!theRepEdges.IsEmpty())
  {

    occ::handle<ShapeBuild_ReShape> theReShape = new ShapeBuild_ReShape;

    // Replace edges
    for (const auto& [anOldEdge, aNewWire] : theRepEdges.Items())
    {
      theReShape->Replace(anOldEdge /*.Oriented(TopAbs_FORWARD)*/,
                          aNewWire /*.Oriented(TopAbs_FORWARD)*/);
    }
    // smh#8
    TopoDS_Shape tmpReShape = theReShape->Apply(result);
    result                  = TopoDS::Shell(tmpReShape);
    if (theReShape->Status(ShapeExtend_OK))
    {
    }
    else if (theReShape->Status(ShapeExtend_FAIL1))
    {
    }
    else
    {

      occ::handle<ShapeFix_Wire>        SFW = new ShapeFix_Wire;
      occ::handle<ShapeFix_Face>        SFF = new ShapeFix_Face;
      ShapeAnalysis_Edge                SAE;
      double                            f, l;
      occ::handle<Geom2d_Curve>         c2d;
      occ::handle<ShapeExtend_WireData> sewd;

      // Perform necessary fixes on subshapes
      // smh#8
      TopoDS_Shape emptyCopiedShell = result.EmptyCopied();
      TopoDS_Shell theShell         = TopoDS::Shell(emptyCopiedShell);
      for (TopoDS_Iterator itf1(result); itf1.More(); itf1.Next())
      {
        TopoDS_Face newface = TopoDS::Face(itf1.Value());
        // smh#8
        TopoDS_Shape emptyCopiedFace = newface.EmptyCopied();
        TopoDS_Face  EmpFace         = TopoDS::Face(emptyCopiedFace);
        for (TopoDS_Iterator itw(newface); itw.More(); itw.Next())
        {
          if (itw.Value().ShapeType() != TopAbs_WIRE)
          {
            continue;
          }
          TopoDS_Wire theWire = TopoDS::Wire(itw.Value());

          sewd = new ShapeExtend_WireData(theWire);
          ShapeAnalysis_WireOrder SAWO(false, 0);
          for (int i = 1; i <= sewd->NbEdges(); i++)
          {

            // smh#8
            TopoDS_Shape tmpFace = EmpFace.Oriented(TopAbs_FORWARD);
            if (!SAE.PCurve(sewd->Edge(i), TopoDS::Face(tmpFace), c2d, f, l))
            {
              continue;
            }
            SAWO.Add(c2d->Value(f).XY(), c2d->Value(l).XY());
          }
          SAWO.Perform();

          SFW->Load(sewd);
          SFW->FixReorder(SAWO);
          SFW->FixReorder();

          SFW->SetFace(EmpFace);
          SFW->SetPrecision(theFixingTolerance);
          SFW->SetMaxTolerance(theSewingTolerance);

          SFW->FixEdgeCurves();
          SFW->FixSelfIntersection();
          theWire = SFW->Wire();
          theBuilder.Add(EmpFace, theWire);
        }
        // #ifdef AIX  CKY : applies to all platforms
        SFF->Init(EmpFace);
        //	  SFF->Init(TopoDS::Face(EmpFace));

        NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
          MapWires;
        MapWires.Clear();
        if (SFF->FixOrientation(MapWires))
        {
          EmpFace = SFF->Face();
        }
        theBuilder.Add(theShell, EmpFace);
      }
      theShell.Closed(BRep_Tool::IsClosed(theShell));
      result = theShell;

      if (!theRepVertices.IsEmpty())
      {

        // Prepare vertices to replace
        TopoDS_Shape theOld, theNew, theRep, theAux;
        for (const auto& [anOldVertex, aReplacementList] : theRepVertices.Items())
        {
          // Get the old vertex, create empty list of replaced vertices
          theOld = anOldVertex;
          NCollection_List<TopoDS_Shape> theNewList;
          // Explore the list of new vertices
          for (const TopoDS_Shape& aReplacement : aReplacementList)
          {
            theNew = aReplacement;
            if (theOldVertices.TryBind(theNew, theOld))
            {
              theNewList.Append(theNew);
              continue;
            }

            // Vertex has a replacing vertex in the map
            theRep = theOldVertices(theNew);
            if (!theRep.IsSame(theOld))
            {
              // Vertex is not in current list
              theOldVertices.Bound(theRep, theOld);
              theNewList.Append(theRep);
              for (const TopoDS_Shape& aRelatedVertex : theNewVertices(theRep))
              {
                theAux                 = aRelatedVertex;
                theOldVertices(theAux) = theOld;
                theNewList.Append(theAux);
              }
              theNewVertices.UnBind(theRep);
            }
          }
          theNewVertices.TryBind(theOld, std::move(theNewList));
        }

        // Update vertices positions and tolerances
        TopoDS_Vertex theNewVert, theOldVert;
        for (const auto& [aNewVertex, anOldVertexList] : theNewVertices.Items())
        {
          theNewVert = TopoDS::Vertex(aNewVertex);
          // Calculate the vertex position
          gp_Pnt theLBound, theRBound, thePosition;
          theLBound = theRBound = BRep_Tool::Pnt(theNewVert);
          for (const TopoDS_Shape& anOldVertex : anOldVertexList)
          {
            thePosition = BRep_Tool::Pnt(TopoDS::Vertex(anOldVertex));
            double val  = thePosition.X();
            if (val < theLBound.X())
            {
              theLBound.SetX(val);
            }
            else if (val > theRBound.X())
            {
              theRBound.SetX(val);
            }
            val = thePosition.Y();
            if (val < theLBound.Y())
            {
              theLBound.SetY(val);
            }
            else if (val > theRBound.Y())
            {
              theRBound.SetY(val);
            }
            val = thePosition.Z();
            if (val < theLBound.Z())
            {
              theLBound.SetZ(val);
            }
            else if (val > theRBound.Z())
            {
              theRBound.SetZ(val);
            }
          }
          thePosition         = gp_Pnt((theLBound.XYZ() + theRBound.XYZ()) / 2.);
          double theTolerance = 0., curtoler;
          // Calculate the vertex tolerance
          for (const TopoDS_Shape& anOldVertex : anOldVertexList)
          {
            theOldVert = TopoDS::Vertex(anOldVertex);
            curtoler =
              thePosition.Distance(BRep_Tool::Pnt(theOldVert)) + BRep_Tool::Tolerance(theOldVert);
            if (curtoler > theTolerance)
            {
              theTolerance = curtoler;
            }
          }
          curtoler =
            thePosition.Distance(BRep_Tool::Pnt(theNewVert)) + BRep_Tool::Tolerance(theNewVert);
          if (curtoler > theTolerance)
          {
            theTolerance = curtoler;
          }
          theBuilder.UpdateVertex(theNewVert, thePosition, theTolerance);
        }

        // Replace vertices
        theReShape->Clear();
        for (const auto& [anOldVertex, aNewVertex] : theOldVertices.Items())
        {
          theReShape->Replace(anOldVertex.Oriented(TopAbs_FORWARD),
                              aNewVertex.Oriented(TopAbs_FORWARD));
        }
        // smh#8
        TopoDS_Shape tmpshape = theReShape->Apply(result);
        result                = TopoDS::Shell(tmpshape);

        if (theReShape->Status(ShapeExtend_FAIL1))
        {
        }
      }
    }
  }

  return result;
}
