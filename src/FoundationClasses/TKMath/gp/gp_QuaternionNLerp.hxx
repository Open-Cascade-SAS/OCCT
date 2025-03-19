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

#ifndef _gp_QuaternionNLerp_HeaderFile
#define _gp_QuaternionNLerp_HeaderFile

#include <gp_Quaternion.hxx>

//! Class perform linear interpolation (approximate rotation interpolation),
//! result quaternion nonunit, its length lay between. sqrt(2)/2  and 1.0
class gp_QuaternionNLerp
{
public:
  //! Compute interpolated quaternion between two quaternions.
  //! @param theStart first  quaternion
  //! @param theEnd   second quaternion
  //! @param theT normalized interpolation coefficient within 0..1 range,
  //!             with 0 pointing to theStart and 1 to theEnd.
  static gp_Quaternion Interpolate(const gp_Quaternion& theQStart,
                                   const gp_Quaternion& theQEnd,
                                   Standard_Real        theT)
  {
    gp_Quaternion      aResult;
    gp_QuaternionNLerp aLerp(theQStart, theQEnd);
    aLerp.Interpolate(theT, aResult);
    return aResult;
  }

public:
  //! Empty constructor,
  gp_QuaternionNLerp() {}

  //! Constructor with initialization.
  gp_QuaternionNLerp(const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    Init(theQStart, theQEnd);
  }

  //! Initialize the tool with Start and End values.
  void Init(const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    InitFromUnit(theQStart.Normalized(), theQEnd.Normalized());
  }

  //! Initialize the tool with Start and End unit quaternions.
  void InitFromUnit(const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    myQStart              = theQStart;
    myQEnd                = theQEnd;
    Standard_Real anInner = myQStart.Dot(myQEnd);
    if (anInner < 0.0)
    {
      myQEnd = -myQEnd;
    }
    myQEnd -= myQStart;
  }

  //! Set interpolated quaternion for theT position (from 0.0 to 1.0)
  void Interpolate(Standard_Real theT, gp_Quaternion& theResultQ) const
  {
    theResultQ = myQStart + myQEnd * theT;
  }

private:
  gp_Quaternion myQStart;
  gp_Quaternion myQEnd;
};

#endif //_gp_QuaternionNLerp_HeaderFile
