// Created on: 2016-08-24
// Created by: Varvara POSKONINA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Graphic3d_PresentationAttributes_HeaderFile
#define _Graphic3d_PresentationAttributes_HeaderFile

#include <Aspect_TypeOfHighlightMethod.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Class defines presentation properties.
class Graphic3d_PresentationAttributes : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_PresentationAttributes, Standard_Transient)
public:

  //! Empty constructor.
  Graphic3d_PresentationAttributes()
  : myBasicColor (Quantity_NOC_WHITE),
    myHiMethod (Aspect_TOHM_COLOR),
    myZLayer   (Graphic3d_ZLayerId_Default),
    myDispMode (0)
  {
    //
  }

  //! Destructor.
  virtual ~Graphic3d_PresentationAttributes() {}

  //! Returns highlight method, Aspect_TOHM_COLOR by default.
  Aspect_TypeOfHighlightMethod Method() const { return myHiMethod; }

  //! Changes highlight method to the given one.
  virtual void SetMethod (const Aspect_TypeOfHighlightMethod theMethod) { myHiMethod = theMethod; }

  //! Returns basic presentation color (including alpha channel).
  const Quantity_ColorRGBA& ColorRGBA() const { return myBasicColor; }

  //! Returns basic presentation color, Quantity_NOC_WHITE by default.
  const Quantity_Color& Color() const { return myBasicColor.GetRGB(); }

  //! Sets basic presentation color (RGB components, does not modifies transparency).
  virtual void SetColor (const Quantity_Color& theColor)
  {
    myBasicColor.ChangeRGB() = theColor;
  }

  //! Returns basic presentation transparency (0 - opaque, 1 - fully transparent), 0 by default (opaque).
  Standard_ShortReal Transparency() const { return 1.0f - myBasicColor.Alpha(); }

  //! Sets basic presentation transparency (0 - opaque, 1 - fully transparent).
  virtual void SetTransparency (const Standard_ShortReal theTranspCoef)
  {
    myBasicColor.SetAlpha (1.0f - theTranspCoef);
  }

  //! Returns presentation Zlayer, Graphic3d_ZLayerId_Default by default.
  //! Graphic3d_ZLayerId_UNKNOWN means undefined (a layer of main presentation to be used).
  Graphic3d_ZLayerId ZLayer() const { return myZLayer; }

  //! Sets presentation Zlayer.
  virtual void SetZLayer (const Graphic3d_ZLayerId theLayer) { myZLayer = theLayer; }

  //! Returns display mode, 0 by default.
  //! -1 means undefined (main display mode of presentation to be used).
  Standard_Integer DisplayMode() const { return myDispMode; }

  //! Sets display mode.
  virtual void SetDisplayMode (const Standard_Integer theMode) { myDispMode = theMode; }

  //! Return basic presentation fill area aspect, NULL by default.
  //! When set, might be used instead of Color() property.
  const Handle(Graphic3d_AspectFillArea3d)& BasicFillAreaAspect() const { return myBasicFillAreaAspect; }

  //! Sets basic presentation fill area aspect.
  virtual void SetBasicFillAreaAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect) { myBasicFillAreaAspect = theAspect; }

protected:

  Handle(Graphic3d_AspectFillArea3d) myBasicFillAreaAspect; //!< presentation fill area aspect
  Quantity_ColorRGBA                 myBasicColor;          //!< presentation color
  Aspect_TypeOfHighlightMethod       myHiMethod;            //!< box or color highlighting
  Graphic3d_ZLayerId                 myZLayer;              //!< Z-layer
  Standard_Integer                   myDispMode;            //!< display mode

};

DEFINE_STANDARD_HANDLE (Graphic3d_PresentationAttributes, Standard_Transient)

#endif // _Graphic3d_PresentationAttributes_HeaderFile
