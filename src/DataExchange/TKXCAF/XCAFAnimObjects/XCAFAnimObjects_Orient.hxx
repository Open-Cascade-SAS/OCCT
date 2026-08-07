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

#ifndef _XCAFAnimObjects_Orient_HeaderFile
#define _XCAFAnimObjects_Orient_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <gp_Quaternion.hxx>

//! Animation operation that represents orientation changes.
//! This operation uses quaternions to represent 3D rotations and orientations,
//! which provides a compact and efficient way to perform smooth orientation
//! interpolation (slerp) between keyframes. Quaternions avoid gimbal lock issues
//! that can occur with Euler angle representations.
class XCAFAnimObjects_Orient : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for an orientation operation with a single quaternion.
  //! @param theOrient Quaternion defining the orientation
  Standard_EXPORT XCAFAnimObjects_Orient(const gp_Quaternion& theOrient);

  //! Constructor for an orientation operation with multiple keyframes.
  //! @param theOrient Array of quaternions defining orientations at each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Orient(const NCollection_Array1<gp_Quaternion>& theOrient,
                                         const NCollection_Array1<double>&        theTimeStamps);

  //! Constructor that creates orientation from a general presentation format.
  //! @param theGeneralPresentation 2D array where each row has 4 values (x,y,z,w) for a quaternion
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Orient(const NCollection_Array2<double>& theGeneralPresentation,
                                         const NCollection_Array1<double>& theTimeStamps);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Orient(const Handle(XCAFAnimObjects_Orient)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Orient
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Orient;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Orient" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Orient"; }

  //! Converts the quaternion representation to a general 2D array format.
  //! Each row contains 4 values (x,y,z,w) for a quaternion.
  //! @return 2D array containing quaternion components
  Standard_EXPORT NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE;

  //! Returns the raw quaternion array containing orientation data.
  //! @return Array of quaternions defining orientations at keyframes
  const NCollection_Array1<gp_Quaternion>& OrientPresentation() const
  {
    return myOrientPresentation;
  }

private:
  //! Array of quaternions for keyframe orientations
  NCollection_Array1<gp_Quaternion> myOrientPresentation;
};

#endif // _XCAFAnimObjects_Orient_HeaderFile
