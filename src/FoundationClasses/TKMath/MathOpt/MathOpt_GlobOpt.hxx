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

#include <NCollection_Vector.hxx>

#include <cmath>

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
  int            NbPopulation       = 40;                        //!< Population/swarm size
  int            NbStarts           = 10;  //!< Number of random starts (for MultiStart)
  double         MutationScale      = 0.8; //!< Mutation scale (for DE)
  double         CrossoverProb      = 0.9; //!< Crossover probability (for DE)
  unsigned int   Seed               = 6;   //!< Random seed
  int            PolishBudgetPerDim = 50;  //!< Max polishing evals per dimension (0 = no polishing)

  //! Default constructor.
  GlobalConfig()
      : NDimConfig(1.0e-8, 200, true)
  {
  }

  //! Constructor with strategy.
  GlobalConfig(GlobalStrategy theStrategy, int theMaxIter = 200)
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

  const int aLower  = theLowerBounds.Lower();
  const int aUpper  = theLowerBounds.Upper();
  const int aNbDims = aUpper - aLower + 1;

  if (theUpperBounds.Length() != aNbDims)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aNbPop = theConfig.NbPopulation;
  if (aNbPop < 4)
  {
    // DE requires at least 4 population members for mutation (3 distinct + target)
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const double aMutScale  = theConfig.MutationScale;
  const double aCrossProb = theConfig.CrossoverProb;

  // Random number generator
  MathUtils::RandomGenerator aRNG(theConfig.Seed);

  // Population: vector of candidate solutions
  NCollection_Vector<math_Vector> aPopulation;
  math_Vector                     aFitness(0, aNbPop - 1);

  // Initialize population
  for (int aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
  {
    aPopulation.Append(math_Vector(aLower, aUpper));
    for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
    {
      const double aRandVal = aRNG.NextReal();
      aPopulation.ChangeValue(aMemberIdx)(aDimIdx) =
        theLowerBounds(aDimIdx) + aRandVal * (theUpperBounds(aDimIdx) - theLowerBounds(aDimIdx));
    }

    double aFitVal;
    if (!theFunc.Value(aPopulation.Value(aMemberIdx), aFitVal))
    {
      aFitVal = std::numeric_limits<double>::max();
    }
    aFitness(aMemberIdx) = aFitVal;
  }

  // Find best
  int    aBestIdx   = 0;
  double aBestValue = aFitness(0);
  for (int aMemberIdx = 1; aMemberIdx < aNbPop; ++aMemberIdx)
  {
    if (aFitness(aMemberIdx) < aBestValue)
    {
      aBestValue = aFitness(aMemberIdx);
      aBestIdx   = aMemberIdx;
    }
  }

  // Trial vector
  math_Vector aTrial(aLower, aUpper);

  // Evolution loop
  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    for (int aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
    {
      // Select 3 distinct random indices different from current member
      int anIdxA, anIdxB, anIdxC;
      do
      {
        anIdxA = static_cast<int>(aRNG.NextReal() * aNbPop);
      } while (anIdxA == aMemberIdx);
      do
      {
        anIdxB = static_cast<int>(aRNG.NextReal() * aNbPop);
      } while (anIdxB == aMemberIdx || anIdxB == anIdxA);
      do
      {
        anIdxC = static_cast<int>(aRNG.NextReal() * aNbPop);
      } while (anIdxC == aMemberIdx || anIdxC == anIdxA || anIdxC == anIdxB);

      // Mutation and crossover
      const int aJRand = aLower + static_cast<int>(aRNG.NextReal() * aNbDims);

      for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
      {
        if (aRNG.NextReal() < aCrossProb || aDimIdx == aJRand)
        {
          // Mutation: DE/rand/1
          const double aMutVal =
            aPopulation.Value(anIdxA)(aDimIdx)
            + aMutScale * (aPopulation.Value(anIdxB)(aDimIdx) - aPopulation.Value(anIdxC)(aDimIdx));
          // Clamp to bounds
          aTrial(aDimIdx) =
            MathUtils::Clamp(aMutVal, theLowerBounds(aDimIdx), theUpperBounds(aDimIdx));
        }
        else
        {
          aTrial(aDimIdx) = aPopulation.Value(aMemberIdx)(aDimIdx);
        }
      }

      // Selection
      double aTrialFitness;
      if (!theFunc.Value(aTrial, aTrialFitness))
      {
        aTrialFitness = std::numeric_limits<double>::max();
      }

      if (aTrialFitness <= aFitness(aMemberIdx))
      {
        aPopulation.ChangeValue(aMemberIdx) = aTrial;
        aFitness(aMemberIdx)                = aTrialFitness;

        if (aTrialFitness < aBestValue)
        {
          aBestValue = aTrialFitness;
          aBestIdx   = aMemberIdx;
        }
      }
    }

    // Check convergence
    double aMaxDiff = 0.0;
    for (int aMemberIdx = 0; aMemberIdx < aNbPop; ++aMemberIdx)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aFitness(aMemberIdx) - aBestValue));
    }

    if (aMaxDiff < theConfig.Tolerance)
    {
      break;
    }
  }

  // Polish the best solution using coordinate-wise Brent's method
  math_Vector aPolished      = aPopulation.Value(aBestIdx);
  double      aPolishedValue = aBestValue;
  if (theConfig.PolishBudgetPerDim > 0)
  {
    int aPolishEvals = 0;
    PolishCoordinateWise(theFunc,
                         aPolished,
                         aPolishedValue,
                         theLowerBounds,
                         theUpperBounds,
                         theConfig.Tolerance,
                         theConfig.PolishBudgetPerDim * aNbDims,
                         aPolishEvals);
  }

  aResult.Status   = Status::OK;
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

  const int aLower  = theLowerBounds.Lower();
  const int aUpper  = theLowerBounds.Upper();
  const int aNbDims = aUpper - aLower + 1;

  if (theUpperBounds.Length() != aNbDims)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  MathUtils::RandomGenerator aRNG(theConfig.Seed);

  math_Vector aBestSolution(aLower, aUpper);
  double      aBestValue = std::numeric_limits<double>::max();
  bool        aFound     = false;

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

  for (int aStartIdx = 0; aStartIdx < theConfig.NbStarts; ++aStartIdx)
  {
    // Random starting point within bounds
    math_Vector aStart(aLower, aUpper);
    for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
    {
      const double aRandVal = aRNG.NextReal();
      aStart(aDimIdx) =
        theLowerBounds(aDimIdx) + aRandVal * (theUpperBounds(aDimIdx) - theLowerBounds(aDimIdx));
    }

    // Run local optimization from this starting point
    VectorResult aLocalResult = Powell(theFunc, aStart, aPowellConfig);

    if (aLocalResult.IsDone() && aLocalResult.Value)
    {
      // Clamp solution to bounds
      math_Vector aSol = *aLocalResult.Solution;
      for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
      {
        aSol(aDimIdx) =
          MathUtils::Clamp(aSol(aDimIdx), theLowerBounds(aDimIdx), theUpperBounds(aDimIdx));
      }

      // Re-evaluate at clamped point
      double aValue;
      if (theFunc.Value(aSol, aValue) && aValue < aBestValue)
      {
        aBestValue    = aValue;
        aBestSolution = aSol;
        aFound        = true;
      }
    }
    else
    {
      // Powell failed, just evaluate at starting point
      double aValue;
      if (theFunc.Value(aStart, aValue) && aValue < aBestValue)
      {
        aBestValue    = aValue;
        aBestSolution = aStart;
        aFound        = true;
      }
    }
  }

  if (!aFound)
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  aResult.Status   = Status::OK;
  aResult.Solution = aBestSolution;
  aResult.Value    = aBestValue;
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
VectorResult GlobalMinimum(Function&                                  theFunc,
                           const math_Vector&                         theLowerBounds,
                           const math_Vector&                         theUpperBounds,
                           const GlobalConfig&                        theConfig,
                           const PSOConfig*                           thePSOConfig,
                           const NCollection_Vector<PSOSeedParticle>* theSeeds = nullptr,
                           PSOStats*                                  theStats = nullptr)
{
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
        aPSOConfig.MaxIterations      = theConfig.MaxIterations / 2;
        aPSOConfig.Tolerance          = theConfig.Tolerance * 10.0;
        aPSOConfig.Seed               = theConfig.Seed;
        aPSOConfig.PolishBudgetPerDim = theConfig.PolishBudgetPerDim;
      }

      VectorResult aPSOResult =
        PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig, theSeeds, theStats);

      if (!aPSOResult.IsDone() || !aPSOResult.Solution)
      {
        return aPSOResult;
      }

      // Local refinement using Powell (gradient-free)
      Config aPowellConfig;
      aPowellConfig.Tolerance     = theConfig.Tolerance;
      aPowellConfig.XTolerance    = theConfig.Tolerance;
      aPowellConfig.FTolerance    = theConfig.Tolerance;
      aPowellConfig.MaxIterations = theConfig.MaxIterations / 2;

      VectorResult aLocalResult = Powell(theFunc, *aPSOResult.Solution, aPowellConfig);

      if (aLocalResult.IsDone() && aLocalResult.Value && aPSOResult.Value
          && *aLocalResult.Value < *aPSOResult.Value)
      {
        // Clamp to bounds
        const int   aLower = theLowerBounds.Lower();
        const int   aUpper = theLowerBounds.Upper();
        math_Vector aSol   = *aLocalResult.Solution;
        for (int aDimIdx = aLower; aDimIdx <= aUpper; ++aDimIdx)
        {
          aSol(aDimIdx) =
            MathUtils::Clamp(aSol(aDimIdx), theLowerBounds(aDimIdx), theUpperBounds(aDimIdx));
        }
        aLocalResult.Solution = aSol;

        // Re-evaluate at clamped point
        double aValue;
        if (theFunc.Value(aSol, aValue))
        {
          aLocalResult.Value = aValue;
        }
        return aLocalResult;
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
