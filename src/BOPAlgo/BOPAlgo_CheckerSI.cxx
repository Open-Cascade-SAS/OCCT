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

//

#include <BOPAlgo_CheckerSI.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PIteratorSI.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BRep_Tool.hxx>
#include <gp_Torus.hxx>
#include <TopExp.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_FaceFace.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//class    : BOPAlgo_FaceSelfIntersect
//purpose  : 
//=======================================================================
class BOPAlgo_FaceSelfIntersect : 
  public IntTools_FaceFace,
  public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_FaceSelfIntersect() : 
    IntTools_FaceFace(),  
    BOPAlgo_Algo(),
    myIF(-1), myTolF(1.e-7) {
  }
  //
  virtual ~BOPAlgo_FaceSelfIntersect() {
  }
  //
  void SetIndex(const Standard_Integer nF) {
    myIF = nF;
  }
  //
  Standard_Integer IndexOfFace() const {
    return myIF;
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myF = aF;
  }
  //
  const TopoDS_Face& Face()const {
    return myF;
  }
  //
  void SetTolF(const Standard_Real aTolF) {
    myTolF = aTolF;
  }
  //
  Standard_Real TolF() const{
    return myTolF;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    IntTools_FaceFace::Perform(myF, myF);
  }
  //
 protected:
  Standard_Integer myIF;
  Standard_Real myTolF;
  TopoDS_Face myF;
};
//end of definition of class BOPAlgo_FaceSelfIntersect

//=======================================================================

typedef NCollection_Vector
  <BOPAlgo_FaceSelfIntersect> BOPAlgo_VectorOfFaceSelfIntersect; 
//
typedef BOPTools_Functor 
  <BOPAlgo_FaceSelfIntersect,
  BOPAlgo_VectorOfFaceSelfIntersect> BOPAlgo_FaceSelfIntersectFunctor;
//
typedef BOPTools_Cnt 
  <BOPAlgo_FaceSelfIntersectFunctor,
  BOPAlgo_VectorOfFaceSelfIntersect> BOPAlgo_FaceSelfIntersectCnt;


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_CheckerSI::BOPAlgo_CheckerSI()
:
  BOPAlgo_PaveFiller()
{
  myLevelOfCheck=BOPDS_DS::NbInterfTypes()-1;
  myNonDestructive=Standard_True;
  SetAvoidBuildPCurve(Standard_True);
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_CheckerSI::~BOPAlgo_CheckerSI()
{
}
//=======================================================================
//function : SetLevelOfCheck
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::SetLevelOfCheck(const Standard_Integer theLevel)
{
  Standard_Integer aNbLists;
  //
  aNbLists=BOPDS_DS::NbInterfTypes();
  if (theLevel >= 0 && theLevel < aNbLists) {
    myLevelOfCheck = theLevel;
  }
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Init()
{
  Clear();
  //
  // 1. myDS
  myDS=new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->Init(myFuzzyValue);
  //
  // 2 myContext
  myContext=new IntTools_Context;
  //
  // 3.myIterator 
  BOPDS_PIteratorSI theIterSI=new BOPDS_IteratorSI(myAllocator);
  theIterSI->SetDS(myDS);
  theIterSI->Prepare(myContext, myUseOBB, myFuzzyValue);
  theIterSI->UpdateByLevelOfCheck(myLevelOfCheck);
  //
  myIterator=theIterSI;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::Perform()
{
  try {
    OCC_CATCH_SIGNALS
    //
    if (myArguments.Extent() != 1) {
      AddError (new BOPAlgo_AlertMultipleArguments);
      return;
    }
    //
    // Perform intersection of sub shapes
    BOPAlgo_PaveFiller::Perform();
    //
    CheckFaceSelfIntersection();
    
    // Perform intersection with solids
    if (!HasErrors())
      PerformVZ();
    //
    if (!HasErrors())
      PerformEZ();
    //
    if (!HasErrors())
      PerformFZ();
    //
    if (!HasErrors())
      PerformZZ();
    //
    // Treat the intersection results
    PostTreat();
  }
  //
  catch (Standard_Failure) {
    AddError (new BOPAlgo_AlertIntersectionFailed);
  }
}
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::PostTreat()
{
  Standard_Integer i, aNb, n1, n2; 
  BOPDS_Pair aPK;
  //
  BOPDS_MapOfPair& aMPK=
    *((BOPDS_MapOfPair*)&myDS->Interferences());

  // 0
  BOPDS_VectorOfInterfVV& aVVs=myDS->InterfVV();
  aNb=aVVs.Length();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVV& aVV=aVVs(i);
    aVV.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 1
  BOPDS_VectorOfInterfVE& aVEs=myDS->InterfVE();
  aNb=aVEs.Length();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVE& aVE=aVEs(i);
    aVE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 2
  BOPDS_VectorOfInterfEE& aEEs=myDS->InterfEE();
  aNb=aEEs.Length();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfEE& aEE=aEEs(i);
    aEE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 3
  BOPDS_VectorOfInterfVF& aVFs=myDS->InterfVF();
  aNb=aVFs.Length();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfVF& aVF=aVFs(i);
    aVF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 4
  BOPDS_VectorOfInterfEF& aEFs=myDS->InterfEF();
  aNb=aEFs.Length();
  for (i=0; i!=aNb; ++i) {
    const BOPDS_InterfEF& aEF=aEFs(i);
    if (aEF.CommonPart().Type()==TopAbs_SHAPE) {
      continue;
    }
    aEF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 5
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNb=aFFs.Length();
  for (i=0; i!=aNb; ++i) {
    Standard_Boolean bTangentFaces, bFlag;
    Standard_Integer aNbC, aNbP, j, iFound;
    //
    const BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(n1, n2);
    //
    bTangentFaces=aFF.TangentFaces();
    aNbP=aFF.Points().Length();
    const BOPDS_VectorOfCurve& aVC=aFF.Curves();
    aNbC=aVC.Length();
    if (!aNbP && !aNbC && !bTangentFaces) {
      continue;
    }
    //
    iFound = (n1 == n2) ? 1 : 0;
    //case of self-intersection inside one face
    if (!iFound)
    {
      if (bTangentFaces) {
        const TopoDS_Face& aF1=*((TopoDS_Face*)&myDS->Shape(n1));
        const TopoDS_Face& aF2=*((TopoDS_Face*)&myDS->Shape(n2));
        bFlag=BOPTools_AlgoTools::AreFacesSameDomain
          (aF1, aF2, myContext, myFuzzyValue);
        if (bFlag) {
          ++iFound;
        }
      }
      else {
        for (j=0; j!=aNbC; ++j) {
          const BOPDS_Curve& aNC=aVC(j);
          const BOPDS_ListOfPaveBlock& aLPBC=aNC.PaveBlocks();
          if (aLPBC.Extent()) {
            ++iFound;
            break;
          }
        }
      }
    }
    //
    if (!iFound) {
      continue;
    }
    //
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  //
  // 6
  BOPDS_VectorOfInterfVZ& aVZs=myDS->InterfVZ();
  aNb=aVZs.Length();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfVZ& aVZ=aVZs(i);
    aVZ.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2)) {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 7
  BOPDS_VectorOfInterfEZ& aEZs=myDS->InterfEZ();
  aNb=aEZs.Length();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfEZ& aEZ=aEZs(i);
    aEZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 8
  BOPDS_VectorOfInterfFZ& aFZs=myDS->InterfFZ();
  aNb=aFZs.Length();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfFZ& aFZ=aFZs(i);
    aFZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 9
  BOPDS_VectorOfInterfZZ& aZZs=myDS->InterfZZ();
  aNb=aZZs.Length();
  for (i=0; i!=aNb; ++i) {
    //
    const BOPDS_InterfZZ& aZZ=aZZs(i);
    aZZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
}

//=======================================================================
//function : CheckFaceSelfIntersection
//purpose  : 
//=======================================================================
void BOPAlgo_CheckerSI::CheckFaceSelfIntersection()
{
  if (myLevelOfCheck < 5)
    return;
  
  BOPDS_Pair aPK;

  BOPDS_MapOfPair& aMPK=
    *((BOPDS_MapOfPair*)&myDS->Interferences());
  aMPK.Clear();
  
  BOPAlgo_VectorOfFaceSelfIntersect aVFace;
  
  Standard_Integer aNbS=myDS->NbSourceShapes();
  
  //
  for (Standard_Integer i = 0; i < aNbS; i++)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;
    //
    const TopoDS_Face& aF = (*(TopoDS_Face*)(&aSI.Shape()));
    BRepAdaptor_Surface BAsurf(aF, Standard_False);
    GeomAbs_SurfaceType aSurfType = BAsurf.GetType();
    if (aSurfType == GeomAbs_Plane ||
        aSurfType == GeomAbs_Cylinder ||
        aSurfType == GeomAbs_Cone ||
        aSurfType == GeomAbs_Sphere)
      continue;

    if (aSurfType == GeomAbs_Torus)
    {
      gp_Torus aTorus = BAsurf.Torus();
      Standard_Real aMajorRadius = aTorus.MajorRadius();
      Standard_Real aMinorRadius = aTorus.MinorRadius();
      if (aMajorRadius > aMinorRadius + Precision::Confusion())
        continue;
    }

    Standard_Real aTolF = BRep_Tool::Tolerance(aF);
    
    BOPAlgo_FaceSelfIntersect& aFaceSelfIntersect = aVFace.Appended();
    //
    aFaceSelfIntersect.SetIndex(i);
    aFaceSelfIntersect.SetFace(aF);
    aFaceSelfIntersect.SetTolF(aTolF);
    //
    aFaceSelfIntersect.SetProgressIndicator(myProgressIndicator);
  }
  
  Standard_Integer aNbFace = aVFace.Length();
  //======================================================
  BOPAlgo_FaceSelfIntersectCnt::Perform(myRunParallel, aVFace);
  //======================================================
  //
  for (Standard_Integer k = 0; k < aNbFace; k++)
  {
    BOPAlgo_FaceSelfIntersect& aFaceSelfIntersect = aVFace(k);
    //
    Standard_Integer nF = aFaceSelfIntersect.IndexOfFace();

    Standard_Boolean bIsDone = aFaceSelfIntersect.IsDone();
    if (bIsDone)
    {
      const IntTools_SequenceOfCurves& aCvsX = aFaceSelfIntersect.Lines();
      const IntTools_SequenceOfPntOn2Faces& aPntsX = aFaceSelfIntersect.Points();
      //
      Standard_Integer aNbCurves = aCvsX.Length();
      Standard_Integer aNbPoints = aPntsX.Length();
      //
      if (aNbCurves || aNbPoints)
      {
        aPK.SetIndices(nF, nF);
        aMPK.Add(aPK);
      }
    }
  }
}
