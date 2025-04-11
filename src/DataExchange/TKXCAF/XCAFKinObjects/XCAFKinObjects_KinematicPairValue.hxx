// Created on: 2020-03-30
// Created by: Irina KRYLOVA
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

#ifndef _XCAFKinObjects_KinematicPairValue_HeaderFile
#define _XCAFKinObjects_KinematicPairValue_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class XCAFKinObjects_PairValueObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFKinObjects_KinematicPairValue;
DEFINE_STANDARD_HANDLE(XCAFKinObjects_KinematicPairValue, TDF_Attribute)

//! Attribute to store kinematic pair value within XCAF document structure.
//!
//! A kinematic pair value represents a specific state or configuration of a
//! kinematic pair. It characterizes how a joint is positioned or oriented for a
//! particular mechanism state. For example, in a revolute joint, the value might
//! represent a specific rotation angle.
//!
//! This attribute is associated with a joint through a reference (tree node) and
//! is typically organized under state labels. It is important that the value type
//! is consistent with the referenced joint's type.
//!
//! The attribute itself does not directly contain data but serves as a way to
//! store/collect necessary standard attributes from the label. The attribute
//! is not stored directly in the OCAF document but is identified by a UAttribute
//! with a special GUID.
//!
//! Use the static IsValidLabel() method to check if a label contains this attribute
//! instead of using the common Label.FindAttribute() method.
class XCAFKinObjects_KinematicPairValue : public TDF_Attribute
{

public:
  //! Default constructor.
  //! Creates an empty kinematic pair value attribute.
  Standard_EXPORT XCAFKinObjects_KinematicPairValue();

  //! Returns the GUID for kinematic pair value attributes.
  //! @return Standard_GUID identifying kinematic pair value attributes
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Checks if the given label has a UAttribute with the necessary GUID.
  //! This method determines if a label represents a kinematic pair value.
  //! @param theLabel Label to check
  //! @return true if the label represents a kinematic pair value, false otherwise
  Standard_EXPORT static Standard_Boolean IsValidLabel(const TDF_Label& theLabel);

  //! Creates (if does not exist) or retrieves a kinematic pair value attribute on the
  //! specified label and associates it with the specified joint.
  //! Also establishes a reference from the value to the joint using tree nodes.
  //! @param theValue Label where to create or find the kinematic pair value attribute
  //! @param theJoint Label of the joint to associate with the value
  //! @return Handle to the kinematic pair value attribute
  Standard_EXPORT static Handle(XCAFKinObjects_KinematicPairValue) Set(const TDF_Label& theValue,
                                                                       const TDF_Label& theJoint);

  //! Creates (if does not exist) or retrieves a kinematic pair value attribute on the
  //! specified label without associating it with any joint.
  //! @param theValue Label where to create or find the kinematic pair value attribute
  //! @return Handle to the kinematic pair value attribute
  Standard_EXPORT static Handle(XCAFKinObjects_KinematicPairValue) Set(const TDF_Label& theValue);

  //! Returns the GUID identifying this attribute.
  //! @return Standard_GUID of this attribute
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Restores the attribute from another attribute.
  //! This implementation is empty as the attribute does not directly store data.
  //! @param theWith Attribute to restore from
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theWith) Standard_OVERRIDE;

  //! Creates a new empty attribute.
  //! @return Handle to the new attribute
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Pastes the attribute into another attribute.
  //! This implementation is empty as the attribute does not directly store data.
  //! @param theInfo Target attribute to paste into
  //! @param theRT Relocation table
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theInfo,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  //! Updates the parent's label with data taken from thePairValueObject.
  //! Stores the type and values from the object into attributes on the label.
  //! The object must contain valid values (non-empty array).
  //! @param thePairValueObject Kinematic pair value object containing the data to be stored
  Standard_EXPORT void SetObject(const Handle(XCAFKinObjects_PairValueObject)& thePairValueObject);

  //! Returns a kinematic pair value object reconstructed from the data stored in the
  //! parent's label.
  //! Creates a new object and populates it with the type and values from the label's attributes.
  //! @return Handle to the reconstructed kinematic pair value object
  Standard_EXPORT Handle(XCAFKinObjects_PairValueObject) GetObject() const;

  //! Dumps the content of this attribute into the JSON stream.
  //! @param theOStream Output stream where JSON data will be written
  //! @param theDepth Recursion depth for JSON structure (-1 for maximum depth)
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  //! Returns the special GUID of UAttribute that indicates a label contains
  //! XCAFKinObjects_KinematicPairValue attribute.
  //! @return Standard_GUID for identifying kinematic pair value labels
  Standard_EXPORT static const Standard_GUID& getUID();

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_KinematicPairValue, TDF_Attribute)
};

#endif
