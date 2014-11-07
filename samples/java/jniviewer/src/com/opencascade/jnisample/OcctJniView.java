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

import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.graphics.PointF;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.SparseArray;
import android.view.MotionEvent;
import android.widget.RelativeLayout;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

//! OpenGL ES 2.0+ view.
//! Performs rendering in parallel thread.
class OcctJniView extends GLSurfaceView
{

  // ! Default constructor.
  public OcctJniView (Context      theContext,
                      AttributeSet theAttrs)
  {
    super (theContext, theAttrs);

    setPreserveEGLContextOnPause (true);
    setEGLContextFactory (new ContextFactory());
    setEGLConfigChooser  (new ConfigChooser());

    RelativeLayout.LayoutParams aLParams = new RelativeLayout.LayoutParams (LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
    aLParams.addRule (RelativeLayout.ALIGN_TOP);

    myRenderer = new OcctJniRenderer();
    setRenderer (myRenderer);
  }

  //! Open file.
  public void open (String thePath)
  {
    final String aPath = thePath;
    queueEvent (new Runnable() { public void run() { myRenderer.open (aPath); }});
  }

  //! Create OpenGL ES 2.0+ context
  private static class ContextFactory implements GLSurfaceView.EGLContextFactory
  {
    private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    public EGLContext createContext (EGL10      theEgl,
                                     EGLDisplay theEglDisplay,
                                     EGLConfig  theEglConfig)
    {
      if (theEglConfig == null)
      {
        return null;
      }

      // reset EGL errors stack
      int anError = EGL10.EGL_SUCCESS;
      while ((anError = theEgl.eglGetError()) != EGL10.EGL_SUCCESS) {}

      int[]      anAttribs   = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
      EGLContext aEglContext = theEgl.eglCreateContext (theEglDisplay, theEglConfig, EGL10.EGL_NO_CONTEXT, anAttribs);

      while ((anError = theEgl.eglGetError()) != EGL10.EGL_SUCCESS)
      {
        OcctJniLogger.postMessage ("Error: eglCreateContext() " + String.format ("0x%x", anError));
      }
      return aEglContext;
    }

    public void destroyContext (EGL10      theEgl,
                                EGLDisplay theEglDisplay,
                                EGLContext theEglContext)
    {
      theEgl.eglDestroyContext (theEglDisplay, theEglContext);
    }
  }

  //! Search for RGB24 config with depth and stencil buffers
  private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
  {
    //! Reset EGL errors stack
    private void popEglErrors (EGL10 theEgl)
    {
      int anError = EGL10.EGL_SUCCESS;
      while ((anError = theEgl.eglGetError()) != EGL10.EGL_SUCCESS)
      {
        OcctJniLogger.postMessage ("EGL Error: " + String.format ("0x%x", anError));
      }
    }

    //! Auxiliary method to dump EGL configuration - for debugging purposes
    @SuppressWarnings("unused")
    private void printConfig (EGL10      theEgl,
                              EGLDisplay theEglDisplay,
                              EGLConfig  theEglConfig)
    {
      int[] THE_ATTRIBS =
      {
        EGL10.EGL_BUFFER_SIZE, EGL10.EGL_ALPHA_SIZE, EGL10.EGL_BLUE_SIZE, EGL10.EGL_GREEN_SIZE, EGL10.EGL_RED_SIZE, EGL10.EGL_DEPTH_SIZE, EGL10.EGL_STENCIL_SIZE,
        EGL10.EGL_CONFIG_CAVEAT,
        EGL10.EGL_CONFIG_ID,
        EGL10.EGL_LEVEL,
        EGL10.EGL_MAX_PBUFFER_HEIGHT, EGL10.EGL_MAX_PBUFFER_PIXELS, EGL10.EGL_MAX_PBUFFER_WIDTH,
        EGL10.EGL_NATIVE_RENDERABLE,  EGL10.EGL_NATIVE_VISUAL_ID,   EGL10.EGL_NATIVE_VISUAL_TYPE,
        0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
        EGL10.EGL_SAMPLES, EGL10.EGL_SAMPLE_BUFFERS,
        EGL10.EGL_SURFACE_TYPE,
        EGL10.EGL_TRANSPARENT_TYPE, EGL10.EGL_TRANSPARENT_RED_VALUE, EGL10.EGL_TRANSPARENT_GREEN_VALUE, EGL10.EGL_TRANSPARENT_BLUE_VALUE,
        0x3039, 0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGB, EGL10.EGL_BIND_TO_TEXTURE_RGBA,
        0x303B, 0x303C, // EGL10.EGL_MIN_SWAP_INTERVAL, EGL10.EGL_MAX_SWAP_INTERVAL
        EGL10.EGL_LUMINANCE_SIZE, EGL10.EGL_ALPHA_MASK_SIZE,
        EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RENDERABLE_TYPE,
        0x3042 // EGL10.EGL_CONFORMANT
      };
      String[] THE_NAMES =
      {
        "EGL_BUFFER_SIZE", "EGL_ALPHA_SIZE", "EGL_BLUE_SIZE", "EGL_GREEN_SIZE", "EGL_RED_SIZE", "EGL_DEPTH_SIZE", "EGL_STENCIL_SIZE",
        "EGL_CONFIG_CAVEAT",
        "EGL_CONFIG_ID",
        "EGL_LEVEL",
        "EGL_MAX_PBUFFER_HEIGHT", "EGL_MAX_PBUFFER_PIXELS", "EGL_MAX_PBUFFER_WIDTH",
        "EGL_NATIVE_RENDERABLE",  "EGL_NATIVE_VISUAL_ID",   "EGL_NATIVE_VISUAL_TYPE",
        "EGL_PRESERVED_RESOURCES",
        "EGL_SAMPLES", "EGL_SAMPLE_BUFFERS",
        "EGL_SURFACE_TYPE",
        "EGL_TRANSPARENT_TYPE", "EGL_TRANSPARENT_RED_VALUE", "EGL_TRANSPARENT_GREEN_VALUE", "EGL_TRANSPARENT_BLUE_VALUE",
        "EGL_BIND_TO_TEXTURE_RGB", "EGL_BIND_TO_TEXTURE_RGBA",
        "EGL_MIN_SWAP_INTERVAL", "EGL_MAX_SWAP_INTERVAL",
        "EGL_LUMINANCE_SIZE", "EGL_ALPHA_MASK_SIZE",
        "EGL_COLOR_BUFFER_TYPE", "EGL_RENDERABLE_TYPE",
        "EGL_CONFORMANT"
      };
      int[] aValue = new int[1];
      for (int anAttrIter = 0; anAttrIter < THE_ATTRIBS.length; ++anAttrIter)
      {
        int    anAttr = THE_ATTRIBS[anAttrIter];
        String aName  = THE_NAMES  [anAttrIter];
        if (theEgl.eglGetConfigAttrib (theEglDisplay, theEglConfig, anAttr, aValue))
        {
          OcctJniLogger.postMessage (String.format ("  %s: %d\n", aName, aValue[0]));
        }
        else
        {
          popEglErrors (theEgl);
        }
      }
    }

    //! Interface implementation
    public EGLConfig chooseConfig (EGL10      theEgl,
                                   EGLDisplay theEglDisplay)
    {
      int EGL_OPENGL_ES2_BIT = 4;
      int[] aCfgAttribs =
      {
        EGL10.EGL_RED_SIZE,     8,
        EGL10.EGL_GREEN_SIZE,   8,
        EGL10.EGL_BLUE_SIZE,    8,
        EGL10.EGL_ALPHA_SIZE,   0,
        EGL10.EGL_DEPTH_SIZE,  24,
        EGL10.EGL_STENCIL_SIZE, 8,
        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL10.EGL_NONE
      };

      EGLConfig aConfigs[] = new EGLConfig[1];
      int[]     aNbConfigs = new int[1];
      if (!theEgl.eglChooseConfig (theEglDisplay, aCfgAttribs, aConfigs, 1, aNbConfigs)
       || aConfigs[0] == null)
      {
        aCfgAttribs[4 * 2 + 1] = 16; // try config with smaller depth buffer
        popEglErrors (theEgl);
        if (!theEgl.eglChooseConfig (theEglDisplay, aCfgAttribs, aConfigs, 1, aNbConfigs)
         || aConfigs[0] == null)
        {
          OcctJniLogger.postMessage ("Error: eglChooseConfig() has failed!");
          return null;
        }
      }

      //printConfig (theEgl, theEglDisplay, aConfigs[0]);
      return aConfigs[0];
    }
  }

  //! Callback to handle touch events
  @Override public boolean onTouchEvent (MotionEvent theEvent)
  {
    int aPointerIndex = theEvent.getActionIndex();
    int aPointerId    = theEvent.getPointerId (aPointerIndex);
    int aMaskedAction = theEvent.getActionMasked();
    switch (aMaskedAction)
    {
      case MotionEvent.ACTION_DOWN:
      case MotionEvent.ACTION_POINTER_DOWN:
      {
        PointF aPntLast = null;
        if (myActivePointers.size() >= 1)
        {
          aPntLast = myActivePointers.get (myActivePointers.keyAt (0));
        }

        final PointF aPnt = new PointF();
        aPnt.x = theEvent.getX (aPointerIndex);
        aPnt.y = theEvent.getY (aPointerIndex);
        myActivePointers.put (aPointerId, aPnt);

        switch (myActivePointers.size())
        {
          case 1:
          {
            final int aStartX = (int )aPnt.x;
            final int aStartY = (int )aPnt.y;
            queueEvent (new Runnable() { public void run() { myRenderer.onStartRotation (aStartX, aStartY); }});
            break;
          }
          case 2:
          {
            myPanFrom.x = (aPntLast.x + aPnt.x) * 0.5f;
            myPanFrom.y = (aPntLast.y + aPnt.y) * 0.5f;
            break;
          }
        }

        break;
      }
      case MotionEvent.ACTION_MOVE:
      {
        for (int aNbPointers = theEvent.getPointerCount(), aPntIter = 0; aPntIter < aNbPointers; ++aPntIter)
        {
          PointF aPnt = myActivePointers.get (theEvent.getPointerId (aPntIter));
          if (aPnt != null)
          {
            aPnt.x = theEvent.getX (aPntIter);
            aPnt.y = theEvent.getY (aPntIter);
          }
        }

        switch (myActivePointers.size())
        {
          case 1:
          {
            PointF aPnt = myActivePointers.get (theEvent.getPointerId (0));
            final int anX = (int )aPnt.x;
            final int anY = (int )aPnt.y;
            queueEvent (new Runnable() { public void run() { myRenderer.onRotation (anX, anY); }});
            break;
          }
          case 2:
          {
            PointF aPnt1 = myActivePointers.get (myActivePointers.keyAt (0));
            PointF aPnt2 = myActivePointers.get (myActivePointers.keyAt (1));
            PointF aPntAver = new PointF ((aPnt1.x + aPnt2.x) * 0.5f,
                                          (aPnt1.y + aPnt2.y) * 0.5f);
            final int aDX = (int )(aPntAver.x - myPanFrom.x);
            final int aDY = (int )(myPanFrom.y -aPntAver.y);
            myPanFrom.x = aPntAver.x;
            myPanFrom.y = aPntAver.y;
            queueEvent (new Runnable() { public void run() { myRenderer.onPanning (aDX, aDY); }});
          }
        }
        break;
      }
      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_POINTER_UP:
      case MotionEvent.ACTION_CANCEL:
      {
        myActivePointers.remove (aPointerId);
        if (myActivePointers.size() == 0)
        {
          final int aPressX      = (int )theEvent.getX (aPointerIndex);
          final int aPressY      = (int )theEvent.getY (aPointerIndex);
          double    aPressTimeMs = theEvent.getEventTime() - theEvent.getDownTime();
          if (aPressTimeMs < 100.0)
          {
            queueEvent (new Runnable() { public void run() { myRenderer.onClick (aPressX, aPressY); }});
            break;
          }
        }
        else if (myActivePointers.size() == 1)
        {
          PointF    aPnt    = myActivePointers.get (myActivePointers.keyAt (0));
          final int aStartX = (int )aPnt.x;
          final int aStartY = (int )aPnt.y;
          queueEvent (new Runnable() { public void run() { myRenderer.onStartRotation (aStartX, aStartY); }});
        }
        //queueEvent (new Runnable() { public void run() { myRenderer.onStopAction(); }});
        break;
      }
    }
    ///invalidate();
    return true;
  }

  //! Fit All
  public void fitAll()
  {
    queueEvent (new Runnable() { public void run() { myRenderer.fitAll(); }});
  }

  //! Move camera
  public void setProj (final OcctJniRenderer.TypeOfOrientation theProj)
  {
    queueEvent (new Runnable() { public void run() { myRenderer.setProj (theProj); }});
  }

  //! OCCT viewer
  private OcctJniRenderer     myRenderer = null;

  //! Touch events cache
  private SparseArray<PointF> myActivePointers = new SparseArray<PointF>();

  //! Starting point for panning event
  private PointF              myPanFrom  = new PointF (0.0f, 0.0f);

}
