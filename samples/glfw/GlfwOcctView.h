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

#ifndef _GlfwOcctView_Header
#define _GlfwOcctView_Header

#include "GlfwOcctWindow.h"

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

//! Sample class creating 3D Viewer within GLFW window.
class GlfwOcctView : protected AIS_ViewController
{
public:
  //! Default constructor.
	GlfwOcctView();

  //! Destructor.
	~GlfwOcctView();

  //! Main application entry point.
  void run();

private:

  //! Create GLFW window.
  void initWindow (int theWidth, int theHeight, const char* theTitle);

  //! Create 3D Viewer.
  void initViewer();

  //! Fill 3D Viewer with a DEMO items.
  void initDemoScene();

  //! Application event loop.
  void mainloop();

  //! Clean up before .
  void cleanup();

//! @name GLWF callbacks
private:
  //! Window resize event.
  void onResize (int theWidth, int theHeight);

  //! Mouse scroll event.
  void onMouseScroll (double theOffsetX, double theOffsetY);

  //! Mouse click event.
  void onMouseButton (int theButton, int theAction, int theMods);

  //! Mouse move event.
  void onMouseMove (int thePosX, int thePosY);

//! @name GLWF callbacks (static functions)
private:

  //! GLFW callback redirecting messages into Message::DefaultMessenger().
  static void errorCallback (int theError, const char* theDescription);

  //! Wrapper for glfwGetWindowUserPointer() returning this class instance.
  static GlfwOcctView* toView (GLFWwindow* theWin);

  //! Window resize callback.
  static void onResizeCallback (GLFWwindow* theWin, int theWidth, int theHeight)
  { toView(theWin)->onResize (theWidth, theHeight); }

  //! Frame-buffer resize callback.
  static void onFBResizeCallback (GLFWwindow* theWin, int theWidth, int theHeight)
  { toView(theWin)->onResize (theWidth, theHeight); }

  //! Mouse scroll callback.
  static void onMouseScrollCallback (GLFWwindow* theWin, double theOffsetX, double theOffsetY)
  { toView(theWin)->onMouseScroll (theOffsetX, theOffsetY); }

  //! Mouse click callback.
  static void onMouseButtonCallback (GLFWwindow* theWin, int theButton, int theAction, int theMods)
  { toView(theWin)->onMouseButton (theButton, theAction, theMods); }

  //! Mouse move callback.
  static void onMouseMoveCallback (GLFWwindow* theWin, double thePosX, double thePosY)
  { toView(theWin)->onMouseMove ((int )thePosX, (int )thePosY); }

private:

  Handle(GlfwOcctWindow) myOcctWindow;
  Handle(V3d_View) myView;
  Handle(AIS_InteractiveContext) myContext;

};

#endif // _GlfwOcctView_Header
