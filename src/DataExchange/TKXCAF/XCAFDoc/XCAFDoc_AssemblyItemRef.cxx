// Created on: 2017-02-16
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#include <XCAFDoc_AssemblyItemRef.hxx>

#include <Standard_GUID.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_AssemblyItemRef, TDF_Attribute)

enum
{
  ExtraRef_None,
  ExtraRef_AttrGUID,
  ExtraRef_SubshapeIndex
};

const Standard_GUID& XCAFDoc_AssemblyItemRef::GetID()
{
  static Standard_GUID s_ID("3F2E4CD6-169B-4747-A321-5670E4291F5D");
  return s_ID;
}

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_AssemblyItemRef::Get(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis;
  theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis);
  return aThis;
}

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_AssemblyItemRef::Set(
  const TDF_Label&              theLabel,
  const XCAFDoc_AssemblyItemId& theItemId)
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis))
  {
    aThis = new XCAFDoc_AssemblyItemRef();
    aThis->SetItem(theItemId);
    theLabel.AddAttribute(aThis);
  }
  return aThis;
}

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_AssemblyItemRef::Set(
  const TDF_Label&              theLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  const Standard_GUID&          theAttrGUID)
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis))
  {
    aThis = new XCAFDoc_AssemblyItemRef();
    aThis->SetItem(theItemId);
    aThis->SetGUID(theAttrGUID);
    theLabel.AddAttribute(aThis);
  }
  return aThis;
}

occ::handle<XCAFDoc_AssemblyItemRef> XCAFDoc_AssemblyItemRef::Set(
  const TDF_Label&              theLabel,
  const XCAFDoc_AssemblyItemId& theItemId,
  const int                     theShapeIndex)
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_AssemblyItemRef::GetID(), aThis))
  {
    aThis = new XCAFDoc_AssemblyItemRef();
    aThis->SetItem(theItemId);
    aThis->SetSubshapeIndex(theShapeIndex);
    theLabel.AddAttribute(aThis);
  }
  return aThis;
}

XCAFDoc_AssemblyItemRef::XCAFDoc_AssemblyItemRef()
    : myExtraRef(ExtraRef_None)
{
}

bool XCAFDoc_AssemblyItemRef::IsOrphan() const
{
  if (myItemId.IsNull())
    return true;

  TDF_Label aRoot = Label().Root();

  occ::handle<TDocStd_Owner> anOwner;
  if (!aRoot.FindAttribute(TDocStd_Owner::GetID(), anOwner))
    return true;

  occ::handle<TDocStd_Document> aDoc = anOwner->GetDocument();
  if (aDoc.IsNull())
    return true;

  occ::handle<TDF_Data> aData = aDoc->GetData();
  if (aData.IsNull())
    return true;

  TDF_Label aLabel;
  TDF_Tool::Label(aData, myItemId.GetPath().Last(), aLabel);
  if (aLabel.IsNull())
    return true;

  if (HasExtraRef())
  {
    if (IsGUID())
    {
      occ::handle<TDF_Attribute> anAttr;
      if (!aLabel.FindAttribute(GetGUID(), anAttr))
        return true;
    }
    else if (IsSubshapeIndex())
    {
      occ::handle<TNaming_NamedShape> aNamedShape;
      if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
        return true;

      TopoDS_Shape                                                  aShape = aNamedShape->Get();
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
      TopExp::MapShapes(aShape, aMap);
      int aSubshapeIndex = GetSubshapeIndex();
      if (aSubshapeIndex < 1 || aMap.Size() < aSubshapeIndex)
        return true;
    }
  }

  return false;
}

bool XCAFDoc_AssemblyItemRef::HasExtraRef() const
{
  return (myExtraRef != ExtraRef_None);
}

bool XCAFDoc_AssemblyItemRef::IsGUID() const
{
  return (myExtraRef == ExtraRef_AttrGUID && Standard_GUID::CheckGUIDFormat(myExtraId.ToCString()));
}

bool XCAFDoc_AssemblyItemRef::IsSubshapeIndex() const
{
  return (myExtraRef == ExtraRef_SubshapeIndex && myExtraId.IsIntegerValue());
}

const XCAFDoc_AssemblyItemId& XCAFDoc_AssemblyItemRef::GetItem() const
{
  return myItemId;
}

Standard_GUID XCAFDoc_AssemblyItemRef::GetGUID() const
{
  if (IsGUID())
    return Standard_GUID(myExtraId.ToCString());
  else
    return Standard_GUID();
}

int XCAFDoc_AssemblyItemRef::GetSubshapeIndex() const
{
  if (IsSubshapeIndex())
    return myExtraId.IntegerValue();
  else
    return 0;
}

void XCAFDoc_AssemblyItemRef::SetItem(const XCAFDoc_AssemblyItemId& theItemId)
{
  Backup();
  myItemId = theItemId;
  ClearExtraRef();
}

void XCAFDoc_AssemblyItemRef::SetItem(const NCollection_List<TCollection_AsciiString>& thePath)
{
  Backup();
  myItemId.Init(thePath);
  ClearExtraRef();
}

void XCAFDoc_AssemblyItemRef::SetItem(const TCollection_AsciiString& theString)
{
  Backup();
  myItemId.Init(theString);
  ClearExtraRef();
}

void XCAFDoc_AssemblyItemRef::SetGUID(const Standard_GUID& theAttrGUID)
{
  Backup();
  myExtraRef = ExtraRef_AttrGUID;
  char aGUIDStr[Standard_GUID_SIZE + 1];
  theAttrGUID.ToCString(aGUIDStr);
  aGUIDStr[Standard_GUID_SIZE] = '\0';
  myExtraId.Clear();
  myExtraId.AssignCat(aGUIDStr);
}

void XCAFDoc_AssemblyItemRef::SetSubshapeIndex(int theSubshapeIndex)
{
  Backup();
  myExtraRef = ExtraRef_SubshapeIndex;
  myExtraId.Clear();
  myExtraId.AssignCat(theSubshapeIndex);
}

void XCAFDoc_AssemblyItemRef::ClearExtraRef()
{
  Backup();
  myExtraRef = ExtraRef_None;
  myExtraId.Clear();
}

const Standard_GUID& XCAFDoc_AssemblyItemRef::ID() const
{
  return GetID();
}

occ::handle<TDF_Attribute> XCAFDoc_AssemblyItemRef::NewEmpty() const
{
  return new XCAFDoc_AssemblyItemRef();
}

void XCAFDoc_AssemblyItemRef::Restore(const occ::handle<TDF_Attribute>& theAttrFrom)
{
  occ::handle<XCAFDoc_AssemblyItemRef> anOther =
    occ::down_cast<XCAFDoc_AssemblyItemRef>(theAttrFrom);
  if (!anOther.IsNull())
  {
    myItemId   = anOther->myItemId;
    myExtraRef = anOther->myExtraRef;
    myExtraId  = anOther->myExtraId;
  }
}

void XCAFDoc_AssemblyItemRef::Paste(const occ::handle<TDF_Attribute>& theAttrInto,
                                    const occ::handle<TDF_RelocationTable>& /*theRT*/) const
{
  occ::handle<XCAFDoc_AssemblyItemRef> anOther =
    occ::down_cast<XCAFDoc_AssemblyItemRef>(theAttrInto);
  if (!anOther.IsNull())
  {
    anOther->myItemId   = myItemId;
    anOther->myExtraRef = myExtraRef;
    anOther->myExtraId  = myExtraId;
  }
}

Standard_OStream& XCAFDoc_AssemblyItemRef::Dump(Standard_OStream& theOS) const
{
  theOS << "Path: " << myItemId.ToString();
  if (IsGUID())
    theOS << "/GUID:" << myExtraId;
  else if (IsSubshapeIndex())
    theOS << "/Subshape: " << myExtraId;
  return theOS;
}

//=================================================================================================

void XCAFDoc_AssemblyItemRef::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myItemId)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myExtraRef)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myExtraId)
}
