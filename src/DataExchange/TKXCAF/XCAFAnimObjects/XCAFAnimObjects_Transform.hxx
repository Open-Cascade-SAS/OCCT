// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _XCAFAnimObjects_Transform_HeaderFile
#define _XCAFAnimObjects_Transform_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <NCollection_Mat4.hxx>

//! Animation operation that represents general transformation matrices.
//! This operation uses 4x4 matrices to represent arbitrary transformations
//! including complex combinations of rotation, translation, scaling, and projection.
//! The transformation matrix allows for the most flexibility in defining shape
//! transformations, as it can represent any affine transformation as well as
//! some non-affine transformations.
class XCAFAnimObjects_Transform : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for a transform operation with a single transformation matrix.
  //! @param theTransform 4x4 matrix defining the transformation
  Standard_EXPORT XCAFAnimObjects_Transform(const NCollection_Mat4<double>& theTransform);

  //! Constructor for a transform operation with multiple keyframes.
  //! @param theTransform Array of 4x4 matrices defining transformations for each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Transform(
    const NCollection_Array1<NCollection_Mat4<double>>& theTransform,
    const NCollection_Array1<double>&                   theTimeStamps);

  //! Constructor that creates transform from a general presentation format.
  //! @param theGeneralPresentation 2D array where each row has 16 values representing a 4x4 matrix
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Transform(
    const NCollection_Array2<double>& theGeneralPresentation,
    const NCollection_Array1<double>& theTimeStamps);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Transform(const Handle(XCAFAnimObjects_Transform)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Transform
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Transform;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Transform" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Transform"; }

  //! Converts the matrix representation to a general 2D array format.
  //! Each row contains 16 values representing a 4x4 matrix in row-major order.
  //! @return 2D array containing matrix values
  Standard_EXPORT NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE;

  //! Returns the raw matrix array containing transformation data.
  //! @return Array of 4x4 matrices defining transformations at each keyframe
  const NCollection_Array1<NCollection_Mat4<double>>& TransformPresentation() const
  {
    return myTransformPresentation;
  }

private:
  //! Array of 4x4 transformation matrices for keyframes
  NCollection_Array1<NCollection_Mat4<double>> myTransformPresentation;
};

#endif // _XCAFAnimObjects_Transform_HeaderFile
