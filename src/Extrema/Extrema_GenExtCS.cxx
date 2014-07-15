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

#include <Extrema_GenExtCS.ixx>

#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Precision.hxx>

#include <Geom_Line.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_ExtPS.hxx>

#include <algorithm>
#include <NCollection_Vec3.hxx>
#include <NCollection_Array1.hxx>

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

Extrema_GenExtCS::Extrema_GenExtCS()
{
  myDone = Standard_False;
  myInit = Standard_False;
}

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

Extrema_GenExtCS::Extrema_GenExtCS(const Adaptor3d_Curve& C, 
  const Adaptor3d_Surface& S, 
  const Standard_Integer NbT, 
  const Standard_Integer NbU, 
  const Standard_Integer NbV, 
  const Standard_Real Tol1, 
  const Standard_Real Tol2)
{
  Initialize(S, NbU, NbV, Tol2);
  Perform(C, NbT, Tol1);
}

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

Extrema_GenExtCS::Extrema_GenExtCS (const Adaptor3d_Curve& C, 
                                    const Adaptor3d_Surface& S, 
                                    const Standard_Integer NbT, 
                                    const Standard_Integer NbU, 
                                    const Standard_Integer NbV, 
                                    const Standard_Real tmin, 
                                    const Standard_Real tsup, 
                                    const Standard_Real Umin, 
                                    const Standard_Real Usup,
                                    const Standard_Real Vmin, 
                                    const Standard_Real Vsup, 
                                    const Standard_Real Tol1, 
                                    const Standard_Real Tol2)
{
  Initialize(S, NbU, NbV, Umin,Usup,Vmin,Vsup,Tol2);
  Perform(C, NbT, tmin, tsup, Tol1);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Initialize (const Adaptor3d_Surface& S, 
                                   const Standard_Integer NbU, 
                                   const Standard_Integer NbV, 
                                   const Standard_Real Tol2)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S,NbU,NbV,myumin,myusup,myvmin,myvsup,Tol2);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Initialize (const Adaptor3d_Surface& S,
                                   const Standard_Integer NbU,
                                   const Standard_Integer NbV,
                                   const Standard_Real Umin,
                                   const Standard_Real Usup,
                                   const Standard_Real Vmin,
                                   const Standard_Real Vsup,
                                   const Standard_Real Tol2)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  myusample = NbU;
  myvsample = NbV;
  myumin = Umin;
  myusup = Usup;
  myvmin = Vmin;
  myvsup = Vsup;
  mytol2 = Tol2;

  const Standard_Real aTrimMaxU = Precision::IsInfinite (myusup) ?  1.0e+10 : myusup;
  const Standard_Real aTrimMinU = Precision::IsInfinite (myumin) ? -1.0e+10 : myumin;
  const Standard_Real aTrimMaxV = Precision::IsInfinite (myvsup) ?  1.0e+10 : myvsup;
  const Standard_Real aTrimMinV = Precision::IsInfinite (myvmin) ? -1.0e+10 : myvmin;

  const Standard_Real aMinU = aTrimMinU + (aTrimMaxU - aTrimMinU) / 1.0e4;
  const Standard_Real aMinV = aTrimMinV + (aTrimMaxV - aTrimMinV) / 1.0e4;
  const Standard_Real aMaxU = aTrimMaxU - (aTrimMaxU - aTrimMinU) / 1.0e4;
  const Standard_Real aMaxV = aTrimMaxV - (aTrimMaxV - aTrimMinV) / 1.0e4;
  
  const Standard_Real aStepSU = (aMaxU - aMinU) / myusample;
  const Standard_Real aStepSV = (aMaxV - aMinV) / myvsample;

  mySurfPnts = new TColgp_HArray2OfPnt (0, myusample, 0, myvsample);

  Standard_Real aSU = aMinU;
  for (Standard_Integer aSUI = 0; aSUI <= myusample; aSUI++, aSU += aStepSU)
  {
    Standard_Real aSV = aMinV;
    for (Standard_Integer aSVI = 0; aSVI <= myvsample; aSVI++, aSV += aStepSV)
    {
      mySurfPnts->ChangeValue (aSUI, aSVI) = myS->Value (aSU, aSV);
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Perform(const Adaptor3d_Curve& C, 
  const Standard_Integer NbT,
  const Standard_Real Tol1)
{
  mytmin = C.FirstParameter();
  mytsup = C.LastParameter();
  Perform(C, NbT, mytmin, mytsup,Tol1);
}

namespace
{
  //! Vector type for storing curve and surface parameters.
  typedef NCollection_Vec3<Standard_Real> Extrema_Vec3d;

  //! Describes particle for using in PSO algorithm.
  struct Extrema_Particle
  {
    //! Creates unitialized particle.
    Extrema_Particle()
     : Distance (DBL_MAX)
    {
      //
    }

    //! Creates particle with the given position and distance.
    Extrema_Particle (const Extrema_Vec3d& thePosition, Standard_Real theDistance)
     : Position (thePosition),
       Distance (theDistance),
       BestPosition (thePosition),
       BestDistance (theDistance)
    {
      //
    }

    //! Compares the particles according to their distances.
    bool operator< (const Extrema_Particle& thePnt) const
    {
      return Distance < thePnt.Distance;
    }
    
    Extrema_Vec3d Position;
    Extrema_Vec3d Velocity;
    Standard_Real Distance;

    Extrema_Vec3d BestPosition;
    Standard_Real BestDistance;
  };
  
  //! Fast random number generator (the algorithm proposed by Ian C. Bullard).
  class Extrema_Random
  {
  private:

    unsigned int myStateHi;
    unsigned int myStateLo;

  public:
    
    //! Creates new Xorshift 64-bit RNG.
    Extrema_Random (unsigned int theSeed = 1)
     : myStateHi (theSeed)
    {
      myStateLo = theSeed ^ 0x49616E42;
    }

    //! Generates new 64-bit integer value.
    unsigned int NextInt()
    {
      myStateHi = (myStateHi >> 2) + (myStateHi << 2);
      
      myStateHi += myStateLo;
      myStateLo += myStateHi;

      return myStateHi;
    }

    //! Generates new floating-point value.
    Standard_Real NextReal()
    {
      return NextInt() / static_cast<Standard_Real> (0xFFFFFFFFu);
    }
  };
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Perform (const Adaptor3d_Curve& C,
                                const Standard_Integer NbT,
                                const Standard_Real tmin,
                                const Standard_Real tsup,
                                const Standard_Real Tol1)
{
  myDone = Standard_False;
  myF.Initialize(C,*myS);
  mytmin = tmin;
  mytsup = tsup;
  mytol1 = Tol1;
  mytsample = NbT;
  // Modif de lvt pour trimer la surface non pas aux infinis mais  a +/- 10000

  Standard_Real trimusup = myusup, trimumin = myumin,trimvsup = myvsup,trimvmin = myvmin;
  if (Precision::IsInfinite(trimusup)){
    trimusup = 1.0e+10;
  }
  if (Precision::IsInfinite(trimvsup)){
    trimvsup = 1.0e+10;
  }
  if (Precision::IsInfinite(trimumin)){
    trimumin = -1.0e+10;
  }
  if (Precision::IsInfinite(trimvmin)){
    trimvmin = -1.0e+10;
  }
  //
  math_Vector Tol(1, 3);
  Tol(1) = mytol1;
  Tol(2) = mytol2;
  Tol(3) = mytol2;
  math_Vector UV(1,3), UVinf(1,3), UVsup(1,3);
  UVinf(1) = mytmin;
  UVinf(2) = trimumin;
  UVinf(3) = trimvmin;

  UVsup(1) = mytsup;
  UVsup(2) = trimusup;
  UVsup(3) = trimvsup;
  // 18/02/02 akm vvv : (OCC163) bad extremas - extrusion surface versus the line.
  //                    Try to preset the initial solution as extrema between
  //                    extrusion direction and the curve.
  if (myS->GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    gp_Dir aDir = myS->Direction();
    Handle(Adaptor3d_HCurve) aCurve = myS->BasisCurve();
    Standard_Real dfUFirst = aCurve->FirstParameter();
    // Create iso line of U=U0
    GeomAdaptor_Curve anAx(new Geom_Line(aCurve->Value(dfUFirst), aDir),
      trimvmin, trimvsup);
    Extrema_ExtCC aLocator(C, anAx);
    if (aLocator.IsDone() && aLocator.NbExt()>0)
    {
      Standard_Integer iExt;
      // Try to find all extremas
      Extrema_POnCurv aP1, aP2;
      for (iExt=1; iExt<=aLocator.NbExt(); iExt++)
      {
        aLocator.Points (iExt, aP1, aP2);
        // Parameter on curve
        UV(1) = aP1.Parameter();
        // To find parameters on surf, try ExtPS
        Extrema_ExtPS aPreciser (aP1.Value(), *myS, mytol2, mytol2);
        if (aPreciser.IsDone())
        {
          // Managed to find extremas between point and surface
          Standard_Integer iPExt;
          for (iPExt=1; iPExt<=aPreciser.NbExt(); iPExt++)
          {
            aPreciser.Point(iPExt).Parameter(UV(2),UV(3));
            math_FunctionSetRoot S1 (myF,UV,Tol,UVinf,UVsup);
          }
        }
        else
        {
          // Failed... try the point on iso line
          UV(2) = dfUFirst;
          UV(3) = aP2.Parameter();
          math_FunctionSetRoot S1 (myF,UV,Tol,UVinf,UVsup);
        }
      } // for (iExt=1; iExt<=aLocator.NbExt(); iExt++)
    } // if (aLocator.IsDone() && aLocator.NbExt()>0)
  } // if (myS.Type() == GeomAbs_ExtrusionSurface)
  else
  {
    // Number of particles used in PSO algorithm (particle swarm optimization)
    const Standard_Integer aNbParticles = 32;

    NCollection_Array1<Extrema_Particle> aParticles (1, aNbParticles);

    const Extrema_Vec3d aMinUV (UVinf(1) + (UVsup(1) - UVinf(1)) / 1.0e4,
                                UVinf(2) + (UVsup(2) - UVinf(2)) / 1.0e4,
                                UVinf(3) + (UVsup(3) - UVinf(3)) / 1.0e4);

    const Extrema_Vec3d aMaxUV (UVsup(1) - (UVsup(1) - UVinf(1)) / 1.0e4,
                                UVsup(2) - (UVsup(2) - UVinf(2)) / 1.0e4,
                                UVsup(3) - (UVsup(3) - UVinf(3)) / 1.0e4);

    Standard_Real aStepCU = (aMaxUV.x() - aMinUV.x()) / mytsample;
    Standard_Real aStepSU = (aMaxUV.y() - aMinUV.y()) / myusample;
    Standard_Real aStepSV = (aMaxUV.z() - aMinUV.z()) / myvsample;

    // Correct number of curve samples in case of low resolution

    Standard_Real aScaleFactor = 5.0;

    Standard_Real aResolutionCU = aStepCU / C.Resolution (1.0);

    Standard_Real aMinResolution = aScaleFactor * Min (aResolutionCU,
      Min (aStepSU / myS->UResolution (1.0), aStepSV / myS->VResolution (1.0)));

    if (aMinResolution > Epsilon (1.0))
    {
      if (aResolutionCU > aMinResolution)
      {
        const Standard_Integer aMaxNbNodes = 50;

        mytsample = Min (aMaxNbNodes, RealToInt (
          mytsample * aResolutionCU / aMinResolution));

        aStepCU = (aMaxUV.x() - aMinUV.x()) / mytsample;
      }
    }

    // Pre-compute curve sample points
    TColgp_HArray1OfPnt aCurvPnts (0, mytsample);

    Standard_Real aCU = aMinUV.x();
    for (Standard_Integer aCUI = 0; aCUI <= mytsample; aCUI++, aCU += aStepCU)
    {
      aCurvPnts.SetValue (aCUI, C.Value (aCU));
    }

    NCollection_Array1<Extrema_Particle>::iterator aWorstPnt = aParticles.begin();

    // Select specified number of particles from pre-computed set of samples
    Standard_Real aSU = aMinUV.y();
    for (Standard_Integer aSUI = 0; aSUI <= myusample; aSUI++, aSU += aStepSU)
    {
      Standard_Real aSV = aMinUV.z();
      for (Standard_Integer aSVI = 0; aSVI <= myvsample; aSVI++, aSV += aStepSV)
      {
        Standard_Real aCU = aMinUV.x();
        for (Standard_Integer aCUI = 0; aCUI <= mytsample; aCUI++, aCU += aStepCU)
        {
          Standard_Real aSqDist = mySurfPnts->Value (aSUI, aSVI).SquareDistance (aCurvPnts.Value (aCUI)); 

          if (aSqDist < aWorstPnt->Distance)
          {
            *aWorstPnt = Extrema_Particle (Extrema_Vec3d (aCU, aSU, aSV), aSqDist);

            aWorstPnt = std::max_element (aParticles.begin(), aParticles.end());
          }
        }
      }
    }

    Extrema_Random aRandom;

    // Generate initial particle velocities
    for (Standard_Integer aPartIdx = 1; aPartIdx <= aNbParticles; ++aPartIdx)
    {
      const Standard_Real aKsi1 = aRandom.NextReal();
      const Standard_Real aKsi2 = aRandom.NextReal();
      const Standard_Real aKsi3 = aRandom.NextReal();

      aParticles.ChangeValue (aPartIdx).Velocity.x() = aStepCU * (aKsi1 - 0.5) * 2.0;
      aParticles.ChangeValue (aPartIdx).Velocity.y() = aStepSU * (aKsi2 - 0.5) * 2.0;
      aParticles.ChangeValue (aPartIdx).Velocity.z() = aStepSV * (aKsi3 - 0.5) * 2.0;
    }

    NCollection_Array1<Extrema_Particle>::iterator aBestPnt =
      std::min_element (aParticles.begin(), aParticles.end());

    Extrema_Vec3d aBestGlobalPosition = aBestPnt->Position;
    Standard_Real aBestGlobalDistance = aBestPnt->Distance;

    // This velocity is used for detecting stagnation state
    Extrema_Vec3d aTerminationVelocity (aStepCU / 2048, aStepSU / 2048, aStepSV / 2048);

    // Run PSO iterations
    for (Standard_Integer aStep = 1, aMaxNbSteps = 100; aStep < aMaxNbSteps; ++aStep)
    {
      Extrema_Vec3d aMinimalVelocity (DBL_MAX, DBL_MAX, DBL_MAX);

      for (Standard_Integer aPartIdx = 1; aPartIdx <= aParticles.Size(); ++aPartIdx)
      {
        const Standard_Real aKsi1 = aRandom.NextReal();
        const Standard_Real aKsi2 = aRandom.NextReal();

        Extrema_Particle& aPoint = aParticles.ChangeValue (aPartIdx);

        const Standard_Real aRetentWeight = 0.72900;
        const Standard_Real aPersonWeight = 1.49445;
        const Standard_Real aSocialWeight = 1.49445;

        aPoint.Velocity = aPoint.Velocity * aRetentWeight +
          (aPoint.BestPosition - aPoint.Position) * (aPersonWeight * aKsi1) +
          (aBestGlobalPosition - aPoint.Position) * (aSocialWeight * aKsi2);

        aPoint.Position += aPoint.Velocity;

        aPoint.Position.x() = Min (Max (aPoint.Position.x(), aMinUV.x()), aMaxUV.x());
        aPoint.Position.y() = Min (Max (aPoint.Position.y(), aMinUV.y()), aMaxUV.y());
        aPoint.Position.z() = Min (Max (aPoint.Position.z(), aMinUV.z()), aMaxUV.z());

        aPoint.Distance = myS->Value (aPoint.Position.y(),
          aPoint.Position.z()).SquareDistance (C.Value (aPoint.Position.x()));

        if (aPoint.Distance < aPoint.BestDistance)
        {
          aPoint.BestDistance = aPoint.Distance;
          aPoint.BestPosition = aPoint.Position;

          if (aPoint.Distance < aBestGlobalDistance)
          {
            aBestGlobalDistance = aPoint.Distance;
            aBestGlobalPosition = aPoint.Position;
          }
        }
        
        aMinimalVelocity.x() = Min (Abs (aPoint.Velocity.x()), aMinimalVelocity.x());
        aMinimalVelocity.y() = Min (Abs (aPoint.Velocity.y()), aMinimalVelocity.y());
        aMinimalVelocity.z() = Min (Abs (aPoint.Velocity.x()), aMinimalVelocity.z());
      }

      if (aMinimalVelocity.x() < aTerminationVelocity.x()
       && aMinimalVelocity.y() < aTerminationVelocity.y()
       && aMinimalVelocity.z() < aTerminationVelocity.z())
      {
        // Minimum number of steps
        const Standard_Integer aMinSteps = 16;

        if (aStep > aMinSteps)
        {
          break;
        }

        for (Standard_Integer aPartIdx = 1; aPartIdx <= aNbParticles; ++aPartIdx)
        {
          const Standard_Real aKsi1 = aRandom.NextReal();
          const Standard_Real aKsi2 = aRandom.NextReal();
          const Standard_Real aKsi3 = aRandom.NextReal();

          Extrema_Particle& aPoint = aParticles.ChangeValue (aPartIdx);
          
          if (aPoint.Position.x() == aMinUV.x() || aPoint.Position.x() == aMaxUV.x())
          {
            aPoint.Velocity.x() = aPoint.Position.x() == aMinUV.x() ?
              aStepCU * aKsi1 : -aStepCU * aKsi1;
          }
          else
          {
            aPoint.Velocity.x() = aStepCU * (aKsi1 - 0.5) * 2.0;
          }

          if (aPoint.Position.y() == aMinUV.y() || aPoint.Position.y() == aMaxUV.y())
          {
            aPoint.Velocity.y() = aPoint.Position.y() == aMinUV.y() ?
              aStepSU * aKsi2 : -aStepSU * aKsi2;
          }
          else
          {
            aPoint.Velocity.y() = aStepSU * (aKsi2 - 0.5) * 2.0;
          }

          if (aPoint.Position.z() == aMinUV.z() || aPoint.Position.z() == aMaxUV.z())
          {
            aPoint.Velocity.z() = aPoint.Position.z() == aMinUV.z() ?
              aStepSV * aKsi3 : -aStepSV * aKsi3;
          }
          else
          {
            aPoint.Velocity.z() = aStepSV * (aKsi3 - 0.5) * 2.0;
          }
        }
      }
    }

    // Find min approximation
    UV(1) = aBestGlobalPosition.x();
    UV(2) = aBestGlobalPosition.y();
    UV(3) = aBestGlobalPosition.z();
    
    math_FunctionSetRoot anA (myF, UV, Tol, UVinf, UVsup, 100, Standard_False);
  }

  myDone = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_GenExtCS::IsDone() const 
{
  return myDone;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_GenExtCS::NbExt() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.NbExt();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_GenExtCS::SquareDistance(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.SquareDistance(N);
}

//=======================================================================
//function : PointOnCurve
//purpose  : 
//=======================================================================

const Extrema_POnCurv& Extrema_GenExtCS::PointOnCurve(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.PointOnCurve(N);
}

//=======================================================================
//function : PointOnSurface
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenExtCS::PointOnSurface(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.PointOnSurface(N);
}

//=======================================================================
//function : BidonSurface
//purpose  : 
//=======================================================================

Adaptor3d_SurfacePtr Extrema_GenExtCS::BidonSurface() const 
{
  return (Adaptor3d_SurfacePtr)0L;
}

//=======================================================================
//function : BidonCurve
//purpose  : 
//=======================================================================

Adaptor3d_CurvePtr Extrema_GenExtCS::BidonCurve() const 
{
  return (Adaptor3d_CurvePtr)0L;
}

