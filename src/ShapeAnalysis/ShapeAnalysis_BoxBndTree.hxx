// Created on: 2005-02-14
// Created by: Alexey MORENOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef ShapeAnalysis_BoxBndTree_HeaderFile
#define ShapeAnalysis_BoxBndTree_HeaderFile

#include <NCollection_UBTree.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Transient.hxx>
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
    
  void DefinePnt (const gp_Pnt& theFPnt, const gp_Pnt& theLPnt)
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
  
  inline Standard_Boolean LastCheckStatus (const ShapeExtend_Status theStatus) const
    { return ShapeExtend::DecodeStatus ( myStatus, theStatus ); }
    
  Standard_Boolean Reject (const Bnd_Box& theBnd) const;
  Standard_Boolean Accept (const Standard_Integer &);
  
 private:
  Bnd_Box                              myFBox;
  Bnd_Box                              myLBox;
  Handle (TopTools_HArray1OfShape)     mySeq;
  Standard_Boolean                     myShared;
  Standard_Integer                     myNb;
  TopoDS_Vertex                        myFVertex;
  TopoDS_Vertex                        myLVertex;
  gp_Pnt                               myFPnt;
  gp_Pnt                               myLPnt;
  TColStd_MapOfInteger                 myList;
  Standard_Real                        myTol;
  Standard_Real                        myMin3d;
  TColStd_Array1OfInteger              myArrIndices;
  Standard_Integer                     myStatus;

};

#endif
