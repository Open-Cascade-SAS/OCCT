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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
static 
  void CheckEdge (const TopoDS_Edge& E,
                  const Standard_Real aMaxTol);
static 
  void CorrectEdgeTolerance (const TopoDS_Edge& myShape,
                             const TopoDS_Face& S,
                             const Standard_Real aMaxTol);
static 
  Standard_Boolean Validate(const Adaptor3d_Curve& CRef,
                            const Adaptor3d_Curve& Other,
                            const Standard_Real Tol,
                            const Standard_Boolean SameParameter,
                            Standard_Real& aNewTolerance);

static
  void CorrectVertexTolerance(const TopoDS_Edge& aE);

static
  void CorrectWires(const TopoDS_Face& aF);



static
  void UpdateEdges(const TopoDS_Face& aF);

static 
  Standard_Real IntersectCurves2d(const gp_Pnt& aPV,
                                  const TopoDS_Face& aF,
                                  const GeomAdaptor_Surface& aS,
                                  const TopoDS_Edge& aE1,
                                  const TopoDS_Edge& aE2);



//=======================================================================
//class    : BOPTools_CPC
//purpose  : 
//=======================================================================
class BOPTools_CPC {
 public:
  BOPTools_CPC()
    : myMaxTol(1.e-7) {
  }
  //
  ~BOPTools_CPC() {
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myEdge=aE;
  }
  //
  const TopoDS_Edge& Edge()const {
    return myEdge;
  }
  //
  void SetMaxTol(const Standard_Real aMaxTol) {
    myMaxTol=aMaxTol;
  }
  //
  Standard_Real MaxTol()const {
    return myMaxTol;
  }
  //
  void Perform() {
    CheckEdge(myEdge, myMaxTol);
  }
  
 protected:
  Standard_Real myMaxTol;
  TopoDS_Edge myEdge;
};
//
//=======================================================================
typedef BOPCol_NCVector<BOPTools_CPC> BOPTools_VectorOfCPC; 
//
typedef BOPCol_Functor 
  <BOPTools_CPC,
  BOPTools_VectorOfCPC> BOPTools_CPCFunctor;
//
typedef BOPCol_Cnt 
  <BOPTools_CPCFunctor,
  BOPTools_VectorOfCPC> BOPTools_CPCCnt;
//
//=======================================================================
//class    : BOPTools_CWT
//purpose  : 
//=======================================================================
class BOPTools_CWT {
 public:
  BOPTools_CWT() {
  }
  //
  ~BOPTools_CWT() {
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myFace=aF;
  }
  //
  void Perform() {
    CorrectWires(myFace);
  }
  //
 protected:
  TopoDS_Face myFace;
};
//=======================================================================
typedef BOPCol_NCVector<BOPTools_CWT> BOPTools_VectorOfCWT; 
//
typedef BOPCol_Functor 
  <BOPTools_CWT,
  BOPTools_VectorOfCWT> BOPTools_CWTFunctor;
//
typedef BOPCol_Cnt 
  <BOPTools_CWTFunctor,
  BOPTools_VectorOfCWT> BOPTools_CWTCnt;
//
//=======================================================================
//class    : BOPTools_CDT
//purpose  : 
//=======================================================================
class BOPTools_CDT {
 public:
  BOPTools_CDT() 
    : myMaxTol(1.e-7) {
  }
  //
  ~BOPTools_CDT() {
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myEdge=aE;
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myFace=aF;
  }
  //
  void SetMaxTol(const Standard_Real aMaxTol) {
    myMaxTol=aMaxTol;
  }
  //
  void Perform() {
    CorrectEdgeTolerance (myEdge, myFace, myMaxTol);
  }
  //
 protected:
  Standard_Real myMaxTol;
  TopoDS_Edge myEdge;
  TopoDS_Face myFace;
};
//=======================================================================
typedef BOPCol_NCVector<BOPTools_CDT> BOPTools_VectorOfCDT; 
//
typedef BOPCol_Functor 
  <BOPTools_CDT,
  BOPTools_VectorOfCDT> BOPTools_CDTFunctor;
//
typedef BOPCol_Cnt 
  <BOPTools_CDTFunctor,
  BOPTools_VectorOfCDT> BOPTools_CDTCnt;
//
//=======================================================================
//class    : BOPTools_CVT
//purpose  : 
//=======================================================================
class BOPTools_CVT {
 public:
  BOPTools_CVT() {
  }
  //
  ~BOPTools_CVT() {
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myEdge=aE;
  }
  //
  void Perform() {
    CorrectVertexTolerance(myEdge);
  }
  //
 protected:
  TopoDS_Edge myEdge;
};
//
//=======================================================================
typedef BOPCol_NCVector<BOPTools_CVT> BOPTools_VectorOfCVT; 
//
typedef BOPCol_Functor 
  <BOPTools_CVT,
  BOPTools_VectorOfCVT> BOPTools_CVTFunctor;
//
typedef BOPCol_Cnt 
  <BOPTools_CVTFunctor,
  BOPTools_VectorOfCVT> BOPTools_CVTCnt;
//
//=======================================================================
//class    : BOPTools_CET
//purpose  : 
//=======================================================================
class BOPTools_CET {
 public:
  BOPTools_CET() {
  }
  //
  ~BOPTools_CET() {
  }
  //
  void SetFace(const TopoDS_Face& aF) {
    myFace=aF;
  }
  //
  void Perform() {
    UpdateEdges(myFace);
  }
  //
 protected:
  TopoDS_Face myFace;
};
//=======================================================================
typedef BOPCol_NCVector<BOPTools_CET> BOPTools_VectorOfCET; 
//
typedef BOPCol_Functor 
  <BOPTools_CET,
  BOPTools_VectorOfCET> BOPTools_CETFunctor;
//
typedef BOPCol_Cnt 
  <BOPTools_CETFunctor,
  BOPTools_VectorOfCET> BOPTools_CETCnt;
//
//
//=======================================================================
  //
//=======================================================================
// Function : CorrectTolerances
// purpose : 
//=======================================================================
void BOPTools_AlgoTools::CorrectTolerances
  (const TopoDS_Shape& aShape,
   const Standard_Real aMaxTol,
   const Standard_Boolean bRunParallel)
{
  BOPTools_AlgoTools::CorrectPointOnCurve(aShape, aMaxTol, bRunParallel);
  BOPTools_AlgoTools::CorrectCurveOnSurface(aShape, aMaxTol, bRunParallel);
}
//
//=======================================================================
// Function : CorrectPointOnCurve
// purpose : 
//=======================================================================
void BOPTools_AlgoTools::CorrectPointOnCurve
  (const TopoDS_Shape& aS,
   const Standard_Real aMaxTol,
   const Standard_Boolean bRunParallel)
{
  TopExp_Explorer aExp;
  BOPTools_VectorOfCPC aVCPC;
  //
  aExp.Init(aS, TopAbs_EDGE);
  for(; aExp.More();  aExp.Next()) {
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aExp.Current());
    BOPTools_CPC& aCPC=aVCPC.Append1();
    aCPC.SetEdge(aE);
    aCPC.SetMaxTol(aMaxTol);
  }
  //
  //======================================================
  BOPTools_CPCCnt::Perform(bRunParallel, aVCPC);
  //======================================================
}
//=======================================================================
// Function : CorrectCurveOnSurface
// purpose : 
//=======================================================================
void BOPTools_AlgoTools::CorrectCurveOnSurface
  (const TopoDS_Shape& aS,
   const Standard_Real aMaxTol,
   const Standard_Boolean bRunParallel)
{
  TopExp_Explorer aExpF, aExpE;
  BOPTools_VectorOfCWT aVCWT;
  BOPTools_VectorOfCDT aVCDT;
  //
  aExpF.Init(aS, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next()) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aExpF.Current());
    //
    BOPTools_CWT& aCWT=aVCWT.Append1();
    aCWT.SetFace(aF);
    //
    aExpE.Init(aF, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next()) {
      const TopoDS_Edge& aE=*((TopoDS_Edge*)&aExpE.Current());
      //
      BOPTools_CDT& aCDT=aVCDT.Append1();
      aCDT.SetEdge(aE);
      aCDT.SetFace(aF);
      aCDT.SetMaxTol(aMaxTol);
    }
  }
  //
  //======================================================
  BOPTools_CWTCnt::Perform(bRunParallel, aVCWT);
  //======================================================
  BOPTools_CDTCnt::Perform(bRunParallel, aVCDT);
  //======================================================
}
//=======================================================================
// Function : CorrectShapeTolerances
// purpose : 
//=======================================================================
void BOPTools_AlgoTools::CorrectShapeTolerances
  (const TopoDS_Shape& aShape,
   const Standard_Boolean bRunParallel)
{ 
  TopExp_Explorer aExp;
  BOPTools_VectorOfCVT aVCVT;
  BOPTools_VectorOfCET aVCET;
  //
  aExp.Init(aShape, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
    BOPTools_CVT& aCVT=aVCVT.Append1();
    aCVT.SetEdge(aE);
  }
  //
  //======================================================
  BOPTools_CVTCnt::Perform(bRunParallel, aVCVT);
  //======================================================
  //
  aExp.Init(aShape, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aExp.Current();
    BOPTools_CET& aCET=aVCET.Append1();
    aCET.SetFace(aF);
  }
  //
  //======================================================
  BOPTools_CETCnt::Perform(bRunParallel, aVCET);
  //======================================================
}
//
//=======================================================================
// Function : CheckEdge
// purpose :  Correct tolerances for Vertices on Edge 
//=======================================================================
void CheckEdge (const TopoDS_Edge& Ed, 
                const Standard_Real aMaxTol)
{
  Standard_Real aTolE, aTol, aD2, aNewTolerance, dd;
  gp_Pnt aPC;
  TopLoc_Location L;
  TopoDS_Edge aE;
  TopoDS_Vertex aV;
  TopoDS_Iterator aItS;
  //
  TopAbs_Orientation aOrV;
  BRep_ListIteratorOfListOfPointRepresentation aItPR;
  BRep_ListIteratorOfListOfCurveRepresentation aItCR;
  //
  aE=Ed;
  aE.Orientation(TopAbs_FORWARD);
  aTolE=BRep_Tool::Tolerance(aE);
  //
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&aE.TShape());
  //
  aItS.Initialize(aE);
  for (; aItS.More(); aItS.Next()) {
    aV= TopoDS::Vertex(aItS.Value());
    //
    Handle(BRep_TVertex)& TV=*((Handle(BRep_TVertex)*)&aV.TShape());
    const gp_Pnt& aPV = TV->Pnt();
    //
    aTol=BRep_Tool::Tolerance(aV);
    aTol=Max(aTol, aTolE);
    dd=0.1*aTol;
    aTol*=aTol;
    //
    const TopLoc_Location& Eloc = aE.Location();
    //
    aItCR.Initialize(TE->Curves());
    while (aItCR.More()) {
      const Handle(BRep_CurveRepresentation)& aCR = aItCR.Value();
      const TopLoc_Location& loc = aCR->Location();
      L = (Eloc * loc).Predivided(aV.Location());
      //
      if (aCR->IsCurve3D()) {
        const Handle(Geom_Curve)& aC = aCR->Curve3D();
        if (!aC.IsNull()) {
          aItPR.Initialize(TV->Points());
          while (aItPR.More()) {
            const Handle(BRep_PointRepresentation)& aPR=aItPR.Value();
            if (aPR->IsPointOnCurve(aC, L)) {
              aPC = aC->Value(aPR->Parameter());
              aPC.Transform(L.Transformation());
              aD2=aPV.SquareDistance(aPC);
              if (aD2 > aTol) {
                aNewTolerance=sqrt(aD2)+dd;
                if (aNewTolerance<aMaxTol)
                  TV->UpdateTolerance(aNewTolerance);
              }
            }
            aItPR.Next();
          }
          //
          aOrV=aV.Orientation();
          if (aOrV==TopAbs_FORWARD || aOrV==TopAbs_REVERSED) {
            Handle(BRep_GCurve) aGC (Handle(BRep_GCurve)::DownCast (aCR));
            
            if (aOrV==TopAbs_FORWARD) {
              aPC=aC->Value(aGC->First());
            }
            else {
              aPC=aC->Value(aGC->Last());
            }
            aPC.Transform(L.Transformation());
            //
            aD2=aPV.SquareDistance(aPC);
            if (aD2 > aTol) {
              aNewTolerance=sqrt(aD2)+dd;
              if (aNewTolerance<aMaxTol) 
                TV->UpdateTolerance(aNewTolerance);
            }
          }
        }
      }
      aItCR.Next();
    }//  while (itcr.More()) {  
  } // for (; aVExp.More(); aVExp.Next()) {
}
//=======================================================================
// Function : CorrectWires
// purpose : 
//=======================================================================
void CorrectWires(const TopoDS_Face& aFx)
{
  Standard_Boolean bIsPeriodic; 
  Standard_Integer i, aNbV;
  Standard_Real aTol, aTol2, aD2, aD2max, aT1, aT2, aT;
  gp_Pnt aP, aPV;
  gp_Pnt2d aP2D;
  TopoDS_Face aF;
  TopoDS_Vertex aV11, aV12, aV21, aV22;;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  //
  aF=aFx;
  aF.Orientation(TopAbs_FORWARD);
  const Handle(Geom_Surface)& aS=BRep_Tool::Surface(aFx);
  GeomAdaptor_Surface aGAS (aS);
  //
  bIsPeriodic=(aGAS.IsUPeriodic() || aGAS.IsVPeriodic()); 
  //
  TopExp::MapShapesAndAncestors(aF, 
                                TopAbs_VERTEX, 
                                TopAbs_EDGE, 
                                aMVE);
  aNbV=aMVE.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&aMVE.FindKey(i));
    aPV=BRep_Tool::Pnt(aV);
    aTol=BRep_Tool::Tolerance(aV);
    aTol2=aTol*aTol;
    //
    aD2max=-1.;
    const TopTools_ListOfShape& aLE=aMVE.FindFromIndex(i);
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Edge& aE=*(TopoDS_Edge*)(&aIt.Value());
      const Handle(Geom2d_Curve)& aC2D=
        BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
      aT=BRep_Tool::Parameter(aV, aE);
      //
      aC2D->D0(aT, aP2D);
      aGAS.D0(aP2D.X(), aP2D.Y(), aP);
      aD2=aPV.SquareDistance(aP);
      if (aD2>aD2max) {
        aD2max=aD2;
      }
      //check self interference
      if (aNbV==2) {
        continue;
      }
      //
      if (bIsPeriodic) {
        continue;
      }
      //
      TopExp::Vertices(aE, aV11, aV12);
      //
      aIt1 = aIt;
      aIt1.Next();
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Edge& aE1=*(TopoDS_Edge*)(&aIt1.Value());
        //
        //do not perform check for edges that have two common vertices
        TopExp::Vertices(aE1, aV21, aV22);
        if ((aV11.IsSame(aV21) && aV12.IsSame(aV22)) ||
            (aV12.IsSame(aV21) && aV11.IsSame(aV22))) {
          continue;
        }
        //
        aD2=IntersectCurves2d(aPV, aF, aGAS, aE, aE1);
        if (aD2>aD2max) {
          aD2max=aD2;
        }
      }// for (; aIt1.More(); aIt1.Next()) {
    }// for (; aIt.More(); aIt.Next()) {
    if (aD2max>aTol2) {
      BRep_Builder aBB;
      //
      aTol=sqrt(aD2max);
      aBB.UpdateVertex(aV, aTol);
    }
  }// for (i=1; i<=aNbV; ++i) {
}
//=======================================================================
// Function : IntersectCurves2d
// purpose  : Intersect 2d curves of edges
//=======================================================================
Standard_Real IntersectCurves2d(const gp_Pnt& aPV,
                                const TopoDS_Face& aF,
                                const GeomAdaptor_Surface& aGAS,
                                const TopoDS_Edge& aE1,
                                const TopoDS_Edge& aE2)
{
  Standard_Real aDist, aD, aT11, aT12, aT21, aT22, aTol2d, aT1, aT2;
  Standard_Integer j, aNbPnt;
  Geom2dInt_GInter aInter;
  gp_Pnt aP;
  gp_Pnt2d aP2D;
  //
  aDist = 0.;
  aTol2d = 1.e-10;//Precision::Confusion();
  //
  const Handle(Geom2d_Curve)& aC2D1=
    BRep_Tool::CurveOnSurface(aE1, aF, aT11, aT12);
  const Handle(Geom2d_Curve)& aC2D2=
    BRep_Tool::CurveOnSurface(aE2, aF, aT21, aT22);
  //
  Geom2dAdaptor_Curve aGAC1(aC2D1), aGAC2(aC2D2);
  IntRes2d_Domain aDom1(aC2D1->Value(aT11), aT11, aTol2d, 
                        aC2D1->Value(aT12), aT12, aTol2d);
  IntRes2d_Domain aDom2(aC2D2->Value(aT21), aT21, aTol2d, 
                        aC2D2->Value(aT22), aT22, aTol2d);
  //
  aInter.Perform(aGAC1, aDom1, aGAC2, aDom2, aTol2d, aTol2d);
  if (aInter.IsDone()) {
    if (aInter.NbSegments()) {
      return aDist;
    }
    aNbPnt = aInter.NbPoints();
    if (aNbPnt) {
      aDist = -Precision::Infinite();
      for (j = 1; j <= aNbPnt; ++j) {
        const IntRes2d_IntersectionPoint& aPoint = aInter.Point(j);
        //
        aT1 = aPoint.ParamOnFirst();
        aT2 = aPoint.ParamOnSecond();
        //
        if ((aT1 < aT11 || aT1 > aT12) ||
            (aT2 < aT21 || aT2 > aT22)) {
          continue;
        }          
        //
        aP2D = aPoint.Value();
        aGAS.D0(aP2D.X(), aP2D.Y(), aP);
        aD=aPV.SquareDistance(aP);
        if (aD > aDist) {
          aDist = 1.01 * aD;
        }
      }
    }
  }
  return aDist;
}
//=======================================================================
// Function : CorrectEdgeTolerance
// purpose :  Correct tolerances for Edge 
//=======================================================================
void CorrectEdgeTolerance (const TopoDS_Edge& myShape, 
                           const TopoDS_Face& S,
                           const Standard_Real aMaxTol)
{
  // 
  // 1. Minimum of conditions to Perform
  Handle (BRep_CurveRepresentation) myCref;
  Handle (Adaptor3d_HCurve) myHCurve;

  myCref.Nullify();

  Handle(BRep_TEdge)& TEx = *((Handle(BRep_TEdge)*)&myShape.TShape());
  BRep_ListIteratorOfListOfCurveRepresentation itcrx(TEx->Curves());
  Standard_Boolean Degenerated, SameParameterx, SameRangex;

  Standard_Integer unique = 0;

  Degenerated    = TEx->Degenerated();
  SameParameterx = TEx->SameParameter();
  SameRangex     = TEx->SameRange();
  
  if (!SameRangex && SameParameterx) {
    return;
  }

  Handle(Geom_Curve) C3d;
  while (itcrx.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcrx.Value();
    if (cr->IsCurve3D()) {
      unique++;
      if (myCref.IsNull() && !cr->Curve3D().IsNull()) {
        myCref = cr;
      }
    }
    itcrx.Next();
  }
  
  if (unique==0) {
    return;//...No3DCurve
  }
  if (unique>1) {
    return;//...Multiple3DCurve;
  }

  if (myCref.IsNull() && !Degenerated) {
    itcrx.Initialize(TEx->Curves());
    while (itcrx.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcrx.Value();
      if (cr->IsCurveOnSurface()) {
        myCref = cr;
        break;
      }
      itcrx.Next();
    }
  }
  
  else if (!myCref.IsNull() && Degenerated){
    return ;//...InvalidDegeneratedFlag;
  }
  
  if (!myCref.IsNull()) {
    Handle(BRep_GCurve) GCref (Handle(BRep_GCurve)::DownCast (myCref));
    Standard_Real First,Last;
    GCref->Range(First,Last);
    if (Last<=First) {
      myCref.Nullify();
      return ;//InvalidRange;
    }
    
    else {
      if (myCref->IsCurve3D()) {
        Handle(Geom_Curve) C3dx = Handle(Geom_Curve)::DownCast
          (myCref->Curve3D()->Transformed 
           (myCref->Location().Transformation()));
        GeomAdaptor_Curve GAC3d(C3dx, First, Last);
        myHCurve = new GeomAdaptor_HCurve(GAC3d);
      }
      else { // curve on surface
        Handle(Geom_Surface) Sref = myCref->Surface();
        Sref = Handle(Geom_Surface)::
          DownCast(Sref->Transformed(myCref->Location().Transformation()));
        const  Handle(Geom2d_Curve)& PCref = myCref->PCurve();
        Handle(GeomAdaptor_HSurface) GAHSref = 
          new GeomAdaptor_HSurface(Sref);
        Handle(Geom2dAdaptor_HCurve) GHPCref = 
          new Geom2dAdaptor_HCurve(PCref, First, Last);
        Adaptor3d_CurveOnSurface ACSref(GHPCref,GAHSref);
        myHCurve = new Adaptor3d_HCurveOnSurface(ACSref);
      }
    }
  }

  //=============================================== 
  // 2. Tolerances in InContext
  {
    if (myCref.IsNull()) 
      return;
    Standard_Boolean ok=Standard_True;;

    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&myShape.TShape());
    Standard_Real Tol = BRep_Tool::Tolerance(TopoDS::Edge(myShape));
    Standard_Real aNewTol=Tol;

    Standard_Boolean SameParameter = TE->SameParameter();
    Standard_Boolean SameRange = TE->SameRange();
    Standard_Real First = myHCurve->FirstParameter();
    Standard_Real Last  = myHCurve->LastParameter();
    Standard_Real Delta =1.e-12;

    Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &S.TShape());
    const TopLoc_Location& Floc = S.Location();
    const TopLoc_Location& TFloc = TF->Location();
    const Handle(Geom_Surface)& Su = TF->Surface();
    TopLoc_Location L = (Floc * TFloc).Predivided(myShape.Location());
    Standard_Boolean pcurvefound = Standard_False;

    BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
    while (itcr.More()) {
      const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
      if (cr != myCref && cr->IsCurveOnSurface(Su,L)) {
        pcurvefound = Standard_True;
        Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
        Standard_Real f,l;
        GC->Range(f,l);
        if (SameRange && (f != First || l != Last)) {
          return ;//BRepCheck_InvalidSameRangeFlag;
        }
 
        Handle(Geom_Surface) Sb = cr->Surface();
        Sb = Handle(Geom_Surface)::
          DownCast (Su->Transformed(L.Transformation()));
        Handle(Geom2d_Curve) PC = cr->PCurve();
        Handle(GeomAdaptor_HSurface) GAHS = 
          new GeomAdaptor_HSurface(Sb);
        Handle(Geom2dAdaptor_HCurve) GHPC = 
          new Geom2dAdaptor_HCurve(PC,f,l);
        Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);
        ok = Validate(myHCurve->Curve(), ACS, 
                      Tol, SameParameter, aNewTol);
        if (ok) {
          if (aNewTol<aMaxTol) {
            TE->UpdateTolerance(aNewTol+Delta); 
            //
            CorrectVertexTolerance(myShape);
          }
        }
        
        if (cr->IsCurveOnClosedSurface()) {
          //checkclosed = Standard_True;
          GHPC->ChangeCurve2d().Load(cr->PCurve2(),f,l); // same bounds
          ACS.Load(GHPC, GAHS); // sans doute inutile
          ok = Validate(myHCurve->Curve(),ACS,Tol,SameParameter, aNewTol);
          if (ok) {
            if (aNewTol<aMaxTol) {
              TE->UpdateTolerance(aNewTol+Delta);
              CorrectVertexTolerance(myShape);
            } 
          }
        }
      }
      itcr.Next();
    }
    
    if (!pcurvefound) {
      Handle(Geom_Plane) P;
      Handle(Standard_Type) styp = Su->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        P = Handle(Geom_Plane)::
          DownCast(Handle(Geom_RectangularTrimmedSurface)::
                   DownCast(Su)->BasisSurface());
      }
      else {
        P = Handle(Geom_Plane)::DownCast(Su);
      }
      if (P.IsNull()) { // not a plane
        return;
      }
      
      else {// on fait la projection a la volee, comme BRep_Tool
        P = Handle(Geom_Plane)::
          DownCast(P->Transformed(L.Transformation()));
        //on projette Cref sur ce plan
        Handle(GeomAdaptor_HSurface) GAHS = new GeomAdaptor_HSurface(P);
        
        // Dub - Normalement myHCurve est une GeomAdaptor_HCurve
        GeomAdaptor_Curve& Gac = 
          Handle(GeomAdaptor_HCurve)::DownCast(myHCurve)->ChangeCurve();
        Handle(Geom_Curve) C3dx = Gac.Curve();
        Handle(Geom_Curve) ProjOnPlane = 
          GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3dx,First,Last), 
                                       P, 
                                       P->Position().Direction(), 
                                       Standard_True);
        
        Handle(GeomAdaptor_HCurve) aHCurve = 
          new GeomAdaptor_HCurve(ProjOnPlane);
        
        ProjLib_ProjectedCurve proj(GAHS,aHCurve);
        Handle(Geom2d_Curve) PC = Geom2dAdaptor::MakeCurve(proj);
        Handle(Geom2dAdaptor_HCurve) GHPC = 
          new Geom2dAdaptor_HCurve(PC, 
                                   myHCurve->FirstParameter(), 
                                   myHCurve->LastParameter());
        
        Adaptor3d_CurveOnSurface ACS(GHPC,GAHS);
        
        Standard_Boolean okx = Validate(myHCurve->Curve(),ACS,
                                        Tol,Standard_True, aNewTol); 
        if (okx) {
          if (aNewTol<aMaxTol) {
            TE->UpdateTolerance(aNewTol+Delta);
            CorrectVertexTolerance(myShape);
          }
        }
      }
    }//end of if (!pcurvefound) {
  } // end of  2. Tolerances in InContext
}
//=======================================================================
//function : CorrectVertexTolerance
//purpose  : 
//=======================================================================
void CorrectVertexTolerance(const TopoDS_Edge& aE)
{
  Standard_Real aTolE, aTolV;
  TopoDS_Iterator aIt;
  //
  aTolE=BRep_Tool::Tolerance(aE);
  aIt.Initialize(aE);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Vertex& aV=*((TopoDS_Vertex*)&aIt.Value());
    aTolV=BRep_Tool::Tolerance(aV);
    if (aTolV<aTolE) {
      Handle(BRep_TVertex)& aTV= *((Handle(BRep_TVertex)*)&aV.TShape());
      aTV->UpdateTolerance(aTolE);
    }
  }
}
//=======================================================================
//function : Validate
//purpose  : 
//=======================================================================
Standard_Boolean Validate(const Adaptor3d_Curve& CRef,
                          const Adaptor3d_Curve& Other,
                          const Standard_Real Tol,
                          const Standard_Boolean SameParameter,
                          Standard_Real& aNewTolerance)
{
  Standard_Real First, Last, MaxDistance, aD, Tol2;

  First = CRef.FirstParameter();
  Last  = CRef.LastParameter();
  MaxDistance = 0.;
  Tol2 = Tol*Tol;
  //
  Standard_Integer NCONTROL=23;
  Standard_Integer i, aNC1=NCONTROL-1;

  Standard_Boolean aFlag=Standard_False;
  Standard_Boolean proj = (!SameParameter || 
                           First != Other.FirstParameter() ||
                           Last  != Other.LastParameter());
  //
  // 1. 
  if (!proj) {
    for (i = 0; i < NCONTROL; i++) {
      Standard_Real prm = ((aNC1-i)*First + i*Last)/aNC1;
      gp_Pnt pref   = CRef.Value(prm);
      gp_Pnt pother = Other.Value(prm);
      
      aD=pref.SquareDistance(pother);

      if (aD > Tol2) {
        if (aD>MaxDistance) {
          MaxDistance=aD;
        }
        aFlag=Standard_True;
      }
    }

    if (aFlag) {
      aNewTolerance=sqrt(MaxDistance);
    }
    return aFlag;
  }
  
  //
  // 2.
  else {
    Extrema_LocateExtPC refd,otherd;
    Standard_Real OFirst, OLast;
    OFirst = Other.FirstParameter();
    OLast  = Other.LastParameter();
    
    gp_Pnt pd  = CRef.Value(First);
    gp_Pnt pdo = Other.Value(OFirst);
    
    aD = pd.SquareDistance(pdo);
    if (aD > Tol2) {
      if (aD>MaxDistance) {
        MaxDistance=aD;
      }
      aFlag=Standard_True;
    }

    pd  = CRef.Value(Last);
    pdo = Other.Value(OLast);
    aD = pd.SquareDistance(pdo);
    if (aD > Tol2 && aD > MaxDistance) {
      MaxDistance=aD;
      aFlag=Standard_True;
    }

    refd.Initialize(CRef, First, Last, CRef.Resolution(Tol));
    otherd.Initialize(Other, OFirst, OLast, Other.Resolution(Tol));
    
    for (i = 2; i< aNC1; i++) {
      Standard_Real rprm = ((aNC1-i)*First + i*Last)/aNC1;
      gp_Pnt pref = CRef.Value(rprm);

      Standard_Real oprm = ((aNC1-i)*OFirst + i*OLast)/aNC1;
      gp_Pnt pother = Other.Value(oprm);

      refd.Perform(pother,rprm);
      if (!refd.IsDone() || refd.SquareDistance() > Tol2) {
        if (refd.IsDone()) {
          aD=refd.SquareDistance();
          if (aD > Tol2 && aD>MaxDistance) {
            aFlag=Standard_True;
            MaxDistance=aD;
          }
        }
      }

      otherd.Perform(pref,oprm);
      if (!otherd.IsDone() || otherd.SquareDistance() > Tol2) {
        
        if (otherd.IsDone()) {
          aD=otherd.SquareDistance();
          if (aD > Tol2 && aD>MaxDistance) {
            aFlag=Standard_True;
            MaxDistance=aD;
          }
        }
      }
    }
  }
  
  aD=sqrt (MaxDistance);
  aNewTolerance=aD;
  return aFlag;
}
//=======================================================================
// Function : UpdateEdges
// purpose : 
//=======================================================================
void UpdateEdges(const TopoDS_Face& aF)
{
  Standard_Real aTolF, aTolE, aTolV;
  TopoDS_Iterator aItF, aItW, aItE;
  BRep_Builder aBB;
  //
  aTolE=aTolF= BRep_Tool::Tolerance(aF);
  aItF.Initialize(aF);
  for (; aItF.More(); aItF.Next()) {
    const TopoDS_Shape& aS = aItF.Value();
    if (aS.ShapeType()==TopAbs_WIRE) {
      aItW.Initialize(aS);
      for (; aItW.More(); aItW.Next()) {
        const TopoDS_Edge& aE=*((TopoDS_Edge*)&aItW.Value());
        aTolE = BRep_Tool::Tolerance(aE);
        if (aTolE < aTolF) {
          aBB.UpdateEdge(aE, aTolF);
          aTolE = aTolF;
        }
        //UpdateVertices(aE);
      }
    }
    else {
      const TopoDS_Vertex& aV=*(TopoDS_Vertex*)&aItF.Value();
      aTolV = BRep_Tool::Tolerance(aV);
      if (aTolV < aTolE) {
        aBB.UpdateVertex(aV, aTolF);
      }
    }
  }
}
//=======================================================================
// Function : ComputeTolerance
// purpose : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools::ComputeTolerance
  (const TopoDS_Face& theFace,
   const TopoDS_Edge& theEdge,
   Standard_Real& theMaxDist,
   Standard_Real& theMaxPar)
{
  BRepLib_CheckCurveOnSurface aCS;
  //
  aCS.Init(theEdge, theFace);
  aCS.Perform();
  if (!aCS.IsDone()) {
    return Standard_False;
  }
  //
  theMaxDist = aCS.MaxDistance();
  theMaxPar  = aCS.MaxParameter();
    //
  return Standard_True;
}
