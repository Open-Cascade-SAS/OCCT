// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_ConeRangeSplitter.hxx>

#include <GCPnts_TangentialDeflection.hxx>
#include <IMeshTools_Parameters.hxx>

//=================================================================================================

std::pair<Standard_Real, Standard_Real> BRepMesh_ConeRangeSplitter::GetSplitSteps(
  const IMeshTools_Parameters&                   theParameters,
  std::pair<Standard_Integer, Standard_Integer>& theStepsNb) const
{
  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

  gp_Cone       aCone = GetDFace()->GetSurface()->Cone();
  Standard_Real aRefR = aCone.RefRadius();
  Standard_Real aSAng = aCone.SemiAngle();
  Standard_Real aRadius =
    Max(Abs(aRefR + aRangeV.first * Sin(aSAng)), Abs(aRefR + aRangeV.second * Sin(aSAng)));

  Standard_Real Dv, Du = GCPnts_TangentialDeflection::ArcAngularStep(aRadius,
                                                                     GetDFace()->GetDeflection(),
                                                                     theParameters.Angle,
                                                                     theParameters.MinSize);

  const Standard_Real    aDiffU = aRangeU.second - aRangeU.first;
  const Standard_Real    aDiffV = aRangeV.second - aRangeV.first;
  const Standard_Real    aScale = (Du * aRadius);
  const Standard_Real    aRatio = Max(1., Log(aDiffV / aScale));
  const Standard_Integer nbU    = (Standard_Integer)(aDiffU / Du);
  const Standard_Integer nbV    = (Standard_Integer)(aDiffV / aScale / aRatio);

  Du = aDiffU / (nbU + 1);
  Dv = aDiffV / (nbV + static_cast<Standard_Integer>(aRatio));

  theStepsNb.first  = nbU;
  theStepsNb.second = nbV;
  return std::make_pair(Du, Dv);
}

//=================================================================================================

Handle(IMeshData::ListOfPnt2d) BRepMesh_ConeRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& theParameters) const
{
  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

  std::pair<Standard_Integer, Standard_Integer> aStepsNb;
  std::pair<Standard_Real, Standard_Real>       aSteps = GetSplitSteps(theParameters, aStepsNb);

  const Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);
  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);

  const Standard_Real aPasMaxV = aRangeV.second - aSteps.second * 0.5;
  const Standard_Real aPasMaxU = aRangeU.second - aSteps.first * 0.5;
  for (Standard_Real aPasV = aRangeV.first + aSteps.second; aPasV < aPasMaxV;
       aPasV += aSteps.second)
  {
    for (Standard_Real aPasU = aRangeU.first + aSteps.first; aPasU < aPasMaxU;
         aPasU += aSteps.first)
    {
      aNodes->Append(gp_Pnt2d(aPasU, aPasV));
    }
  }

  return aNodes;
}
