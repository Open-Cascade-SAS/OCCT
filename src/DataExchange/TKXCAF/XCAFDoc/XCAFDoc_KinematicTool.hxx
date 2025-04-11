// Created on: 2020-03-16
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

#ifndef _XCAFKinObjects_KinematicTool_HeaderFile
#define _XCAFKinObjects_KinematicTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <Standard_Integer.hxx>

class XCAFKinObjects_KinematicPair;
class XCAFKinObjects_KinematicPairValue;
class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;

class XCAFDoc_KinematicTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_KinematicTool, TDF_Attribute)

//! Provides tools to store and retrieve Kinematics in and from TDocStd_Document.
//! Kinematic data storage consists of several mechanisms, each mechanism has
//! its own sets of links and joints. The tool manages the hierarchy of kinematic
//! elements (mechanisms, links, joints, states, values) and their relationships.
//!
//! The document structure is organized as follows:
//! - Each mechanism has a label under the base label
//! - Under each mechanism label, there are child labels for links (1), joints (2),
//!   and optionally states (3)
//! - Links represent rigid bodies that can be moved by joints
//! - Joints define the connection between two links and constrain their relative movement
//! - States represent specific configurations of the kinematic system
//! - Values define parameters for joints in various states
class XCAFDoc_KinematicTool : public TDF_Attribute
{

public:
  //! Default constructor.
  Standard_EXPORT XCAFDoc_KinematicTool();

  //! Creates (if not exist) KinematicTool on the specified label.
  //! @param theLabel Label where KinematicTool should be created
  //! @return Handle to the created or found KinematicTool
  Standard_EXPORT static Handle(XCAFDoc_KinematicTool) Set(const TDF_Label& theLabel);

  //! Returns the GUID for KinematicTool attribute.
  //! @return Standard_GUID identifying KinematicTool
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Returns the base label under which kinematic data is stored.
  //! @return TDF_Label that serves as the root for all kinematic data
  Standard_EXPORT TDF_Label BaseLabel() const;

  //! Creates a new mechanism as a child under the base label.
  //! Creates child labels for links (1) and joints (2).
  //! @return TDF_Label of the newly created mechanism
  Standard_EXPORT TDF_Label AddMechanism();

  //! Checks if the given label represents a mechanism.
  //! A valid mechanism has child labels for links (1) and joints (2),
  //! and optionally for states (3).
  //! @param theLabel Label to check
  //! @return true if the label represents a mechanism, false otherwise
  Standard_EXPORT Standard_Boolean IsMechanism(const TDF_Label& theLabel) const;

  //! Removes the given mechanism with all underlying information.
  //! Removes all links, joints, states and their associated data.
  //! @param theLabel Label of the mechanism to remove
  Standard_EXPORT void RemoveMechanism(const TDF_Label& theLabel);

  //! Adds a new empty label for a link under the specified mechanism.
  //! @param theMechanism Parent mechanism
  //! @param isBase Whether the link is the base link of the mechanism
  //! @return TDF_Label of the newly created link
  Standard_EXPORT TDF_Label AddLink(const TDF_Label&       theMechanism,
                                    const Standard_Boolean isBase = Standard_False);

  //! Adds a new label with a link and associates it with the given shapes.
  //! @param theMechanism Parent mechanism
  //! @param theShapes Sequence of labels pointing to shapes that make up the link
  //! @param isBase Whether the link is the base link of the mechanism
  //! @return TDF_Label of the newly created link
  Standard_EXPORT TDF_Label AddLink(const TDF_Label&         theMechanism,
                                    const TDF_LabelSequence& theShapes,
                                    const Standard_Boolean   isBase = Standard_False);

  //! Adds a new label with a link and associates it with a single shape.
  //! @param theMechanism Parent mechanism
  //! @param theShape Label pointing to the shape for the link
  //! @param isBase Whether the link is the base link of the mechanism
  //! @return TDF_Label of the newly created link
  Standard_EXPORT TDF_Label AddLink(const TDF_Label&       theMechanism,
                                    const TDF_Label&       theShape,
                                    const Standard_Boolean isBase = Standard_False);

  //! Checks if the given label represents a link.
  //! @param theLink Label to check
  //! @return true if the label represents a link, false otherwise
  Standard_EXPORT Standard_Boolean IsLink(const TDF_Label& theLink) const;

  //! Sets shapes references for a link.
  //! Removes any existing shape references before setting the new ones.
  //! @param theLink Link to set shapes for
  //! @param theShapes Sequence of labels pointing to shapes for the link
  //! @return true if successful, false otherwise
  Standard_EXPORT Standard_Boolean SetLink(const TDF_Label&         theLink,
                                           const TDF_LabelSequence& theShapes);

  //! Sets the given link as the base link for its parent mechanism.
  //! Only one base link is allowed per mechanism.
  //! @param theLink Link to set as base
  //! @return true if successful, false otherwise
  Standard_EXPORT Standard_Boolean SetBaseLink(const TDF_Label& theLink);

  //! Sets a single shape reference for a link.
  //! @param theLink Link to set shape for
  //! @param theShape Label pointing to the shape for the link
  //! @return true if successful, false otherwise
  Standard_EXPORT Standard_Boolean SetLink(const TDF_Label& theLink, const TDF_Label& theShape);

  //! Removes the given link and its references.
  //! Also removes all joints connected to this link.
  //! @param theLink Link to remove
  Standard_EXPORT void RemoveLink(const TDF_Label& theLink);

  //! Adds a new empty label for a joint under the specified mechanism.
  //! @param theMechanism Parent mechanism
  //! @return TDF_Label of the newly created joint
  Standard_EXPORT TDF_Label AddJoint(const TDF_Label& theMechanism);

  //! Adds a new label with a joint connecting the two given links.
  //! @param theMechanism Parent mechanism
  //! @param theLink1 First link to connect
  //! @param theLink2 Second link to connect
  //! @return TDF_Label of the newly created joint
  Standard_EXPORT TDF_Label AddJoint(const TDF_Label& theMechanism,
                                     const TDF_Label& theLink1,
                                     const TDF_Label& theLink2);

  //! Checks if the given label represents a joint.
  //! @param theJoint Label to check
  //! @return true if the label represents a joint, false otherwise
  Standard_EXPORT Standard_Boolean IsJoint(const TDF_Label& theJoint) const;

  //! Sets references to links for a joint.
  //! @param theJoint Joint to set links for
  //! @param theLink1 First link of the joint
  //! @param theLink2 Second link of the joint
  //! @return true if successful, false otherwise
  Standard_EXPORT Standard_Boolean SetJoint(const TDF_Label& theJoint,
                                            const TDF_Label& theLink1,
                                            const TDF_Label& theLink2);

  //! Removes the given joint and its references.
  //! @param theJoint Joint to remove
  Standard_EXPORT void RemoveJoint(const TDF_Label& theJoint);

  //! Checks if the given label represents a value.
  //! @param theValue Label to check
  //! @return true if the label represents a value, false otherwise
  Standard_EXPORT Standard_Boolean IsValue(const TDF_Label& theValue) const;

  //! Adds a new empty label for a state under the specified mechanism.
  //! @param theMechanism Parent mechanism
  //! @return TDF_Label of the newly created state
  Standard_EXPORT TDF_Label AddState(const TDF_Label& theMechanism);

  //! Checks if the given label represents a state.
  //! @param theState Label to check
  //! @return true if the label represents a state, false otherwise
  Standard_EXPORT Standard_Boolean IsState(const TDF_Label& theState) const;

  //! Adds a new empty label for a value under the specified state.
  //! @param theState Parent state
  //! @return TDF_Label of the newly created value
  Standard_EXPORT TDF_Label AddValue(const TDF_Label& theState);

  //! Removes the given value and its references.
  //! @param theValue Value to remove
  Standard_EXPORT void RemoveValue(const TDF_Label& theValue);

  //! Gets XCAFKinObjects_KinematicPair attribute from the given label.
  //! If such attribute is missing (first attempt after document import), sets the attribute.
  //! @param theLabel Label to check and get attribute from
  //! @return Handle to the kinematic pair attribute or NULL if label is invalid
  Standard_EXPORT Handle(XCAFKinObjects_KinematicPair) GetKinematicPair(const TDF_Label& theLabel);

  //! Gets XCAFKinObjects_KinematicPairValue attribute from the given label.
  //! If such attribute is missing (first attempt after document import), sets the attribute.
  //! @param theLabel Label to check and get attribute from
  //! @return Handle to the kinematic pair value attribute or NULL if label is invalid
  Standard_EXPORT Handle(XCAFKinObjects_KinematicPairValue) GetKinematicPairValue(
    const TDF_Label& theLabel);

  //! Retrieves all states labels of the given mechanism.
  //! @param theMechanism Parent mechanism
  //! @return Sequence of state labels
  Standard_EXPORT TDF_LabelSequence GetStates(const TDF_Label& theMechanism) const;

  //! Retrieves all value labels of the given state.
  //! @param theState Parent state
  //! @return Sequence of value labels
  Standard_EXPORT TDF_LabelSequence GetValuesOfState(const TDF_Label& theState) const;

  //! Gets reference to state of the given value.
  //! @param theValue Value to get state for
  //! @return Label of the parent state
  Standard_EXPORT TDF_Label GetStateOfValue(const TDF_Label& theValue) const;

  //! Gets reference to joint of the given value.
  //! @param theValue Value to get joint for
  //! @return Label of the referenced joint
  Standard_EXPORT TDF_Label GetJointOfValue(const TDF_Label& theValue) const;

  //! Gets references to values of the given joint.
  //! @param theJoint Joint to get values for
  //! @return Sequence of value labels
  Standard_EXPORT TDF_LabelSequence GetValuesOfJoint(const TDF_Label& theJoint) const;

  //! Removes the given state and all its values.
  //! @param theState State to remove
  Standard_EXPORT void RemoveState(const TDF_Label& theState);

  //! Retrieves all mechanisms labels.
  //! @return Sequence of mechanism labels
  Standard_EXPORT TDF_LabelSequence GetMechanisms() const;

  //! Retrieves all link labels of the given mechanism.
  //! @param theMechanism Parent mechanism
  //! @return Sequence of link labels
  Standard_EXPORT TDF_LabelSequence GetLinks(const TDF_Label& theMechanism) const;

  //! Retrieves all joint labels of the given mechanism.
  //! @param theMechanism Parent mechanism
  //! @return Sequence of joint labels
  Standard_EXPORT TDF_LabelSequence GetJoints(const TDF_Label& theMechanism) const;

  //! Gets references to links of the given joint.
  //! @param theJoint Joint to get links for
  //! @param theLink1 [out] First link of the joint
  //! @param theLink2 [out] Second link of the joint
  Standard_EXPORT void GetLinksOfJoint(const TDF_Label& theJoint,
                                       TDF_Label&       theLink1,
                                       TDF_Label&       theLink2) const;

  //! Gets references to joints of the given link.
  //! @param theLink Link to get joints for
  //! @param toFirst Whether to include joints where the link is the first link
  //! @param toSecond Whether to include joints where the link is the second link
  //! @return Sequence of joint labels
  Standard_EXPORT TDF_LabelSequence
    GetJointsOfLink(const TDF_Label&       theLink,
                    const Standard_Boolean toFirst  = Standard_True,
                    const Standard_Boolean toSecond = Standard_True) const;

public:
  //! Gets reference shapes of the given link.
  //! @param theLink Link to get shapes for
  //! @return Sequence of shape labels
  Standard_EXPORT TDF_LabelSequence GetRefShapes(const TDF_Label& theLink) const;

  //! Returns the GUID identifying this attribute.
  //! @return Standard_GUID of this attribute
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Restores the attribute from another attribute.
  //! @param with Attribute to restore from
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

  //! Creates a new empty attribute.
  //! @return Handle to the new attribute
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Pastes the attribute into another attribute.
  //! @param into Target attribute to paste into
  //! @param theRT Relocation table
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       into,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream.
  //! @param theOStream Stream to output data to
  //! @param theDepth Depth of the dumped information
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  //! Gets the root label for links of the given mechanism.
  //! @param theMechanism Mechanism to get links root for
  //! @return Label of links root (child 1)
  Standard_EXPORT TDF_Label getRootOfLinks(const TDF_Label& theMechanism) const;

  //! Gets the root label for joints of the given mechanism.
  //! @param theMechanism Mechanism to get joints root for
  //! @return Label of joints root (child 2)
  Standard_EXPORT TDF_Label getRootOfJoints(const TDF_Label& theMechanism) const;

  //! Gets the root label for states of the given mechanism.
  //! @param theMechanism Mechanism to get states root for
  //! @return Label of states root (child 3) or null if not exist
  Standard_EXPORT TDF_Label getRootOfStates(const TDF_Label& theMechanism) const;

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_KinematicTool, TDF_Attribute)
};

#endif // _XCAFKinObjects_KinematicTool_HeaderFile
