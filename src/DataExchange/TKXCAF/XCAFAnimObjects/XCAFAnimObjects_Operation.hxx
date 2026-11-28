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

#ifndef _XCAFAnimObjects_Operation_HeaderFile
#define _XCAFAnimObjects_Operation_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <TCollection_AsciiString.hxx>
#include <XCAFAnimObjects_OperationType.hxx>

//! Abstract base class for animation operations.
//! This class defines a common interface for all types of animation operations
//! such as translation, rotation, scaling, etc. Each operation contains transformation
//! data and optional time stamps that define when the operation should be applied.
//! Different subclasses implement specific transformation types.
class XCAFAnimObjects_Operation : public Standard_Transient
{
public:
  //! Constructor for an operation without time stamps.
  //! @param theIsInverse Flag indicating if the operation should be applied in reverse
  Standard_EXPORT XCAFAnimObjects_Operation(const bool theIsInverse = false);

  //! Constructor for an operation with time stamps.
  //! @param theTimeStamps Array of time values when the operation should be applied
  //! @param theIsInverse Flag indicating if the operation should be applied in reverse
  Standard_EXPORT XCAFAnimObjects_Operation(const NCollection_Array1<double>& theTimeStamps,
                                            const bool                        theIsInverse = false);

  //! Copy constructor.
  //! @param theOperation Source operation to copy from
  Standard_EXPORT XCAFAnimObjects_Operation(const Handle(XCAFAnimObjects_Operation)& theOperation);

  //! Returns the time stamps array for this operation.
  //! Time stamps define the moments when keyframes are applied.
  //! @return Array of time values
  const NCollection_Array1<double>& TimeStamps() const { return myTimeStamps; }

  //! Checks if this operation has time stamps defined.
  //! @return true if the operation has time stamps, false otherwise
  bool HasTimeStamps() const { return !myTimeStamps.IsEmpty(); }

  //! Returns the type identifier for this operation.
  //! Must be implemented by derived classes to specify their type.
  //! @return Operation type enum value
  Standard_EXPORT virtual XCAFAnimObjects_OperationType GetType() const = 0;

  //! Returns the type name of this operation as a string.
  //! This can be used for display purposes or custom type identification.
  //! @return String representation of the operation type
  Standard_EXPORT virtual TCollection_AsciiString GetTypeName() const = 0;

  //! Checks if this operation should be applied in reverse.
  //! @return true if the operation should be inverted, false otherwise
  bool IsInverse() const { return myIsInverse; }

  //! Sets whether this operation should be applied in reverse.
  //! @param theIsInverse true to apply the operation in reverse, false otherwise
  void SetInverse(const bool theIsInverse) { myIsInverse = theIsInverse; }

  //! Returns the general representation of this operation as a 2D array.
  //! Each row represents a keyframe, with columns containing the transformation values.
  //! Must be implemented by derived classes to provide their specific data format.
  //! @return 2D array of transformation values
  Standard_EXPORT virtual NCollection_Array2<double> GeneralPresentation() const = 0;

  DEFINE_STANDARD_RTTIEXT(XCAFAnimObjects_Operation, Standard_Transient)

private:
  //! Flag indicating if operation should be applied in reverse
  bool myIsInverse;
  //! Array of time values when keyframes should be applied
  NCollection_Array1<double> myTimeStamps;
};

#endif // _XCAFAnimObjects_Operation_HeaderFile
