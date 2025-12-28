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
  const std::pair<double, double>& aRangeU = GetRangeU();
  const std::pair<double, double>& aRangeV = GetRangeV();

  const double aDiffU = aRangeU.second - aRangeU.first;
  const double aDiffV = aRangeV.second - aRangeV.first;

  const gp_Torus aTorus = GetDFace()->GetSurface()->Torus();
  const double   r      = aTorus.MinorRadius();
  const double   R      = aTorus.MajorRadius();

  const double oldDv = GCPnts_TangentialDeflection::ArcAngularStep(r,
                                                                   GetDFace()->GetDeflection(),
                                                                   theParameters.Angle,
                                                                   theParameters.MinSize);

  double Dv = 0.9 * oldDv; // TWOTHIRD * oldDv;
  Dv        = oldDv;

  const int nbV = std::max((int)(aDiffV / Dv), 2);
  Dv            = aDiffV / (nbV + 1);

  double       Du;
  const double ru = R + r;
  if (ru > 1.e-16)
  {
    Du = GCPnts_TangentialDeflection::ArcAngularStep(ru,
                                                     GetDFace()->GetDeflection(),
                                                     theParameters.Angle,
                                                     theParameters.MinSize);

    const double aa = sqrt(Du * Du + oldDv * oldDv);
    if (aa < gp::Resolution())
    {
      return Handle(IMeshData::ListOfPnt2d)();
    }

    Du *= std::min(oldDv, Du) / aa;
  }
  else
  {
    Du = Dv;
  }

  int nbU = std::max((int)(aDiffU / Du), 2);
  nbU     = std::max(nbU, (int)(nbV * aDiffU * R / (aDiffV * r) / 5.));
  Du      = aDiffU / (nbU + 1);

  const occ::handle<NCollection_IncAllocator> aTmpAlloc =
    new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE);

  Handle(IMeshData::SequenceOfReal) aParamU, aParamV;
  if (R < r)
  {
    // As the points of edges are returned.
    // in this case, the points are not representative.

    //-- Choose DeltaX and DeltaY so that to avoid skipping points on the grid
    aParamU = new IMeshData::SequenceOfReal(aTmpAlloc);
    for (int i = 0; i <= nbU; i++)
    {
      aParamU->Append(aRangeU.first + i * Du);
    }
  } // R<r
  else // U if R > r
  {
    aParamU = fillParams(GetParametersU(), GetRangeU(), nbU, 0.5, aTmpAlloc);
  }

  aParamV = fillParams(GetParametersV(), GetRangeV(), nbV, 2. / 3., aTmpAlloc);

  const std::pair<double, double> aNewRangeU(aRangeU.first + Du * 0.1, aRangeU.second - Du * 0.1);

  const std::pair<double, double> aNewRangeV(aRangeV.first + Dv * 0.1, aRangeV.second - Dv * 0.1);

  Handle(IMeshData::ListOfPnt2d) aNodes = new IMeshData::ListOfPnt2d(aTmpAlloc);
  for (int i = 1; i <= aParamU->Length(); ++i)
  {
    const double aPasU = aParamU->Value(i);
    if (aPasU >= aNewRangeU.first && aPasU < aNewRangeU.second)
    {
      for (int j = 1; j <= aParamV->Length(); ++j)
      {
        const double aPasV = aParamV->Value(j);
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
  const IMeshData::IMapOfReal&                 theParams,
  const std::pair<double, double>&             theRange,
  const int                                    theStepsNb,
  const double                                 theScale,
  const occ::handle<NCollection_IncAllocator>& theAllocator) const
{
  Handle(IMeshData::SequenceOfReal) aParams = new IMeshData::SequenceOfReal(theAllocator);

  const int                  aLength = theParams.Size();
  NCollection_Array1<double> aParamArray(1, aLength);

  for (int j = 1; j <= aLength; ++j)
  {
    aParamArray(j) = theParams(j);
  }

  // Calculate DU, leave array of parameters
  const double aDiff = std::abs(theRange.second - theRange.first);
  double       aStep = FUN_CalcAverageDUV(aParamArray, aLength);
  aStep              = std::max(aStep, aDiff / (double)theStepsNb / 2.);

  double aStdStep = aDiff / (double)aLength;
  if (aStep > aStdStep)
  {
    aStdStep = aStep;
  }
  aStdStep *= theScale;

  // Add parameters
  for (int j = 1; j <= aLength; ++j)
  {
    const double pp = aParamArray(j);

    bool      isToInsert    = true;
    const int aParamsLength = aParams->Length();
    for (int i = 1; i <= aParamsLength && isToInsert; ++i)
    {
      isToInsert = (std::abs(aParams->Value(i) - pp) > aStdStep);
    }

    if (isToInsert)
    {
      aParams->Append(pp);
    }
  }

  return aParams;
}

//=================================================================================================

double BRepMesh_TorusRangeSplitter::FUN_CalcAverageDUV(NCollection_Array1<double>& P,
                                                       const int                   PLen) const
{
  int    i, j, n = 0;
  double p, result = 0.;

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
      p = std::abs(P(i) - P(i - 1));
      if (p > 1.e-7)
      {
        result += p;
        n++;
      }
    }
  }
  return (n ? (result / (double)n) : -1.);
}
