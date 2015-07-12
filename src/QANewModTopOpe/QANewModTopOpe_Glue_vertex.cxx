// Created on: 2001-01-16
// Created by: Michael SAZONOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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
#include <BRepExtrema_DistShapeShape.hxx>
#include <gp_Pnt.hxx>
#include <QANewModTopOpe_Glue.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : PerformVertex
//purpose  : 
//=======================================================================
void
QANewModTopOpe_Glue::PerformVertex() 
{
  TopoDS_Shape& myS1=myArguments.First();
  TopoDS_Shape& myS2=myTools.First();

  BRepExtrema_DistShapeShape aExtrema (myS1, myS2);
  if (!aExtrema.IsDone()) return;
  if (aExtrema.InnerSolution()) {
    if (myS2.ShapeType() != TopAbs_VERTEX) return;
    TopoDS_Vertex aV = TopoDS::Vertex(myS2);
    BRep_Builder aBld;
    aBld.Add(myS1,aV.Oriented(TopAbs_INTERNAL));

    TopTools_ListOfShape aList;
    aList.Append (aV);
    myMapGener.Bind(myS1, aList);

   
    
    myShape = myS1;
    myShape.Orientation(myS1.Orientation());
    Done();
 
  }
  else {
    Standard_Integer nbSol = aExtrema.NbSolution(), i;
    Standard_Real aDist = aExtrema.Value();
    TopoDS_Shape aDummy = myS2.Oriented(TopAbs_FORWARD);
    TopoDS_Vertex aVer2 = TopoDS::Vertex (aDummy);
    Standard_Real aTol2 = BRep_Tool::Tolerance(aVer2);
    
    TopTools_MapOfShape aMapPassed;
    aMapPassed.Add (myS2);
    TopTools_IndexedDataMapOfShapeListOfShape aMapAnc;
    TopExp::MapShapesAndAncestors (myS1, TopAbs_VERTEX, TopAbs_EDGE, aMapAnc);
    TopExp::MapShapesAndAncestors (myS1, TopAbs_VERTEX, TopAbs_FACE, aMapAnc);
    TopExp::MapShapesAndAncestors (myS1, TopAbs_EDGE, TopAbs_FACE, aMapAnc);
    BRep_Builder aBld;
    
    // pass 1: process contacted vertices
    for (i=1; i <= nbSol; i++) {
      TopoDS_Shape aShape = aExtrema.SupportOnShape1(i);
      if (aShape.ShapeType() != TopAbs_VERTEX ||
	  aMapPassed.Contains(aShape)) continue;
      
      const TopoDS_Vertex& aVer = TopoDS::Vertex (aShape);
      
      if(aVer.IsSame(aVer2)) continue;
      
      Standard_Real aTol1 = BRep_Tool::Tolerance(aVer);
      if (aDist > aTol1 && aDist > aTol2) continue;
      
      aTol2 = Max (aTol2, aTol1 + aDist);
      aBld.UpdateVertex (aVer2, aTol2);
      
      // substitute aVer2 instead of aVer
      TopTools_ListOfShape aList;
      aList.Append (aVer2);
      mySubst.Substitute (aVer, aList);
      aMapPassed.Add(aVer);
      TopTools_ListIteratorOfListOfShape aIter(aMapAnc.FindFromKey(aVer));
      for (; aIter.More(); aIter.Next()) {
	aMapPassed.Add(aIter.Value());
      }
      myMapModif.Bind(aVer, aList);
    }
    
    // pass 2: process contacted edges
    for (i=1; i <= nbSol; i++) {
      TopoDS_Shape aShape = aExtrema.SupportOnShape1(i);
      if (aShape.ShapeType() != TopAbs_EDGE ||
	  aMapPassed.Contains(aShape)) continue;
      
      const TopoDS_Edge& aEdge = TopoDS::Edge (aShape);
      Standard_Real aTol1 = BRep_Tool::Tolerance(aEdge);
      if (aDist > aTol1 && aDist > aTol2) continue;
      Standard_Real aPar;
      aExtrema.ParOnEdgeS1(i, aPar);
      
      // construct new edge
      TopoDS_Edge aNewEdge;
      InsertVertexInEdge (aEdge, aVer2, aPar, aNewEdge);
      
      // substitute edge
      TopTools_ListOfShape aList;
      aList.Append (aNewEdge.Oriented(TopAbs_FORWARD));
      mySubst.Substitute (aEdge, aList);
      aMapPassed.Add(aEdge);
      TopTools_ListIteratorOfListOfShape aIter(aMapAnc.FindFromKey(aEdge));
      for (; aIter.More(); aIter.Next()) {
	aMapPassed.Add(aIter.Value());
      }
      // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aEdge, TopTools_ListOfShape());
      TopTools_ListOfShape aList1;
      myMapModif.Bind(aEdge, aList1);
      myMapModif(aEdge).Append (aNewEdge);
      // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aEdge, TopTools_ListOfShape());
      TopTools_ListOfShape aList2;
      myMapGener.Bind(aEdge, aList2);
      myMapGener(aEdge).Append (aVer2);
    }

    // pass 3: process contacted faces
    for (i=1; i <= nbSol; i++) {
      TopoDS_Shape aShape = aExtrema.SupportOnShape1(i);
      if (aShape.ShapeType() != TopAbs_FACE ||
	  aMapPassed.Contains(aShape)) continue;
      
      const TopoDS_Face& aFace = TopoDS::Face (aShape);
      Standard_Real aTol1 = BRep_Tool::Tolerance(aFace);
      if (aDist > aTol1 && aDist > aTol2) continue;
      Standard_Real aParU, aParV;
      aExtrema.ParOnFaceS1(i, aParU, aParV);
      
      // construct new face
      TopoDS_Face aNewFace = aFace;
      aNewFace.EmptyCopy();
      aNewFace.Orientation (TopAbs_FORWARD);
      aBld.NaturalRestriction (aNewFace, BRep_Tool::NaturalRestriction(aFace));
      // add old subshapes
      TopoDS_Iterator aIterF (aFace, Standard_False);
      for (; aIterF.More(); aIterF.Next()) {
	aBld.Add (aNewFace, aIterF.Value());
      }
      // add new internal vertex
      aTol2 = Max (aTol2, aTol1);
      aBld.Add (aNewFace, aVer2.Oriented(TopAbs_INTERNAL));
      aBld.UpdateVertex (aVer2, aParU, aParV, aNewFace, aTol2);
      
      // substitute face
      TopTools_ListOfShape aList;
      aList.Append (aNewFace);
      mySubst.Substitute (aFace, aList);
      aMapPassed.Add(aFace);
      // for Mandrake-10 - mkv,02.06.06 - myMapModif.Bind(aFace, TopTools_ListOfShape());
      TopTools_ListOfShape aList3;
      myMapModif.Bind(aFace, aList3);
      myMapModif(aFace).Append (aNewFace);
      // for Mandrake-10 - mkv,02.06.06 - myMapGener.Bind(aFace, TopTools_ListOfShape());
      TopTools_ListOfShape aList4;
      myMapGener.Bind(aFace, aList4);
      myMapGener(aFace).Append (aVer2);
    }
    
    mySubst.Build(myS1);
    
    TopExp_Explorer aExp (myS1, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFace = aExp.Current();
      if(myMapModif.IsBound(aFace)) continue;
      if (mySubst.IsCopied(aFace)) {
	if (!mySubst.Copy(aFace).IsEmpty()) {	
	  myMapModif.Bind(aFace,mySubst.Copy(aFace));
	}
      }
    }
    
    aExp.Init(myS1, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& anEdge = aExp.Current();
      if(myMapModif.IsBound(anEdge)) continue;
      if (mySubst.IsCopied(anEdge)) {
	if (!mySubst.Copy(anEdge).IsEmpty()) {	
	  myMapModif.Bind(anEdge,mySubst.Copy(anEdge));
	}
      }
    }

    if (mySubst.IsCopied(myS1)) {
      myShape = mySubst.Copy(myS1).First();
      myShape.Orientation(myS1.Orientation());
      Done();
    }
  }
}
