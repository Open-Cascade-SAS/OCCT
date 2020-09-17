// Created on: 2019-02-25
// Created by: Artem NOVIKOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Prs3d_ToolSector_HeaderFile
#define _Prs3d_ToolSector_HeaderFile

#include <Prs3d_ToolQuadric.hxx>

//! Standard presentation algorithm that outputs graphical primitives for disk surface.
class Prs3d_ToolSector : public Prs3d_ToolQuadric
{
public:

  //! Generate primitives for 3D quadric surface.
  //! @param theRadius   [in] sector radius
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  //! @param theTrsf     [in] optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theRadius,
                                                                    const Standard_Integer theNbSlices,
                                                                    const Standard_Integer theNbStacks,
                                                                    const gp_Trsf&         theTrsf);
public:

  //! Initializes the algorithm creating a sector (quadrant).
  //! @param theRadius   [in] sector radius
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Standard_EXPORT Prs3d_ToolSector (const Standard_Real    theRadius,
                                    const Standard_Integer theNbSlices,
                                    const Standard_Integer theNbStacks);
protected:

  //! Computes vertex at given parameter location of the surface.
  Standard_EXPORT virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) const Standard_OVERRIDE;

  //! Computes normal at given parameter location of the surface.
  virtual gp_Dir Normal (const Standard_Real , const Standard_Real ) const Standard_OVERRIDE
  {
    return gp_Dir (0.0, 0.0, -1.0);
  }

protected:

  Standard_Real myRadius; //!< sector radius

};

#endif
