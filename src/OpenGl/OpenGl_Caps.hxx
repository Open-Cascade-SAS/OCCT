// Created on: 2013-08-25
// Created by: Kirill GAVRILOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _OpenGl_Caps_H__
#define _OpenGl_Caps_H__

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <Handle_Standard_Transient.hxx>

//! Class to define graphich driver capabilities.
//! Notice that these options will be ignored if particular functionality does not provided by GL driver
class OpenGl_Caps : public Standard_Transient
{

public: //! @name flags to disable particular functionality, should be used only for testing purposes!

  Standard_Boolean vboDisable;        //!< flag permits VBO usage, will significantly affect performance (OFF by default)
  Standard_Boolean pntSpritesDisable; //!< flag permits Point Sprites usage, will significantly affect performance (OFF by default)
  Standard_Boolean keepArrayData;     //!< Disables freeing CPU memory after building VBOs (OFF by default)

public: //! @name context creation parameters

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

public: //! @name flags to activate verbose output

  //! Print GLSL program compilation/linkage warnings, if any. OFF by default.
  Standard_Boolean glslWarnings;

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

  DEFINE_STANDARD_RTTI(OpenGl_Caps) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Caps, Standard_Transient)

#endif // _OpenGl_Caps_H__
