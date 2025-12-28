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
  //! @param[in] theInnerRadius  inner disc radius
  //! @param[in] theOuterRadius  outer disc radius
  //! @param[in] theNbSlices     number of slices within U parameter
  //! @param[in] theNbStacks     number of stacks within V parameter
  //! @param[in] theTrsf         optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT static occ::handle<Graphic3d_ArrayOfTriangles> Create(const double theInnerRadius,
                                                                        const double theOuterRadius,
                                                                        const int    theNbSlices,
                                                                        const int    theNbStacks,
                                                                        const gp_Trsf& theTrsf);

public:
  //! Initializes the algorithm creating a disk.
  //! @param[in] theInnerRadius  inner disk radius
  //! @param[in] theOuterRadius  outer disk radius
  //! @param[in] theNbSlices     number of slices within U parameter
  //! @param[in] theNbStacks     number of stacks within V parameter
  Standard_EXPORT Prs3d_ToolDisk(const double theInnerRadius,
                                 const double theOuterRadius,
                                 const int    theNbSlices,
                                 const int    theNbStacks);

  //! Set angle range in radians [0, 2*PI] by default.
  //! @param[in] theStartAngle  Start angle in counter clockwise order
  //! @param[in] theEndAngle    End   angle in counter clockwise order
  void SetAngleRange(double theStartAngle, double theEndAngle)
  {
    myStartAngle = theStartAngle;
    myEndAngle   = theEndAngle;
  }

protected:
  //! Computes vertex at given parameter location of the surface.
  Standard_EXPORT virtual gp_Pnt Vertex(const double theU, const double theV) const override;

  //! Computes normal at given parameter location of the surface.
  virtual gp_Dir Normal(const double, const double) const override { return gp_Dir(gp_Dir::D::NZ); }

protected:
  double myInnerRadius; //!< Inner disk radius
  double myOuterRadius; //!< Outer disk radius
  double myStartAngle;  //!< Start angle in counter clockwise order
  double myEndAngle;    //!< End   angle in counter clockwise order
};

#endif
