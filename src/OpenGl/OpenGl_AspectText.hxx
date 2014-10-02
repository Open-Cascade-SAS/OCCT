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

#include <InterfaceGraphic_Graphic3d.hxx>
#include <Font_FontAspect.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>
#include <Graphic3d_CAspectText.hxx>

#include <TCollection_AsciiString.hxx>

#include <OpenGl_Element.hxx>
#include <Handle_OpenGl_ShaderProgram.hxx>

//! Text representation parameters
class OpenGl_AspectText : public OpenGl_Element
{

public:

  Standard_EXPORT OpenGl_AspectText();
  Standard_EXPORT virtual ~OpenGl_AspectText();

  //! Copy parameters
  Standard_EXPORT void SetAspect (const CALL_DEF_CONTEXTTEXT& theAspect);

  //! @return font family name
  const TCollection_AsciiString& FontName() const
  {
    return myFont;
  }

  //! @return font family name
  TCollection_AsciiString& ChangeFontName()
  {
    return myFont;
  }

  //! @return is zoomable flag
  bool IsZoomable() const
  {
    return myZoomable;
  }

  //! @return rotation angle
  float Angle() const
  {
    return myAngle;
  }

  //! @return font aspect (regular/bold/italic)
  Font_FontAspect FontAspect() const
  {
    return myFontAspect;
  }

  //! @param theValue font aspect (regular/bold/italic)
  void SetFontAspect (const Font_FontAspect theValue)
  {
    myFontAspect = theValue;
  }

  //! @return text color
  const TEL_COLOUR& Color() const
  {
    return myColor;
  }

  //! @return text color
  TEL_COLOUR& ChangeColor()
  {
    return myColor;
  }

  //! @return annotation style
  Aspect_TypeOfStyleText StyleType() const
  {
    return myStyleType;
  }

  //! @return subtitle style (none/blend/decale/subtitle)
  Aspect_TypeOfDisplayText DisplayType() const
  {
    return myDisplayType;
  }

  void SetDisplayType (const Aspect_TypeOfDisplayText theType)
  {
    myDisplayType = theType;
  }

  //! @return subtitle color
  const TEL_COLOUR& SubtitleColor() const
  {
    return mySubtitleColor;
  }

  //! @return subtitle color
  TEL_COLOUR& ChangeSubtitleColor()
  {
    return mySubtitleColor;
  }

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

  TCollection_AsciiString         myFont;
  TEL_COLOUR                      myColor;
  TEL_COLOUR                      mySubtitleColor;
  float                           myAngle;
  Aspect_TypeOfStyleText          myStyleType;
  Aspect_TypeOfDisplayText        myDisplayType;
  Font_FontAspect                 myFontAspect;
  bool                            myZoomable;
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

#endif // OpenGl_AspectText_Header
