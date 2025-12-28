// Created on: 1999-05-06
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_SplitSurfaceAngle.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceAngle, ShapeUpgrade_SplitSurface)

//=================================================================================================

ShapeUpgrade_SplitSurfaceAngle::ShapeUpgrade_SplitSurfaceAngle(const double MaxAngle)
{
  myMaxAngle = MaxAngle;
}

//=================================================================================================

void ShapeUpgrade_SplitSurfaceAngle::SetMaxAngle(const double MaxAngle)
{
  myMaxAngle = MaxAngle;
}

//=================================================================================================

double ShapeUpgrade_SplitSurfaceAngle::MaxAngle() const
{
  return myMaxAngle;
}

//=================================================================================================

void ShapeUpgrade_SplitSurfaceAngle::Compute(const bool /*Segment*/)
{
  occ::handle<Geom_Surface> S;
  double                    U1 = 0., U2 = 0.;
  bool                      isRect = false;
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> rts =
      occ::down_cast<Geom_RectangularTrimmedSurface>(mySurface);
    isRect = true;
    double V1, V2;
    rts->Bounds(U1, U2, V1, V2);
    S = rts->BasisSurface();
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_OffsetSurface> ofs = occ::down_cast<Geom_OffsetSurface>(mySurface);
    S                                   = ofs->BasisSurface();
  }
  else
    S = mySurface;

  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))
      || S->IsKind(STANDARD_TYPE(Geom_ConicalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
      || S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {

    double UFirst     = myUSplitValues->Sequence().First();
    double ULast      = myUSplitValues->Sequence().Last();
    double maxAngle   = myMaxAngle; // maximal u length of segment
    double uLength    = ULast - UFirst;
    int    nbSegments = int((uLength - Precision::Angular()) / maxAngle) + 1;
    if (nbSegments == 1)
      if (!isRect || !(uLength < maxAngle) || !((U2 - U1) < maxAngle))
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    double                                     segAngle    = uLength / nbSegments;
    double                                     currAngle   = segAngle + UFirst;
    occ::handle<NCollection_HSequence<double>> splitValues = new NCollection_HSequence<double>;
    for (int i = 1; i < nbSegments; i++, currAngle += segAngle)
      splitValues->Append(currAngle);
    SetUSplitValues(splitValues);
  }
}
