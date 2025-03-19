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

#ifndef _WNT_HIDSpaceMouse_Header
#define _WNT_HIDSpaceMouse_Header

#include <Aspect_VKey.hxx>
#include <Graphic3d_Vec.hxx>

//! Wrapper over Space Mouse data chunk within WM_INPUT event (known also as Raw Input in WinAPI).
//! This class predefines specific list of supported devices, which does not depend on 3rdparty
//! library provided by mouse vendor. Supported input chunks:
//! - Rotation (3 directions);
//! - Translation (3 directions);
//! - Pressed buttons.
//!
//! To use the class, register Raw Input device:
//! @code
//!  Handle(WNT_Window) theWindow;
//!  RAWINPUTDEVICE aRawInDevList[1];
//!  RAWINPUTDEVICE& aRawSpace = aRawInDevList[0];
//!  aRawSpace.usUsagePage = HID_USAGE_PAGE_GENERIC;
//!  aRawSpace.usUsage     = HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER;
//!  aRawSpace.dwFlags     = 0; // RIDEV_DEVNOTIFY
//!  aRawSpace.hwndTarget  = (HWND )theWindow->NativeHandle();
//!  if (!::RegisterRawInputDevices (aRawInDevList, 1, sizeof(aRawInDevList[0]))) { Error; }
//! @endcode
//!
//! Then handle WM_INPUT events within window message loop.
//! @code
//!  AIS_ViewController theViewCtrl;
//!  case WM_INPUT:
//!  {
//!    UINT aSize = 0;
//!    ::GetRawInputData ((HRAWINPUT )theLParam, RID_INPUT, NULL, &aSize, sizeof(RAWINPUTHEADER));
//!    NCollection_LocalArray<BYTE> aRawData (aSize); // receive Raw Input for any device and
//!    process known devices if (aSize == 0 || ::GetRawInputData ((HRAWINPUT )theLParam, RID_INPUT,
//!    aRawData, &aSize, sizeof(RAWINPUTHEADER)) != aSize)
//!    {
//!      break;
//!    }
//!    const RAWINPUT* aRawInput = (RAWINPUT* )(BYTE* )aRawData;
//!    if (aRawInput->header.dwType != RIM_TYPEHID)
//!    {
//!      break;
//!    }
//!
//!    RID_DEVICE_INFO aDevInfo; aDevInfo.cbSize = sizeof(RID_DEVICE_INFO);
//!    UINT aDevInfoSize = sizeof(RID_DEVICE_INFO);
//!    if (::GetRawInputDeviceInfoW (aRawInput->header.hDevice, RIDI_DEVICEINFO, &aDevInfo,
//!    &aDevInfoSize) != sizeof(RID_DEVICE_INFO)
//!     || (aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_LOGITECH
//!      && aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_3DCONNEXION))
//!    {
//!      break;
//!    }
//!
//!    Aspect_VKeySet& aKeys = theViewCtrl.ChangeKeys();
//!    const double aTimeStamp = theViewCtrl.EventTime();
//!    WNT_HIDSpaceMouse aSpaceData (aDevInfo.hid.dwProductId, aRawInput->data.hid.bRawData,
//!    aRawInput->data.hid.dwSizeHid); if (aSpaceData.IsTranslation())
//!    {
//!      // process translation input
//!      bool isIdle = true, isQuadric = true;
//!      const Graphic3d_Vec3d aTrans = aSpaceData.Translation (isIdle, isQuadric);
//!      aKeys.KeyFromAxis (Aspect_VKey_NavSlideLeft, Aspect_VKey_NavSlideRight, aTimeStamp,
//!      aTrans.x()); aKeys.KeyFromAxis (Aspect_VKey_NavForward,   Aspect_VKey_NavBackward,
//!      aTimeStamp, aTrans.y()); aKeys.KeyFromAxis (Aspect_VKey_NavSlideUp,
//!      Aspect_VKey_NavSlideDown,  aTimeStamp, aTrans.z());
//!    }
//!    if (aSpaceData.IsRotation()) {} // process rotation input
//!    if (aSpaceData.IsKeyState()) {} // process keys input
//!    break;
//!  }
//! @endcode
class WNT_HIDSpaceMouse
{
public:
  //! Vendor HID identifier.
  enum
  {
    VENDOR_ID_LOGITECH    = 0x46D,
    VENDOR_ID_3DCONNEXION = 0x256F
  };

  //! Return if product id is known by this class.
  Standard_EXPORT static bool IsKnownProduct(unsigned long theProductId);

public:
  //! Main constructor.
  Standard_EXPORT WNT_HIDSpaceMouse(unsigned long        theProductId,
                                    const Standard_Byte* theData,
                                    Standard_Size        theSize);

  //! Return the raw value range.
  int16_t RawValueRange() const { return myValueRange; }

  //! Set the raw value range.
  void SetRawValueRange(int16_t theRange)
  {
    myValueRange = theRange > myValueRange ? theRange : myValueRange;
  }

  //! Return TRUE if data chunk defines new translation values.
  bool IsTranslation() const
  {
    return myData[0] == SpaceRawInput_Translation && (mySize == 7 || mySize == 13);
  }

  //! Return new translation values.
  //! @param[out] theIsIdle  flag indicating idle state (no translation)
  //! @param[in] theIsQuadric  flag to apply non-linear scale factor
  //! @return vector of 3 elements defining translation values within [-1..1] range, 0 meaning idle,
  //!         .x defining left/right slide, .y defining forward/backward and .z defining up/down
  //!         slide.
  Standard_EXPORT Graphic3d_Vec3d Translation(bool& theIsIdle, bool theIsQuadric) const;

  //! Return TRUE if data chunk defines new rotation values.
  bool IsRotation() const
  {
    return (myData[0] == SpaceRawInput_Rotation && mySize == 7)
           || (myData[0] == SpaceRawInput_Translation && mySize == 13);
  }

  //! Return new rotation values.
  //! @param[out] theIsIdle  flag indicating idle state (no rotation)
  //! @param[in] theIsQuadric  flag to apply non-linear scale factor
  //! @return vector of 3 elements defining rotation values within [-1..1] range, 0 meaning idle,
  //!         .x defining tilt, .y defining roll and .z defining spin.
  Standard_EXPORT Graphic3d_Vec3d Rotation(bool& theIsIdle, bool theIsQuadric) const;

  //! Return TRUE for key state data chunk.
  bool IsKeyState() const { return myData[0] == SpaceRawInput_KeyState; }

  //! Return new keystate.
  uint32_t KeyState() const { return *reinterpret_cast<const uint32_t*>(myData + 1); }

  //! Convert key state bit into virtual key.
  Standard_EXPORT Aspect_VKey HidToSpaceKey(unsigned short theKeyBit) const;

private:
  //! Translate raw data chunk of 3 int16 values into normalized vec3.
  //! The values are considered within the range [-350; 350], with 0 as neutral state.
  Graphic3d_Vec3d fromRawVec3(bool&                theIsIdle,
                              const Standard_Byte* theData,
                              bool                 theIsTrans,
                              bool                 theIsQuadric) const;

  //! Data chunk type.
  enum
  {
    SpaceRawInput_Translation = 0x01, //!< translation data chunk
    SpaceRawInput_Rotation    = 0x02, //!< rotation    data chunk
    SpaceRawInput_KeyState    = 0x03, //!< keystate    data chunk
  };

private:
  const Standard_Byte* myData;       //!< RAW data chunk
  Standard_Size        mySize;       //!< size of RAW data chunk
  unsigned long        myProductId;  //!< product id
  mutable int16_t      myValueRange; //!< RAW value range
};

#endif // _WNT_HIDSpaceMouse_Header
