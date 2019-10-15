// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef _WasmOcctView_HeaderFile
#define _WasmOcctView_HeaderFile

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

#include <emscripten.h>
#include <emscripten/html5.h>

class AIS_ViewCube;

//! Sample class creating 3D Viewer within Emscripten canvas.
class WasmOcctView : protected AIS_ViewController
{
public:
  //! Default constructor.
  WasmOcctView();

  //! Destructor.
  virtual ~WasmOcctView();

  //! Main application entry point.
  void run();

  //! Return interactive context.
  const Handle(AIS_InteractiveContext)& Context() const { return myContext; }

  //! Return view.
  const Handle(V3d_View)& View() const { return myView; }

  //! Return device pixel ratio for handling high DPI displays.
  float DevicePixelRatio() const { return myDevicePixelRatio; }

private:

  //! Create window.
  void initWindow();

  //! Create 3D Viewer.
  bool initViewer();

  //! Fill 3D Viewer with a DEMO items.
  void initDemoScene();

  //! Application event loop.
  void mainloop();

  //! Request view redrawing.
  void updateView();

  //! Flush events and redraw view.
  void redrawView();

  //! Handle view redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) override;

  //! Dump WebGL context information.
  void dumpGlInfo (bool theIsBasic);

  //! Initialize pixel scale ratio.
  void initPixelScaleRatio();

  //! Return point from logical units to backing store.
  Graphic3d_Vec2d convertPointToBacking (const Graphic3d_Vec2d& thePnt) const
  {
    return thePnt * myDevicePixelRatio;
  }

  //! Return point from logical units to backing store.
  Graphic3d_Vec2i convertPointToBacking (const Graphic3d_Vec2i& thePnt) const
  {
    Graphic3d_Vec2d aPnt = Graphic3d_Vec2d (thePnt) * myDevicePixelRatio + Graphic3d_Vec2d (0.5);
    return Graphic3d_Vec2i (aPnt);
  }

//! @name Emscripten callbacks
private:
  //! Window resize event.
  EM_BOOL onResizeEvent (int theEventType, const EmscriptenUiEvent* theEvent);

  //! Mouse event.
  EM_BOOL onMouseEvent (int theEventType, const EmscriptenMouseEvent* theEvent);

  //! Scroll event.
  EM_BOOL onWheelEvent (int theEventType, const EmscriptenWheelEvent* theEvent);

  //! Touch event.
  EM_BOOL onTouchEvent (int theEventType, const EmscriptenTouchEvent* theEvent);

  //! Key down event.
  EM_BOOL onKeyDownEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent);

  //! Key up event.
  EM_BOOL onKeyUpEvent (int theEventType, const EmscriptenKeyboardEvent* theEvent);

//! @name Emscripten callbacks (static functions)
private:

  static EM_BOOL onResizeCallback (int theEventType, const EmscriptenUiEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onResizeEvent (theEventType, theEvent); }

  static void onRedrawView (void* theView)
  { return ((WasmOcctView* )theView)->redrawView(); }

  static EM_BOOL onMouseCallback (int theEventType, const EmscriptenMouseEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onMouseEvent (theEventType, theEvent); }

  static EM_BOOL onWheelCallback (int theEventType, const EmscriptenWheelEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onWheelEvent (theEventType, theEvent); }

  static EM_BOOL onTouchCallback (int theEventType, const EmscriptenTouchEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onTouchEvent (theEventType, theEvent); }

  static EM_BOOL onKeyDownCallback (int theEventType, const EmscriptenKeyboardEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onKeyDownEvent (theEventType, theEvent); }

  static EM_BOOL onKeyUpCallback (int theEventType, const EmscriptenKeyboardEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onKeyUpEvent (theEventType, theEvent); }

private:

  Handle(AIS_InteractiveContext) myContext;          //!< interactive context
  Handle(V3d_View)               myView;             //!< 3D view
  Handle(Prs3d_TextAspect)       myTextStyle;        //!< text style for OSD elements
  Handle(AIS_ViewCube)           myViewCube;         //!< view cube object
  TCollection_AsciiString        myCanvasId;         //!< canvas element id on HTML page
  Aspect_Touch                   myClickTouch;       //!< single touch position for handling clicks
  OSD_Timer                      myDoubleTapTimer;   //!< timer for handling double tap
  float                          myDevicePixelRatio; //!< device pixel ratio for handling high DPI displays
  unsigned int                   myUpdateRequests;   //!< counter for unhandled update requests

};

#endif // _WasmOcctView_HeaderFile
