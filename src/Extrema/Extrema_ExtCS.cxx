// Created on: 1995-07-18
// Created by: Modelistation
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

//  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_ExtPElS.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

Extrema_ExtCS::Extrema_ExtCS() 
{
  myDone = Standard_False;
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
  const Adaptor3d_Surface& S,
  const Standard_Real    TolC,
  const Standard_Real    TolS)

{
  Initialize(S, S.FirstUParameter(), S.LastUParameter(), 
    S.FirstVParameter(), S.LastVParameter(), 
    TolC, TolS);
  Perform(C, C.FirstParameter(), C.LastParameter());
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
  const Adaptor3d_Surface& S,
  const Standard_Real    UCinf,
  const Standard_Real    UCsup,
  const Standard_Real    Uinf,	
  const Standard_Real    Usup,
  const Standard_Real    Vinf,	
  const Standard_Real    Vsup,
  const Standard_Real    TolC,
  const Standard_Real    TolS)

{
  Initialize(S, Uinf, Usup, Vinf, Vsup, TolC, TolS);
  Perform(C, UCinf, UCsup);
}


void Extrema_ExtCS::Initialize(const Adaptor3d_Surface& S,
  const Standard_Real    Uinf,	
  const Standard_Real    Usup,
  const Standard_Real    Vinf,	
  const Standard_Real    Vsup,
  const Standard_Real    TolC,
  const Standard_Real    TolS)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  myIsPar = Standard_False;
  myuinf  = Uinf;
  myusup  = Usup;
  myvinf  = Vinf;
  myvsup  = Vsup;
  mytolC  = TolC;
  mytolS  = TolS;
  myStype  = myS->GetType();
}


void Extrema_ExtCS::Perform(const Adaptor3d_Curve& C,
  const Standard_Real  Uinf,
  const Standard_Real  Usup)
{
  myucinf = Uinf;
  myucsup = Usup;
  myPOnS.Clear();
  myPOnC.Clear();
  mySqDist.Clear();
  Standard_Integer i, j;
  Standard_Integer NbT, NbU, NbV;
  NbT = NbU = NbV = 10;
  GeomAbs_CurveType myCtype  = C.GetType();


  switch(myCtype) {

  case GeomAbs_Line: 
    {

      switch(myStype) {
      case GeomAbs_Sphere:
        myExtElCS.Perform(C.Line(), myS->Sphere());
        break;   
      case GeomAbs_Cylinder:
        myExtElCS.Perform(C.Line(), myS->Cylinder());
        break;
      case GeomAbs_Plane:
        myExtElCS.Perform(C.Line(), myS->Plane());
        if (myExtElCS.IsParallel())   break;

      case GeomAbs_Torus:
      case GeomAbs_Cone:
      case GeomAbs_BezierSurface:
      case GeomAbs_BSplineSurface:
      case GeomAbs_SurfaceOfRevolution:
      case GeomAbs_SurfaceOfExtrusion:
      case GeomAbs_OffsetSurface:
      case GeomAbs_OtherSurface:
        {
          Standard_Real cfirst = myucinf, clast = myucsup;
          Standard_Real ufirst = myS->FirstUParameter(), ulast = myS->LastUParameter(), 
            vfirst = myS->FirstVParameter(), vlast = myS->LastVParameter();

          if(Precision::IsInfinite(Abs(cfirst)) || Precision::IsInfinite(Abs(clast))) {

            Bnd_Box aSurfBox;
            BndLib_AddSurface::Add(*myS, ufirst, ulast, vfirst, vlast, Precision::Confusion(), aSurfBox);
            Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
            aSurfBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            Standard_Real tmin = Precision::Infinite(), tmax = -tmin;
            gp_Lin aLin = C.Line();


            if(!( Precision::IsInfinite(Abs(xmin)) || Precision::IsInfinite(Abs(xmax)) || 
              Precision::IsInfinite(Abs(ymin)) || Precision::IsInfinite(Abs(ymax)) || 
              Precision::IsInfinite(Abs(zmin)) || Precision::IsInfinite(Abs(zmax)))  ) {

                Extrema_ExtPElC anExt;
                Extrema_POnCurv aPntOnLin;
                Standard_Real aParOnLin;
                Standard_Real lim = Precision::Infinite();
                gp_Pnt aLimPntArray[8];

                aLimPntArray[0].SetCoord(xmin, ymin, zmin);
                aLimPntArray[1].SetCoord(xmax, ymin, zmin);
                aLimPntArray[2].SetCoord(xmin, ymax, zmin);
                aLimPntArray[3].SetCoord(xmax, ymax, zmin);
                aLimPntArray[4].SetCoord(xmin, ymin, zmax);
                aLimPntArray[5].SetCoord(xmax, ymin, zmax);
                aLimPntArray[6].SetCoord(xmin, ymax, zmax);
                aLimPntArray[7].SetCoord(xmax, ymax, zmax);

                for(i = 0; i <= 7; i++) {
                  anExt.Perform(aLimPntArray[i], aLin, Precision::Confusion(), -lim, lim);
                  aPntOnLin = anExt.Point(1);
                  aParOnLin = aPntOnLin.Parameter();
                  tmin = Min(aParOnLin, tmin);
                  tmax = Max(aParOnLin, tmax);
                }

            }
            else {
              tmin = -1.e+50;
              tmax =  1.e+50;
            }


            cfirst = Max(cfirst, tmin);
            clast  = Min(clast,  tmax);

          }

          if (myS->IsUPeriodic())
            NbU = 13;
          if (myS->IsVPeriodic())
            NbV = 13;

          Extrema_GenExtCS Ext(C, *myS, NbT, NbU, NbV, cfirst, clast, ufirst, ulast,
            vfirst, vlast, mytolC, mytolS);

          myDone = Ext.IsDone();
          if (myDone) {
            Standard_Integer NbExt = Ext.NbExt();
            Standard_Real T,U,V;
            Extrema_POnCurv PC;
            Extrema_POnSurf PS;
            for (i = 1; i <= NbExt; i++) {
              PC = Ext.PointOnCurve(i);
              PS = Ext.PointOnSurface(i);
              T = PC.Parameter();
              PS.Parameter(U, V);
              AddSolution(C, T, U, V, PC.Value(), PS.Value(), Ext.SquareDistance(i));
            }
          }
          return;

        }
      }
      break;
    }
    //  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 Begin
  case GeomAbs_Circle:
    {
      if(myStype == GeomAbs_Cylinder) {
        myExtElCS.Perform(C.Circle(), myS->Cylinder());
        break;
      }
      else if(myStype == GeomAbs_Plane)
      {
        myExtElCS.Perform(C.Circle(), myS->Plane());
        break;
      }
    }
  case GeomAbs_Hyperbola: 
    {
      if(myCtype == GeomAbs_Hyperbola && myStype == GeomAbs_Plane) {
        //  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 End
        myExtElCS.Perform(C.Hyperbola(), myS->Plane());
        break;
      }
    }
  default:
    {
      Extrema_GenExtCS Ext;
      Ext.Initialize(*myS, NbU, NbV, mytolS);
      if(myCtype == GeomAbs_Hyperbola) {
        Standard_Real tmin = Max(-20., C.FirstParameter());
        Standard_Real tmax = Min(20., C.LastParameter());
        Ext.Perform(C, NbT, tmin, tmax, mytolC); // to avoid overflow
      }
      else {
        if((myCtype == GeomAbs_Circle       && NbT < 13) ||
           (myCtype == GeomAbs_BSplineCurve && NbT < 13) )
        {
          NbT = 13;
        }
        Ext.Perform(C, NbT, mytolC);
      }

      myDone = Ext.IsDone();
      if (myDone) {
        Standard_Integer NbExt = Ext.NbExt();
        Standard_Real T,U,V;
        Extrema_POnCurv PC;
        Extrema_POnSurf PS;
        for (i = 1; i <= NbExt; i++) {
          PC = Ext.PointOnCurve(i);
          PS = Ext.PointOnSurface(i);
          T = PC.Parameter();
          PS.Parameter(U, V);
          AddSolution(C, T, U, V, PC.Value(), PS.Value(), Ext.SquareDistance(i));
        }

        //Add sharp points
        Standard_Integer SolNumber = mySqDist.Length();
        Standard_Address CopyC = (Standard_Address)&C;
        Adaptor3d_Curve& aC = *(Adaptor3d_Curve*)CopyC;
        Standard_Integer NbIntervals = aC.NbIntervals(GeomAbs_C1);
        TColStd_Array1OfReal SharpPoints(1, NbIntervals+1);
        aC.Intervals(SharpPoints, GeomAbs_C1);
        
        Extrema_ExtPS aProjPS;
        aProjPS.Initialize (*myS,
                            myS->FirstUParameter(),
                            myS->LastUParameter(),
                            myS->FirstVParameter(),
                            myS->LastVParameter(),
                            mytolS,
                            mytolS);

        for (i = 2; i < SharpPoints.Upper(); ++i)
        {
          T = SharpPoints(i);
          gp_Pnt aPnt = C.Value(T);
          aProjPS.Perform (aPnt);
          if (!aProjPS.IsDone())
            continue;
          Standard_Integer NbProj = aProjPS.NbExt(), jmin = 0;
          Standard_Real MinSqDist = RealLast();
          for (j = 1; j <= NbProj; j++)
          {
            Standard_Real aSqDist = aProjPS.SquareDistance(j);
            if (aSqDist < MinSqDist)
            {
              MinSqDist = aSqDist;
              jmin = j;
            }
          }
          if (jmin != 0)
          {
            aProjPS.Point(jmin).Parameter(U,V);
            AddSolution(C, T, U, V,
                        aPnt, aProjPS.Point(jmin).Value(), MinSqDist);
          }
        }
        //Cut sharp solutions to keep only minimum and maximum
        Standard_Integer imin = SolNumber + 1, imax = mySqDist.Length();
        for (i = SolNumber + 1; i <= mySqDist.Length(); i++)
        {
          if (mySqDist(i) < mySqDist(imin))
            imin = i;
          if (mySqDist(i) > mySqDist(imax))
            imax = i;
        }
        if (mySqDist.Length() > SolNumber + 2)
        {
          Standard_Real MinSqDist = mySqDist(imin);
          Standard_Real MaxSqDist = mySqDist(imax);
          Extrema_POnCurv MinPC = myPOnC(imin);
          Extrema_POnCurv MaxPC = myPOnC(imax);
          Extrema_POnSurf MinPS = myPOnS(imin);
          Extrema_POnSurf MaxPS = myPOnS(imax);

          mySqDist.Remove(SolNumber + 1, mySqDist.Length());
          myPOnC.Remove(SolNumber + 1, myPOnC.Length());
          myPOnS.Remove(SolNumber + 1, myPOnS.Length());

          mySqDist.Append(MinSqDist);
          myPOnC.Append(MinPC);
          myPOnS.Append(MinPS);
          mySqDist.Append(MaxSqDist);
          myPOnC.Append(MaxPC);
          myPOnS.Append(MaxPS);
        }
      }
      return;
    }
    break;
  }

  myDone = myExtElCS.IsDone();
  if (myDone) {
    myIsPar = myExtElCS.IsParallel();
    if (myIsPar) {
      mySqDist.Append(myExtElCS.SquareDistance(1));
    }
    else {
      Standard_Integer NbExt = myExtElCS.NbExt();
      Standard_Real U, V;
      for (i = 1; i <= NbExt; i++) {
        Extrema_POnCurv PC;
        Extrema_POnSurf PS;
        myExtElCS.Points(i, PC, PS);
        Standard_Real Ucurve = PC.Parameter();
        PS.Parameter(U, V);
        AddSolution(C, Ucurve, U, V, PC.Value(), PS.Value(), myExtElCS.SquareDistance(i));
      }
      if(mySqDist.Length() == 0 && NbExt > 0)
      {
        //Analytical extremas seem to be out of curve/surface boundaries.
        //For plane it is possible to add extremity points of curve
        if(myStype == GeomAbs_Plane)
        {
          gp_Pln aPln = myS->Plane();
          gp_Pnt PC, PP;
          if(!Precision::IsInfinite(myucinf))
          {
            PC = C.Value(myucinf);
            ElSLib::PlaneParameters(aPln.Position(), PC, U, V);
            PP = ElSLib::PlaneValue(U, V, aPln.Position());
            AddSolution(C, myucinf, U, V, PC, PP, PC.SquareDistance(PP));
          }
          if(!Precision::IsInfinite(myucsup))
          {
            PC = C.Value(myucsup);
            ElSLib::PlaneParameters(aPln.Position(), PC, U, V);
            PP = ElSLib::PlaneValue(U, V, aPln.Position());
            AddSolution(C, myucsup, U, V, PC, PP, PC.SquareDistance(PP));
          }
        }
      }
    }
  }

}


Standard_Boolean Extrema_ExtCS::IsDone() const
{
  return myDone;
}

Standard_Boolean Extrema_ExtCS::IsParallel() const
{
  return myIsPar;
}


Standard_Real Extrema_ExtCS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) throw StdFail_NotDone();
  if (myIsPar && N != 1) throw StdFail_InfiniteSolutions();
  if ((N < 1) || (N > mySqDist.Length())) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


Standard_Integer Extrema_ExtCS::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return myPOnC.Length();
}



void Extrema_ExtCS::Points(const Standard_Integer N,
  Extrema_POnCurv&       P1,
  Extrema_POnSurf&       P2) const
{
  if(!myDone) throw StdFail_NotDone();
  P1 = myPOnC.Value(N);
  P2 = myPOnS.Value(N);
}

Standard_Boolean Extrema_ExtCS::AddSolution(const Adaptor3d_Curve& theCurve,
  const Standard_Real aT,
  const Standard_Real aU,
  const Standard_Real aV,
  const gp_Pnt& PointOnCurve,
  const gp_Pnt& PointOnSurf,
  const Standard_Real SquareDist)
{
  Standard_Boolean Added = Standard_False;

  Standard_Real T = aT, U = aU, V = aV;

  if (theCurve.IsPeriodic())
    T = ElCLib::InPeriod(T, myucinf, myucinf + theCurve.Period());
  if (myS->IsUPeriodic())
    U = ElCLib::InPeriod(U, myuinf, myuinf + myS->UPeriod());
  if (myS->IsVPeriodic())
    V = ElCLib::InPeriod(V, myvinf, myvinf + myS->VPeriod());

  Extrema_POnCurv aPC;
  Extrema_POnSurf aPS;
  if ((myucinf-T) <= mytolC && (T-myucsup) <= mytolC &&
    (myuinf-U) <= mytolS && (U-myusup) <= mytolS &&
    (myvinf-V) <= mytolS && (V-myvsup) <= mytolS)
  {
    Standard_Boolean IsNewSolution = Standard_True;
    for (Standard_Integer j = 1; j <= mySqDist.Length(); j++)
    {
      aPC = myPOnC(j);
      aPS = myPOnS(j);
      Standard_Real Tj = aPC.Parameter();
      Standard_Real Uj, Vj;
      aPS.Parameter(Uj, Vj);
      if (Abs(T - Tj) <= mytolC &&
        Abs(U - Uj) <= mytolS &&
        Abs(V - Vj) <= mytolS)
      {
        IsNewSolution = Standard_False;
        break;
      }
    }
    if (IsNewSolution)
    {
      mySqDist.Append(SquareDist);
      aPC.SetValues(T, PointOnCurve);
      myPOnC.Append(aPC);
      myPOnS.Append(Extrema_POnSurf(U, V, PointOnSurf));
      Added = Standard_True;
    }
  }
  return Added;
}
