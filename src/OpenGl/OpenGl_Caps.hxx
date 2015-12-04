// Created on: 2013-08-25
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_Caps_H__
#define _OpenGl_Caps_H__

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! Class to define graphic driver capabilities.
//! Notice that these options will be ignored if particular functionality does not provided by GL driver
class OpenGl_Caps : public Standard_Transient
{

public: //! @name flags to disable particular functionality, should be used only for testing purposes!

  Standard_Boolean vboDisable;        //!< flag permits VBO usage, will significantly affect performance (OFF by default)
  Standard_Boolean pntSpritesDisable; //!< flag permits Point Sprites usage, will significantly affect performance (OFF by default)
  Standard_Boolean keepArrayData;     //!< Disables freeing CPU memory after building VBOs (OFF by default)
  Standard_Boolean ffpEnable;         //!< Enables FFP (fixed-function pipeline), do not use built-in GLSL programs (ON by default on desktop OpenGL and OFF on OpenGL ES)
  Standard_Boolean useSystemBuffer;   //!< Enables usage of system backbuffer for blitting (OFF by default on desktop OpenGL and ON on OpenGL ES for testing)
  Standard_Integer swapInterval;      //!< controls swap interval - 0 for VSync off and 1 for VSync on, 1 by default

public: //! @name context creation parameters

  /**
   * Specify that driver should not swap back/front buffers at the end of frame.
   * Useful when OCCT Viewer is integrated into existing OpenGL rendering pipeline as part,
   * thus swapping part is performed outside.
   *
   * OFF by default.
   */
  Standard_Boolean buffersNoSwap;

  /**
   * Request stereoscopic context (with Quad Buffer). This flag requires support in OpenGL driver.
   *
   * OFF by default.
   */
  Standard_Boolean contextStereo;

  /**
   * Request debug GL context. This flag requires support in OpenGL driver.
   *
   * When turned on OpenGL driver emits error and warning messages to provided callback
   * (see OpenGl_Context - messages will be printed to standard output).
   * Affects performance - thus should not be turned on by products in released state.
   *
   * OFF by default. Currently implemented only for Windows (WGL).
   */
  Standard_Boolean contextDebug;

  /**
   * Request debug GL context to emit messages within main thread (when contextDebug is specified!).
   *
   * Some implementations performs GL rendering within dedicated thread(s),
   * in this case debug messages will be pushed from unknown thread making call stack useless,
   * since it does not interconnected to application calls.
   * This option asks GL driver to switch into synchronized implementation.
   * Affects performance - thus should not be turned on by products in released state.
   *
   * OFF by default.
   */
  Standard_Boolean contextSyncDebug;

  /**
   * Disable hardware acceleration.
   *
   * This flag overrides default behavior, when accelerated context always preferred over software ones:
   * - on Windows will force Microsoft software implementation;
   * - on Mac OS X, forces Apple software implementation.
   *
   * Software implementations are dramatically slower - should never be used.
   *
   * OFF by default. Currently implemented only for Windows (WGL) and Mac OS X (Cocoa).
   */
  Standard_Boolean contextNoAccel;

  /**
   * Request backward-compatible GL context. This flag requires support in OpenGL driver.
   *
   * Backward-compatible profile includes deprecated functionality like FFP (fixed-function pipeline),
   * and might be useful for compatibility with application OpenGL code.
   *
   * Most drivers support all features within backward-compatibility profile,
   * but some limit functionality to OpenGL 2.1 (e.g. OS X) when core profile is not explicitly requested.
   *
   * Requires OpenGL 3.2+ drivers.
   * Has no effect on OpenGL ES 2.0+ drivers (which do not provie FFP compatibility).
   * Interacts with ffpEnable option, which should be disabled within core profile.
   *
   * ON by default.
   */
  Standard_Boolean contextCompatible;

public: //! @name flags to activate verbose output

  //! Print GLSL program compilation/linkage warnings, if any. OFF by default.
  Standard_Boolean glslWarnings;

  //! Suppress redundant messages from debug GL context. ON by default.
  Standard_Boolean suppressExtraMsg;

public: //! @name class methods

  //! Default constructor - initialize with most optimal values.
  Standard_EXPORT OpenGl_Caps();

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_Caps();

  //! Copy maker.
  Standard_EXPORT OpenGl_Caps& operator= (const OpenGl_Caps& theCopy);

private:

  //! Not implemented
  OpenGl_Caps (const OpenGl_Caps& );

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_Caps,Standard_Transient) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Caps, Standard_Transient)

#endif // _OpenGl_Caps_H__
