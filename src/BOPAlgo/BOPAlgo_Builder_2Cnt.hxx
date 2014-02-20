// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef BOPAlgo_Builder_2Cnt_HeaderFile
#define BOPAlgo_Builder_2Cnt_HeaderFile

#include <NCollection_IndexedDataMap.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <BOPCol_TBB.hxx>
#include <BOPCol_NCVector.hxx>

#include <BOPTools_Set.hxx>
#include <BOPTools_SetMapHasher.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPInt_Context.hxx>

#include <BOPAlgo_BuilderFace.hxx>
#include <BOPAlgo_BuilderSolid.hxx>

typedef BOPCol_NCVector<BOPAlgo_BuilderFace> BOPAlgo_VectorOfBuilderFace;
//
//=======================================================================
//class    : BOPAlgo_BuilderFaceFunctor
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderFaceFunctor {
 protected:
  BOPAlgo_VectorOfBuilderFace* myPVBF;
  //
 public:
  //
  BOPAlgo_BuilderFaceFunctor(BOPAlgo_VectorOfBuilderFace& aVBF) 
    : myPVBF(&aVBF) {
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const{
    Standard_Integer i, iBeg, iEnd;
    //
    BOPAlgo_VectorOfBuilderFace& aVBF=*myPVBF;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_BuilderFace& aBF=aVBF(i);
      //
      aBF.Perform();
    }
  }
};
//=======================================================================
//class    : BOPAlgo_BuilderFaceCnt
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderFaceCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static void Perform(const Standard_Boolean bRunParallel,
				      BOPAlgo_VectorOfBuilderFace& aVBF) {
    //
    BOPAlgo_BuilderFaceFunctor aBFF(aVBF);
    Standard_Integer aNbBF=aVBF.Extent();
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Integer>(0,aNbBF), aBFF);
    }
    else {
      aBFF.operator()(flexible_range<Standard_Integer>(0,aNbBF));
    }
  }
  //
};
//
typedef BOPCol_NCVector<TopoDS_Shape> BOPAlgo_VectorOfShape;
//
typedef BOPCol_NCVector<BOPAlgo_VectorOfShape> \
  BOPAlgo_VectorOfVectorOfShape;
//
typedef NCollection_IndexedDataMap\
  <BOPTools_Set, Standard_Integer, BOPTools_SetMapHasher> \
    BOPAlgo_IndexedDataMapOfSetInteger;
//
//=======================================================================
//class    : BOPAlgo_PairOfShapeBoolean
//purpose  : 
//=======================================================================
class BOPAlgo_PairOfShapeBoolean {
 public:
  BOPAlgo_PairOfShapeBoolean()
    : myFlag(Standard_False) {
  }
  //
  TopoDS_Shape& Shape1() {
    return myShape1;
  }
  //
  TopoDS_Shape& Shape2() {
    return myShape2;
  }
  //
  Standard_Boolean& Flag() {
    return myFlag;
  }
  //
 protected: 
  Standard_Boolean myFlag;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
};
//
typedef BOPCol_NCVector<BOPAlgo_PairOfShapeBoolean> \
  BOPAlgo_VectorOfPairOfShapeBoolean;
//
//=======================================================================
//function : BOPAlgo_BuilderSDFaceFunctor
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderSDFaceFunctor {
 protected:
  BOPAlgo_VectorOfPairOfShapeBoolean* myPVPSB;
  
 public:
  //
  BOPAlgo_BuilderSDFaceFunctor(BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB)
    : myPVPSB(&aVPSB){
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const {
    Standard_Boolean bFlag;
    Standard_Integer i, iBeg, iEnd;
    Handle(BOPInt_Context) aContext;
    //
    aContext=new BOPInt_Context;
    //
    BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB=*myPVPSB;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_PairOfShapeBoolean& aPSB=aVPSB(i);
      const TopoDS_Face& aFj=(*(TopoDS_Face*)(&aPSB.Shape1()));
      const TopoDS_Face& aFk=(*(TopoDS_Face*)(&aPSB.Shape2()));
      bFlag=BOPTools_AlgoTools::AreFacesSameDomain(aFj, aFk, aContext);
      if (bFlag) {
	aPSB.Flag()=bFlag;
      }
    }
  }
};
//
//=======================================================================
//function : BOPAlgo_BuilderSDFaceCnt
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderSDFaceCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static 
    void Perform(const Standard_Boolean bRunParallel,
		 BOPAlgo_VectorOfPairOfShapeBoolean& aVPSB) {
    Standard_Integer aNbVPSB;
    //
    aNbVPSB=aVPSB.Extent();
    BOPAlgo_BuilderSDFaceFunctor aBFF(aVPSB);
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Integer>(0,aNbVPSB), aBFF);
    }
    else {
      aBFF.operator()(flexible_range<Standard_Integer>(0,aNbVPSB));
    }
  }
  //
};
//
typedef BOPCol_NCVector<BOPAlgo_BuilderSolid> BOPAlgo_VectorOfBuilderSolid;
//
//=======================================================================
//class    : BOPAlgo_BuilderSolidFunctor
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderSolidFunctor {
 protected:
  BOPAlgo_VectorOfBuilderSolid* myPVBS;
  //
 public:
  //
  BOPAlgo_BuilderSolidFunctor(BOPAlgo_VectorOfBuilderSolid& aVBS) 
    : myPVBS(&aVBS) {
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const{
    Standard_Integer i, iBeg, iEnd;
    //
    BOPAlgo_VectorOfBuilderSolid& aVBS=*myPVBS;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_BuilderSolid& aBS=aVBS(i);
      //
      aBS.Perform();
    }
  }
};
//=======================================================================
//class    : BOPAlgo_BuilderSolidCnt
//purpose  : The class provides the interface and implementation 
//           of the parallel computations
//=======================================================================
class BOPAlgo_BuilderSolidCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static void Perform(const Standard_Boolean bRunParallel,
				      BOPAlgo_VectorOfBuilderSolid& aVBS) {
    
    Standard_Integer aNbBS=aVBS.Extent();
    BOPAlgo_BuilderSolidFunctor aBSF(aVBS);
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Integer>(0,aNbBS), aBSF);
    }
    else {
      aBSF.operator()(flexible_range<Standard_Integer>(0,aNbBS));
    }
  }
  //
};

#endif
