// Created on: 2004-05-10
// Created by: Michael SAZONOV
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

#include <MeshTest_CheckTopology.hxx>
#include <BRep_Tool.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Connect.hxx>

//=======================================================================
//function : Perform
//purpose  : Performs checking
//=======================================================================

void MeshTest_CheckTopology::Perform (Draw_Interpretor& di)
{
  TopTools_IndexedMapOfShape aMapF;
  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
  TopExp::MapShapes (myShape, TopAbs_FACE, aMapF);
  TopExp::MapShapesAndAncestors (myShape, TopAbs_EDGE, TopAbs_FACE, aMapEF);

  // check polygons
  Standard_Integer ie;
  for (ie=1; ie <= aMapEF.Extent(); ie++) {
    const TopoDS_Edge& aEdge = TopoDS::Edge(aMapEF.FindKey(ie));
    const TopTools_ListOfShape& aFaces = aMapEF(ie);
    if (aFaces.Extent() < 2) continue;

    // get polygon on first face
    const TopoDS_Face& aFace1 = TopoDS::Face(aFaces.First());
    TopLoc_Location aLoc1;
    Handle(Poly_Triangulation) aT1 = BRep_Tool::Triangulation(aFace1, aLoc1);
    Handle(Poly_PolygonOnTriangulation) aPoly1 =
      BRep_Tool::PolygonOnTriangulation(aEdge, aT1, aLoc1);
    if (aPoly1.IsNull() || aT1.IsNull()) {
#ifdef OCCT_DEBUG
      cout<<"problem getting PolygonOnTriangulation of edge "<<ie<<endl;
#endif
      continue;
    }
    const TColStd_Array1OfInteger& aNodes1 = aPoly1->Nodes();

    // cycle on other polygons
    TopTools_ListIteratorOfListOfShape it(aFaces);
    it.Next();
    for (; it.More(); it.Next()) {
      const TopoDS_Face& aFace2 = TopoDS::Face(it.Value());
      TopLoc_Location aLoc2;
      Handle(Poly_Triangulation) aT2 = BRep_Tool::Triangulation(aFace2, aLoc2);
      Handle(Poly_PolygonOnTriangulation) aPoly2 =
	BRep_Tool::PolygonOnTriangulation(aEdge, aT2, aLoc2);
      if (aPoly2.IsNull() || aT2.IsNull()) {
#ifdef OCCT_DEBUG
	cout<<"problem getting PolygonOnTriangulation of edge "<<ie<<endl;
#endif
	continue;
      }
      const TColStd_Array1OfInteger& aNodes2 = aPoly2->Nodes();

      // check equality of polygons lengths
      if (aNodes2.Length() != aNodes1.Length()) {
	myAsyncEdges.Append(ie);
	break;
      }

      // check distances between corresponding points
      Standard_Real aDefle = Max(aT1->Deflection(), aT2->Deflection());
      const TColgp_Array1OfPnt& aPoints1 = aT1->Nodes();
      const TColgp_Array1OfPnt& aPoints2 = aT2->Nodes();
      Standard_Integer iF1 = aMapF.FindIndex(aFace1);
      Standard_Integer iF2 = aMapF.FindIndex(aFace2);
      Standard_Integer i1 = aNodes1.Lower();
      Standard_Integer i2 = aNodes2.Lower();
      gp_Trsf aTrsf1 = aFace1.Location().Transformation();
      gp_Trsf aTrsf2 = aFace2.Location().Transformation();
      for (; i1 <= aNodes1.Upper(); i1++, i2++) {
	gp_Pnt aP1 = aPoints1(aNodes1(i1)).Transformed(aTrsf1);
	gp_Pnt aP2 = aPoints2(aNodes2(i2)).Transformed(aTrsf2);
	Standard_Real aDist = aP1.Distance(aP2);
	if (aDist > aDefle) {
	  myErrors.Append(iF1);
	  myErrors.Append(i1);
	  myErrors.Append(iF2);
	  myErrors.Append(i2);
	  myErrorsVal.Append(aDist);
	}
      }
    }
  }

  // check triangulations
  Standard_Integer iF;
  for (iF=1; iF <= aMapF.Extent(); iF++) {
    const TopoDS_Face& aFace = TopoDS::Face(aMapF.FindKey(iF));
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aT = BRep_Tool::Triangulation(aFace, aLoc);
    if (aT.IsNull()) {
      di << "face " <<iF <<" has no triangulation\n";
      continue;
    }

    // remember boundary nodes
    TColStd_PackedMapOfInteger aMapBndNodes;
    TopExp_Explorer ex(aFace, TopAbs_EDGE);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Edge& aEdge = TopoDS::Edge(ex.Current());
      Handle(Poly_PolygonOnTriangulation) aPoly =
	BRep_Tool::PolygonOnTriangulation(aEdge, aT, aLoc);
      if (aPoly.IsNull()) continue;
      const TColStd_Array1OfInteger& aNodes = aPoly->Nodes();
      Standard_Integer i;
      for (i=aNodes.Lower(); i <= aNodes.Upper(); i++)
	aMapBndNodes.Add(aNodes(i));
    }

    TColStd_PackedMapOfInteger aUsedNodes;

    // check of free links and nodes
    Poly_Connect aConn(aT);
    const Poly_Array1OfTriangle& aTriangles = aT->Triangles();
    Standard_Integer nbTri = aT->NbTriangles(), i, j, n[3], t[3];
    for (i = 1; i <= nbTri; i++) {
      aTriangles(i).Get(n[0], n[1], n[2]);
      
      aUsedNodes.Add (n[0]);
      aUsedNodes.Add (n[1]);
      aUsedNodes.Add (n[2]);

      aConn.Triangles(i, t[0], t[1], t[2]);
      for (j = 0; j < 3; j++) {
	if (t[j] == 0) {
	  // free link found
	  Standard_Integer k = (j+1) % 3;  // the following node of the edge
	  Standard_Integer n1 = n[j];
	  Standard_Integer n2 = n[k];
	  // skip if it is on boundary
	  if (aMapBndNodes.Contains(n1) && aMapBndNodes.Contains(n2))
	    continue;
	  if (!myMapFaceLinks.Contains(iF)) {
            Handle(TColStd_HSequenceOfInteger) tmpSeq = new TColStd_HSequenceOfInteger;
	    myMapFaceLinks.Add(iF, tmpSeq);
          }
	  Handle(TColStd_HSequenceOfInteger)& aSeq = myMapFaceLinks.ChangeFromKey(iF);
	  aSeq->Append(n1);
	  aSeq->Append(n2);
	}
      }
    }
    
    // check of free nodes
    Standard_Integer aNbNodes = aT->NbNodes();
    for (Standard_Integer k = 1; k <= aNbNodes; k++)
      if ( ! aUsedNodes.Contains(k) )
      {
	myFreeNodeFaces.Append (iF);
	myFreeNodeNums.Append (k);
      }
  }
}

//=======================================================================
//function : GetFreeLink
//purpose  : gets the numbers of nodes of a free link with the given index
//           in the face with the given index
//=======================================================================

void MeshTest_CheckTopology::GetFreeLink(const Standard_Integer theFaceIndex,
	 			 	 const Standard_Integer theLinkIndex,
					 Standard_Integer& theNode1,
					 Standard_Integer& theNode2) const
{
  const Handle(TColStd_HSequenceOfInteger)& aSeq = myMapFaceLinks(theFaceIndex);
  Standard_Integer aInd = (theLinkIndex-1)*2 + 1;
  theNode1 = aSeq->Value(aInd);
  theNode2 = aSeq->Value(aInd+1);
}

//=======================================================================
//function : GetCrossFaceError
//purpose  : gets the attributes of a cross face error with the given index
//=======================================================================

void MeshTest_CheckTopology::GetCrossFaceError(const Standard_Integer theIndex,
					       Standard_Integer& theFace1,
					       Standard_Integer& theNode1,
					       Standard_Integer& theFace2,
					       Standard_Integer& theNode2,
					       Standard_Real&    theValue) const
{
  Standard_Integer aInd = (theIndex-1)*4 + 1;
  theFace1 = myErrors(aInd);
  theNode1 = myErrors(aInd+1);
  theFace2 = myErrors(aInd+2);
  theNode2 = myErrors(aInd+3);
  theValue = myErrorsVal(theIndex);
}
