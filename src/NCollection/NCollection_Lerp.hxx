// Created by: Kirill GAVRILOV
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

#ifndef _NCollection_Lerp_HeaderFile
#define _NCollection_Lerp_HeaderFile

//! Simple linear interpolation tool (also known as mix() in GLSL).
//! The main purpose of this template class is making interpolation routines more readable.
template<class T>
class NCollection_Lerp
{
public:
  //! Compute interpolated value between two values.
  //! @param theStart first  value
  //! @param theEnd   second value
  //! @param theT normalized interpolation coefficient within [0, 1] range,
  //!             with 0 pointing to theStart and 1 to theEnd.
  static T Interpolate (const T& theStart,
                        const T& theEnd,
                        double theT)
  {
    T aResult;
    NCollection_Lerp aLerp (theStart, theEnd);
    aLerp.Interpolate (theT, aResult);
    return aResult;
  }

public:

  //! Empty constructor
  NCollection_Lerp() : myStart(), myEnd() {}

  //! Main constructor.
  NCollection_Lerp (const T& theStart, const T& theEnd)
  {
    Init (theStart, theEnd);
  }

  //! Initialize values.
  void Init (const T& theStart, const T& theEnd)
  {
    myStart = theStart;
    myEnd   = theEnd;
  }

  //! Compute interpolated value between two values.
  //! @param theT normalized interpolation coefficient within [0, 1] range,
  //!             with 0 pointing to first value and 1 to the second value.
  //! @param theResult [out] interpolated value
  void Interpolate (double theT, T& theResult) const
  {
    theResult = (1.0 - theT) * myStart + theT * myEnd;
  }

private:
  T myStart;
  T myEnd;
};

#endif // _NCollection_Lerp_HeaderFile
