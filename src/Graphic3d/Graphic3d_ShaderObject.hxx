// Created on: 2013-09-20
// Created by: Denis BOGOLEPOV
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

#ifndef _Graphic3d_ShaderObject_HeaderFile
#define _Graphic3d_ShaderObject_HeaderFile

#include <OSD_Path.hxx>

#include <Graphic3d_TypeOfShaderObject.hxx>
#include <Graphic3d_ShaderObject_Handle.hxx>

//! This class is responsible for managing shader objects.
class Graphic3d_ShaderObject : public Standard_Transient
{
private:

  //! Creates new shader object of specified type.
  Standard_EXPORT Graphic3d_ShaderObject (const Graphic3d_TypeOfShaderObject theType);

public:

  //! Releases resources of shader object. 
  Standard_EXPORT virtual ~Graphic3d_ShaderObject();
  
  //! Checks if the shader object is valid or not.
  Standard_EXPORT virtual Standard_Boolean IsDone() const;
  
  //! Returns the full path to the shader source.
  const OSD_Path& Path() const { return myPath; }

  //! Returns the source code of the shader object.
  const TCollection_AsciiString& Source() const { return mySource; }

  //! Returns type of the shader object.
  Graphic3d_TypeOfShaderObject Type() const { return myType; }
  
  //! Returns unique ID used to manage resource in graphic driver.
  const TCollection_AsciiString& GetId() const { return myID; }

  //! Creates new shader object from specified file.
  Standard_EXPORT static Handle(Graphic3d_ShaderObject) CreateFromFile (const Graphic3d_TypeOfShaderObject theType,
                                                                        const TCollection_AsciiString&     thePath);

  //! Creates new shader object from specified source.
  Standard_EXPORT static Handle(Graphic3d_ShaderObject) CreateFromSource (const Graphic3d_TypeOfShaderObject theType,
                                                                          const TCollection_AsciiString&     theSource);

public:

  DEFINE_STANDARD_RTTI (Graphic3d_ShaderObject)

protected:

  TCollection_AsciiString myID;     //!< the ID of shader object
  TCollection_AsciiString mySource; //!< the source code of shader object
  OSD_Path                myPath;   //!< the path to shader source (may be empty)

private:
  
  //! The type of shader object.
  Graphic3d_TypeOfShaderObject myType;
};

#endif
