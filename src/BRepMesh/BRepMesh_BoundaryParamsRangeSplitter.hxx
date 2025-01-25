// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_BoundaryParamsRangeSplitter_HeaderFile
#define _BRepMesh_BoundaryParamsRangeSplitter_HeaderFile

#include <BRepMesh_NURBSRangeSplitter.hxx>

//! Auxiliary class extending UV range splitter in order to generate
//! internal nodes for NURBS surface.
class BRepMesh_BoundaryParamsRangeSplitter : public BRepMesh_NURBSRangeSplitter
{
public:
  //! Constructor.
  BRepMesh_BoundaryParamsRangeSplitter() {}

  //! Destructor.
  virtual ~BRepMesh_BoundaryParamsRangeSplitter() {}

  //! Registers border point.
  virtual void AddPoint(const gp_Pnt2d& thePoint) Standard_OVERRIDE
  {
    BRepMesh_NURBSRangeSplitter::AddPoint(thePoint);
    GetParametersU().Add(thePoint.X());
    GetParametersV().Add(thePoint.Y());
  }

protected:
  //! Initializes U and V parameters lists using CN continuity intervals.
  virtual Standard_Boolean initParameters() const Standard_OVERRIDE { return Standard_True; }
};

#endif
