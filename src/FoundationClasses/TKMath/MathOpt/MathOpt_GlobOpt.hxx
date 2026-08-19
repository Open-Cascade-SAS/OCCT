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

#ifndef _MathOpt_GlobOpt_HeaderFile
#define _MathOpt_GlobOpt_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Random.hxx>
#include <MathOpt_PSO.hxx>
#include <MathOpt_BFGS.hxx>
#include <MathOpt_Powell.hxx>
#include <MathUtils_Core.hxx>
#include "MathOpt_Utils.hxx"

#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>

#include <cmath>
#include <optional>

namespace MathOpt
{
using namespace MathUtils;

//! Global optimization strategy selection.
enum class GlobalStrategy
{
  PSO,                  //!< Particle Swarm Optimization only
  MultiStart,           //!< Multiple local optimizations from random starts
  PSOHybrid,            //!< PSO followed by local refinement
  DifferentialEvolution //!< Differential Evolution algorithm
};

//! Configuration for global optimization.
struct GlobalConfig : NDimConfig
{
  GlobalStrategy Strategy           = GlobalStrategy::PSOHybrid; //!< Algorithm to use
  size_t         NbPopulation       = 40;                        //!< Population/swarm size
  size_t         NbStarts           = 10;  //!< Number of random starts (for MultiStart)
  double         MutationScale      = 0.8; //!< Mutation scale (for DE)
  double         CrossoverProb      = 0.9; //!< Crossover probability (for DE)
  unsigned int   Seed               = 6;   //!< Random seed
  uint32_t       PolishBudgetPerDim = 50;  //!< Max polishing evals per dimension (0 = no polishing)

  //! Default constructor.
  GlobalConfig()
      : NDimConfig(1.0e-8, 200, true)
  {
  }

  //! Constructor with strategy.
  GlobalConfig(GlobalStrategy theStrategy, uint32_t theMaxIter = 200)
      : NDimConfig(1.0e-8, theMaxIter, true),
        Strategy(theStrategy)
  {
  }
};

//! Differential Evolution algorithm for global optimization.
//!
//! DE is a stochastic, population-based optimization algorithm.
//! It uses mutation, crossover, and selection operations to evolve
//! a population of candidate solutions.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds
//! @param theUpperBounds upper bounds
//! @param theConfig optimization configuration
//! @return result containing best solution found
template <typename Function>
VectorResult DifferentialEvolution(Function&           theFunc,
                                   const math_Vector&  theLowerBounds,
                                   const math_Vector&  theUpperBounds,
                                   const GlobalConfig& theConfig = GlobalConfig())
{
  VectorResult aResult;

  const size_t aNbDims = theLowerBounds.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsValidBounds(theLowerBounds, theUpperBounds)
      || !std::isfinite(theConfig.MutationScale) || theConfig.MutationScale < 0.0
      || !std::isfinite(theConfig.CrossoverProb) || theConfig.CrossoverProb < 0.0
      || theConfig.CrossoverProb > 1.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const size_t aNbPop = theConfig.NbPopulation;
  if (aNbPop < 4)
  {
    // DE requires at least 4 population members for mutation (3 distinct + target)
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double                     aMutScale  = theConfig.MutationScale;
  const double                     aCrossProb = theConfig.CrossoverProb;
  Utils::CheckedFunction<Function> aCheckedFunc(theFunc);

  auto LowerBound = [&](size_t theIndex) { return theLowerBounds.At(theIndex); };
  auto UpperBound = [&](size_t theIndex) { return theUpperBounds.At(theIndex); };

  // Random number generator
  MathUtils::RandomGenerator aRNG(theConfig.Seed);

  // Population: vector of candidate solutions
  NCollection_DynamicArray<math_Vector>           aPopulation(std::min<size_t>(aNbPop, 8));
  NCollection_LinearVector<std::optional<double>> aFitness;

  // Initialize population
  for (size_t aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
  {
    aPopulation.Append(math_Vector(aNbDims));
    aFitness.Append(std::nullopt);
    for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
    {
      const double aRandVal = aRNG.NextReal();
      aPopulation.ChangeValue(aMemberIdx).ChangeAt(aDimIdx) =
        LowerBound(aDimIdx) + aRandVal * (UpperBound(aDimIdx) - LowerBound(aDimIdx));
    }

    double aFitVal = 0.0;
    if (aCheckedFunc.Value(aPopulation.Value(aMemberIdx), aFitVal))
    {
      aFitness.ChangeValue(aMemberIdx) = aFitVal;
    }
  }

  // Find best
  std::optional<size_t> aBestIdx;
  std::optional<double> aBestValue;
  for (size_t aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
  {
    const std::optional<double>& aMemberFitness = aFitness.Value(aMemberIdx);
    if (aMemberFitness.has_value() && (!aBestValue || *aMemberFitness < *aBestValue))
    {
      aBestValue = *aMemberFitness;
      aBestIdx   = aMemberIdx;
    }
  }
  if (!aBestIdx)
  {
    aResult.Status = aCheckedFunc.FailureStatus != Status::OK ? aCheckedFunc.FailureStatus
                                                               : Status::NumericalError;
    return aResult;
  }

  // Trial vector
  math_Vector aTrial(aNbDims);

  // Evolution loop
  bool hasConverged = false;
  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    for (size_t aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
    {
      // Select 3 distinct random indices different from current member
      size_t anIdxA, anIdxB, anIdxC;
      do
      {
        anIdxA = static_cast<size_t>(aRNG.NextReal() * aNbPop);
      } while (anIdxA == aMemberIdx);
      do
      {
        anIdxB = static_cast<size_t>(aRNG.NextReal() * aNbPop);
      } while (anIdxB == aMemberIdx || anIdxB == anIdxA);
      do
      {
        anIdxC = static_cast<size_t>(aRNG.NextReal() * aNbPop);
      } while (anIdxC == aMemberIdx || anIdxC == anIdxA || anIdxC == anIdxB);

      // Mutation and crossover
      const size_t aJRand = static_cast<size_t>(aRNG.NextReal() * aNbDims);

      for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
      {
        if (aRNG.NextReal() < aCrossProb || aDimIdx == aJRand)
        {
          // Mutation: DE/rand/1
          const double aMutVal =
            aPopulation.Value(anIdxA).At(aDimIdx)
            + aMutScale
                * (aPopulation.Value(anIdxB).At(aDimIdx) - aPopulation.Value(anIdxC).At(aDimIdx));
          // Clamp to bounds
          aTrial.ChangeAt(aDimIdx) =
            MathUtils::Clamp(aMutVal, LowerBound(aDimIdx), UpperBound(aDimIdx));
        }
        else
        {
          aTrial.ChangeAt(aDimIdx) = aPopulation.Value(aMemberIdx).At(aDimIdx);
        }
      }

      // Selection
      double     aTrialFitness = 0.0;
      const bool isTrialValid  = aCheckedFunc.Value(aTrial, aTrialFitness);

      if (isTrialValid
          && (!aFitness.Value(aMemberIdx) || aTrialFitness <= *aFitness.Value(aMemberIdx)))
      {
        aPopulation.ChangeValue(aMemberIdx) = aTrial;
        aFitness.ChangeValue(aMemberIdx)    = aTrialFitness;

        if (aTrialFitness < *aBestValue)
        {
          aBestValue = aTrialFitness;
          aBestIdx   = aMemberIdx;
        }
      }
    }

    // Check convergence
    double aMaxDiff = 0.0;
    size_t aNbValid = 0;
    for (size_t aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
    {
      if (aFitness.Value(aMemberIdx))
      {
        ++aNbValid;
        aMaxDiff = std::max(aMaxDiff, std::abs(*aFitness.Value(aMemberIdx) - *aBestValue));
      }
    }

    if (aNbValid == aNbPop && aMaxDiff < theConfig.Tolerance)
    {
      hasConverged = true;
      break;
    }
  }

  // Polish the best solution using coordinate-wise Brent's method
  math_Vector aPolished      = aPopulation.Value(*aBestIdx);
  double      aPolishedValue = *aBestValue;
  if (theConfig.PolishBudgetPerDim > 0)
  {
    size_t aPolishEvals = 0;
    PolishCoordinateWise(aCheckedFunc,
                         aPolished,
                         aPolishedValue,
                         theLowerBounds,
                         theUpperBounds,
                         theConfig.Tolerance,
                         theConfig.PolishBudgetPerDim * aNbDims,
                         aPolishEvals);
  }

  aResult.Status   = hasConverged ? Status::OK : Status::MaxIterations;
  aResult.Solution = aPolished;
  aResult.Value    = aPolishedValue;
  return aResult;
}

//! Multi-start local optimization.
//!
//! Runs multiple local optimizations from random starting points
//! and returns the best result found. Uses Powell's method for
//! local optimization (gradient-free).
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds
//! @param theUpperBounds upper bounds
//! @param theConfig optimization configuration
//! @return result containing best solution found
template <typename Function>
VectorResult MultiStart(Function&           theFunc,
                        const math_Vector&  theLowerBounds,
                        const math_Vector&  theUpperBounds,
                        const GlobalConfig& theConfig = GlobalConfig())
{
  VectorResult aResult;

  const size_t aNbDims = theLowerBounds.Size();

  if (!Utils::IsValidConfig(theConfig) || !Utils::IsValidBounds(theLowerBounds, theUpperBounds)
      || theConfig.NbStarts == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  MathUtils::RandomGenerator                         aRNG(theConfig.Seed);
  Utils::CheckedFunction<Function>                   aCheckedFunc(theFunc);
  Utils::BoundedFunction<Utils::CheckedFunction<Function>> aBoundedFunc(aCheckedFunc,
                                                                         theLowerBounds,
                                                                         theUpperBounds);

  auto LowerBound = [&](size_t theIndex) { return theLowerBounds.At(theIndex); };
  auto UpperBound = [&](size_t theIndex) { return theUpperBounds.At(theIndex); };

  std::optional<math_Vector> aBestSolution;
  std::optional<double>      aBestValue;

  // Configure Powell optimization for local refinement
  Config aPowellConfig;
  aPowellConfig.Tolerance     = theConfig.Tolerance;
  aPowellConfig.XTolerance    = theConfig.Tolerance;
  aPowellConfig.FTolerance    = theConfig.Tolerance;
  aPowellConfig.MaxIterations = theConfig.MaxIterations / theConfig.NbStarts;
  if (aPowellConfig.MaxIterations < 10)
  {
    aPowellConfig.MaxIterations = 10;
  }

  for (size_t aStartIdx = 0; aStartIdx < theConfig.NbStarts; ++aStartIdx)
  {
    // Random starting point within bounds
    math_Vector aStart(aNbDims);
    for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
    {
      const double aRandVal = aRNG.NextReal();
      aStart.ChangeAt(aDimIdx) =
        LowerBound(aDimIdx) + aRandVal * (UpperBound(aDimIdx) - LowerBound(aDimIdx));
    }

    // Run local optimization from this starting point
    VectorResult aLocalResult = Powell(aBoundedFunc, aStart, aPowellConfig);

    if (aLocalResult.Solution.has_value() && aLocalResult.Value.has_value())
    {
      // Powell's internal coordinates are unconstrained; map its result back to the box.
      math_Vector aSol = *aLocalResult.Solution;
      for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
      {
        aSol.ChangeAt(aDimIdx) =
          MathUtils::Clamp(aSol.At(aDimIdx), LowerBound(aDimIdx), UpperBound(aDimIdx));
      }

      // Re-evaluate at clamped point
      double aValue;
      if (aCheckedFunc.Value(aSol, aValue) && (!aBestValue || aValue < *aBestValue))
      {
        aBestValue    = aValue;
        aBestSolution = aSol;
      }
    }
    else
    {
      // Powell failed, just evaluate at starting point
      double aValue;
      if (aCheckedFunc.Value(aStart, aValue) && (!aBestValue || aValue < *aBestValue))
      {
        aBestValue    = aValue;
        aBestSolution = aStart;
      }
    }
  }

  if (!aBestValue)
  {
    aResult.Status = aCheckedFunc.FailureStatus != Status::OK ? aCheckedFunc.FailureStatus
                                                               : Status::NumericalError;
    return aResult;
  }

  aResult.Status   = Status::OK;
  aResult.Solution = *aBestSolution;
  aResult.Value    = *aBestValue;
  return aResult;
}

//! Unified global optimization interface.
//!
//! Selects appropriate algorithm based on configuration and
//! provides a consistent interface for all global optimization methods.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theConfig solver configuration
//! @return result containing best solution found
template <typename Function>
VectorResult GlobalMinimum(Function&           theFunc,
                           const math_Vector&  theLowerBounds,
                           const math_Vector&  theUpperBounds,
                           const GlobalConfig& theConfig = GlobalConfig())
{
  return GlobalMinimum(theFunc,
                       theLowerBounds,
                       theUpperBounds,
                       theConfig,
                       nullptr,
                       nullptr,
                       nullptr);
}

//! Unified global optimization interface with PSO-specific options.
//!
//! For PSO and PSOHybrid strategies, uses the provided PSO configuration,
//! seed particles, and stats output. For other strategies, these are ignored.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to minimize
//! @param theLowerBounds lower bounds for each variable
//! @param theUpperBounds upper bounds for each variable
//! @param theConfig solver configuration
//! @param thePSOConfig optional PSO-specific configuration (overrides auto-generated)
//! @param theSeeds optional seed particles for PSO initialization
//! @param theStats optional PSO statistics output
//! @return result containing best solution found
template <typename Function>
VectorResult GlobalMinimum(Function&                                        theFunc,
                           const math_Vector&                               theLowerBounds,
                           const math_Vector&                               theUpperBounds,
                           const GlobalConfig&                              theConfig,
                           const PSOConfig*                                 thePSOConfig,
                           const NCollection_DynamicArray<PSOSeedParticle>* theSeeds = nullptr,
                           PSOStats*                                        theStats = nullptr)
{
  if (!Utils::IsValidConfig(theConfig) || !Utils::IsValidBounds(theLowerBounds, theUpperBounds))
  {
    VectorResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  switch (theConfig.Strategy)
  {
    case GlobalStrategy::PSO: {
      PSOConfig aPSOConfig;
      if (thePSOConfig != nullptr)
      {
        aPSOConfig = *thePSOConfig;
      }
      else
      {
        aPSOConfig.NbParticles        = theConfig.NbPopulation;
        aPSOConfig.MaxIterations      = theConfig.MaxIterations;
        aPSOConfig.Tolerance          = theConfig.Tolerance;
        aPSOConfig.Seed               = theConfig.Seed;
        aPSOConfig.PolishBudgetPerDim = theConfig.PolishBudgetPerDim;
      }
      return PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig, theSeeds, theStats);
    }

    case GlobalStrategy::MultiStart:
      return MultiStart(theFunc, theLowerBounds, theUpperBounds, theConfig);

    case GlobalStrategy::PSOHybrid: {
      // Run PSO first for global exploration
      PSOConfig aPSOConfig;
      if (thePSOConfig != nullptr)
      {
        // Honor caller's PSO configuration fully
        aPSOConfig = *thePSOConfig;
      }
      else
      {
        // Auto-generate PSO config from GlobalConfig:
        // use half iterations and relaxed tolerance for the PSO phase
        aPSOConfig.NbParticles        = theConfig.NbPopulation;
        aPSOConfig.MaxIterations      = std::max(uint32_t{1}, theConfig.MaxIterations / 2);
        aPSOConfig.Tolerance          = theConfig.Tolerance * 10.0;
        aPSOConfig.Seed               = theConfig.Seed;
        aPSOConfig.PolishBudgetPerDim = theConfig.PolishBudgetPerDim;
      }

      VectorResult aPSOResult =
        PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig, theSeeds, theStats);

      if (!aPSOResult.Solution.has_value() || !aPSOResult.Value.has_value())
      {
        return aPSOResult;
      }

      // Local refinement using Powell (gradient-free)
      Config aPowellConfig;
      aPowellConfig.Tolerance     = theConfig.Tolerance;
      aPowellConfig.XTolerance    = theConfig.Tolerance;
      aPowellConfig.FTolerance    = theConfig.Tolerance;
      aPowellConfig.MaxIterations = std::max(uint32_t{1}, theConfig.MaxIterations / 2);

      Utils::BoundedFunction<Function> aBoundedFunc(theFunc, theLowerBounds, theUpperBounds);
      VectorResult aLocalResult = Powell(aBoundedFunc, *aPSOResult.Solution, aPowellConfig);

      if (aLocalResult.Status == Status::CallbackError
          || aLocalResult.Status == Status::NumericalError)
      {
        return aLocalResult;
      }

      if (aLocalResult.IsDone() && aLocalResult.Solution && aPSOResult.Value)
      {
        // Clamp to bounds
        const size_t aNbDims = theLowerBounds.Size();
        math_Vector  aSol    = *aLocalResult.Solution;
        for (size_t aDimIdx = 0; aDimIdx < aNbDims; ++aDimIdx)
        {
          aSol.ChangeAt(aDimIdx) = MathUtils::Clamp(aSol.At(aDimIdx),
                                                    theLowerBounds.At(aDimIdx),
                                                    theUpperBounds.At(aDimIdx));
        }
        aLocalResult.Solution = aSol;

        // Re-evaluate at clamped point
        double aValue;
        if (Utils::Value(aBoundedFunc, aSol, aValue))
        {
          aLocalResult.Value = aValue;
          if (aValue < *aPSOResult.Value)
          {
            return aLocalResult;
          }
          // The hybrid phase converged even when the retained PSO candidate is better.
          aPSOResult.Status = Status::OK;
        }
        else
        {
          aLocalResult.Status = Status::NumericalError;
          return aLocalResult;
        }
      }

      return aPSOResult;
    }

    case GlobalStrategy::DifferentialEvolution:
      return DifferentialEvolution(theFunc, theLowerBounds, theUpperBounds, theConfig);

    default:
      VectorResult aResult;
      aResult.Status = Status::InvalidInput;
      return aResult;
  }
}

} // namespace MathOpt

#endif // _MathOpt_GlobOpt_HeaderFile
