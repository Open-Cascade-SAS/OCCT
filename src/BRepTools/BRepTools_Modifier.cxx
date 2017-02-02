// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

// IFV 04.06.99 - PRO18974 - processing of INTERNAL shapes.

#include <BRepTools_Modification.hxx>
#include <BRepTools_Modifier.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#if 0
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#endif

#include <Geom2d_Line.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>

#include <gp.hxx>

#include <Standard_NullObject.hxx>
#include <gp_Trsf.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Message_ProgressSentry.hxx>
#include <Geom_Surface.hxx>

static void SetShapeFlags(const TopoDS_Shape& theInSh, TopoDS_Shape& theOutSh);

//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier (Standard_Boolean theMutableInput):
myDone(Standard_False), myMutableInput (theMutableInput)
{}

//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier (const TopoDS_Shape& S) :
myShape(S),myDone(Standard_False), myMutableInput (Standard_False)
{
  Put(S);
}

//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier
  (const TopoDS_Shape& S,
   const Handle(BRepTools_Modification)& M)
   : myShape(S), myDone(Standard_False), 
     myMutableInput (Standard_False)
{
  Put(S);
  Perform(M);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepTools_Modifier::Init(const TopoDS_Shape& S)
{
  myShape = S;
  myDone = Standard_False;
  Put(S);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
#ifdef DEBUG_Modifier
static TopTools_IndexedMapOfShape MapE, MapF;
#endif

void BRepTools_Modifier::Perform(const Handle(BRepTools_Modification)& M, const Handle(Message_ProgressIndicator) & aProgress)
{
  if (myShape.IsNull()) {
    throw Standard_NullObject();
  }
#ifdef DEBUG_Modifier
  MapE.Clear(); MapF.Clear();
  TopExp::MapShapes(myShape, TopAbs_EDGE, MapE);
  TopExp::MapShapes(myShape, TopAbs_FACE, MapF);
#endif
  TopTools_DataMapIteratorOfDataMapOfShapeShape theIter(myMap);

  Message_ProgressSentry aPSentry(aProgress, "Converting Shape", 0, 2, 1);

  TopTools_IndexedDataMapOfShapeListOfShape aMVE, aMEF;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, aMEF);

  CreateNewVertices(aMVE, M);

  FillNewCurveInfo(aMEF, M);

  FillNewSurfaceInfo(M);

  if (!myMutableInput)
    CreateOtherVertices(aMVE, aMEF, M);

  Standard_Boolean aNewGeom;
  Rebuild(myShape, M, aNewGeom, aProgress);

  if (!aPSentry.More())
  {
    // The processing was broken
    return;
  }

  aPSentry.Next();

  if (myShape.ShapeType() == TopAbs_FACE) {
    if (myShape.Orientation() == TopAbs_REVERSED) {
      myMap(myShape).Reverse();
    }
    else{
      myMap(myShape).Orientation(myShape.Orientation());
    } 
  }
  else {
    myMap(myShape).Orientation(myShape.Orientation());
  }

  // Update the continuities


  BRep_Builder aBB;

/*
  Standard_Boolean RecomputeTriangles = Standard_False;
  Standard_Real MaxDeflection = RealFirst();
  Handle(Poly_Triangulation) Tr;
  Handle(Poly_Polygon3D) Po;
  TopLoc_Location Loc;
*/

  for (int ii = 1; ii <= aMEF.Extent(); ii++)
  {
    const TopoDS_Edge& CurE = TopoDS::Edge(aMEF.FindKey(ii));
    const TopoDS_Edge& NewE = TopoDS::Edge(myMap(CurE));
    if (!CurE.IsSame(NewE)) {
      TopTools_ListIteratorOfListOfShape it;
      it.Initialize(aMEF.FindFromKey(CurE));
      TopoDS_Face F1,F2;
      while (it.More() && F2.IsNull()) {
        if (F1.IsNull()) {
          F1 = TopoDS::Face(it.Value());
        }
        else {
          F2 = TopoDS::Face(it.Value());
        }
        it.Next();
      }
      if (!F2.IsNull()) {
        const TopoDS_Face& newf1  = TopoDS::Face(myMap(F1));
        const TopoDS_Face& newf2  = TopoDS::Face(myMap(F2));
        GeomAbs_Shape Newcont = M->Continuity(CurE,F1,F2,NewE,newf1,newf2);
        if (Newcont > GeomAbs_C0) {
          aBB.Continuity(NewE,newf1,newf2,Newcont);
        }
      }
    }
    theIter.Next();
  }
/*
  if (RecomputeTriangles) {
    BRepMesh_IncrementalMesh(myMap(myShape),MaxDeflection);
  }
*/

  myDone = Standard_True;

}

//=======================================================================
//function : Put
//purpose  : 
//=======================================================================

void BRepTools_Modifier::Put(const TopoDS_Shape& S)
{
  if (!myMap.IsBound(S)) {
    myMap.Bind(S,TopoDS_Shape());
    for(TopoDS_Iterator theIterator(S,Standard_False);theIterator.More();theIterator.Next()) {

      Put(theIterator.Value());
    }
  }
}

//=======================================================================
//function : Rebuild
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_Modifier::Rebuild
  (const TopoDS_Shape& S,
   const Handle(BRepTools_Modification)& M,
   Standard_Boolean& theNewGeom,
   const Handle(Message_ProgressIndicator)& aProgress)
{
#ifdef DEBUG_Modifier
  int iF = MapF.Contains(S) ? MapF.FindIndex(S) : 0;
  int iE = MapE.Contains(S) ? MapE.FindIndex(S) : 0;
#endif
  TopAbs_ShapeEnum ts = S.ShapeType();
  TopoDS_Shape& result = myMap(S);
  if (!result.IsNull())
  {
    theNewGeom = myHasNewGeom.Contains(S);
    return !S.IsSame(result);
  }
  Standard_Boolean rebuild = Standard_False, RevWires = Standard_False;
  TopAbs_Orientation ResOr = TopAbs_FORWARD;
  BRep_Builder B;
  Standard_Real tol;
  Standard_Boolean No3DCurve = Standard_False; // en fait, si on n`a pas de 
  //modif geometry 3d , it is necessary to test the existence of a curve 3d.

  // new geometry ?

  switch (ts) {
  case TopAbs_FACE:
    {
      rebuild = myNSInfo.IsBound(TopoDS::Face(S));
      if (rebuild) 
      {
        const NewSurfaceInfo& aNSinfo = myNSInfo(TopoDS::Face(S));
        RevWires = aNSinfo.myRevWires;
        B.MakeFace(TopoDS::Face(result),aNSinfo.mySurface,
          aNSinfo.myLoc.Predivided(S.Location()),aNSinfo.myToler);
        result.Location(S.Location());
        if (aNSinfo.myRevFace) 
          ResOr = TopAbs_REVERSED;
        // set specifics flags of a Face
        B.NaturalRestriction(TopoDS::Face(result), BRep_Tool::NaturalRestriction(TopoDS::Face(S)));
      }

      // update triangulation on the copied face
      Handle(Poly_Triangulation) aTriangulation;
      if (M->NewTriangulation(TopoDS::Face(S), aTriangulation))
      {
        if (rebuild) // the copied face already exists => update it
          B.UpdateFace(TopoDS::Face(result), aTriangulation);
        else
        { // create new face with bare triangulation
          B.MakeFace(TopoDS::Face(result), aTriangulation);
          result.Location(S.Location());
        }
        rebuild = Standard_True;
      }
    }
    break;

  case TopAbs_EDGE:
    {
      rebuild = myNCInfo.IsBound(TopoDS::Edge(S));
      if (rebuild) 
      {
        const NewCurveInfo& aNCinfo = myNCInfo(TopoDS::Edge(S));
        if (aNCinfo.myCurve.IsNull()) {
	  B.MakeEdge(TopoDS::Edge(result));
	  B.Degenerated(TopoDS::Edge(result),
			BRep_Tool::Degenerated(TopoDS::Edge(S)));
          B.UpdateEdge(TopoDS::Edge(result),aNCinfo.myToler);  //OCC217
	  No3DCurve = Standard_True;
	}
	else {
          B.MakeEdge(TopoDS::Edge(result),aNCinfo.myCurve,
            aNCinfo.myLoc.Predivided(S.Location()),aNCinfo.myToler);
	  No3DCurve = Standard_False;
	}
	result.Location(S.Location());
//	result.Orientation(S.Orientation());

	// set specifics flags of an Edge
	B.SameParameter(TopoDS::Edge(result),
			BRep_Tool::SameParameter(TopoDS::Edge(S)));
	B.SameRange(TopoDS::Edge(result),
		    BRep_Tool::SameRange(TopoDS::Edge(S)));
      }

      // update polygonal structure on the edge
      Handle(Poly_Polygon3D) aPolygon;
      if (M->NewPolygon(TopoDS::Edge(S), aPolygon))
      {
        if (rebuild) // the copied edge already exists => update it
          B.UpdateEdge(TopoDS::Edge(result), aPolygon, S.Location());
        else
        { // create new edge with bare polygon
          B.MakeEdge(TopoDS::Edge(result), aPolygon);
          result.Location(S.Location());
        }
        rebuild = Standard_True;
      }
    }
    break;
  default:
    ;
  }

  // rebuild sub-shapes and test new sub-shape ?

  Standard_Boolean newgeom = rebuild;
  theNewGeom = rebuild;

  TopoDS_Iterator it;

  {
    Standard_Integer aShapeCount = 0;
    {
      for (it.Initialize(S, Standard_False); it.More(); it.Next()) ++aShapeCount;
    }
    
    Message_ProgressSentry aPSentry(aProgress, "Converting SubShapes", 0, aShapeCount, 1);
    //
    for (it.Initialize(S, Standard_False); it.More() && aPSentry.More(); it.Next(), aPSentry.Next()) {
      // always call Rebuild
      Standard_Boolean isSubNewGeom = Standard_False;
      Standard_Boolean subrebuilt = Rebuild(it.Value(), M, isSubNewGeom, aProgress);
      rebuild =  subrebuilt || rebuild ;
      theNewGeom = theNewGeom || isSubNewGeom;
    }
    if (!aPSentry.More())
    {
      // The processing was broken
      return Standard_False;
    }
  }
  if (theNewGeom)
    myHasNewGeom.Add(S);

  // make an empty copy
  if (rebuild && !newgeom) {
    result = S.EmptyCopied();
    result.Orientation(TopAbs_FORWARD);
  }

  // copy the sub-elements 
  
  if (rebuild) {
    TopAbs_Orientation orient;
    for (it.Initialize(S,Standard_False); it.More(); it.Next()) {
      orient = it.Value().Orientation();
      if (RevWires || myMap(it.Value()).Orientation() == TopAbs_REVERSED) {
	orient = TopAbs::Reverse(orient);
      }
      B.Add(result,myMap(it.Value()).Oriented(orient));
    }


    if (ts == TopAbs_FACE) {
      // pcurves
      Handle(Geom2d_Curve) curve2d; //,curve2d1;
      TopoDS_Face face = TopoDS::Face(S);
      TopAbs_Orientation fcor = face.Orientation();
      if(fcor != TopAbs_REVERSED) fcor = TopAbs_FORWARD;

      TopExp_Explorer ex(face.Oriented(fcor),TopAbs_EDGE);
      for (;ex.More(); ex.Next()) 
      {
        const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());

#ifdef DEBUG_Modifier
        iE = MapE.Contains(edge) ? MapE.FindIndex(edge) : 0;
#endif
        if (theNewGeom && M->NewCurve2d
            (edge, face, TopoDS::Edge(myMap(ex.Current())), TopoDS::Face(result), curve2d, tol))
        {
          // rem dub 16/09/97 : Make constant topology or not make at all.
          // Do not make if CopySurface = 1
          // Atention, TRUE sewing edges (RealyClosed)  
          // stay even if  CopySurface is true.
    
          // check that edge contains two pcurves on this surface:
          // either it is true seam on the current face, or belongs to two faces
          // built on that same surface (see OCC21772)
          // Note: this check could be made separate method in BRepTools
          Standard_Boolean isClosed = Standard_False;
          if(BRep_Tool::IsClosed(edge,face))
          {
            isClosed = ( ! newgeom || BRepTools::IsReallyClosed(edge,face) );
            if ( ! isClosed )
            {
              TopLoc_Location aLoc;
              TopoDS_Shape resface = (myMap.IsBound(face) ? myMap(face) : face);
              if(resface.IsNull())
                resface = face;
              Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(resface), aLoc);
              // check other faces sharing the same surface
              TopExp_Explorer aExpF(myShape,TopAbs_FACE);
              for( ; aExpF.More() && !isClosed; aExpF.Next())
              {
                TopoDS_Face anOther = TopoDS::Face(aExpF.Current());
                if(anOther.IsSame(face))
                  continue;
                TopoDS_Shape resface2 = (myMap.IsBound(anOther) ? myMap(anOther) : anOther);
                if(resface2.IsNull())
                  resface2 = anOther;
                TopLoc_Location anOtherLoc;
                Handle(Geom_Surface) anOtherSurf = 
                  BRep_Tool::Surface(TopoDS::Face(resface2), anOtherLoc);
                if ( aSurf == anOtherSurf && aLoc.IsEqual (anOtherLoc) )
                {
                  TopExp_Explorer aExpE(anOther,TopAbs_EDGE);
                  for( ; aExpE.More() && !isClosed ; aExpE.Next())
                    isClosed = edge.IsSame(aExpE.Current());
                }
              }
            }
          }
          if (isClosed) 
          {
            TopoDS_Edge CurE = TopoDS::Edge(myMap(edge));
            TopoDS_Shape aLocalResult = result;
            aLocalResult.Orientation(TopAbs_FORWARD);
            TopoDS_Face CurF = TopoDS::Face(aLocalResult);
            Handle(Geom2d_Curve) curve2d1, currcurv;
            Standard_Real f,l;
            if ((!RevWires && fcor != edge.Orientation()) ||
              ( RevWires && fcor == edge.Orientation())) {
                CurE.Orientation(TopAbs_FORWARD);
                curve2d1 = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
                if (curve2d1.IsNull()) curve2d1 = new Geom2d_Line(gp::OX2d());
                B.UpdateEdge (CurE, curve2d1, curve2d, CurF, 0.);
            }
            else {
              CurE.Orientation(TopAbs_REVERSED);
              curve2d1 = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
              if (curve2d1.IsNull()) curve2d1 = new Geom2d_Line(gp::OX2d());
              B.UpdateEdge (CurE, curve2d, curve2d1, CurF, 0.);
            }
            currcurv = BRep_Tool::CurveOnSurface(CurE,CurF,f,l);
            B.Range(CurE,f,l);
          }
          else {
            B.UpdateEdge(TopoDS::Edge(myMap(ex.Current())),
              curve2d,
              TopoDS::Face(result), 0.);
          }

          TopLoc_Location theLoc;
          Standard_Real theF,theL;
          Handle(Geom_Curve) C3D = BRep_Tool::Curve(TopoDS::Edge(myMap(ex.Current())), theLoc, theF, theL);
          if (C3D.IsNull()) { // Update vertices
            Standard_Real param;
            TopExp_Explorer ex2(edge,TopAbs_VERTEX);
            while (ex2.More()) {
              const TopoDS_Vertex& vertex = TopoDS::Vertex(ex2.Current());
              if (!M->NewParameter(vertex, edge, param, tol)) {
                tol = BRep_Tool::Tolerance(vertex);
                param = BRep_Tool::Parameter(vertex,edge);
              }

              TopAbs_Orientation vtxrelat = vertex.Orientation();
              if (edge.Orientation() == TopAbs_REVERSED) {
                // Update considere l'edge FORWARD, et le vertex en relatif
                vtxrelat= TopAbs::Reverse(vtxrelat);
              }
              //if (myMap(edge).Orientation() == TopAbs_REVERSED) {
              //  vtxrelat= TopAbs::Reverse(vtxrelat);
              //}

              TopoDS_Vertex aLocalVertex = TopoDS::Vertex(myMap(vertex));
              aLocalVertex.Orientation(vtxrelat);
              //B.UpdateVertex(TopoDS::Vertex
              //(myMap(vertex).Oriented(vtxrelat)),
              B.UpdateVertex(aLocalVertex, param, TopoDS::Edge(myMap(edge)), tol);
              ex2.Next();
            }
          }
        }

        // Copy polygon on triangulation
        Handle(Poly_PolygonOnTriangulation) aPolyOnTria_1, aPolyOnTria_2;
        Standard_Boolean aNewPonT = M->NewPolygonOnTriangulation(edge, face, aPolyOnTria_1);
        if (BRepTools::IsReallyClosed(edge, face))
        {
          // Obtain triangulation on reversed edge
          TopoDS_Edge anEdgeRev = edge;
          anEdgeRev.Reverse();
          aNewPonT = M->NewPolygonOnTriangulation(anEdgeRev, face, aPolyOnTria_2) || aNewPonT;
          // It there is only one polygon on triangulation, store it to aPolyOnTria_1
          if (aPolyOnTria_1.IsNull() && !aPolyOnTria_2.IsNull())
          {
            aPolyOnTria_1 = aPolyOnTria_2;
            aPolyOnTria_2 = Handle(Poly_PolygonOnTriangulation)();
          }
        }
        if (aNewPonT)
        {
          TopLoc_Location aLocation;
          Handle(Poly_Triangulation) aNewFaceTria =
              BRep_Tool::Triangulation(TopoDS::Face(myMap(face)), aLocation);
          TopoDS_Edge aNewEdge = TopoDS::Edge(myMap(edge));
          if (aPolyOnTria_2.IsNull())
            B.UpdateEdge(aNewEdge, aPolyOnTria_1, aNewFaceTria, aLocation);
          else
          {
            if (edge.Orientation() == TopAbs_FORWARD)
              B.UpdateEdge(aNewEdge, aPolyOnTria_1, aPolyOnTria_2, aNewFaceTria, aLocation);
            else
              B.UpdateEdge(aNewEdge, aPolyOnTria_2, aPolyOnTria_1, aNewFaceTria, aLocation);
          }
        }
      }

    }

//    else if (ts == TopAbs_EDGE) {
    else if (ts == TopAbs_EDGE && !No3DCurve) {
      // Vertices
      Standard_Real param;
      const TopoDS_Edge& edge = TopoDS::Edge(S);
      TopAbs_Orientation edor = edge.Orientation();
      if(edor != TopAbs_REVERSED) edor = TopAbs_FORWARD;
      TopExp_Explorer ex(edge.Oriented(edor), TopAbs_VERTEX);
      while (ex.More()) {
        const TopoDS_Vertex& vertex = TopoDS::Vertex(ex.Current());

        if (!M->NewParameter(vertex, edge, param, tol)) {
          tol = BRep_Tool::Tolerance(vertex);
          param = BRep_Tool::Parameter(vertex,edge);
        }

        TopAbs_Orientation vtxrelat = vertex.Orientation();
        if (edor == TopAbs_REVERSED) {
          // Update considere l'edge FORWARD, et le vertex en relatif
          vtxrelat= TopAbs::Reverse(vtxrelat);
        }

        //if (result.Orientation() == TopAbs_REVERSED) {
        //  vtxrelat= TopAbs::Reverse(vtxrelat);
        //}
        TopoDS_Vertex aLocalVertex = TopoDS::Vertex(myMap(vertex));
        aLocalVertex.Orientation(vtxrelat);
        //B.UpdateVertex(TopoDS::Vertex(myMap(vertex).Oriented(vtxrelat)),
        if (myMutableInput || !aLocalVertex.IsSame(vertex))
          B.UpdateVertex(aLocalVertex, param, TopoDS::Edge(result), tol);
        ex.Next();
      }

    }

    // update flags

    result.Orientable(S.Orientable());
    result.Closed(S.Closed());
    result.Infinite(S.Infinite());
  }
  else
    result = S;

  // Set flag of the shape.
  result.Orientation(ResOr);

  SetShapeFlags(S, result);

  return rebuild;
}

void BRepTools_Modifier::CreateNewVertices( const TopTools_IndexedDataMapOfShapeListOfShape& theMVE, const Handle(BRepTools_Modification)& M)
{
  double aToler;
  BRep_Builder aBB;
  gp_Pnt aPnt;  
  for (int i = 1; i <= theMVE.Extent(); i++ )
  {
    //fill MyMap only with vertices with NewPoint == true
    const TopoDS_Vertex& aV = TopoDS::Vertex(theMVE.FindKey(i));
    Standard_Boolean IsNewP = M->NewPoint(aV, aPnt, aToler);
    if (IsNewP)
    {
      TopoDS_Vertex aNewV;
      aBB.MakeVertex(aNewV, aPnt, aToler);
      SetShapeFlags(aV, aNewV);
      myMap(aV) = aNewV;
      myHasNewGeom.Add(aV);
    }
    else if (myMutableInput)
      myMap(aV) = aV.Oriented(TopAbs_FORWARD);
  }
}

void BRepTools_Modifier::FillNewCurveInfo(const TopTools_IndexedDataMapOfShapeListOfShape& theMEF, const Handle(BRepTools_Modification)& M)
{
  Handle(Geom_Curve) aCurve;
  TopLoc_Location aLocation;
  BRepTools_Modifier::NewCurveInfo aNCinfo;
  double aToler;
  for (int i = 1; i <= theMEF.Extent(); i++ )
  {
    const TopoDS_Edge& anE = TopoDS::Edge(theMEF.FindKey(i));
    Standard_Boolean IsNewCur = M->NewCurve(anE, aCurve, aLocation, aToler);
    if (IsNewCur)
    {
      aNCinfo.myCurve = aCurve;
      aNCinfo.myLoc = aLocation;
      aNCinfo.myToler = aToler;
      myNCInfo.Bind(anE, aNCinfo);
      myHasNewGeom.Add(anE);
    }
  }
}

void BRepTools_Modifier::FillNewSurfaceInfo(const Handle(BRepTools_Modification)& M)
{
  TopTools_IndexedMapOfShape aMF;  
  TopExp::MapShapes(myShape, TopAbs_FACE, aMF);
  BRepTools_Modifier::NewSurfaceInfo aNSinfo;
  for (int i = 1; i <= aMF.Extent(); i++ )
  {
    const TopoDS_Face& aF = TopoDS::Face(aMF(i));
    Standard_Boolean RevFace;
    Standard_Boolean RevWires;
    Handle(Geom_Surface) aSurface;
    TopLoc_Location aLocation;
    double aToler1;
    Standard_Boolean IsNewSur = M->NewSurface(aF, aSurface, aLocation, aToler1, RevWires,RevFace);
    if (IsNewSur)
    {
      aNSinfo.mySurface = aSurface;
      aNSinfo.myLoc = aLocation;
      aNSinfo.myToler = aToler1;
      aNSinfo.myRevWires = RevWires;
      aNSinfo.myRevFace = RevFace;
      myNSInfo.Bind(aF, aNSinfo);
      myHasNewGeom.Add(aF);
    }
    else
    {
      //check if subshapes will be modified 
      Standard_Boolean notRebuilded = Standard_True;
      TopExp_Explorer exE(aF, TopAbs_EDGE);
      while (exE.More() && notRebuilded) 
      {
        const TopoDS_Edge& anEE = TopoDS::Edge(exE.Current());
        if (myNCInfo.IsBound(anEE))
        {
          notRebuilded = Standard_False;
          break;
        }
        TopExp_Explorer exV(anEE, TopAbs_VERTEX);
        while (exV.More() && notRebuilded) 
        {
          const TopoDS_Vertex& aVV = TopoDS::Vertex(exV.Current());
          if (!myMap(aVV).IsNull())
          {
            notRebuilded = Standard_False;
            break;
          }
          exV.Next();
        }
        exE.Next();
      }
      if (notRebuilded)
      {
        //subshapes is not going to be modified
        myNonUpdFace.Add(aF); 
      }
    }
  }

}

void BRepTools_Modifier::CreateOtherVertices(const TopTools_IndexedDataMapOfShapeListOfShape& theMVE, 
                                             const TopTools_IndexedDataMapOfShapeListOfShape& theMEF, 
                                             const Handle(BRepTools_Modification)& M)
{
  double aToler;
  //The following logic in some ways repeats the logic from the Rebuild() method.
  //If the face with its subshapes is not going to be modified 
  //(i.e. NewSurface() for this face and NewCurve(), NewPoint() for its edges/vertices returns false)
  //then the calling of NewCurve2d() for this face with its edges is not performed. 
  //Therefore, the updating of vertices will not present in such cases and 
  //the EmptyCopied() operation for vertices from this face is not needed. 

  for (int i = 1; i <= theMVE.Extent(); i++ )
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(theMVE.FindKey(i));
    TopoDS_Vertex aNewV = TopoDS::Vertex(myMap(aV));
    if ( aNewV.IsNull()) 
    {
       const TopTools_ListOfShape& aLEdges = theMVE(i);
       Standard_Boolean toReplace = Standard_False;
       TopTools_ListIteratorOfListOfShape it(aLEdges);
       for (; it.More() && !toReplace; it.Next()) 
       {
         const TopoDS_Edge& anE = TopoDS::Edge(it.Value());
         if (myNCInfo.IsBound(anE) && !myNCInfo(anE).myCurve.IsNull())
            toReplace = Standard_True;

         if (!toReplace)
         {
           const TopTools_ListOfShape& aLFaces = theMEF.FindFromKey(anE);
           TopTools_ListIteratorOfListOfShape it2(aLFaces);
           for (; it2.More(); it2.Next()) 
           {
             const TopoDS_Face& aF = TopoDS::Face(it2.Value());
             if (!myNonUpdFace.Contains(aF))
             {
               Handle(Geom2d_Curve) aCurve2d;
               //some NewCurve2d()s may use NewE arg internally, so the 
               //null TShape as an arg may lead to the exceptions 
               TopoDS_Edge aDummyE = TopoDS::Edge(anE.EmptyCopied());
               if (M->NewCurve2d(anE, aF, aDummyE, TopoDS_Face(), aCurve2d, aToler))
               {
                 toReplace = true;
                 break;
               }
             }
           }
         }
       }
       if (toReplace)
         aNewV = TopoDS::Vertex(aV.EmptyCopied());
       else
         aNewV = aV;
       aNewV.Orientation(TopAbs_FORWARD);
       myMap(aV) = aNewV;
    }
  }
}


static void SetShapeFlags(const TopoDS_Shape& theInSh, TopoDS_Shape& theOutSh)
{
  theOutSh.Modified  (theInSh.Modified());
  theOutSh.Checked   (theInSh.Checked());
  theOutSh.Orientable(theInSh.Orientable());
  theOutSh.Closed    (theInSh.Closed());
  theOutSh.Infinite  (theInSh.Infinite());
  theOutSh.Convex    (theInSh.Convex());
}


Standard_Boolean BRepTools_Modifier::IsMutableInput() const
{
  return myMutableInput;
}

void BRepTools_Modifier::SetMutableInput(Standard_Boolean theMutableInput)
{
  myMutableInput = theMutableInput;
}

