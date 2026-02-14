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
#include <MathUtils_LineSearch.hxx>
#include <MathUtils_Random.hxx>

#include <NCollection_Vector.hxx>

#include <cmath>
#include <optional>

namespace MathOpt
{
using namespace MathUtils;

//! Initialization mode for PSO particles.
enum class PSOInitMode
{
  RandomOnly,      //!< All particles randomly initialized (default)
  SeededOnly,      //!< Initialize from seed particles only
  SeededPlusRandom //!< Seed particles + remaining random
};

//! Boundary handling mode for particles leaving the search space.
enum class PSOBoundaryMode
{
  Clamp,   //!< Clamp to bound, reverse velocity with -0.5 damping (default)
  Reflect, //!< Reflect coordinate, damp velocity by -0.5
  Wrap     //!< Periodic wrap into bounds
};

//! Inertia weight schedule.
enum class PSOInertiaSchedule
{
  Constant,   //!< Fixed omega (default)
  LinearDecay //!< Linear decay from Omega to OmegaMin over iterations
};

//! Seed particle for PSO initialization.
struct PSOSeedParticle
{
  math_Vector                Position; //!< Initial position (will be clamped to bounds)
  std::optional<double>      Value;    //!< Known function value (nullopt = will be evaluated)
  std::optional<math_Vector> Velocity; //!< Initial velocity (nullopt = generate bounded random)

  PSOSeedParticle(const math_Vector& thePos)
      : Position(thePos)
  {
  }

  PSOSeedParticle(const math_Vector& thePos, const double theValue)
      : Position(thePos),
        Value(theValue)
  {
  }
};

//! Statistics collected during PSO execution.
struct PSOStats
{
  int    NbFunctionEvals       = 0;                        //!< Total function evaluations
  int    NbIterations          = 0;                        //!< Iterations performed
  int    NbBoundaryCorrections = 0;                        //!< Boundary corrections applied
  int    NbStagnationEvents    = 0;                        //!< Times stagnation was detected
  int    NbRestarts            = 0;                        //!< Restarts performed
  double InitialBest = std::numeric_limits<double>::max(); //!< Best value after initialization
  double FinalBest   = std::numeric_limits<double>::max(); //!< Best value at termination
};

//! Configuration for Particle Swarm Optimization.
struct PSOConfig : NDimConfig
{
  int          NbParticles   = 40;  //!< Number of particles in the swarm
  double       Omega         = 0.7; //!< Inertia weight (velocity decay)
  double       PhiPersonal   = 1.5; //!< Personal best attraction coefficient
  double       PhiGlobal     = 1.5; //!< Global best attraction coefficient
  double       VelocityClamp = 0.5; //!< Max velocity as fraction of search space
  unsigned int Seed          = 6;   //!< Random seed for reproducibility

  PSOInitMode           InitMode        = PSOInitMode::RandomOnly;
  PSOBoundaryMode       BoundaryMode    = PSOBoundaryMode::Clamp;
  PSOInertiaSchedule    InertiaSchedule = PSOInertiaSchedule::Constant;
  double                OmegaMin        = 0.4; //!< Min inertia for LinearDecay
  int                   MinIterations   = 0;   //!< Minimum iterations before convergence
  std::optional<double> TargetValue;          //!< Early stop if best <= target (nullopt = disabled)
  double                NoImproveTol   = 0.0; //!< Stagnation tolerance (0 = use Tolerance)
  int                   NoImproveIters = 10;  //!< Stagnation iteration threshold
  double RestartFraction    = 0.0; //!< Fraction of particles to reinitialize (0 = no restarts)
  int    MaxRestarts        = 0;   //!< Maximum restart count (0 = unlimited when fraction > 0)
  int    PolishBudgetPerDim = 50;  //!< Max polishing evals per dimension (0 = no polishing)

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

//! Coordinate-wise polishing using Brent's 1D minimization.
//! For each dimension, performs Brent's method directly on the single coordinate,
//! modifying only one element of the position vector per evaluation (zero-allocation).
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param thePosition current best position (updated in place)
//! @param theValue current best value (updated in place)
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theTolerance convergence tolerance for Brent's method
//! @param theMaxPolishEvals maximum total function evaluations for polishing
//! @param theEvalCount output: number of function evaluations used
template <typename Function>
void PolishCoordinateWise(Function&          theFunc,
                          math_Vector&       thePosition,
                          double&            theValue,
                          const math_Vector& theLowerBounds,
                          const math_Vector& theUpperBounds,
                          double             theTolerance,
                          int                theMaxPolishEvals,
                          int&               theEvalCount)
{
  theEvalCount = 0;

  const int aLower  = thePosition.Lower();
  const int aUpper  = thePosition.Upper();
  const int aNbDims = aUpper - aLower + 1;

  // Fair per-coordinate cap: total budget split across dimensions and passes
  const int aNbPasses      = (aNbDims > 1) ? 2 : 1;
  const int aPerCoordLimit = std::max(1, theMaxPolishEvals / (aNbDims * aNbPasses));

  for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
  {
    if (theEvalCount >= theMaxPolishEvals)
    {
      break;
    }

    const int aMaxIter = std::min(aPerCoordLimit, theMaxPolishEvals - theEvalCount);
    BrentAlongCoordinate(theFunc,
                         thePosition,
                         aDimIdx,
                         theLowerBounds(aDimIdx),
                         theUpperBounds(aDimIdx),
                         theValue,
                         theTolerance,
                         aMaxIter,
                         theEvalCount);
  }

  // Second pass for non-separable functions — dimensions may interact
  if (theEvalCount < theMaxPolishEvals && aNbDims > 1)
  {
    for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
    {
      if (theEvalCount >= theMaxPolishEvals)
      {
        break;
      }

      const int aMaxIter = std::min(aPerCoordLimit, theMaxPolishEvals - theEvalCount);
      BrentAlongCoordinate(theFunc,
                           thePosition,
                           aDimIdx,
                           theLowerBounds(aDimIdx),
                           theUpperBounds(aDimIdx),
                           theValue,
                           theTolerance,
                           aMaxIter,
                           theEvalCount);
    }
  }
}

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
//! @param theSeeds optional seed particles for initialization
//! @param theStats optional output statistics
//! @return result containing best solution found
template <typename Function>
VectorResult PSO(Function&                                  theFunc,
                 const math_Vector&                         theLowerBounds,
                 const math_Vector&                         theUpperBounds,
                 const PSOConfig&                           theConfig,
                 const NCollection_Vector<PSOSeedParticle>* theSeeds,
                 PSOStats*                                  theStats = nullptr)
{
  VectorResult aResult;

  const int aLower  = theLowerBounds.Lower();
  const int aUpper  = theLowerBounds.Upper();
  const int aNbDims = aUpper - aLower + 1;

  // Check dimensions
  if (theUpperBounds.Length() != aNbDims)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Check bounds
  for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
  {
    if (theLowerBounds(aDimIdx) >= theUpperBounds(aDimIdx))
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
  }

  const int aNbParticles = theConfig.NbParticles;
  if (aNbParticles <= 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Initialize stats
  PSOStats aLocalStats;

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
  for (int aPartIdx = 0; aPartIdx < aNbParticles; ++aPartIdx)
  {
    aSwarm.Append(Particle(aLower, aUpper));
  }

  // Random number generator
  MathUtils::RandomGenerator aRNG(theConfig.Seed);

  // Compute velocity limits
  math_Vector aVelMax(aLower, aUpper);
  math_Vector aRange(aLower, aUpper);
  for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
  {
    aRange(aDimIdx)  = theUpperBounds(aDimIdx) - theLowerBounds(aDimIdx);
    aVelMax(aDimIdx) = theConfig.VelocityClamp * aRange(aDimIdx);
  }

  // Initialize particles
  math_Vector aGlobalBest(aLower, aUpper);
  double      aGlobalBestValue = std::numeric_limits<double>::max();

  // Determine how many particles are seeded
  const int aNbSeeds = (theSeeds != nullptr) ? theSeeds->Length() : 0;
  int       aSeeded  = 0;

  // Validate seed dimensions and index ranges
  if (aNbSeeds > 0)
  {
    for (int aSeedIdx = 0; aSeedIdx < aNbSeeds; ++aSeedIdx)
    {
      const PSOSeedParticle& aSeed = theSeeds->Value(aSeedIdx);
      if (aSeed.Position.Lower() != aLower || aSeed.Position.Upper() != aUpper)
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
      if (aSeed.Velocity.has_value()
          && (aSeed.Velocity->Lower() != aLower || aSeed.Velocity->Upper() != aUpper))
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
    }
  }

  // Validate SeededOnly requires at least one seed
  if (theConfig.InitMode == PSOInitMode::SeededOnly && aNbSeeds == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  if (aNbSeeds > 0
      && (theConfig.InitMode == PSOInitMode::SeededPlusRandom
          || theConfig.InitMode == PSOInitMode::SeededOnly))
  {
    // Initialize from seeds
    const int aNbDirect = std::min(aNbSeeds, aNbParticles);
    for (int aSeedIdx = 0; aSeedIdx < aNbDirect; ++aSeedIdx)
    {
      Particle&              aParticle = aSwarm.ChangeValue(aSeedIdx);
      const PSOSeedParticle& aSeed     = theSeeds->Value(aSeedIdx);

      // Clamp seed position to bounds, track if any coordinate was clamped
      bool aWasClamped = false;
      for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
      {
        const double aClamped = MathUtils::Clamp(aSeed.Position(aDimIdx),
                                                 theLowerBounds(aDimIdx),
                                                 theUpperBounds(aDimIdx));
        if (aClamped != aSeed.Position(aDimIdx))
        {
          aWasClamped = true;
        }
        aParticle.Position(aDimIdx) = aClamped;
      }

      // Use seed velocity or generate random
      if (aSeed.Velocity.has_value())
      {
        const math_Vector& aSeedVel = *aSeed.Velocity;
        for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
        {
          aParticle.Velocity(aDimIdx) =
            MathUtils::Clamp(aSeedVel(aDimIdx), -aVelMax(aDimIdx), aVelMax(aDimIdx));
        }
      }
      else
      {
        for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
        {
          aParticle.Velocity(aDimIdx) = (2.0 * aRNG.NextReal() - 1.0) * aVelMax(aDimIdx);
        }
      }

      // Use known value only if position was not clamped; otherwise re-evaluate
      if (aSeed.Value.has_value() && !aWasClamped)
      {
        aParticle.CurrentValue = *aSeed.Value;
      }
      else
      {
        if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
        {
          aParticle.CurrentValue = std::numeric_limits<double>::max();
        }
        ++aLocalStats.NbFunctionEvals;
      }

      aParticle.BestPosition = aParticle.Position;
      aParticle.BestValue    = aParticle.CurrentValue;

      if (aParticle.BestValue < aGlobalBestValue)
      {
        aGlobalBestValue = aParticle.BestValue;
        aGlobalBest      = aParticle.BestPosition;
      }
    }
    aSeeded = aNbDirect;

    // For SeededOnly: if seeds < NbParticles, jitter best seeds to fill
    if (theConfig.InitMode == PSOInitMode::SeededOnly && aSeeded < aNbParticles)
    {
      for (int aPartIdx = aSeeded; aPartIdx < aNbParticles; ++aPartIdx)
      {
        Particle& aParticle = aSwarm.ChangeValue(aPartIdx);
        // Pick a seed to jitter (round-robin)
        const int              aSrcIdx = aPartIdx % aSeeded;
        const PSOSeedParticle& aSrc    = theSeeds->Value(aSrcIdx);

        for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
        {
          double aJitter              = (2.0 * aRNG.NextReal() - 1.0) * 0.1 * aRange(aDimIdx);
          aParticle.Position(aDimIdx) = MathUtils::Clamp(aSrc.Position(aDimIdx) + aJitter,
                                                         theLowerBounds(aDimIdx),
                                                         theUpperBounds(aDimIdx));
          aParticle.Velocity(aDimIdx) = (2.0 * aRNG.NextReal() - 1.0) * aVelMax(aDimIdx);
        }

        if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
        {
          aParticle.CurrentValue = std::numeric_limits<double>::max();
        }
        ++aLocalStats.NbFunctionEvals;

        aParticle.BestPosition = aParticle.Position;
        aParticle.BestValue    = aParticle.CurrentValue;

        if (aParticle.BestValue < aGlobalBestValue)
        {
          aGlobalBestValue = aParticle.BestValue;
          aGlobalBest      = aParticle.BestPosition;
        }
      }
      aSeeded = aNbParticles;
    }
  }

  // Fill remaining particles randomly
  for (int aPartIdx = aSeeded; aPartIdx < aNbParticles; ++aPartIdx)
  {
    Particle& aParticle = aSwarm.ChangeValue(aPartIdx);
    for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
    {
      aParticle.Position(aDimIdx) = theLowerBounds(aDimIdx) + aRNG.NextReal() * aRange(aDimIdx);
      aParticle.Velocity(aDimIdx) = (2.0 * aRNG.NextReal() - 1.0) * aVelMax(aDimIdx);
    }

    if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
    {
      aParticle.CurrentValue = std::numeric_limits<double>::max();
    }
    ++aLocalStats.NbFunctionEvals;

    aParticle.BestPosition = aParticle.Position;
    aParticle.BestValue    = aParticle.CurrentValue;

    if (aParticle.BestValue < aGlobalBestValue)
    {
      aGlobalBestValue = aParticle.BestValue;
      aGlobalBest      = aParticle.BestPosition;
    }
  }

  aLocalStats.InitialBest = aGlobalBestValue;

  // Main PSO loop
  double       aPrevBest        = aGlobalBestValue;
  int          aStagnationCount = 0;
  const double aStagnTol =
    (theConfig.NoImproveTol > 0.0) ? theConfig.NoImproveTol : theConfig.Tolerance;

  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    // Compute inertia weight
    double anOmega = theConfig.Omega;
    if (theConfig.InertiaSchedule == PSOInertiaSchedule::LinearDecay)
    {
      anOmega = theConfig.Omega
                - (theConfig.Omega - theConfig.OmegaMin) * static_cast<double>(anIter)
                    / static_cast<double>(theConfig.MaxIterations);
    }

    // Update each particle
    for (int aPartIdx = 0; aPartIdx < aNbParticles; ++aPartIdx)
    {
      Particle& aParticle = aSwarm.ChangeValue(aPartIdx);

      // Update velocity
      for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
      {
        const double aRand1 = aRNG.NextReal();
        const double aRand2 = aRNG.NextReal();

        double aVnew =
          anOmega * aParticle.Velocity(aDimIdx)
          + theConfig.PhiPersonal * aRand1
              * (aParticle.BestPosition(aDimIdx) - aParticle.Position(aDimIdx))
          + theConfig.PhiGlobal * aRand2 * (aGlobalBest(aDimIdx) - aParticle.Position(aDimIdx));

        // Clamp velocity
        aVnew                       = MathUtils::Clamp(aVnew, -aVelMax(aDimIdx), aVelMax(aDimIdx));
        aParticle.Velocity(aDimIdx) = aVnew;
      }

      // Update position with boundary handling
      for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
      {
        double aXnew = aParticle.Position(aDimIdx) + aParticle.Velocity(aDimIdx);

        if (aXnew < theLowerBounds(aDimIdx) || aXnew > theUpperBounds(aDimIdx))
        {
          ++aLocalStats.NbBoundaryCorrections;

          switch (theConfig.BoundaryMode)
          {
            case PSOBoundaryMode::Clamp: {
              aXnew = MathUtils::Clamp(aXnew, theLowerBounds(aDimIdx), theUpperBounds(aDimIdx));
              aParticle.Velocity(aDimIdx) = -0.5 * aParticle.Velocity(aDimIdx);
              break;
            }
            case PSOBoundaryMode::Reflect: {
              if (aXnew < theLowerBounds(aDimIdx))
              {
                aXnew = 2.0 * theLowerBounds(aDimIdx) - aXnew;
              }
              else
              {
                aXnew = 2.0 * theUpperBounds(aDimIdx) - aXnew;
              }
              // Re-clamp in case reflection overshoots the other bound
              aXnew = MathUtils::Clamp(aXnew, theLowerBounds(aDimIdx), theUpperBounds(aDimIdx));
              aParticle.Velocity(aDimIdx) = -0.5 * aParticle.Velocity(aDimIdx);
              break;
            }
            case PSOBoundaryMode::Wrap: {
              const double aRangeSize = aRange(aDimIdx);
              aXnew                   = aXnew - theLowerBounds(aDimIdx);
              aXnew                   = std::fmod(aXnew, aRangeSize);
              if (aXnew < 0.0)
              {
                aXnew += aRangeSize;
              }
              aXnew += theLowerBounds(aDimIdx);
              break;
            }
          }
        }

        aParticle.Position(aDimIdx) = aXnew;
      }

      // Evaluate fitness
      if (!theFunc.Value(aParticle.Position, aParticle.CurrentValue))
      {
        aParticle.CurrentValue = std::numeric_limits<double>::max();
      }
      ++aLocalStats.NbFunctionEvals;

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

    // Check for target value early stop
    if (theConfig.TargetValue.has_value() && aGlobalBestValue <= *theConfig.TargetValue)
    {
      break;
    }

    // Check for convergence (stagnation) after minimum iterations
    if (anIter >= theConfig.MinIterations)
    {
      if (std::abs(aGlobalBestValue - aPrevBest) < aStagnTol * (1.0 + std::abs(aGlobalBestValue)))
      {
        ++aStagnationCount;
        if (aStagnationCount >= theConfig.NoImproveIters)
        {
          ++aLocalStats.NbStagnationEvents;

          // Restart logic
          if (theConfig.RestartFraction > 0.0
              && (theConfig.MaxRestarts == 0 || aLocalStats.NbRestarts < theConfig.MaxRestarts))
          {
            ++aLocalStats.NbRestarts;
            aStagnationCount = 0;

            // Reinitialize worst particles
            int aNbRestart = static_cast<int>(std::ceil(theConfig.RestartFraction * aNbParticles));
            aNbRestart     = std::min(aNbRestart, aNbParticles - 1); // keep at least the best

            // Find best particle index
            int aBestIdx = 0;
            for (int aFindIdx = 1; aFindIdx < aNbParticles; ++aFindIdx)
            {
              if (aSwarm.Value(aFindIdx).BestValue < aSwarm.Value(aBestIdx).BestValue)
              {
                aBestIdx = aFindIdx;
              }
            }

            // Sort by fitness descending (simple selection of worst)
            // Reinitialize aNbRestart worst particles
            NCollection_Vector<int> aWorstIndices;
            for (int aCollIdx = 0; aCollIdx < aNbParticles; ++aCollIdx)
            {
              if (aCollIdx != aBestIdx)
              {
                aWorstIndices.Append(aCollIdx);
              }
            }
            // Simple approach: sort by BestValue descending, take first aNbRestart
            for (int aSortOuter = 0; aSortOuter < aWorstIndices.Length() - 1; ++aSortOuter)
            {
              for (int aSortInner = aSortOuter + 1; aSortInner < aWorstIndices.Length();
                   ++aSortInner)
              {
                if (aSwarm.Value(aWorstIndices.Value(aSortOuter)).BestValue
                    < aSwarm.Value(aWorstIndices.Value(aSortInner)).BestValue)
                {
                  const int aTmp                        = aWorstIndices.Value(aSortOuter);
                  aWorstIndices.ChangeValue(aSortOuter) = aWorstIndices.Value(aSortInner);
                  aWorstIndices.ChangeValue(aSortInner) = aTmp;
                }
              }
            }

            const int aNbToRestart = std::min(aNbRestart, aWorstIndices.Length());
            for (int aRestIdx = 0; aRestIdx < aNbToRestart; ++aRestIdx)
            {
              Particle& aRestartPart = aSwarm.ChangeValue(aWorstIndices.Value(aRestIdx));
              for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
              {
                aRestartPart.Position(aDimIdx) =
                  theLowerBounds(aDimIdx) + aRNG.NextReal() * aRange(aDimIdx);
                aRestartPart.Velocity(aDimIdx) = (2.0 * aRNG.NextReal() - 1.0) * aVelMax(aDimIdx);
              }

              if (!theFunc.Value(aRestartPart.Position, aRestartPart.CurrentValue))
              {
                aRestartPart.CurrentValue = std::numeric_limits<double>::max();
              }
              ++aLocalStats.NbFunctionEvals;

              aRestartPart.BestPosition = aRestartPart.Position;
              aRestartPart.BestValue    = aRestartPart.CurrentValue;

              if (aRestartPart.BestValue < aGlobalBestValue)
              {
                aGlobalBestValue = aRestartPart.BestValue;
                aGlobalBest      = aRestartPart.BestPosition;
              }
            }
          }
          else
          {
            // No restart budget — converged
            break;
          }
        }
      }
      else
      {
        aStagnationCount = 0;
      }
    }
    aPrevBest = aGlobalBestValue;
  }

  // Polish the global best using coordinate-wise Brent's method
  int aPolishEvals = 0;
  if (theConfig.PolishBudgetPerDim > 0)
  {
    PolishCoordinateWise(theFunc,
                         aGlobalBest,
                         aGlobalBestValue,
                         theLowerBounds,
                         theUpperBounds,
                         theConfig.Tolerance,
                         theConfig.PolishBudgetPerDim * aNbDims,
                         aPolishEvals);
    aLocalStats.NbFunctionEvals += aPolishEvals;
  }

  aLocalStats.NbIterations = static_cast<int>(aResult.NbIterations);
  aLocalStats.FinalBest    = aGlobalBestValue;

  if (theStats != nullptr)
  {
    *theStats = aLocalStats;
  }

  aResult.Status   = Status::OK;
  aResult.Solution = aGlobalBest;
  aResult.Value    = aGlobalBestValue;
  return aResult;
}

//! Particle Swarm Optimization for global minimization (basic overload).
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
  return PSO(theFunc, theLowerBounds, theUpperBounds, theConfig, nullptr, nullptr);
}

} // namespace MathOpt

#endif // _MathOpt_PSO_HeaderFile
