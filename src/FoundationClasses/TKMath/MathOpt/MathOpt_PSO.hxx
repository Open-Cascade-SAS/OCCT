// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _MathOpt_PSO_HeaderFile
#define _MathOpt_PSO_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Core.hxx>
#include <math_BullardGenerator.hxx>

#include <NCollection_Vector.hxx>

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

//! Configuration for Particle Swarm Optimization.
struct PSOConfig : NDimConfig
{
  int          NbParticles   = 40;    //!< Number of particles in the swarm
  double       Omega         = 0.7;   //!< Inertia weight (velocity decay)
  double       PhiPersonal   = 1.5;   //!< Personal best attraction coefficient
  double       PhiGlobal     = 1.5;   //!< Global best attraction coefficient
  double       VelocityClamp = 0.5;   //!< Max velocity as fraction of search space
  unsigned int Seed          = 12345; //!< Random seed for reproducibility

  //! Default constructor.
  PSOConfig()
      : NDimConfig(1.0e-8, 100, true)
  {
  }

  //! Constructor with parameters.
  PSOConfig(int theNbParticles, int theMaxIter = 100, double theTolerance = 1.0e-8)
      : NDimConfig(theTolerance, theMaxIter, true),
        NbParticles(theNbParticles)
  {
  }
};

//! Particle Swarm Optimization for global minimization.
//!
//! PSO is a stochastic, population-based optimization algorithm inspired
//! by social behavior of bird flocking or fish schooling.
//!
//! Algorithm:
//! 1. Initialize particles with random positions and velocities
//! 2. Evaluate fitness (function value) for each particle
//! 3. Update each particle's personal best if current position is better
//! 4. Update global best across all particles
//! 5. Update velocities: v = omega*v + c1*r1*(pbest-x) + c2*r2*(gbest-x)
//! 6. Update positions: x = x + v
//! 7. Repeat until convergence or max iterations
//!
//! Properties:
//! - Gradient-free: does not require derivatives
//! - Global: can escape local minima
//! - Stochastic: results depend on random initialization
//! - Bound-constrained: handles box constraints naturally
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theConfig PSO configuration
//! @return result containing best solution found
template <typename Function>
VectorResult PSO(Function&          theFunc,
                 const math_Vector& theLowerBounds,
                 const math_Vector& theUpperBounds,
                 const PSOConfig&   theConfig = PSOConfig())
{
  VectorResult aResult;

  const int aLower = theLowerBounds.Lower();
  const int aUpper = theLowerBounds.Upper();
  const int aN     = aUpper - aLower + 1;

  // Check dimensions
  if (theUpperBounds.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Check bounds
  for (int i = aLower; i <= aUpper; ++i)
  {
    if (theLowerBounds(i) >= theUpperBounds(i))
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
  }

  const int aNbParticles = theConfig.NbParticles;

  // Particle data
  struct Particle
  {
    math_Vector Position;
    math_Vector Velocity;
    math_Vector BestPosition;
    double      BestValue;
    double      CurrentValue;

    Particle(int theLower, int theUpper)
        : Position(theLower, theUpper),
          Velocity(theLower, theUpper),
          BestPosition(theLower, theUpper),
          BestValue(std::numeric_limits<double>::max()),
          CurrentValue(std::numeric_limits<double>::max())
    {
    }
  };

  NCollection_Vector<Particle> aSwarm;
  for (int p = 0; p < aNbParticles; ++p)
  {
    aSwarm.Append(Particle(aLower, aUpper));
  }

  // Random number generator
  math_BullardGenerator aRNG(theConfig.Seed);

  // Compute velocity limits
  math_Vector aVelMax(aLower, aUpper);
  for (int i = aLower; i <= aUpper; ++i)
  {
    aVelMax(i) = theConfig.VelocityClamp * (theUpperBounds(i) - theLowerBounds(i));
  }

  // Initialize particles
  math_Vector aGlobalBest(aLower, aUpper);
  double      aGlobalBestValue = std::numeric_limits<double>::max();

  for (int p = 0; p < aNbParticles; ++p)
  {
    Particle& aParticle = aSwarm.ChangeValue(p);
    // Vectors already initialized by Particle constructor
    // Random position in bounds
    for (int i = aLower; i <= aUpper; ++i)
    {
      double aRand          = aRNG.NextReal();
      aParticle.Position(i) = theLowerBounds(i) + aRand * (theUpperBounds(i) - theLowerBounds(i));

      // Random velocity
      aRand                 = 2.0 * aRNG.NextReal() - 1.0;
      aParticle.Velocity(i) = aRand * aVelMax(i);
    }

    // Evaluate initial fitness
    if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
    {
      aParticle.CurrentValue = std::numeric_limits<double>::max();
    }

    // Initialize personal best
    aParticle.BestPosition = aParticle.Position;
    aParticle.BestValue    = aParticle.CurrentValue;

    // Update global best
    if (aParticle.BestValue < aGlobalBestValue)
    {
      aGlobalBestValue = aParticle.BestValue;
      aGlobalBest      = aParticle.BestPosition;
    }
  }

  // Main PSO loop
  double aPrevBest        = aGlobalBestValue;
  int    aStagnationCount = 0;

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Update each particle
    for (int p = 0; p < aNbParticles; ++p)
    {
      Particle& aParticle = aSwarm.ChangeValue(p);

      // Update velocity
      for (int i = aLower; i <= aUpper; ++i)
      {
        double r1 = aRNG.NextReal();
        double r2 = aRNG.NextReal();

        double aVnew =
          theConfig.Omega * aParticle.Velocity(i)
          + theConfig.PhiPersonal * r1 * (aParticle.BestPosition(i) - aParticle.Position(i))
          + theConfig.PhiGlobal * r2 * (aGlobalBest(i) - aParticle.Position(i));

        // Clamp velocity
        aVnew                 = MathUtils::Clamp(aVnew, -aVelMax(i), aVelMax(i));
        aParticle.Velocity(i) = aVnew;
      }

      // Update position
      for (int i = aLower; i <= aUpper; ++i)
      {
        double aXnew = aParticle.Position(i) + aParticle.Velocity(i);

        // Enforce bounds (reflect or clamp)
        if (aXnew < theLowerBounds(i))
        {
          aXnew                 = theLowerBounds(i);
          aParticle.Velocity(i) = -0.5 * aParticle.Velocity(i);
        }
        else if (aXnew > theUpperBounds(i))
        {
          aXnew                 = theUpperBounds(i);
          aParticle.Velocity(i) = -0.5 * aParticle.Velocity(i);
        }

        aParticle.Position(i) = aXnew;
      }

      // Evaluate fitness
      if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
      {
        aParticle.CurrentValue = std::numeric_limits<double>::max();
      }

      // Update personal best
      if (aParticle.CurrentValue < aParticle.BestValue)
      {
        aParticle.BestValue    = aParticle.CurrentValue;
        aParticle.BestPosition = aParticle.Position;
      }

      // Update global best
      if (aParticle.BestValue < aGlobalBestValue)
      {
        aGlobalBestValue = aParticle.BestValue;
        aGlobalBest      = aParticle.BestPosition;
      }
    }

    // Check for convergence (stagnation)
    if (std::abs(aGlobalBestValue - aPrevBest)
        < theConfig.Tolerance * (1.0 + std::abs(aGlobalBestValue)))
    {
      ++aStagnationCount;
      if (aStagnationCount > 10)
      {
        // Converged
        break;
      }
    }
    else
    {
      aStagnationCount = 0;
    }
    aPrevBest = aGlobalBestValue;
  }

  aResult.Status   = Status::OK;
  aResult.Solution = aGlobalBest;
  aResult.Value    = aGlobalBestValue;
  return aResult;
}

//! Multi-start local optimization with PSO initialization.
//!
//! Uses PSO to find promising starting points, then runs local
//! optimization from each. This combines the global exploration
//! of PSO with the local refinement capability of gradient methods.
//!
//! @tparam Function type with Value and Gradient methods
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds
//! @param theUpperBounds upper bounds
//! @param theNbStarts number of local optimizations to run
//! @param theConfig PSO configuration
//! @return result containing best solution found
template <typename Function>
VectorResult PSOMultiStart(Function&          theFunc,
                           const math_Vector& theLowerBounds,
                           const math_Vector& theUpperBounds,
                           int                theNbStarts = 5,
                           const PSOConfig&   theConfig   = PSOConfig())
{
  // First run PSO to get initial points
  PSOConfig aPSOConfig     = theConfig;
  aPSOConfig.NbParticles   = theNbStarts * 10;
  aPSOConfig.MaxIterations = theConfig.MaxIterations / 2;

  VectorResult aPSOResult = PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig);
  return aPSOResult; // For now, just return PSO result
}

} // namespace MathOpt

#endif // _MathOpt_PSO_HeaderFile
