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

#ifndef _BRepMeshData_Edge_HeaderFile
#define _BRepMeshData_Edge_HeaderFile

#include <IMeshData_Edge.hxx>
#include <NCollection_IncAllocator.hxx>
#include <IMeshData_Types.hxx>

//! Default implementation of edge data model entity.
class BRepMeshData_Edge : public IMeshData_Edge
{
public:
  DEFINE_INC_ALLOC

  //! Constructor.
  Standard_EXPORT BRepMeshData_Edge(const TopoDS_Edge&                      theEdge,
                                    const Handle(NCollection_IncAllocator)& theAllocator);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMeshData_Edge();

  //! Returns number of pcurves assigned to current edge.
  Standard_EXPORT virtual Standard_Integer PCurvesNb() const Standard_OVERRIDE;

  //! Adds discrete pcurve for the specified discrete face.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& AddPCurve(
    const IMeshData::IFacePtr& theDFace,
    const TopAbs_Orientation   theOrientation) Standard_OVERRIDE;

  //! Returns pcurve for the specified discrete face.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& GetPCurve(
    const IMeshData::IFacePtr& theDFace,
    const TopAbs_Orientation   theOrientation) const Standard_OVERRIDE;

  //! Returns pcurve with the given index.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& GetPCurve(
    const Standard_Integer theIndex) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMeshData_Edge, IMeshData_Edge)

private:
  Handle(NCollection_IncAllocator)        myAllocator;
  IMeshData::VectorOfIPCurveHandles       myPCurves;
  IMeshData::DMapOfIFacePtrsListOfInteger myPCurvesMap;
};

#endif