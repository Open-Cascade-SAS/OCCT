// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _V3d_Plane_H__
#define _V3d_Plane_H__

#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_Structure.hxx>
#include <V3d_View.hxx>

DEFINE_STANDARD_HANDLE (V3d_Plane, MMgt_TShared)

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
class V3d_Plane : public MMgt_TShared
{
public:

  //! Creates a clipping plane from plane coefficients.
  Standard_EXPORT V3d_Plane (const Quantity_Parameter theA = 0.0,
                             const Quantity_Parameter theB = 0.0,
                             const Quantity_Parameter theC = 1.0,
                             const Quantity_Parameter theD = 0.0);

  //! Change plane equation.
  Standard_EXPORT void SetPlane (const Quantity_Parameter theA,
                                 const Quantity_Parameter theB,
                                 const Quantity_Parameter theC,
                                 const Quantity_Parameter theD);

  //! Display the plane representation in the choosen view.
  Standard_EXPORT virtual void Display (const Handle(V3d_View)& theView,
                                        const Quantity_Color& theColor = Quantity_NOC_GRAY);

  //! Erase the plane representation.
  Standard_EXPORT void Erase();

  //! Returns the parameters of the plane.
  Standard_EXPORT void Plane (Quantity_Parameter& theA,
                              Quantity_Parameter& theB,
                              Quantity_Parameter& theC,
                              Quantity_Parameter& theD) const;

  //! Returns TRUE when the plane representation is displayed <br>
  Standard_EXPORT Standard_Boolean IsDisplayed() const;

  //! Use this method to pass clipping plane implementation for
  //! standard clipping workflow.
  //! @return clipping plane implementation handle.
  Standard_EXPORT const Handle(Graphic3d_ClipPlane)& ClipPlane() const
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

  DEFINE_STANDARD_RTTI(V3d_Plane)
};

#endif
