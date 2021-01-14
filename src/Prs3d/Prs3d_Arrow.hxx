// Created on: 1993-04-15
// Created by: Jean-Louis Frenkel
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Prs3d_Arrow_HeaderFile
#define _Prs3d_Arrow_HeaderFile

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Presentation.hxx>

class gp_Ax1;
class gp_Pnt;
class gp_Dir;

//! Provides class methods to draw an arrow at a given location, along a given direction and using a given angle.
class Prs3d_Arrow
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines the representation of the arrow as shaded triangulation.
  //! @param theAxis       axis definition (arrow origin and direction)
  //! @param theTubeRadius tube (cylinder) radius
  //! @param theAxisLength overall arrow length (cylinder + cone)
  //! @param theConeRadius cone radius (arrow tip)
  //! @param theConeLength cone length (arrow tip)
  //! @param theNbFacettes tessellation quality for each part
  Standard_EXPORT static Handle(Graphic3d_ArrayOfTriangles) DrawShaded (const gp_Ax1&          theAxis,
                                                                        const Standard_Real    theTubeRadius,
                                                                        const Standard_Real    theAxisLength,
                                                                        const Standard_Real    theConeRadius,
                                                                        const Standard_Real    theConeLength,
                                                                        const Standard_Integer theNbFacettes);

  //! Defines the representation of the arrow as a container of segments.
  //! @param theLocation   location of the arrow tip
  //! @param theDir        direction of the arrow
  //! @param theAngle      angle of opening of the arrow head
  //! @param theLength     length of the arrow (from the tip)
  //! @param theNbSegments count of points on polyline where location is connected
  Standard_EXPORT static Handle(Graphic3d_ArrayOfSegments) DrawSegments (const gp_Pnt& theLocation,
                                                                         const gp_Dir& theDir,
                                                                         const Standard_Real theAngle,
                                                                         const Standard_Real theLength,
                                                                         const Standard_Integer theNbSegments);

  //! Defines the representation of the arrow.
  //! Note that this method does NOT assign any presentation aspects to the primitives group!
  //! @param theGroup     presentation group to add primitives
  //! @param theLocation  location of the arrow tip
  //! @param theDirection direction of the arrow
  //! @param theAngle     angle of opening of the arrow head
  //! @param theLength    length of the arrow (from the tip)
  Standard_EXPORT static void Draw (const Handle(Graphic3d_Group)& theGroup,
                                    const gp_Pnt& theLocation,
                                    const gp_Dir& theDirection,
                                    const Standard_Real theAngle,
                                    const Standard_Real theLength);

};

#endif // _Prs3d_Arrow_HeaderFile
