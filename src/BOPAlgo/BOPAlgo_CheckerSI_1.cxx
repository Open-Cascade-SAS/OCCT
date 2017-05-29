// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_CheckerSI.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_IteratorSI.hxx>

#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>

#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <IntTools_Context.hxx>

#include <gp_Pnt.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_VertexSolid
//purpose  : 
//=======================================================================
class BOPAlgo_VertexSolid  {
 public:
  DEFINE_STANDARD_ALLOC
  
  BOPAlgo_VertexSolid()
    : myIV(-1), myIZ(-1), myState(TopAbs_UNKNOWN) {
  };
  //
  virtual ~BOPAlgo_VertexSolid(){
  };
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nZ){
    myIV=nV;
    myIZ=nZ;
  }
  //
  void Indices(Standard_Integer& nV,
               Standard_Integer& nZ) const {
    nV=myIV;
    nZ=myIZ;
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  const TopoDS_Vertex& Vertex()const {
    return myV;
  }
  //
  void SetSolid(const TopoDS_Solid& aZ) {
    myZ=aZ;
  }
  //
  const TopoDS_Solid& Solid()const {
    return myZ;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  TopAbs_State State() const{
    return myState;
  };
  //
  void Perform() {
    Standard_Real aTol;
    gp_Pnt aPV;
    //
    BRepClass3d_SolidClassifier& aSC=myContext->SolidClassifier(myZ);
    //
    aPV=BRep_Tool::Pnt(myV);
    aTol=BRep_Tool::Tolerance(myV);
    //
    aSC.Perform(aPV, aTol);
    //
    myState=aSC.State();
  };
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIZ;
  TopAbs_State myState;
  TopoDS_Vertex myV;
  TopoDS_Solid myZ;
  Handle(IntTools_Context) myContext;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_VertexSolid> BOPAlgo_VectorOfVertexSolid; 
//
typedef BOPCol_ContextFunctor 
  <BOPAlgo_VertexSolid,
  BOPAlgo_VectorOfVertexSolid,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_VertexSolidFunctor;
//
typedef BOPCol_ContextCnt 
  <BOPAlgo_VertexSolidFunctor,
  BOPAlgo_VectorOfVertexSolid,
  Handle(IntTools_Context)> BOPAlgo_VertexSolidCnt;
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_ShapeSolid
//purpose  : 
//=======================================================================
class BOPAlgo_ShapeSolid  {
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_ShapeSolid() : 
    myIE(-1), 
    myIZ(-1), 
    myHasInterf(Standard_False), 
    myDS(NULL) {
  };
  //
  virtual ~BOPAlgo_ShapeSolid(){
  };
  //
  void SetIndices(const Standard_Integer nE,
                  const Standard_Integer nZ){
    myIE=nE;
    myIZ=nZ;
  }
  //
  void Indices(Standard_Integer& nE,
               Standard_Integer& nZ) const {
    nE=myIE;
    nZ=myIZ;
  }
  //
  void SetDS(BOPDS_DS* pDS) {
    myDS=pDS;
  }
  //
  Standard_Boolean HasInterf() const{
    return myHasInterf;
  };
  //
  virtual void Perform() {
    Standard_Boolean bHasInterf;
    //
    myHasInterf=Standard_False;
    //
    bHasInterf=myDS->HasInterfShapeSubShapes(myIE, myIZ);
    if (!bHasInterf) {
      myHasInterf=myDS->HasInterfShapeSubShapes(myIZ, myIE);
    }
  };
  //
 protected:
  Standard_Integer myIE;
  Standard_Integer myIZ;
  Standard_Boolean myHasInterf;
  BOPDS_DS* myDS;
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_ShapeSolid> BOPAlgo_VectorOfShapeSolid; 
//
typedef BOPCol_Functor 
  <BOPAlgo_ShapeSolid,
  BOPAlgo_VectorOfShapeSolid> BOPAlgo_ShapeSolidFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_ShapeSolidFunctor,
  BOPAlgo_VectorOfShapeSolid> BOPAlgo_ShapeSolidCnt;
//
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPAlgo_SolidSolid
//purpose  : 
//=======================================================================
class BOPAlgo_SolidSolid : public  BOPAlgo_ShapeSolid {
 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_SolidSolid() : 
    BOPAlgo_ShapeSolid() {
  };
  //
  virtual ~BOPAlgo_SolidSolid(){
  };
  //
  virtual void Perform() {
    Standard_Boolean bFlag;
    //
    bFlag=Standard_False;
    myHasInterf=Standard_False;
    //
    myHasInterf=myDS->HasInterfShapeSubShapes(myIZ, myIE, bFlag);
    if (!myHasInterf) {
      myHasInterf=myDS->HasInterfShapeSubShapes(myIE, myIZ, bFlag);
    }
  };
};
//=======================================================================
typedef BOPCol_NCVector
  <BOPAlgo_SolidSolid> BOPAlgo_VectorOfSolidSolid; 
//
typedef BOPCol_Functor 
  <BOPAlgo_SolidSolid,
  BOPAlgo_VectorOfSolidSolid> BOPAlgo_SolidSolidFunctor;
//
typedef BOPCol_Cnt 
  <BOPAlgo_SolidSolidFunctor,
  BOPAlgo_VectorOfSolidSolid> BOPAlgo_SolidSolidCnt;
//
/////////////////////////////////////////////////////////////////////////

//=======================================================================
//function : PerformVZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformVZ()
{
  Standard_Integer iSize, nV, nZ, k, aNbVVS;
  TopAbs_State aState;
  BOPDS_MapOfPair aMPK;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPDS_VectorOfInterfVZ& aVZs=myDS->InterfVZ();
  aVZs.SetIncrement(iSize);
  //
  BOPAlgo_VectorOfVertexSolid aVVS;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nV, nZ);
    //
    if (myDS->HasInterfShapeSubShapes(nV, nZ)) {
      continue;
    }
    //
    Standard_Integer nVSD = nV;
    myDS->HasShapeSD(nV, nVSD);
    //
    BOPDS_Pair aPK;
    aPK.SetIndices(nVSD, nZ);
    if (!aMPK.Add(aPK)) {
      continue;
    }
    //
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&myDS->Shape(nVSD));
    const TopoDS_Solid& aZ=*((TopoDS_Solid*)&myDS->Shape(nZ));
    //
    BOPAlgo_VertexSolid& aVertexSolid=aVVS.Append1();
    aVertexSolid.SetIndices(nV, nZ);
    aVertexSolid.SetVertex(aV);
    aVertexSolid.SetSolid(aZ);
  }
  //
  aNbVVS=aVVS.Extent();
  //=============================================================
  BOPAlgo_VertexSolidCnt::Perform(myRunParallel, aVVS, myContext);
  //=============================================================
  for (k=0; k < aNbVVS; ++k) {
    const BOPAlgo_VertexSolid& aVertexSolid=aVVS(k);
    aState=aVertexSolid.State();
    if (aState==TopAbs_IN)  {
      aVertexSolid.Indices(nV, nZ);
      //
      BOPDS_InterfVZ& aVZ=aVZs.Append1();
      aVZ.SetIndices(nV, nZ);
      //
      myDS->AddInterf(nV, nZ);
    }
  }
}
//=======================================================================
//function : PerformEZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformEZ()
{ 
  PerformSZ(TopAbs_EDGE);
}
//=======================================================================
//function : PerformFZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformFZ()
{ 
  PerformSZ(TopAbs_FACE);
}
//=======================================================================
//function : PerformZZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformZZ()
{
  Standard_Boolean bHasInterf;
  Standard_Integer iSize, nZ1, nZ, k, aNbSolidSolid;
  //
  myIterator->Initialize(TopAbs_SOLID, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPAlgo_VectorOfSolidSolid aVSolidSolid;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nZ1, nZ);
    //
    BOPAlgo_SolidSolid& aSolidSolid=aVSolidSolid.Append1();
    aSolidSolid.SetIndices(nZ1, nZ);
    aSolidSolid.SetDS(myDS);
  }
  //
  aNbSolidSolid=aVSolidSolid.Extent();
  //======================================================
  BOPAlgo_SolidSolidCnt::Perform(myRunParallel, aVSolidSolid);
  //======================================================
  //
  BOPDS_VectorOfInterfZZ& aZZs=myDS->InterfZZ();
  //
  aZZs.SetIncrement(iSize);
  //
  for (k=0; k < aNbSolidSolid; ++k) {
    const BOPAlgo_SolidSolid& aSolidSolid=aVSolidSolid(k);
    bHasInterf=aSolidSolid.HasInterf();
    if (bHasInterf) {
      aSolidSolid.Indices(nZ1, nZ);
      //
      BOPDS_InterfZZ& aZZ=aZZs.Append1();
      aZZ.SetIndices(nZ1, nZ);
      //
      myDS->AddInterf(nZ1, nZ);
    }
  }
}
//=======================================================================
//function : PerformSZ
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PerformSZ(const TopAbs_ShapeEnum aTS)
{
  Standard_Boolean bHasInterf;
  Standard_Integer iSize, nS, nZ, k, aNbShapeSolid;
  //
  myIterator->Initialize(aTS, TopAbs_SOLID);
  iSize=myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  BOPAlgo_VectorOfShapeSolid aVShapeSolid;
  //
  for (; myIterator->More(); myIterator->Next()) {
    myIterator->Value(nS, nZ);
    //
    BOPAlgo_ShapeSolid& aShapeSolid=aVShapeSolid.Append1();
    aShapeSolid.SetIndices(nS, nZ);
    aShapeSolid.SetDS(myDS);
  }
  //
  aNbShapeSolid=aVShapeSolid.Extent();
  //======================================================
  BOPAlgo_ShapeSolidCnt::Perform(myRunParallel, aVShapeSolid);
  //======================================================
  //
  BOPDS_VectorOfInterfEZ& aEZs=myDS->InterfEZ();
  BOPDS_VectorOfInterfFZ& aFZs=myDS->InterfFZ();
  //
  if (aTS==TopAbs_EDGE) {
    aEZs.SetIncrement(iSize);
  }
  else {//if (aTS==TopAbs_FACE)
    aFZs.SetIncrement(iSize);
  }
  //
  for (k=0; k < aNbShapeSolid; ++k) {
    const BOPAlgo_ShapeSolid& aShapeSolid=aVShapeSolid(k);
    bHasInterf=aShapeSolid.HasInterf();
    if (bHasInterf) {
      aShapeSolid.Indices(nS, nZ);
      //
      if (aTS==TopAbs_EDGE) {
        BOPDS_InterfEZ& aEZ=aEZs.Append1();
        aEZ.SetIndices(nS, nZ);
      }
      else  {//if (aTS==TopAbs_FACE)
        BOPDS_InterfFZ& aFZ=aFZs.Append1();
        aFZ.SetIndices(nS, nZ);
      }
      //
      myDS->AddInterf(nS, nZ);
    }
  }
}
