// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _gp_TrsfNLerp_HeaderFile
#define _gp_TrsfNLerp_HeaderFile

#include <gp_Trsf.hxx>
#include <gp_QuaternionNLerp.hxx>
#include <NCollection_Lerp.hxx>
#include <Precision.hxx>

//! Linear interpolation tool for transformation defined by gp_Trsf.
//!
//! In general case, there is a no well-defined interpolation between arbitrary transformations,
//! because desired transient values might vary depending on application needs.
//!
//! This tool performs independent interpolation of three logical
//! transformation parts - rotation (using gp_QuaternionNLerp), translation and scale factor.
//! Result of such interpolation might be not what application expects,
//! thus this tool might be considered for simple cases or for interpolating between small intervals.
template<> class NCollection_Lerp<gp_Trsf>
{
public:

  //! Empty constructor
  NCollection_Lerp() {}

  //! Main constructor.
  NCollection_Lerp (const gp_Trsf& theStart, const gp_Trsf& theEnd)
  {
    Init (theStart, theEnd);
  }

  //! Initialize values.
  void Init (const gp_Trsf& theStart, const gp_Trsf& theEnd)
  {
    myTrsfStart = theStart;
    myTrsfEnd   = theEnd;
    myLocLerp  .Init (theStart.TranslationPart(), theEnd.TranslationPart());
    myRotLerp  .Init (theStart.GetRotation(),     theEnd.GetRotation());
    myScaleLerp.Init (theStart.ScaleFactor(),     theEnd.ScaleFactor());
  }

  //! Compute interpolated value between two values.
  //! @param theT normalized interpolation coefficient within [0, 1] range,
  //!             with 0 pointing to first value and 1 to the second value.
  //! @param theResult [out] interpolated value
  void Interpolate (double theT, gp_Trsf& theResult) const
  {
    if (Abs (theT - 0.0) < Precision::Confusion())
    {
      theResult = myTrsfStart;
      return;
    }
    else if (Abs (theT - 1.0) < Precision::Confusion())
    {
      theResult = myTrsfEnd;
      return;
    }

    gp_XYZ aLoc;
    gp_Quaternion aRot;
    Standard_Real aScale = 1.0;
    myLocLerp  .Interpolate (theT, aLoc);
    myRotLerp  .Interpolate (theT, aRot);
    myScaleLerp.Interpolate (theT, aScale);
    theResult = gp_Trsf();
    theResult.SetRotation (aRot);
    theResult.SetTranslationPart (aLoc);
    theResult.SetScaleFactor (aScale);
  }

private:
  NCollection_Lerp<gp_XYZ>        myLocLerp;
  NCollection_Lerp<Standard_Real> myScaleLerp;
  gp_QuaternionNLerp              myRotLerp;
  gp_Trsf                         myTrsfStart;
  gp_Trsf                         myTrsfEnd;
};

typedef NCollection_Lerp<gp_Trsf> gp_TrsfNLerp;

#endif // _gp_TrsfNLerp_HeaderFile
