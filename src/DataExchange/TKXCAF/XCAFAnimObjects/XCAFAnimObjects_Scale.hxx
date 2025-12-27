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

#ifndef _XCAFAnimObjects_Scale_HeaderFile
#define _XCAFAnimObjects_Scale_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <gp_XYZ.hxx>

//! Animation operation that represents scaling transformations.
//! This operation can handle both uniform and non-uniform scaling.
//! Scale values are represented as XYZ triplets, where each component
//! specifies the scaling factor along the corresponding axis.
//! A value of 1.0 represents no scale change, while values greater than 1.0
//! increase the size, and values between 0 and 1.0 decrease the size.
class XCAFAnimObjects_Scale : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for a scale operation with a single scaling factor.
  //! @param theScale XYZ values defining scale factors along each axis
  Standard_EXPORT XCAFAnimObjects_Scale(const gp_XYZ& theScale);

  //! Constructor for a scale operation with multiple keyframes.
  //! @param theScale Array of XYZ values defining scale factors for each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Scale(const NCollection_Array1<gp_XYZ>& theScale,
                                        const NCollection_Array1<double>& theTimeStamps);

  //! Constructor that creates scale from a general presentation format.
  //! @param theGeneralPresentation 2D array where each row has 3 values (X,Y,Z) for scaling
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Scale(const NCollection_Array2<double>& theGeneralPresentation,
                                        const NCollection_Array1<double>& theTimeStamps);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Scale(const Handle(XCAFAnimObjects_Scale)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Scale
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Scale;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Scale" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Scale"; }

  //! Converts the XYZ representation to a general 2D array format.
  //! Each row contains 3 values (X,Y,Z) for scaling factors.
  //! @return 2D array containing scale values
  Standard_EXPORT NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE;

  //! Returns the raw XYZ array containing scale data.
  //! @return Array of XYZ values defining scale factors at keyframes
  const NCollection_Array1<gp_XYZ>& ScalePresentation() const { return myScalePresentation; }

private:
  NCollection_Array1<gp_XYZ> myScalePresentation; //!< Array of XYZ scale factors for keyframes
};

#endif // _XCAFAnimObjects_Scale_HeaderFile
