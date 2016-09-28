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

#ifndef _Graphic3d_HighlightStyle_HeaderFile
#define _Graphic3d_HighlightStyle_HeaderFile

#include <Aspect_TypeOfHighlightMethod.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Graphic3d_Vec4.hxx>
#include <Quantity_ColorRGBA.hxx>

//! A class for setting up highlight properties, such as:
//! - highlight method (box or some of object's presentations);
//! - highlight color;
//! - transparency coefficient.
class Graphic3d_HighlightStyle : public Standard_Transient
{
public:

  //! Creates the style. Default parameters are:
  //! - method: color;
  //! - color: white;
  //! - transparency: 0.f.
  Graphic3d_HighlightStyle (const Aspect_TypeOfHighlightMethod theMethod = Aspect_TOHM_COLOR,
                            const Quantity_Color&              theColor = Quantity_NOC_WHITE,
                            const Standard_ShortReal           theTransparency = 0.f)
    : myMethod (theMethod)
  {
    myColor.ChangeRGB() = theColor;
    myColor.SetAlpha (1.f - theTransparency);
    myColorFltPtr = new Graphic3d_Vec4 (myColor.GetRGB(), myColor.Alpha());
  };

  //! Creates a copy of the given style
  Graphic3d_HighlightStyle (const Handle(Graphic3d_HighlightStyle)& theOther)
    : myMethod      (theOther->myMethod),
      myColor       (theOther->myColor)
  {
    myColorFltPtr = new Graphic3d_Vec4 (myColor.GetRGB(), myColor.Alpha());
  };

  //! Deletes color pointer stored for consistency with TKOpenGl
  virtual ~Graphic3d_HighlightStyle()
  {
    delete myColorFltPtr;
    myColorFltPtr = NULL;
  }

  //! Changes current highlight method to the given one
  void SetMethod (const Aspect_TypeOfHighlightMethod theMethod)
  {
    myMethod = theMethod;
  }

  //! Returns current highlight method
  Aspect_TypeOfHighlightMethod Method() const
  {
    return myMethod;
  }

  //! Changes highlight color to the given one
  void SetColor (const Quantity_Color& theColor)
  {
    myColor.ChangeRGB() = theColor;
    myColorFltPtr->xyz() = theColor;
  }

  //! Returns current highlight color
  const Quantity_Color& Color() const
  {
    return myColor.GetRGB();
  }

  //! Changes transparency of a highlight presentation to the given one
  void SetTransparency (const Standard_ShortReal theTranspCoef)
  {
    myColor.SetAlpha (1.f - theTranspCoef);
    myColorFltPtr->a() = myColor.Alpha();
  }

  //! Returns current transparency of a highlight presentation
  Standard_ShortReal Transparency() const
  {
    return 1.f - myColor.Alpha();
  }

  //! Returns pointer to current highlight color in RGBA format
  const Graphic3d_Vec4* ColorFltPtr() const
  {
    return myColorFltPtr;
  }

  DEFINE_STANDARD_RTTIEXT (Graphic3d_HighlightStyle, Standard_Transient)

private:

  Aspect_TypeOfHighlightMethod myMethod;      //!< Box or color highlighting
  Quantity_ColorRGBA           myColor;       //!< 3-component highlight color with opacity
  Graphic3d_Vec4*              myColorFltPtr; //!< For consistency with TKOpenGl
};

DEFINE_STANDARD_HANDLE (Graphic3d_HighlightStyle, Standard_Transient)

#endif // _Graphic3d_HighlightStyle_HeaderFile
