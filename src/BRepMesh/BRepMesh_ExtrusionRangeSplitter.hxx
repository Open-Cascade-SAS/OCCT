// Created on: 2022-09-07
// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _BRepMesh_ExtrusionRangeSplitter_HeaderFile
#define _BRepMesh_ExtrusionRangeSplitter_HeaderFile

#include <BRepMesh_NURBSRangeSplitter.hxx>

//! Auxiliary class analysing extrusion surface in order to generate internal nodes.
class BRepMesh_ExtrusionRangeSplitter : public BRepMesh_NURBSRangeSplitter
{
public:

  //! Constructor.
  BRepMesh_ExtrusionRangeSplitter()
  {
  }

  //! Destructor.
  virtual ~BRepMesh_ExtrusionRangeSplitter()
  {
  }

protected:

  //! Returns number of intervals computed using available geometrical parameters.
  Standard_EXPORT virtual Standard_Integer getUndefinedIntervalNb(
    const Handle(Adaptor3d_Surface)& theSurface,
    const Standard_Boolean           isU,
    const GeomAbs_Shape              theContinuity) const Standard_OVERRIDE;
};

#endif
