// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_FClass2d.hxx>
#include <IntTools_SurfaceRangeLocalizeData.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_State.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IntTools_Context, Standard_Transient)

//
//=================================================================================================

IntTools_Context::IntTools_Context()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myFClass2dMap(100, myAllocator),
      myProjPSMap(100, myAllocator),
      myProjPCMap(100, myAllocator),
      mySClassMap(100, myAllocator),
      myProjPTMap(100, myAllocator),
      myHatcherMap(100, myAllocator),
      myProjSDataMap(100, myAllocator),
      myBndBoxDataMap(100, myAllocator),
      mySurfAdaptorMap(100, myAllocator),
      myOBBMap(100, myAllocator),
      myCreateFlag(0),
      myPOnSTolerance(1.e-12)
{
}

//=================================================================================================

IntTools_Context::IntTools_Context(const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : myAllocator(theAllocator),
      myFClass2dMap(100, myAllocator),
      myProjPSMap(100, myAllocator),
      myProjPCMap(100, myAllocator),
      mySClassMap(100, myAllocator),
      myProjPTMap(100, myAllocator),
      myHatcherMap(100, myAllocator),
      myProjSDataMap(100, myAllocator),
      myBndBoxDataMap(100, myAllocator),
      mySurfAdaptorMap(100, myAllocator),
      myOBBMap(100, myAllocator),
      myCreateFlag(1),
      myPOnSTolerance(1.e-12)
{
}

//=================================================================================================

IntTools_Context::~IntTools_Context()
{
  for (NCollection_DataMap<TopoDS_Shape, IntTools_FClass2d*, TopTools_ShapeMapHasher>::Iterator
         anIt(myFClass2dMap);
       anIt.More();
       anIt.Next())
  {
    IntTools_FClass2d* pFClass2d = anIt.Value();
    ;
    (*pFClass2d).~IntTools_FClass2d();
    myAllocator->Free(pFClass2d);
  }
  myFClass2dMap.Clear();

  clearCachedPOnSProjectors();
  for (NCollection_DataMap<TopoDS_Shape, GeomAPI_ProjectPointOnCurve*, TopTools_ShapeMapHasher>::
         Iterator anIt(myProjPCMap);
       anIt.More();
       anIt.Next())
  {
    GeomAPI_ProjectPointOnCurve* pProjPC = anIt.Value();
    (*pProjPC).~GeomAPI_ProjectPointOnCurve();
    myAllocator->Free(pProjPC);
  }
  myProjPCMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape, BRepClass3d_SolidClassifier*, TopTools_ShapeMapHasher>::
         Iterator anIt(mySClassMap);
       anIt.More();
       anIt.Next())
  {
    BRepClass3d_SolidClassifier* pSC = anIt.Value();
    (*pSC).~BRepClass3d_SolidClassifier();
    myAllocator->Free(pSC);
  }
  mySClassMap.Clear();

  for (NCollection_DataMap<occ::handle<Geom_Curve>, GeomAPI_ProjectPointOnCurve*>::Iterator anIt(
         myProjPTMap);
       anIt.More();
       anIt.Next())
  {
    GeomAPI_ProjectPointOnCurve* pProjPT = anIt.Value();
    (*pProjPT).~GeomAPI_ProjectPointOnCurve();
    myAllocator->Free(pProjPT);
  }
  myProjPTMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape, Geom2dHatch_Hatcher*, TopTools_ShapeMapHasher>::Iterator
         anIt(myHatcherMap);
       anIt.More();
       anIt.Next())
  {
    Geom2dHatch_Hatcher* pHatcher = anIt.Value();
    (*pHatcher).~Geom2dHatch_Hatcher();
    myAllocator->Free(pHatcher);
  }
  myHatcherMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape,
                           IntTools_SurfaceRangeLocalizeData*,
                           TopTools_ShapeMapHasher>::Iterator anIt(myProjSDataMap);
       anIt.More();
       anIt.Next())
  {
    IntTools_SurfaceRangeLocalizeData* pSData = anIt.Value();
    (*pSData).~IntTools_SurfaceRangeLocalizeData();
    myAllocator->Free(pSData);
  }
  myProjSDataMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape, Bnd_Box*, TopTools_ShapeMapHasher>::Iterator anIt(
         myBndBoxDataMap);
       anIt.More();
       anIt.Next())
  {
    Bnd_Box* pBox = anIt.Value();
    (*pBox).~Bnd_Box();
    myAllocator->Free(pBox);
  }
  myBndBoxDataMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape, BRepAdaptor_Surface*, TopTools_ShapeMapHasher>::Iterator
         anIt(mySurfAdaptorMap);
       anIt.More();
       anIt.Next())
  {
    BRepAdaptor_Surface* pSurfAdaptor = anIt.Value();
    (*pSurfAdaptor).~BRepAdaptor_Surface();
    myAllocator->Free(pSurfAdaptor);
  }
  mySurfAdaptorMap.Clear();

  for (NCollection_DataMap<TopoDS_Shape, Bnd_OBB*, TopTools_ShapeMapHasher>::Iterator anIt(
         myOBBMap);
       anIt.More();
       anIt.Next())
  {
    Bnd_OBB* pOBB = anIt.Value();
    (*pOBB).~Bnd_OBB();
    myAllocator->Free(pOBB);
  }
  myOBBMap.Clear();
}

//=================================================================================================

Bnd_Box& IntTools_Context::BndBox(const TopoDS_Shape& aS)
{
  Bnd_Box* pBox = nullptr;
  if (!myBndBoxDataMap.Find(aS, pBox))
  {
    //
    pBox = (Bnd_Box*)myAllocator->Allocate(sizeof(Bnd_Box));
    new (pBox) Bnd_Box();
    //
    Bnd_Box& aBox = *pBox;
    BRepBndLib::Add(aS, aBox);
    //
    myBndBoxDataMap.Bind(aS, pBox);
  }
  return *pBox;
}

//=================================================================================================

bool IntTools_Context::IsInfiniteFace(const TopoDS_Face& aFace)
{
  const Bnd_Box& aBox = BndBox(aFace);
  return aBox.IsOpenXmax() || aBox.IsOpenXmin() || aBox.IsOpenYmax() || aBox.IsOpenYmin()
         || aBox.IsOpenZmax() || aBox.IsOpenZmin();
}

//=================================================================================================

IntTools_FClass2d& IntTools_Context::FClass2d(const TopoDS_Face& aF)
{
  IntTools_FClass2d* pFClass2d = nullptr;
  if (!myFClass2dMap.Find(aF, pFClass2d))
  {
    double      aTolF;
    TopoDS_Face aFF;
    //
    aFF = aF;
    aFF.Orientation(TopAbs_FORWARD);
    aTolF = BRep_Tool::Tolerance(aFF);
    //
    pFClass2d = (IntTools_FClass2d*)myAllocator->Allocate(sizeof(IntTools_FClass2d));
    new (pFClass2d) IntTools_FClass2d(aFF, aTolF);
    //
    myFClass2dMap.Bind(aFF, pFClass2d);
  }
  return *pFClass2d;
}

//=================================================================================================

GeomAPI_ProjectPointOnSurf& IntTools_Context::ProjPS(const TopoDS_Face& aF)
{
  GeomAPI_ProjectPointOnSurf* pProjPS = nullptr;
  if (!myProjPSMap.Find(aF, pProjPS))
  {
    double Umin, Usup, Vmin, Vsup;
    UVBounds(aF, Umin, Usup, Vmin, Vsup);
    const occ::handle<Geom_Surface>& aS = BRep_Tool::Surface(aF);
    //
    pProjPS =
      (GeomAPI_ProjectPointOnSurf*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnSurf));
    new (pProjPS) GeomAPI_ProjectPointOnSurf();
    pProjPS->Init(aS, Umin, Usup, Vmin, Vsup, myPOnSTolerance);
    pProjPS->SetExtremaFlag(Extrema_ExtFlag_MIN); ///
    //
    myProjPSMap.Bind(aF, pProjPS);
  }
  return *pProjPS;
}

//=================================================================================================

GeomAPI_ProjectPointOnCurve& IntTools_Context::ProjPC(const TopoDS_Edge& aE)
{
  GeomAPI_ProjectPointOnCurve* pProjPC = nullptr;
  if (!myProjPCMap.Find(aE, pProjPC))
  {
    double f, l;
    //
    occ::handle<Geom_Curve> aC3D = BRep_Tool::Curve(aE, f, l);
    //
    pProjPC =
      (GeomAPI_ProjectPointOnCurve*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnCurve));
    new (pProjPC) GeomAPI_ProjectPointOnCurve();
    pProjPC->Init(aC3D, f, l);
    //
    myProjPCMap.Bind(aE, pProjPC);
  }
  return *pProjPC;
}

//=================================================================================================

GeomAPI_ProjectPointOnCurve& IntTools_Context::ProjPT(const occ::handle<Geom_Curve>& aC3D)

{
  GeomAPI_ProjectPointOnCurve* pProjPT = nullptr;
  if (!myProjPTMap.Find(aC3D, pProjPT))
  {
    double f, l;
    f = aC3D->FirstParameter();
    l = aC3D->LastParameter();
    //
    pProjPT =
      (GeomAPI_ProjectPointOnCurve*)myAllocator->Allocate(sizeof(GeomAPI_ProjectPointOnCurve));
    new (pProjPT) GeomAPI_ProjectPointOnCurve();
    pProjPT->Init(aC3D, f, l);
    //
    myProjPTMap.Bind(aC3D, pProjPT);
  }
  return *pProjPT;
}

//=================================================================================================

BRepClass3d_SolidClassifier& IntTools_Context::SolidClassifier(const TopoDS_Solid& aSolid)
{
  BRepClass3d_SolidClassifier* pSC = nullptr;
  if (!mySClassMap.Find(aSolid, pSC))
  {
    pSC = (BRepClass3d_SolidClassifier*)myAllocator->Allocate(sizeof(BRepClass3d_SolidClassifier));
    new (pSC) BRepClass3d_SolidClassifier(aSolid);
    //
    mySClassMap.Bind(aSolid, pSC);
  }
  return *pSC;
}

//=================================================================================================

BRepAdaptor_Surface& IntTools_Context::SurfaceAdaptor(const TopoDS_Face& theFace)
{
  BRepAdaptor_Surface* pBAS = nullptr;
  if (!mySurfAdaptorMap.Find(theFace, pBAS))
  {
    //
    pBAS = (BRepAdaptor_Surface*)myAllocator->Allocate(sizeof(BRepAdaptor_Surface));
    new (pBAS) BRepAdaptor_Surface(theFace, true);
    //
    mySurfAdaptorMap.Bind(theFace, pBAS);
  }
  return *pBAS;
}

//=================================================================================================

Geom2dHatch_Hatcher& IntTools_Context::Hatcher(const TopoDS_Face& aF)
{
  Geom2dHatch_Hatcher* pHatcher = nullptr;
  if (!myHatcherMap.Find(aF, pHatcher))
  {
    double                           aTolArcIntr, aTolTangfIntr, aTolHatch2D, aTolHatch3D;
    double                           aU1, aU2, aEpsT;
    TopAbs_Orientation               aOrE;
    occ::handle<Geom_Surface>        aS;
    occ::handle<Geom2d_Curve>        aC2D;
    occ::handle<Geom2d_TrimmedCurve> aCT2D;
    TopoDS_Face                      aFF;
    TopExp_Explorer                  aExp;
    //
    aTolHatch2D   = 1.e-8;
    aTolHatch3D   = 1.e-8;
    aTolArcIntr   = 1.e-10;
    aTolTangfIntr = 1.e-10;
    aEpsT         = Precision::PConfusion();
    //
    Geom2dHatch_Intersector aIntr(aTolArcIntr, aTolTangfIntr);
    pHatcher = (Geom2dHatch_Hatcher*)myAllocator->Allocate(sizeof(Geom2dHatch_Hatcher));
    new (pHatcher) Geom2dHatch_Hatcher(aIntr, aTolHatch2D, aTolHatch3D, true, false);
    //
    aFF = aF;
    aFF.Orientation(TopAbs_FORWARD);
    aS = BRep_Tool::Surface(aFF);

    aExp.Init(aFF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& aE = *((TopoDS_Edge*)&aExp.Current());
      aOrE                  = aE.Orientation();
      //
      aC2D = BRep_Tool::CurveOnSurface(aE, aFF, aU1, aU2);
      if (aC2D.IsNull())
      {
        continue;
      }
      if (fabs(aU1 - aU2) < aEpsT)
      {
        continue;
      }
      //
      aCT2D = new Geom2d_TrimmedCurve(aC2D, aU1, aU2);
      Geom2dAdaptor_Curve aGAC(aCT2D);
      pHatcher->AddElement(aGAC, aOrE);
    } // for (; aExp.More() ; aExp.Next()) {
    //
    myHatcherMap.Bind(aFF, pHatcher);
  }
  return *pHatcher;
}

//=================================================================================================

Bnd_OBB& IntTools_Context::OBB(const TopoDS_Shape& aS, const double theGap)
{
  Bnd_OBB* pBox = nullptr;
  if (!myOBBMap.Find(aS, pBox))
  {
    pBox = (Bnd_OBB*)myAllocator->Allocate(sizeof(Bnd_OBB));
    new (pBox) Bnd_OBB();
    //
    Bnd_OBB& aBox = *pBox;
    BRepBndLib::AddOBB(aS, aBox);
    aBox.Enlarge(theGap);
    //
    myOBBMap.Bind(aS, pBox);
  }
  return *pBox;
}

//=================================================================================================

IntTools_SurfaceRangeLocalizeData& IntTools_Context::SurfaceData(const TopoDS_Face& aF)
{
  IntTools_SurfaceRangeLocalizeData* pSData = nullptr;
  if (!myProjSDataMap.Find(aF, pSData))
  {
    pSData = (IntTools_SurfaceRangeLocalizeData*)myAllocator->Allocate(
      sizeof(IntTools_SurfaceRangeLocalizeData));
    new (pSData) IntTools_SurfaceRangeLocalizeData(3,
                                                   3,
                                                   10. * Precision::PConfusion(),
                                                   10. * Precision::PConfusion());
    //
    myProjSDataMap.Bind(aF, pSData);
  }
  return *pSData;
}

//=================================================================================================

int IntTools_Context::ComputePE(const gp_Pnt&      aP1,
                                const double       aTolP1,
                                const TopoDS_Edge& aE2,
                                double&            aT,
                                double&            aDist)
{
  if (!BRep_Tool::IsGeometric(aE2))
  {
    return -2;
  }
  double aTolE2, aTolSum;
  int    aNbProj;
  //
  GeomAPI_ProjectPointOnCurve& aProjector = ProjPC(aE2);
  aProjector.Perform(aP1);

  aNbProj = aProjector.NbPoints();
  if (aNbProj)
  {
    // point falls on the curve
    aDist = aProjector.LowerDistance();
    //
    aTolE2  = BRep_Tool::Tolerance(aE2);
    aTolSum = aTolP1 + aTolE2 + Precision::Confusion();
    //
    aT = aProjector.LowerDistanceParameter();
    if (aDist > aTolSum)
    {
      return -4;
    }
  }
  else
  {
    // point falls out of the curve, check distance to vertices
    TopoDS_Edge     aEFwd = TopoDS::Edge(aE2.Oriented(TopAbs_FORWARD));
    TopoDS_Iterator itV(aEFwd);
    aDist = RealLast();
    for (; itV.More(); itV.Next())
    {
      const TopoDS_Vertex& aV = TopoDS::Vertex(itV.Value());
      if (aV.Orientation() == TopAbs_FORWARD || aV.Orientation() == TopAbs_REVERSED)
      {
        gp_Pnt aPV    = BRep_Tool::Pnt(aV);
        aTolSum       = aTolP1 + BRep_Tool::Tolerance(aV) + Precision::Confusion();
        double aDist1 = aP1.Distance(aPV);
        if (aDist1 < aDist && aDist1 < aTolSum)
        {
          aDist = aDist1;
          aT    = BRep_Tool::Parameter(aV, aEFwd);
        }
      }
    }
    if (Precision::IsInfinite(aDist))
    {
      return -3;
    }
  }
  return 0;
}

//=================================================================================================

int IntTools_Context::ComputeVE(const TopoDS_Vertex& theV,
                                const TopoDS_Edge&   theE,
                                double&              theT,
                                double&              theTol,
                                const double         theFuzz)
{
  if (BRep_Tool::Degenerated(theE))
  {
    return -1;
  }
  if (!BRep_Tool::IsGeometric(theE))
  {
    return -2;
  }
  double aDist, aTolV, aTolE, aTolSum;
  int    aNbProj;
  gp_Pnt aP;
  //
  aP = BRep_Tool::Pnt(theV);
  //
  GeomAPI_ProjectPointOnCurve& aProjector = ProjPC(theE);
  aProjector.Perform(aP);

  aNbProj = aProjector.NbPoints();
  if (!aNbProj)
  {
    return -3;
  }
  //
  aDist = aProjector.LowerDistance();
  //
  aTolV   = BRep_Tool::Tolerance(theV);
  aTolE   = BRep_Tool::Tolerance(theE);
  aTolSum = aTolV + aTolE + std::max(theFuzz, Precision::Confusion());
  //
  theTol = aDist + aTolE;
  theT   = aProjector.LowerDistanceParameter();
  if (aDist > aTolSum)
  {
    return -4;
  }
  return 0;
}

//=================================================================================================

int IntTools_Context::ComputeVF(const TopoDS_Vertex& theVertex,
                                const TopoDS_Face&   theFace,
                                double&              theU,
                                double&              theV,
                                double&              theTol,
                                const double         theFuzz)
{
  double aTolV, aTolF, aTolSum, aDist;
  gp_Pnt aP;

  aP = BRep_Tool::Pnt(theVertex);
  //
  // 1. Check if the point is projectable on the surface
  GeomAPI_ProjectPointOnSurf& aProjector = ProjPS(theFace);
  aProjector.Perform(aP);
  //
  if (!aProjector.IsDone())
  { // the point is not  projectable on the surface
    return -1;
  }
  //
  // 2. Check the distance between the projection point and
  //    the original point
  aDist = aProjector.LowerDistance();
  //
  aTolV = BRep_Tool::Tolerance(theVertex);
  aTolF = BRep_Tool::Tolerance(theFace);
  //
  aTolSum = aTolV + aTolF + std::max(theFuzz, Precision::Confusion());
  theTol  = aDist + aTolF;
  aProjector.LowerDistanceParameters(theU, theV);
  //
  if (aDist > aTolSum)
  {
    // the distance is too large
    return -2;
  }
  //
  gp_Pnt2d aP2d(theU, theV);
  bool     pri = IsPointInFace(theFace, aP2d);
  if (!pri)
  { //  the point lays on the surface but out of the face
    return -3;
  }
  return 0;
}

//=================================================================================================

TopAbs_State IntTools_Context::StatePointFace(const TopoDS_Face& aF, const gp_Pnt2d& aP2d)
{
  TopAbs_State       aState;
  IntTools_FClass2d& aClass2d = FClass2d(aF);
  aState                      = aClass2d.Perform(aP2d);
  return aState;
}

//=================================================================================================

bool IntTools_Context::IsPointInFace(const TopoDS_Face& aF, const gp_Pnt2d& aP2d)
{
  TopAbs_State aState = StatePointFace(aF, aP2d);
  if (aState == TopAbs_OUT || aState == TopAbs_ON)
  {
    return false;
  }
  return true;
}

//=================================================================================================

bool IntTools_Context::IsPointInFace(const gp_Pnt& aP, const TopoDS_Face& aF, const double aTol)
{
  bool   bIn = false;
  double aDist;
  //
  GeomAPI_ProjectPointOnSurf& aProjector = ProjPS(aF);
  aProjector.Perform(aP);
  //
  bool bDone = aProjector.IsDone();
  if (bDone)
  {
    aDist = aProjector.LowerDistance();
    if (aDist < aTol)
    {
      double U, V;
      //
      aProjector.LowerDistanceParameters(U, V);
      gp_Pnt2d aP2D(U, V);
      bIn = IsPointInFace(aF, aP2D);
    }
  }
  //
  return bIn;
}

//=================================================================================================

bool IntTools_Context::IsPointInOnFace(const TopoDS_Face& aF, const gp_Pnt2d& aP2d)
{
  TopAbs_State aState = StatePointFace(aF, aP2d);
  if (aState == TopAbs_OUT)
  {
    return false;
  }
  return true;
}

//=================================================================================================

bool IntTools_Context::IsValidPointForFace(const gp_Pnt&      aP,
                                           const TopoDS_Face& aF,
                                           const double       aTol)
{
  bool   bFlag;
  double Umin, U, V;

  GeomAPI_ProjectPointOnSurf& aProjector = ProjPS(aF);
  aProjector.Perform(aP);

  bFlag = aProjector.IsDone();
  if (bFlag)
  {

    Umin = aProjector.LowerDistance();
    // if (Umin > 1.e-3) { // it was
    if (Umin > aTol)
    {
      return !bFlag;
    }
    //
    aProjector.LowerDistanceParameters(U, V);
    gp_Pnt2d aP2D(U, V);
    bFlag = IsPointInOnFace(aF, aP2D);
  }
  return bFlag;
}

//=================================================================================================

bool IntTools_Context::IsValidPointForFaces(const gp_Pnt&      aP,
                                            const TopoDS_Face& aF1,
                                            const TopoDS_Face& aF2,
                                            const double       aTol)
{
  bool bFlag1, bFlag2;

  bFlag1 = IsValidPointForFace(aP, aF1, aTol);
  if (!bFlag1)
  {
    return bFlag1;
  }
  bFlag2 = IsValidPointForFace(aP, aF2, aTol);
  return bFlag2;
}

//=================================================================================================

bool IntTools_Context::IsValidBlockForFace(const double          aT1,
                                           const double          aT2,
                                           const IntTools_Curve& aC,
                                           const TopoDS_Face&    aF,
                                           const double          aTol)
{
  bool   bFlag;
  double aTInterm;
  gp_Pnt aPInterm;

  aTInterm = IntTools_Tools::IntermediatePoint(aT1, aT2);

  const occ::handle<Geom_Curve>& aC3D = aC.Curve();
  // point 3D
  aC3D->D0(aTInterm, aPInterm);
  //
  bFlag = IsValidPointForFace(aPInterm, aF, aTol);
  return bFlag;
}

//=================================================================================================

bool IntTools_Context::IsValidBlockForFaces(const double          theT1,
                                            const double          theT2,
                                            const IntTools_Curve& theC,
                                            const TopoDS_Face&    theF1,
                                            const TopoDS_Face&    theF2,
                                            const double          theTol)
{
  const int                        aNbElem = 2;
  const occ::handle<Geom2d_Curve>& aPC1    = theC.FirstCurve2d();
  const occ::handle<Geom2d_Curve>& aPC2    = theC.SecondCurve2d();
  const occ::handle<Geom_Curve>&   aC3D    = theC.Curve();

  const occ::handle<Geom2d_Curve>* anArrPC[aNbElem] = {&aPC1, &aPC2};
  const TopoDS_Face*               anArrF[aNbElem]  = {&theF1, &theF2};

  const double aMidPar = IntTools_Tools::IntermediatePoint(theT1, theT2);
  const gp_Pnt aP(aC3D->Value(aMidPar));

  bool     bFlag = true;
  gp_Pnt2d aPnt2D;

  for (int i = 0; (i < 2) && bFlag; ++i)
  {
    const occ::handle<Geom2d_Curve>& aPC = *anArrPC[i];
    const TopoDS_Face&               aF  = *anArrF[i];

    if (!aPC.IsNull())
    {
      aPC->D0(aMidPar, aPnt2D);
      bFlag = IsPointInOnFace(aF, aPnt2D);
    }
    else
    {
      bFlag = IsValidPointForFace(aP, aF, theTol);
    }
  }

  return bFlag;
}

//=================================================================================================

bool IntTools_Context::IsVertexOnLine(const TopoDS_Vertex&  aV,
                                      const IntTools_Curve& aC,
                                      const double          aTolC,
                                      double&               aT)
{
  bool   bRet;
  double aTolV;
  //
  aTolV = BRep_Tool::Tolerance(aV);
  bRet  = IntTools_Context::IsVertexOnLine(aV, aTolV, aC, aTolC, aT);
  //
  return bRet;
}

//=================================================================================================

bool IntTools_Context::IsVertexOnLine(const TopoDS_Vertex&  aV,
                                      const double          aTolV,
                                      const IntTools_Curve& aC,
                                      const double          aTolC,
                                      double&               aT)
{
  double aFirst, aLast, aDist, aTolSum;
  int    aNbProj;
  gp_Pnt aPv;

  aPv = BRep_Tool::Pnt(aV);

  const occ::handle<Geom_Curve>& aC3D = aC.Curve();

  aTolSum = aTolV + aTolC;
  //
  GeomAdaptor_Curve aGAC(aC3D);
  GeomAbs_CurveType aType = aGAC.GetType();
  if (aType == GeomAbs_BSplineCurve || aType == GeomAbs_BezierCurve)
  {
    aTolSum = 2. * aTolSum;
    if (aTolSum < 1.e-5)
    {
      aTolSum = 1.e-5;
    }
  }
  else
  {
    aTolSum = 2. * aTolSum; // xft
    if (aTolSum < 1.e-6)
      aTolSum = 1.e-6;
  }
  //
  aFirst = aC3D->FirstParameter();
  aLast  = aC3D->LastParameter();
  //
  // Checking extremities first
  // It is necessary to chose the closest bound to the point
  bool   bFirstValid = false;
  double aFirstDist  = Precision::Infinite();
  //
  if (!Precision::IsInfinite(aFirst))
  {
    gp_Pnt aPCFirst = aC3D->Value(aFirst);
    aFirstDist      = aPv.Distance(aPCFirst);
    if (aFirstDist < aTolSum)
    {
      bFirstValid = true;
      aT          = aFirst;
      //
      if (aFirstDist > aTolV)
      {
        Extrema_LocateExtPC anExt(aPv, aGAC, aFirst, 1.e-10);

        if (anExt.IsDone())
        {
          Extrema_POnCurv aPOncurve = anExt.Point();
          aT                        = aPOncurve.Parameter();

          if ((aT > (aLast + aFirst) * 0.5) || (aPv.Distance(aPOncurve.Value()) > aTolSum)
              || (aPCFirst.Distance(aPOncurve.Value()) < Precision::Confusion()))
            aT = aFirst;
        }
        else
        {
          // Local search may fail. Try to use more precise algo.
          Extrema_ExtPC anExt2(aPv, aGAC, 1.e-10);
          double        aMinDist = RealLast();
          int           aMinIdx  = -1;
          if (anExt2.IsDone())
          {
            for (int anIdx = 1; anIdx <= anExt2.NbExt(); anIdx++)
            {
              if (anExt2.IsMin(anIdx) && anExt2.SquareDistance(anIdx) < aMinDist)
              {
                aMinDist = anExt2.SquareDistance(anIdx);
                aMinIdx  = anIdx;
              }
            }
          }
          if (aMinIdx != -1)
          {
            const Extrema_POnCurv& aPOncurve = anExt2.Point(aMinIdx);
            aT                               = aPOncurve.Parameter();

            if ((aT > (aLast + aFirst) * 0.5) || (aPv.Distance(aPOncurve.Value()) > aTolSum)
                || (aPCFirst.Distance(aPOncurve.Value()) < Precision::Confusion()))
              aT = aFirst;
          }
        }
      }
    }
  }
  //
  if (!Precision::IsInfinite(aLast))
  {
    gp_Pnt aPCLast = aC3D->Value(aLast);
    aDist          = aPv.Distance(aPCLast);
    if (bFirstValid && (aFirstDist < aDist))
    {
      return true;
    }
    //
    if (aDist < aTolSum)
    {
      aT = aLast;
      //
      if (aDist > aTolV)
      {
        Extrema_LocateExtPC anExt(aPv, aGAC, aLast, 1.e-10);

        if (anExt.IsDone())
        {
          Extrema_POnCurv aPOncurve = anExt.Point();
          aT                        = aPOncurve.Parameter();

          if ((aT < (aLast + aFirst) * 0.5) || (aPv.Distance(aPOncurve.Value()) > aTolSum)
              || (aPCLast.Distance(aPOncurve.Value()) < Precision::Confusion()))
            aT = aLast;
        }
        else
        {
          // Local search may fail. Try to use more precise algo.
          Extrema_ExtPC anExt2(aPv, aGAC, 1.e-10);
          double        aMinDist = RealLast();
          int           aMinIdx  = -1;
          if (anExt2.IsDone())
          {
            for (int anIdx = 1; anIdx <= anExt2.NbExt(); anIdx++)
            {
              if (anExt2.IsMin(anIdx) && anExt2.SquareDistance(anIdx) < aMinDist)
              {
                aMinDist = anExt2.SquareDistance(anIdx);
                aMinIdx  = anIdx;
              }
            }
          }
          if (aMinIdx != -1)
          {
            const Extrema_POnCurv& aPOncurve = anExt2.Point(aMinIdx);
            aT                               = aPOncurve.Parameter();

            if ((aT < (aLast + aFirst) * 0.5) || (aPv.Distance(aPOncurve.Value()) > aTolSum)
                || (aPCLast.Distance(aPOncurve.Value()) < Precision::Confusion()))
              aT = aLast;
          }
        }
      }
      //
      return true;
    }
  }
  else if (bFirstValid)
  {
    return true;
  }
  //
  GeomAPI_ProjectPointOnCurve& aProjector = ProjPT(aC3D);
  aProjector.Perform(aPv);

  aNbProj = aProjector.NbPoints();
  if (!aNbProj)
  {
    occ::handle<Geom_BoundedCurve> aBC = occ::down_cast<Geom_BoundedCurve>(aC3D);
    if (!aBC.IsNull())
    {
      gp_Pnt aPStart = aBC->StartPoint();
      gp_Pnt aPEnd   = aBC->EndPoint();

      aDist = aPv.Distance(aPStart);
      if (aDist < aTolSum)
      {
        aT = aFirst;
        return true;
      }

      aDist = aPv.Distance(aPEnd);
      if (aDist < aTolSum)
      {
        aT = aLast;
        return true;
      }
    }

    return false;
  }

  aDist = aProjector.LowerDistance();

  if (aDist > aTolSum)
  {
    return false;
  }

  aT = aProjector.LowerDistanceParameter();

  return true;
}

//=================================================================================================

bool IntTools_Context::ProjectPointOnEdge(const gp_Pnt& aP, const TopoDS_Edge& anEdge, double& aT)
{
  int aNbPoints;

  GeomAPI_ProjectPointOnCurve& aProjector = ProjPC(anEdge);
  aProjector.Perform(aP);

  aNbPoints = aProjector.NbPoints();
  if (aNbPoints)
  {
    aT = aProjector.LowerDistanceParameter();
    return true;
  }
  return false;
}

//=================================================================================================

void IntTools_Context::SetPOnSProjectionTolerance(const double theValue)
{
  myPOnSTolerance = theValue;
  clearCachedPOnSProjectors();
}

//=================================================================================================

void IntTools_Context::clearCachedPOnSProjectors()
{
  for (NCollection_DataMap<TopoDS_Shape, GeomAPI_ProjectPointOnSurf*, TopTools_ShapeMapHasher>::
         Iterator aIt(myProjPSMap);
       aIt.More();
       aIt.Next())
  {
    GeomAPI_ProjectPointOnSurf* pProjPS = aIt.Value();
    (*pProjPS).~GeomAPI_ProjectPointOnSurf();
    myAllocator->Free(pProjPS);
  }
  myProjPSMap.Clear();
}

//=================================================================================================

void IntTools_Context::UVBounds(const TopoDS_Face& theFace,
                                double&            UMin,
                                double&            UMax,
                                double&            VMin,
                                double&            VMax)
{
  const BRepAdaptor_Surface& aBAS = SurfaceAdaptor(theFace);
  UMin                            = aBAS.FirstUParameter();
  UMax                            = aBAS.LastUParameter();
  VMin                            = aBAS.FirstVParameter();
  VMax                            = aBAS.LastVParameter();
}
