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

#ifndef V3d_Plane_HeaderFile
#define V3d_Plane_HeaderFile

#include <V3d_View.hxx>

//! Obsolete clip plane presentation class.
//! Ported on new core of Graphic3d_ClipPlane approach.
//! Please access Graphic3d_ClipPlane via ClipPlane() method
//! to use it for standard clipping workflow.
//! Example of use:
//! @code
//!
//! occ::handle<V3d_Plane> aPlane (0, 1, 0, -20);
//! occ::handle<V3d_View> aView;
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
  Standard_EXPORT V3d_Plane(const double theA = 0.0,
                            const double theB = 0.0,
                            const double theC = 1.0,
                            const double theD = 0.0);

  //! Change plane equation.
  Standard_EXPORT void SetPlane(const double theA,
                                const double theB,
                                const double theC,
                                const double theD);

  //! Display the plane representation in the chosen view.
  Standard_EXPORT virtual void Display(const occ::handle<V3d_View>& theView,
                                       const Quantity_Color&        theColor = Quantity_NOC_GRAY);

  //! Erase the plane representation.
  Standard_EXPORT void Erase();

  //! Returns the parameters of the plane.
  Standard_EXPORT void Plane(double& theA, double& theB, double& theC, double& theD) const;

  //! Returns TRUE when the plane representation is displayed.
  Standard_EXPORT bool IsDisplayed() const;

  //! Use this method to pass clipping plane implementation for
  //! standard clipping workflow.
  //! @return clipping plane implementation handle.
  const occ::handle<Graphic3d_ClipPlane>& ClipPlane() const { return myPlane; }

private:
  //! Updates the plane representation.
  Standard_EXPORT void Update();

protected:
  occ::handle<Graphic3d_Structure> myGraphicStructure;

private:
  occ::handle<Graphic3d_ClipPlane> myPlane; //!< clip plane implementation.

public:
  DEFINE_STANDARD_RTTIEXT(V3d_Plane, Standard_Transient)
};

#endif
