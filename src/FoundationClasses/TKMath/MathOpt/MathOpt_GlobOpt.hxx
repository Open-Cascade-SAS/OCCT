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
#include <MathOpt_PSO.hxx>
#include <MathOpt_BFGS.hxx>
#include <MathOpt_Powell.hxx>
#include <MathUtils_Core.hxx>
#include <math_BullardGenerator.hxx>

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
  GlobalStrategy Strategy      = GlobalStrategy::PSOHybrid; //!< Algorithm to use
  int            NbPopulation  = 40;                        //!< Population/swarm size
  int            NbStarts      = 10;    //!< Number of random starts (for MultiStart)
  double         MutationScale = 0.8;   //!< Mutation scale (for DE)
  double         CrossoverProb = 0.9;   //!< Crossover probability (for DE)
  unsigned int   Seed          = 12345; //!< Random seed

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

  const int aLower = theLowerBounds.Lower();
  const int aUpper = theLowerBounds.Upper();
  const int aN     = aUpper - aLower + 1;

  if (theUpperBounds.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int    aNP = theConfig.NbPopulation;
  const double aF  = theConfig.MutationScale;
  const double aCR = theConfig.CrossoverProb;

  // Random number generator
  math_BullardGenerator aRNG(theConfig.Seed);

  // Population: vector of candidate solutions
  NCollection_Vector<math_Vector> aPopulation;
  math_Vector                     aFitness(0, aNP - 1);

  // Initialize population
  for (int i = 0; i < aNP; ++i)
  {
    aPopulation.Append(math_Vector(aLower, aUpper));
    for (int j = aLower; j <= aUpper; ++j)
    {
      double r = aRNG.NextReal();
      aPopulation.ChangeValue(i)(j) =
        theLowerBounds(j) + r * (theUpperBounds(j) - theLowerBounds(j));
    }

    double aFit;
    if (!theFunc.Value(aPopulation.Value(i), aFit))
    {
      aFit = std::numeric_limits<double>::max();
    }
    aFitness(i) = aFit;
  }

  // Find best
  int    aBestIdx   = 0;
  double aBestValue = aFitness(0);
  for (int i = 1; i < aNP; ++i)
  {
    if (aFitness(i) < aBestValue)
    {
      aBestValue = aFitness(i);
      aBestIdx   = i;
    }
  }

  // Trial vector
  math_Vector aTrial(aLower, aUpper);

  // Evolution loop
  for (int anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    for (int i = 0; i < aNP; ++i)
    {
      // Select 3 distinct random indices different from i
      int a, b, c;
      do
      {
        a = static_cast<int>(aRNG.NextReal() * aNP);
      } while (a == i);
      do
      {
        b = static_cast<int>(aRNG.NextReal() * aNP);
      } while (b == i || b == a);
      do
      {
        c = static_cast<int>(aRNG.NextReal() * aNP);
      } while (c == i || c == a || c == b);

      // Mutation and crossover
      int jRand = aLower + static_cast<int>(aRNG.NextReal() * aN);

      for (int j = aLower; j <= aUpper; ++j)
      {
        if (aRNG.NextReal() < aCR || j == jRand)
        {
          // Mutation: DE/rand/1
          double aVal =
            aPopulation.Value(a)(j) + aF * (aPopulation.Value(b)(j) - aPopulation.Value(c)(j));
          // Clamp to bounds
          aTrial(j) = MathUtils::Clamp(aVal, theLowerBounds(j), theUpperBounds(j));
        }
        else
        {
          aTrial(j) = aPopulation.Value(i)(j);
        }
      }

      // Selection
      double aTrialFitness;
      if (!theFunc.Value(aTrial, aTrialFitness))
      {
        aTrialFitness = std::numeric_limits<double>::max();
      }

      if (aTrialFitness <= aFitness(i))
      {
        aPopulation.ChangeValue(i) = aTrial;
        aFitness(i)                = aTrialFitness;

        if (aTrialFitness < aBestValue)
        {
          aBestValue = aTrialFitness;
          aBestIdx   = i;
        }
      }
    }

    // Check convergence
    double aMaxDiff = 0.0;
    for (int i = 0; i < aNP; ++i)
    {
      aMaxDiff = std::max(aMaxDiff, std::abs(aFitness(i) - aBestValue));
    }

    if (aMaxDiff < theConfig.Tolerance)
    {
      break;
    }
  }

  aResult.Status   = Status::OK;
  aResult.Solution = aPopulation.Value(aBestIdx);
  aResult.Value    = aBestValue;
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

  const int aLower = theLowerBounds.Lower();
  const int aUpper = theLowerBounds.Upper();
  const int aN     = aUpper - aLower + 1;

  if (theUpperBounds.Length() != aN)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  math_BullardGenerator aRNG(theConfig.Seed);

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

  for (int s = 0; s < theConfig.NbStarts; ++s)
  {
    // Random starting point within bounds
    math_Vector aStart(aLower, aUpper);
    for (int i = aLower; i <= aUpper; ++i)
    {
      double r  = aRNG.NextReal();
      aStart(i) = theLowerBounds(i) + r * (theUpperBounds(i) - theLowerBounds(i));
    }

    // Run local optimization from this starting point
    VectorResult aLocalResult = Powell(theFunc, aStart, aPowellConfig);

    if (aLocalResult.IsDone() && aLocalResult.Value)
    {
      // Clamp solution to bounds
      math_Vector aSol = *aLocalResult.Solution;
      for (int i = aLower; i <= aUpper; ++i)
      {
        aSol(i) = MathUtils::Clamp(aSol(i), theLowerBounds(i), theUpperBounds(i));
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
  switch (theConfig.Strategy)
  {
    case GlobalStrategy::PSO: {
      PSOConfig aPSOConfig;
      aPSOConfig.NbParticles   = theConfig.NbPopulation;
      aPSOConfig.MaxIterations = theConfig.MaxIterations;
      aPSOConfig.Tolerance     = theConfig.Tolerance;
      aPSOConfig.Seed          = theConfig.Seed;
      return PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig);
    }

    case GlobalStrategy::MultiStart:
      return MultiStart(theFunc, theLowerBounds, theUpperBounds, theConfig);

    case GlobalStrategy::PSOHybrid: {
      // Run PSO first for global exploration
      PSOConfig aPSOConfig;
      aPSOConfig.NbParticles   = theConfig.NbPopulation;
      aPSOConfig.MaxIterations = theConfig.MaxIterations / 2;
      aPSOConfig.Tolerance     = theConfig.Tolerance * 10.0;
      aPSOConfig.Seed          = theConfig.Seed;

      VectorResult aPSOResult = PSO(theFunc, theLowerBounds, theUpperBounds, aPSOConfig);

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
        for (int i = aLower; i <= aUpper; ++i)
        {
          aSol(i) = MathUtils::Clamp(aSol(i), theLowerBounds(i), theUpperBounds(i));
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
