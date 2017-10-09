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

#include <BRepMesh_CylinderRangeSplitter.hxx>
#include <GCPnts_TangentialDeflection.hxx>

//=======================================================================
// Function: Reset
// Purpose : 
//=======================================================================
void BRepMesh_CylinderRangeSplitter::Reset(const IMeshData::IFaceHandle& theDFace,
                                           const IMeshTools_Parameters&  theParameters)
{
  BRepMesh_DefaultRangeSplitter::Reset(theDFace, theParameters);

  const Standard_Real aRadius = GetDFace()->GetSurface()->Cylinder().Radius();
  myDu = GCPnts_TangentialDeflection::ArcAngularStep(
    aRadius, GetDFace()->GetDeflection(),
    theParameters.Angle, theParameters.MinSize);
}

//=======================================================================
// Function: GenerateSurfaceNodes
// Purpose : 
//=======================================================================
Handle(IMeshData::ListOfPnt2d) BRepMesh_CylinderRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& /*theParameters*/) const
{
  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

  const Standard_Real aRadius = GetDFace()->GetSurface()->Cylinder().Radius();

  Standard_Integer nbU = 0;
  Standard_Integer nbV = 0;
  const Standard_Real su = aRangeU.second - aRangeU.first;
  const Standard_Real sv = aRangeV.second - aRangeV.first;
  const Standard_Real aArcLen = su * aRadius;
  if (aArcLen > GetDFace()->GetDeflection())
  {
    // Calculate parameters for iteration in U direction
    nbU = (Standard_Integer) (su / myDu);

    /*
    // Calculate parameters for iteration in V direction
    const Standard_Real aDv = nbU*sv / aArcLen;
    // Protection against overflow during casting to int in case 
    // of long cylinder with small radius.
    nbV = aDv > static_cast<Standard_Real> (IntegerLast()) ?
      0 : (Standard_Integer) (aDv);
    nbV = Min(nbV, 100 * nbU);
    */
  }

  const Standard_Real Du = su / (nbU + 1);
  const Standard_Real Dv = sv / (nbV + 1);

  const Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);
  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);

  const Standard_Real aPasMaxV = aRangeV.second - Dv*0.5;
  const Standard_Real aPasMaxU = aRangeU.second - Du*0.5;
  for (Standard_Real aPasV = aRangeV.first + Dv; aPasV < aPasMaxV; aPasV += Dv)
  {
    for (Standard_Real aPasU = aRangeU.first + Du; aPasU < aPasMaxU; aPasU += Du)
    {
      aNodes->Append(gp_Pnt2d(aPasU, aPasV));
    }
  }

  return aNodes;
}

//=======================================================================
// Function: computeDelta
// Purpose : 
//=======================================================================
void BRepMesh_CylinderRangeSplitter::computeDelta(
  const Standard_Real /*theLengthU*/,
  const Standard_Real theLengthV)
{
  const std::pair<double, double>& aRangeV = GetRangeV();
  myDelta.first  = myDu / Max(theLengthV, aRangeV.second - aRangeV.first);
  myDelta.second = 1.;
}
