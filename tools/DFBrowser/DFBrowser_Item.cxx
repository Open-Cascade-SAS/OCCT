// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/DFBrowser_Item.hxx>

#include <inspector/DFBrowser_ItemRole.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>

#include <QIcon>
#include <QObject>

const int INFO_LENGHT = 60;

// =======================================================================
// function : hasAttribute
// purpose :
// =======================================================================
bool DFBrowser_Item::HasAttribute() const
{
  initItem();
  return myAttributeGUID != Standard_GUID();
}

// =======================================================================
// function : getAttribute
// purpose :
// =======================================================================
Handle(TDF_Attribute) DFBrowser_Item::GetAttribute() const
{
  initItem();
  Handle(TDF_Attribute) anAttribute;
  GetLabel().FindAttribute (myAttributeGUID, anAttribute);
  return anAttribute;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_Item::Init()
{
  DFBrowser_ItemBasePtr aParentItem = itemDynamicCast<DFBrowser_ItemBase> (Parent());
  if (!aParentItem)
    return;
  TDF_Label aParentLabel = aParentItem->GetLabel();
  // items can exist only by items with not empty label
  if (aParentLabel.IsNull())
    return;

  int aNbAttributes = aParentLabel.NbAttributes();
  int aRowId = Row();
  if (aRowId < aNbAttributes)
  {
    Handle(TDF_Attribute) anAttribute;
    int anAttributeId = 0;
    for (TDF_AttributeIterator anAttrIt (aParentLabel); anAttrIt.More(); anAttrIt.Next(), anAttributeId++)
    {
      if (anAttributeId == aRowId)
        anAttribute = anAttrIt.Value();
    }
    SetAttribute (anAttribute);
  }
  else {
    int aCurrentId = aRowId - aNbAttributes;
    TDF_ChildIterator aLabelsIt (aParentLabel);
    TDF_Label aLabel;
    for (int aLabelId = 0; aLabelsIt.More(); aLabelsIt.Next(), aLabelId++)
    {
      if (aLabelId < aCurrentId)
        continue;
      aLabel = aLabelsIt.Value();
      break;
    }
    if (!aLabel.IsNull())
      setLabel (aLabel);
  }
  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : reset
// purpose :
// =======================================================================
void DFBrowser_Item::Reset()
{
  SetAttribute (Handle(TDF_Attribute)());

  DFBrowser_ItemBase::Reset();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int DFBrowser_Item::initRowCount() const
{
  return HasAttribute() ? 0 : DFBrowser_ItemBase::initRowCount();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant DFBrowser_Item::initValue (const int theItemRole) const
{
  if (!HasAttribute())
    return DFBrowser_ItemBase::initValue (theItemRole);

  if (theItemRole == DFBrowserPane_ItemRole_DisplayExtended || theItemRole == DFBrowserPane_ItemRole_ToolTipExtended)
  {
    int aRole = theItemRole == DFBrowserPane_ItemRole_DisplayExtended ? Qt::DisplayRole : Qt::ToolTipRole;
    QVariant aValue = DFBrowser_Module::GetAttributeInfo (GetAttribute(), GetModule(), aRole, Column());
    QString anAdditionalInfo = DFBrowser_Module::GetAttributeInfo (GetAttribute(), GetModule(),
                                                                    DFBrowser_ItemRole_AdditionalInfo, Column()).toString();
    if (!anAdditionalInfo.isEmpty())
    {
      if (theItemRole == DFBrowserPane_ItemRole_DisplayExtended && anAdditionalInfo.length() > INFO_LENGHT)
        anAdditionalInfo = anAdditionalInfo.mid (0, INFO_LENGHT - 3) + "...";
      if (!anAdditionalInfo.isEmpty())
        aValue = QVariant (aValue.toString() + QString (" [%1]").arg (anAdditionalInfo));
      //if (aRole == Qt::ToolTipRole)
      //  aValue = wrapTextByWords(aValue.toString().toStdString(), INFO_LENGHT).c_str();
    }
    return aValue;
  }

  return DFBrowser_Module::GetAttributeInfo (GetAttribute(), GetModule(), theItemRole, Column());
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowser_Item::GetAttributeInfo (int theRole) const
{
  initItem();
  return cachedValue (theRole);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void DFBrowser_Item::initItem() const
{
  if (IsInitialized())
    return;
  
  const_cast<DFBrowser_Item*>(this)->Init();
}

// =======================================================================
// function : SetAttribute
// purpose :
// =======================================================================
void DFBrowser_Item::SetAttribute (Handle(TDF_Attribute) theAttribute)
{
  if (!theAttribute.IsNull())
  {
    setLabel (theAttribute->Label());
    myAttributeGUID = theAttribute->ID();
  }
  else
  {
    setLabel (TDF_Label());
    myAttributeGUID = Standard_GUID();
  }
}
