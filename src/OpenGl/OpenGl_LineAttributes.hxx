// Created on: 2011-09-20
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

#ifndef _OpenGl_LineAttributes_Header
#define _OpenGl_LineAttributes_Header

#include <OpenGl_Resource.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Font_FontAspect.hxx>
#include <Graphic3d_HatchStyle.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Handle(Graphic3d_HatchStyle), unsigned int> OpenGl_MapOfHatchStylesAndIds;

class OpenGl_Context;

//! Utility class to manage OpenGL state of polygon hatching rasterization
//! and keeping its cached state. The hatching rasterization is implemented
//! using glPolygonStipple function of OpenGL. State of hatching is controlled
//! by two parameters - type of hatching and IsEnabled parameter.
//! The hatching rasterization is enabled only if non-zero index pattern type
//! is selected (zero by default is reserved for solid filling) and if
//! IsEnabled flag is set to true. The IsEnabled parameter is useful for temporarily
//! turning on/off the hatching rasterization without making any costly GL calls
//! for changing the hatch pattern. This is a sharable resource class - it creates
//! OpenGL context objects for each hatch pattern to achieve quicker switching between
//! them, thesse GL objects are freed when the resource is released by owner context.
//! @note The implementation is not supported by Core Profile and by ES version.
class OpenGl_LineAttributes : public OpenGl_Resource
{
public:

  //! Default constructor.
  //! By default the parameters are:
  //! - IsEnabled (true),
  //! - TypeOfHatch (0).
  OpenGl_LineAttributes();

  //! Default destructor.
  virtual ~OpenGl_LineAttributes();

  //! Release GL resources.
  virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Index of currently selected type of hatch.
  int TypeOfHatch() const { return myTypeOfHatch; }

  //! Sets type of the hatch.
  int SetTypeOfHatch (const OpenGl_Context*               theGlCtx,
                      const Handle(Graphic3d_HatchStyle)& theStyle);

  //! Current enabled state of the hatching rasterization.
  bool IsEnabled() const { return myIsEnabled; }

  //! Turns on/off the hatching rasterization rasterization.
  bool SetEnabled (const OpenGl_Context* theGlCtx, const bool theToEnable);

protected:

  unsigned int init (const OpenGl_Context* theGlCtx,
                     const Handle(Graphic3d_HatchStyle)& theStyle);

protected:

  int myTypeOfHatch; //!< Currently activated type of hatch
  bool myIsEnabled; //!< Current enabled state of hatching rasterization.
  OpenGl_MapOfHatchStylesAndIds myStyles; //!< Hatch patterns

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_LineAttributes,OpenGl_Resource)
};

DEFINE_STANDARD_HANDLE(OpenGl_LineAttributes, OpenGl_Resource)

#endif // _OpenGl_LineAttributes_Header
