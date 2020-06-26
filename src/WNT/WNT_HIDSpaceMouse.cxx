// Copyright (c) 2019-2020 OPEN CASCADE SAS
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

#include <WNT_HIDSpaceMouse.hxx>

namespace
{
  //! Enumeration of known Space Mouse models.
  enum SpacePid
  {
    // VENDOR_ID_LOGITECH
    SpacePid_SpaceMouse             = 0xC603,
    SpacePid_CADMan                 = 0xC605,
    SpacePid_SpaceMouseClassic      = 0xC606,
    SpacePid_SpaceBall5000          = 0xC621,
    SpacePid_SpaceTraveler          = 0xC623,
    SpacePid_SpacePilot             = 0xC625,
    SpacePid_SpaceNavigator         = 0xC626, //!< has only 2 "menu" buttons (second one is treated as SpaceVKey_Fit)
    SpacePid_SpaceExplorer          = 0xC627, //!< 15 buttons
    SpacePid_NavigatorForNotebooks  = 0xC628, //!< has only 2 "menu" buttons (second one is treated as SpaceVKey_Fit)
    SpacePid_SpacePilotPro          = 0xC629, //!< 31 buttons
    SpacePid_SpaceMousePro          = 0xC62B, //!< has only 15 buttons, but codes range from 0 to 26
    // VENDOR_ID_3DCONNEXION
    SpacePid_SpaceMouseWireless1    = 0xC62E, //!< [plugged in] has only  2 buttons
    SpacePid_SpaceMouseWireless2    = 0xC62F, //!< [wireless]   has only  2 buttons
    SpacePid_SpaceMouseProWireless1 = 0xC631, //!< [plugged in] has only 15 buttons
    SpacePid_SpaceMouseProWireless2 = 0xC632, //!< [wireless]   has only 15 buttons
    SpacePid_SpaceMouseEnterprise   = 0xC633, //!< 31 buttons
    SpacePid_SpaceMouseCompact      = 0xC635
  };

  //! Enumeration of known keys available on various Space Mouse models.
  enum SpaceVKey
  {
    SpaceVKey_INVALID = 0,
    SpaceVKey_Menu = 1, SpaceVKey_Fit,
    SpaceVKey_Top, SpaceVKey_Left, SpaceVKey_Right, SpaceVKey_Front, SpaceVKey_Bottom, SpaceVKey_Back,
    SpaceVKey_RollCW, SpaceVKey_RollCCW,
    SpaceVKey_ISO1, SpaceVKey_ISO2,
    SpaceVKey_1, SpaceVKey_2, SpaceVKey_3, SpaceVKey_4, SpaceVKey_5, SpaceVKey_6, SpaceVKey_7, SpaceVKey_8, SpaceVKey_9, SpaceVKey_10,
    SpaceVKey_Esc, SpaceVKey_Alt, SpaceVKey_Shift, SpaceVKey_Ctrl,
    SpaceVKey_Rotate, SpaceVKey_PanZoom, SpaceVKey_Dominant,
    SpaceVKey_Plus, SpaceVKey_Minus,
  };

  //! The raw value range on tested device is [-350; 350].
  enum { THE_RAW_RANGE_350 = 350 };

  //! Convert key state bit into virtual key.
  static SpaceVKey hidToSpaceKey (unsigned long theProductId,
                                  unsigned short theKeyBit)
  {
    static const SpaceVKey THE_PILOT_KEYS[] =
    {
      SpaceVKey_1, SpaceVKey_2, SpaceVKey_3, SpaceVKey_4, SpaceVKey_5, SpaceVKey_6,
      SpaceVKey_Top, SpaceVKey_Left, SpaceVKey_Right, SpaceVKey_Front,
      SpaceVKey_Esc, SpaceVKey_Alt, SpaceVKey_Shift, SpaceVKey_Ctrl,
      SpaceVKey_Fit, SpaceVKey_Menu,
      SpaceVKey_Plus, SpaceVKey_Minus,
      SpaceVKey_Dominant, SpaceVKey_Rotate
    };
    const int THE_NB_PILOT_KEYS = sizeof(THE_PILOT_KEYS) / sizeof(SpaceVKey);

    static const SpaceVKey THE_EXPLORER_KEYS[] =
    {
      SpaceVKey_1, SpaceVKey_2,
      SpaceVKey_Top, SpaceVKey_Left, SpaceVKey_Right, SpaceVKey_Front,
      SpaceVKey_Esc, SpaceVKey_Alt,  SpaceVKey_Shift, SpaceVKey_Ctrl,
      SpaceVKey_Fit, SpaceVKey_Menu,
      SpaceVKey_Plus, SpaceVKey_Minus,
      SpaceVKey_Rotate
    };
    const int THE_NB_EXPLORER_KEYS = sizeof(THE_EXPLORER_KEYS) / sizeof(SpaceVKey);

    // shared by latest 3Dconnexion hardware
    static const SpaceVKey THE_SPACEMOUSEPRO_KEYS[] =
    {
      SpaceVKey_Menu, SpaceVKey_Fit,
      SpaceVKey_Top, SpaceVKey_Left, SpaceVKey_Right, SpaceVKey_Front, SpaceVKey_Bottom, SpaceVKey_Back,
      SpaceVKey_RollCW, SpaceVKey_RollCCW, SpaceVKey_ISO1, SpaceVKey_ISO2,
      SpaceVKey_1, SpaceVKey_2, SpaceVKey_3, SpaceVKey_4,
      SpaceVKey_5, SpaceVKey_6, SpaceVKey_7, SpaceVKey_8, SpaceVKey_9, SpaceVKey_10,
      SpaceVKey_Esc, SpaceVKey_Alt, SpaceVKey_Shift, SpaceVKey_Ctrl,
      SpaceVKey_Rotate,
      SpaceVKey_PanZoom, SpaceVKey_Dominant, SpaceVKey_Plus, SpaceVKey_Minus
    };
    const int THE_NB_SPACEMOUSEPRO_KEYS = sizeof(THE_SPACEMOUSEPRO_KEYS) / sizeof(SpaceVKey);

    switch (theProductId)
    {
      case SpacePid_SpacePilot:
        return theKeyBit < THE_NB_PILOT_KEYS ? THE_PILOT_KEYS[theKeyBit] : SpaceVKey_INVALID;
      case SpacePid_SpaceExplorer:
        return theKeyBit < THE_NB_EXPLORER_KEYS ? THE_EXPLORER_KEYS[theKeyBit] : SpaceVKey_INVALID;
      case SpacePid_SpaceNavigator:
      case SpacePid_NavigatorForNotebooks:
      case SpacePid_SpacePilotPro:
      case SpacePid_SpaceMousePro:
      case SpacePid_SpaceMouseWireless1:
      case SpacePid_SpaceMouseWireless2:
      case SpacePid_SpaceMouseProWireless1:
      case SpacePid_SpaceMouseProWireless2:
      case SpacePid_SpaceMouseEnterprise:
      case SpacePid_SpaceMouseCompact:
        return theKeyBit < THE_NB_SPACEMOUSEPRO_KEYS ? THE_SPACEMOUSEPRO_KEYS[theKeyBit] : SpaceVKey_INVALID;
    }
    return SpaceVKey_INVALID;
  }

}

// =======================================================================
// function : WNT_HIDSpaceMouse
// purpose  :
// =======================================================================
WNT_HIDSpaceMouse::WNT_HIDSpaceMouse (unsigned long theProductId,
                                      const Standard_Byte* theData,
                                      Standard_Size theSize)
: myData (theData),
  mySize (theSize),
  myProductId (theProductId),
  myValueRange (THE_RAW_RANGE_350)
{
  //
}

// =======================================================================
// function : IsKnownProduct
// purpose  :
// =======================================================================
bool WNT_HIDSpaceMouse::IsKnownProduct (unsigned long theProductId)
{
  switch (theProductId)
  {
    case SpacePid_SpacePilot:
    case SpacePid_SpaceExplorer:
    case SpacePid_SpaceNavigator:
    case SpacePid_NavigatorForNotebooks:
    case SpacePid_SpacePilotPro:
    case SpacePid_SpaceMousePro:
    case SpacePid_SpaceMouseWireless1:
    case SpacePid_SpaceMouseWireless2:
    case SpacePid_SpaceMouseProWireless1:
    case SpacePid_SpaceMouseProWireless2:
    case SpacePid_SpaceMouseEnterprise:
    case SpacePid_SpaceMouseCompact:
      return true;
  }
  return false;
}

// =======================================================================
// function : Translation
// purpose  :
// =======================================================================
Graphic3d_Vec3d WNT_HIDSpaceMouse::Translation (bool& theIsIdle,
                                                bool theIsQuadric) const
{
  theIsIdle = true;
  return myData[0] == SpaceRawInput_Translation
      && (mySize == 7 || mySize == 13)
       ? fromRawVec3 (theIsIdle, myData + 1, true, theIsQuadric)
       : Graphic3d_Vec3d();
}

// =======================================================================
// function : Rotation
// purpose  :
// =======================================================================
Graphic3d_Vec3d WNT_HIDSpaceMouse::Rotation (bool& theIsIdle,
                                             bool theIsQuadric) const
{
  theIsIdle = true;
  if (myData[0] == SpaceRawInput_Rotation && mySize == 7)
  {
    return fromRawVec3 (theIsIdle, myData + 1, false, theIsQuadric);
  }
  else if (myData[0] == SpaceRawInput_Translation && mySize == 13)
  {
    return fromRawVec3 (theIsIdle, myData + 7, false, theIsQuadric);
  }
  return Graphic3d_Vec3d();
}

// =======================================================================
// function : fromRawVec3
// purpose  :
// =======================================================================
Graphic3d_Vec3d WNT_HIDSpaceMouse::fromRawVec3 (bool& theIsIdle,
                                                const Standard_Byte* theData,
                                                bool theIsTrans,
                                                bool theIsQuadric) const
{
  theIsIdle = true;
  const NCollection_Vec3<int16_t>& aRaw16 = *reinterpret_cast<const NCollection_Vec3<int16_t>*>(theData);
  Graphic3d_Vec3d aVec (aRaw16.x(), aRaw16.y(), aRaw16.z());
  if (theIsTrans)
  {
    static const int16_t THE_MIN_RAW_TRANS   = 4;
    static const int16_t THE_MIN_RAW_TRANS_Z = 8;
    for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
    {
      if (aRaw16[aCompIter] > -THE_MIN_RAW_TRANS && aRaw16[aCompIter] < THE_MIN_RAW_TRANS)
      {
        aVec[aCompIter] = 0.0;
      }
      else
      {
        theIsIdle = false;
      }
    }
    if (aRaw16.z() > -THE_MIN_RAW_TRANS_Z && aRaw16.z() < THE_MIN_RAW_TRANS_Z)
    {
      aVec.z() = 0.0;
    }
  }
  else
  {
    for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
    {
      if (aRaw16[aCompIter] != 0)
      {
        theIsIdle = false;
        break;
      }
    }
  }

  // determine raw value range
  for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
  {
    if (aRaw16[aCompIter] > myValueRange
    || -aRaw16[aCompIter] > myValueRange)
    {
      myValueRange = 32767; // SHRT_MAX
      break;
    }
  }

  if (!theIsQuadric)
  {
    return aVec / double(myValueRange);
  }

  for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
  {
    aVec[aCompIter] =  aRaw16[aCompIter] > 0
                    ?  aVec[aCompIter] * aVec[aCompIter]
                    : -aVec[aCompIter] * aVec[aCompIter];
  }
  return aVec / (double(myValueRange) * double(myValueRange));
}

// =======================================================================
// function : HidToSpaceKey
// purpose  :
// =======================================================================
Aspect_VKey WNT_HIDSpaceMouse::HidToSpaceKey (unsigned short theKeyBit) const
{
  const SpaceVKey aKey = hidToSpaceKey (myProductId, theKeyBit);
  switch (aKey)
  {
    case SpaceVKey_1:
    case SpaceVKey_2:
    case SpaceVKey_3:
    case SpaceVKey_4:
    case SpaceVKey_5:
    case SpaceVKey_6:
    case SpaceVKey_7:
    case SpaceVKey_8:
    case SpaceVKey_9:
    case SpaceVKey_10:
      return (int(aKey) - int(SpaceVKey_1)) + Aspect_VKey_1;
    case SpaceVKey_Esc:
      return Aspect_VKey_Escape;
    case SpaceVKey_Shift:
      return Aspect_VKey_Shift;
    case SpaceVKey_Alt:
      return Aspect_VKey_Alt;
    case SpaceVKey_Ctrl:
      return Aspect_VKey_Control;
    case SpaceVKey_Top:
      return Aspect_VKey_ViewTop;
    case SpaceVKey_Bottom:
      return Aspect_VKey_ViewBottom;
    case SpaceVKey_Left:
      return Aspect_VKey_ViewLeft;
    case SpaceVKey_Right:
      return Aspect_VKey_ViewRight;
    case SpaceVKey_Front:
      return Aspect_VKey_ViewFront;
    case SpaceVKey_Back:
      return Aspect_VKey_ViewBack;
    case SpaceVKey_ISO1:
      return Aspect_VKey_ViewAxoLeftProj;
    case SpaceVKey_ISO2:
      return Aspect_VKey_ViewAxoRightProj;
    case SpaceVKey_Fit:
      return Aspect_VKey_ViewFitAll;
    case SpaceVKey_RollCW:
      return Aspect_VKey_ViewRoll90CW;
    case SpaceVKey_RollCCW:
      return Aspect_VKey_ViewRoll90CCW;
    case SpaceVKey_Rotate:
      return Aspect_VKey_ViewSwitchRotate;
    default:
      break;
  }
  return Aspect_VKey_UNKNOWN;
}
