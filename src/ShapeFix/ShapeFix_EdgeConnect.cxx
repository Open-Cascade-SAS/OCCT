// Created on: 1999-05-11
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
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeFix_EdgeConnect.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//#define POSITION_USES_MEAN_POINT
//=======================================================================
//function : ShapeFix_EdgeConnect
//=======================================================================
ShapeFix_EdgeConnect::ShapeFix_EdgeConnect () {}

//=======================================================================
//function : Add
//purpose  : Adds connectivity information for two edges
//=======================================================================

void ShapeFix_EdgeConnect::Add (const TopoDS_Edge& aFirst, const TopoDS_Edge& aSecond)
{
  // Select vertices to connect
  TopoDS_Vertex theFirstVertex = TopExp::LastVertex( aFirst, Standard_True );
  TopoDS_Vertex theSecondVertex = TopExp::FirstVertex( aSecond, Standard_True );

  // Make necessary bindings
  if ( myVertices.IsBound( theFirstVertex ) ) {
    // First vertex is bound - find shared vertex
    TopoDS_Vertex theFirstShared = TopoDS::Vertex( myVertices( theFirstVertex ) );
    if ( myVertices.IsBound( theSecondVertex ) ) {
      // Second vertex is bound - find shared vertex
      TopoDS_Vertex theSecondShared = TopoDS::Vertex( myVertices( theSecondVertex ) );
      if ( !theFirstShared.IsSame(theSecondShared) ) {
	// Concatenate lists
	TopTools_ListOfShape& theFirstList = myLists( theFirstShared );
	TopTools_ListOfShape& theSecondList = myLists( theSecondShared );
	for ( TopTools_ListIteratorOfListOfShape theIterator( theSecondList );
	      theIterator.More();
	      theIterator.Next() ) {
	  // Rebind shared vertex for current one
	  myVertices( theIterator.Value() ) = theFirstShared;
	  // Skip the following edge
	  theIterator.Next();
	}
	// Append second list to the first one
	theFirstList.Append( theSecondList );
	// Unbind the second shared vertex
	myLists.UnBind( theSecondShared );
      }
    }
    else {
      // Bind second vertex with shared vertex of the first one
      myVertices.Bind( theSecondVertex, theFirstShared );
      // Add second vertex and second edge to the list
      TopTools_ListOfShape& theFirstList = myLists( theFirstShared );
      theFirstList.Append( theSecondVertex );
      theFirstList.Append( aSecond );
    }
  }
  else {
    if ( myVertices.IsBound( theSecondVertex ) ) {
      // Second vertex is bound - find shared vertex
      TopoDS_Vertex& theSecondShared = TopoDS::Vertex( myVertices( theSecondVertex ) );
      // Bind first vertex with shared vertex of the second one
      myVertices.Bind( theFirstVertex, theSecondShared );
      // Add first vertex and first edge to the list
      TopTools_ListOfShape& theSecondList = myLists( theSecondShared );
      theSecondList.Append( theFirstVertex );
      theSecondList.Append( aFirst );
    }
    else {
      // None is bound - create new bindings
      myVertices.Bind( theFirstVertex, theFirstVertex );
      myVertices.Bind( theSecondVertex, theFirstVertex );
      TopTools_ListOfShape theNewList;
      theNewList.Append( theFirstVertex );
      theNewList.Append( aFirst );
      theNewList.Append( theSecondVertex );
      theNewList.Append( aSecond );
      myLists.Bind( theFirstVertex, theNewList );
    }
  }
}

//=======================================================================
//function : Add
//purpose  : Adds connectivity information for the whole shape
//=======================================================================

void ShapeFix_EdgeConnect::Add (const TopoDS_Shape& aShape)
{
  for ( TopExp_Explorer expw( aShape, TopAbs_WIRE ); expw.More(); expw.Next() ) {
    TopoDS_Wire theWire = TopoDS::Wire(expw.Current());
    TopExp_Explorer expe( theWire, TopAbs_EDGE );
    if (expe.More()) {
      // Obtain the first edge and remember it
      TopoDS_Edge theEdge = TopoDS::Edge(expe.Current());
      TopoDS_Edge theFirst = theEdge;
      expe.Next();
      for (; expe.More(); expe.Next()) {
	// Obtain second edge and connect it
	TopoDS_Edge theNext = TopoDS::Edge(expe.Current());
	Add( theEdge, theNext );
	theEdge = theNext;
      }
      // Connect first and last edges if wire is closed
      if (theWire.Closed()) Add( theEdge, theFirst );
    }
  }
}

//=======================================================================
//function : Build
//purpose  : Builds shared vertices
//=======================================================================

void ShapeFix_EdgeConnect::Build ()
{
  TopTools_ListIteratorOfListOfShape theLIterator;
  BRep_ListIteratorOfListOfCurveRepresentation theCIterator;

  TColgp_SequenceOfXYZ thePositions;
  gp_XYZ thePosition;
  Standard_Real theMaxDev;
  BRep_Builder theBuilder;

  // Iterate on shared vertices
  for ( TopTools_DataMapIteratorOfDataMapOfShapeListOfShape theSIterator( myLists );
        theSIterator.More();
        theSIterator.Next() ) {
    TopoDS_Vertex theSharedVertex = TopoDS::Vertex( theSIterator.Key() );
    const TopTools_ListOfShape& theList = theSIterator.Value();

    thePositions.Clear();

    // Iterate on edges, accumulating positions
    for ( theLIterator.Initialize( theList );
	  theLIterator.More();
	  theLIterator.Next() ) {
      TopoDS_Vertex& theVertex = TopoDS::Vertex( theLIterator.Value() );
      theLIterator.Next();
      TopoDS_Edge& theEdge = TopoDS::Edge( theLIterator.Value() );

      // Determine usage of curve bound points
      TopoDS_Vertex theStart, theEnd;
      theEdge.Orientation(TopAbs_FORWARD);
      TopExp::Vertices( theEdge, theStart, theEnd );
      Standard_Boolean use_start = ( theVertex.IsSame( theStart ) );
      Standard_Boolean use_end   = ( theVertex.IsSame( theEnd ) );
      
      // Iterate on edge curves, accumulating positions
      for (theCIterator.Initialize((*((Handle(BRep_TEdge)*)&theEdge.TShape()))->ChangeCurves());
	   theCIterator.More(); theCIterator.Next()) {
	Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(theCIterator.Value());
	if ( GC.IsNull() ) continue;
	// Calculate vertex position for this curve
	Standard_Real theFParam, theLParam;
	GC->Range( theFParam, theLParam );
	gp_Pnt thePoint;
	if (use_start) {
	  GC->D0( theFParam, thePoint );
	  thePositions.Append( thePoint.XYZ() );
	}
	if (use_end) {
	  GC->D0( theLParam, thePoint );
	  thePositions.Append( thePoint.XYZ() );
	}
      }
    }
      
    Standard_Integer i, theNbPos = thePositions.Length();

    // Calculate vertex position
    thePosition = gp_XYZ(0.,0.,0.);

#ifdef POSITION_USES_MEAN_POINT
#undef POSITION_USES_MEAN_POINT
    for ( i = 1; i <= theNbPos; i++ ) thePosition += thePositions.Value(i);
    if ( theNbPos > 1 ) thePosition /= theNbPos;
#else
    gp_XYZ theLBound(0.,0.,0.), theRBound(0.,0.,0.);
    for ( i = 1; i <= theNbPos; i++ ) {
      thePosition = thePositions.Value(i);
      if ( i == 1 ) theLBound = theRBound = thePosition;
      Standard_Real val = thePosition.X();
      if ( val < theLBound.X() ) theLBound.SetX( val );
      else if ( val > theRBound.X() ) theRBound.SetX( val );
      val = thePosition.Y();
      if ( val < theLBound.Y() ) theLBound.SetY( val );
      else if ( val > theRBound.Y() ) theRBound.SetY( val );
      val = thePosition.Z();
      if ( val < theLBound.Z() ) theLBound.SetZ( val );
      else if ( val > theRBound.Z() ) theRBound.SetZ( val );
    }
    if ( theNbPos > 1 ) thePosition = (theLBound + theRBound)/2.;
#endif    

    // Calculate maximal deviation
    theMaxDev = 0.;

    for ( i = 1; i <= theNbPos; i++ ) {
      Standard_Real theDeviation = (thePosition-thePositions.Value(i)).Modulus();
      if ( theDeviation > theMaxDev ) theMaxDev = theDeviation;
    }
    theMaxDev *= 1.0001; // To avoid numerical roundings
    if ( theMaxDev < Precision::Confusion() ) theMaxDev = Precision::Confusion();
      
    // Update shared vertex
    theBuilder.UpdateVertex( theSharedVertex, gp_Pnt(thePosition), theMaxDev );

    // Iterate on edges, adding shared vertex
    for ( theLIterator.Initialize( theList );
	  theLIterator.More();
	  theLIterator.Next() ) {
      TopoDS_Vertex& theVertex = TopoDS::Vertex( theLIterator.Value() );
      theLIterator.Next();
      TopoDS_Edge& theEdge = TopoDS::Edge( theLIterator.Value() );

      // Determine usage of old vertices
      TopoDS_Vertex theStart, theEnd;
      theEdge.Orientation(TopAbs_FORWARD);
      TopExp::Vertices( theEdge, theStart, theEnd );
      Standard_Boolean use_start = ( theVertex.IsSame( theStart ) );
      Standard_Boolean use_end   = ( theVertex.IsSame( theEnd ) );

      // Prepare vertex to remove
      TopoDS_Vertex theOldVertex;
      if (use_start) theOldVertex = theStart; // start is preferred for closed edges
      else theOldVertex = theEnd;

      // Prepare vertex to add
      TopoDS_Vertex theNewVertex;
      //smh#8 Porting AIX
      if (use_start) {
	TopoDS_Shape tmpshapeFwd = theSharedVertex.Oriented(TopAbs_FORWARD);
	theNewVertex = TopoDS::Vertex(tmpshapeFwd);
      }
      else {
	TopoDS_Shape tmpshapeRev = theSharedVertex.Oriented(TopAbs_REVERSED);
	theNewVertex = TopoDS::Vertex(tmpshapeRev);
      }
      if ( !theOldVertex.IsSame(theNewVertex) ) {
	// Replace vertices
	Standard_Boolean freeflag = theEdge.Free();
	theEdge.Free(Standard_True); //smh
	theBuilder.Remove( theEdge, theOldVertex );
	theBuilder.Add( theEdge, theNewVertex );
	if (use_start && use_end) {
	  // process special case for closed edge
	  theBuilder.Remove( theEdge, theOldVertex.Oriented(TopAbs_REVERSED) ); // remove reversed from closed edge
	  theBuilder.Add( theEdge, theNewVertex.Oriented(TopAbs_REVERSED) ); // add reversed to closed edge
	}
	theEdge.Free(freeflag);
      }
    }
  }

  // Clear maps after build
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void ShapeFix_EdgeConnect::Clear ()
{
  myVertices.Clear();
  myLists.Clear();
}
