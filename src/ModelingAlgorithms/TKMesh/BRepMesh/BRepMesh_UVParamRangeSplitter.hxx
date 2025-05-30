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

#ifndef _BRepMesh_UVParamRangeSplitter_HeaderFile
#define _BRepMesh_UVParamRangeSplitter_HeaderFile

#include <BRepMesh_DefaultRangeSplitter.hxx>
#include <IMeshData_Types.hxx>

//! Intended to generate internal mesh nodes using UV parameters of boundary discrete points.
class BRepMesh_UVParamRangeSplitter : public BRepMesh_DefaultRangeSplitter
{
public:
  //! Constructor.
  BRepMesh_UVParamRangeSplitter()
      : myAllocator(new NCollection_IncAllocator(IMeshData::MEMORY_BLOCK_SIZE_HUGE)),
        myUParams(1, myAllocator),
        myVParams(1, myAllocator)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_UVParamRangeSplitter() {}

  //! Resets this splitter.
  virtual void Reset(const IMeshData::IFaceHandle& theDFace,
                     const IMeshTools_Parameters&  theParameters) Standard_OVERRIDE
  {
    BRepMesh_DefaultRangeSplitter::Reset(theDFace, theParameters);
    myUParams.Clear();
    myVParams.Clear();
    myAllocator->Reset(Standard_False);
  }

public:
  //! Returns U parameters.
  const IMeshData::IMapOfReal& GetParametersU() const { return myUParams; }

  //! Returns U parameters.
  IMeshData::IMapOfReal& GetParametersU() { return myUParams; }

  //! Returns V parameters.
  const IMeshData::IMapOfReal& GetParametersV() const { return myVParams; }

  //! Returns V parameters.
  IMeshData::IMapOfReal& GetParametersV() { return myVParams; }

private:
  Handle(NCollection_IncAllocator) myAllocator;
  IMeshData::IMapOfReal            myUParams;
  IMeshData::IMapOfReal            myVParams;
};

#endif
