// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <AppCont_Function.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <BSplCLib.hxx>
#include <PLib.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Approx_FitAndDivide2d.hxx>
#include <math.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_ListOfTransient.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_ExtPS.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <Standard_DomainError.hxx>
//#include <GeomLib_IsIso.hxx>
//#include <GeomLib_CheckSameParameter.hxx>

#ifdef OCCT_DEBUG
#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#endif
//static Standard_Integer compteur = 0;
#endif

struct aFuncStruct
{
  Handle(Adaptor3d_HSurface) mySurf; // Surface where to project.
  Handle(Adaptor3d_HCurve)   myCurve; // Curve to project.
  Handle(Adaptor2d_HCurve2d) myInitCurve2d; // Initial 2dcurve projection.
  Standard_Real mySqProjOrtTol; // Used to filter non-orthogonal projected point.
  Standard_Real myTolU;
  Standard_Real myTolV;
  Standard_Real myPeriod[2]; // U and V period correspondingly.
};

//=======================================================================
//function : computePeriodicity
//purpose  : Compute period information on adaptor.
//=======================================================================
static void computePeriodicity(const Handle(Adaptor3d_HSurface)& theSurf,
                               Standard_Real &theUPeriod,
                               Standard_Real &theVPeriod)
{
  theUPeriod = 0.0;
  theVPeriod = 0.0;

  // Compute once information about periodicity.
  // Param space may be reduced in case of rectangular trimmed surface,
  // in this case really trimmed bounds should be set as unperiodic.
  Standard_Real aTrimF, aTrimL, aBaseF, aBaseL, aDummyF, aDummyL;
  Handle(Geom_Surface) aS = GeomAdaptor::MakeSurface(theSurf->Surface(), Standard_False); // Not trim.
  // U param space.
  if (theSurf->IsUPeriodic())
  {
    theUPeriod = theSurf->UPeriod();
  }
  else if(theSurf->IsUClosed())
  {
    theUPeriod = theSurf->LastUParameter() - theSurf->FirstUParameter();
  }
  if (theUPeriod != 0.0)
  {
    aTrimF = theSurf->FirstUParameter(); // Trimmed first
    aTrimL = theSurf->LastUParameter(); // Trimmed last
    aS->Bounds(aBaseF, aBaseL, aDummyF, aDummyL); // Non-trimmed values.
    if (Abs (aBaseF - aTrimF) + Abs (aBaseL - aTrimL) > Precision::PConfusion())
    {
      // Param space reduced.
      theUPeriod = 0.0;
    }
  }

  // V param space.
  if (theSurf->IsVPeriodic())
  {
    theVPeriod = theSurf->VPeriod();
  }
  else if(theSurf->IsVClosed())
  {
    theVPeriod = theSurf->LastVParameter() - theSurf->FirstVParameter();
  }
  if (theVPeriod != 0.0)
  {
    aTrimF = theSurf->FirstVParameter(); // Trimmed first
    aTrimL = theSurf->LastVParameter(); // Trimmed last
    aS->Bounds(aDummyF, aDummyL, aBaseF, aBaseL); // Non-trimmed values.
    if (Abs (aBaseF - aTrimF) + Abs (aBaseL - aTrimL) > Precision::PConfusion())
    {
      // Param space reduced.
      theVPeriod = 0.0;
    }
  }
}

//=======================================================================
//function : aFuncValue
//purpose  : compute functional value in (theU,theV) point
//=======================================================================
static Standard_Real anOrthogSqValue(const gp_Pnt& aBasePnt,
                                     const Handle(Adaptor3d_HSurface)& Surf,
                                     const Standard_Real theU,
                                     const Standard_Real theV)
{
  // Since find projection, formula is:
  // F1 = Dot(S_U, Vec(aBasePnt, aProjPnt))
  // F2 = Dot(S_V, Vec(aBasePnt, aProjPnt))

  gp_Pnt aProjPnt;
  gp_Vec aSu, aSv;

  Surf->D1(theU, theV, aProjPnt, aSu, aSv);
  gp_Vec aBaseVec(aBasePnt, aProjPnt);

  if (aSu.SquareMagnitude() > Precision::SquareConfusion())
    aSu.Normalize();

  if (aSv.SquareMagnitude() > Precision::SquareConfusion())
    aSv.Normalize();

  Standard_Real aFirstPart = aSu.Dot(aBaseVec);
  Standard_Real aSecondPart = aSv.Dot(aBaseVec);
  return (aFirstPart * aFirstPart + aSecondPart * aSecondPart);
}

//=======================================================================
//function : Value
//purpose  : (OCC217 - apo)- Compute Point2d that project on polar surface(<Surf>) 3D<Curve>
//            <InitCurve2d> use for calculate start 2D point.
//=======================================================================
static gp_Pnt2d Function_Value(const Standard_Real theU,
                               const aFuncStruct& theData)
{
  gp_Pnt2d p2d = theData.myInitCurve2d->Value(theU) ;
  gp_Pnt p = theData.myCurve->Value(theU);
  gp_Pnt aSurfPnt = theData.mySurf->Value(p2d.X(), p2d.Y());
  Standard_Real aSurfPntDist = aSurfPnt.SquareDistance(p);

  Standard_Real Uinf, Usup, Vinf, Vsup;
  Uinf = theData.mySurf->Surface().FirstUParameter();
  Usup = theData.mySurf->Surface().LastUParameter();
  Vinf = theData.mySurf->Surface().FirstVParameter();
  Vsup = theData.mySurf->Surface().LastVParameter();

  // Check case when curve is close to co-parametrized isoline on surf.
  if (Abs (p2d.X() - Uinf) < Precision::PConfusion() ||
      Abs (p2d.X() - Usup) < Precision::PConfusion() )
  {
    // V isoline.
    gp_Pnt aPnt;
    theData.mySurf->D0(p2d.X(), theU, aPnt);
    if (aPnt.SquareDistance(p) < aSurfPntDist)
      p2d.SetY(theU);
  }

  if (Abs (p2d.Y() - Vinf) < Precision::PConfusion() ||
      Abs (p2d.Y() - Vsup) < Precision::PConfusion() )
  {
    // U isoline.
    gp_Pnt aPnt;
    theData.mySurf->D0(theU, p2d.Y(), aPnt);
    if (aPnt.SquareDistance(p) < aSurfPntDist)
      p2d.SetX(theU);
  }

  Standard_Integer decalU = 0, decalV = 0;
  Standard_Real U0 = p2d.X(), V0 = p2d.Y();

  GeomAbs_SurfaceType Type = theData.mySurf->GetType();
  if((Type != GeomAbs_BSplineSurface) && 
     (Type != GeomAbs_BezierSurface)  &&
     (Type != GeomAbs_OffsetSurface)    )
  {
    // Analytical cases.
    Standard_Real S = 0., T = 0.;
    switch (Type)
    {
    case GeomAbs_Cylinder:
      {
        gp_Cylinder Cylinder = theData.mySurf->Cylinder();
        ElSLib::Parameters( Cylinder, p, S, T);
        if(U0 < Uinf) decalU = -int((Uinf - U0)/(2*M_PI))-1;
        if(U0 > Usup) decalU =  int((U0 - Usup)/(2*M_PI))+1;
        S += decalU*2*M_PI;
        break;
      }
    case GeomAbs_Cone:
      {
        gp_Cone Cone = theData.mySurf->Cone();
        ElSLib::Parameters( Cone, p, S, T);
        if(U0 < Uinf) decalU = -int((Uinf - U0)/(2*M_PI))-1;
        if(U0 > Usup) decalU =  int((U0 - Usup)/(2*M_PI))+1;
        S += decalU*2*M_PI;
        break;
      }
    case GeomAbs_Sphere:
      {
        gp_Sphere Sphere = theData.mySurf->Sphere();
        ElSLib::Parameters( Sphere, p, S, T);
        if(U0 < Uinf) decalU = -int((Uinf - U0)/(2*M_PI))-1;
        if(U0 > Usup) decalU =  int((U0 - Usup)/(2*M_PI))+1;
        S += decalU*2*M_PI;
        if(V0 < Vinf) decalV = -int((Vinf - V0)/(2*M_PI))-1;
        if(V0 > (Vsup+(Vsup-Vinf))) decalV =  int((V0 - Vsup+(Vsup-Vinf))/(2*M_PI))+1;
        T += decalV*2*M_PI;
        if(0.4*M_PI < Abs(U0 - S) && Abs(U0 - S) < 1.6*M_PI)
        {
          T = M_PI - T;
          if(U0 < S)
            S -= M_PI;
          else
            S += M_PI;
        }
        break;
      }
    case GeomAbs_Torus:
      {
        gp_Torus Torus = theData.mySurf->Torus();
        ElSLib::Parameters( Torus, p, S, T);
        if(U0 < Uinf) decalU = -int((Uinf - U0)/(2*M_PI))-1;
        if(U0 > Usup) decalU =  int((U0 - Usup)/(2*M_PI))+1;
        if(V0 < Vinf) decalV = -int((Vinf - V0)/(2*M_PI))-1;
        if(V0 > Vsup) decalV =  int((V0 - Vsup)/(2*M_PI))+1;
        S += decalU*2*M_PI; T += decalV*2*M_PI;
        break;
      }
    default:
      throw Standard_NoSuchObject("ProjLib_ComputeApproxOnPolarSurface::Value");
    }
    return gp_Pnt2d(S, T);
  }

  // Non-analytical case.
  Standard_Real Dist2Min = RealLast();
  Standard_Real uperiod = theData.myPeriod[0],
                vperiod = theData.myPeriod[1],
                u, v;

  // U0 and V0 are the points within the initialized period.
  if(U0 < Uinf)
  {
    if(!uperiod)
      U0 = Uinf;
    else
    {
      decalU = int((Uinf - U0)/uperiod)+1;
      U0 += decalU*uperiod;
    }
  }
  if(U0 > Usup)
  {
    if(!uperiod)
      U0 = Usup;
    else
    {
      decalU = -(int((U0 - Usup)/uperiod)+1);
      U0 += decalU*uperiod;
    }
  }
  if(V0 < Vinf)
  {
    if(!vperiod)
      V0 = Vinf;
    else
    {
      decalV = int((Vinf - V0)/vperiod)+1;
      V0 += decalV*vperiod;
    }
  }
  if(V0 > Vsup)
  {
    if(!vperiod)
      V0 = Vsup;
    else
    {
      decalV = -int((V0 - Vsup)/vperiod)-1;
      V0 += decalV*vperiod;
    }
  }

  // The surface around (U0,V0) is reduced.
  Standard_Real uLittle = (Usup - Uinf)/10, vLittle = (Vsup - Vinf)/10;
  Standard_Real uInfLi = 0, vInfLi = 0,uSupLi = 0, vSupLi = 0;
  if((U0 - Uinf) > uLittle) uInfLi = U0 - uLittle; else uInfLi = Uinf;
  if((V0 - Vinf) > vLittle) vInfLi = V0 - vLittle; else vInfLi = Vinf;
  if((Usup - U0) > uLittle) uSupLi = U0 + uLittle; else uSupLi = Usup;
  if((Vsup - V0) > vLittle) vSupLi = V0 + vLittle; else vSupLi = Vsup;

  GeomAdaptor_Surface SurfLittle;
  if (Type == GeomAbs_BSplineSurface)
  {
    Handle(Geom_Surface) GBSS(theData.mySurf->Surface().BSpline());
    SurfLittle.Load(GBSS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else if (Type == GeomAbs_BezierSurface)
  {
    Handle(Geom_Surface) GS(theData.mySurf->Surface().Bezier());
    SurfLittle.Load(GS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else if (Type == GeomAbs_OffsetSurface)
  {
    Handle(Geom_Surface) GS = GeomAdaptor::MakeSurface(theData.mySurf->Surface());
    SurfLittle.Load(GS, uInfLi, uSupLi, vInfLi, vSupLi);
  }
  else
  {
    throw Standard_NoSuchObject ("ProjLib_ComputeApproxOnPolarSurface::ProjectUsingInitialCurve2d() - unknown surface type");
  }

  // Try to run simple search with initial point (U0, V0).
  Extrema_GenLocateExtPS  locext(SurfLittle, theData.myTolU, theData.myTolV);
  locext.Perform(p, U0, V0);
  if (locext.IsDone()) 
  {
    locext.Point().Parameter(u, v);
    Dist2Min = anOrthogSqValue(p, theData.mySurf, u, v);
    if (Dist2Min < theData.mySqProjOrtTol && // Point is projection.
        locext.SquareDistance() < aSurfPntDist + Precision::SquareConfusion()) // Point better than initial.
    {
      gp_Pnt2d pnt(u - decalU*uperiod,v - decalV*vperiod);
      return pnt;
    }
  }

  // Perform whole param space search.
  Extrema_ExtPS  ext(p, SurfLittle, theData.myTolU, theData.myTolV);
  if (ext.IsDone() && ext.NbExt() >= 1)
  {
    Dist2Min = ext.SquareDistance(1);
    Standard_Integer GoodValue = 1;
    for (Standard_Integer i = 2 ; i <= ext.NbExt() ; i++ )
    {
      if( Dist2Min > ext.SquareDistance(i))
      {
        Dist2Min = ext.SquareDistance(i);
        GoodValue = i;
      }
    }
    ext.Point(GoodValue).Parameter(u, v);
    Dist2Min = anOrthogSqValue(p, theData.mySurf, u, v);
    if (Dist2Min < theData.mySqProjOrtTol && // Point is projection.
        ext.SquareDistance(GoodValue) < aSurfPntDist + Precision::SquareConfusion()) // Point better than initial.
    {
      gp_Pnt2d pnt(u - decalU*uperiod,v - decalV*vperiod);
      return pnt;
    }
  }

  // Both searches return bad values, use point from initial 2dcurve.
  return p2d;
}


//=======================================================================
//function : ProjLib_PolarFunction
//purpose  : (OCC217 - apo)- This class produce interface to call "gp_Pnt2d Function_Value(...)"
//=======================================================================

class ProjLib_PolarFunction : public AppCont_Function
{
  aFuncStruct myStruct;

  public :

  ProjLib_PolarFunction(const Handle(Adaptor3d_HCurve) & C, 
                        const Handle(Adaptor3d_HSurface)& Surf,
                        const Handle(Adaptor2d_HCurve2d)& InitialCurve2d,
                        const Standard_Real Tol3d)
  {
    myNbPnt = 0;
    myNbPnt2d = 1;

    computePeriodicity(Surf, myStruct.myPeriod[0], myStruct.myPeriod[1]);

    myStruct.myCurve = C;
    myStruct.myInitCurve2d = InitialCurve2d;
    myStruct.mySurf = Surf;
    myStruct.mySqProjOrtTol = 10000.0 * Tol3d * Tol3d;
    myStruct.myTolU = Surf->UResolution(Tol3d);
    myStruct.myTolV = Surf->VResolution(Tol3d);
  }

  ~ProjLib_PolarFunction() {}

  Standard_Real FirstParameter() const
  {
    return myStruct.myCurve->FirstParameter();
  }

  Standard_Real LastParameter() const
  {
    return myStruct.myCurve->LastParameter();
  }

  gp_Pnt2d Value(const Standard_Real t) const
  {
    return Function_Value(t, myStruct);
  }

  Standard_Boolean Value(const Standard_Real   theT,
                         NCollection_Array1<gp_Pnt2d>& thePnt2d,
                         NCollection_Array1<gp_Pnt>&   /*thePnt*/) const
  {
    thePnt2d(1) = Function_Value(theT, myStruct);
    return Standard_True;
  }

  Standard_Boolean D1(const Standard_Real   /*theT*/,
                      NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
                      NCollection_Array1<gp_Vec>&   /*theVec*/) const
    {return Standard_False;}
};

//=======================================================================
//function : ProjLib_ComputeApproxOnPolarSurface
//purpose  : 
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface()
: myProjIsDone(Standard_False),
  myTolerance(Precision::Approximation()),
  myTolReached(-1.0),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
}

//=======================================================================
//function : ProjLib_ComputeApproxOnPolarSurface
//purpose  : 
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface
                    (const Handle(Adaptor2d_HCurve2d)& theInitialCurve2d,
                     const Handle(Adaptor3d_HCurve)&   theCurve,
                     const Handle(Adaptor3d_HSurface)& theSurface,
                     const Standard_Real               theTolerance3D)
: myProjIsDone(Standard_False),
  myTolerance(theTolerance3D),
  myTolReached(-1.0),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  myBSpline = Perform(theInitialCurve2d, theCurve, theSurface);
}

//=======================================================================
//function : ProjLib_ComputeApproxOnPolarSurface
//purpose  : case without curve of initialization
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface
                      (const Handle(Adaptor3d_HCurve)&   theCurve,
                       const Handle(Adaptor3d_HSurface)& theSurface,
                       const Standard_Real               theTolerance3D)
: myProjIsDone(Standard_False),
  myTolerance(theTolerance3D),
  myTolReached(-1.0),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  const Handle(Adaptor2d_HCurve2d) anInitCurve2d;
  myBSpline = Perform(anInitCurve2d, theCurve, theSurface);  
} 

//=======================================================================
//function : ProjLib_ComputeApproxOnPolarSurface
//purpose  : Process the case of sewing
//=======================================================================

ProjLib_ComputeApproxOnPolarSurface::ProjLib_ComputeApproxOnPolarSurface
                (const Handle(Adaptor2d_HCurve2d)& theInitialCurve2d,
                 const Handle(Adaptor2d_HCurve2d)& theInitialCurve2dBis,
                 const Handle(Adaptor3d_HCurve)&   theCurve,
                 const Handle(Adaptor3d_HSurface)& theSurface,
                 const Standard_Real               theTolerance3D)
: myProjIsDone(Standard_False),
  myTolerance(theTolerance3D),
  myTolReached(-1.0),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  // InitialCurve2d and InitialCurve2dBis are two pcurves of the sewing 
  Handle(Geom2d_BSplineCurve) bsc =
    Perform(theInitialCurve2d, theCurve, theSurface);

  if(myProjIsDone) {
    gp_Pnt2d P2dproj, P2d, P2dBis;
    P2dproj = bsc->StartPoint();
    P2d    = theInitialCurve2d->Value(theInitialCurve2d->FirstParameter());
    P2dBis = theInitialCurve2dBis->Value(theInitialCurve2dBis->FirstParameter());

    Standard_Real Dist, DistBis;
    Dist    = P2dproj.Distance(P2d);
    DistBis = P2dproj.Distance(P2dBis);
    if( Dist < DistBis)  {
      // myBSpline2d is the pcurve that is found. It is translated to obtain myCurve2d
      myBSpline = bsc;
      Handle(Geom2d_Geometry) GG = myBSpline->Translated(P2d, P2dBis);
      my2ndCurve = Handle(Geom2d_Curve)::DownCast(GG);
    }
    else {
      my2ndCurve = bsc;
      Handle(Geom2d_Geometry) GG = my2ndCurve->Translated(P2dBis, P2d);
      myBSpline = Handle(Geom2d_BSplineCurve)::DownCast(GG);
    }
  }
}


//=======================================================================
//function : Concat
//purpose  : 
//=======================================================================

static Handle(Geom2d_BSplineCurve) Concat(Handle(Geom2d_BSplineCurve) C1,
                                          Handle(Geom2d_BSplineCurve) C2,
                                          Standard_Real theUJump,
                                          Standard_Real theVJump)
{
  Standard_Integer deg, deg1, deg2;
  deg1 = C1->Degree();
  deg2 = C2->Degree();
  
  if ( deg1 < deg2) {
    C1->IncreaseDegree(deg2);
    deg = deg2;
  }
  else if ( deg2 < deg1) {
    C2->IncreaseDegree(deg1);
    deg = deg1;
  }
  else deg = deg1;

  Standard_Integer np1,np2,nk1,nk2,np,nk;
  np1 = C1->NbPoles();
  nk1 = C1->NbKnots();
  np2 = C2->NbPoles();
  nk2 = C2->NbKnots();
  nk = nk1 + nk2 -1;
  np = np1 + np2 -1;

  TColStd_Array1OfReal    K1(1,nk1); C1->Knots(K1);
  TColStd_Array1OfInteger M1(1,nk1); C1->Multiplicities(M1);
  TColgp_Array1OfPnt2d    P1(1,np1); C1->Poles(P1);
  TColStd_Array1OfReal    K2(1,nk2); C2->Knots(K2);
  TColStd_Array1OfInteger M2(1,nk2); C2->Multiplicities(M2);
  TColgp_Array1OfPnt2d    P2(1,np2); C2->Poles(P2);

  // Compute the new BSplineCurve
  TColStd_Array1OfReal    K(1,nk);
  TColStd_Array1OfInteger M(1,nk);
  TColgp_Array1OfPnt2d    P(1,np);

  Standard_Integer i, count = 0;
  // Set Knots and Mults
  for ( i = 1; i <= nk1; i++) {
    count++;
    K(count) = K1(i);
    M(count) = M1(i);
  }
  M(count) = deg;
  for ( i = 2; i <= nk2; i++) {
    count++;
    K(count) = K2(i);
    M(count) = M2(i);
  }
  // Set the Poles
  count = 0;
  for (i = 1; i <= np1; i++) {
    count++;
    P(count) = P1(i);
  }
  for (i = 2; i <= np2; i++) {
    count++;
    P(count).SetX(P2(i).X() + theUJump);
    P(count).SetY(P2(i).Y() + theVJump);
  }

  Handle(Geom2d_BSplineCurve) BS = 
    new Geom2d_BSplineCurve(P,K,M,deg);
  return BS;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ProjLib_ComputeApproxOnPolarSurface::Perform
(const Handle(Adaptor3d_HCurve)& Curve, const Handle(Adaptor3d_HSurface)& S)
{
  const Handle(Adaptor2d_HCurve2d) anInitCurve2d;
  myBSpline = Perform(anInitCurve2d, Curve, S);  
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) ProjLib_ComputeApproxOnPolarSurface::Perform
(const Handle(Adaptor2d_HCurve2d)& InitialCurve2d,
 const Handle(Adaptor3d_HCurve)& Curve,
 const Handle(Adaptor3d_HSurface)& S)
{
  //OCC217
  Standard_Real Tol3d = myTolerance; 
  Standard_Real ParamTol = Precision::PApproximation();

  Handle(Adaptor2d_HCurve2d) AHC2d = InitialCurve2d;
  Handle(Adaptor3d_HCurve) AHC = Curve;
  
// if the curve 3d is a BSpline with degree C0, it is cut into sections with degree C1
// -> bug cts18237
  GeomAbs_CurveType typeCurve = Curve->GetType();
  if(typeCurve == GeomAbs_BSplineCurve) {
    TColStd_ListOfTransient LOfBSpline2d;
    Handle(Geom_BSplineCurve) BSC = Curve->BSpline();
    Standard_Integer nbInter = Curve->NbIntervals(GeomAbs_C1);
    if(nbInter > 1) {
      Standard_Integer i, j;
      Handle(Geom_TrimmedCurve) GTC;
      Handle(Geom2d_TrimmedCurve) G2dTC;
      TColStd_Array1OfReal Inter(1,nbInter+1);
      Curve->Intervals(Inter,GeomAbs_C1);
      Standard_Real firstinter = Inter.Value(1), secondinter = Inter.Value(2);
      // initialization 3d
      GTC = new Geom_TrimmedCurve(BSC, firstinter, secondinter);
      AHC = new GeomAdaptor_HCurve(GTC);
      
      // if there is an initialization curve: 
      // - either this is a BSpline C0, with discontinuity at the same parameters of nodes
      // and the sections C1 are taken
      // - or this is a curve C1 and the sections of intrest are taken otherwise the curve is created.
      
      // initialization 2d
      Standard_Integer nbInter2d;
      Standard_Boolean C2dIsToCompute;
      C2dIsToCompute = InitialCurve2d.IsNull();
      Handle(Geom2d_BSplineCurve) BSC2d;
      Handle(Geom2d_Curve) G2dC;
      
      if(!C2dIsToCompute) {
	nbInter2d = InitialCurve2d->NbIntervals(GeomAbs_C1);
	TColStd_Array1OfReal Inter2d(1,nbInter2d+1);
	InitialCurve2d->Intervals(Inter2d,GeomAbs_C1);
	j = 1;
	for(i = 1,j = 1;i <= nbInter;i++)
	  if(Abs(Inter.Value(i) - Inter2d.Value(j)) < ParamTol) { //OCC217
	  //if(Abs(Inter.Value(i) - Inter2d.Value(j)) < myTolerance) {
	    if (j > nbInter2d) break;
	    j++;
	  }
	if(j != (nbInter2d+1)) {
	  C2dIsToCompute = Standard_True;
	}
      }
      
      if(C2dIsToCompute) {
	AHC2d = BuildInitialCurve2d(AHC, S);
      }
      else {
	typeCurve = InitialCurve2d->GetType();
	switch (typeCurve) {
	case GeomAbs_Line: {
	  G2dC = new Geom2d_Line(InitialCurve2d->Line());
	  break;
	}
	case GeomAbs_Circle: {
	  G2dC = new Geom2d_Circle(InitialCurve2d->Circle());
	  break;
	}
	case GeomAbs_Ellipse: {
	  G2dC = new Geom2d_Ellipse(InitialCurve2d->Ellipse());
	  break;
	}
	case GeomAbs_Hyperbola: {
	  G2dC = new Geom2d_Hyperbola(InitialCurve2d->Hyperbola());
	  break;
	}
	case GeomAbs_Parabola: {
	  G2dC = new Geom2d_Parabola(InitialCurve2d->Parabola());
	  break;
	}
	case GeomAbs_BezierCurve: {
	  G2dC = InitialCurve2d->Bezier();
	  break;
	}
	case GeomAbs_BSplineCurve: {
	  G2dC = InitialCurve2d->BSpline();
	  break;
	}
        case GeomAbs_OtherCurve:
        default:
	  break;
	}
	gp_Pnt2d fp2d = G2dC->Value(firstinter), lp2d = G2dC->Value(secondinter);
	gp_Pnt fps, lps, fpc, lpc;
	S->D0(fp2d.X(), fp2d.Y(), fps);
	S->D0(lp2d.X(), lp2d.Y(), lps);
	Curve->D0(firstinter, fpc);
	Curve->D0(secondinter, lpc);
	//OCC217
	if((fps.IsEqual(fpc, Tol3d)) &&
	   (lps.IsEqual(lpc, Tol3d))) {
	//if((fps.IsEqual(fpc, myTolerance)) &&
	//   (lps.IsEqual(lpc, myTolerance))) {
	  G2dTC = new Geom2d_TrimmedCurve(G2dC, firstinter, secondinter);
	  Geom2dAdaptor_Curve G2dAC(G2dTC);
	  AHC2d = new Geom2dAdaptor_HCurve(G2dAC);
	  myProjIsDone = Standard_True;
	}
	else {
	  AHC2d = BuildInitialCurve2d(AHC, S);
	  C2dIsToCompute = Standard_True;
	}
      }
	
      if(myProjIsDone) {
	BSC2d = ProjectUsingInitialCurve2d(AHC, S, AHC2d);
 	if(BSC2d.IsNull()) 
        {
            return Handle(Geom2d_BSplineCurve)();
        }
	LOfBSpline2d.Append(BSC2d);
      }
      else {
	return Handle(Geom2d_BSplineCurve)();
      }
      


      Standard_Real iinter, ip1inter;
      Standard_Integer nbK2d, deg;
      nbK2d = BSC2d->NbKnots(); deg = BSC2d->Degree();

      for(i = 2;i <= nbInter;i++) {
	iinter = Inter.Value(i);
	ip1inter = Inter.Value(i+1);
	// general case 3d
	GTC->SetTrim(iinter, ip1inter);
	AHC = new GeomAdaptor_HCurve(GTC);
	
	// general case 2d
	if(C2dIsToCompute) {
	  AHC2d = BuildInitialCurve2d(AHC, S);
	}
	else {
	  gp_Pnt2d fp2d = G2dC->Value(iinter), lp2d = G2dC->Value(ip1inter);
	  gp_Pnt fps, lps, fpc, lpc;
	  S->D0(fp2d.X(), fp2d.Y(), fps);
	  S->D0(lp2d.X(), lp2d.Y(), lps);
	  Curve->D0(iinter, fpc);
	  Curve->D0(ip1inter, lpc);
	  //OCC217
  	  if((fps.IsEqual(fpc, Tol3d)) &&
	     (lps.IsEqual(lpc, Tol3d))) {
	  //if((fps.IsEqual(fpc, myTolerance)) &&
	  //   (lps.IsEqual(lpc, myTolerance))) {
	    G2dTC->SetTrim(iinter, ip1inter);
	    Geom2dAdaptor_Curve G2dAC(G2dTC);
	    AHC2d = new Geom2dAdaptor_HCurve(G2dAC);
	    myProjIsDone = Standard_True;
	  }
	  else {
	    AHC2d = BuildInitialCurve2d(AHC, S);
	  }
	}
	if(myProjIsDone) {
	  BSC2d = ProjectUsingInitialCurve2d(AHC, S, AHC2d);
	  if(BSC2d.IsNull()) {
	    return Handle(Geom2d_BSplineCurve)();
	  }
	  LOfBSpline2d.Append(BSC2d);
	  nbK2d += BSC2d->NbKnots() - 1;
	  deg = Max(deg, BSC2d->Degree());
	}
	else {
	  return Handle(Geom2d_BSplineCurve)();
	}
      }

      Standard_Real anUPeriod, anVPeriod;
      computePeriodicity(S, anUPeriod, anVPeriod);
      Standard_Integer NbC = LOfBSpline2d.Extent();
      Handle(Geom2d_BSplineCurve) CurBS;
      CurBS = Handle(Geom2d_BSplineCurve)::DownCast(LOfBSpline2d.First());
      LOfBSpline2d.RemoveFirst();
      for (Standard_Integer ii = 2; ii <= NbC; ii++)
      {
        Handle(Geom2d_BSplineCurve) BS = 
          Handle(Geom2d_BSplineCurve)::DownCast(LOfBSpline2d.First());

        //Check for period jump in point of contact.
        gp_Pnt2d aC1End = CurBS->Pole(CurBS->NbPoles()); // End of C1.
        gp_Pnt2d aC2Beg = BS->Pole(1); // Beginning of C2.
        Standard_Real anUJump = 0.0, anVJump = 0.0;

        if (anUPeriod > 0.0 &&
            Abs (aC1End.X() - aC2Beg.X()) > (anUPeriod ) / 2.01)
        {
          Standard_Real aMultCoeff =  aC2Beg.X() < aC1End.X() ? 1.0 : -1.0;
          anUJump = (anUPeriod) * aMultCoeff;
        }

        if (anVPeriod &&
            Abs (aC1End.Y() - aC2Beg.Y()) > (anVPeriod) / 2.01)
        {
          Standard_Real aMultCoeff =  aC2Beg.Y() < aC1End.Y() ? 1.0 : -1.0;
          anVJump = (anVPeriod) * aMultCoeff;
        }

        CurBS = Concat(CurBS,BS, anUJump, anVJump);
        LOfBSpline2d.RemoveFirst();
      }
      return CurBS;
    }
  }
  
  if(InitialCurve2d.IsNull()) {
    AHC2d = BuildInitialCurve2d(Curve, S);
    if(!myProjIsDone) 
      return Handle(Geom2d_BSplineCurve)(); 
  }
  return ProjectUsingInitialCurve2d(AHC, S, AHC2d);     
}


//=======================================================================
//function : ProjLib_BuildInitialCurve2d
//purpose  : 
//=======================================================================

Handle(Adaptor2d_HCurve2d) 
     ProjLib_ComputeApproxOnPolarSurface::
     BuildInitialCurve2d(const Handle(Adaptor3d_HCurve)&   Curve,
			 const Handle(Adaptor3d_HSurface)& Surf)
{
  //  discretize the Curve with quasiuniform deflection
  //  density at least NbOfPnts points
  myProjIsDone = Standard_False;
  
  //OCC217
  Standard_Real Tol3d = myTolerance; 
  Standard_Real TolU = Surf->UResolution(Tol3d), TolV = Surf->VResolution(Tol3d);
  Standard_Real DistTol3d = 100.0*Tol3d;
  if(myMaxDist > 0.)
  {
    DistTol3d = myMaxDist;
  }
  Standard_Real DistTol3d2 = DistTol3d * DistTol3d;
  Standard_Real uperiod = 0.0, vperiod = 0.0;
  computePeriodicity(Surf, uperiod, vperiod);

  // NO myTol is Tol2d !!!!
  //Standard_Real TolU = myTolerance, TolV = myTolerance;
  //Standard_Real Tol3d = 100*myTolerance; // At random Balthazar.

  Standard_Integer NbOfPnts = 61; 
  GCPnts_QuasiUniformAbscissa QUA(Curve->GetCurve(),NbOfPnts);
  NbOfPnts = QUA.NbPoints();
  TColgp_Array1OfPnt Pts(1,NbOfPnts);
  TColStd_Array1OfReal Param(1,NbOfPnts);
  Standard_Integer i, j;
  for( i = 1; i <= NbOfPnts ; i++ ) { 
    Param(i) = QUA.Parameter(i);
    Pts(i) = Curve->Value(Param(i));
  }
  
  TColgp_Array1OfPnt2d Pts2d(1,NbOfPnts);
  TColStd_Array1OfInteger Mult(1,NbOfPnts);
  Mult.Init(1);
  Mult(1) = Mult(NbOfPnts) = 2;
  
  Standard_Real Uinf, Usup, Vinf, Vsup;
  Uinf = Surf->Surface().FirstUParameter();
  Usup = Surf->Surface().LastUParameter();
  Vinf = Surf->Surface().FirstVParameter();
  Vsup = Surf->Surface().LastVParameter();
  GeomAbs_SurfaceType Type = Surf->GetType();
  if((Type != GeomAbs_BSplineSurface) && (Type != GeomAbs_BezierSurface) &&
     (Type != GeomAbs_OffsetSurface)) {
    Standard_Real S, T;
//    Standard_Integer usens = 0, vsens = 0; 
    // to know the position relatively to the period
    switch (Type) {
//    case GeomAbs_Plane:
//      {
//	gp_Pln Plane = Surf->Plane();
//	for ( i = 1 ; i <= NbOfPnts ; i++) { 
//	  ElSLib::Parameters( Plane, Pts(i), S, T);
//	  Pts2d(i).SetCoord(S,T);
//	}
//	myProjIsDone = Standard_True;
//	break;
//      }
    case GeomAbs_Cylinder:
      {
//	Standard_Real Sloc, Tloc;
        Standard_Real Sloc;
        Standard_Integer usens = 0;
        gp_Cylinder Cylinder = Surf->Cylinder();
        ElSLib::Parameters( Cylinder, Pts(1), S, T);
        Pts2d(1).SetCoord(S,T);
        for ( i = 2 ; i <= NbOfPnts ; i++) { 
          Sloc = S;
          ElSLib::Parameters( Cylinder, Pts(i), S, T);
          if(Abs(Sloc - S) > M_PI) {
            if(Sloc > S)
              usens++;
            else
              usens--;
          }
          Pts2d(i).SetCoord(S+usens*2*M_PI,T);
        }
        myProjIsDone = Standard_True;
        break;
      }
    case GeomAbs_Cone:
      {
//	Standard_Real Sloc, Tloc;
        Standard_Real Sloc;
        Standard_Integer usens = 0;
        gp_Cone Cone = Surf->Cone();
        ElSLib::Parameters( Cone, Pts(1), S, T);
        Pts2d(1).SetCoord(S,T);
        for ( i = 2 ; i <= NbOfPnts ; i++) { 
          Sloc = S;
          ElSLib::Parameters( Cone, Pts(i), S, T);
          if(Abs(Sloc - S) > M_PI) {
            if(Sloc > S)
              usens++;
            else
              usens--;
          }
          Pts2d(i).SetCoord(S+usens*2*M_PI,T);
        }
        myProjIsDone = Standard_True;
        break;
      }
    case GeomAbs_Sphere:
      {
	Standard_Real Sloc, Tloc;
	Standard_Integer usens = 0, vsens = 0; //usens steps by half-period
	Standard_Boolean vparit = Standard_False;
	gp_Sphere Sphere = Surf->Sphere();
	ElSLib::Parameters( Sphere, Pts(1), S, T);
	Pts2d(1).SetCoord(S,T);
	for ( i = 2 ; i <= NbOfPnts ; i++) { 
	  Sloc = S;Tloc = T;
	  ElSLib::Parameters( Sphere, Pts(i), S, T);
	  if(1.6*M_PI < Abs(Sloc - S)) {
	    if(Sloc > S)
	      usens += 2;
	    else
	      usens -= 2;
    }
	  if(1.6*M_PI > Abs(Sloc - S) && Abs(Sloc - S) > 0.4*M_PI) {
	    vparit = !vparit;
	    if(Sloc > S)
	      usens++;
	    else
	      usens--;
	    if(Abs(Tloc - Vsup) < (Vsup - Vinf)/5)
	      vsens++;
	    else
	      vsens--;
	  }
	  if(vparit) {
	    Pts2d(i).SetCoord(S+usens*M_PI,(M_PI - T)*(vsens-1));
	  }	  
	  else {
	    Pts2d(i).SetCoord(S+usens*M_PI,T+vsens*M_PI);
	    
	  }
	}
	myProjIsDone = Standard_True;
	break;
      }
    case GeomAbs_Torus:
      {
	Standard_Real Sloc, Tloc;
	Standard_Integer usens = 0, vsens = 0;
	gp_Torus Torus = Surf->Torus();
	ElSLib::Parameters( Torus, Pts(1), S, T);
	Pts2d(1).SetCoord(S,T);
	for ( i = 2 ; i <= NbOfPnts ; i++) { 
	  Sloc = S; Tloc = T;
	  ElSLib::Parameters( Torus, Pts(i), S, T);
	  if(Abs(Sloc - S) > M_PI) {
	    if(Sloc > S)
	      usens++;
	    else
	      usens--;
    }
	  if(Abs(Tloc - T) > M_PI) {
	    if(Tloc > T)
	      vsens++;
	    else
	      vsens--;
    }
	  Pts2d(i).SetCoord(S+usens*2*M_PI,T+vsens*2*M_PI);
	}
	myProjIsDone = Standard_True;
	break;
      }
    default:
      throw Standard_NoSuchObject("ProjLib_ComputeApproxOnPolarSurface::BuildInitialCurve2d");
    }
  }
  else {
    myProjIsDone = Standard_False;
    Standard_Real Dist2Min = 1.e+200, u = 0., v = 0.;
    gp_Pnt pntproj;

    TColgp_SequenceOfPnt2d Sols;
    Standard_Boolean areManyZeros = Standard_False;
    
    Curve->D0(Param.Value(1), pntproj) ;
    Extrema_ExtPS  aExtPS(pntproj, Surf->Surface(), TolU, TolV) ;
    Standard_Real aMinSqDist = RealLast();
    if (aExtPS.IsDone())
    {
      for (i = 1; i <= aExtPS.NbExt(); i++)
      {
        Standard_Real aSqDist = aExtPS.SquareDistance(i);
        if (aSqDist < aMinSqDist)
          aMinSqDist = aSqDist;
      }
    }
    if (aMinSqDist > DistTol3d2) //try to project with less tolerance
    {
      TolU = Min(TolU, Precision::PConfusion());
      TolV = Min(TolV, Precision::PConfusion());
      aExtPS.Initialize(Surf->Surface(),
                        Surf->Surface().FirstUParameter(), Surf->Surface().LastUParameter(), 
                        Surf->Surface().FirstVParameter(), Surf->Surface().LastVParameter(),
                        TolU, TolV);
      aExtPS.Perform(pntproj);
    }

    if( aExtPS.IsDone() && aExtPS.NbExt() >= 1 ) {

      Standard_Integer GoodValue = 1;

      for ( i = 1 ; i <= aExtPS.NbExt() ; i++ ) {
	if( aExtPS.SquareDistance(i) < DistTol3d2 ) {
	  if( aExtPS.SquareDistance(i) <= 1.e-18 ) {
	    aExtPS.Point(i).Parameter(u,v);
	    gp_Pnt2d p2d(u,v);
	    Standard_Boolean isSame = Standard_False;
	    for( j = 1; j <= Sols.Length(); j++ ) {
	      if( p2d.SquareDistance( Sols.Value(j) ) <= 1.e-18 ) {
		isSame = Standard_True;
		break;
	      }
	    }
	    if( !isSame ) Sols.Append( p2d );
	  }
	  if( Dist2Min > aExtPS.SquareDistance(i) ) {
	    Dist2Min = aExtPS.SquareDistance(i);
	    GoodValue = i;
	  }
	}
      }

      if( Sols.Length() > 1 ) areManyZeros = Standard_True;

      if( Dist2Min <= DistTol3d2) {
	if( !areManyZeros ) {
	  aExtPS.Point(GoodValue).Parameter(u,v);
	  Pts2d(1).SetCoord(u,v);
	  myProjIsDone = Standard_True;
	}
	else {
	  Standard_Integer nbSols = Sols.Length();
	  Standard_Real Dist2Max = -1.e+200;
	  for( i = 1; i <= nbSols; i++ ) {
	    const gp_Pnt2d& aP1 = Sols.Value(i);
	    for( j = i+1; j <= nbSols; j++ ) {
	      const gp_Pnt2d& aP2 = Sols.Value(j);
	      Standard_Real aDist2 = aP1.SquareDistance(aP2);
	      if( aDist2 > Dist2Max ) Dist2Max = aDist2;
	    }
	  }
      Standard_Real aMaxT2 = Max(TolU,TolV);
      aMaxT2 *= aMaxT2;
	  if( Dist2Max > aMaxT2 ) {
	    Standard_Integer tPp = 0;
	    for( i = 1; i <= 5; i++ ) {
	      Standard_Integer nbExtOk = 0;
	      Standard_Integer indExt = 0;
	      Standard_Integer iT = 1 + (NbOfPnts - 1)/5*i;
	      Curve->D0( Param.Value(iT), pntproj );
	      Extrema_ExtPS aTPS( pntproj, Surf->Surface(), TolU, TolV );
	      Dist2Min = 1.e+200;
	      if( aTPS.IsDone() && aTPS.NbExt() >= 1 ) {
		for( j = 1 ; j <= aTPS.NbExt() ; j++ ) {
		  if( aTPS.SquareDistance(j) < DistTol3d2 ) {
		    nbExtOk++;
		    if( aTPS.SquareDistance(j) < Dist2Min ) {
		      Dist2Min = aTPS.SquareDistance(j);
		      indExt = j;
		    }
		  }
		}
	      }
	      if( nbExtOk == 1 ) {
		tPp = iT;
		aTPS.Point(indExt).Parameter(u,v);
		break;
	      }
	    }

	    if( tPp != 0 ) {
	      gp_Pnt2d aPp = gp_Pnt2d(u,v);
	      gp_Pnt2d aPn;
	      j = 1;
	      Standard_Boolean isFound = Standard_False;
	      while( !isFound ) { 
		Curve->D0( Param.Value(tPp+j), pntproj );
		Extrema_ExtPS aTPS( pntproj, Surf->Surface(), TolU, TolV );
		Dist2Min = 1.e+200;
		Standard_Integer indExt = 0;
		if( aTPS.IsDone() && aTPS.NbExt() >= 1 ) {
		  for( i = 1 ; i <= aTPS.NbExt() ; i++ ) {
		    if( aTPS.SquareDistance(i) < DistTol3d2 && aTPS.SquareDistance(i) < Dist2Min ) {
		      Dist2Min = aTPS.SquareDistance(i);
		      indExt = i;
		      isFound = Standard_True;
		    }
		  }
		}
		if( isFound ) {
		  aTPS.Point(indExt).Parameter(u,v);
		  aPn = gp_Pnt2d(u,v);
		  break;
		}
		j++;
		if( (tPp+j) > NbOfPnts ) break;
	      }

	      if( isFound ) {
		gp_Vec2d atV(aPp,aPn);
		Standard_Boolean isChosen = Standard_False;
		for( i = 1; i <= nbSols; i++ ) {
		  const gp_Pnt2d& aP1 = Sols.Value(i);
		  gp_Vec2d asV(aP1,aPp);
		  if( asV.Dot(atV) > 0. ) {
		    isChosen = Standard_True;
		    Pts2d(1).SetCoord(aP1.X(),aP1.Y());
		    myProjIsDone = Standard_True;
		    break;
		  }
		}
		if( !isChosen ) {
		  aExtPS.Point(GoodValue).Parameter(u,v);
		  Pts2d(1).SetCoord(u,v);
		  myProjIsDone = Standard_True;
		}
	      }
	      else {
		aExtPS.Point(GoodValue).Parameter(u,v);
		Pts2d(1).SetCoord(u,v);
		myProjIsDone = Standard_True;
	      }
	    }
	    else {
	      aExtPS.Point(GoodValue).Parameter(u,v);
	      Pts2d(1).SetCoord(u,v);
	      myProjIsDone = Standard_True;
	    }
	  }
	  else {
	    aExtPS.Point(GoodValue).Parameter(u,v);
	    Pts2d(1).SetCoord(u,v);
	    myProjIsDone = Standard_True;
	  }
	}
      }
      
      //  calculate the following points with GenLocate_ExtPS
      // (and store the result and each parameter in a sequence)
      Standard_Integer usens = 0, vsens = 0; 
      // to know the position relatively to the period
      Standard_Real U0 = u, V0 = v, U1 = u, V1 = v;
      // U0 and V0 are the points in the initialized period 
      // (period with u and v),
      // U1 and V1 are the points for construction of poles

      for ( i = 2 ; i <= NbOfPnts ; i++) 
	if(myProjIsDone) {
	  myProjIsDone = Standard_False;
	  Dist2Min = RealLast();
	  Curve->D0(Param.Value(i), pntproj);
          Extrema_GenLocateExtPS  aLocateExtPS(Surf->Surface(), TolU, TolV);
          aLocateExtPS.Perform(pntproj, U0, V0);

	  if (aLocateExtPS.IsDone())
          {
	    if (aLocateExtPS.SquareDistance() < DistTol3d2)
            {  //OCC217
              //if (aLocateExtPS.SquareDistance() < Tol3d * Tol3d) {
	      (aLocateExtPS.Point()).Parameter(U0,V0);
	      U1 = U0 + usens*uperiod;
	      V1 = V0 + vsens*vperiod;
	      Pts2d(i).SetCoord(U1,V1);
	      myProjIsDone = Standard_True;
	    }
            else
            {
              Extrema_ExtPS aGlobalExtr(pntproj, Surf->Surface(), TolU, TolV);
              if (aGlobalExtr.IsDone())
              {
                Standard_Real LocalMinSqDist = RealLast();
                Standard_Integer imin = 0;
                for (Standard_Integer isol = 1; isol <= aGlobalExtr.NbExt(); isol++)
                {
                  Standard_Real aSqDist = aGlobalExtr.SquareDistance(isol);
                  if (aSqDist < LocalMinSqDist)
                  {
                    LocalMinSqDist = aSqDist;
                    imin = isol;
                  }
                }
                if (LocalMinSqDist < DistTol3d2)
                {
                  Standard_Real LocalU, LocalV;
                  aGlobalExtr.Point(imin).Parameter(LocalU, LocalV);
                  if (uperiod > 0. && Abs(U0 - LocalU) >= uperiod/2.)
                  {
                    if (LocalU > U0)
                      usens = -1;
                    else
                      usens = 1;
                  }
                  if (vperiod > 0. && Abs(V0 - LocalV) >= vperiod/2.)
                  {
                    if (LocalV > V0)
                      vsens = -1;
                    else
                      vsens = 1;
                  }
                  U0 = LocalU; V0 = LocalV;
                  U1 = U0 + usens*uperiod;
                  V1 = V0 + vsens*vperiod;
                  Pts2d(i).SetCoord(U1,V1);
                  myProjIsDone = Standard_True;

                  if((i == 2) && (!IsEqual(uperiod, 0.0) || !IsEqual(vperiod, 0.0)))
                  {//Make 1st point more precise for periodic surfaces
                    const Standard_Integer aSize = 3;
                    const gp_Pnt2d aP(Pts2d(2)); 
                    Standard_Real aUpar[aSize], aVpar[aSize];
                    Pts2d(1).Coord(aUpar[1], aVpar[1]);
                    aUpar[0] = aUpar[1] - uperiod;
                    aUpar[2] = aUpar[1] + uperiod;
                    aVpar[0] = aVpar[1] - vperiod;
                    aVpar[2] = aVpar[1] + vperiod;

                    Standard_Real aSQdistMin = RealLast();
                    Standard_Integer aBestUInd = 1, aBestVInd = 1;
                    const Standard_Integer  aSizeU = IsEqual(uperiod, 0.0) ? 1 : aSize,
                                            aSizeV = IsEqual(vperiod, 0.0) ? 1 : aSize;
                    for(Standard_Integer uInd = 0; uInd < aSizeU; uInd++)
                    {
                      for(Standard_Integer vInd = 0; vInd < aSizeV; vInd++)
                      {
                        Standard_Real aSQdist = aP.SquareDistance(gp_Pnt2d(aUpar[uInd], aVpar[vInd]));
                        if(aSQdist < aSQdistMin)
                        {
                          aSQdistMin = aSQdist;
                          aBestUInd = uInd;
                          aBestVInd = vInd;
                        }
                      }
                    }

                    Pts2d(1).SetCoord(aUpar[aBestUInd], aVpar[aBestVInd]);
                  }//if(i == 2) condition
                }
              }
            }
          }
	  if(!myProjIsDone && uperiod) {
	    Standard_Real aUinf, aUsup, Uaux;
	    aUinf = Surf->Surface().FirstUParameter();
	    aUsup = Surf->Surface().LastUParameter();
	    if((aUsup - U0) > (U0 - aUinf)) 
	      Uaux = 2*aUinf - U0 + uperiod;
	    else 
	      Uaux = 2*aUsup - U0 - uperiod;

            Extrema_GenLocateExtPS  locext(Surf->Surface(), TolU, TolV);
            locext.Perform(pntproj, Uaux, V0);

	    if (locext.IsDone())
	      if (locext.SquareDistance() < DistTol3d2) {  //OCC217
	      //if (locext.SquareDistance() < Tol3d * Tol3d) {
		(locext.Point()).Parameter(u,v);
		if((aUsup - U0) > (U0 - aUinf)) 
		  usens--;
		else 
		  usens++;
		U0 = u; V0 = v;
		U1 = U0 + usens*uperiod;
		V1 = V0 + vsens*vperiod;
		Pts2d(i).SetCoord(U1,V1);
		myProjIsDone = Standard_True;
	      }
	  }
	  if(!myProjIsDone && vperiod) {
	    Standard_Real aVinf, aVsup, Vaux;
	    aVinf = Surf->Surface().FirstVParameter();
	    aVsup = Surf->Surface().LastVParameter();
	    if((aVsup - V0) > (V0 - aVinf)) 
	      Vaux = 2*aVinf - V0 + vperiod;
	    else 
	      Vaux = 2*aVsup - V0 - vperiod;

            Extrema_GenLocateExtPS  locext(Surf->Surface(), TolU, TolV);
            locext.Perform(pntproj, U0, Vaux);

	    if (locext.IsDone())
	      if (locext.SquareDistance() < DistTol3d2) {  //OCC217
	      //if (locext.SquareDistance() < Tol3d * Tol3d) {
		(locext.Point()).Parameter(u,v);
		if((aVsup - V0) > (V0 - aVinf)) 
		  vsens--;
		else 
		  vsens++;
		U0 = u; V0 = v;
		U1 = U0 + usens*uperiod;
		V1 = V0 + vsens*vperiod;
		Pts2d(i).SetCoord(U1,V1);
		myProjIsDone = Standard_True;
	      }
	  }	
	  if(!myProjIsDone && uperiod && vperiod) {
	    Standard_Real Uaux, Vaux;
	    if((Usup - U0) > (U0 - Uinf)) 
	      Uaux = 2*Uinf - U0 + uperiod;
	    else 
	      Uaux = 2*Usup - U0 - uperiod;
	    if((Vsup - V0) > (V0 - Vinf)) 
	      Vaux = 2*Vinf - V0 + vperiod;
	    else 
	      Vaux = 2*Vsup - V0 - vperiod;

            Extrema_GenLocateExtPS  locext(Surf->Surface(), TolU, TolV);
            locext.Perform(pntproj, Uaux, Vaux);

	    if (locext.IsDone())
	      if (locext.SquareDistance() < DistTol3d2) {
	      //if (locext.SquareDistance() < Tol3d * Tol3d) {
		(locext.Point()).Parameter(u,v);
		if((Usup - U0) > (U0 - Uinf)) 
		  usens--;
		else 
		  usens++;
		if((Vsup - V0) > (V0 - Vinf)) 
		  vsens--;
		else 
		  vsens++;
		U0 = u; V0 = v;
		U1 = U0 + usens*uperiod;
		V1 = V0 + vsens*vperiod;
		Pts2d(i).SetCoord(U1,V1);
		myProjIsDone = Standard_True;
	      }
	  }
	  if(!myProjIsDone) {
	    Extrema_ExtPS ext(pntproj, Surf->Surface(), TolU, TolV) ;
	    if (ext.IsDone()) {
	      Dist2Min = ext.SquareDistance(1);
	      Standard_Integer aGoodValue = 1;
	      for ( j = 2 ; j <= ext.NbExt() ; j++ )
		if( Dist2Min > ext.SquareDistance(j)) {
		  Dist2Min = ext.SquareDistance(j);
		  aGoodValue = j;
		}
	      if (Dist2Min < DistTol3d2) {
	      //if (Dist2Min < Tol3d * Tol3d) {
		(ext.Point(aGoodValue)).Parameter(u,v);
		if(uperiod) {
		  if((U0 - u) > (2*uperiod/3)) {
		    usens++;
		  }
		  else
		    if((u - U0) > (2*uperiod/3)) {
		      usens--;
		    }
    }
		if(vperiod) {
		  if((V0 - v) > (vperiod/2)) {
		    vsens++;
		  }
		  else
		    if((v - V0) > (vperiod/2)) {
		      vsens--;
		    }
      }
		U0 = u; V0 = v;
		U1 = U0 + usens*uperiod;
		V1 = V0 + vsens*vperiod;
		Pts2d(i).SetCoord(U1,V1);
		myProjIsDone = Standard_True;
	      }
	    }
	  }
	}
	else break;
    }    
  }
  // -- Pnts2d is transformed into Geom2d_BSplineCurve, with the help of Param and Mult
  if(myProjIsDone) {
    myBSpline = new Geom2d_BSplineCurve(Pts2d,Param,Mult,1);
    //jgv: put the curve into parametric range
    gp_Pnt2d MidPoint = myBSpline->Value(0.5*(myBSpline->FirstParameter() + myBSpline->LastParameter()));
    Standard_Real TestU = MidPoint.X(), TestV = MidPoint.Y();
    Standard_Real sense = 0.;
    if (uperiod)
    {
      if (TestU < Uinf - TolU)
        sense = 1.;
      else if (TestU > Usup + TolU)
        sense = -1;
      while (TestU < Uinf - TolU || TestU > Usup + TolU)
        TestU += sense * uperiod;
    }
    if (vperiod)
    {
      sense = 0.;
      if (TestV < Vinf - TolV)
        sense = 1.;
      else if (TestV > Vsup + TolV)
        sense = -1.;
      while (TestV < Vinf - TolV || TestV > Vsup + TolV)
        TestV += sense * vperiod;
    }
    gp_Vec2d Offset(TestU - MidPoint.X(), TestV - MidPoint.Y());
    if (Abs(Offset.X()) > gp::Resolution() ||
        Abs(Offset.Y()) > gp::Resolution())
      myBSpline->Translate(Offset);
    //////////////////////////////////////////
    Geom2dAdaptor_Curve GAC(myBSpline);
    Handle(Adaptor2d_HCurve2d) IC2d = new Geom2dAdaptor_HCurve(GAC);
#ifdef OCCT_DEBUG
//    char name [100];
//    sprintf(name,"%s_%d","build",compteur++);
//    DrawTrSurf::Set(name,myBSpline);
#endif
    return IC2d;
  }
  else {
//  Modified by Sergey KHROMOV - Thu Apr 18 10:57:50 2002 Begin
//     Standard_NoSuchObject_Raise_if(1,"ProjLib_Compu: build echec");
//  Modified by Sergey KHROMOV - Thu Apr 18 10:57:51 2002 End
    return Handle(Adaptor2d_HCurve2d)();
  }
//  myProjIsDone = Standard_False;
//  Modified by Sergey KHROMOV - Thu Apr 18 10:58:01 2002 Begin
//   Standard_NoSuchObject_Raise_if(1,"ProjLib_ComputeOnPS: build echec");
//  Modified by Sergey KHROMOV - Thu Apr 18 10:58:02 2002 End
}

//=======================================================================
//function : ProjLib_ProjectUsingInitialCurve2d
//purpose  : 
//=======================================================================
Handle(Geom2d_BSplineCurve) 
     ProjLib_ComputeApproxOnPolarSurface::
     ProjectUsingInitialCurve2d(const Handle(Adaptor3d_HCurve)& Curve,
				const Handle(Adaptor3d_HSurface)& Surf,
				const Handle(Adaptor2d_HCurve2d)& InitCurve2d)
{  
  //OCC217
  Standard_Real Tol3d = myTolerance;
  Standard_Real DistTol3d = 100.0*Tol3d;
  if(myMaxDist > 0.)
  {
    DistTol3d = myMaxDist;
  }
  Standard_Real DistTol3d2 = DistTol3d * DistTol3d;
  Standard_Real TolU = Surf->UResolution(Tol3d), TolV = Surf->VResolution(Tol3d);
  Standard_Real Tol2d = Max(Sqrt(TolU*TolU + TolV*TolV), Precision::PConfusion());

  Standard_Integer i;
  GeomAbs_SurfaceType TheTypeS = Surf->GetType();
  GeomAbs_CurveType TheTypeC = Curve->GetType();
  if(TheTypeS == GeomAbs_Plane) {
    Standard_Real S, T;
    gp_Pln Plane = Surf->Plane();
    if(TheTypeC == GeomAbs_BSplineCurve) {
      myTolReached = Precision::Confusion();
      Handle(Geom_BSplineCurve) BSC = Curve->BSpline();
      TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
      for(i = 1;i <= Curve->NbPoles();i++) {
	ElSLib::Parameters( Plane, BSC->Pole(i), S, T);
	Poles2d(i).SetCoord(S,T);
      }
      TColStd_Array1OfReal Knots(1, BSC->NbKnots());
      BSC->Knots(Knots);
      TColStd_Array1OfInteger Mults(1, BSC->NbKnots());
      BSC->Multiplicities(Mults);
      if(BSC->IsRational()) {
	TColStd_Array1OfReal Weights(1, BSC->NbPoles());
	BSC->Weights(Weights); 
	return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
				       BSC->Degree(), BSC->IsPeriodic()) ;
      }
      return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
				     BSC->Degree(), BSC->IsPeriodic()) ;
      
    }
    if(TheTypeC == GeomAbs_BezierCurve) {
      myTolReached = Precision::Confusion();
      Handle(Geom_BezierCurve) BC = Curve->Bezier();
      TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
      for(i = 1;i <= Curve->NbPoles();i++) {
	ElSLib::Parameters( Plane, BC->Pole(i), S, T);
	Poles2d(i).SetCoord(S,T);
      }
      TColStd_Array1OfReal Knots(1, 2);
      Knots.SetValue(1,0.0);
      Knots.SetValue(2,1.0);
      TColStd_Array1OfInteger Mults(1, 2);
      Mults.Init(BC->NbPoles());
      if(BC->IsRational()) {
	TColStd_Array1OfReal Weights(1, BC->NbPoles());
	BC->Weights(Weights); 
	return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
				       BC->Degree(), BC->IsPeriodic()) ;
      }
      return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
				     BC->Degree(), BC->IsPeriodic()) ;
    }
  }
  if(TheTypeS == GeomAbs_BSplineSurface) {
    Handle(Geom_BSplineSurface) BSS = Surf->BSpline();
    if((BSS->MaxDegree() == 1) &&
       (BSS->NbUPoles() == 2) &&
       (BSS->NbVPoles() == 2)) {
      gp_Pnt p11 = BSS->Pole(1,1);
      gp_Pnt p12 = BSS->Pole(1,2);
      gp_Pnt p21 = BSS->Pole(2,1);
      gp_Pnt p22 = BSS->Pole(2,2);
      gp_Vec V1(p11,p12);
      gp_Vec V2(p21,p22);
      if(V1.IsEqual(V2,Tol3d,Tol3d/(p11.Distance(p12)*180/M_PI))){  
	Standard_Integer Dist2Min = IntegerLast();
	Standard_Real u,v;
	if(TheTypeC == GeomAbs_BSplineCurve) {
          myTolReached = Tol3d;
	  Handle(Geom_BSplineCurve) BSC = Curve->BSpline();
	  TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
	  for(i = 1;i <= Curve->NbPoles();i++) {
	    myProjIsDone = Standard_False;
	    Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS  extrloc(Surf->Surface(), TolU, TolV);
            extrloc.Perform(BSC->Pole(i), (p11.X()+p22.X())/2, (p11.Y()+p22.Y())/2);

	    if (extrloc.IsDone()) {
	      Dist2Min = (Standard_Integer ) extrloc.SquareDistance();
	      if (Dist2Min < DistTol3d2) {  
		(extrloc.Point()).Parameter(u,v);
		Poles2d(i).SetCoord(u,v);
		myProjIsDone = Standard_True;
	      }
	      else break;
	    }
	    else break;
	    if(!myProjIsDone) 
	      break;
	  }
	  if(myProjIsDone) {
	    TColStd_Array1OfReal Knots(1, BSC->NbKnots());
	    BSC->Knots(Knots);
	    TColStd_Array1OfInteger Mults(1, BSC->NbKnots());
	    BSC->Multiplicities(Mults);
	    if(BSC->IsRational()) {
	      TColStd_Array1OfReal Weights(1, BSC->NbPoles());
	      BSC->Weights(Weights); 
	      return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
					     BSC->Degree(), BSC->IsPeriodic()) ;
	    }
	    return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
					   BSC->Degree(), BSC->IsPeriodic()) ;
	    
	    
	  }
	} 
	if(TheTypeC == GeomAbs_BezierCurve) {
          myTolReached = Tol3d;
	  Handle(Geom_BezierCurve) BC = Curve->Bezier();
	  TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
	  for(i = 1;i <= Curve->NbPoles();i++) {
	    Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS  extrloc(Surf->Surface(), TolU, TolV);
            extrloc.Perform(BC->Pole(i), 0.5, 0.5);

	    if (extrloc.IsDone()) {
	      Dist2Min = (Standard_Integer ) extrloc.SquareDistance();
	      if (Dist2Min < DistTol3d2) {  
		(extrloc.Point()).Parameter(u,v);
		Poles2d(i).SetCoord(u,v);
		myProjIsDone = Standard_True;
	      }
	      else break;
	    }
	    else break;
	    if(myProjIsDone) 
	      myProjIsDone = Standard_False;
	    else break;
	  }
	  if(myProjIsDone) {
	    TColStd_Array1OfReal Knots(1, 2);
	    Knots.SetValue(1,0.0);
	    Knots.SetValue(2,1.0);
	    TColStd_Array1OfInteger Mults(1, 2);
	    Mults.Init(BC->NbPoles());
	    if(BC->IsRational()) {
	      TColStd_Array1OfReal Weights(1, BC->NbPoles());
	      BC->Weights(Weights); 
	      return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
						    BC->Degree(), BC->IsPeriodic()) ;
	    }
	    return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
						  BC->Degree(), BC->IsPeriodic()) ;
	  }
	} 
      }
    }
  }
  else if(TheTypeS == GeomAbs_BezierSurface) {
    Handle(Geom_BezierSurface) BS = Surf->Bezier();
    if((BS->MaxDegree() == 1) &&
       (BS->NbUPoles() == 2) &&
       (BS->NbVPoles() == 2)) {
      gp_Pnt p11 = BS->Pole(1,1);
      gp_Pnt p12 = BS->Pole(1,2);
      gp_Pnt p21 = BS->Pole(2,1);
      gp_Pnt p22 = BS->Pole(2,2);
      gp_Vec V1(p11,p12);
      gp_Vec V2(p21,p22);
      if(V1.IsEqual(V2,Tol3d,Tol3d/(p11.Distance(p12)*180/M_PI))){ 
	Standard_Integer Dist2Min = IntegerLast();
	Standard_Real u,v;
 
//	gp_Pnt pntproj;
	if(TheTypeC == GeomAbs_BSplineCurve) {
          myTolReached = Tol3d;
	  Handle(Geom_BSplineCurve) BSC = Curve->BSpline();
 	  TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
	  for(i = 1;i <= Curve->NbPoles();i++) {
	    myProjIsDone = Standard_False;
	    Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS  extrloc(Surf->Surface(), TolU, TolV);
            extrloc.Perform(BSC->Pole(i), (p11.X()+p22.X())/2, (p11.Y()+p22.Y())/2);

	    if (extrloc.IsDone()) {
	      Dist2Min = (Standard_Integer ) extrloc.SquareDistance();
	      if (Dist2Min < DistTol3d2) {  
		(extrloc.Point()).Parameter(u,v);
		Poles2d(i).SetCoord(u,v);
		myProjIsDone = Standard_True;
	      }
	      else break;
	    }
	    else break;
	    if(!myProjIsDone) 
	      break;
	  }
	  if(myProjIsDone) {
	    TColStd_Array1OfReal Knots(1, BSC->NbKnots());
	    BSC->Knots(Knots);
	    TColStd_Array1OfInteger Mults(1, BSC->NbKnots());
	    BSC->Multiplicities(Mults);
	    if(BSC->IsRational()) {
	      TColStd_Array1OfReal Weights(1, BSC->NbPoles());
	      BSC->Weights(Weights); 
	      return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
						    BSC->Degree(), BSC->IsPeriodic()) ;
	    }
	    return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
						  BSC->Degree(), BSC->IsPeriodic()) ;
	    
	    
	  }
	} 
	if(TheTypeC == GeomAbs_BezierCurve) {
          myTolReached = Tol3d;
	  Handle(Geom_BezierCurve) BC = Curve->Bezier();
	  TColgp_Array1OfPnt2d Poles2d(1,Curve->NbPoles());
	  for(i = 1;i <= Curve->NbPoles();i++) {
	    Dist2Min = IntegerLast();

            Extrema_GenLocateExtPS  extrloc(Surf->Surface(), TolU, TolV);
            extrloc.Perform(BC->Pole(i), 0.5, 0.5);

	    if (extrloc.IsDone()) {
	      Dist2Min = (Standard_Integer ) extrloc.SquareDistance();
	      if (Dist2Min < DistTol3d2) {  
		(extrloc.Point()).Parameter(u,v);
		Poles2d(i).SetCoord(u,v);
		myProjIsDone = Standard_True;
	      }
	      else break;
	    }
	    else break;
	    if(myProjIsDone) 
	      myProjIsDone = Standard_False;
	    else break;
	  }
	  if(myProjIsDone) {
	    TColStd_Array1OfReal Knots(1, 2);
	    Knots.SetValue(1,0.0);
	    Knots.SetValue(2,1.0);
	    TColStd_Array1OfInteger Mults(1, 2);
	    Mults.Init(BC->NbPoles());
	    if(BC->IsRational()) {
	      TColStd_Array1OfReal Weights(1, BC->NbPoles());
	      BC->Weights(Weights); 
	      return new Geom2d_BSplineCurve(Poles2d, Weights, Knots, Mults,
						    BC->Degree(), BC->IsPeriodic()) ;
	    }
	    return new Geom2d_BSplineCurve(Poles2d, Knots, Mults,
						  BC->Degree(), BC->IsPeriodic()) ;
	  }
	} 
      }
    }
  }

  ProjLib_PolarFunction F(Curve, Surf, InitCurve2d, Tol3d) ;  

#ifdef OCCT_DEBUG
  Standard_Integer Nb = 50;
  
  Standard_Real U, U1, U2;
  U1 = F.FirstParameter();
  U2 = F.LastParameter();
  
  TColgp_Array1OfPnt2d    DummyPoles(1,Nb+1);
  TColStd_Array1OfReal    DummyKnots(1,Nb+1);
  TColStd_Array1OfInteger DummyMults(1,Nb+1);
  DummyMults.Init(1);
  DummyMults(1) = 2;
  DummyMults(Nb+1) = 2;
  for (Standard_Integer ij = 0; ij <= Nb; ij++) {
    U = (Nb-ij)*U1 + ij*U2;
    U /= Nb;
    DummyPoles(ij+1) = F.Value(U);
    DummyKnots(ij+1) = ij;
  }
  Handle(Geom2d_BSplineCurve) DummyC2d =
    new Geom2d_BSplineCurve(DummyPoles, DummyKnots, DummyMults, 1);
#ifdef DRAW
  Standard_CString Temp = "bs2d";
  DrawTrSurf::Set(Temp,DummyC2d);
#endif
//  DrawTrSurf::Set((Standard_CString ) "bs2d",DummyC2d);
  Handle(Geom2dAdaptor_HCurve) DDD = 
    Handle(Geom2dAdaptor_HCurve)::DownCast(InitCurve2d);
  
#ifdef DRAW
  Temp = "initc2d";
  DrawTrSurf::Set(Temp,DDD->ChangeCurve2d().Curve());
#endif
//  DrawTrSurf::Set((Standard_CString ) "initc2d",DDD->ChangeCurve2d().Curve());
#endif

  Standard_Integer Deg1,Deg2;
  Deg1 = 2; 
  Deg2 = 8;  
  if(myDegMin > 0)
  {
    Deg1 = myDegMin;
  }
  if(myDegMax > 0)
  {
    Deg2 = myDegMax;
  }
  Standard_Integer aMaxSegments = 1000;
  if(myMaxSegments > 0)
  {
    aMaxSegments = myMaxSegments;
  }
  AppParCurves_Constraint aFistC = AppParCurves_TangencyPoint, aLastC = AppParCurves_TangencyPoint;
  if(myBndPnt != AppParCurves_TangencyPoint)
  {
    aFistC = myBndPnt; 
    aLastC = myBndPnt;
  }

  Approx_FitAndDivide2d Fit(Deg1, Deg2, Tol3d, Tol2d, Standard_True, aFistC, aLastC);
  Fit.SetMaxSegments(aMaxSegments);
  Fit.Perform(F);

  Standard_Real anOldTol2d = Tol2d;
  Standard_Real aNewTol2d = 0;
  if(Fit.IsAllApproximated()) {
    Standard_Integer j;
    Standard_Integer NbCurves = Fit.NbMultiCurves();
    Standard_Integer MaxDeg = 0;
    // To transform the MultiCurve into BSpline, it is required that all  
    // Bezier constituing it have the same degree -> Calculation of MaxDeg
    Standard_Integer NbPoles  = 1;
    for (j = 1; j <= NbCurves; j++) {
      Standard_Integer Deg = Fit.Value(j).Degree();
      MaxDeg = Max ( MaxDeg, Deg);
      Fit.Error(j,Tol3d, Tol2d);
      aNewTol2d = Max(aNewTol2d, Tol2d);
    }
    //
    myTolReached = Max(myTolReached, myTolerance * (aNewTol2d / anOldTol2d));
    //
    NbPoles = MaxDeg * NbCurves + 1;               //Tops on the BSpline
    TColgp_Array1OfPnt2d  Poles( 1, NbPoles);
      
    TColgp_Array1OfPnt2d TempPoles( 1, MaxDeg + 1);//to augment the degree
    
    TColStd_Array1OfReal Knots( 1, NbCurves + 1);  //Nodes of the BSpline
    
    Standard_Integer Compt = 1;
    for (i = 1; i <= NbCurves; i++) {
      Fit.Parameters(i, Knots(i), Knots(i+1)); 
      AppParCurves_MultiCurve MC = Fit.Value( i);       //Load the Ith Curve
      TColgp_Array1OfPnt2d Poles2d( 1, MC.Degree() + 1);//Retrieve the tops
      MC.Curve(1, Poles2d);
      
      //Eventual augmentation of the degree
      Standard_Integer Inc = MaxDeg - MC.Degree();
      if ( Inc > 0) {
//	BSplCLib::IncreaseDegree( Inc, Poles2d, PLib::NoWeights(), 
	BSplCLib::IncreaseDegree( MaxDeg, Poles2d, BSplCLib::NoWeights(), 
			 TempPoles, BSplCLib::NoWeights());
	//update of tops of the PCurve
	for (Standard_Integer k = 1 ; k <= MaxDeg + 1; k++) {
	  Poles.SetValue( Compt, TempPoles( k));
	  Compt++;
	}
      }
      else {
	//update of tops of the PCurve
	for (Standard_Integer k = 1 ; k <= MaxDeg + 1; k++) {
	  Poles.SetValue( Compt, Poles2d( k));
	  Compt++;
	}
      } 
      
      Compt--;
    }
    
    //update of fields of  ProjLib_Approx
    Standard_Integer NbKnots = NbCurves + 1;

    TColStd_Array1OfInteger   Mults( 1, NbKnots);
    Mults.Init(MaxDeg);
    Mults.SetValue( 1, MaxDeg + 1);
    Mults.SetValue(NbKnots, MaxDeg + 1);
    myProjIsDone = Standard_True;
    Handle(Geom2d_BSplineCurve) Dummy =
      new Geom2d_BSplineCurve(Poles,Knots,Mults,MaxDeg);
    
    // try to smoother the Curve GeomAbs_C1.

    Standard_Boolean OK = Standard_True;
    Standard_Real aSmoothTol = Max(Precision::Confusion(), aNewTol2d);
    for (Standard_Integer ij = 2; ij < NbKnots; ij++) {
      OK = OK && Dummy->RemoveKnot(ij,MaxDeg-1, aSmoothTol);  
    }
#ifdef OCCT_DEBUG
    if (!OK) {
      cout << "ProjLib_ComputeApproxOnPolarSurface : Smoothing echoue"<<endl;
    }
#endif 
    return Dummy;
  }
  return Handle(Geom2d_BSplineCurve)();
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve)  
     ProjLib_ComputeApproxOnPolarSurface::BSpline() const 
     
{
  return myBSpline ;
}

//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)  
     ProjLib_ComputeApproxOnPolarSurface::Curve2d() const 
     
{
  Standard_NoSuchObject_Raise_if
    (!myProjIsDone,
     "ProjLib_ComputeApproxOnPolarSurface:2ndCurve2d");
  return my2ndCurve ;
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ComputeApproxOnPolarSurface::IsDone() const 
     
{
  return myProjIsDone;
}
//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void ProjLib_ComputeApproxOnPolarSurface::SetTolerance(const Standard_Real theTol) 
     
{
  myTolerance = theTol;
}
//=======================================================================
//function : SetDegree
//purpose  : 
//=======================================================================
void ProjLib_ComputeApproxOnPolarSurface::SetDegree(
                                       const Standard_Integer theDegMin, 
                                       const Standard_Integer theDegMax)
{
  myDegMin = theDegMin;
  myDegMax = theDegMax;
}
//=======================================================================
//function : SetMaxSegments
//purpose  : 
//=======================================================================
void ProjLib_ComputeApproxOnPolarSurface::SetMaxSegments(
                                   const Standard_Integer theMaxSegments)
{
  myMaxSegments = theMaxSegments;
}

//=======================================================================
//function : SetBndPnt
//purpose  : 
//=======================================================================
void ProjLib_ComputeApproxOnPolarSurface::SetBndPnt(
                                 const AppParCurves_Constraint theBndPnt)
{
  myBndPnt = theBndPnt;
}

//=======================================================================
//function : SetMaxDist
//purpose  : 
//=======================================================================
void ProjLib_ComputeApproxOnPolarSurface::SetMaxDist(
                                          const Standard_Real theMaxDist)
{
  myMaxDist = theMaxDist;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ComputeApproxOnPolarSurface::Tolerance() const 
     
{
  return myTolReached;
}

