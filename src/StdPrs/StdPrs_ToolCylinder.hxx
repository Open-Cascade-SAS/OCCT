// Created on: 2016-02-04
// Created by: Anastasia BORISOVA
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

#ifndef _StdPrs_ToolCylinder_HeaderFile
#define _StdPrs_ToolCylinder_HeaderFile

#include <Standard.hxx>
#include <StdPrs_ToolQuadric.hxx>

//! Standard presentation algorithm that outputs graphical primitives for cylindrical surface.
class StdPrs_ToolCylinder : public StdPrs_ToolQuadric
{
public:

  //! Generate primitives for 3D quadric surface and return a filled array.
  Standard_EXPORT static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theBottomRad,
                                                                    const Standard_Real    theTopRad,
                                                                    const Standard_Real    theHeight,
                                                                    const Standard_Integer theNbSlices,
                                                                    const Standard_Integer theNbStacks,
                                                                    const gp_Trsf&         theTrsf);
public:

  DEFINE_STANDARD_ALLOC

  //! Initializes the algorithm.
  Standard_EXPORT StdPrs_ToolCylinder (const Standard_Real    theBottomRad,
                                       const Standard_Real    theTopRad,
                                       const Standard_Real    theHeight,
                                       const Standard_Integer theNbSlices,
                                       const Standard_Integer theNbStacks);

protected:

  //! Computes vertex at given parameter location of the surface.
  Standard_EXPORT virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) Standard_OVERRIDE;

  //! Computes normal at given parameter location of the surface.
  Standard_EXPORT virtual gp_Dir Normal (const Standard_Real theU, const Standard_Real theV) Standard_OVERRIDE;

protected:

  Standard_Real myBottomRadius;
  Standard_Real myTopRadius;
  Standard_Real myHeight;

};

#endif // _StdPrs_ToolCylinder_HeaderFile
