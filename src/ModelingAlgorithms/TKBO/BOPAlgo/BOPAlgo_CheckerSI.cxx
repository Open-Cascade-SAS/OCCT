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
#include <BOPAlgo_Algo.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Pair.hxx>
#include <NCollection_DynamicArray.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <IntTools_Context.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <IntPatch_Intersection.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <cmath>

//=================================================================================================

namespace
{

//! Returns the boundary vertex at the classifier's edge parameter, or null.
TopoDS_Vertex classifiedVertex(const BRepClass_FaceClassifier& theClassifier);

//! Returns true for equivalent seam points or a shared boundary vertex.
bool isSeamPoint(const TopoDS_Face&        theFace,
                 const IntSurf_PntOn2S&    thePoint,
                 BRepClass_FaceClassifier& theFirstClassifier,
                 BRepClass_FaceClassifier& theSecondClassifier);

//! Checks every walking point of a retained interval for seam equivalence.
bool isSeamLine(const TopoDS_Face&                theFace,
                const occ::handle<IntPatch_Line>& theLine,
                const double                      theFirst,
                const double                      theLast,
                BRepClass_FaceClassifier&         theFirstClassifier,
                BRepClass_FaceClassifier&         theSecondClassifier);

//! Checks surface self-intersections inside both copies of a face domain.
class BOPAlgo_FaceSelfIntersect : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  //! Initializes an unchecked face task.
  BOPAlgo_FaceSelfIntersect();

  //
  ~BOPAlgo_FaceSelfIntersect() override = default;

  //! Sets the face index in the interference data.
  void SetIndex(const int theIndex);

  //! Returns the face index in the interference data.
  int IndexOfFace() const;

  //! Retains the shared face for this task.
  void SetFace(const TopoDS_Face& theFace);

  //! Returns true when the geometric and domain checks completed.
  bool IsDone() const;

  //! Returns true for a self-intersection within the trimmed face.
  bool HasInterference() const;

  //! Checks both surface preimages against the face domain and seams.
  void Perform() override;

  //
protected:
  int         myIF;              //!< Face index in the interference data.
  TopoDS_Face myF;               //!< Shared face retained during the check.
  bool        myIsDone;          //!< True after a completed intersection check.
  bool        myHasInterference; //!< True for an intersection within both domains.
};

//==================================================================================================

TopoDS_Vertex classifiedVertex(const BRepClass_FaceClassifier& theClassifier)
{
  const BRepClass_Edge& anEdge = theClassifier.Edge();
  if (anEdge.Edge().IsNull())
  {
    return TopoDS_Vertex();
  }
  double aFirst, aLast;
  BRep_Tool::Range(anEdge.Edge(), anEdge.Face(), aFirst, aLast);
  const double aParameter = theClassifier.EdgeParameter();
  if (std::abs(aParameter - aFirst) <= Precision::PConfusion())
  {
    return TopExp::FirstVertex(anEdge.Edge());
  }
  if (std::abs(aParameter - aLast) <= Precision::PConfusion())
  {
    return TopExp::LastVertex(anEdge.Edge());
  }
  return TopoDS_Vertex();
}

//==================================================================================================

bool isSeamPoint(const TopoDS_Face&        theFace,
                 const IntSurf_PntOn2S&    thePoint,
                 BRepClass_FaceClassifier& theFirstClassifier,
                 BRepClass_FaceClassifier& theSecondClassifier)
{
  double aU1, aV1, aU2, aV2;
  thePoint.Parameters(aU1, aV1, aU2, aV2);
  theFirstClassifier.Perform(theFace, gp_Pnt2d(aU1, aV1), Precision::PConfusion());
  theSecondClassifier.Perform(theFace, gp_Pnt2d(aU2, aV2), Precision::PConfusion());
  if (theFirstClassifier.State() != TopAbs_ON || theSecondClassifier.State() != TopAbs_ON)
  {
    return false;
  }
  const TopoDS_Edge& aFirstEdge  = theFirstClassifier.Edge().Edge();
  const TopoDS_Edge& aSecondEdge = theSecondClassifier.Edge().Edge();
  if (!aFirstEdge.IsNull() && aFirstEdge.IsSame(aSecondEdge)
      && BRep_Tool::IsClosed(aFirstEdge, theFace) && BRep_Tool::SameParameter(aFirstEdge)
      && std::abs(theFirstClassifier.EdgeParameter() - theSecondClassifier.EdgeParameter())
           <= Precision::PConfusion())
  {
    return true;
  }
  const TopoDS_Vertex aFirstVertex = classifiedVertex(theFirstClassifier);
  return !aFirstVertex.IsNull() && aFirstVertex.IsSame(classifiedVertex(theSecondClassifier));
}

//==================================================================================================

bool isSeamLine(const TopoDS_Face&                theFace,
                const occ::handle<IntPatch_Line>& theLine,
                const double                      theFirst,
                const double                      theLast,
                BRepClass_FaceClassifier&         theFirstClassifier,
                BRepClass_FaceClassifier&         theSecondClassifier)
{
  const occ::handle<IntPatch_WLine> aLine = occ::down_cast<IntPatch_WLine>(theLine);
  if (aLine.IsNull() || theFirst < 1.0 || theLast > aLine->NbPnts() || theFirst > theLast)
  {
    return false;
  }
  // Walking-line parameters are point indices. Preserve other parameter layouts.
  const int aFirst = static_cast<int>(theFirst);
  const int aLast  = static_cast<int>(theLast);
  if (aFirst != theFirst || aLast != theLast)
  {
    return false;
  }
  const int aMiddle = aFirst + (aLast - aFirst) / 2;
  if (!isSeamPoint(theFace, aLine->Point(aMiddle), theFirstClassifier, theSecondClassifier))
  {
    return false;
  }
  // Retain the wrapper before the classifiers are reused for other points.
  const TopoDS_Edge aSeam = theFirstClassifier.Edge().Edge();
  if (aSeam.IsNull() || !aSeam.IsSame(theSecondClassifier.Edge().Edge())
      || !BRep_Tool::IsClosed(aSeam, theFace) || !BRep_Tool::SameParameter(aSeam)
      || std::abs(theFirstClassifier.EdgeParameter() - theSecondClassifier.EdgeParameter())
           > Precision::PConfusion())
  {
    return false;
  }
  const TopoDS_Vertex aFirstVertex = TopExp::FirstVertex(aSeam);
  const TopoDS_Vertex aLastVertex  = TopExp::LastVertex(aSeam);
  for (int i = aFirst; i <= aLast; ++i)
  {
    if (!isSeamPoint(theFace, aLine->Point(i), theFirstClassifier, theSecondClassifier))
    {
      return false;
    }
    if (!aSeam.IsSame(theFirstClassifier.Edge().Edge())
        || !aSeam.IsSame(theSecondClassifier.Edge().Edge()))
    {
      const TopoDS_Vertex aVertex = classifiedVertex(theFirstClassifier);
      if (aVertex.IsNull() || (!aVertex.IsSame(aFirstVertex) && !aVertex.IsSame(aLastVertex)))
      {
        return false;
      }
    }
  }
  return true;
}

//==================================================================================================

BOPAlgo_FaceSelfIntersect::BOPAlgo_FaceSelfIntersect()
    : myIF(-1),
      myIsDone(false),
      myHasInterference(false)
{
}

//==================================================================================================

void BOPAlgo_FaceSelfIntersect::SetIndex(const int theIndex)
{
  myIF = theIndex;
}

//==================================================================================================

int BOPAlgo_FaceSelfIntersect::IndexOfFace() const
{
  return myIF;
}

//==================================================================================================

void BOPAlgo_FaceSelfIntersect::SetFace(const TopoDS_Face& theFace)
{
  myF = theFace;
}

//==================================================================================================

bool BOPAlgo_FaceSelfIntersect::IsDone() const
{
  return myIsDone;
}

//==================================================================================================

bool BOPAlgo_FaceSelfIntersect::HasInterference() const
{
  return myHasInterference;
}

//==================================================================================================

void BOPAlgo_FaceSelfIntersect::Perform()
{
  myIsDone          = false;
  myHasInterference = false;
  Message_ProgressScope aPS(myProgressRange, nullptr, 1);
  if (UserBreak(aPS))
  {
    return;
  }

  const occ::handle<Geom_Surface> aGeometry = BRep_Tool::Surface(myF);
  if (aGeometry.IsNull())
  {
    return;
  }
  double aUFirst, aULast, aVFirst, aVLast;
  BRepTools::UVBounds(myF, aUFirst, aULast, aVFirst, aVLast);
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(aGeometry, aUFirst, aULast, aVFirst, aVLast);
  const occ::handle<BRepTopAdaptor_TopolTool> aFirstDomain =
    new BRepTopAdaptor_TopolTool(new BRepAdaptor_Surface(myF));
  const occ::handle<BRepTopAdaptor_TopolTool> aSecondDomain =
    new BRepTopAdaptor_TopolTool(new BRepAdaptor_Surface(myF));

  // Both preimages must belong to the trimmed face, including its inner wires.
  // No fitted 3D intersection curve is needed to establish an interference.
  const double aTolerance = 2.0 * BRep_Tool::Tolerance(myF) + Precision::Confusion();
  const double aMaxStep =
    IntPatch_Intersection::DefineUVMaxStep(aSurface, aFirstDomain, aSurface, aSecondDomain);
  const double          aDeflection = aSurface->GetType() == GeomAbs_BSplineSurface ? 0.01 : 0.1;
  IntPatch_Intersection anIntersection;
  anIntersection.SetTolerances(aTolerance, aTolerance, aMaxStep, aDeflection);
  anIntersection.Perform(aSurface, aFirstDomain, aTolerance, aTolerance);
  if (!anIntersection.IsDone() || UserBreak(aPS))
  {
    return;
  }
  BRepClass_FaceClassifier aFirstClassifier;
  BRepClass_FaceClassifier aSecondClassifier;
  for (int i = 1; i <= anIntersection.NbPnts(); ++i)
  {
    double aU1, aV1, aU2, aV2;
    anIntersection.Point(i).PntOn2S().Parameters(aU1, aV1, aU2, aV2);
    const TopAbs_State aFirstState =
      aFirstDomain->Classify(gp_Pnt2d(aU1, aV1), Precision::PConfusion());
    const TopAbs_State aSecondState =
      aSecondDomain->Classify(gp_Pnt2d(aU2, aV2), Precision::PConfusion());
    if (aFirstState == TopAbs_UNKNOWN || aSecondState == TopAbs_UNKNOWN)
    {
      return;
    }
    if (aFirstState != TopAbs_OUT && aSecondState != TopAbs_OUT
        && !isSeamPoint(myF,
                        anIntersection.Point(i).PntOn2S(),
                        aFirstClassifier,
                        aSecondClassifier))
    {
      myHasInterference = true;
      myIsDone          = true;
      return;
    }
  }
  GeomInt_LineConstructor aConstructor;
  aConstructor.Load(aFirstDomain, aSecondDomain, aSurface, aSurface);
  for (int i = 1; i <= anIntersection.NbLines(); ++i)
  {
    aConstructor.Perform(anIntersection.Line(i));
    if (!aConstructor.IsDone())
    {
      return;
    }
    for (int j = 1; j <= aConstructor.NbParts(); ++j)
    {
      double aFirst, aLast;
      aConstructor.Part(j, aFirst, aLast);
      if (!isSeamLine(myF,
                      anIntersection.Line(i),
                      aFirst,
                      aLast,
                      aFirstClassifier,
                      aSecondClassifier))
      {
        myHasInterference = true;
        myIsDone          = true;
        return;
      }
    }
  }
  myIsDone = true;
}

//=======================================================================

typedef NCollection_DynamicArray<BOPAlgo_FaceSelfIntersect> BOPAlgo_VectorOfFaceSelfIntersect;

} // namespace

//=================================================================================================

BOPAlgo_CheckerSI::BOPAlgo_CheckerSI()

{
  myLevelOfCheck   = BOPDS_DS::NbInterfTypes() - 1;
  myNonDestructive = true;
  SetAvoidBuildPCurve(true);
}

//=================================================================================================

BOPAlgo_CheckerSI::~BOPAlgo_CheckerSI() = default;

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
  BOPDS_IteratorSI* theIterSI = new BOPDS_IteratorSI(myAllocator);
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

    Message_ProgressScope aPSZZ(aPS.Next(), nullptr, 4);
    // Perform intersection with solids
    if (!HasErrors())
    {
      PerformVZ(aPSZZ.Next());
    }
    //
    if (!HasErrors())
    {
      PerformEZ(aPSZZ.Next());
    }
    //
    if (!HasErrors())
    {
      PerformFZ(aPSZZ.Next());
    }
    //
    if (!HasErrors())
    {
      PerformZZ(aPSZZ.Next());
    }
    //
    if (HasErrors())
    {
      return;
    }

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
  NCollection_DynamicArray<BOPDS_InterfVV>& aVVs = myDS->InterfVV();
  aNb                                            = aVVs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfVE>& aVEs = myDS->InterfVE();
  aNb                                            = aVEs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfEE>& aEEs = myDS->InterfEE();
  aNb                                            = aEEs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfVF>& aVFs = myDS->InterfVF();
  aNb                                            = aVFs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfEF>& aEFs = myDS->InterfEF();
  aNb                                            = aEFs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfFF>& aFFs = myDS->InterfFF();
  aNb                                            = aFFs.Length();
  for (i = 0; i != aNb; ++i)
  {
    bool bTangentFaces, bFlag;
    int  aNbC, aNbP, j, iFound;
    //
    const BOPDS_InterfFF& aFF = aFFs(i);
    aFF.Indices(n1, n2);
    //
    bTangentFaces                                    = aFF.TangentFaces();
    aNbP                                             = aFF.Points().Length();
    const NCollection_DynamicArray<BOPDS_Curve>& aVC = aFF.Curves();
    aNbC                                             = aVC.Length();
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
  NCollection_DynamicArray<BOPDS_InterfVZ>& aVZs = myDS->InterfVZ();
  aNb                                            = aVZs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfEZ>& aEZs = myDS->InterfEZ();
  aNb                                            = aEZs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfFZ>& aFZs = myDS->InterfFZ();
  aNb                                            = aFZs.Length();
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
  NCollection_DynamicArray<BOPDS_InterfZZ>& aZZs = myDS->InterfZZ();
  aNb                                            = aZZs.Length();
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
  {
    return;
  }

  BOPDS_Pair aPK;

  NCollection_Map<BOPDS_Pair>& aMPK = *((NCollection_Map<BOPDS_Pair>*)&myDS->Interferences());
  aMPK.Clear();

  BOPAlgo_VectorOfFaceSelfIntersect aVFace;

  int aNbS = myDS->NbSourceShapes();

  Message_ProgressScope aPSOuter(theRange, nullptr, 1);

  //
  for (int i = 0; i < aNbS; i++)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    //
    const TopoDS_Face&  aF = (*(TopoDS_Face*)(&aSI.Shape()));
    BRepAdaptor_Surface BAsurf(aF, false);
    GeomAbs_SurfaceType aSurfType = BAsurf.GetType();
    if (aSurfType == GeomAbs_Plane || aSurfType == GeomAbs_Cylinder || aSurfType == GeomAbs_Cone
        || aSurfType == GeomAbs_Sphere)
    {
      continue;
    }

    if (aSurfType == GeomAbs_Torus)
    {
      gp_Torus aTorus       = BAsurf.Torus();
      double   aMajorRadius = aTorus.MajorRadius();
      double   aMinorRadius = aTorus.MinorRadius();
      if (aMajorRadius > aMinorRadius + Precision::Confusion())
      {
        continue;
      }
    }

    BOPAlgo_FaceSelfIntersect& aFaceSelfIntersect = aVFace.Appended();
    //
    aFaceSelfIntersect.SetRunParallel(myRunParallel);
    aFaceSelfIntersect.SetIndex(i);
    aFaceSelfIntersect.SetFace(aF);
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

    if (!aFaceSelfIntersect.IsDone())
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
      return;
    }
    if (aFaceSelfIntersect.HasInterference())
    {
      aPK.SetIndices(nF, nF);
      aMPK.Add(aPK);
    }
  }
}
