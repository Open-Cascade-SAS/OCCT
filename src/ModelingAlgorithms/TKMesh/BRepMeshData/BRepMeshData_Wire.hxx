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

#ifndef _BRepMeshData_Wire_HeaderFile
#define _BRepMeshData_Wire_HeaderFile

#include <IMeshData_Wire.hxx>
#include <IMeshData_Types.hxx>

//! Default implementation of wire data model entity.
class BRepMeshData_Wire : public IMeshData_Wire
{
public:
  DEFINE_INC_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMeshData_Wire(const TopoDS_Wire&                           theWire,
                                    const int                                    theEdgeNb,
                                    const occ::handle<NCollection_IncAllocator>& theAllocator);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_Wire();

  //! Gets number of children.
  Standard_EXPORT virtual int EdgesNb() const override;

  //! Adds new discrete edge with specified orientation to wire chain.
  //! @return index of added edge in wire chain.
  Standard_EXPORT virtual int AddEdge(const IMeshData::IEdgePtr& theDEdge,
                                      const TopAbs_Orientation   theOrientation) override;

  //! Gets edge with the given index.
  Standard_EXPORT virtual const IMeshData::IEdgePtr& GetEdge(const int theIndex) const override;

  //! Returns True if orientation of discrete edge with the given index is forward.
  Standard_EXPORT virtual TopAbs_Orientation GetEdgeOrientation(const int theIndex) const override;

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_Wire, IMeshData_Wire)

private:
  IMeshData::VectorOfIEdgePtrs   myDEdges;
  IMeshData::VectorOfOrientation myDEdgesOri;
};

#endif