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
#include "MathOpt_Utils.hxx"

#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>

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
  std::optional<double>      Value;    //!< Advisory value; the callback is always evaluated
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
  size_t NbFunctionEvals       = 0;                        //!< Total function evaluations
  uint32_t NbIterations        = 0;                        //!< Iterations performed
  size_t NbBoundaryCorrections = 0;                        //!< Boundary corrections applied
  size_t NbStagnationEvents    = 0;                        //!< Times stagnation was detected
  size_t NbRestarts            = 0;                        //!< Restarts performed
  std::optional<double> InitialBest; //!< Best value after initialization
  std::optional<double> FinalBest;   //!< Best value at termination
};

//! Configuration for Particle Swarm Optimization.
struct PSOConfig : NDimConfig
{
  size_t       NbParticles   = 40;  //!< Number of particles in the swarm
  double       Omega         = 0.7; //!< Inertia weight (velocity decay)
  double       PhiPersonal   = 1.5; //!< Personal best attraction coefficient
  double       PhiGlobal     = 1.5; //!< Global best attraction coefficient
  double       VelocityClamp = 0.5; //!< Max velocity as fraction of search space
  unsigned int Seed          = 6;   //!< Random seed for reproducibility

  PSOInitMode           InitMode        = PSOInitMode::RandomOnly;
  PSOBoundaryMode       BoundaryMode    = PSOBoundaryMode::Clamp;
  PSOInertiaSchedule    InertiaSchedule = PSOInertiaSchedule::Constant;
  double                OmegaMin        = 0.4; //!< Min inertia for LinearDecay
  uint32_t              MinIterations   = 0;   //!< Completed updates before stagnation checks
  std::optional<double> TargetValue;          //!< Early stop if best <= target (nullopt = disabled)
  double                NoImproveTol   = 0.0; //!< Stagnation tolerance (0 = use Tolerance)
  uint32_t              NoImproveIters = 10;  //!< Stagnation iteration threshold
  double RestartFraction    = 0.0; //!< Fraction of particles to reinitialize (0 = no restarts)
  uint32_t MaxRestarts      = 0;   //!< Maximum restart count (0 = unlimited when fraction > 0)
  uint32_t PolishBudgetPerDim = 50; //!< Max polishing evals per dimension (0 = no polishing)

  //! Default constructor.
  PSOConfig()
      : NDimConfig(1.0e-8, 100, true)
  {
  }

  //! Constructor with parameters.
  PSOConfig(size_t theNbParticles, uint32_t theMaxIter = 100, double theTolerance = 1.0e-8)
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
                           size_t             theMaxPolishEvals,
                           size_t&            theEvalCount)
{
  theEvalCount = 0;

  const size_t aNbDims = thePosition.Size();

  // Fair per-coordinate cap: total budget split across dimensions and passes
  const size_t   aNbPasses = (aNbDims > 1) ? 2 : 1;
  const uint32_t aPerCoordLimit =
    static_cast<uint32_t>(std::max<size_t>(1, theMaxPolishEvals / (aNbDims * aNbPasses)));

  for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
  {
    if (theEvalCount >= theMaxPolishEvals)
    {
      break;
    }

    const uint32_t aMaxIter = static_cast<uint32_t>(
      std::min<size_t>(aPerCoordLimit, theMaxPolishEvals - theEvalCount));
    BrentAlongCoordinate(theFunc,
                         thePosition,
                         aDimIdx,
                          theLowerBounds.At(aDimIdx),
                          theUpperBounds.At(aDimIdx),
                         theValue,
                         theTolerance,
                         aMaxIter,
                         theEvalCount);
  }

  // Second pass for non-separable functions - dimensions may interact
  if (theEvalCount < theMaxPolishEvals && aNbDims > 1)
  {
    for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
    {
      if (theEvalCount >= theMaxPolishEvals)
      {
        break;
      }

      const uint32_t aMaxIter = static_cast<uint32_t>(
        std::min<size_t>(aPerCoordLimit, theMaxPolishEvals - theEvalCount));
      BrentAlongCoordinate(theFunc,
                           thePosition,
                           aDimIdx,
                            theLowerBounds.At(aDimIdx),
                            theUpperBounds.At(aDimIdx),
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
VectorResult PSO(Function&                                        theFunc,
                 const math_Vector&                               theLowerBounds,
                 const math_Vector&                               theUpperBounds,
                 const PSOConfig&                                 theConfig,
                 const NCollection_DynamicArray<PSOSeedParticle>* theSeeds,
                 PSOStats*                                        theStats = nullptr)
{
  VectorResult aResult;

  const size_t aNbDims = theLowerBounds.Size();

  if (!Utils::IsValidConfig(theConfig)
      || !Utils::IsValidBounds(theLowerBounds, theUpperBounds))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const size_t aNbParticles = theConfig.NbParticles;
  const bool isInitModeValid = theConfig.InitMode == PSOInitMode::RandomOnly
                               || theConfig.InitMode == PSOInitMode::SeededOnly
                               || theConfig.InitMode == PSOInitMode::SeededPlusRandom;
  const bool isBoundaryModeValid = theConfig.BoundaryMode == PSOBoundaryMode::Clamp
                                   || theConfig.BoundaryMode == PSOBoundaryMode::Reflect
                                   || theConfig.BoundaryMode == PSOBoundaryMode::Wrap;
  const bool isScheduleValid = theConfig.InertiaSchedule == PSOInertiaSchedule::Constant
                               || theConfig.InertiaSchedule == PSOInertiaSchedule::LinearDecay;
  if (aNbParticles == 0 || !isInitModeValid || !isBoundaryModeValid || !isScheduleValid
      || !std::isfinite(theConfig.Omega) || !std::isfinite(theConfig.OmegaMin)
      || !std::isfinite(theConfig.PhiPersonal) || theConfig.PhiPersonal < 0.0
      || !std::isfinite(theConfig.PhiGlobal) || theConfig.PhiGlobal < 0.0
       || !std::isfinite(theConfig.VelocityClamp) || theConfig.VelocityClamp < 0.0
       || theConfig.MinIterations > theConfig.MaxIterations
      || !std::isfinite(theConfig.NoImproveTol) || theConfig.NoImproveTol < 0.0
      || theConfig.NoImproveIters == 0 || !std::isfinite(theConfig.RestartFraction)
       || theConfig.RestartFraction < 0.0 || theConfig.RestartFraction > 1.0
       || (theConfig.TargetValue.has_value() && !std::isfinite(*theConfig.TargetValue)))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Initialize stats
  PSOStats aLocalStats;
  Utils::CheckedFunction<Function> aCheckedFunc(theFunc);

  auto LowerBound = [&](size_t theIndex) {
    return theLowerBounds.At(theIndex);
  };
  auto UpperBound = [&](size_t theIndex) {
    return theUpperBounds.At(theIndex);
  };

  // Particle data
  struct Particle
  {
    math_Vector Position;
    math_Vector Velocity;
    math_Vector BestPosition;
    std::optional<double> BestValue;
    std::optional<double> CurrentValue;

    Particle(size_t theSize)
        : Position(theSize),
          Velocity(theSize),
          BestPosition(theSize)
    {
    }
  };

  NCollection_DynamicArray<Particle> aSwarm(std::min<size_t>(aNbParticles, 8));
  for (size_t aPartIdx = 0; aPartIdx < aNbParticles; ++aPartIdx)
  {
    aSwarm.Append(Particle(aNbDims));
  }

  // Random number generator
  MathUtils::RandomGenerator aRNG(theConfig.Seed);

  // Compute velocity limits
  math_Vector aVelMax(aNbDims);
  math_Vector aRange(aNbDims);
  for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
  {
    aRange.ChangeAt(aDimIdx)  = UpperBound(aDimIdx) - LowerBound(aDimIdx);
    aVelMax.ChangeAt(aDimIdx) = theConfig.VelocityClamp * aRange.At(aDimIdx);
  }

  // Initialize particles
  math_Vector           aGlobalBest(aNbDims);
  std::optional<double> aGlobalBestValue;

  // Determine how many particles are seeded
  const size_t aNbSeeds = (theSeeds != nullptr) ? theSeeds->Size() : 0;
  size_t       aSeeded  = 0;

  // Validate seed dimensions and index ranges
  if (aNbSeeds > 0)
  {
    for (size_t aSeedIdx = 0; aSeedIdx < aNbSeeds; ++aSeedIdx)
    {
      const PSOSeedParticle& aSeed = theSeeds->Value(aSeedIdx);
      if (aSeed.Position.Size() != aNbDims)
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
      if (aSeed.Velocity.has_value()
          && aSeed.Velocity->Size() != aNbDims)
      {
        aResult.Status = Status::InvalidInput;
        return aResult;
      }
      if (!Utils::IsFinite(aSeed.Position)
          || (aSeed.Velocity.has_value() && !Utils::IsFinite(*aSeed.Velocity))
          || (aSeed.Value.has_value() && !std::isfinite(*aSeed.Value)))
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
    const size_t aNbDirect = std::min(aNbSeeds, aNbParticles);
    for (size_t aSeedIdx = 0; aSeedIdx < aNbDirect; ++aSeedIdx)
    {
      Particle&              aParticle = aSwarm.ChangeValue(aSeedIdx);
      const PSOSeedParticle& aSeed     = theSeeds->Value(aSeedIdx);

      // Clamp seed position to bounds.
      for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
      {
        const double aClamped = MathUtils::Clamp(aSeed.Position.At(aDimIdx),
                                                 LowerBound(aDimIdx),
                                                 UpperBound(aDimIdx));
        aParticle.Position.ChangeAt(aDimIdx) = aClamped;
      }

      // Use seed velocity or generate random
      if (aSeed.Velocity.has_value())
      {
        const math_Vector& aSeedVel = *aSeed.Velocity;
        for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
        {
          aParticle.Velocity.ChangeAt(aDimIdx) =
            MathUtils::Clamp(aSeedVel.At(aDimIdx), -aVelMax.At(aDimIdx), aVelMax.At(aDimIdx));
        }
      }
      else
      {
        for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
        {
          aParticle.Velocity.ChangeAt(aDimIdx) =
            (2.0 * aRNG.NextReal() - 1.0) * aVelMax.At(aDimIdx);
        }
      }

      // Always verify fitness at the actual initial position.
      double aCurrentValue = 0.0;
      if (aCheckedFunc.Value(aParticle.Position, aCurrentValue))
      {
        aParticle.CurrentValue = aCurrentValue;
        aParticle.BestValue    = aCurrentValue;
      }
      ++aLocalStats.NbFunctionEvals;

      aParticle.BestPosition = aParticle.Position;

      if (aParticle.BestValue
          && (!aGlobalBestValue || *aParticle.BestValue < *aGlobalBestValue))
      {
        aGlobalBestValue = *aParticle.BestValue;
        aGlobalBest      = aParticle.BestPosition;
      }
    }
    aSeeded = aNbDirect;

    // For SeededOnly: if seeds < NbParticles, jitter best seeds to fill
    if (theConfig.InitMode == PSOInitMode::SeededOnly && aSeeded < aNbParticles)
    {
      for (size_t aPartIdx = aSeeded; aPartIdx < aNbParticles; ++aPartIdx)
      {
        Particle& aParticle = aSwarm.ChangeValue(aPartIdx);
        // Pick a seed to jitter (round-robin)
        const size_t           aSrcIdx = aPartIdx % aSeeded;
        const PSOSeedParticle& aSrc    = theSeeds->Value(aSrcIdx);

        for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
        {
          const double aJitter =
            (2.0 * aRNG.NextReal() - 1.0) * 0.1 * aRange.At(aDimIdx);
          aParticle.Position.ChangeAt(aDimIdx) =
            MathUtils::Clamp(aSrc.Position.At(aDimIdx) + aJitter,
                             LowerBound(aDimIdx),
                             UpperBound(aDimIdx));
          aParticle.Velocity.ChangeAt(aDimIdx) =
            (2.0 * aRNG.NextReal() - 1.0) * aVelMax.At(aDimIdx);
        }

        double aCurrentValue = 0.0;
        if (aCheckedFunc.Value(aParticle.Position, aCurrentValue))
        {
          aParticle.CurrentValue = aCurrentValue;
          aParticle.BestValue    = aCurrentValue;
        }
        ++aLocalStats.NbFunctionEvals;

        aParticle.BestPosition = aParticle.Position;

        if (aParticle.BestValue
            && (!aGlobalBestValue || *aParticle.BestValue < *aGlobalBestValue))
        {
          aGlobalBestValue = *aParticle.BestValue;
          aGlobalBest      = aParticle.BestPosition;
        }
      }
      aSeeded = aNbParticles;
    }
  }

  // Fill remaining particles randomly
  for (size_t aPartIdx = aSeeded; aPartIdx < aNbParticles; ++aPartIdx)
  {
    Particle& aParticle = aSwarm.ChangeValue(aPartIdx);
    for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
    {
      aParticle.Position.ChangeAt(aDimIdx) =
        LowerBound(aDimIdx) + aRNG.NextReal() * aRange.At(aDimIdx);
      aParticle.Velocity.ChangeAt(aDimIdx) =
        (2.0 * aRNG.NextReal() - 1.0) * aVelMax.At(aDimIdx);
    }

    double aCurrentValue = 0.0;
    if (aCheckedFunc.Value(aParticle.Position, aCurrentValue))
    {
      aParticle.CurrentValue = aCurrentValue;
      aParticle.BestValue    = aCurrentValue;
    }
    ++aLocalStats.NbFunctionEvals;

    aParticle.BestPosition = aParticle.Position;

    if (aParticle.BestValue
        && (!aGlobalBestValue || *aParticle.BestValue < *aGlobalBestValue))
    {
      aGlobalBestValue = *aParticle.BestValue;
      aGlobalBest      = aParticle.BestPosition;
    }
  }

  aLocalStats.InitialBest = aGlobalBestValue;

  if (!aGlobalBestValue)
  {
    aResult.Status = aCheckedFunc.FailureStatus != Status::OK ? aCheckedFunc.FailureStatus
                                                               : Status::NumericalError;
    if (theStats != nullptr)
    {
      *theStats = aLocalStats;
    }
    return aResult;
  }

  // Main PSO loop
  double       aPrevBest        = *aGlobalBestValue;
  uint32_t     aStagnationCount = 0;
  bool         isConverged      = false;
  const double aStagnTol =
    (theConfig.NoImproveTol > 0.0) ? theConfig.NoImproveTol : theConfig.Tolerance;

  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
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
    for (size_t aPartIdx = 0; aPartIdx < aNbParticles; ++aPartIdx)
    {
      Particle& aParticle = aSwarm.ChangeValue(aPartIdx);

      // Update velocity
      for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
      {
        const double aRand1 = aRNG.NextReal();
        const double aRand2 = aRNG.NextReal();

        double aVnew =
          anOmega * aParticle.Velocity.At(aDimIdx)
          + theConfig.PhiPersonal * aRand1
              * (aParticle.BestPosition.At(aDimIdx) - aParticle.Position.At(aDimIdx))
          + theConfig.PhiGlobal * aRand2
              * (aGlobalBest.At(aDimIdx) - aParticle.Position.At(aDimIdx));

        // Clamp velocity
        aVnew = MathUtils::Clamp(aVnew, -aVelMax.At(aDimIdx), aVelMax.At(aDimIdx));
        aParticle.Velocity.ChangeAt(aDimIdx) = aVnew;
      }

      // Update position with boundary handling
      for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
      {
        double aXnew = aParticle.Position.At(aDimIdx) + aParticle.Velocity.At(aDimIdx);

        if (aXnew < LowerBound(aDimIdx) || aXnew > UpperBound(aDimIdx))
        {
          ++aLocalStats.NbBoundaryCorrections;

          switch (theConfig.BoundaryMode)
          {
            case PSOBoundaryMode::Clamp: {
              aXnew = MathUtils::Clamp(aXnew, LowerBound(aDimIdx), UpperBound(aDimIdx));
              aParticle.Velocity.ChangeAt(aDimIdx) = -0.5 * aParticle.Velocity.At(aDimIdx);
              break;
            }
            case PSOBoundaryMode::Reflect: {
              if (aXnew < LowerBound(aDimIdx))
              {
                aXnew = 2.0 * LowerBound(aDimIdx) - aXnew;
              }
              else
              {
                aXnew = 2.0 * UpperBound(aDimIdx) - aXnew;
              }
              // Re-clamp in case reflection overshoots the other bound
              aXnew = MathUtils::Clamp(aXnew, LowerBound(aDimIdx), UpperBound(aDimIdx));
              aParticle.Velocity.ChangeAt(aDimIdx) = -0.5 * aParticle.Velocity.At(aDimIdx);
              break;
            }
            case PSOBoundaryMode::Wrap: {
              const double aRangeSize = aRange.At(aDimIdx);
              aXnew                   = aXnew - LowerBound(aDimIdx);
              aXnew                   = std::fmod(aXnew, aRangeSize);
              if (aXnew < 0.0)
              {
                aXnew += aRangeSize;
              }
              aXnew += LowerBound(aDimIdx);
              break;
            }
          }
        }

        aParticle.Position.ChangeAt(aDimIdx) = aXnew;
      }

      // Evaluate fitness
      double     aCurrentValue  = 0.0;
      const bool isCurrentValid = aCheckedFunc.Value(aParticle.Position, aCurrentValue);
      if (isCurrentValid)
      {
        aParticle.CurrentValue = aCurrentValue;
      }
      else
      {
        aParticle.CurrentValue.reset();
      }
      ++aLocalStats.NbFunctionEvals;

      // Update personal best
      if (aParticle.CurrentValue
          && (!aParticle.BestValue || *aParticle.CurrentValue < *aParticle.BestValue))
      {
        aParticle.BestValue    = *aParticle.CurrentValue;
        aParticle.BestPosition = aParticle.Position;
      }

      // Update global best
      if (aParticle.BestValue && *aParticle.BestValue < *aGlobalBestValue)
      {
        aGlobalBestValue = *aParticle.BestValue;
        aGlobalBest      = aParticle.BestPosition;
      }
    }

    // Check for target value early stop
    if (theConfig.TargetValue.has_value() && *aGlobalBestValue <= *theConfig.TargetValue)
    {
      isConverged = true;
      break;
    }

    // Check for convergence (stagnation) after minimum iterations
    if (aResult.NbIterations > theConfig.MinIterations)
    {
      if (std::abs(*aGlobalBestValue - aPrevBest)
          < aStagnTol * (1.0 + std::abs(*aGlobalBestValue)))
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
            size_t aNbRestart =
              static_cast<size_t>(std::ceil(theConfig.RestartFraction * aNbParticles));
            aNbRestart = std::min(aNbRestart, aNbParticles - 1); // keep at least the best

            // Find best particle index
            std::optional<size_t> aBestIdx;
            for (size_t aFindIdx = 0; aFindIdx < aNbParticles; ++aFindIdx)
            {
              const std::optional<double>& aBestValue = aSwarm.Value(aFindIdx).BestValue;
              if (aBestValue
                  && (!aBestIdx
                      || *aBestValue < *aSwarm.Value(*aBestIdx).BestValue))
              {
                aBestIdx = aFindIdx;
              }
            }

            // Sort by fitness descending (simple selection of worst)
            // Reinitialize aNbRestart worst particles
            NCollection_LinearVector<size_t> aWorstIndices;
            for (size_t aCollIdx = 0; aCollIdx < aNbParticles; ++aCollIdx)
            {
              if (aCollIdx != *aBestIdx)
              {
                aWorstIndices.Append(aCollIdx);
              }
            }
            // Simple approach: sort by BestValue descending, take first aNbRestart
            for (size_t aSortOuter = 0; aSortOuter + 1 < aWorstIndices.Size(); ++aSortOuter)
            {
              for (size_t aSortInner = aSortOuter + 1; aSortInner < aWorstIndices.Size();
                   ++aSortInner)
              {
                const std::optional<double>& anOuterValue =
                  aSwarm.Value(aWorstIndices.Value(aSortOuter)).BestValue;
                const std::optional<double>& anInnerValue =
                  aSwarm.Value(aWorstIndices.Value(aSortInner)).BestValue;
                if (anOuterValue && (!anInnerValue || *anOuterValue < *anInnerValue))
                {
                  const size_t aTmp                     = aWorstIndices.Value(aSortOuter);
                  aWorstIndices.ChangeValue(aSortOuter) = aWorstIndices.Value(aSortInner);
                  aWorstIndices.ChangeValue(aSortInner) = aTmp;
                }
              }
            }

            const size_t aNbToRestart =
              std::min(static_cast<size_t>(aNbRestart), aWorstIndices.Size());
            for (size_t aRestIdx = 0; aRestIdx < aNbToRestart; ++aRestIdx)
            {
              Particle& aRestartPart = aSwarm.ChangeValue(aWorstIndices.Value(aRestIdx));
              for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
              {
                aRestartPart.Position.ChangeAt(aDimIdx) =
                  LowerBound(aDimIdx) + aRNG.NextReal() * aRange.At(aDimIdx);
                aRestartPart.Velocity.ChangeAt(aDimIdx) =
                  (2.0 * aRNG.NextReal() - 1.0) * aVelMax.At(aDimIdx);
              }

              double aCurrentValue = 0.0;
              if (aCheckedFunc.Value(aRestartPart.Position, aCurrentValue))
              {
                aRestartPart.CurrentValue = aCurrentValue;
                aRestartPart.BestValue    = aCurrentValue;
              }
              else
              {
                aRestartPart.CurrentValue.reset();
                aRestartPart.BestValue.reset();
              }
              ++aLocalStats.NbFunctionEvals;

              aRestartPart.BestPosition = aRestartPart.Position;

              if (aRestartPart.BestValue && *aRestartPart.BestValue < *aGlobalBestValue)
              {
                aGlobalBestValue = *aRestartPart.BestValue;
                aGlobalBest      = aRestartPart.BestPosition;
              }
            }
          }
          else
          {
            // No restart budget - converged
            isConverged = true;
            break;
          }
        }
      }
      else
      {
        aStagnationCount = 0;
      }
    }
    aPrevBest = *aGlobalBestValue;
  }

  // Polish the global best using coordinate-wise Brent's method
  size_t aPolishEvals = 0;
  if (theConfig.PolishBudgetPerDim > 0)
  {
    PolishCoordinateWise(aCheckedFunc,
                         aGlobalBest,
                         *aGlobalBestValue,
                         theLowerBounds,
                         theUpperBounds,
                         theConfig.Tolerance,
                         theConfig.PolishBudgetPerDim * aNbDims,
                         aPolishEvals);
    aLocalStats.NbFunctionEvals += aPolishEvals;
  }

  aLocalStats.NbIterations = aResult.NbIterations;
  aLocalStats.FinalBest    = *aGlobalBestValue;

  if (theStats != nullptr)
  {
    *theStats = aLocalStats;
  }

  aResult.Status   = isConverged ? Status::OK : Status::MaxIterations;
  aResult.Solution = aGlobalBest;
  aResult.Value    = *aGlobalBestValue;
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
