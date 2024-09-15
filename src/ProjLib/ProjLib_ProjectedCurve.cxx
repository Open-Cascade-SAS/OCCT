// Created on: 1993-08-25
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

//  Modified by skv - Wed Aug 11 15:45:58 2004 OCC6272

#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <ProjLib_ComputeApprox.hxx>
#include <ProjLib_Projector.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <ProjLib_Plane.hxx>
#include <ProjLib_Cylinder.hxx>
#include <ProjLib_Cone.hxx>
#include <ProjLib_Sphere.hxx>
#include <ProjLib_Torus.hxx>
#include <Precision.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <gp_Vec2d.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2dConvert.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <NCollection_DataMap.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ProjLib_ProjectedCurve, Adaptor2d_Curve2d)

//=======================================================================
//function : ComputeTolU
//purpose  : 
//=======================================================================

static Standard_Real ComputeTolU(const Handle(Adaptor3d_Surface)& theSurf,
                                 const Standard_Real theTolerance)
{
  Standard_Real aTolU = theSurf->UResolution(theTolerance);
  if (theSurf->IsUPeriodic())
  {
    aTolU = Min(aTolU, 0.01*theSurf->UPeriod());
  }

  return aTolU;
}

//=======================================================================
//function : ComputeTolV
//purpose  : 
//=======================================================================

static Standard_Real ComputeTolV(const Handle(Adaptor3d_Surface)& theSurf,
                                 const Standard_Real theTolerance)
{
  Standard_Real aTolV = theSurf->VResolution(theTolerance);
  if (theSurf->IsVPeriodic())
  {
    aTolV = Min(aTolV, 0.01*theSurf->VPeriod());
  }

  return aTolV;
}

//=======================================================================
//function : IsoIsDeg
//purpose  : 
//=======================================================================

static Standard_Boolean IsoIsDeg  (const Adaptor3d_Surface& S,
				   const Standard_Real      Param,
				   const GeomAbs_IsoType    IT,
				   const Standard_Real      TolMin,
				   const Standard_Real      TolMax) 
{
    Standard_Real U1=0.,U2=0.,V1=0.,V2=0.,T;
    Standard_Boolean Along = Standard_True;
    U1 = S.FirstUParameter();
    U2 = S.LastUParameter();
    V1 = S.FirstVParameter();
    V2 = S.LastVParameter();
    gp_Vec D1U,D1V;
    gp_Pnt P;
    Standard_Real Step,D1NormMax;
    if (IT == GeomAbs_IsoV) 
    {
      Step = (U2 - U1)/10;
      D1NormMax=0.;
      for (T=U1;T<=U2;T=T+Step) 
      {
        S.D1(T,Param,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1U.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;
    }
    else 
    {
      Step = (V2 - V1)/10;
      D1NormMax=0.;
      for (T=V1;T<=V2;T=T+Step) 
      {
	S.D1(Param,T,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1V.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;


    }
    return Along;
}

//=======================================================================
//function : TrimC3d
//purpose  : 
//=======================================================================

static void TrimC3d(Handle(Adaptor3d_Curve)& myCurve,
                    Standard_Boolean* IsTrimmed,
                    const Standard_Real dt,
                    const gp_Pnt& Pole,
                    Standard_Integer* SingularCase,
                    const Standard_Integer NumberOfSingularCase,
                    const Standard_Real TolConf)
{
  Standard_Real f = myCurve->FirstParameter();
  Standard_Real l = myCurve->LastParameter();

  gp_Pnt P = myCurve->Value(f);

  if(P.Distance(Pole) <= TolConf) {
    IsTrimmed[0] = Standard_True;
    f = f+dt;
    myCurve = myCurve->Trim(f, l, Precision::Confusion());
    SingularCase[0] = NumberOfSingularCase;
  }
  
  P = myCurve->Value(l);
  if(P.Distance(Pole) <= TolConf) {
    IsTrimmed[1] = Standard_True;
    l = l-dt;
    myCurve = myCurve->Trim(f, l, Precision::Confusion());
    SingularCase[1] = NumberOfSingularCase;
  }
}

//=======================================================================
//function : ExtendC2d
//purpose  : 
//=======================================================================

static void ExtendC2d (Handle(Geom2d_BSplineCurve)& aRes,
                       const Standard_Real /*t*/,
                       const Standard_Real /*dt*/,
                       const Standard_Real u1,
                       const Standard_Real u2,
                       const Standard_Real v1,
                       const Standard_Real v2,
                       const Standard_Integer FirstOrLast,
                       const Standard_Integer NumberOfSingularCase)
{
  Standard_Real theParam = (FirstOrLast == 0)? aRes->FirstParameter()
    : aRes->LastParameter();

  gp_Pnt2d                              aPBnd;
  gp_Vec2d                              aVBnd;
  gp_Dir2d                              aDBnd;
  Handle(Geom2d_TrimmedCurve)           aSegment;
  Geom2dConvert_CompCurveToBSplineCurve aCompCurve(aRes, Convert_RationalC1);
  constexpr Standard_Real               aTol = Precision::Confusion();

  aRes->D1(theParam, aPBnd, aVBnd);
  aDBnd.SetXY(aVBnd.XY());
  gp_Lin2d aLin(aPBnd, aDBnd); //line in direction of derivative

  gp_Pnt2d thePole;
  gp_Dir2d theBoundDir;
  switch (NumberOfSingularCase)
  {
  case 1:
    {
      thePole.SetCoord(u1, v1);
      theBoundDir.SetCoord(0., 1.);
      break;
    }
  case 2:
    {
      thePole.SetCoord(u2, v1);
      theBoundDir.SetCoord(0., 1.);
      break;
    }
  case 3:
    {
      thePole.SetCoord(u1, v1);
      theBoundDir.SetCoord(1., 0.);
      break;
    }
  case 4:
    {
      thePole.SetCoord(u1, v2);
      theBoundDir.SetCoord(1., 0.);
      break;
    }
  }
  gp_Lin2d BoundLin(thePole, theBoundDir); //one of the bounds of rectangle
  Standard_Real ParOnLin = 0.;
  if (theBoundDir.IsParallel(aDBnd, 100.*Precision::Angular()))
  {
    ParOnLin = ElCLib::Parameter(aLin, thePole);
  }
  else
  {
    Standard_Real U1x = BoundLin.Direction().X();
    Standard_Real U1y = BoundLin.Direction().Y();
    Standard_Real U2x = aLin.Direction().X();
    Standard_Real U2y = aLin.Direction().Y();
    Standard_Real Uo21x = aLin.Location().X() - BoundLin.Location().X();
    Standard_Real Uo21y = aLin.Location().Y() - BoundLin.Location().Y();

    Standard_Real D = U1y*U2x - U1x*U2y;

    ParOnLin = (Uo21y * U1x - Uo21x * U1y) / D; //parameter of intersection point
  }
  
  Handle(Geom2d_Line) aSegLine = new Geom2d_Line(aLin);
  aSegment = (FirstOrLast == 0)?
    new Geom2d_TrimmedCurve(aSegLine, ParOnLin, 0.) :
    new Geom2d_TrimmedCurve(aSegLine, 0., ParOnLin);
  
  Standard_Boolean anAfter = FirstOrLast != 0;
  aCompCurve.Add(aSegment, aTol, anAfter);
  aRes = aCompCurve.BSplineCurve();
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

static void Project(ProjLib_Projector& P, Handle(Adaptor3d_Curve)& C)
{
  GeomAbs_CurveType CType = C->GetType();
  switch (CType) {
    case GeomAbs_Line:
      P.Project(C->Line());
      break;
    case GeomAbs_Circle:
      P.Project(C->Circle());
      break;
    case GeomAbs_Ellipse:
      P.Project(C->Ellipse());
      break;
    case GeomAbs_Hyperbola:
      P.Project(C->Hyperbola());
      break;
    case GeomAbs_Parabola:
      P.Project(C->Parabola());
      break;
    case GeomAbs_BSplineCurve:
    case GeomAbs_BezierCurve:
    case GeomAbs_OffsetCurve:
    case GeomAbs_OtherCurve:    // try the approximation
      break;
    default:
      throw Standard_NoSuchObject(" ");
  }
}

//=======================================================================
//function : ProjLib_ProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_ProjectedCurve::ProjLib_ProjectedCurve() :
  myTolerance(Precision::Confusion()),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
}


//=======================================================================
//function : ProjLib_ProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_ProjectedCurve::ProjLib_ProjectedCurve
(const Handle(Adaptor3d_Surface)& S) :
  myTolerance(Precision::Confusion()),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  Load(S);
}


//=======================================================================
//function : ProjLib_ProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_ProjectedCurve::ProjLib_ProjectedCurve
(const Handle(Adaptor3d_Surface)& S,
 const Handle(Adaptor3d_Curve)& C) :
  myTolerance(Precision::Confusion()),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  Load(S);
  Perform(C);
}


//=======================================================================
//function : ProjLib_ProjectedCurve
//purpose  : 
//=======================================================================

ProjLib_ProjectedCurve::ProjLib_ProjectedCurve
(const Handle(Adaptor3d_Surface)& S,
 const Handle(Adaptor3d_Curve)&   C,
 const Standard_Real             Tol) :
  myTolerance(Max(Tol, Precision::Confusion())),
  myDegMin(-1), myDegMax(-1),
  myMaxSegments(-1),
  myMaxDist(-1.),
  myBndPnt(AppParCurves_TangencyPoint)
{
  Load(S);
  Perform(C);
}

//=======================================================================
//function : ShallowCopy
//purpose  : 
//=======================================================================

Handle(Adaptor2d_Curve2d) ProjLib_ProjectedCurve::ShallowCopy() const
{
  Handle(ProjLib_ProjectedCurve) aCopy = new ProjLib_ProjectedCurve();

  aCopy->myTolerance   = myTolerance;
  if (!mySurface.IsNull())
  {
    aCopy->mySurface = mySurface->ShallowCopy();
  }
  if (!myCurve.IsNull())
  {
    aCopy->myCurve = myCurve->ShallowCopy();
  }
  aCopy->myResult      = myResult;
  aCopy->myDegMin      = myDegMin;
  aCopy->myDegMax      = myDegMax;
  aCopy->myMaxSegments = myMaxSegments;
  aCopy->myMaxDist     = myMaxDist;
  aCopy->myBndPnt      = myBndPnt;

  return aCopy;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::Load(const Handle(Adaptor3d_Surface)& S)
{
  mySurface = S ;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::Load(const Standard_Real theTol)
{
  myTolerance = theTol;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::Perform(const Handle(Adaptor3d_Curve)& C)
{
  myTolerance = Max(myTolerance, Precision::Confusion());
  myCurve = C;
  Standard_Real FirstPar = C->FirstParameter();
  Standard_Real LastPar  = C->LastParameter();
  GeomAbs_SurfaceType SType = mySurface->GetType();
  GeomAbs_CurveType   CType = myCurve->GetType();
  Standard_Boolean isAnalyticalSurf = Standard_True;
  Standard_Boolean IsTrimmed[2] = { Standard_False, Standard_False };
  Standard_Integer SingularCase[2];
  const Standard_Real eps = 0.01;
  Standard_Real TolConf = Precision::Confusion();
  Standard_Real dt = (LastPar - FirstPar) * eps;
  Standard_Real U1 = 0.0, U2 = 0.0, V1 = 0.0, V2 = 0.0;
  U1 = mySurface->FirstUParameter();
  U2 = mySurface->LastUParameter();
  V1 = mySurface->FirstVParameter();
  V2 = mySurface->LastVParameter();

  switch (SType)
  {
    case GeomAbs_Plane:
      {
        ProjLib_Plane P(mySurface->Plane());
        Project(P,myCurve);
        myResult = P;
      }
      break;

    case GeomAbs_Cylinder:
      {
        ProjLib_Cylinder P(mySurface->Cylinder());
        Project(P,myCurve);
        myResult = P;
      }
      break;

    case GeomAbs_Cone:
      {
        ProjLib_Cone P(mySurface->Cone());
        Project(P,myCurve);
        myResult = P;
      }
      break;

    case GeomAbs_Sphere:
      {
        ProjLib_Sphere P(mySurface->Sphere());
        Project(P,myCurve);
        if ( P.IsDone())
        {
          // on met dans la pseudo-periode ( car Sphere n'est pas
          // periodique en V !)
          P.SetInBounds(myCurve->FirstParameter());
        }
        else
        {
          const Standard_Real Vmax = M_PI / 2.;
          const Standard_Real Vmin = -Vmax;
          const Standard_Real minang = 1.e-5 * M_PI;
          gp_Sphere aSph = mySurface->Sphere();
          Standard_Real anR = aSph.Radius();
          Standard_Real f = myCurve->FirstParameter();
          Standard_Real l = myCurve->LastParameter();

          gp_Pnt Pf = myCurve->Value(f);
          gp_Pnt Pl = myCurve->Value(l);
          gp_Pnt aLoc = aSph.Position().Location();
          Standard_Real maxdist = Max(Pf.Distance(aLoc), Pl.Distance(aLoc));
          TolConf = Max(anR * minang, Abs(anR - maxdist));

          //Surface has pole at V = Vmin and Vmax
          gp_Pnt Pole = mySurface->Value(U1, Vmin);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 3, TolConf);
          Pole = mySurface->Value(U1, Vmax);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 4, TolConf);
        }
        myResult = P;
      }
      break;

    case GeomAbs_Torus:
      {
        ProjLib_Torus P(mySurface->Torus());
        Project(P,myCurve);
        myResult = P;
      }
      break;

    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface:
      {
        isAnalyticalSurf = Standard_False;
        Standard_Real f, l;
        f = myCurve->FirstParameter();
        l = myCurve->LastParameter();
        dt = (l - f) * eps;

        const Adaptor3d_Surface& S = *mySurface;
        U1 = S.FirstUParameter();
        U2 = S.LastUParameter();
        V1 = S.FirstVParameter();
        V2 = S.LastVParameter();

        if(IsoIsDeg(S, U1, GeomAbs_IsoU, 0., myTolerance))
        {
          //Surface has pole at U = Umin
          gp_Pnt Pole = mySurface->Value(U1, V1);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 1, TolConf);
        }

        if(IsoIsDeg(S, U2, GeomAbs_IsoU, 0., myTolerance))
        {
          //Surface has pole at U = Umax
          gp_Pnt Pole = mySurface->Value(U2, V1);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 2, TolConf);
        }

        if(IsoIsDeg(S, V1, GeomAbs_IsoV, 0., myTolerance))
        {
          //Surface has pole at V = Vmin
          gp_Pnt Pole = mySurface->Value(U1, V1);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 3, TolConf);
        }

        if(IsoIsDeg(S, V2, GeomAbs_IsoV, 0., myTolerance))
        {
          //Surface has pole at V = Vmax
          gp_Pnt Pole = mySurface->Value(U1, V2);
          TrimC3d(myCurve, IsTrimmed, dt, Pole, SingularCase, 4, TolConf);
        }

        ProjLib_ComputeApproxOnPolarSurface polar;
        polar.SetTolerance(myTolerance);
        polar.SetDegree(myDegMin, myDegMax);
        polar.SetMaxSegments(myMaxSegments);
        polar.SetBndPnt(myBndPnt);
        polar.SetMaxDist(myMaxDist);
        polar.Perform(myCurve, mySurface); 

        Handle(Geom2d_BSplineCurve) aRes = polar.BSpline();

        if (!aRes.IsNull())
        {
          myTolerance = polar.Tolerance();
          if( (IsTrimmed[0] || IsTrimmed[1]))
          {
            if(IsTrimmed[0])
            {
              //Add segment before start of curve
              f = myCurve->FirstParameter();
              ExtendC2d(aRes, f, -dt, U1, U2, V1, V2, 0, SingularCase[0]);
            }
            if(IsTrimmed[1])
            {
              //Add segment after end of curve
              l = myCurve->LastParameter();
              ExtendC2d(aRes, l,  dt, U1, U2, V1, V2, 1, SingularCase[1]);
            }
            Handle(Geom2d_Curve) NewCurve2d;
            GeomLib::SameRange(Precision::PConfusion(), aRes,
              aRes->FirstParameter(), aRes->LastParameter(),
              FirstPar, LastPar, NewCurve2d);
            aRes = Handle(Geom2d_BSplineCurve)::DownCast(NewCurve2d);
          }
          myResult.SetBSpline(aRes);
          myResult.Done();
          myResult.SetType(GeomAbs_BSplineCurve);
        }
      }
      break;

    default:
      {
        isAnalyticalSurf = Standard_False;
        Standard_Real Vsingular[2] = {0.0 , 0.0}; //for surfaces of revolution
        Standard_Real f = 0.0, l = 0.0;
        dt = 0.0;

        if(mySurface->GetType() == GeomAbs_SurfaceOfRevolution)
        {
          //Check possible singularity

          gp_Pnt P = mySurface->AxeOfRevolution().Location();
          gp_Dir N = mySurface->AxeOfRevolution().Direction();

          gp_Lin L(P, N);

          f = myCurve->FirstParameter();
          l = myCurve->LastParameter();
          dt = (l - f) * eps;

          P = myCurve->Value(f);
          if(L.Distance(P) < Precision::Confusion())
          {
            IsTrimmed[0] = Standard_True;
            f = f + dt;
            myCurve = myCurve->Trim(f, l, Precision::Confusion());
            // Searching the parameter on the basis curve for surface of revolution
            Extrema_ExtPC anExtr(P, *mySurface->BasisCurve(), myTolerance);
            if (anExtr.IsDone())
            {
              Standard_Real aMinDist = RealLast();
              for(Standard_Integer anIdx = 1; anIdx <= anExtr.NbExt(); anIdx++)
              {
                if (anExtr.IsMin(anIdx) &&
                    anExtr.SquareDistance(anIdx) < aMinDist)
                {
                  aMinDist = anExtr.SquareDistance(anIdx);
                  Vsingular[0] = anExtr.Point(anIdx).Parameter();
                }
              }
            }
            else
              Vsingular[0] = ElCLib::Parameter(L, P);
            //SingularCase[0] = 3;
          }

          P = myCurve->Value(l);
          if(L.Distance(P) < Precision::Confusion())
          {
            IsTrimmed[1] = Standard_True;
            l = l - dt;
            myCurve = myCurve->Trim(f, l, Precision::Confusion());
            // Searching the parameter on the basis curve for surface of revolution
            Extrema_ExtPC anExtr(P, *mySurface->BasisCurve(), myTolerance);
            if (anExtr.IsDone())
            {
              Standard_Real aMinDist = RealLast();
              for(Standard_Integer anIdx = 1; anIdx <= anExtr.NbExt(); anIdx++)
              {
                if (anExtr.IsMin(anIdx) &&
                    anExtr.SquareDistance(anIdx) < aMinDist)
                {
                  aMinDist = anExtr.SquareDistance(anIdx);
                  Vsingular[1] = anExtr.Point(anIdx).Parameter();
                }
              }
            }
            else
              Vsingular[1] = ElCLib::Parameter(L, P);
            //SingularCase[1] = 4;
          }
        }

        Standard_Real aTolU = Max(ComputeTolU(mySurface, myTolerance), Precision::Confusion());
        Standard_Real aTolV = Max(ComputeTolV(mySurface, myTolerance), Precision::Confusion());
        Standard_Real aTol2d = Sqrt(aTolU*aTolU + aTolV*aTolV);

        Standard_Real aMaxDist = 100. * myTolerance;
        if(myMaxDist > 0.)
        {
          aMaxDist = myMaxDist;
        }
        Handle(ProjLib_HCompProjectedCurve) HProjector = new ProjLib_HCompProjectedCurve (mySurface,myCurve, aTolU, aTolV, aMaxDist);

        // Normalement, dans le cadre de ProjLib, le resultat 
        // doit etre une et une seule courbe !!!
        // De plus, cette courbe ne doit pas etre Single point
        Standard_Integer NbCurves = HProjector->NbCurves();
        Standard_Real Udeb = 0.0,Ufin = 0.0;
        if (NbCurves > 0)
        {
          HProjector->Bounds(1, Udeb, Ufin);
        }
        else 
        {
          return;
        }
        // Approximons cette courbe algorithmique.
        Standard_Boolean Only3d = Standard_False;
        Standard_Boolean Only2d = Standard_True;
        GeomAbs_Shape Continuity = GeomAbs_C1;
        if(myBndPnt == AppParCurves_PassPoint)
        {
          Continuity = GeomAbs_C0;
        }
        Standard_Integer MaxDegree = 14;
        if(myDegMax > 0)
        {
          MaxDegree = myDegMax;
        }
        Standard_Integer MaxSeg    = 16;
        if(myMaxSegments > 0)
        {
          MaxSeg = myMaxSegments;
        }

        Approx_CurveOnSurface appr(HProjector, mySurface, Udeb, Ufin, myTolerance);
        appr.Perform(MaxSeg, MaxDegree, Continuity, Only3d, Only2d);

        Handle(Geom2d_BSplineCurve) aRes = appr.Curve2d();

        if (!aRes.IsNull())
        {
          aTolU = appr.MaxError2dU();
          aTolV = appr.MaxError2dV();
          Standard_Real aNewTol2d = Sqrt(aTolU*aTolU + aTolV*aTolV);
          myTolerance *= (aNewTol2d / aTol2d);
          if(IsTrimmed[0] || IsTrimmed[1])
          {
            // Treatment only for surface of revolution
            Standard_Real u1, u2, v1, v2;
            u1 = mySurface->FirstUParameter();
            u2 = mySurface->LastUParameter();
            v1 = mySurface->FirstVParameter();
            v2 = mySurface->LastVParameter();

            if(IsTrimmed[0])
            {
              //Add segment before start of curve
              ExtendC2d(aRes, f, -dt, u1, u2, Vsingular[0], v2, 0, 3);
            }
            if(IsTrimmed[1])
            {
              //Add segment after end of curve
              ExtendC2d(aRes, l,  dt, u1, u2, v1, Vsingular[1], 1, 4);
            }
            Handle(Geom2d_Curve) NewCurve2d;
            GeomLib::SameRange(Precision::PConfusion(), aRes,
              aRes->FirstParameter(), aRes->LastParameter(),
              FirstPar, LastPar, NewCurve2d);
            aRes = Handle(Geom2d_BSplineCurve)::DownCast(NewCurve2d);
            if(Continuity == GeomAbs_C0)
            {
              // try to smoother the Curve GeomAbs_C1.
              Standard_Integer aDeg = aRes->Degree();
              Standard_Boolean OK = Standard_True;
              Standard_Real aSmoothTol = Max(Precision::Confusion(), aNewTol2d);
              for (Standard_Integer ij = 2; ij < aRes->NbKnots(); ij++) {
                OK = OK && aRes->RemoveKnot(ij, aDeg-1, aSmoothTol);  
              }
            }
          }

          myResult.SetBSpline(aRes);
          myResult.Done();
          myResult.SetType(GeomAbs_BSplineCurve);
        }
      }
  }

  if ( !myResult.IsDone() && isAnalyticalSurf)
  {
    // Use advanced analytical projector if base analytical projection failed.
    ProjLib_ComputeApprox Comp;
    Comp.SetTolerance(myTolerance);
    Comp.SetDegree(myDegMin, myDegMax);
    Comp.SetMaxSegments(myMaxSegments);
    Comp.SetBndPnt(myBndPnt);
    Comp.Perform(myCurve, mySurface);
    if (Comp.Bezier().IsNull() && Comp.BSpline().IsNull())
      return; // advanced projector has been failed too
    myResult.Done();
    Handle(Geom2d_BSplineCurve) aRes;
    if (Comp.BSpline().IsNull())
    {
      aRes = Geom2dConvert::CurveToBSplineCurve(Comp.Bezier());
    }
    else
    {
      aRes = Comp.BSpline();
    }
    if ((IsTrimmed[0] || IsTrimmed[1]))
    {
      if (IsTrimmed[0])
      {
        //Add segment before start of curve
        Standard_Real f = myCurve->FirstParameter();
        ExtendC2d(aRes, f, -dt, U1, U2, V1, V2, 0, SingularCase[0]);
      }
      if (IsTrimmed[1])
      {
        //Add segment after end of curve
        Standard_Real l = myCurve->LastParameter();
        ExtendC2d(aRes, l, dt, U1, U2, V1, V2, 1, SingularCase[1]);
      }
      Handle(Geom2d_Curve) NewCurve2d;
      GeomLib::SameRange(Precision::PConfusion(), aRes,
        aRes->FirstParameter(), aRes->LastParameter(),
        FirstPar, LastPar, NewCurve2d);
      aRes = Handle(Geom2d_BSplineCurve)::DownCast(NewCurve2d);
      myResult.SetBSpline(aRes);
      myResult.SetType(GeomAbs_BSplineCurve);
    }
    else
    {
      // set the type
      if (SType == GeomAbs_Plane && CType == GeomAbs_BezierCurve)
      {
        myResult.SetType(GeomAbs_BezierCurve);
        myResult.SetBezier(Comp.Bezier());
      }
      else
      {
        myResult.SetType(GeomAbs_BSplineCurve);
        myResult.SetBSpline(Comp.BSpline());
      }
    }
    // set the periodicity flag
    if (SType == GeomAbs_Plane        &&
      CType == GeomAbs_BSplineCurve &&
      myCurve->IsPeriodic())
    {
      myResult.SetPeriodic();
    }
    myTolerance = Comp.Tolerance();
  }

  Standard_Boolean isPeriodic[] = {mySurface->IsUPeriodic(),
                                   mySurface->IsVPeriodic()};
  if (myResult.IsDone() &&
     (isPeriodic[0] || isPeriodic[1]))
  {
    // Check result curve to be in params space.

    // U and V parameters space correspondingly.
    const Standard_Real aSurfFirstPar[2] = {mySurface->FirstUParameter(),
                                            mySurface->FirstVParameter()};
    Standard_Real aSurfPeriod[2] = {0.0, 0.0};
    if (isPeriodic[0])
      aSurfPeriod[0] = mySurface->UPeriod();
    if (isPeriodic[1])
      aSurfPeriod[1] = mySurface->VPeriod();

    for(Standard_Integer anIdx = 1; anIdx <= 2; anIdx++)
    {
      if (!isPeriodic[anIdx - 1])
        continue;

      if (myResult.GetType() == GeomAbs_BSplineCurve)
      {
        NCollection_DataMap<Standard_Integer, Standard_Integer> aMap; 
        Handle(Geom2d_BSplineCurve) aRes = myResult.BSpline();
        const Standard_Integer aDeg = aRes->Degree();

        for(Standard_Integer aKnotIdx = aRes->FirstUKnotIndex();
                             aKnotIdx < aRes->LastUKnotIndex();
                             aKnotIdx++)
        {
          const Standard_Real aFirstParam = aRes->Knot(aKnotIdx);
          const Standard_Real aLastParam  = aRes->Knot(aKnotIdx + 1);

          for(Standard_Integer anIntIdx = 0; anIntIdx <= aDeg; anIntIdx++)
          {
            const Standard_Real aCurrParam = aFirstParam + (aLastParam - aFirstParam) * anIntIdx / (aDeg + 1.0);
            gp_Pnt2d aPnt2d;
            aRes->D0(aCurrParam, aPnt2d);

            Standard_Integer aMapKey = Standard_Integer ((aPnt2d.Coord(anIdx) - aSurfFirstPar[anIdx - 1]) / aSurfPeriod[anIdx - 1]);

            if (aPnt2d.Coord(anIdx) - aSurfFirstPar[anIdx - 1] < 0.0)
              aMapKey--;

            if (aMap.IsBound(aMapKey))
              aMap.ChangeFind(aMapKey)++;
            else
              aMap.Bind(aMapKey, 1);
          }
        }

        Standard_Integer aMaxPoints = 0, aMaxIdx = 0;
        NCollection_DataMap<Standard_Integer, Standard_Integer>::Iterator aMapIter(aMap);
        for( ; aMapIter.More(); aMapIter.Next())
        {
          if (aMapIter.Value() > aMaxPoints)
          {
            aMaxPoints = aMapIter.Value();
            aMaxIdx = aMapIter.Key();
          }
        }
        if (aMaxIdx != 0)
        {
          gp_Pnt2d aFirstPnt = aRes->Value(aRes->FirstParameter());
          gp_Pnt2d aSecondPnt = aFirstPnt;
          aSecondPnt.SetCoord(anIdx, aFirstPnt.Coord(anIdx) - aSurfPeriod[anIdx - 1] * aMaxIdx);
          aRes->Translate(gp_Vec2d(aFirstPnt, aSecondPnt));
        }
      }

      if (myResult.GetType() == GeomAbs_Line)
      {
        Standard_Real aT1 = myCurve->FirstParameter();
        Standard_Real aT2 = myCurve->LastParameter();

        if (anIdx == 1)
        {
          // U param space.
          myResult.UFrame(aT1, aT2, aSurfFirstPar[anIdx - 1], aSurfPeriod[anIdx - 1]);
        }
        else
        {
          // V param space.
          myResult.VFrame(aT1, aT2, aSurfFirstPar[anIdx - 1], aSurfPeriod[anIdx - 1]);
        }
      }
    }
  }
}

//=======================================================================
//function : SetDegree
//purpose  : 
//=======================================================================
void ProjLib_ProjectedCurve::SetDegree(const Standard_Integer theDegMin, 
                                       const Standard_Integer theDegMax)
{
  myDegMin = theDegMin;
  myDegMax = theDegMax;
}
//=======================================================================
//function : SetMaxSegments
//purpose  : 
//=======================================================================
void ProjLib_ProjectedCurve::SetMaxSegments(const Standard_Integer theMaxSegments)
{
  myMaxSegments = theMaxSegments;
}

//=======================================================================
//function : SetBndPnt
//purpose  : 
//=======================================================================
void ProjLib_ProjectedCurve::SetBndPnt(const AppParCurves_Constraint theBndPnt)
{
  myBndPnt = theBndPnt;
}

//=======================================================================
//function : SetMaxDist
//purpose  : 
//=======================================================================
void ProjLib_ProjectedCurve::SetMaxDist(const Standard_Real theMaxDist)
{
  myMaxDist = theMaxDist;
}

//=======================================================================
//function : GetSurface
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_Surface)& ProjLib_ProjectedCurve::GetSurface() const
{
  return mySurface;
}


//=======================================================================
//function : GetCurve
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_Curve)& ProjLib_ProjectedCurve::GetCurve() const
{
  return myCurve;
}


//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectedCurve::GetTolerance() const 
{
  return myTolerance;
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectedCurve::FirstParameter() const 
{
  return myCurve->FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectedCurve::LastParameter() const 
{
  return myCurve->LastParameter();
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ProjLib_ProjectedCurve::Continuity() const
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Continuity() - method is not implemented");
}


//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectedCurve::NbIntervals(const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::NbIntervals() - method is not implemented");
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

//void ProjLib_ProjectedCurve::Intervals(TColStd_Array1OfReal&  T,
void ProjLib_ProjectedCurve::Intervals(TColStd_Array1OfReal&  ,
				       const GeomAbs_Shape ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Intervals() - method is not implemented");
}


//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectedCurve::IsClosed() const
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::IsClosed() - method is not implemented");
}


//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectedCurve::IsPeriodic() const
{
  return myResult.IsPeriodic();
}


//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectedCurve::Period() const
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Period() - method is not implemented");
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d ProjLib_ProjectedCurve::Value(const Standard_Real ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Value() - method is not implemented");
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::D0(const Standard_Real , gp_Pnt2d& ) const
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::D0() - method is not implemented");
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::D1(const Standard_Real ,
			              gp_Pnt2d&     , 
                                      gp_Vec2d&     ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::D1() - method is not implemented");
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::D2(const Standard_Real , 
			              gp_Pnt2d&     , 
                                      gp_Vec2d&     , 
                                      gp_Vec2d&     ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::D2() - method is not implemented");
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void ProjLib_ProjectedCurve::D3(const Standard_Real, 
				      gp_Pnt2d&, 
			              gp_Vec2d&, 
				      gp_Vec2d&, 
			              gp_Vec2d&) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::D3() - method is not implemented");
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec2d ProjLib_ProjectedCurve::DN(const Standard_Real, 
				    const Standard_Integer) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::DN() - method is not implemented");
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real ProjLib_ProjectedCurve::Resolution(const Standard_Real) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Resolution() - method is not implemented");
}
    

//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType ProjLib_ProjectedCurve::GetType() const
{
  return myResult.GetType();
}


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin2d ProjLib_ProjectedCurve::Line() const
{
  return myResult.Line();
}


//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ2d ProjLib_ProjectedCurve::Circle() const
{
  return myResult.Circle();
}


//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips2d ProjLib_ProjectedCurve::Ellipse() const
{
  return myResult.Ellipse();
}


//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr2d ProjLib_ProjectedCurve::Hyperbola() const
{
  return myResult.Hyperbola();
}


//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab2d ProjLib_ProjectedCurve::Parabola() const
{
  return myResult.Parabola();
}



//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectedCurve::Degree() const
{
  Standard_NoSuchObject_Raise_if 
    ( (GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve),
     "ProjLib_ProjectedCurve:Degree");
  if (GetType() == GeomAbs_BSplineCurve) {
    return myResult.BSpline()->Degree();
  }
  else if (GetType() == GeomAbs_BezierCurve) {
    return myResult.Bezier()->Degree();
  }

  // portage WNT
  return 0;
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean ProjLib_ProjectedCurve::IsRational() const 
{
  Standard_NoSuchObject_Raise_if 
    ( (GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve),
     "ProjLib_ProjectedCurve:IsRational");
  if (GetType() == GeomAbs_BSplineCurve) {
    return myResult.BSpline()->IsRational();
  }
  else if (GetType() == GeomAbs_BezierCurve) {
    return myResult.Bezier()->IsRational();
  }
  // portage WNT
  return Standard_False;
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectedCurve::NbPoles() const
{
  Standard_NoSuchObject_Raise_if 
    ( (GetType() != GeomAbs_BSplineCurve) &&
      (GetType() != GeomAbs_BezierCurve)   
     ,"ProjLib_ProjectedCurve:NbPoles"  );
  if (GetType() == GeomAbs_BSplineCurve) {
    return myResult.BSpline()->NbPoles();
  }
  else if (GetType() == GeomAbs_BezierCurve) {
    return myResult.Bezier()->NbPoles();
  }

  // portage WNT
  return 0;
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer ProjLib_ProjectedCurve::NbKnots() const 
{
  Standard_NoSuchObject_Raise_if ( GetType() != GeomAbs_BSplineCurve, 
				  "ProjLib_ProjectedCurve:NbKnots");
  return myResult.BSpline()->NbKnots();
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom2d_BezierCurve) ProjLib_ProjectedCurve::Bezier() const 
{
 return myResult.Bezier() ;
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) ProjLib_ProjectedCurve::BSpline() const 
{
 return myResult.BSpline() ;
}
//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor2d_Curve2d) ProjLib_ProjectedCurve::Trim 
//(const Standard_Real First,
// const Standard_Real Last,
// const Standard_Real Tolerance) const 
(const Standard_Real ,
 const Standard_Real ,
 const Standard_Real ) const 
{
  throw Standard_NotImplemented ("ProjLib_ProjectedCurve::Trim() - method is not implemented");
}
