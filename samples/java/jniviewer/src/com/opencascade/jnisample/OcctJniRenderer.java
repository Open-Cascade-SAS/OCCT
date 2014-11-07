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
  OcctJniRenderer()
  {
    if (OcctJniActivity.areNativeLoaded)
    {
      myCppViewer = cppCreate();
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
      cppRedraw (myCppViewer);
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

  //! Initialize rotation (remember first point position)
  public void onStartRotation (int theStartX, int theStartY)
  {
    if (myCppViewer != 0)
    {
      cppStartRotation (myCppViewer, theStartX, theStartY);
    }
  }

  //! Perform rotation (relative to first point)
  public void onRotation (int theX, int theY)
  {
    if (myCppViewer != 0)
    {
      cppOnRotation (myCppViewer, theX, theY);
    }
  }

  //! Perform panning
  public void onPanning (int theDX, int theDY)
  {
    if (myCppViewer != 0)
    {
      cppOnPanning (myCppViewer, theDX, theDY);
    }
  }

  //! Perform selection
  public void onClick (int theX, int theY)
  {
    if (myCppViewer != 0)
    {
      cppOnClick (myCppViewer, theX, theY);
    }
  }

  //! Stop previously active action (e.g. discard first rotation point)
  public void onStopAction()
  {
    if (myCppViewer != 0)
    {
      cppStopAction (myCppViewer);
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
  private native long cppCreate();

  //! Destroy instance of C++ class
  private native void cppDestroy (long theCppPtr);

  //! Initialize OCCT viewer (steal OpenGL ES context bound to this thread)
  private native void cppInit    (long theCppPtr);

  //! Resize OCCT viewer
  private native void cppResize  (long theCppPtr, int theWidth, int theHeight);

  //! Open CAD file
  private native void cppOpen    (long theCppPtr, String thePath);

  //! Handle detection in the viewer
  private native void cppMoveTo  (long theCppPtr, int theX, int theY);

  //! Redraw OCCT viewer
  private native void cppRedraw  (long theCppPtr);

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

  //! Initialize rotation
  private native void cppStartRotation (long theCppPtr, int theStartX, int theStartY);

  //! Perform rotation
  private native void cppOnRotation    (long theCppPtr, int theX,  int theY);

  //! Perform panning
  private native void cppOnPanning     (long theCppPtr, int theDX, int theDY);

  //! Perform selection
  private native void cppOnClick       (long theCppPtr, int theX,  int theY);

  //! Stop action (rotation / panning / scaling)
  private native void cppStopAction    (long theCppPtr);

  private long myCppViewer = 0;   //!< pointer to c++ class instance

}
