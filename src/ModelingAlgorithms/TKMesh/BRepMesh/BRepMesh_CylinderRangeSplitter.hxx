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

#ifndef _BRepMesh_CylinderRangeSplitter_HeaderFile
#define _BRepMesh_CylinderRangeSplitter_HeaderFile

#include <BRepMesh_DefaultRangeSplitter.hxx>

//! Auxiliary class extending default range splitter in
//! order to generate internal nodes for cylindrical surface.
class BRepMesh_CylinderRangeSplitter : public BRepMesh_DefaultRangeSplitter
{
public:
  //! Constructor.
  BRepMesh_CylinderRangeSplitter()
      : myDu(1.)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_CylinderRangeSplitter() {}

  //! Resets this splitter. Must be called before first use.
  Standard_EXPORT virtual void Reset(const IMeshData::IFaceHandle& theDFace,
                                     const IMeshTools_Parameters&  theParameters) Standard_OVERRIDE;

  //! Returns list of nodes generated using surface data and specified parameters.
  Standard_EXPORT virtual Handle(IMeshData::ListOfPnt2d) GenerateSurfaceNodes(
    const IMeshTools_Parameters& theParameters) const Standard_OVERRIDE;

protected:
  //! Computes parametric delta taking length along U and V into account.
  Standard_EXPORT virtual void computeDelta(const Standard_Real theLengthU,
                                            const Standard_Real theLengthV) Standard_OVERRIDE;

private:
  Standard_Real myDu;
};

#endif
