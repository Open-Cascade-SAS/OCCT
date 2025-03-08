// Created on: 2015-01-16
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef OpenGl_BackgroundArray_Header
#define OpenGl_BackgroundArray_Header

#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_FillMethod.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_TypeOfBackground.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Vec.hxx>

//! Tool class for generating reusable data for
//! gradient or texture background rendering.
class OpenGl_BackgroundArray : public OpenGl_PrimitiveArray
{
public:
  //! Main constructor.
  Standard_EXPORT OpenGl_BackgroundArray(const Graphic3d_TypeOfBackground theType);

  //! Render primitives to the window
  Standard_EXPORT void Render(const Handle(OpenGl_Workspace)& theWorkspace,
                              Graphic3d_Camera::Projection    theProjection) const;

  //! Check if background parameters are set properly
  Standard_EXPORT bool IsDefined() const;

  //! Sets background texture parameters
  Standard_EXPORT void SetTextureParameters(const Aspect_FillMethod theFillMethod);

  //! Sets texture fill method
  Standard_EXPORT void SetTextureFillMethod(const Aspect_FillMethod theFillMethod);

  //! Gets background texture fill method
  Aspect_FillMethod TextureFillMethod() const { return myFillMethod; }

  //! Gets background gradient fill method
  Aspect_GradientFillMethod GradientFillMethod() const { return myGradientParams.type; }

  //! Returns color of gradient background for the given index.
  const OpenGl_Vec4& GradientColor(const Standard_Integer theIndex) const
  {
    return (&myGradientParams.color1)[theIndex];
  }

  //! Sets type of gradient fill method
  Standard_EXPORT void SetGradientFillMethod(const Aspect_GradientFillMethod theType);

  //! Sets background gradient parameters
  Standard_EXPORT void SetGradientParameters(const Quantity_Color&           theColor1,
                                             const Quantity_Color&           theColor2,
                                             const Aspect_GradientFillMethod theType);

protected: //! @name Internal structure for storing gradient parameters
  struct OpenGl_GradientParameters
  {
    OpenGl_Vec4               color1;
    OpenGl_Vec4               color2;
    Aspect_GradientFillMethod type;
  };

protected:
  //! Fill attributes arrays for background array according to its type:
  //! - for gradient background its attributes consist of colors and gradient coordinates
  //! - for texture one its attributes consist of position and texture coordinates.
  Standard_EXPORT Standard_Boolean init(const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Initializes gradient arrays.
  Standard_EXPORT Standard_Boolean createGradientArray(const Handle(OpenGl_Context)& theCtx) const;

  //! Initializes texture arrays.
  //! @param theWorkspace OpenGl workspace that stores texture in the current enabled face aspect.
  Standard_EXPORT Standard_Boolean
    createTextureArray(const Handle(OpenGl_Workspace)& theWorkspace) const;

  //! Initializes cubemap arrays.
  Standard_EXPORT Standard_Boolean createCubeMapArray() const;

  //! Marks array parameters as changed,
  //! on next rendering stage array data is to be updated.
  Standard_EXPORT void invalidateData();

  using OpenGl_PrimitiveArray::Render;

protected:
  // clang-format off
  Graphic3d_TypeOfBackground        myType;           //!< Type of background: texture or gradient.
  Aspect_FillMethod                 myFillMethod;     //!< Texture parameters
  mutable OpenGl_GradientParameters myGradientParams; //!< Gradient parameters
  mutable Standard_Integer          myViewWidth;      //!< view width  used for array initialization
  mutable Standard_Integer          myViewHeight;     //!< view height used for array initialization
  mutable Standard_Boolean          myToUpdate;       //!< Shows if array parameters were changed and data (myAttribs storage) is to be updated
  // clang-format on
};

#endif // OpenGl_BackgroundArray_Header
