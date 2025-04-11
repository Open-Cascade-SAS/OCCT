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

#include <XCAFKinObjects_KinematicPairValue.hxx>

#include <XCAFDoc.hxx>
#include <XCAFKinObjects_KinematicPair.hxx>
#include <XCAFKinObjects_PairValueObject.hxx>
#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFKinObjects_KinematicPairValue, TDF_Attribute)

//=================================================================================================

XCAFKinObjects_KinematicPairValue::XCAFKinObjects_KinematicPairValue()
{
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPairValue::GetID()
{
  static Standard_GUID PairValueID("4F451E46-0AC9-4270-90E4-D96605CDE635");
  return PairValueID;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPairValue::getUID()
{
  static Standard_GUID PairUID("4F451E46-0AC9-4270-90E4-D96605CDE636");
  return PairUID;
}

//=================================================================================================

Standard_Boolean XCAFKinObjects_KinematicPairValue::IsValidLabel(const TDF_Label& theLabel)
{
  TDataStd_UAttribute::Set(theLabel, XCAFKinObjects_KinematicPairValue::getUID());
  Handle(TDataStd_UAttribute) anAttr;
  return theLabel.FindAttribute(XCAFKinObjects_KinematicPairValue::getUID(), anAttr);
}

//=================================================================================================

Handle(XCAFKinObjects_KinematicPairValue) XCAFKinObjects_KinematicPairValue::Set(const TDF_Label& theValue,
                                                                         const TDF_Label& theJoint)
{
  TDataStd_UAttribute::Set(theValue, XCAFKinObjects_KinematicPairValue::getUID());
  Handle(XCAFKinObjects_KinematicPairValue) anAttr;
  if (!theValue.FindAttribute(XCAFKinObjects_KinematicPairValue::GetID(), anAttr))
  {
    anAttr = new XCAFKinObjects_KinematicPairValue();
    theValue.AddAttribute(anAttr);
  }

  // check for exist old reference
  Handle(TDataStd_TreeNode) aJointNode, aValueNode;
  if (theValue.FindAttribute(XCAFDoc::KinematicRefJointGUID(), aValueNode))
  {
    if (aValueNode->HasFather() && aValueNode->Father()->Label() == theJoint)
      return anAttr;
    aValueNode->Remove();
  }

  // Add reference to Joint
  aValueNode = TDataStd_TreeNode::Set(theJoint, XCAFDoc::KinematicRefJointGUID());
  aJointNode = TDataStd_TreeNode::Set(theValue, XCAFDoc::KinematicRefJointGUID());

  aValueNode->Append(aJointNode);
  return anAttr;
}

//=================================================================================================

Handle(XCAFKinObjects_KinematicPairValue) XCAFKinObjects_KinematicPairValue::Set(const TDF_Label& theValue)
{
  TDataStd_UAttribute::Set(theValue, XCAFKinObjects_KinematicPairValue::getUID());
  Handle(XCAFKinObjects_KinematicPairValue) anAttr;
  if (!theValue.FindAttribute(XCAFKinObjects_KinematicPairValue::GetID(), anAttr))
  {
    anAttr = new XCAFKinObjects_KinematicPairValue();
    theValue.AddAttribute(anAttr);
  }

  return anAttr;
}

//=================================================================================================

void XCAFKinObjects_KinematicPairValue::SetObject(const Handle(XCAFKinObjects_PairValueObject)& theObject)
{
  Backup();

  // Check validity of object
  if (theObject->GetAllValues()->Length() == 0)
  {
    return;
  }
  TCollection_AsciiString aName = "Value ";
  aName.AssignCat(TCollection_AsciiString(Label().Tag()));
  TDataStd_Name::Set(Label(), aName);
  TDataStd_Integer::Set(Label(), theObject->Type());

  Handle(TDataStd_RealArray) aValuesAttr;
  aValuesAttr = TDataStd_RealArray::Set(Label(), 1, theObject->GetAllValues()->Length());
  aValuesAttr->ChangeArray(theObject->GetAllValues());
}

//=================================================================================================

Handle(XCAFKinObjects_PairValueObject) XCAFKinObjects_KinematicPairValue::GetObject() const
{
  // Type
  Handle(TDataStd_Integer)           aTypeAttr;
  Handle(XCAFKinObjects_PairValueObject) anObject = new XCAFKinObjects_PairValueObject();
  if (Label().FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
  {
    int aType = aTypeAttr->Get();
    anObject->SetType((XCAFKinObjects_PairType)aType);
  }

  // Values
  Handle(TDataStd_RealArray) aValuesAttr;
  if (Label().FindAttribute(TDataStd_RealArray::GetID(), aValuesAttr))
  {
    Handle(TColStd_HArray1OfReal) aValuesArray = aValuesAttr->Array();
    anObject->SetAllValues(aValuesArray);
  }

  return anObject;
}

//=================================================================================================

const Standard_GUID& XCAFKinObjects_KinematicPairValue::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFKinObjects_KinematicPairValue::Restore(const Handle(TDF_Attribute)& /*theWith*/) {}

//=================================================================================================

Handle(TDF_Attribute) XCAFKinObjects_KinematicPairValue::NewEmpty() const
{
  return new XCAFKinObjects_KinematicPairValue();
}

//=================================================================================================

void XCAFKinObjects_KinematicPairValue::Paste(const Handle(TDF_Attribute)& /*theInfo*/,
                                          const Handle(TDF_RelocationTable)& /*theRT*/) const
{
}

//=================================================================================================

void XCAFKinObjects_KinematicPairValue::DumpJson(Standard_OStream& theOStream,
                                             Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)
}
