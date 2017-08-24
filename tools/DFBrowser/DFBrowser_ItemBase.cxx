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

#include <inspector/DFBrowser_ItemBase.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>

#include <QColor>
#include <QIcon>
#include <QVariant>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_ItemBase::DFBrowser_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
: TreeModel_ItemBase (theParent, theRow, theColumn), myModule (0), myIsUseAdditionalInfo (true)
{
}

// =======================================================================
// function : reset
// purpose :
// =======================================================================
void DFBrowser_ItemBase::Reset()
{
  setLabel (TDF_Label());
  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : GetLabel
// purpose :
// =======================================================================
TDF_Label DFBrowser_ItemBase::GetLabel() const
{
  initItem();
  return myLabel;
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowser_ItemBase::data (const QModelIndex& theIndex, int theRole) const
{
  int aRole = theRole;
  if (Column() == 0 && useAdditionalInfo())
  {
    switch (theRole)
    {
      case Qt::DisplayRole: { aRole = DFBrowserPane_ItemRole_DisplayExtended; break; }
      case Qt::ToolTipRole: { aRole = DFBrowserPane_ItemRole_ToolTipExtended; break; }
    }
  }
  return TreeModel_ItemBase::data (theIndex, aRole);
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int DFBrowser_ItemBase::initRowCount() const
{
  TDF_Label aLabel = GetLabel();
  if (aLabel.IsNull())
    return 0;

  return aLabel.NbChildren() + aLabel.NbAttributes();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant DFBrowser_ItemBase::initValue (const int theItemRole) const
{
  switch (theItemRole)
  {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      return DFBrowser_Tools::GetLabelInfo (myLabel, false);
    case DFBrowserPane_ItemRole_DisplayExtended:
    case DFBrowserPane_ItemRole_ToolTipExtended:
      return DFBrowser_Tools::GetLabelInfo (myLabel, true);
    case Qt::ForegroundRole:
    {
      QVariant aValue = QColor (Qt::black);
      if (DFBrowser_Tools::IsEmptyLabel(GetLabel()))
        aValue = QColor (Qt::lightGray);
      else
      { //! TEMPORARY HERE : should be moved in the pane of TDataStd_Name kind of attribute
        Handle(TDataStd_Name) aName;
        if (useAdditionalInfo() && myLabel.FindAttribute (TDataStd_Name::GetID(), aName))
          aValue = QColor (Qt::darkGreen);
      }
      return aValue;
    }
    case Qt::DecorationRole: return DFBrowser_Tools::GetLabelIcon (myLabel);
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr DFBrowser_ItemBase::createChild (int theRow, int theColumn)
{
  TreeModel_ItemBasePtr anItem = DFBrowser_Item::CreateItem (currentItem(), theRow, theColumn);
  DFBrowser_ItemBasePtr aBaseItem = itemDynamicCast<DFBrowser_ItemBase> (anItem);
  aBaseItem->SetModule (GetModule());

  return anItem;
}

// =======================================================================
// function : SetUseAdditionalInfo
// purpose :
// =======================================================================
bool DFBrowser_ItemBase::SetUseAdditionalInfo (const bool theValue)
{
  bool aPreviousValue = myIsUseAdditionalInfo;
  myIsUseAdditionalInfo = theValue;
  return aPreviousValue;
}

