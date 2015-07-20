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


//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier ():myDone(Standard_False)
{}

//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier (const TopoDS_Shape& S) :
  myShape(S),myDone(Standard_False)
{
  myMap.Clear();
  Put(S);
}

//=======================================================================
//function : BRepTools_Modifier
//purpose  : 
//=======================================================================

BRepTools_Modifier::BRepTools_Modifier
  (const TopoDS_Shape& S,
   const Handle(BRepTools_Modification)& M) : myShape(S),myDone(Standard_False)
{
  myMap.Clear();
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
  myMap.Clear();
  Put(S);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepTools_Modifier::Perform(const Handle(BRepTools_Modification)& M, const Handle(Message_ProgressIndicator) & aProgress)
{
  if (myShape.IsNull()) {
    Standard_NullObject::Raise();
  }
  TopTools_DataMapIteratorOfDataMapOfShapeShape theIter(myMap);

  // Set to Null the value of shapes, in case when another modification is applied to the start shape.

  if (!theIter.Value().IsNull()) {
    while (theIter.More()) {
      myMap(theIter.Value()).Nullify();
      theIter.Next();
    }
    theIter.Reset();
  }

  /*
  while (theIter.More()) {
    Rebuild(theIter.Key(),M);
    theIter.Next();
  }
  */

  Message_ProgressSentry aPSentry(aProgress, "Converting Shape", 0, 2, 1);

  Rebuild(myShape, M, aProgress);

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

  TopTools_IndexedDataMapOfShapeListOfShape theEFMap;
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,theEFMap);
  BRep_Builder B;

/*
  Standard_Boolean RecomputeTriangles = Standard_False;
  Standard_Real MaxDeflection = RealFirst();
  Handle(Poly_Triangulation) Tr;
  Handle(Poly_Polygon3D) Po;
  TopLoc_Location Loc;
*/

  while (theIter.More()) {
    const TopoDS_Shape& S = theIter.Key();
/*
    if (S.ShapeType() == TopAbs_FACE && !S.IsSame(theIter.Value())) {
      Tr = BRep_Tool::Triangulation(TopoDS::Face(S),Loc);
      if (!Tr.IsNull()) {
        RecomputeTriangles = Standard_True;
        MaxDeflection = Max(MaxDeflection,Tr->Deflection());
      }
    }
    else */ if (S.ShapeType() == TopAbs_EDGE && !S.IsSame(theIter.Value())) {
      const TopoDS_Edge& edg = TopoDS::Edge(S);
/*
      Po = BRep_Tool::Polygon3D(edg,Loc);
      if (!Po.IsNull()) {
	RecomputeTriangles = Standard_True;
	MaxDeflection = Max(MaxDeflection,Po->Deflection());
      }
*/
      TopTools_ListIteratorOfListOfShape it;
      it.Initialize(theEFMap.FindFromKey(edg));
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
        const TopoDS_Edge& newedg = TopoDS::Edge(myMap(edg));
        const TopoDS_Face& newf1  = TopoDS::Face(myMap(F1));
        const TopoDS_Face& newf2  = TopoDS::Face(myMap(F2));
        GeomAbs_Shape Newcont = M->Continuity(edg,F1,F2,newedg,newf1,newf2);
        if (Newcont > GeomAbs_C0) {
          B.Continuity(newedg,newf1,newf2,Newcont);
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
   const Handle(Message_ProgressIndicator)& aProgress)
{
  TopoDS_Shape& result = myMap(S);
//  if (!result.IsNull()) return ! S.IsEqual(result);
  if (!result.IsNull()) return ! S.IsSame(result);
  Standard_Boolean rebuild = Standard_False, RevWires = Standard_False;
  TopAbs_Orientation ResOr = TopAbs_FORWARD;
  BRep_Builder B;
  Standard_Real tol;
  Standard_Boolean No3DCurve = Standard_False; // en fait, si on n`a pas de 
  //modif geometry 3d , it is necessary to test the existence of a curve 3d.

  // new geometry ?

  TopAbs_ShapeEnum ts = S.ShapeType();
  switch (ts) {
  case TopAbs_FACE:
    {
      Standard_Boolean RevFace;
      Handle(Geom_Surface) surface;
      TopLoc_Location location;
      rebuild = M->NewSurface(TopoDS::Face(S),surface,location,tol,
			      RevWires,RevFace);
      if (rebuild) {
	B.MakeFace(TopoDS::Face(result),surface,
		   location.Predivided(S.Location()),tol);
	result.Location(S.Location());
//	result.Orientation(S.Orientation());
	if (RevFace) {
	  ResOr = TopAbs_REVERSED;
	}
	// set specifics flags of a Face
	B.NaturalRestriction(TopoDS::Face(result),
			     BRep_Tool::NaturalRestriction(TopoDS::Face(S)));
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
      Handle(Geom_Curve) curve;
      TopLoc_Location location;
      rebuild = M->NewCurve(TopoDS::Edge(S),curve,location,tol);
      if (rebuild) {
	if (curve.IsNull()) {
	  B.MakeEdge(TopoDS::Edge(result));
	  B.Degenerated(TopoDS::Edge(result),
			BRep_Tool::Degenerated(TopoDS::Edge(S)));
	  B.UpdateEdge(TopoDS::Edge(result),tol);  //OCC217
	  No3DCurve = Standard_True;
	}
	else {
	  B.MakeEdge(TopoDS::Edge(result),curve,
		     location.Predivided(S.Location()),tol);
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
    }
    break;

  case TopAbs_VERTEX:
    {
      gp_Pnt vtx;
      rebuild = M->NewPoint(TopoDS::Vertex(S),vtx,tol);
      if (rebuild) {
	B.MakeVertex(TopoDS::Vertex(result),vtx,tol);
      }
    }
    break;

  default:
    {
    }
  }

  // rebuild sub-shapes and test new sub-shape ?

  Standard_Boolean newgeom = rebuild;

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
      Standard_Boolean subrebuilt = Rebuild(it.Value(), M, aProgress);
      rebuild =  subrebuilt || rebuild ;
    }
    if (!aPSentry.More())
    {
      // The processing was broken
      return Standard_False;
    }
  }

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

        if (M->NewCurve2d(edge, face, TopoDS::Edge(myMap(ex.Current())), TopoDS::Face(result), curve2d, tol)) 
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
            currcurv = BRep_Tool::CurveOnSurface(edge,face,f,l);
            B.Range(edge,f,l);
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

  result.Modified  (S.Modified());
  result.Checked   (S.Checked());
  result.Orientable(S.Orientable());
  result.Closed    (S.Closed());
  result.Infinite  (S.Infinite());
  result.Convex    (S.Convex());

  return rebuild;
}


