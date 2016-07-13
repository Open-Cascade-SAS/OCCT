// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef OpenGl_AspectText_Header
#define OpenGl_AspectText_Header

#include <Font_FontAspect.hxx>
#include <Graphic3d_AspectText3d.hxx>

#include <TCollection_AsciiString.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_ShaderProgram;

//! Text representation parameters
class OpenGl_AspectText : public OpenGl_Element
{

public:

  //! Empty constructor.
  Standard_EXPORT OpenGl_AspectText();

  //! Create and assign parameters.
  Standard_EXPORT OpenGl_AspectText (const Handle(Graphic3d_AspectText3d)& theAspect);

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_AspectText();

  //! Return text aspect.
  const Handle(Graphic3d_AspectText3d)& Aspect() const { return myAspect; }

  //! Assign new parameters.
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectText3d)& theAspect);

  //! Init and return OpenGl shader program resource.
  //! @return shader program resource.
  const Handle(OpenGl_ShaderProgram)& ShaderProgramRes (const Handle(OpenGl_Context)& theCtx) const
  {
    if (!myResources.IsShaderReady())
    {
      myResources.BuildShader (theCtx, myAspect->ShaderProgram());
      myResources.SetShaderReady();
    }

    return myResources.ShaderProgram;
  }

  Standard_EXPORT virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  Standard_EXPORT virtual void Release (OpenGl_Context* theContext);

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

  Handle(Graphic3d_AspectText3d) myAspect;

public:

  DEFINE_STANDARD_ALLOC

};

#endif // OpenGl_AspectText_Header
