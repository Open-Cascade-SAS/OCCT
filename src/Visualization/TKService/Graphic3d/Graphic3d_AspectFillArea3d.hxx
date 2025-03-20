// Created on: 1991-11-04
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_AspectFillArea3d_HeaderFile
#define _Graphic3d_AspectFillArea3d_HeaderFile

#include <Graphic3d_Aspects.hxx>

//! This class defines graphic attributes for opaque 3d primitives (polygons, triangles,
//! quadrilaterals).
class Graphic3d_AspectFillArea3d : public Graphic3d_Aspects
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectFillArea3d, Graphic3d_Aspects)
public:
  //! Creates a context table for fill area primitives defined with the following default values:
  //!
  //! InteriorStyle : Aspect_IS_EMPTY
  //! InteriorColor : Quantity_NOC_CYAN1
  //! EdgeColor     : Quantity_NOC_WHITE
  //! EdgeLineType  : Aspect_TOL_SOLID
  //! EdgeWidth     : 1.0
  //! FrontMaterial : NOM_BRASS
  //! BackMaterial  : NOM_BRASS
  //! HatchStyle    : Aspect_HS_SOLID
  //!
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  Standard_EXPORT Graphic3d_AspectFillArea3d();

  //! Creates a context table for fill area primitives defined with the specified values.
  //! Display of back-facing filled polygons.
  //! No distinction between external and internal faces of FillAreas.
  //! The edges are not drawn.
  //! Polygon offset parameters: mode = Aspect_POM_None, factor = 1., units = 0.
  Standard_EXPORT Graphic3d_AspectFillArea3d(const Aspect_InteriorStyle      theInterior,
                                             const Quantity_Color&           theInteriorColor,
                                             const Quantity_Color&           theEdgeColor,
                                             const Aspect_TypeOfLine         theEdgeLineType,
                                             const Standard_Real             theEdgeWidth,
                                             const Graphic3d_MaterialAspect& theFrontMaterial,
                                             const Graphic3d_MaterialAspect& theBackMaterial);

public:
  Standard_DEPRECATED("Deprecated method, ToDrawEdges() should be used instead")

  bool Edge() const { return ToDrawEdges(); }
};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectFillArea3d, Graphic3d_Aspects)

#endif // _Graphic3d_AspectFillArea3d_HeaderFile
