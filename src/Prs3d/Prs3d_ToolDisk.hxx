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

#ifndef _Prs3d_ToolDisk_HeaderFile
#define _Prs3d_ToolDisk_HeaderFile

#include <Prs3d_ToolQuadric.hxx>

//! Standard presentation algorithm that outputs graphical primitives for disk surface.
class Prs3d_ToolDisk : public Prs3d_ToolQuadric
{
public:

  //! Generate primitives for 3D quadric surface.
  //! @param theInnerRadius [in] inner disc radius
  //! @param theOuterRadius [in] outer disc radius
  //! @param theNbSlices    [in] number of slices within U parameter
  //! @param theNbStacks    [in] number of stacks within V parameter
  //! @param theTrsf        [in] optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theInnerRadius,
                                                                    const Standard_Real    theOuterRadius,
                                                                    const Standard_Integer theNbSlices,
                                                                    const Standard_Integer theNbStacks,
                                                                    const gp_Trsf&         theTrsf);
public:

  //! Initializes the algorithm creating a disk.
  //! @param theInnerRadius [in] inner disk radius
  //! @param theOuterRadius [in] outer disk radius
  //! @param theNbSlices    [in] number of slices within U parameter
  //! @param theNbStacks    [in] number of stacks within V parameter
  Standard_EXPORT Prs3d_ToolDisk (const Standard_Real    theInnerRadius,
                                  const Standard_Real    theOuterRadius,
                                  const Standard_Integer theNbSlices,
                                  const Standard_Integer theNbStacks);

  //! Set angle range in radians [0, 2*PI] by default.
  //! @param theStartAngle [in] Start angle in counter clockwise order
  //! @param theEndAngle   [in] End   angle in counter clockwise order
  void SetAngleRange (Standard_Real theStartAngle,
                      Standard_Real theEndAngle)
  {
    myStartAngle = theStartAngle;
    myEndAngle   = theEndAngle;
  }

protected:

  //! Computes vertex at given parameter location of the surface.
  Standard_EXPORT virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) const Standard_OVERRIDE;

  //! Computes normal at given parameter location of the surface.
  virtual gp_Dir Normal (const Standard_Real , const Standard_Real ) const Standard_OVERRIDE
  {
    return gp_Dir (0.0, 0.0, -1.0);
  }

protected:

  Standard_Real myInnerRadius; //!< Inner disk radius
  Standard_Real myOuterRadius; //!< Outer disk radius
  Standard_Real myStartAngle;  //!< Start angle in counter clockwise order
  Standard_Real myEndAngle;    //!< End   angle in counter clockwise order

};

#endif
