// Created on: 2020-09-17
// Created by: Marina ZERNOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Prs3d_ToolTorus_HeaderFile
#define _Prs3d_ToolTorus_HeaderFile

#include <Prs3d_ToolQuadric.hxx>

//! Standard presentation algorithm that outputs graphical primitives for torus surface.
class Prs3d_ToolTorus : public Prs3d_ToolQuadric
{
public:

  //! Generate primitives for 3D quadric surface (complete torus).
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  //! @param theTrsf     [in] optional transformation to apply
  //! @return generated triangulation
  static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theMajorRad,
                                                    const Standard_Real    theMinorRad,
                                                    const Standard_Integer theNbSlices,
                                                    const Standard_Integer theNbStacks,
                                                    const gp_Trsf&         theTrsf)
  {
    return Create (theMajorRad, theMinorRad, 0.0, M_PI * 2.0, M_PI * 2.0, theNbSlices, theNbStacks, theTrsf);
  }

  //! Generate primitives for 3D quadric surface (torus segment).
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle    [in] angle to create a torus pipe segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  //! @param theTrsf     [in] optional transformation to apply
  //! @return generated triangulation
  static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theMajorRad,
                                                    const Standard_Real    theMinorRad,
                                                    const Standard_Real    theAngle,
                                                    const Standard_Integer theNbSlices,
                                                    const Standard_Integer theNbStacks,
                                                    const gp_Trsf&         theTrsf)
  {
    return Create (theMajorRad, theMinorRad, 0.0, M_PI * 2.0, theAngle, theNbSlices, theNbStacks, theTrsf);
  }

  //! Generate primitives for 3D quadric surface (torus ring segment).
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle1   [in] first  angle to create a torus ring segment
  //! @param theAngle2   [in] second angle to create a torus ring segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  //! @param theTrsf     [in] optional transformation to apply
  //! @return generated triangulation
  static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theMajorRad,
                                                    const Standard_Real    theMinorRad,
                                                    const Standard_Real    theAngle1,
                                                    const Standard_Real    theAngle2,
                                                    const Standard_Integer theNbSlices,
                                                    const Standard_Integer theNbStacks,
                                                    const gp_Trsf&         theTrsf)
  {
    return Create (theMajorRad, theMinorRad, theAngle1, theAngle2, M_PI * 2.0, theNbSlices, theNbStacks, theTrsf);
  }

  //! Generate primitives for 3D quadric surface (segment of the torus ring segment).
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle1   [in] first  angle to create a torus ring segment
  //! @param theAngle2   [in] second angle to create a torus ring segment
  //! @param theAngle    [in] angle to create a torus pipe segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  //! @param theTrsf     [in] optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT static Handle(Graphic3d_ArrayOfTriangles) Create (const Standard_Real    theMajorRad,
                                                                    const Standard_Real    theMinorRad,
                                                                    const Standard_Real    theAngle1,
                                                                    const Standard_Real    theAngle2,
                                                                    const Standard_Real    theAngle,
                                                                    const Standard_Integer theNbSlices,
                                                                    const Standard_Integer theNbStacks,
                                                                    const gp_Trsf&         theTrsf);

public:

  //! Initializes the algorithm creating a complete torus.
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Prs3d_ToolTorus (const Standard_Real    theMajorRad,
                   const Standard_Real    theMinorRad,
                   const Standard_Integer theNbSlices,
                   const Standard_Integer theNbStacks)
  {
    init (theMajorRad, theMinorRad, 0.0, M_PI * 2.0, M_PI * 2.0, theNbSlices, theNbStacks);
  }

  //! Initializes the algorithm creating a torus pipe segment.
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle    [in] angle to create a torus pipe segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Prs3d_ToolTorus (const Standard_Real    theMajorRad,
                   const Standard_Real    theMinorRad,
                   const Standard_Real    theAngle,
                   const Standard_Integer theNbSlices,
                   const Standard_Integer theNbStacks)
  {
    init (theMajorRad, theMinorRad, 0.0, M_PI * 2.0, theAngle, theNbSlices, theNbStacks);
  }

  //! Initializes the algorithm creating a torus ring segment.
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle1   [in] first  angle to create a torus ring segment
  //! @param theAngle2   [in] second angle to create a torus ring segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Prs3d_ToolTorus (const Standard_Real    theMajorRad,
                   const Standard_Real    theMinorRad,
                   const Standard_Real    theAngle1,
                   const Standard_Real    theAngle2,
                   const Standard_Integer theNbSlices,
                   const Standard_Integer theNbStacks)
  {
    init (theMajorRad, theMinorRad, theAngle1, theAngle2, M_PI * 2.0, theNbSlices, theNbStacks);
  }

  //! Initializes the algorithm creating a torus ring segment.
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle1   [in] first  angle to create a torus ring segment
  //! @param theAngle2   [in] second angle to create a torus ring segment
  //! @param theAngle    [in] angle to create a torus pipe segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Prs3d_ToolTorus (const Standard_Real    theMajorRad,
                   const Standard_Real    theMinorRad,
                   const Standard_Real    theAngle1,
                   const Standard_Real    theAngle2,
                   const Standard_Real    theAngle,
                   const Standard_Integer theNbSlices,
                   const Standard_Integer theNbStacks)
  {
    init (theMajorRad, theMinorRad, theAngle1, theAngle2, theAngle, theNbSlices, theNbStacks);
  }

private:

  //! Initialisation
  //! @param theMajorRad [in] distance from the center of the pipe to the center of the torus
  //! @param theMinorRad [in] radius of the pipe
  //! @param theAngle1   [in] first  angle to create a torus ring segment
  //! @param theAngle2   [in] second angle to create a torus ring segment
  //! @param theAngle    [in] angle to create a torus pipe segment
  //! @param theNbSlices [in] number of slices within U parameter
  //! @param theNbStacks [in] number of stacks within V parameter
  Standard_EXPORT void init (const Standard_Real    theMajorRad,
                             const Standard_Real    theMinorRad,
                             const Standard_Real    theAngle1,
                             const Standard_Real    theAngle2,
                             const Standard_Real    theAngle,
                             const Standard_Integer theNbSlices,
                             const Standard_Integer theNbStacks);

protected:

  //! Computes vertex at given parameter location of the surface.
  Standard_EXPORT virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) const Standard_OVERRIDE;

  //! Computes normal at given parameter location of the surface.
  Standard_EXPORT virtual gp_Dir Normal (const Standard_Real theU, const Standard_Real theV) const Standard_OVERRIDE;

protected:

  Standard_Real myMajorRadius; //!< distance from the center of the pipe to the center of the torus
  Standard_Real myMinorRadius; //!< radius of the pipe
  Standard_Real myAngle;       //!< angle to create a torus pipe segment
  Standard_Real myVMin;        //!< first angle to create a torus ring segment
  Standard_Real myVMax;        //!< second angle to create a torus ring segment

};

#endif // _Prs3d_ToolTorus_HeaderFile
