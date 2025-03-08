// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_Algo.hxx>

#include <TColStd_MapOfInteger.hxx>

//=================================================================================================

BOPAlgo_Algo::BOPAlgo_Algo()
    : BOPAlgo_Options(NCollection_BaseAllocator::CommonBaseAllocator())
{
}

//=================================================================================================

BOPAlgo_Algo::BOPAlgo_Algo(const Handle(NCollection_BaseAllocator)& theAllocator)
    : BOPAlgo_Options(theAllocator)
{
}

//=================================================================================================

BOPAlgo_Algo::~BOPAlgo_Algo() {}

//=================================================================================================

void BOPAlgo_Algo::CheckData()
{
  GetReport()->Clear(Message_Fail);
}

//=================================================================================================

void BOPAlgo_Algo::CheckResult()
{
  GetReport()->Clear(Message_Fail);
}

//=================================================================================================

void BOPAlgo_Algo::analyzeProgress(const Standard_Real theWhole, BOPAlgo_PISteps& theSteps) const
{
  Standard_Real aWhole = theWhole;

  // Fill progress steps for constant operations
  fillPIConstants(theWhole, theSteps);

  TColStd_Array1OfReal& aSteps = theSteps.ChangeSteps();
  TColStd_MapOfInteger  aMIConst;
  for (Standard_Integer i = aSteps.Lower(); i <= aSteps.Upper(); ++i)
  {
    if (aSteps(i) > 0.)
    {
      aMIConst.Add(i);
      aWhole -= aSteps(i);
    }
  }

  // Fill progress steps for other operations
  fillPISteps(theSteps);

  Standard_Real aSum = 0.;
  for (Standard_Integer i = aSteps.Lower(); i <= aSteps.Upper(); ++i)
  {
    if (!aMIConst.Contains(i))
    {
      aSum += aSteps(i);
    }
  }

  // Normalize steps
  if (aSum > 0.)
  {
    for (Standard_Integer i = aSteps.Lower(); i <= aSteps.Upper(); ++i)
    {
      if (!aMIConst.Contains(i))
      {
        aSteps(i) = aWhole * aSteps(i) / aSum;
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_Algo::fillPIConstants(const Standard_Real, BOPAlgo_PISteps&) const {}

//=================================================================================================

void BOPAlgo_Algo::fillPISteps(BOPAlgo_PISteps&) const {}
