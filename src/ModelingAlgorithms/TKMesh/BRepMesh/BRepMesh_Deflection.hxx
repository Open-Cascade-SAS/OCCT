// Created on: 2016-04-19
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

#ifndef _BRepMesh_Deflection_HeaderFile
#define _BRepMesh_Deflection_HeaderFile

#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <IMeshData_Types.hxx>

struct IMeshTools_Parameters;

//! Auxiliary tool encompassing methods to compute deflection of shapes.
class BRepMesh_Deflection : public Standard_Transient
{
public:
  //! Returns absolute deflection for theShape with respect to the
  //! relative deflection and theMaxShapeSize.
  //! @param theShape shape for that the deflection should be computed.
  //! @param theRelativeDeflection relative deflection.
  //! @param theMaxShapeSize maximum size of the whole shape.
  //! @return absolute deflection for the shape.
  Standard_EXPORT static Standard_Real ComputeAbsoluteDeflection(
    const TopoDS_Shape& theShape,
    const Standard_Real theRelativeDeflection,
    const Standard_Real theMaxShapeSize);

  //! Computes and updates deflection of the given discrete edge.
  Standard_EXPORT static void ComputeDeflection(const IMeshData::IEdgeHandle& theDEdge,
                                                const Standard_Real           theMaxShapeSize,
                                                const IMeshTools_Parameters&  theParameters);

  //! Computes and updates deflection of the given discrete wire.
  Standard_EXPORT static void ComputeDeflection(const IMeshData::IWireHandle& theDWire,
                                                const IMeshTools_Parameters&  theParameters);

  //! Computes and updates deflection of the given discrete face.
  Standard_EXPORT static void ComputeDeflection(const IMeshData::IFaceHandle& theDFace,
                                                const IMeshTools_Parameters&  theParameters);

  //! Checks if the deflection of current polygonal representation
  //! is consistent with the required deflection.
  //! @param[in] theCurrent  Current deflection.
  //! @param[in] theRequired  Required deflection.
  //! @param[in] theAllowDecrease  Flag controlling the check. If decrease is allowed,
  //! to be consistent the current and required deflections should be approximately the same.
  //! If not allowed, the current deflection should be less than required.
  //! @param[in] theRatio  The ratio for comparison of the deflections (value from 0 to 1).
  Standard_EXPORT static Standard_Boolean IsConsistent(const Standard_Real    theCurrent,
                                                       const Standard_Real    theRequired,
                                                       const Standard_Boolean theAllowDecrease,
                                                       const Standard_Real    theRatio = 0.1);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_Deflection, Standard_Transient)
};

#endif