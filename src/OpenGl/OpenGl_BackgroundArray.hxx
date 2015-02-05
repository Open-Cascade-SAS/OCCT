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
#include <Graphic3d_TypeOfBackground.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Workspace.hxx>

//! Tool class for generating reusable data for
//! gradient or texture background rendering.
class OpenGl_BackgroundArray : public OpenGl_PrimitiveArray
{
public:

  //! Main constructor.
  Standard_EXPORT OpenGl_BackgroundArray (const Graphic3d_TypeOfBackground theType);

  //! Check if background parameters are set properly
  Standard_EXPORT bool IsDefined() const;

  //! Fill attributes arrays for background array according to its type:
  //! - for gradient background its attributes consist of colors and gradient coordinates
  //! - for texture one its attributes consist of position and texure coordinates.
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Workspace)& theWorkspace);

  //! Sets background texture parameters
  Standard_EXPORT void SetTextureParameters (const Aspect_FillMethod theFillMethod);

  //! Sets texture fill method
  Standard_EXPORT void SetTextureFillMethod (const Aspect_FillMethod theFillMethod);

  //! Gets background texture fill method
  Aspect_FillMethod TextureFillMethod() const { return myFillMethod; }

  //! Gets background gradient fill method
  Aspect_GradientFillMethod GradientFillMethod() const { return myGradientParams.type; }

  //! Sets type of gradient fill method
  Standard_EXPORT void SetGradientFillMethod (const Aspect_GradientFillMethod theType);

  //! Sets background gradient parameters
  Standard_EXPORT void SetGradientParameters (const Quantity_Color&           theColor1,
                                              const Quantity_Color&           theColor2,
                                              const Aspect_GradientFillMethod theType);

  //! Shows if array parameters were changed
  Standard_Boolean IsDataChanged() const { return myToUpdate; }

  //! Shows if view sizes were changed
  //! (texture coordinates is no be changed)
  Standard_Boolean IsViewSizeChanged (const Handle(OpenGl_Workspace)& theWorkspace) const
  {
    return myType == Graphic3d_TOB_TEXTURE
       && (myViewWidth  != theWorkspace->Width()
        || myViewHeight != theWorkspace->Height());
  }

protected: //! @name Internal structure for storing gradient parameters

  struct OpenGl_GradientParameters
  {
    OpenGl_Vec4 color1;
    OpenGl_Vec4 color2;
    Aspect_GradientFillMethod type;
  };

protected:

  //! Initializes gradient arrays.
  Standard_EXPORT Standard_Boolean createGradientArray();

  //! Initializes texture arrays.
  //! @param theWorkspace OpenGl workspace that stores texture in the current enabled face aspect.
  Standard_EXPORT Standard_Boolean createTextureArray (const Handle(OpenGl_Workspace)& theWorkspace);

  //! Marks array parameters as changed,
  //! on next rendering stage array data is to be updated.
  Standard_EXPORT void invalidateData();

protected:

  Standard_Boolean           myToUpdate;       //!< Shows if array parameters were changed and data (myAttribs storage) is to be updated
  Graphic3d_TypeOfBackground myType;           //!< Type of background: texture or gradient.
  Standard_Integer           myViewWidth;      //!< view width  used for array initialization
  Standard_Integer           myViewHeight;     //!< view height used for array initialization
  Aspect_FillMethod          myFillMethod;     //!< Texture parameters
  OpenGl_GradientParameters  myGradientParams; //!< Gradient parameters

};

#endif // OpenGl_BackgroundArray_Header
