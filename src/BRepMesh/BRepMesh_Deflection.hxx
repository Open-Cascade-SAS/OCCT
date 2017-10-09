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

#include <Standard_Type.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <IMeshData_Types.hxx>

class Bnd_Box;
class TopoDS_Face;
class TopoDS_Edge;
struct IMeshTools_Parameters;

//! Auxiliary tool encompassing methods to compute deflection of shapes.
class BRepMesh_Deflection : public Standard_Transient
{
public:

  //! Returns relative deflection for edge with respect to shape size.
  //! @param theEdge edge for which relative deflection should be computed.
  //! @param theDeflection absolute deflection.
  //! @param theMaxShapeSize maximum size of a shape.
  //! @param theAdjustmentCoefficient coefficient of adjustment between maximum 
  //! size of shape and calculated relative deflection.
  //! @return relative deflection for the edge.
  Standard_EXPORT static Standard_Real RelativeEdgeDeflection (
    const TopoDS_Edge&  theEdge,
    const Standard_Real theDeflection,
    const Standard_Real theMaxShapeSize,
    Standard_Real&      theAdjustmentCoefficient);

  //! Computes and updates deflection of the given discrete edge.
  Standard_EXPORT static void ComputeDeflection (
    const IMeshData::IEdgeHandle& theDEdge,
    const Standard_Real           theMaxShapeSize,
    const IMeshTools_Parameters&  theParameters);

  //! Computes and updates deflection of the given discrete wire.
  Standard_EXPORT static void ComputeDeflection (
    const IMeshData::IWireHandle& theDWire,
    const IMeshTools_Parameters&  theParameters);

  //! Computes and updates deflection of the given discrete face.
  Standard_EXPORT static void ComputeDeflection (
    const IMeshData::IFaceHandle& theDFace,
    const IMeshTools_Parameters&  theParameters);

  DEFINE_STANDARD_RTTI_INLINE(BRepMesh_Deflection, Standard_Transient)
};

#endif