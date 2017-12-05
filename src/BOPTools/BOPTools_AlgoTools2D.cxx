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


#include <BOPTools_AlgoTools2D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ProjLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

static
  Standard_Real MaxToleranceEdge (const TopoDS_Face& );

//=======================================================================
//function : BuildPCurveForEdgeOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace (const TopoDS_Edge& aE,
                                                     const TopoDS_Face& aF,
                                                     const Handle(IntTools_Context)& theContext)
{
  BRep_Builder aBB;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real  aTolPC, aTolFact, aTolEdge, aFirst, aLast;
  
  Standard_Boolean aHasOld;
  aHasOld=BOPTools_AlgoTools2D::HasCurveOnSurface (aE, aF, aC2D, 
                                                   aFirst, aLast, 
                                                   aTolEdge);
  if (aHasOld) {
    return;
  }
  

  BOPTools_AlgoTools2D::CurveOnSurface(aE, aF, aC2D, aTolPC, theContext);
  
  aTolEdge=BRep_Tool::Tolerance(aE);

  aTolFact=Max(aTolEdge, aTolPC);

  aBB.UpdateEdge(aE, aC2D, aF, aTolFact);
  return;
}

//=======================================================================
//function : EdgeTangent
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools2D::EdgeTangent
  (const TopoDS_Edge& anEdge, 
   const Standard_Real aT,
   gp_Vec& aTau)
{
  Standard_Boolean isdgE;
  Standard_Real first, last;
  
  isdgE = BRep_Tool::Degenerated(anEdge); 
  if (isdgE) {
    return Standard_False;
  }

  Handle(Geom_Curve) aC=BRep_Tool::Curve(anEdge, first, last);
  gp_Pnt aP;
  aC->D1(aT, aP, aTau);
  Standard_Real mod = aTau.Magnitude();
  if(mod > gp::Resolution()) {
    aTau /= mod;
  }
  else {
    return Standard_False;
  }
  //aTau.Normalize(); 
  if (anEdge.Orientation() == TopAbs_REVERSED){
    aTau.Reverse();
  }
  return Standard_True;
}

//=======================================================================
//function : PointOnOnSurface
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::PointOnSurface (const TopoDS_Edge& aE,
                                           const TopoDS_Face& aF,
                                           const Standard_Real aParameter,
                                           Standard_Real& U,
                                           Standard_Real& V,
                                           const Handle(IntTools_Context)& theContext)
{
  gp_Pnt2d aP2D;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real aToler, aFirst, aLast;

  BOPTools_AlgoTools2D::CurveOnSurface (aE, aF, aC2D, aFirst, aLast, aToler, theContext);
  aC2D->D0(aParameter, aP2D);
  U=aP2D.X();
  V=aP2D.Y();
  return;
}

//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::CurveOnSurface (const TopoDS_Edge& aE,
                                           const TopoDS_Face& aF,
                                           Handle(Geom2d_Curve)& aC2D,
                                           Standard_Real& aToler,
                                           const Handle(IntTools_Context)& theContext)
{
  Standard_Real aFirst, aLast; 
  //
  BOPTools_AlgoTools2D::CurveOnSurface(aE, aF, aC2D, aFirst, aLast, aToler, theContext); 
  //
  return;
}
//=======================================================================
//function : CurveOnSurface
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::CurveOnSurface (const TopoDS_Edge& aE,
                                           const TopoDS_Face& aF,
                                           Handle(Geom2d_Curve)& aC2D,
                                           Standard_Real& aFirst,
                                           Standard_Real& aLast,
                                           Standard_Real& aToler,
                                           const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean aHasOld;
  Handle(Geom2d_Curve) C2D;

  aHasOld=BOPTools_AlgoTools2D::HasCurveOnSurface (aE, aF, C2D, 
                                                   aFirst, aLast, 
                                                   aToler);
  if (aHasOld) {
    aC2D=C2D;
    return;
  }

  BOPTools_AlgoTools2D::Make2D(aE, aF, C2D, aFirst, aLast, aToler, theContext);
  aC2D=C2D;
  return;
}
//=======================================================================
//function : HasCurveOnSurface
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools2D::HasCurveOnSurface 
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF,
   Handle(Geom2d_Curve)& aC2D,
   Standard_Real& aFirst,
   Standard_Real& aLast,
   Standard_Real& aToler)
{
  Standard_Boolean aHasOld;
  
  aToler=BRep_Tool::Tolerance(aE);
  BRep_Tool::Range(aE, aFirst, aLast);

  if((aLast - aFirst) < Precision::PConfusion()) {
    return Standard_False;
  }

  aC2D=BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  aHasOld=!aC2D.IsNull();
  return aHasOld;
}
//=======================================================================
//function : HasCurveOnSurface
//purpose  : 
//=======================================================================
Standard_Boolean BOPTools_AlgoTools2D::HasCurveOnSurface 
  (const TopoDS_Edge& aE,
   const TopoDS_Face& aF)
{
  Standard_Boolean bHasOld;
  Handle(Geom2d_Curve) aC2D;
  Standard_Real aFirst, aLast;
  //
  BRep_Tool::Range(aE, aFirst, aLast);
  //
  if((aLast - aFirst) < Precision::PConfusion()) {
    return Standard_False;
  }
  //
  aC2D=BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  bHasOld=!aC2D.IsNull();
  //
  return bHasOld;
}
//=======================================================================
//function : AdjustPCurveOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::AdjustPCurveOnFace
  (const TopoDS_Face& theF,
   const Handle(Geom_Curve)&   theC3D,
   const Handle(Geom2d_Curve)& theC2D,
   Handle(Geom2d_Curve)& theC2DA,
   const Handle(IntTools_Context)& theContext)
{
  Standard_Real aT1 = theC3D->FirstParameter();
  Standard_Real aT2 = theC3D->LastParameter();
  //
  BOPTools_AlgoTools2D::AdjustPCurveOnFace (theF, aT1, aT2, theC2D, theC2DA, theContext);
}
//=======================================================================
//function : AdjustPCurveOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::AdjustPCurveOnFace 
  (const TopoDS_Face& theF,
   const Standard_Real theFirst,
   const Standard_Real theLast,
   const Handle(Geom2d_Curve)& theC2D, 
   Handle(Geom2d_Curve)& theC2DA,
   const Handle(IntTools_Context)& theContext)
{
  BRepAdaptor_Surface aBASTmp;
  const BRepAdaptor_Surface* pBAS;
  if (!theContext.IsNull()) {
    pBAS = &theContext->SurfaceAdaptor(theF);
  }
  else {
    aBASTmp.Initialize(theF, Standard_True);
    pBAS = &aBASTmp;
  }
  //
  BOPTools_AlgoTools2D::AdjustPCurveOnSurf(*pBAS, theFirst, theLast, theC2D, theC2DA);
}
//=======================================================================
//function : AdjustPCurveOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::AdjustPCurveOnSurf
  (const BRepAdaptor_Surface& aBAS,
   const Standard_Real aFirst,
   const Standard_Real aLast,
   const Handle(Geom2d_Curve)& aC2D,
   Handle(Geom2d_Curve)& aC2DA)
{
  Standard_Boolean mincond, maxcond;
  Standard_Real UMin, UMax, VMin, VMax, aT, u2, v2, du, dv, aDelta;
  Standard_Real aUPeriod;
  //
  const TopoDS_Face& aF=aBAS.Face();
  UMin=aBAS.FirstUParameter();
  UMax=aBAS.LastUParameter();
  VMin=aBAS.FirstVParameter();
  VMax=aBAS.LastVParameter();
  //
  aDelta=Precision::PConfusion(); 
  
  aT =.5*(aFirst+aLast);

  gp_Pnt2d pC2D; 
  aC2D->D0(aT, pC2D);

  u2 = pC2D.X();
  v2 = pC2D.Y();
  //
  // du
  du = 0.;
  if (aBAS.IsUPeriodic()) {
    aUPeriod = aBAS.UPeriod(); 
    
    //
    // a. try to clarify u2 using the precision (aDelta)
    if (fabs(u2-UMin) < aDelta) {
      u2=UMin;
    }
    else if (fabs(u2-UMin-aUPeriod) < aDelta) {
      u2=UMin+aUPeriod;
    }
    // b. compute du again using clarified value of u2
    GeomInt::AdjustPeriodic(u2, UMin, UMax, aUPeriod, u2, du, 0.);
    //
    if (du==0.) {
      if (aBAS.GetType()==GeomAbs_Cylinder) {
        Standard_Real aR, dFi, aTol;
        //
        gp_Cylinder aCylinder=aBAS.Cylinder();
        aR=aCylinder.Radius();
        aTol=MaxToleranceEdge(aF);
        dFi=aTol/aR;
        if (dFi<aDelta) {
          dFi=aDelta;
        }
        //
        mincond = (UMin - u2 > dFi);
        maxcond = (u2 - UMax > dFi);
        if (mincond || maxcond) {
          du = ( mincond ) ? aUPeriod : -aUPeriod;
        }
      }
    } 
  }
  
  // dv
  dv = 0.;
  if (aBAS.IsVPeriodic()) {
    Standard_Real aVPeriod, aVm, aVr, aVmid, dVm, dVr;
    //
    aVPeriod = aBAS.VPeriod();
    mincond = (VMin - v2 > aDelta);
    maxcond = (v2 - VMax > aDelta);
    //
    if (mincond || maxcond) {
      dv = ( mincond ) ? aVPeriod : -aVPeriod;
    }
    //
    if ((VMax-VMin<aVPeriod) && dv) {
      aVm=v2;
      aVr=v2+dv;
      aVmid=0.5*(VMin+VMax);
      dVm=fabs(aVm-aVmid);
      dVr=fabs(aVr-aVmid);
      if (dVm<dVr) {
        dv=0.;
      }
    }
  }
  //
  {
    //check the point with classifier
    Standard_Real u,v;
    u = u2 + du;
    v = v2 + dv;
    if (aBAS.IsUPeriodic()) {
      aUPeriod = aBAS.UPeriod(); 
      if ((UMax - UMin - 2*aDelta) > aUPeriod) {
        if ((u > (UMin + aDelta + aUPeriod)) ||
            (u < (UMax - aDelta - aUPeriod))) {
          BRepClass_FaceClassifier aClassifier;
          aClassifier.Perform(aF, gp_Pnt2d(u, v), aDelta);
          TopAbs_State Status = aClassifier.State();
          if (Status == TopAbs_OUT) {
            du += (u > (UMin + aDelta + aUPeriod)) ? -aUPeriod : aUPeriod;
          }
        }
      }
    }
    //
    u = u2 + du;
    if (aBAS.IsVPeriodic()) {
      Standard_Real aVPeriod = aBAS.VPeriod(); 
      if ((VMax - VMin - 2*aDelta) > aVPeriod) {
        if ((v > (VMin + aDelta + aVPeriod)) ||
            (v < (VMax - aDelta - aVPeriod))) {
          BRepClass_FaceClassifier aClassifier;
          aClassifier.Perform(aF, gp_Pnt2d(u, v), aDelta);
          TopAbs_State Status = aClassifier.State();
          if (Status == TopAbs_OUT) {
            dv += (v > (VMin + aDelta + aVPeriod)) ? -aVPeriod : aVPeriod;
          }
        }
      }
    }
  }
  // Translation if necessary
  Handle(Geom2d_Curve) aC2Dx=aC2D;

  if ( du != 0. || dv != 0.) {
    Handle(Geom2d_Curve) PCT = Handle(Geom2d_Curve)::DownCast(aC2Dx->Copy());
    gp_Vec2d aV2D(du,dv);
    PCT->Translate(aV2D);
    aC2Dx = PCT;
  }

  aC2DA=aC2Dx;
}

//=======================================================================
//function : IntermediatePoint
//purpose  : 
//=======================================================================
Standard_Real BOPTools_AlgoTools2D::IntermediatePoint 
  (const Standard_Real aFirst,
   const Standard_Real aLast)
{
  //define parameter division number as 10*e^(-PI) = 0.43213918
  const Standard_Real PAR_T = 0.43213918;
  Standard_Real aParm;
  aParm=(1.-PAR_T)*aFirst + PAR_T*aLast;
  return aParm;
}
//=======================================================================
//function : IntermediatePoint
//purpose  : 
//=======================================================================
Standard_Real BOPTools_AlgoTools2D::IntermediatePoint 
  (const TopoDS_Edge& aE)
                                                
{
  Standard_Real aT, aT1, aT2;

  Handle(Geom_Curve)aC1=BRep_Tool::Curve(aE, aT1, aT2);
  if (aC1.IsNull())
    BRep_Tool::Range(aE, aT1, aT2);

  aT=BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);
  return aT;
}
//=======================================================================
//function : Make2D
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::Make2D (const TopoDS_Edge& aE,
                                   const TopoDS_Face& aF,
                                   Handle(Geom2d_Curve)& aC2D,
                                   Standard_Real& aFirst,
                                   Standard_Real& aLast,
                                   Standard_Real& aToler,
                                   const Handle(IntTools_Context)& theContext)
{
  Standard_Boolean aLocIdentity;
  Standard_Real f3d, l3d;
  TopLoc_Location aLoc;

  Handle(Geom2d_Curve) C2D; 
  
  
  C2D=BRep_Tool::CurveOnSurface(aE, aF, aFirst, aLast);
  
  if (!C2D.IsNull()) {
    aC2D=C2D;
    return;
  }

  Handle(Geom_Curve) C3D2, C3D;
  C3D = BRep_Tool::Curve(aE, aLoc, f3d, l3d);
  //
  if (C3D.IsNull()) { 
    // aE has no 3D curve, so nothing is done
  }
  //
  aLocIdentity=aLoc.IsIdentity();
    
  if (aLocIdentity) {
    C3D2 = C3D;
  }
  else {
    C3D2 = Handle(Geom_Curve)::
      DownCast(C3D->Transformed(aLoc.Transformation()));
  }
  
  //
  aToler = BRep_Tool::Tolerance(aE);
  BOPTools_AlgoTools2D::MakePCurveOnFace(aF, C3D2, f3d, l3d, aC2D, aToler, theContext);
  //
  aFirst = f3d; 
  aLast  = l3d;
}

//=======================================================================
//function : MakePCurveOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::MakePCurveOnFace (const TopoDS_Face& aF,
                                             const Handle(Geom_Curve)& aC3D,
                                             Handle(Geom2d_Curve)& aC2D, //->
                                             Standard_Real& TolReached2d,
                                             const Handle(IntTools_Context)& theContext)
{
  Standard_Real aFirst, aLast;

  aFirst = aC3D -> FirstParameter();
  aLast  = aC3D -> LastParameter();
  //
  TolReached2d=0.;
  //
  BOPTools_AlgoTools2D::MakePCurveOnFace
    (aF, aC3D, aFirst, aLast, aC2D, TolReached2d, theContext);
}

//=======================================================================
//function : MakePCurveOnFace
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::MakePCurveOnFace
  (const TopoDS_Face& aF,
   const Handle(Geom_Curve)& aC3D,
   const Standard_Real aT1,
   const Standard_Real aT2,
   Handle(Geom2d_Curve)& aC2D,
   Standard_Real& TolReached2d,
   const Handle(IntTools_Context)& theContext)
{
  BRepAdaptor_Surface aBASTmp;
  const BRepAdaptor_Surface* pBAS;
  if (!theContext.IsNull()) {
    pBAS = &theContext->SurfaceAdaptor(aF);
  }
  else {
    aBASTmp.Initialize(aF, Standard_True);
    pBAS = &aBASTmp;
  }
  //
  Handle(BRepAdaptor_HSurface) aBAHS = new BRepAdaptor_HSurface(*pBAS);
  Handle(GeomAdaptor_HCurve) aBAHC = new GeomAdaptor_HCurve(aC3D, aT1, aT2);
  //
  Standard_Real aTolR;
  Standard_Real aTR = Precision::Confusion();//1.e-7;
  Standard_Real aMaxTol = 1.e3 * aTR; //0.0001
  Standard_Boolean isAnaSurf =  ProjLib::IsAnaSurf(aBAHS);

  //when the type of surface is GeomAbs_SurfaceOfRevolution
  if (pBAS->GetType() == GeomAbs_SurfaceOfRevolution) 
  {
    if (TolReached2d > aTR) {
      aTR=TolReached2d;
    }
    //
    ProjLib_ProjectedCurve aProj1(aBAHS, aBAHC, aTR);
    ProjLib::MakePCurveOfType(aProj1, aC2D);
    aTolR = aProj1.GetTolerance();
  } 
  else 
  {
    ProjLib_ProjectedCurve aProjCurv(aBAHS);
    Standard_Integer aDegMin = -1, aDegMax = -1, aMaxSegments = -1;
    Standard_Real aMaxDist = -1;
    AppParCurves_Constraint aBndPnt = AppParCurves_TangencyPoint;
    if ((TolReached2d  >= 10. * aTR) && (TolReached2d <= aMaxTol || isAnaSurf))
    {
      aTR = Min(aMaxTol, 0.1*TolReached2d);
      aMaxSegments = 100;
      aMaxDist = 1.e3*TolReached2d;
      if(!isAnaSurf || TolReached2d > 1.)
      {
        aBndPnt = AppParCurves_PassPoint;
      }
    }
    else if(TolReached2d > aMaxTol)
    {
      aTR = Min(TolReached2d, 1.e3 * aMaxTol);
      aMaxDist = 1.e2 * aTR;
      aMaxSegments = 100;
    }
    aProjCurv.Load(aTR);
    aProjCurv.SetDegree(aDegMin, aDegMax);
    aProjCurv.SetMaxSegments(aMaxSegments);
    aProjCurv.SetBndPnt(aBndPnt);
    aProjCurv.SetMaxDist(aMaxDist);
    aProjCurv.Perform(aBAHC);
    ProjLib::MakePCurveOfType(aProjCurv, aC2D);
    aTolR=aProjCurv.GetTolerance();
  }
  //
  if (aC2D.IsNull() && (aTR < aMaxTol || aTR < TolReached2d))
  { 
    aTR = Max(TolReached2d, aMaxTol);
    ProjLib_ProjectedCurve aProjCurvAgain(aBAHS, aBAHC, aTR);// 2
    ProjLib::MakePCurveOfType(aProjCurvAgain, aC2D);
    aTolR = aProjCurvAgain.GetTolerance();
  }
  //
  if(aC2D.IsNull())
  {
    throw Standard_ConstructionError("BOPTools_AlgoTools2D::MakePCurveOnFace : PCurve is Null");
  }
  //
  TolReached2d=aTolR;

  // Adjust curve for periodic surface
  Handle(Geom2d_Curve) aC2DA;
  BOPTools_AlgoTools2D::AdjustPCurveOnSurf (*pBAS, aT1, aT2, aC2D, aC2DA);
  aC2D = aC2DA;

  // Make sure that the range of the 2D curve is sufficient for representation of the 3D curve.
  Standard_Real aTCFirst = aC2D->FirstParameter();
  Standard_Real aTCLast  = aC2D->LastParameter();
  if ((aTCFirst - aT1) > Precision::PConfusion() ||
      (aT2 - aTCLast ) > Precision::PConfusion())
  {
    if (aTCFirst < aT1) aTCFirst = aT1;
    if (aTCLast  > aT2) aTCLast  = aT2;

    GeomLib::SameRange(Precision::PConfusion(), aC2D,
                       aTCFirst, aTCLast, aT1, aT2, aC2D);
  }

  // compute the appropriate tolerance for the edge
  Handle(Geom_Surface) aS = pBAS->Surface().Surface();
  aS = Handle(Geom_Surface)::DownCast(aS->Transformed(pBAS->Trsf()));
  //
  Standard_Real aT;
  if (IntTools_Tools::ComputeTolerance
      (aC3D, aC2D, aS, aT1, aT2, aTolR, aT)) {
    if (aTolR > TolReached2d) {
      TolReached2d = aTolR;
    }
  }
}

//=======================================================================
//function : MaxToleranceEdge
//purpose  : 
//=======================================================================
Standard_Real MaxToleranceEdge (const TopoDS_Face& aF) 
{
  Standard_Real aTol, aTolMax;
  TopExp_Explorer aExp;
  //
  aTolMax=0.;
  aExp.Init(aF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE=*((TopoDS_Edge *)&aExp.Current());
    aTol=BRep_Tool::Tolerance(aE);
    if (aTol>aTolMax) {
      aTolMax=aTol;
    }
  }
  return aTolMax;
}

//=======================================================================
//function : IsEdgeIsoline
//purpose  : 
//=======================================================================
void BOPTools_AlgoTools2D::IsEdgeIsoline( const TopoDS_Edge& theE,
                                          const TopoDS_Face& theF,
                                          Standard_Boolean& isTheUIso,
                                          Standard_Boolean& isTheVIso)
{
  isTheUIso = isTheVIso = Standard_False;

  gp_Vec2d aT;
  gp_Pnt2d aP;
  Standard_Real aFirst = 0.0, aLast = 0.0;
  const Handle(Geom2d_Curve) aPC = BRep_Tool::CurveOnSurface(theE, theF, aFirst, aLast);

  aPC->D1(0.5*(aFirst+aLast), aP, aT);

  const Standard_Real aSqMagn = aT.SquareMagnitude();
  if(aSqMagn <= gp::Resolution())
    return;

  //Normalyze aT
  aT /= sqrt(aSqMagn);

  //sin(da) ~ da, when da->0.
  const Standard_Real aTol = Precision::Angular();
  const gp_Vec2d aRefVDir(0.0, 1.0), aRefUDir(1.0, 0.0);

  const Standard_Real aDPv = aT.CrossMagnitude(aRefVDir),
                      aDPu = aT.CrossMagnitude(aRefUDir);

  isTheUIso = (aDPv <= aTol);
  isTheVIso = (aDPu <= aTol);
}
