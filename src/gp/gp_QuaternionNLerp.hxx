// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _gp_QuaternionNLerp_HeaderFile
#define _gp_QuaternionNLerp_HeaderFile

#include <gp_Quaternion.hxx>

/**
 * Class perform linear interpolation (approximate rotation interpolation),
 * result quaternion nonunit, its length lay between. sqrt(2)/2  and 1.0
 */
class gp_QuaternionNLerp
{

public:

  gp_QuaternionNLerp() {}

  gp_QuaternionNLerp (const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    Init (theQStart, theQEnd);
  }

  void Init (const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    InitFromUnit (theQStart.Normalized(), theQEnd.Normalized());
  }

  void InitFromUnit (const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
  {
    myQStart = theQStart;
    myQEnd   = theQEnd;
    Standard_Real anInner = myQStart.Dot (myQEnd);
    if (anInner < 0.0)
    {
      myQEnd = -myQEnd;
    }
    myQEnd -= myQStart;
  }

  //! Set interpolated quaternion for theT position (from 0.0 to 1.0)
  void Interpolate (Standard_Real theT, gp_Quaternion& theResultQ) const
  {
    theResultQ = myQStart + myQEnd * theT;
  }

  static gp_Quaternion Interpolate (const gp_Quaternion& theQStart,
                                    const gp_Quaternion& theQEnd,
                                    Standard_Real theT)
  {
    gp_Quaternion aResultQ;
    gp_QuaternionNLerp aNLerp (theQStart, theQEnd);
    aNLerp.Interpolate (theT, aResultQ);
    return aResultQ;
  }

private:

  gp_Quaternion myQStart;
  gp_Quaternion myQEnd;

};

#endif //_gp_QuaternionNLerp_HeaderFile
