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

#ifndef _IMeshData_Wire_HeaderFile
#define _IMeshData_Wire_HeaderFile

#include <IMeshData_TessellatedShape.hxx>
#include <IMeshData_StatusOwner.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <IMeshData_Types.hxx>

//! Interface class representing discrete model of a wire.
//! Wire should represent an ordered set of edges.
class IMeshData_Wire : public IMeshData_TessellatedShape, public IMeshData_StatusOwner
{
public:
  //! Destructor.
  virtual ~IMeshData_Wire() {}

  //! Returns TopoDS_Face attached to model.
  const TopoDS_Wire& GetWire() const { return TopoDS::Wire(GetShape()); }

  //! Returns number of edges.
  Standard_EXPORT virtual Standard_Integer EdgesNb() const = 0;

  //! Adds new discrete edge with specified orientation to wire chain.
  //! @return index of added edge in wire chain.
  Standard_EXPORT virtual Standard_Integer AddEdge(const IMeshData::IEdgePtr& theDEdge,
                                                   const TopAbs_Orientation   theOrientation) = 0;

  //! Returns discrete edge with the given index.
  Standard_EXPORT virtual const IMeshData::IEdgePtr& GetEdge(
    const Standard_Integer theIndex) const = 0;

  //! Returns True if orientation of discrete edge with the given index is forward.
  Standard_EXPORT virtual TopAbs_Orientation GetEdgeOrientation(
    const Standard_Integer theIndex) const = 0;

  DEFINE_STANDARD_RTTIEXT(IMeshData_Wire, IMeshData_TessellatedShape)

protected:
  //! Constructor.
  //! Initializes empty model.
  IMeshData_Wire(const TopoDS_Wire& theWire)
      : IMeshData_TessellatedShape(theWire)
  {
  }
};

#endif