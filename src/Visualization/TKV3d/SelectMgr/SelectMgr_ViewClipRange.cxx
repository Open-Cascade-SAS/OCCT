// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <SelectMgr_ViewClipRange.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>

//=================================================================================================

void SelectMgr_ViewClipRange::AddClippingPlanes(const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                const gp_Ax1&                         thePickRay)
{
  const gp_Dir& aViewRayDir = thePickRay.Direction();
  const gp_Pnt& aNearPnt    = thePickRay.Location();

  NCollection_Vec4<double> aPlaneABCD;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(thePlanes); aPlaneIt.More();
       aPlaneIt.Next())
  {
    const occ::handle<Graphic3d_ClipPlane>& aClipPlane = aPlaneIt.Value();
    if (!aClipPlane->IsOn())
    {
      continue;
    }

    Bnd_Range aSubRange(RealFirst(), RealLast());
    for (const Graphic3d_ClipPlane* aSubPlaneIter = aClipPlane.get(); aSubPlaneIter != NULL;
         aSubPlaneIter                            = aSubPlaneIter->ChainNextPlane().get())
    {
      const gp_Pln aGeomPlane = aSubPlaneIter->ToPlane();
      aGeomPlane.Coefficients(aPlaneABCD[0], aPlaneABCD[1], aPlaneABCD[2], aPlaneABCD[3]);

      const gp_XYZ& aPlaneDirXYZ = aGeomPlane.Axis().Direction().XYZ();
      double aDotProduct  = aViewRayDir.XYZ().Dot(aPlaneDirXYZ);
      double aDistance    = -aNearPnt.XYZ().Dot(aPlaneDirXYZ) - aPlaneABCD[3];
      double aDistToPln   = 0.0;

      // check whether the pick line is parallel to clip plane
      if (std::abs(aDotProduct) < Precision::Angular())
      {
        if (aDistance < 0.0)
        {
          continue;
        }
        aDistToPln  = RealLast();
        aDotProduct = 1.0;
      }
      else
      {
        // compute distance to point of pick line intersection with the plane
        const double aParam = aDistance / aDotProduct;

        const gp_Pnt anIntersectionPnt = aNearPnt.XYZ() + aViewRayDir.XYZ() * aParam;
        aDistToPln                     = anIntersectionPnt.Distance(aNearPnt);
        if (aParam < 0.0)
        {
          // the plane is "behind" the ray
          aDistToPln = -aDistToPln;
        }
      }

      // change depth limits for case of opposite and directed planes
      if (!aClipPlane->IsChain())
      {
        if (aDotProduct < 0.0)
        {
          ChangeUnclipRange().TrimTo(aDistToPln);
        }
        else
        {
          ChangeUnclipRange().TrimFrom(aDistToPln);
        }
      }
      else
      {
        if (aDotProduct < 0.0)
        {
          aSubRange.TrimFrom(aDistToPln);
        }
        else
        {
          aSubRange.TrimTo(aDistToPln);
        }
      }
    }

    if (!aSubRange.IsVoid() && aClipPlane->IsChain())
    {
      AddClipSubRange(aSubRange);
    }
  }
}

//=================================================================================================

void SelectMgr_ViewClipRange::DumpJson(Standard_OStream& theOStream,
                                       int  theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_ViewClipRange)

  for (size_t aRangeIter = 0; aRangeIter < myClipRanges.size(); ++aRangeIter)
  {
    Bnd_Range aClipRange = myClipRanges[aRangeIter];
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aClipRange)
  }
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myUnclipRange)
}
