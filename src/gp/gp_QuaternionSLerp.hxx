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

#ifndef _gp_QuaternionSLerp_HeaderFile
#define _gp_QuaternionSLerp_HeaderFile

#include <gp_Quaternion.hxx>

/**
 * Perform Spherical Linear Interpolation of the quaternions,
 * return unit length quaternion.
 */
class gp_QuaternionSLerp
{

public:

  gp_QuaternionSLerp() {}

  gp_QuaternionSLerp (const gp_Quaternion& theQStart, const gp_Quaternion& theQEnd)
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
    Standard_Real cosOmega = myQStart.Dot (myQEnd);
    if (cosOmega < 0.0)
    {
      cosOmega = -cosOmega;
      myQEnd = -myQEnd;
    }
    if (cosOmega > 0.9999)
    {
      cosOmega = 0.9999;
    }
    myOmega = ACos (cosOmega);
    Standard_Real invSinOmega = (1.0 / Sin (myOmega));
    myQStart.Scale (invSinOmega);
    myQEnd.Scale (invSinOmega);
  }

  //! Set interpolated quaternion for theT position (from 0.0 to 1.0)
  void Interpolate (Standard_Real theT, gp_Quaternion& theResultQ) const
  {
    theResultQ = myQStart * Sin((1.0 - theT) * myOmega) + myQEnd * Sin (theT * myOmega);
  }

private:

  gp_Quaternion myQStart;
  gp_Quaternion myQEnd;
  Standard_Real myOmega;

};

#endif //_gp_QuaternionSLerp_HeaderFile
