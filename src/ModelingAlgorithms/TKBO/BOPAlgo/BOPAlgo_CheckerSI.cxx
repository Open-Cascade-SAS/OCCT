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
#include <NCollection_Map.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PIteratorSI.hxx>
#include <NCollection_Vector.hxx>
#include <BRep_Tool.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_FaceFace.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//=================================================================================================

class BOPAlgo_FaceSelfIntersect : public IntTools_FaceFace, public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_FaceSelfIntersect()
      : IntTools_FaceFace(),
        BOPAlgo_ParallelAlgo(),
        myIF(-1),
        myTolF(1.e-7)
  {
  }

  //
  virtual ~BOPAlgo_FaceSelfIntersect() {}

  //
  void SetIndex(const int nF) { myIF = nF; }

  //
  int IndexOfFace() const { return myIF; }

  //
  void SetFace(const TopoDS_Face& aF) { myF = aF; }

  //
  const TopoDS_Face& Face() const { return myF; }

  //
  void SetTolF(const double aTolF) { myTolF = aTolF; }

  //
  double TolF() const { return myTolF; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    IntTools_FaceFace::Perform(myF, myF, myRunParallel);
  }

  //
protected:
  int         myIF;
  double      myTolF;
  TopoDS_Face myF;
};

// end of definition of class BOPAlgo_FaceSelfIntersect

//=======================================================================

typedef NCollection_Vector<BOPAlgo_FaceSelfIntersect> BOPAlgo_VectorOfFaceSelfIntersect;

//=================================================================================================

BOPAlgo_CheckerSI::BOPAlgo_CheckerSI()
    : BOPAlgo_PaveFiller()
{
  myLevelOfCheck   = BOPDS_DS::NbInterfTypes() - 1;
  myNonDestructive = true;
  SetAvoidBuildPCurve(true);
}

//=================================================================================================

BOPAlgo_CheckerSI::~BOPAlgo_CheckerSI() {}

//=================================================================================================

void BOPAlgo_CheckerSI::SetLevelOfCheck(const int theLevel)
{
  int aNbLists;
  //
  aNbLists = BOPDS_DS::NbInterfTypes();
  if (theLevel >= 0 && theLevel < aNbLists)
  {
    myLevelOfCheck = theLevel;
  }
}

//=================================================================================================

void BOPAlgo_CheckerSI::Init(const Message_ProgressRange& /*theRange*/)
{
  Clear();
  //
  // 1. myDS
  myDS = new BOPDS_DS(myAllocator);
  myDS->SetArguments(myArguments);
  myDS->Init(myFuzzyValue);
  //
  // 2 myContext
  myContext = new IntTools_Context;
  //
  // 3.myIterator
  BOPDS_PIteratorSI theIterSI = new BOPDS_IteratorSI(myAllocator);
  theIterSI->SetDS(myDS);
  theIterSI->Prepare(myContext, myUseOBB, myFuzzyValue);
  theIterSI->UpdateByLevelOfCheck(myLevelOfCheck);
  //
  myIterator = theIterSI;
}

//=================================================================================================

void BOPAlgo_CheckerSI::Perform(const Message_ProgressRange& theRange)
{
  try
  {
    OCC_CATCH_SIGNALS
    //
    if (myArguments.Extent() != 1)
    {
      AddError(new BOPAlgo_AlertMultipleArguments);
      return;
    }
    //
    Message_ProgressScope aPS(theRange, "Checking shape on self-intersection", 10);
    // Perform intersection of sub shapes
    BOPAlgo_PaveFiller::Perform(aPS.Next(8));
    if (UserBreak(aPS))
    {
      return;
    }
    //
    CheckFaceSelfIntersection(aPS.Next());

    Message_ProgressScope aPSZZ(aPS.Next(), NULL, 4);
    // Perform intersection with solids
    if (!HasErrors())
      PerformVZ(aPSZZ.Next());
    //
    if (!HasErrors())
      PerformEZ(aPSZZ.Next());
    //
    if (!HasErrors())
      PerformFZ(aPSZZ.Next());
    //
    if (!HasErrors())
      PerformZZ(aPSZZ.Next());
    //
    if (HasErrors())
      return;

    // Treat the intersection results
    PostTreat();
  }
  //
  catch (Standard_Failure const&)
  {
    AddError(new BOPAlgo_AlertIntersectionFailed);
  }
}

//=================================================================================================

void BOPAlgo_CheckerSI::PostTreat()
{
  int        i, aNb, n1, n2;
  BOPDS_Pair aPK;
  //
  NCollection_Map<BOPDS_Pair>& aMPK = *((NCollection_Map<BOPDS_Pair>*)&myDS->Interferences());

  // 0
  NCollection_Vector<BOPDS_InterfVV>& aVVs = myDS->InterfVV();
  aNb                                      = aVVs.Length();
  for (i = 0; i != aNb; ++i)
  {
    const BOPDS_InterfVV& aVV = aVVs(i);
    aVV.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 1
  NCollection_Vector<BOPDS_InterfVE>& aVEs = myDS->InterfVE();
  aNb                                      = aVEs.Length();
  for (i = 0; i != aNb; ++i)
  {
    const BOPDS_InterfVE& aVE = aVEs(i);
    aVE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 2
  NCollection_Vector<BOPDS_InterfEE>& aEEs = myDS->InterfEE();
  aNb                                      = aEEs.Length();
  for (i = 0; i != aNb; ++i)
  {
    const BOPDS_InterfEE& aEE = aEEs(i);
    aEE.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 3
  NCollection_Vector<BOPDS_InterfVF>& aVFs = myDS->InterfVF();
  aNb                                      = aVFs.Length();
  for (i = 0; i != aNb; ++i)
  {
    const BOPDS_InterfVF& aVF = aVFs(i);
    aVF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 4
  NCollection_Vector<BOPDS_InterfEF>& aEFs = myDS->InterfEF();
  aNb                                      = aEFs.Length();
  for (i = 0; i != aNb; ++i)
  {
    const BOPDS_InterfEF& aEF = aEFs(i);
    if (aEF.CommonPart().Type() == TopAbs_SHAPE)
    {
      continue;
    }
    aEF.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 5
  NCollection_Vector<BOPDS_InterfFF>& aFFs = myDS->InterfFF();
  aNb                                      = aFFs.Length();
  for (i = 0; i != aNb; ++i)
  {
    bool bTangentFaces, bFlag;
    int  aNbC, aNbP, j, iFound;
    //
    const BOPDS_InterfFF& aFF = aFFs(i);
    aFF.Indices(n1, n2);
    //
    bTangentFaces                              = aFF.TangentFaces();
    aNbP                                       = aFF.Points().Length();
    const NCollection_Vector<BOPDS_Curve>& aVC = aFF.Curves();
    aNbC                                       = aVC.Length();
    if (!aNbP && !aNbC && !bTangentFaces)
    {
      continue;
    }
    //
    iFound = (n1 == n2) ? 1 : 0;
    // case of self-intersection inside one face
    if (!iFound)
    {
      if (bTangentFaces)
      {
        const TopoDS_Face& aF1 = *((TopoDS_Face*)&myDS->Shape(n1));
        const TopoDS_Face& aF2 = *((TopoDS_Face*)&myDS->Shape(n2));
        bFlag = BOPTools_AlgoTools::AreFacesSameDomain(aF1, aF2, myContext, myFuzzyValue);
        if (bFlag)
        {
          ++iFound;
        }
      }
      else
      {
        for (j = 0; j != aNbC; ++j)
        {
          const BOPDS_Curve&                                    aNC   = aVC(j);
          const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPBC = aNC.PaveBlocks();
          if (aLPBC.Extent())
          {
            ++iFound;
            break;
          }
        }
      }
    }
    //
    if (!iFound)
    {
      continue;
    }
    //
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  //
  // 6
  NCollection_Vector<BOPDS_InterfVZ>& aVZs = myDS->InterfVZ();
  aNb                                      = aVZs.Length();
  for (i = 0; i != aNb; ++i)
  {
    //
    const BOPDS_InterfVZ& aVZ = aVZs(i);
    aVZ.Indices(n1, n2);
    if (myDS->IsNewShape(n1) || myDS->IsNewShape(n2))
    {
      continue;
    }
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 7
  NCollection_Vector<BOPDS_InterfEZ>& aEZs = myDS->InterfEZ();
  aNb                                      = aEZs.Length();
  for (i = 0; i != aNb; ++i)
  {
    //
    const BOPDS_InterfEZ& aEZ = aEZs(i);
    aEZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 8
  NCollection_Vector<BOPDS_InterfFZ>& aFZs = myDS->InterfFZ();
  aNb                                      = aFZs.Length();
  for (i = 0; i != aNb; ++i)
  {
    //
    const BOPDS_InterfFZ& aFZ = aFZs(i);
    aFZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
  //
  // 9
  NCollection_Vector<BOPDS_InterfZZ>& aZZs = myDS->InterfZZ();
  aNb                                      = aZZs.Length();
  for (i = 0; i != aNb; ++i)
  {
    //
    const BOPDS_InterfZZ& aZZ = aZZs(i);
    aZZ.Indices(n1, n2);
    aPK.SetIndices(n1, n2);
    aMPK.Add(aPK);
  }
}

//=================================================================================================

void BOPAlgo_CheckerSI::CheckFaceSelfIntersection(const Message_ProgressRange& theRange)
{
  if (myLevelOfCheck < 5)
    return;

  BOPDS_Pair aPK;

  NCollection_Map<BOPDS_Pair>& aMPK = *((NCollection_Map<BOPDS_Pair>*)&myDS->Interferences());
  aMPK.Clear();

  BOPAlgo_VectorOfFaceSelfIntersect aVFace;

  int aNbS = myDS->NbSourceShapes();

  Message_ProgressScope aPSOuter(theRange, NULL, 1);

  //
  for (int i = 0; i < aNbS; i++)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;
    //
    const TopoDS_Face&  aF = (*(TopoDS_Face*)(&aSI.Shape()));
    BRepAdaptor_Surface BAsurf(aF, false);
    GeomAbs_SurfaceType aSurfType = BAsurf.GetType();
    if (aSurfType == GeomAbs_Plane || aSurfType == GeomAbs_Cylinder || aSurfType == GeomAbs_Cone
        || aSurfType == GeomAbs_Sphere)
      continue;

    if (aSurfType == GeomAbs_Torus)
    {
      gp_Torus aTorus       = BAsurf.Torus();
      double   aMajorRadius = aTorus.MajorRadius();
      double   aMinorRadius = aTorus.MinorRadius();
      if (aMajorRadius > aMinorRadius + Precision::Confusion())
        continue;
    }

    double aTolF = BRep_Tool::Tolerance(aF);

    BOPAlgo_FaceSelfIntersect& aFaceSelfIntersect = aVFace.Appended();
    //
    aFaceSelfIntersect.SetRunParallel(myRunParallel);
    aFaceSelfIntersect.SetIndex(i);
    aFaceSelfIntersect.SetFace(aF);
    aFaceSelfIntersect.SetTolF(aTolF);
  }

  int                   aNbFace = aVFace.Length();
  Message_ProgressScope aPSParallel(aPSOuter.Next(),
                                    "Checking surface on self-intersection",
                                    aNbFace);
  for (int iF = 0; iF < aNbFace; ++iF)
  {
    aVFace.ChangeValue(iF).SetProgressRange(aPSParallel.Next());
  }
  //======================================================
  BOPTools_Parallel::Perform(myRunParallel, aVFace);
  //======================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //
  for (int k = 0; k < aNbFace; k++)
  {
    BOPAlgo_FaceSelfIntersect& aFaceSelfIntersect = aVFace(k);
    //
    int nF = aFaceSelfIntersect.IndexOfFace();

    bool bIsDone = aFaceSelfIntersect.IsDone();
    if (bIsDone)
    {
      const NCollection_Sequence<IntTools_Curve>&       aCvsX  = aFaceSelfIntersect.Lines();
      const NCollection_Sequence<IntTools_PntOn2Faces>& aPntsX = aFaceSelfIntersect.Points();
      //
      int aNbCurves = aCvsX.Length();
      int aNbPoints = aPntsX.Length();
      //
      if (aNbCurves || aNbPoints)
      {
        aPK.SetIndices(nF, nF);
        aMPK.Add(aPK);
      }
    }
  }
}
