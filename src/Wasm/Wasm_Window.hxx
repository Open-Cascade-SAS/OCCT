// Created by: Kirill Gavrilov
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Wasm_Window_HeaderFile
#define _Wasm_Window_HeaderFile

#include <Aspect_Window.hxx>

#include <Aspect_VKey.hxx>
#include <Aspect_Handle.hxx>
#include <Graphic3d_Vec2.hxx>

class Aspect_WindowInputListener;

struct EmscriptenMouseEvent;
struct EmscriptenWheelEvent;
struct EmscriptenTouchEvent;
struct EmscriptenKeyboardEvent;
struct EmscriptenUiEvent;
struct EmscriptenFocusEvent;

//! This class defines WebAssembly window (HTML5 canvas) intended for creation of OpenGL (WebGL) context.
//!
//! Note that canvas may define an independent dimensions for backing store (WebGL buffer to render)
//! and for CSS (logical units to present buffer onto screen).
//! These dimensions differ when browser is dragged into a high pixel density screen (HiDPI),
//! or when user scales page in the browser (in both cases window.devicePixelRatio JavaScript property becomes not equal to 1.0).
//!
//! By default, Wasm_Window::DoResize() will scale backing store of a canvas basing on DevicePixelRatio() scale factor
//! to ensure canvas content being rendered with the native resolution and not stretched by browser.
//! This, however, might have side effects:
//! - a slow GPU might experience performance issues on drawing into larger buffer (e.g. HiDPI);
//! - user interface displayed in 3D Viewer (e.g. AIS presentations) should be scaled proportionally to be accessible,
//!   which might require extra processing at application level.
//! Consider changing ToScaleBacking flag passed to Wasm_Window constructor in case of issues.
class Wasm_Window : public Aspect_Window
{
  DEFINE_STANDARD_RTTIEXT(Wasm_Window, Aspect_Window)
public:

  //! Convert Emscripten mouse buttons into Aspect_VKeyMouse.
  Standard_EXPORT static Aspect_VKeyMouse MouseButtonsFromNative (unsigned short theButtons);

  //! Convert DOM virtual key into Aspect_VKey.
  Standard_EXPORT static Aspect_VKey VirtualKeyFromNative (Standard_Integer theKey);

public:

  //! Wraps existing HTML5 canvas into window.
  //! @param[in] theCanvasId target HTML element id defined in a querySelector() syntax
  //! @param[in] theToScaleBacking when TRUE, window will automatically scale backing store of canvas
  //!                              basing on DevicePixelRatio() scale factor within DoResize()
  Standard_EXPORT Wasm_Window (const TCollection_AsciiString& theCanvasId,
                               const bool theToScaleBacking = true);

  //! Destroys the window.
  Standard_EXPORT virtual ~Wasm_Window();

  //! Return true if window is not hidden.
  virtual Standard_Boolean IsMapped() const Standard_OVERRIDE { return myIsMapped; }

  //! Change window mapped flag to TRUE.
  virtual void Map()   const Standard_OVERRIDE { myIsMapped = Standard_True; }

  //! Change window mapped flag to FALSE.
  virtual void Unmap() const Standard_OVERRIDE { myIsMapped = Standard_False; }

  //! Resize window.
  //! In case of ToScaleBacking flag, this method will resize the backing store of canvas
  //! basing on DevicePixelRatio() scale factor and CSS canvas size.
  Standard_EXPORT virtual Aspect_TypeOfResize DoResize() Standard_OVERRIDE;

  //! Apply the mapping change to the window.
  virtual Standard_Boolean DoMapping() const Standard_OVERRIDE { return Standard_True; }

  //! Returns window ratio equal to the physical width/height dimensions.
  Standard_EXPORT virtual Standard_Real Ratio() const Standard_OVERRIDE;

  //! Returns The Window POSITION in PIXEL
  Standard_EXPORT virtual void Position (Standard_Integer& theX1,
                                         Standard_Integer& theY1,
                                         Standard_Integer& theX2,
                                         Standard_Integer& theY2) const Standard_OVERRIDE;

  //! Return the window size in pixels.
  Standard_EXPORT virtual void Size (Standard_Integer& theWidth,
                                     Standard_Integer& theHeight) const Standard_OVERRIDE;

  //! Set new window size in logical (density-independent units).
  //! Backing store will be resized basing on DevicePixelRatio().
  Standard_EXPORT void SetSizeLogical (const Graphic3d_Vec2d& theSize);

  //! Set new window size in pixels.
  //! Logical size of the element will be resized basing on DevicePixelRatio().
  Standard_EXPORT void SetSizeBacking (const Graphic3d_Vec2i& theSize);

  //! Returns canvas id.
  const TCollection_AsciiString& CanvasId() const { return myCanvasId; }

  //! Current EGL implementation in Emscripten accepts only 0 for native window id.
  virtual Aspect_Drawable NativeHandle() const Standard_OVERRIDE { return 0; }

  //! Always returns 0 for this class.
  virtual Aspect_Drawable NativeParentHandle() const Standard_OVERRIDE { return 0; }

  //! Always returns 0 for this class.
  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return 0; }

  //! Return device pixel ratio (logical to backing store scale factor).
  virtual Standard_Real DevicePixelRatio() const Standard_OVERRIDE { return myDevicePixelRatio; }

  //! Sets device pixel ratio for a window with IsVirtual() flag.
  void SetDevicePixelRatio (Standard_Real theDevicePixelRatio) { myDevicePixelRatio = theDevicePixelRatio; }

  //! Invalidate entire window content through generation of Expose event.
  Standard_EXPORT virtual void InvalidateContent (const Handle(Aspect_DisplayConnection)& theDisp) Standard_OVERRIDE;

public:

  //! Process a single window message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessMessage (Aspect_WindowInputListener& theListener,
                                               int theEventType, const void* theEvent);

  //! Process a mouse input message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessMouseEvent (Aspect_WindowInputListener& theListener,
                                                  int theEventType, const EmscriptenMouseEvent* theEvent);

  //! Process a (mouse) wheel input message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessWheelEvent (Aspect_WindowInputListener& theListener,
                                                  int theEventType, const EmscriptenWheelEvent* theEvent);

  //! Process a mouse input message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessTouchEvent (Aspect_WindowInputListener& theListener,
                                                  int theEventType, const EmscriptenTouchEvent* theEvent);

  //! Process a keyboard input message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessKeyEvent (Aspect_WindowInputListener& theListener,
                                               int theEventType, const EmscriptenKeyboardEvent* theEvent);

  //! Process a UI input message (like window resize).
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessUiEvent (Aspect_WindowInputListener& theListener,
                                               int theEventType, const EmscriptenUiEvent* theEvent);

  //! Process a focus input change message.
  //! @param[in,out] theListener listener to redirect message
  //! @param[in] theEventType message type to process
  //! @param[in] theEvent message to process
  //! @return TRUE if message has been processed
  Standard_EXPORT virtual bool ProcessFocusEvent (Aspect_WindowInputListener& theListener,
                                                  int theEventType, const EmscriptenFocusEvent* theEvent);

protected:

  TCollection_AsciiString  myCanvasId;
  Graphic3d_Vec2i          mySize;
  Standard_Real            myDevicePixelRatio;
  Standard_Boolean         myToScaleBacking;
  mutable Standard_Boolean myIsMapped;

};

#endif // _Wasm_Window_HeaderFile
