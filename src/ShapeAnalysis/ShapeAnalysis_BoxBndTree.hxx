// Created on: 2005-02-14
// Created by: Alexey MORENOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef ShapeAnalysis_BoxBndTree_HeaderFile
#define ShapeAnalysis_BoxBndTree_HeaderFile

#include <NCollection_UBTree.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <MMgt_TShared.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <ShapeExtend.hxx>
#include <ShapeExtend_Status.hxx>
#include <TopoDS_Vertex.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_Array1OfInteger.hxx>

typedef NCollection_UBTree <Standard_Integer , Bnd_Box> ShapeAnalysis_BoxBndTree;

class ShapeAnalysis_BoxBndTreeSelector
  : public ShapeAnalysis_BoxBndTree::Selector
{
 public:
  ShapeAnalysis_BoxBndTreeSelector
    (Handle (TopTools_HArray1OfShape) theSeq,
     Standard_Boolean theShared)
    :  mySeq(theSeq), myShared(theShared), myNb(0), myTol(1e-7), myMin3d(1e-7),myArrIndices(1,2),
      myStatus(ShapeExtend::EncodeStatus (ShapeExtend_OK))
      {
        myArrIndices.Init(0);
      }
  
  void DefineBoxes (const Bnd_Box& theFBox, const Bnd_Box& theLBox)
    { myFBox = theFBox;
      myLBox = theLBox; 
      myArrIndices.Init(0);
       
  }
  
  void DefineVertexes (TopoDS_Vertex theVf, TopoDS_Vertex theVl)
    { myFVertex = theVf;
      myLVertex = theVl; 
       myStatus=ShapeExtend::EncodeStatus (ShapeExtend_OK);
    }
    
  void DefinePnt (gp_Pnt theFPnt, gp_Pnt theLPnt)
    { myFPnt = theFPnt;
      myLPnt = theLPnt; 
       myStatus =ShapeExtend::EncodeStatus (ShapeExtend_OK);
    }
  
  Standard_Integer GetNb ()
    { return myNb; }
  
  void  SetNb (Standard_Integer theNb)
    { myNb = theNb; }
 
  void LoadList(Standard_Integer elem)
    { myList.Add(elem); }
  
  void SetStop ()
    { myStop = Standard_False; }

  void SetTolerance (Standard_Real theTol)
    { 
      myTol = theTol;
      myMin3d = theTol; 
      myStatus=ShapeExtend::EncodeStatus (ShapeExtend_OK);
    }

  Standard_Boolean ContWire(Standard_Integer nbWire)
    { return myList.Contains(nbWire); }
  
  inline Standard_Boolean LastCheckStatus (const ShapeExtend_Status Status) const
    { return ShapeExtend::DecodeStatus ( myStatus, Status ); }
    
  Standard_Boolean Reject (const Bnd_Box& theBnd) const;
  Standard_Boolean Accept (const Standard_Integer &);
  
 private:
  Bnd_Box                              myFBox;
  Bnd_Box                              myLBox;
  Handle (TopTools_HArray1OfShape)     mySeq;
  Standard_Boolean                     myShared;
  Standard_Integer                     myStatus;
  Standard_Integer                     myNb;
  TopoDS_Vertex                        myFVertex;
  TopoDS_Vertex                        myLVertex;
  gp_Pnt                               myFPnt;
  gp_Pnt                               myLPnt;
  TColStd_MapOfInteger                 myList;
  Standard_Real                        myTol;
  Standard_Real                        myMin3d;
  TColStd_Array1OfInteger              myArrIndices;
};

#endif
