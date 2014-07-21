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

#ifndef _math_PSO_HeaderFile
#define _math_PSO_HeaderFile

#include <math_BullardGenerator.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>

class math_PSOParticlesPool;

//! In this class implemented variation of Particle Swarm Optimization (PSO) method.
//! A. Ismael F. Vaz, L. N. Vicente 
//! "A particle swarm pattern search method for bound constrained global optimization"
class math_PSO
{
public:

  /**
  * Constructor.
  *
  * @param theFunc defines the objective function. It should exist during all lifetime of class instance.
  * @param theLowBorder defines lower border of search space.
  * @param theUppBorder defines upper border of search space.
  * @param theSteps defines steps of regular grid, used for particle generation.
                    This parameter used to define stop condition (TerminalVelocity).
  * @param theNbParticles defines number of particles.
  * @param theNbIter defines maximum number of iterations.
  */
  Standard_EXPORT math_PSO(math_MultipleVarFunction* theFunc,
                           const math_Vector& theLowBorder,
                           const math_Vector& theUppBorder,
                           const math_Vector& theSteps,
                           const Standard_Integer theNbParticles = 32,
                           const Standard_Integer theNbIter = 100);

  //! Perform computations, particles array is constructed inside of this function.
  Standard_EXPORT void Perform(const math_Vector& theSteps,
                               Standard_Real& theValue,
                               math_Vector& theOutPnt,
                               const Standard_Integer theNbIter = 100);

  //! Perform computations with given particles array.
  Standard_EXPORT void Perform(math_PSOParticlesPool& theParticles,
                               Standard_Integer theNbParticles,
                               Standard_Real& theValue,
                               math_Vector& theOutPnt,
                               const Standard_Integer theNbIter = 100);

private:

  void performPSOWithGivenParticles(math_PSOParticlesPool& theParticles,
                                    Standard_Integer theNbParticles,
                                    Standard_Real& theValue,
                                    math_Vector& theOutPnt,
                                    const Standard_Integer theNbIter = 100);

  math_MultipleVarFunction *myFunc;
  math_Vector myLowBorder; // Lower border.
  math_Vector myUppBorder; // Upper border.
  math_Vector mySteps; // steps used in PSO algorithm.
  Standard_Integer myN; // Dimension count.
  Standard_Integer myNbParticles; // Particles number.
  Standard_Integer myNbIter;
};

#endif
