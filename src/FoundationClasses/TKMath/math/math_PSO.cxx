// Created on: 2014-07-18
// Created by: Alexander Malyshev
// Copyright (c) 2014-2014 OPEN CASCADE SAS
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

#include <math_PSO.hxx>

#include <math_BullardGenerator.hxx>
#include <math_PSOParticlesPool.hxx>

const double aBorderDivisor = 1.0e+4;

//=================================================================================================

math_PSO::math_PSO(math_MultipleVarFunction* theFunc,
                   const math_Vector&        theLowBorder,
                   const math_Vector&        theUppBorder,
                   const math_Vector&        theSteps,
                   const int                 theNbParticles,
                   const int                 theNbIter)
    : myLowBorder(1, theFunc->NbVariables()),
      myUppBorder(1, theFunc->NbVariables()),
      mySteps(1, theFunc->NbVariables())
{
  myN           = theFunc->NbVariables();
  myNbParticles = theNbParticles;
  myNbIter      = theNbIter;
  myFunc        = theFunc;

  myLowBorder = theLowBorder;
  myUppBorder = theUppBorder;
  mySteps     = theSteps;
}

//=================================================================================================

void math_PSO::Perform(math_PSOParticlesPool& theParticles,
                       int                    theNbParticles,
                       double&                theValue,
                       math_Vector&           theOutPnt,
                       const int              theNbIter)
{
  performPSOWithGivenParticles(theParticles, theNbParticles, theValue, theOutPnt, theNbIter);
}

//=================================================================================================

void math_PSO::Perform(const math_Vector& theSteps,
                       double&            theValue,
                       math_Vector&       theOutPnt,
                       const int          theNbIter)
{
  // Initialization.
  math_Vector aMinUV(1, myN), aMaxUV(1, myN);
  aMinUV   = myLowBorder + (myUppBorder - myLowBorder) / aBorderDivisor;
  aMaxUV   = myUppBorder - (myUppBorder - myLowBorder) / aBorderDivisor;
  myNbIter = theNbIter;
  mySteps  = theSteps;

  // To generate initial distribution it is necessary to have grid steps.
  math_PSOParticlesPool aPool(myNbParticles, myN);

  // Generate initial particles distribution.
  bool        isRegularGridFinished = false;
  double      aCurrValue;
  math_Vector aCurrPoint(1, myN);

  PSO_Particle* aParticle = aPool.GetWorstParticle();
  aCurrPoint              = aMinUV;
  do
  {
    myFunc->Value(aCurrPoint, aCurrValue);

    if (aCurrValue < aParticle->Distance)
    {
      int aDimIdx;
      for (aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
      {
        aParticle->Position[aDimIdx]     = aCurrPoint(aDimIdx + 1);
        aParticle->BestPosition[aDimIdx] = aCurrPoint(aDimIdx + 1);
      }
      aParticle->Distance     = aCurrValue;
      aParticle->BestDistance = aCurrValue;

      aParticle = aPool.GetWorstParticle();
    }

    // Step.
    aCurrPoint(1) += std::max(mySteps(1), 1.0e-15); // Avoid too small step
    for (int aDimIdx = 1; aDimIdx < myN; ++aDimIdx)
    {
      if (aCurrPoint(aDimIdx) > aMaxUV(aDimIdx))
      {
        aCurrPoint(aDimIdx) = aMinUV(aDimIdx);
        aCurrPoint(aDimIdx + 1) += mySteps(aDimIdx + 1);
      }
      else
        break;
    }

    // Stop criteria.
    if (aCurrPoint(myN) > aMaxUV(myN))
      isRegularGridFinished = true;
  } while (!isRegularGridFinished);

  performPSOWithGivenParticles(aPool, myNbParticles, theValue, theOutPnt, theNbIter);
}

//=================================================================================================

void math_PSO::performPSOWithGivenParticles(math_PSOParticlesPool& theParticles,
                                            int                    theNbParticles,
                                            double&                theValue,
                                            math_Vector&           theOutPnt,
                                            const int              theNbIter)
{
  math_Vector aMinUV(1, myN), aMaxUV(1, myN);
  aMinUV                            = myLowBorder + (myUppBorder - myLowBorder) / aBorderDivisor;
  aMaxUV                            = myUppBorder - (myUppBorder - myLowBorder) / aBorderDivisor;
  myNbIter                          = theNbIter;
  myNbParticles                     = theNbParticles;
  math_PSOParticlesPool& aParticles = theParticles;

  // Current particle data.
  math_Vector   aCurrPoint(1, myN);
  math_Vector   aBestGlobalPosition(1, myN);
  PSO_Particle* aParticle = nullptr;

  // Generate initial particle velocities.
  math_BullardGenerator aRandom;
  for (int aPartIdx = 1; aPartIdx <= myNbParticles; ++aPartIdx)
  {
    aParticle = aParticles.GetParticle(aPartIdx);
    for (int aDimIdx = 1; aDimIdx <= myN; ++aDimIdx)
    {
      const double aKsi                = aRandom.NextReal();
      aParticle->Velocity[aDimIdx - 1] = mySteps(aDimIdx) * (aKsi - 0.5) * 2.0;
    }
  }

  aParticle = aParticles.GetBestParticle();
  for (int aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
    aBestGlobalPosition(aDimIdx + 1) = aParticle->Position[aDimIdx];
  double aBestGlobalDistance = aParticle->Distance;

  // This velocity is used for detecting stagnation state.
  math_Vector aTerminationVelocity(1, myN);
  aTerminationVelocity = mySteps / 2048.0;
  // This velocity shows minimal velocity on current step.
  math_Vector aMinimalVelocity(1, myN);

  // Run PSO iterations
  for (int aStep = 1; aStep < myNbIter; ++aStep)
  {
    aMinimalVelocity.Init(RealLast());

    for (int aPartIdx = 1; aPartIdx <= myNbParticles; ++aPartIdx)
    {
      const double aKsi1 = aRandom.NextReal();
      const double aKsi2 = aRandom.NextReal();

      aParticle = aParticles.GetParticle(aPartIdx);

      const double aRetentWeight = 0.72900;
      const double aPersonWeight = 1.49445;
      const double aSocialWeight = 1.49445;

      for (int aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
      {
        aParticle->Velocity[aDimIdx] =
          aParticle->Velocity[aDimIdx] * aRetentWeight
          + (aParticle->BestPosition[aDimIdx] - aParticle->Position[aDimIdx])
              * (aPersonWeight * aKsi1)
          + (aBestGlobalPosition(aDimIdx + 1) - aParticle->Position[aDimIdx])
              * (aSocialWeight * aKsi2);

        aParticle->Position[aDimIdx] += aParticle->Velocity[aDimIdx];
        aParticle->Position[aDimIdx] =
          std::min(std::max(aParticle->Position[aDimIdx], aMinUV(aDimIdx + 1)),
                   aMaxUV(aDimIdx + 1));
        aCurrPoint(aDimIdx + 1) = aParticle->Position[aDimIdx];

        aMinimalVelocity(aDimIdx + 1) =
          std::min(std::abs(aParticle->Velocity[aDimIdx]), aMinimalVelocity(aDimIdx + 1));
      }

      myFunc->Value(aCurrPoint, aParticle->Distance);
      if (aParticle->Distance < aParticle->BestDistance)
      {
        aParticle->BestDistance = aParticle->Distance;
        for (int aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
          aParticle->BestPosition[aDimIdx] = aParticle->Position[aDimIdx];

        if (aParticle->Distance < aBestGlobalDistance)
        {
          aBestGlobalDistance = aParticle->Distance;
          for (int aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
            aBestGlobalPosition(aDimIdx + 1) = aParticle->Position[aDimIdx];
        }
      }
    }

    bool isTerminalVelocityReached = true;
    for (int aDimIdx = 1; aDimIdx <= myN; ++aDimIdx)
    {
      if (aMinimalVelocity(aDimIdx) > aTerminationVelocity(aDimIdx))
      {
        isTerminalVelocityReached = false;
        break;
      }
    }

    if (isTerminalVelocityReached)
    {
      // Minimum number of steps
      const int aMinSteps = 16;

      if (aStep > aMinSteps)
      {
        break;
      }

      for (int aPartIdx = 1; aPartIdx <= myNbParticles; ++aPartIdx)
      {
        const double aKsi = aRandom.NextReal();

        aParticle = aParticles.GetParticle(aPartIdx);

        for (int aDimIdx = 0; aDimIdx < myN; ++aDimIdx)
        {
          if (aParticle->Position[aDimIdx] == aMinUV(aDimIdx + 1)
              || aParticle->Position[aDimIdx] == aMaxUV(aDimIdx + 1))
          {
            aParticle->Velocity[aDimIdx] = aParticle->Position[aDimIdx] == aMinUV(aDimIdx + 1)
                                             ? mySteps(aDimIdx + 1) * aKsi
                                             : -mySteps(aDimIdx + 1) * aKsi;
          }
          else
          {
            aParticle->Velocity[aDimIdx] = mySteps(aDimIdx + 1) * (aKsi - 0.5) * 2.0;
          }
        }
      }
    }
  }
  theValue  = aBestGlobalDistance;
  theOutPnt = aBestGlobalPosition;
}
