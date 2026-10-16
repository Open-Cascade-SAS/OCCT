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

#ifndef _XCAFAnimObjects_Object_HeaderFile
#define _XCAFAnimObjects_Object_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <XCAFAnimObjects_InterpolationType.hxx>

class XCAFAnimObjects_Operation;

//! The main class representing an animation object in XCAF document.
//! An animation object contains an ordered list of operations that define
//! how a shape should transform over time, along with an interpolation type
//! that specifies how values between key frames should be calculated.
//! Operations can include translations, rotations, scales and other transformations.
class XCAFAnimObjects_Object : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(XCAFAnimObjects_Object, Standard_Transient)

public:
  //! Default constructor.
  //! Creates an empty animation object with the default interpolation type (Step).
  Standard_EXPORT XCAFAnimObjects_Object();

  //! Copy constructor.
  //! Creates a new animation object by copying the operations from another object.
  //! @param theObject Source animation object to copy from
  Standard_EXPORT XCAFAnimObjects_Object(const Handle(XCAFAnimObjects_Object)& theObject);

  //! Adds a new operation to the end of the operations list.
  //! Operations are applied sequentially in the order they are added.
  //! @param theOperation Operation to add to the animation
  void AppendNewOperation(const Handle(XCAFAnimObjects_Operation)& theOperation)
  {
    myOrderedOperations.Append(theOperation);
  }

  //! Returns the current interpolation type for this animation.
  //! The interpolation type determines how intermediate values between key frames are calculated.
  //! @return Current interpolation type
  XCAFAnimObjects_InterpolationType GetInterpolationType() const { return myInterpolationType; }

  //! Sets the interpolation type for this animation.
  //! @param theType Interpolation type to set (Linear, Step, CubicSpline, etc.)
  void SetInterpolationType(const XCAFAnimObjects_InterpolationType theType)
  {
    myInterpolationType = theType;
  }

  //! Returns a reference to the ordered list of operations.
  //! This is a const access that doesn't allow modification.
  //! @return Const reference to the list of operations
  const NCollection_List<Handle(XCAFAnimObjects_Operation)>& GetOrderedOperations() const
  {
    return myOrderedOperations;
  }

  //! Returns a modifiable reference to the ordered list of operations.
  //! This allows adding, removing, or reordering operations.
  //! @return Non-const reference to the list of operations
  NCollection_List<Handle(XCAFAnimObjects_Operation)>& ChangeOrderedOperations()
  {
    return myOrderedOperations;
  }

private:
  //! Type of interpolation for this animation
  XCAFAnimObjects_InterpolationType myInterpolationType;
  //! List of operations in sequence order
  NCollection_List<Handle(XCAFAnimObjects_Operation)> myOrderedOperations;
};

#endif // _XCAFAnimObjects_Object_HeaderFile
