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

#ifndef _XCAFAnimObjects_CustomOperation_HeaderFile
#define _XCAFAnimObjects_CustomOperation_HeaderFile

#include <XCAFAnimObjects_Operation.hxx>
#include <NCollection_Array2.hxx>
#include <TCollection_AsciiString.hxx>

//! Custom animation operation that allows defining arbitrary transformations.
//! Unlike predefined operations like rotation or translation, custom operations
//! can represent any kind of transformation by directly providing transformation data
//! as arrays of values. This class can be used to implement specialized operations
//! that don't fit into standard transformation categories.
class XCAFAnimObjects_CustomOperation : public XCAFAnimObjects_Operation
{
public:
  //! Constructor for a custom operation with a single keyframe.
  //! @param thePresentation Array of values representing the transformation data
  //! @param theCustomTypeName Name identifying the type of custom operation
  Standard_EXPORT XCAFAnimObjects_CustomOperation(const NCollection_Array1<double>& thePresentation,
                                                  const TCollection_AsciiString& theCustomTypeName);

  //! Constructor for a custom operation with multiple keyframes.
  //! @param thePresentation 2D array of transformation values, where each row corresponds to a
  //! keyframe
  //! @param theTimeStamps Array of time values corresponding to each keyframe
  //! @param theCustomTypeName Name identifying the type of custom operation
  Standard_EXPORT XCAFAnimObjects_CustomOperation(const NCollection_Array2<double>& thePresentation,
                                                  const NCollection_Array1<double>& theTimeStamps,
                                                  const TCollection_AsciiString& theCustomTypeName);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_CustomOperation(
    const Handle(XCAFAnimObjects_CustomOperation)& theOperation);

  //! Returns the type identifier for this operation.
  //! @return XCAFAnimObjects_OperationType_Custom
  XCAFAnimObjects_OperationType GetType() const Standard_OVERRIDE
  {
    return XCAFAnimObjects_OperationType_Custom;
  }

  //! Returns a 2D array containing the general presentation of this operation.
  //! Each row represents a keyframe, with columns containing the transformation values.
  //! @return 2D array of transformation values
  NCollection_Array2<double> GeneralPresentation() const Standard_OVERRIDE
  {
    return myPresentation;
  }

  //! Returns the custom type name of this operation.
  //! This name can be used to identify specific subtypes of custom operations.
  //! @return String identifier for the custom operation type
  TCollection_AsciiString GetTypeName() const Standard_OVERRIDE { return myTypeName; }

  //! Returns the raw presentation data for the custom operation.
  //! This allows direct access to the transformation values.
  //! @return 2D array containing the transformation data
  const NCollection_Array2<double>& CustomPresentation() const { return myPresentation; }

private:
  TCollection_AsciiString    myTypeName;     //!< Custom operation type identifier
  NCollection_Array2<double> myPresentation; //!< 2D array containing transformation data
};

#endif // _XCAFAnimObjects_CustomOperation_HeaderFile
