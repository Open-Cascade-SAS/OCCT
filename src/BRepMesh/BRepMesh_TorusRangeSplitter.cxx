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

#include <BRepMesh_TorusRangeSplitter.hxx>
#include <GCPnts_TangentialDeflection.hxx>

//=================================================================================================

Handle(IMeshData::ListOfPnt2d) BRepMesh_TorusRangeSplitter::GenerateSurfaceNodes(
  const IMeshTools_Parameters& theParameters) const
{
  const std::pair<Standard_Real, Standard_Real>& aRangeU = GetRangeU();
  const std::pair<Standard_Real, Standard_Real>& aRangeV = GetRangeV();

  const Standard_Real aDiffU = aRangeU.second - aRangeU.first;
  const Standard_Real aDiffV = aRangeV.second - aRangeV.first;

  const gp_Torus      aTorus = GetDFace()->GetSurface()->Torus();
  const Standard_Real r      = aTorus.MinorRadius();
  const Standard_Real R      = aTorus.MajorRadius();

  const Standard_Real oldDv =
    GCPnts_TangentialDeflection::ArcAngularStep(r,
                                                GetDFace()->GetDeflection(),
                                                theParameters.Angle,
                                                theParameters.MinSize);

  Standard_Real Dv = 0.9 * oldDv; // TWOTHIRD * oldDv;
  Dv               = oldDv;

  const Standard_Integer nbV = Max((Standard_Integer)(aDiffV / Dv), 2);
  Dv                         = aDiffV / (nbV + 1);

  Standard_Real       Du;
  const Standard_Real ru = R + r;
  if (ru > 1.e-16)
  {
    Du = GCPnts_TangentialDeflection::ArcAngularStep(ru,
                                                     GetDFace()->GetDeflection(),
                                                     theParameters.Angle,
                                                     theParameters.MinSize);

    const Standard_Real aa = sqrt(Du * Du + oldDv * oldDv);
    if (aa < gp::Resolution())
    {
      return Handle(IMeshData::ListOfPnt2d)();
    }

    Du *= Min(oldDv, Du) / aa;
  }
  else
  {
    Du = Dv;
  }

  Standard_Integer nbU = Max((Standard_Integer)(aDiffU / Du), 2);
  nbU                  = Max(nbU, (Standard_Integer)(nbV * aDiffU * R / (aDiffV * r) / 5.));
  Du                   = aDiffU / (nbU + 1);

  const Handle(NCollection_IncAllocator) aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  Handle(IMeshData::SequenceOfReal) aParamU, aParamV;
  if (R < r)
  {
    // As the points of edges are returned.
    // in this case, the points are not representative.

    //-- Choose DeltaX and DeltaY so that to avoid skipping points on the grid
    aParamU = new IMeshData::SequenceOfReal(aTmpAlloc);
    for (Standard_Integer i = 0; i <= nbU; i++)
    {
      aParamU->Append(aRangeU.first + i * Du);
    }
  } // R<r
  else // U if R > r
  {
    aParamU = fillParams(GetParametersU(), GetRangeU(), nbU, 0.5, aTmpAlloc);
  }

  aParamV = fillParams(GetParametersV(), GetRangeV(), nbV, 2. / 3., aTmpAlloc);

  const std::pair<Standard_Real, Standard_Real> aNewRangeU(aRangeU.first + Du * 0.1,
                                                           aRangeU.second - Du * 0.1);

  const std::pair<Standard_Real, Standard_Real> aNewRangeV(aRangeV.first + Dv * 0.1,
                                                           aRangeV.second - Dv * 0.1);

  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);
  for (Standard_Integer i = 1; i <= aParamU->Length(); ++i)
  {
    const Standard_Real aPasU = aParamU->Value(i);
    if (aPasU >= aNewRangeU.first && aPasU < aNewRangeU.second)
    {
      for (Standard_Integer j = 1; j <= aParamV->Length(); ++j)
      {
        const Standard_Real aPasV = aParamV->Value(j);
        if (aPasV >= aNewRangeV.first && aPasV < aNewRangeV.second)
        {
          aNodes->Append(gp_Pnt2d(aPasU, aPasV));
        }
      }
    }
  }

  return aNodes;
}

//=================================================================================================

void BRepMesh_TorusRangeSplitter::AddPoint(const gp_Pnt2d& thePoint)
{
  BRepMesh_DefaultRangeSplitter::AddPoint(thePoint);
  GetParametersU().Add(thePoint.X());
  GetParametersV().Add(thePoint.Y());
}

//=================================================================================================

Handle(IMeshData::SequenceOfReal) BRepMesh_TorusRangeSplitter::fillParams(
  const IMeshData::IMapOfReal&                   theParams,
  const std::pair<Standard_Real, Standard_Real>& theRange,
  const Standard_Integer                         theStepsNb,
  const Standard_Real                            theScale,
  const Handle(NCollection_IncAllocator)&        theAllocator) const
{
  Handle(IMeshData::SequenceOfReal) aParams = new IMeshData::SequenceOfReal(theAllocator);

  const Standard_Integer aLength = theParams.Size();
  TColStd_Array1OfReal   aParamArray(1, aLength);

  for (Standard_Integer j = 1; j <= aLength; ++j)
  {
    aParamArray(j) = theParams(j);
  }

  // Calculate DU, leave array of parameters
  const Standard_Real aDiff = Abs(theRange.second - theRange.first);
  Standard_Real       aStep = FUN_CalcAverageDUV(aParamArray, aLength);
  aStep                     = Max(aStep, aDiff / (Standard_Real)theStepsNb / 2.);

  Standard_Real aStdStep = aDiff / (Standard_Real)aLength;
  if (aStep > aStdStep)
  {
    aStdStep = aStep;
  }
  aStdStep *= theScale;

  // Add parameters
  for (Standard_Integer j = 1; j <= aLength; ++j)
  {
    const Standard_Real pp = aParamArray(j);

    Standard_Boolean       isToInsert    = Standard_True;
    const Standard_Integer aParamsLength = aParams->Length();
    for (Standard_Integer i = 1; i <= aParamsLength && isToInsert; ++i)
    {
      isToInsert = (Abs(aParams->Value(i) - pp) > aStdStep);
    }

    if (isToInsert)
    {
      aParams->Append(pp);
    }
  }

  return aParams;
}

//=================================================================================================

Standard_Real BRepMesh_TorusRangeSplitter::FUN_CalcAverageDUV(TColStd_Array1OfReal&  P,
                                                              const Standard_Integer PLen) const
{
  Standard_Integer i, j, n = 0;
  Standard_Real    p, result = 0.;

  for (i = 1; i <= PLen; i++)
  {
    // Sort
    for (j = i + 1; j <= PLen; j++)
    {
      if (P(i) > P(j))
      {
        p    = P(i);
        P(i) = P(j);
        P(j) = p;
      }
    }
    // Accumulate
    if (i != 1)
    {
      p = Abs(P(i) - P(i - 1));
      if (p > 1.e-7)
      {
        result += p;
        n++;
      }
    }
  }
  return (n ? (result / (Standard_Real)n) : -1.);
}
