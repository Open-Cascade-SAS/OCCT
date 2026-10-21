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

#ifndef _XCAFAnimObjects_Rotate_HeaderFile
#define _XCAFAnimObjects_Rotate_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <gp_Quaternion.hxx>

//! Animation operation that represents rotation transformations.
//! This operation can represent various types of rotations including:
//! - Single axis rotations (X, Y, Z)
//! - Two-axis rotations (XY, XZ, YX, etc.)
//! - Three-axis rotations with different order of application (XYZ, ZYX, etc.)
//! Rotation values are stored in radians.
class XCAFAnimObjects_Rotate : public XCAFAnimObjects_Operation
{
public:
  //! Enumeration defining rotation types with different axis combinations and application orders.
  //! The order of axes in the name indicates the order of rotation application.
  enum XCAFAnimObjects_Rotate_Type
  {
    XCAFAnimObjects_Rotate_Type_XYZ = 0, //!< Rotation around X, then Y, then Z axes (Euler angles)
    XCAFAnimObjects_Rotate_Type_XZY,     //!< Rotation around X, then Z, then Y axes
    XCAFAnimObjects_Rotate_Type_YZX,     //!< Rotation around Y, then Z, then X axes
    XCAFAnimObjects_Rotate_Type_YXZ,     //!< Rotation around Y, then X, then Z axes
    XCAFAnimObjects_Rotate_Type_ZXY,     //!< Rotation around Z, then X, then Y axes
    XCAFAnimObjects_Rotate_Type_ZYX,     //!< Rotation around Z, then Y, then X axes
    XCAFAnimObjects_Rotate_Type_XY,      //!< Rotation around X, then Y axes only
    XCAFAnimObjects_Rotate_Type_XZ,      //!< Rotation around X, then Z axes only
    XCAFAnimObjects_Rotate_Type_YX,      //!< Rotation around Y, then X axes only
    XCAFAnimObjects_Rotate_Type_YZ,      //!< Rotation around Y, then Z axes only
    XCAFAnimObjects_Rotate_Type_ZX,      //!< Rotation around Z, then X axes only
    XCAFAnimObjects_Rotate_Type_ZY,      //!< Rotation around Z, then Y axes only
    XCAFAnimObjects_Rotate_Type_X,       //!< Rotation around X axis only
    XCAFAnimObjects_Rotate_Type_Y,       //!< Rotation around Y axis only
    XCAFAnimObjects_Rotate_Type_Z        //!< Rotation around Z axis only
  };

public:
  //! Constructor for a rotation operation with multiple keyframes.
  //! @param theRotate Array of rotation values (1, 2, or 3 values per keyframe depending on type)
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  //! @param theRotateType Rotation type specifying which axes and in what order to rotate around
  Standard_EXPORT XCAFAnimObjects_Rotate(
    const NCollection_Array1<double>& theRotate,
    const NCollection_Array1<double>& theTimeStamps,
    const XCAFAnimObjects_Rotate_Type theRotateType = XCAFAnimObjects_Rotate_Type_XYZ);

  //! Constructor that creates rotation from a general presentation format.
  //! @param theGeneralPresentation 2D array containing rotation values for each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  //! @param theRotateType Rotation type specifying which axes and in what order to rotate around
  Standard_EXPORT XCAFAnimObjects_Rotate(
    const NCollection_Array2<double>& theGeneralPresentation,
    const NCollection_Array1<double>& theTimeStamps,
    const XCAFAnimObjects_Rotate_Type theRotateType = XCAFAnimObjects_Rotate_Type_XYZ);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Rotate(const Handle(XCAFAnimObjects_Rotate)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Rotate
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Rotate;
  }

  //! Returns the rotation type which specifies how the rotation is applied.
  //! @return Rotation type enum value
  XCAFAnimObjects_Rotate_Type GetRotateType() const { return myRotateType; }

  //! Sets the rotation type which specifies how the rotation is applied.
  //! @param theRotateType New rotation type to set
  void SetRotateType(const XCAFAnimObjects_Rotate_Type theRotateType)
  {
    myRotateType = theRotateType;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Rotate" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Rotate"; }

  //! Returns a 2D array containing the general presentation of this operation.
  //! Each row represents a keyframe, with columns containing rotation values.
  //! @return 2D array of rotation values
  NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE { return myRotatePresentation; }

  //! Returns the raw rotation array containing rotation data.
  //! @return 2D array with rotation values at each keyframe
  const NCollection_Array2<double>& RotatePresentation() const { return myRotatePresentation; }

private:
  XCAFAnimObjects_Rotate_Type myRotateType;         //!< Type of rotation specifying axes and order
  NCollection_Array2<double>  myRotatePresentation; //!< 2D array containing rotation values
};

#endif // _XCAFAnimObjects_Rotate_HeaderFile
