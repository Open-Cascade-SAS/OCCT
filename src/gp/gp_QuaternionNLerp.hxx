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
