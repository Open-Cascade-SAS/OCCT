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

#include <math_PSOParticlesPool.hxx>
#include <algorithm>


//=======================================================================
//function : math_PSOParticlesPool
//purpose  : Constructor
//=======================================================================
math_PSOParticlesPool::math_PSOParticlesPool(const Standard_Integer theParticlesCount,
                                             const Standard_Integer theDimensionCount)
: myParticlesPool(1, theParticlesCount),
  myMemory(0, theParticlesCount * (theDimensionCount  // Position
                                 + theDimensionCount  // Velocity
                                 + theDimensionCount) // BestPosition
                                 - 1) 
{
  myParticlesCount = theParticlesCount;
  myDimensionCount = theDimensionCount;

  // Pointers adjusting.
  Standard_Integer aParIdx, aShiftIdx;
  for(aParIdx = 1; aParIdx <= myParticlesCount; ++aParIdx)
  {
    aShiftIdx = (theDimensionCount * 3) * (aParIdx - 1);
    myParticlesPool(aParIdx).Position     = &myMemory(aShiftIdx);
    myParticlesPool(aParIdx).Velocity     = &myMemory(aShiftIdx + theDimensionCount);
    myParticlesPool(aParIdx).BestPosition = &myMemory(aShiftIdx + 2 * theDimensionCount);
  }
}

//=======================================================================
//function : ~math_PSOParticlesPool
//purpose  : Destructor
//=======================================================================
math_PSOParticlesPool::~math_PSOParticlesPool()
{
}

//=======================================================================
//function : GetParticle
//purpose  : 
//=======================================================================
PSO_Particle* math_PSOParticlesPool::GetParticle(const Standard_Integer theIdx)
{
  return &myParticlesPool(theIdx);
}

//=======================================================================
//function : GetBestParticle
//purpose  : 
//=======================================================================
PSO_Particle* math_PSOParticlesPool::GetBestParticle()
{
  return &*std::min_element(myParticlesPool.begin(), myParticlesPool.end());
}

//=======================================================================
//function : GetWorstParticle
//purpose  : 
//=======================================================================
PSO_Particle* math_PSOParticlesPool::GetWorstParticle()
{
  return &*std::max_element(myParticlesPool.begin(), myParticlesPool.end());
}