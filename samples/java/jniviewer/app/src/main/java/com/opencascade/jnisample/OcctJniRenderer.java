// Copyright (c) 2014 OPEN CASCADE SAS
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

package com.opencascade.jnisample;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;

//! Wrapper for C++ OCCT viewer.
public class OcctJniRenderer implements GLSurfaceView.Renderer
{

  //! Wrapper for V3d_TypeOfOrientation
  enum TypeOfOrientation
  {
    Xpos, // front
    Ypos, // left
    Zpos, // top
    Xneg, // back
    Yneg, // right
    Zneg  // bottom
  };

  //! Empty constructor.
  OcctJniRenderer (GLSurfaceView theView,
                   float theScreenDensity)
  {
    myView = theView; // this makes cyclic dependency, but it is OK for JVM
    if (OcctJniActivity.areNativeLoaded)
    {
      myCppViewer = cppCreate (theScreenDensity);
    }
  }

  //! Open file.
  public void open (String thePath)
  {
    if (myCppViewer != 0)
    {
      cppOpen (myCppViewer, thePath);
    }
  }

  //! Update viewer.
  public void onDrawFrame (GL10 theGl)
  {
    if (myCppViewer != 0)
    {
      if (cppRedraw (myCppViewer))
      {
        myView.requestRender(); // this method is allowed from any thread
      }
    }
  }

  //! (re)initialize viewer.
  public void onSurfaceChanged (GL10 theGl, int theWidth, int theHeight)
  {
    if (myCppViewer != 0)
    {
      cppResize (myCppViewer, theWidth, theHeight);
    }
  }

  public void onSurfaceCreated (GL10 theGl, EGLConfig theEglConfig)
  {
    if (myCppViewer != 0)
    {
      cppInit (myCppViewer);
    }
  }

  //! Add touch point.
  public void onAddTouchPoint (int theId, float theX, float theY)
  {
    if (myCppViewer != 0)
    {
      cppAddTouchPoint (myCppViewer, theId, theX, theY);
    }
  }

  //! Update touch point.
  public void onUpdateTouchPoint (int theId, float theX, float theY)
  {
    if (myCppViewer != 0)
    {
      cppUpdateTouchPoint (myCppViewer, theId, theX, theY);
    }
  }

  //! Remove touch point.
  public void onRemoveTouchPoint (int theId)
  {
    if (myCppViewer != 0)
    {
      cppRemoveTouchPoint (myCppViewer, theId);
    }
  }

  //! Select in 3D Viewer.
  public void onSelectInViewer (float theX, float theY)
  {
    if (myCppViewer != 0)
    {
      cppSelectInViewer (myCppViewer, theX, theY);
    }
  }

  //! Fit All
  public void fitAll()
  {
    if (myCppViewer != 0)
    {
      cppFitAll (myCppViewer);
    }
  }

  //! Move camera
  public void setProj (TypeOfOrientation theProj)
  {
    if (myCppViewer == 0)
    {
      return;
    }

    switch (theProj)
    {
      case Xpos: cppSetXposProj (myCppViewer); break;
      case Ypos: cppSetYposProj (myCppViewer); break;
      case Zpos: cppSetZposProj (myCppViewer); break;
      case Xneg: cppSetXnegProj (myCppViewer); break;
      case Yneg: cppSetYnegProj (myCppViewer); break;
      case Zneg: cppSetZnegProj (myCppViewer); break;
    }
  }

  //! Post message to the text view.
  public void postMessage (String theText)
  {
    OcctJniLogger.postMessage (theText);
  }

  //! Create instance of C++ class
  private native long cppCreate (float theDispDensity);

  //! Destroy instance of C++ class
  private native void cppDestroy (long theCppPtr);

  //! Initialize OCCT viewer (steal OpenGL ES context bound to this thread)
  private native void cppInit    (long theCppPtr);

  //! Resize OCCT viewer
  private native void cppResize  (long theCppPtr, int theWidth, int theHeight);

  //! Open CAD file
  private native void cppOpen    (long theCppPtr, String thePath);

  //! Add touch point
  private native void cppAddTouchPoint (long theCppPtr, int theId, float theX, float theY);

  //! Update touch point
  private native void cppUpdateTouchPoint (long theCppPtr, int theId, float theX, float theY);

  //! Remove touch point
  private native void cppRemoveTouchPoint (long theCppPtr, int theId);

  //! Select in 3D Viewer.
  private native void cppSelectInViewer (long theCppPtr, float theX, float theY);

  //! Redraw OCCT viewer
  //! Returns TRUE if more frames are requested.
  private native boolean cppRedraw  (long theCppPtr);

  //! Fit All
  private native void cppFitAll  (long theCppPtr);

  //! Move camera
  private native void cppSetXposProj (long theCppPtr);

  //! Move camera
  private native void cppSetYposProj (long theCppPtr);

  //! Move camera
  private native void cppSetZposProj (long theCppPtr);

  //! Move camera
  private native void cppSetXnegProj (long theCppPtr);

  //! Move camera
  private native void cppSetYnegProj (long theCppPtr);

  //! Move camera
  private native void cppSetZnegProj (long theCppPtr);

  private GLSurfaceView myView = null; //!< back reference to the View
  private long myCppViewer = 0;   //!< pointer to c++ class instance

}
