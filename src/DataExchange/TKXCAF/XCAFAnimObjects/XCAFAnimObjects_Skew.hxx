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

#ifndef _XCAFAnimObjects_Skew_HeaderFile
#define _XCAFAnimObjects_Skew_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <gp_XYZ.hxx>

//! Animation operation that represents skew/shear transformations.
//! Skewing distorts the shape along one or more axes, changing the angles between
//! faces while preserving parallel relationships. This operation can be used to
//! create non-rectangular parallelepipeds from rectangular ones, or other
//! shear-based deformations.
//! The skew values are represented as XYZ triplets defining the skew factors along each axis.
class XCAFAnimObjects_Skew : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for a skew operation with a single skew factor.
  //! @param theSkew XYZ values defining skew factors along each axis
  Standard_EXPORT XCAFAnimObjects_Skew(const gp_XYZ& theSkew);

  //! Constructor for a skew operation with multiple keyframes.
  //! @param theSkew Array of XYZ values defining skew factors for each keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Skew(const NCollection_Array1<gp_XYZ>& theSkew,
                                       const NCollection_Array1<double>& theTimeStamps);

  //! Constructor that creates skew from a general presentation format.
  //! @param theGeneralPresentation 2D array where each row has 3 values for skewing
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  Standard_EXPORT XCAFAnimObjects_Skew(const NCollection_Array2<double>& theGeneralPresentation,
                                       const NCollection_Array1<double>& theTimeStamps);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Skew(const Handle(XCAFAnimObjects_Skew)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Skew
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Skew;
  }

  //! Returns the type name of this operation as a string.
  //! @return "Skew" string
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return "Skew"; }

  //! Converts the XYZ representation to a general 2D array format.
  //! Each row contains 3 values for the skew factors.
  //! @return 2D array containing skew values
  Standard_EXPORT NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE;

  //! Returns the raw XYZ array containing skew data.
  //! @return Array of XYZ values defining skew factors at keyframes
  const NCollection_Array1<gp_XYZ>& SkewPresentation() const { return mySkewPresentation; }

private:
  NCollection_Array1<gp_XYZ> mySkewPresentation; //!< Array of XYZ skew factors for keyframes
};

#endif // _XCAFAnimObjects_Skew_HeaderFile
