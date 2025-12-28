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
#include <IMeshTools_Parameters.hxx>

//=================================================================================================

void BRepMesh_CylinderRangeSplitter::Reset(const IMeshData::IFaceHandle& theDFace,
                                           const IMeshTools_Parameters&  theParameters)
{
  BRepMesh_DefaultRangeSplitter::Reset(theDFace, theParameters);

  const double aRadius = GetDFace()->GetSurface()->Cylinder().Radius();
  myDu                 = GCPnts_TangentialDeflection::ArcAngularStep(aRadius,
                                                     GetDFace()->GetDeflection(),
                                                     theParameters.Angle,
                                                     theParameters.MinSize);
}

//=================================================================================================

Handle(IMeshData::ListOfPnt2d) BRepMesh_CylinderRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& /*theParameters*/) const
{
  const std::pair<double, double>& aRangeU = GetRangeU();
  const std::pair<double, double>& aRangeV = GetRangeV();

  const double aRadius = GetDFace()->GetSurface()->Cylinder().Radius();

  int          nbU     = 0;
  int          nbV     = 0;
  const double su      = aRangeU.second - aRangeU.first;
  const double sv      = aRangeV.second - aRangeV.first;
  const double aArcLen = su * aRadius;
  if (aArcLen > GetDFace()->GetDeflection())
  {
    // Calculate parameters for iteration in U direction
    nbU = (int)(su / myDu);

    /*
    // Calculate parameters for iteration in V direction
    const double aDv = nbU*sv / aArcLen;
    // Protection against overflow during casting to int in case
    // of long cylinder with small radius.
    nbV = aDv > static_cast<double> (IntegerLast()) ?
      0 : (int) (aDv);
    nbV = std::min(nbV, 100 * nbU);
    */
  }

  const double Du = su / (nbU + 1);
  const double Dv = sv / (nbV + 1);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);
  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);

  const double aPasMaxV = aRangeV.second - Dv * 0.5;
  const double aPasMaxU = aRangeU.second - Du * 0.5;
  for (double aPasV = aRangeV.first + Dv; aPasV < aPasMaxV; aPasV += Dv)
  {
    for (double aPasU = aRangeU.first + Du; aPasU < aPasMaxU; aPasU += Du)
    {
      aNodes->Append(gp_Pnt2d(aPasU, aPasV));
    }
  }

  return aNodes;
}

//=================================================================================================

void BRepMesh_CylinderRangeSplitter::computeDelta(const double /*theLengthU*/,
                                                  const double theLengthV)
{
  const std::pair<double, double>& aRangeV = GetRangeV();
  myDelta.first  = myDu / std::max(theLengthV, aRangeV.second - aRangeV.first);
  myDelta.second = 1.;
}
