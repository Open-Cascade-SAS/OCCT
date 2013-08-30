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

public: //! @name flags to disable particular functionality

  Standard_Boolean vboDisable;        //!< flag permits VBO usage, will significantly affect performance (OFF by default)
  Standard_Boolean pntSpritesDisable; //!< flag permits Point Sprites usage, will significantly affect performance (OFF by default)

public: //! @name context creation parameters

  Standard_Boolean contextDebug; //!< request debug GL context (requires support in OpenGL driver), useful for debugging of visualization code (OFF by default, currently implemented only fow Windows)

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
