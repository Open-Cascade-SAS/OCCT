// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_AspectLine_Header
#define _OpenGl_AspectLine_Header

#include <TCollection_AsciiString.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_CAspectLine.hxx>

#include <Handle_OpenGl_ShaderProgram.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectLine : public OpenGl_Element
{
public:

  Standard_EXPORT OpenGl_AspectLine();

  Standard_EXPORT OpenGl_AspectLine (const OpenGl_AspectLine &AnOther);

  Standard_EXPORT void SetAspect (const CALL_DEF_CONTEXTLINE& theAspect);

  const TEL_COLOUR&  Color() const { return myColor; }
  TEL_COLOUR&        ChangeColor() { return myColor; }
  Aspect_TypeOfLine  Type()  const { return myType; }
  float              Width() const { return myWidth; }

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsShaderReady())
    {
      myResources.BuildShader (theCtx, myShaderProgram);
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

protected:

  TEL_COLOUR                      myColor;
  Aspect_TypeOfLine               myType;
  float                           myWidth;
  Handle(Graphic3d_ShaderProgram) myShaderProgram;

protected:

  //! OpenGl resources
  mutable struct Resources
  {
  public:
    Resources() : myIsShaderReady (Standard_False) {}

    Standard_Boolean IsShaderReady() const { return myIsShaderReady; }
    void SetShaderReady()       { myIsShaderReady = Standard_True; }
    void ResetShaderReadiness() { myIsShaderReady = Standard_False; }

    Standard_EXPORT void BuildShader (const Handle(OpenGl_Context)&          theCtx,
                                      const Handle(Graphic3d_ShaderProgram)& theShader);

    Handle(OpenGl_ShaderProgram) ShaderProgram;
    TCollection_AsciiString      ShaderProgramId;

  private:

    Standard_Boolean myIsShaderReady;

  } myResources;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_AspectLine_Header
