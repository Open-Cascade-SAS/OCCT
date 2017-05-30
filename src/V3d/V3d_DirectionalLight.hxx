// Created on: 1992-01-22
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _V3d_DirectionalLight_HeaderFile
#define _V3d_DirectionalLight_HeaderFile

#include <V3d_PositionLight.hxx>
#include <V3d_TypeOfOrientation.hxx>

class V3d_Viewer;
class V3d_DirectionalLight;
DEFINE_STANDARD_HANDLE(V3d_DirectionalLight, V3d_PositionLight)

//! Directional light source for a viewer.
class V3d_DirectionalLight : public V3d_PositionLight
{
public:

  //! Creates a directional light source in the viewer.
  Standard_EXPORT V3d_DirectionalLight (const Handle(V3d_Viewer)& theViewer,
                                        const V3d_TypeOfOrientation theDirection = V3d_XposYposZpos,
                                        const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                        const Standard_Boolean theIsHeadlight = Standard_False);

  //! Creates a directional light source in the viewer.
  //! theXt, theYt, theZt : Coordinate of light source Target.
  //! theXp, theYp, theZp : Coordinate of light source Position.
  //! The others parameters describe before.
  Standard_EXPORT V3d_DirectionalLight (const Handle(V3d_Viewer)& theViewer,
                                        const Standard_Real theXt,
                                        const Standard_Real theYt,
                                        const Standard_Real theZt,
                                        const Standard_Real theXp,
                                        const Standard_Real theYp,
                                        const Standard_Real theZp,
                                        const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                        const Standard_Boolean theIsHeadlight = Standard_False);

  //! Defines the direction of the light source by a predefined orientation.
  Standard_EXPORT void SetDirection (const V3d_TypeOfOrientation theDirection);

  //! Defines the direction of the light source by the predefined vector theXm, theYm, theZm.
  //! Warning: raises  BadValue from V3d if the vector is null.
  Standard_EXPORT void SetDirection (const Standard_Real theXm,
                                     const Standard_Real theYm,
                                     const Standard_Real theZm);

  //! Defines the point of light source representation.
  Standard_EXPORT void SetDisplayPosition (const Standard_Real theX,
                                           const Standard_Real theY,
                                           const Standard_Real theZ);

  //! Calls SetDisplayPosition method.
  Standard_EXPORT virtual void SetPosition (const Standard_Real theXp,
                                            const Standard_Real theYp,
                                            const Standard_Real theZp) Standard_OVERRIDE;

  //! Modifies the smoothing angle (in radians)
  Standard_EXPORT void SetSmoothAngle (const Standard_Real theValue);

  //! Display the graphic structure of light source
  //! in the chosen view. We have three type of representation
  //! - SIMPLE   : Only the light source is displayed.
  //! - PARTIAL  : The light source and the light space are
  //! displayed.
  //! - COMPLETE : The same representation as PARTIAL.
  //! We can choose the "SAMELAST" as parameter of representation
  //! In this case the graphic structure representation will be
  //! the last displayed.
  Standard_EXPORT void Display (const Handle(V3d_View)& theView,
                                const V3d_TypeOfRepresentation theRepresentation) Standard_OVERRIDE;

  //! Calls DisplayPosition method.
  Standard_EXPORT virtual void Position (Standard_Real& theX,
                                         Standard_Real& theY,
                                         Standard_Real& theZ) const Standard_OVERRIDE;

  //! Returns the chosen position to represent the light source.
  Standard_EXPORT void DisplayPosition (Standard_Real& theX,
                                        Standard_Real& theY,
                                        Standard_Real& theZ) const;

  //! Returns the theVx, theVy, theVz direction of the light source.
  Standard_EXPORT void Direction (Standard_Real& theVx,
                                  Standard_Real& theVy,
                                  Standard_Real& theVz) const;

  DEFINE_STANDARD_RTTIEXT(V3d_DirectionalLight,V3d_PositionLight)

private:

  //! Defines the representation of the directional light source.
  Standard_EXPORT void Symbol (const Handle(Graphic3d_Group)& theSymbol,
                               const Handle(V3d_View)& theView) const Standard_OVERRIDE;

private:

  Graphic3d_Vertex myDisplayPosition;
};

#endif // _V3d_DirectionalLight_HeaderFile
