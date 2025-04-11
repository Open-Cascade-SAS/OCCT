// Created on: 2020-03-17
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

#ifndef _XCAFKinObjects_KinematicPair_HeaderFile
#define _XCAFKinObjects_KinematicPair_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class XCAFKinObjects_PairObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFKinObjects_KinematicPair;
DEFINE_STANDARD_HANDLE(XCAFKinObjects_KinematicPair, TDF_Attribute)

//! Attribute to store kinematic pair data within XCAF document structure.
//!
//! A kinematic pair represents a constraint between two links in a kinematic mechanism,
//! defining how they can move relative to each other. Examples include revolute joints,
//! prismatic joints, spherical joints, etc.
//!
//! The attribute itself does not contain data directly, but serves as a way to
//! store/collect necessary standard attributes from the label and its children. The attribute
//! is not stored directly in the OCAF document but is identified by a UAttribute with a
//! special GUID.
//!
//! Use the static IsValidLabel() method to check if a label contains this attribute
//! instead of using the common Label.FindAttribute() method.
//!
//! The attribute manages child labels with the following structure:
//! - Child label 1: First transformation data
//! - Child label 2: Second transformation data
//! - Child label 3: First geometric parameters
//! - Child label 4: Second geometric parameters
class XCAFKinObjects_KinematicPair : public TDF_Attribute
{

public:
  //! Default constructor.
  //! Creates an empty kinematic pair attribute.
  Standard_EXPORT XCAFKinObjects_KinematicPair();

  //! Returns the GUID for kinematic pair attributes.
  //! @return Standard_GUID identifying kinematic pair attributes
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Checks if the given label has a UAttribute with the necessary GUID.
  //! This method determines if a label represents a kinematic pair.
  //! @param theLabel Label to check
  //! @return true if the label represents a kinematic pair, false otherwise
  Standard_EXPORT static Standard_Boolean IsValidLabel(const TDF_Label& theLabel);

  //! Creates (if does not exist) or retrieves a kinematic pair attribute on the specified label.
  //! Also ensures the label has the required UAttribute with special GUID.
  //! @param theLabel Label where kinematic pair attribute should be created or found
  //! @return Handle to the kinematic pair attribute
  Standard_EXPORT static Handle(XCAFKinObjects_KinematicPair) Set(const TDF_Label& theLabel);

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

  //! Updates parent's label and its sub-labels with data taken from thePairObject.
  //! Old data associated with the label will be lost. This method stores all
  //! necessary information like type, name, transformations, limits, and geometry.
  //!
  //! Different types of pair objects are handled according to their specific requirements:
  //! - Low order pairs (simple joints like revolute, prismatic)
  //! - Low order pairs with motion coupling (like screw)
  //! - High order pairs (more complex joints involving curves and surfaces)
  //!
  //! @param thePairObject Kinematic pair object containing the data to be stored
  Standard_EXPORT void SetObject(const Handle(XCAFKinObjects_PairObject)& thePairObject);

  //! Returns a kinematic pair object reconstructed from the data stored in the
  //! parent's label and its sub-labels.
  //!
  //! The method creates an appropriate type of object (low order, with coupling, or high order)
  //! based on the stored pair type, and populates it with all necessary data including name,
  //! transformations, limits, parameters, and geometry.
  //!
  //! @return Handle to the reconstructed kinematic pair object
  Standard_EXPORT Handle(XCAFKinObjects_PairObject) GetObject() const;

  //! Dumps the content of this attribute into the JSON stream.
  //! @param theOStream Output stream where JSON data will be written
  //! @param theDepth Recursion depth for JSON structure (-1 for maximum depth)
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  //! Returns the GUID for limits attributes used by kinematic pairs.
  //! @return Standard_GUID for limits attributes
  Standard_EXPORT static const Standard_GUID& getLimitsID();

  //! Returns the GUID for parameter attributes used by kinematic pairs.
  //! @return Standard_GUID for parameter attributes
  Standard_EXPORT static const Standard_GUID& getParamsID();

  //! Returns the special GUID of UAttribute that indicates a label contains
  //! XCAFKinObjects_KinematicPair attribute.
  //! @return Standard_GUID for identifying kinematic pair labels
  Standard_EXPORT static const Standard_GUID& getUID();

  DEFINE_STANDARD_RTTIEXT(XCAFKinObjects_KinematicPair, TDF_Attribute)
};

#endif
