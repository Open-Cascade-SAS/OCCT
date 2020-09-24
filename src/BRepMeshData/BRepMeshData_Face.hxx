// Created on: 2016-04-07
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

#ifndef _BRepMeshData_Face_HeaderFile
#define _BRepMeshData_Face_HeaderFile

#include <IMeshData_Types.hxx>
#include <IMeshData_Face.hxx>
#include <IMeshData_Wire.hxx>

//! Default implementation of face data model entity.
class BRepMeshData_Face : public IMeshData_Face
{
public:

  DEFINE_INC_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMeshData_Face (
    const TopoDS_Face&                       theFace,
    const Handle (NCollection_IncAllocator)& theAllocator);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_Face ();

  //! Gets number of children.
  Standard_EXPORT virtual Standard_Integer WiresNb () const Standard_OVERRIDE;

  //! Gets wire with the given index.
  Standard_EXPORT virtual const IMeshData::IWireHandle& GetWire (
    const Standard_Integer theIndex) const Standard_OVERRIDE;

  //! Adds wire to discrete model of face.
  Standard_EXPORT virtual const IMeshData::IWireHandle& AddWire (
    const TopoDS_Wire&     theWire,
    const Standard_Integer theEdgeNb = 0) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_Face, IMeshData_Face)

private:

  Handle (NCollection_IncAllocator) myAllocator;
  IMeshData::VectorOfIWireHandles   myDWires;
};

#endif