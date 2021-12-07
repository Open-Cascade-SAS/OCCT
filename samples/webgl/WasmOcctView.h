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

  //! Return global viewer instance.
  static WasmOcctView& Instance();

public: //! @name methods exported by Module

  //! Set cubemap background.
  //! File will be loaded asynchronously.
  //! @param theImagePath [in] image path to load
  static void setCubemapBackground (const std::string& theImagePath);

  //! Clear all named objects from viewer.
  static void removeAllObjects();

  //! Fit all/selected objects into view.
  //! @param theAuto [in] fit selected objects (TRUE) or all objects (FALSE)
  static void fitAllObjects (bool theAuto);

  //! Remove named object from viewer.
  //! @param theName [in] object name
  //! @return FALSE if object was not found
  static bool removeObject (const std::string& theName);

  //! Temporarily hide named object.
  //! @param theName [in] object name
  //! @return FALSE if object was not found
  static bool eraseObject (const std::string& theName);

  //! Display temporarily hidden object.
  //! @param theName [in] object name
  //! @return FALSE if object was not found
  static bool displayObject (const std::string& theName);

  //! Show/hide ground.
  //! @param theToShow [in] show or hide flag
  static void displayGround (bool theToShow);

  //! Open object from the given URL.
  //! File will be loaded asynchronously.
  //! @param theName      [in] object name
  //! @param theModelPath [in] model path
  static void openFromUrl (const std::string& theName,
                           const std::string& theModelPath);

  //! Open object from memory.
  //! @param theName    [in] object name
  //! @param theBuffer  [in] pointer to data
  //! @param theDataLen [in] data length
  //! @param theToFree  [in] free theBuffer if set to TRUE
  //! @return FALSE on reading error
  static bool openFromMemory (const std::string& theName,
                              uintptr_t theBuffer, int theDataLen,
                              bool theToFree);

  //! Open BRep object from memory.
  //! @param theName    [in] object name
  //! @param theBuffer  [in] pointer to data
  //! @param theDataLen [in] data length
  //! @param theToFree  [in] free theBuffer if set to TRUE
  //! @return FALSE on reading error
  static bool openBRepFromMemory (const std::string& theName,
                                  uintptr_t theBuffer, int theDataLen,
                                  bool theToFree);

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

  //! Request view redrawing.
  void UpdateView();

private:

  //! Create window.
  void initWindow();

  //! Create 3D Viewer.
  bool initViewer();

  //! Fill 3D Viewer with a DEMO items.
  void initDemoScene();

  //! Application event loop.
  void mainloop();

  //! Flush events and redraw view.
  void redrawView();

  //! Handle view redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) override;

  //! Schedule processing of window input events with the next repaint event.
  virtual void ProcessInput() override;

  //! Handle key down event.
  virtual void KeyDown (Aspect_VKey theKey,
                        double theTime,
                        double thePressure) override;

  //! Handle key up event.
  virtual void KeyUp (Aspect_VKey theKey,
                      double theTime) override;

  //! Dump WebGL context information.
  void dumpGlInfo (bool theIsBasic);

  //! Initialize pixel scale ratio.
  void initPixelScaleRatio();

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

  //! Focus change event.
  EM_BOOL onFocusEvent (int theEventType, const EmscriptenFocusEvent* theEvent);

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

  static EM_BOOL onFocusCallback (int theEventType, const EmscriptenFocusEvent* theEvent, void* theView)
  { return ((WasmOcctView* )theView)->onFocusEvent (theEventType, theEvent); }

private:

  //! Register hot-keys for specified Action.
  void addActionHotKeys (Aspect_VKey theAction,
                         unsigned int theHotKey1 = 0,
                         unsigned int theHotKey2 = 0,
                         unsigned int theHotKey3 = 0,
                         unsigned int theHotKey4 = 0,
                         unsigned int theHotKey5 = 0)
  {
    if (theHotKey1 != 0) { myNavKeyMap.Bind (theHotKey1, theAction); }
    if (theHotKey2 != 0) { myNavKeyMap.Bind (theHotKey2, theAction); }
    if (theHotKey3 != 0) { myNavKeyMap.Bind (theHotKey3, theAction); }
    if (theHotKey4 != 0) { myNavKeyMap.Bind (theHotKey4, theAction); }
    if (theHotKey5 != 0) { myNavKeyMap.Bind (theHotKey5, theAction); }
  }

  //! Handle navigation keys.
  bool navigationKeyModifierSwitch (unsigned int theModifOld,
                                    unsigned int theModifNew,
                                    double       theTimeStamp);

  //! Handle hot-key.
  bool processKeyPress (Aspect_VKey theKey);

private:

  NCollection_IndexedDataMap<TCollection_AsciiString, Handle(AIS_InteractiveObject)> myObjects; //!< map of named objects

  NCollection_DataMap<unsigned int, Aspect_VKey> myNavKeyMap; //!< map of Hot-Key (key+modifiers) to Action

  Handle(AIS_InteractiveContext) myContext;          //!< interactive context
  Handle(V3d_View)               myView;             //!< 3D view
  Handle(Prs3d_TextAspect)       myTextStyle;        //!< text style for OSD elements
  Handle(AIS_ViewCube)           myViewCube;         //!< view cube object
  TCollection_AsciiString        myCanvasId;         //!< canvas element id on HTML page
  Graphic3d_Vec2i                myWinSizeOld;
  float                          myDevicePixelRatio; //!< device pixel ratio for handling high DPI displays
  unsigned int                   myNbUpdateRequests; //!< counter for unhandled update requests

};

#endif // _WasmOcctView_HeaderFile
