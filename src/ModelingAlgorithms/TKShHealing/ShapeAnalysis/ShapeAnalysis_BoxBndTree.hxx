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
#include <TopoDS_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <ShapeExtend.hxx>
#include <ShapeExtend_Status.hxx>
#include <TopoDS_Vertex.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>

typedef NCollection_UBTree<int, Bnd_Box> ShapeAnalysis_BoxBndTree;

class ShapeAnalysis_BoxBndTreeSelector : public NCollection_UBTree<int, Bnd_Box>::Selector
{
public:
  ShapeAnalysis_BoxBndTreeSelector(occ::handle<NCollection_HArray1<TopoDS_Shape>> theSeq,
                                   bool                                           theShared)
      : mySeq(theSeq),
        myShared(theShared),
        myNb(0),
        myTol(1e-7),
        myMin3d(1e-7),
        myArrIndices(1, 2),
        myStatus(ShapeExtend::EncodeStatus(ShapeExtend_OK))
  {
    myArrIndices.Init(0);
  }

  void DefineBoxes(const Bnd_Box& theFBox, const Bnd_Box& theLBox)
  {
    myFBox = theFBox;
    myLBox = theLBox;
    myArrIndices.Init(0);
  }

  void DefineVertexes(TopoDS_Vertex theVf, TopoDS_Vertex theVl)
  {
    myFVertex = theVf;
    myLVertex = theVl;
    myStatus  = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  }

  void DefinePnt(const gp_Pnt& theFPnt, const gp_Pnt& theLPnt)
  {
    myFPnt   = theFPnt;
    myLPnt   = theLPnt;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  }

  int GetNb() { return myNb; }

  void SetNb(int theNb) { myNb = theNb; }

  void LoadList(int elem) { myList.Add(elem); }

  void SetStop() { myStop = false; }

  void SetTolerance(double theTol)
  {
    myTol    = theTol;
    myMin3d  = theTol;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  }

  bool ContWire(int nbWire) { return myList.Contains(nbWire); }

  inline bool LastCheckStatus(const ShapeExtend_Status theStatus) const
  {
    return ShapeExtend::DecodeStatus(myStatus, theStatus);
  }

  bool Reject(const Bnd_Box& theBnd) const override;
  bool Accept(const int&) override;

private:
  Bnd_Box                                        myFBox;
  Bnd_Box                                        myLBox;
  occ::handle<NCollection_HArray1<TopoDS_Shape>> mySeq;
  bool                                           myShared;
  int                                            myNb;
  TopoDS_Vertex                                  myFVertex;
  TopoDS_Vertex                                  myLVertex;
  gp_Pnt                                         myFPnt;
  gp_Pnt                                         myLPnt;
  NCollection_Map<int>                           myList;
  double                                         myTol;
  double                                         myMin3d;
  NCollection_Array1<int>                        myArrIndices;
  int                                            myStatus;
};

#endif
