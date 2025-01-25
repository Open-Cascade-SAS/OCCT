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

#ifndef _math_PSOParticlesPool_HeaderFile
#define _math_PSOParticlesPool_HeaderFile

#include <NCollection_Array1.hxx>

//! Describes particle pool for using in PSO algorithm.
//! Indexes:
//! 0 <= aDimidx <= myDimensionCount - 1
struct PSO_Particle
{
  Standard_Real* Position; // Data for pointers allocated within PSOParticlesPool instance.
  Standard_Real* Velocity; // Not need to delete it manually.
  Standard_Real* BestPosition;
  Standard_Real  Distance;
  Standard_Real  BestDistance;

  PSO_Particle()
  {
    Distance     = RealLast();
    BestDistance = RealLast();
    Position     = 0;
    Velocity     = 0;
    BestPosition = 0;
  }

  //! Compares the particles according to their distances.
  bool operator<(const PSO_Particle& thePnt) const { return Distance < thePnt.Distance; }
};

// Indexes:
// 1 <= aParticleIdx <= myParticlesCount
class math_PSOParticlesPool
{
public:
  Standard_EXPORT math_PSOParticlesPool(const Standard_Integer theParticlesCount,
                                        const Standard_Integer theDimensionCount);

  Standard_EXPORT PSO_Particle* GetParticle(const Standard_Integer theIdx);

  Standard_EXPORT PSO_Particle* GetBestParticle();

  Standard_EXPORT PSO_Particle* GetWorstParticle();

  Standard_EXPORT ~math_PSOParticlesPool();

private:
  NCollection_Array1<PSO_Particle>  myParticlesPool;
  NCollection_Array1<Standard_Real> myMemory; // Stores particles vector data.
  Standard_Integer                  myParticlesCount;
  Standard_Integer                  myDimensionCount;
};

#endif
