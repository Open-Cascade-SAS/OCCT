// Created on: 1996-01-26
// Created by: PLOTNIKOV Eugeny
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _WNT_Window_HeaderFile
#define _WNT_Window_HeaderFile

#include <Aspect_Window.hxx>

#if defined(_WIN32) && !defined(OCCT_UWP)

  #include <Aspect_Drawable.hxx>
  #include <Aspect_VKey.hxx>
  #include <Aspect_Handle.hxx>
  #include <WNT_Dword.hxx>

class Aspect_WindowInputListener;
class WNT_WClass;
typedef struct tagMSG MSG;

//! This class defines Windows NT window
class WNT_Window : public Aspect_Window
{
  DEFINE_STANDARD_RTTIEXT(WNT_Window, Aspect_Window)
public:
  //! Convert WInAPI virtual key (VK_ enumeration) into Aspect_VKey.
  Standard_EXPORT static Aspect_VKey VirtualKeyFromNative(int theKey);

  //! Convert WPARAM from mouse event to key flags.
  Standard_EXPORT static Aspect_VKeyFlags MouseKeyFlagsFromEvent(WPARAM theKeys);

  //! Convert WPARAM from mouse event to mouse buttons bitmask.
  Standard_EXPORT static Aspect_VKeyMouse MouseButtonsFromEvent(WPARAM theKeys);

  //! Use GetAsyncKeyState() to fetch actual mouse key flags regardless of event loop.
  Standard_EXPORT static Aspect_VKeyFlags MouseKeyFlagsAsync();

  //! Use GetAsyncKeyState() to fetch actual mouse buttons state regardless of event loop.
  Standard_EXPORT static Aspect_VKeyMouse MouseButtonsAsync();

public:
  //! Creates a Window defined by his position and size in pixels from the Parent Window.
  //! Trigger: Raises WindowDefinitionError if the Position out of the Screen Space or the window
  //! creation failed.
  Standard_EXPORT WNT_Window(const char*                    theTitle,
                             const occ::handle<WNT_WClass>& theClass,
                             const WNT_Dword&               theStyle,
                             const int                      thePxLeft,
                             const int                      thePxTop,
                             const int                      thePxWidth,
                             const int                      thePxHeight,
                             const Quantity_NameOfColor     theBackColor = Quantity_NOC_MATRAGRAY,
                             const Aspect_Handle            theParent    = 0,
                             const Aspect_Handle            theMenu      = 0,
                             void* const                    theClientStruct = 0);

  //! Creates a Window based on the existing window handle.
  Standard_EXPORT WNT_Window(const Aspect_Handle        theHandle,
                             const Quantity_NameOfColor theBackColor = Quantity_NOC_MATRAGRAY);

  //! Destroys the Window and all resources attached to it.
  Standard_EXPORT virtual ~WNT_Window();

  //! Sets cursor for ENTIRE WINDOW CLASS to which the Window belongs.
  Standard_EXPORT void SetCursor(const Aspect_Handle theCursor) const;

  //! Opens the window <me>.
  Standard_EXPORT virtual void Map() const override;

  //! Opens a window according to the map mode.
  //! This method is specific to Windows NT.
  //! @param[in] theMapMode  can be one of SW_xxx constants defined in <windows.h>
  Standard_EXPORT void Map(const int theMapMode) const;

  //! Closes the window <me>.
  Standard_EXPORT virtual void Unmap() const override;

  //! Applies the resizing to the window <me>.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() override;

  //! Does nothing on Windows.
  virtual bool DoMapping() const override { return true; }

  //! Changes variables due to window position.
  Standard_EXPORT void SetPos(const int X, const int Y, const int X1, const int Y1);

  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  Standard_EXPORT virtual bool IsMapped() const override;

  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions.
  Standard_EXPORT virtual double Ratio() const override;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position(int& X1, int& Y1, int& X2, int& Y2) const override;

  //! Returns The Window SIZE in PIXEL
  Standard_EXPORT virtual void Size(int& Width, int& Height) const override;

  //! Returns native Window handle (HWND)
  virtual Aspect_Drawable NativeHandle() const override { return (Aspect_Drawable)myHWindow; }

  //! Returns parent of native Window handle (HWND on Windows).
  virtual Aspect_Drawable NativeParentHandle() const override
  {
    return (Aspect_Drawable)myHParentWindow;
  }

  //! Returns nothing on Windows
  virtual Aspect_FBConfig NativeFBConfig() const override { return NULL; }

  //! Sets window title.
  Standard_EXPORT virtual void SetTitle(const TCollection_AsciiString& theTitle) override;

  //! Invalidate entire window content by calling InvalidateRect() WinAPI function, resulting in
  //! WM_PAINT event put into window message loop. Method can be called from non-window thread, and
  //! system will also automatically aggregate multiple events into single one.
  Standard_EXPORT virtual void InvalidateContent(
    const occ::handle<Aspect_DisplayConnection>& theDisp =
      occ::handle<Aspect_DisplayConnection>()) override;

public:
  //! Returns the Windows NT handle of the created window <me>.
  Aspect_Handle HWindow() const { return myHWindow; }

  //! Returns the Windows NT handle parent of the created window <me>.
  Aspect_Handle HParentWindow() const { return myHParentWindow; }

  //! Raw input flags.
  enum RawInputMask
  {
    RawInputMask_Mouse      = 0x01, //!< HID_USAGE_GENERIC_MOUSE
    RawInputMask_SpaceMouse = 0x02, //!< HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER
  };

  //! RegisterRawInputDevices() wrapper.
  //! @param[in] theRawDeviceMask  mask of RawInputMask flags
  //! @return number of actually registered device types
  Standard_EXPORT int RegisterRawInputDevices(unsigned int theRawDeviceMask);

  //! Process a single window message.
  //! @param[in][out] theListener  listener to redirect message
  //! @param[in][out] theMsg  message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessMessage(Aspect_WindowInputListener& theListener, MSG& theMsg);

private:
  class TouchInputHelper;

protected:
  occ::handle<WNT_WClass>       myWClass;
  occ::handle<TouchInputHelper> myTouchInputHelper;
  Aspect_Handle                 myHWindow;
  Aspect_Handle                 myHParentWindow;
  int                           myXLeft;
  int                           myYTop;
  int                           myXRight;
  int                           myYBottom;
  bool                          myIsForeign;
};

#endif // _WIN32
#endif // _WNT_Window_HeaderFile
