// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _V3d_Plane_H__
#define _V3d_Plane_H__

#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_Structure.hxx>
#include <V3d_View.hxx>

class V3d_Plane;
DEFINE_STANDARD_HANDLE (V3d_Plane, Standard_Transient)

//! Obsolete clip plane presentation class.
//! Ported on new core of Graphic3d_ClipPlane approach.
//! Please access Graphic3d_ClipPlane via ClipPlane() method
//! to use it for standard clipping workflow.
//! Example of use:
//! @code
//!
//! Handle(V3d_Plane) aPlane (0, 1, 0, -20);
//! Handle(V3d_View) aView;
//! aView->AddClipPlane (aPlane->ClipPlane());
//!
//! aPlane->Display (aView);
//! aPlane->SetPlane (0, 1, 0, -30);
//! aView->RemoveClipPlane (aPlane->ClipPlane());
//!
//! @endcode
//! Use interface of this class to modify plane equation synchronously
//! with clipping equation.
class V3d_Plane : public Standard_Transient
{
public:

  //! Creates a clipping plane from plane coefficients.
  Standard_EXPORT V3d_Plane (const Standard_Real theA = 0.0,
                             const Standard_Real theB = 0.0,
                             const Standard_Real theC = 1.0,
                             const Standard_Real theD = 0.0);

  //! Change plane equation.
  Standard_EXPORT void SetPlane (const Standard_Real theA,
                                 const Standard_Real theB,
                                 const Standard_Real theC,
                                 const Standard_Real theD);

  //! Display the plane representation in the choosen view.
  Standard_EXPORT virtual void Display (const Handle(V3d_View)& theView,
                                        const Quantity_Color& theColor = Quantity_NOC_GRAY);

  //! Erase the plane representation.
  Standard_EXPORT void Erase();

  //! Returns the parameters of the plane.
  Standard_EXPORT void Plane (Standard_Real& theA,
                              Standard_Real& theB,
                              Standard_Real& theC,
                              Standard_Real& theD) const;

  //! Returns TRUE when the plane representation is displayed.
  Standard_EXPORT Standard_Boolean IsDisplayed() const;

  //! Use this method to pass clipping plane implementation for
  //! standard clipping workflow.
  //! @return clipping plane implementation handle.
  const Handle(Graphic3d_ClipPlane)& ClipPlane() const
  {
    return myPlane;
  }

private:

  //! Updates the the plane representation.
  Standard_EXPORT void Update();

protected:

  Handle(Graphic3d_Structure) myGraphicStructure;

private:

  Handle(Graphic3d_ClipPlane) myPlane; //!< clip plane implementation.

public:

  DEFINE_STANDARD_RTTIEXT(V3d_Plane,Standard_Transient)
};

#endif
