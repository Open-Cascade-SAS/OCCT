// File:      ShapeAnalysis_BoxBndTree.hxx
// Created:   14.02.05 12:38:56
// Author:    Alexey MORENOV
// Copyright: Open CASCADE  2005

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

typedef NCollection_UBTree <Standard_Integer , Bnd_Box> ShapeAnalysis_BoxBndTree;

class ShapeAnalysis_BoxBndTreeSelector
  : public ShapeAnalysis_BoxBndTree::Selector
{
 public:
  ShapeAnalysis_BoxBndTreeSelector
    (Handle (TopTools_HArray1OfShape) theSeq,
     Standard_Boolean theShared)
    :  mySeq(theSeq), myShared(theShared), myNb(0), myTol(1e-7), myMin3d(1e-7),
      myStatus(ShapeExtend::EncodeStatus (ShapeExtend_OK)){}
  
  void DefineBoxes (const Bnd_Box& theFBox, const Bnd_Box& theLBox)
    { myFBox = theFBox;
      myLBox = theLBox; }
  
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
};

#endif
