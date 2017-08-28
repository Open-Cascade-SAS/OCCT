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

#include <inspector/DFBrowser_TreeModel.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemApplication.hxx>
#include <inspector/DFBrowser_ItemDocument.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Window.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>
#include <NCollection_List.hxx>

#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

#include <QAbstractItemModel>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_TreeModel::DFBrowser_TreeModel (QObject* theParent, DFBrowser_Module* theModule)
: TreeModel_ModelBase (theParent)
{
  m_pRootItem = DFBrowser_ItemApplication::CreateItem (TreeModel_ItemBasePtr());
  DFBrowser_ItemApplicationPtr aRootItem = itemDynamicCast<DFBrowser_ItemApplication> (m_pRootItem);
  aRootItem->SetModule (theModule);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_TreeModel::Init (const Handle(TDocStd_Application)& theApplication)
{
  DFBrowser_ItemApplicationPtr aRootItem = itemDynamicCast<DFBrowser_ItemApplication> (m_pRootItem);
  Reset();
  aRootItem->SetApplication (theApplication);
  EmitLayoutChanged();
}

// =======================================================================
// function : GetTDocStdApplication
// purpose :
// =======================================================================
Handle(TDocStd_Application) DFBrowser_TreeModel::GetTDocStdApplication() const
{
  DFBrowser_ItemApplicationPtr aRootItem = itemDynamicCast<DFBrowser_ItemApplication> (m_pRootItem);
  return aRootItem->GetApplication();
}

// =======================================================================
// function : FindIndex
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeModel::FindIndex (const TDF_Label& theLabel) const
{
  TDF_Label aRoot = theLabel.Root();

  NCollection_List<TDF_Label> aLabels;
  aLabels.Prepend (theLabel);
  TDF_Label aFather = theLabel.Father();
  if (!aFather.IsNull())
  {
    while (aFather != aRoot)
    {
      aLabels.Prepend (aFather);
      aFather = aFather.Father();
    }
  }
  bool aDocumentItemFound = false;
  QModelIndex aParentIndex = index (0, 0);
  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex); // application item
  // find document, where label of document item is equal to Root label
  for (int aChildId = 0, aCount = aParentItem->rowCount(); aChildId < aCount; aChildId++)
  {
    QModelIndex anIndex = index (aChildId, 0, aParentIndex);
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    DFBrowser_ItemDocumentPtr anItem = itemDynamicCast<DFBrowser_ItemDocument> (anItemBase);
    if (anItem->GetLabel() == aRoot)
    {
      aParentItem = anItem;
      aParentIndex = anIndex;
      aDocumentItemFound = true;
      break;
    }
  }
  if (!aDocumentItemFound) // document is not found
    return QModelIndex();

  for (NCollection_List<TDF_Label>::const_iterator aLabelIt = aLabels.begin(); aLabelIt != aLabels.end()
       && aParentIndex.isValid(); aLabelIt++)
  {
    const TDF_Label aLabel = *aLabelIt;
    for (int aParentChildId = 0, aCount = aParentItem->rowCount(); aParentChildId < aCount; aParentChildId++)
    {
      QModelIndex anIndex = index (aParentChildId, 0, aParentIndex);
      DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (TreeModel_ModelBase::GetItemByIndex (anIndex));
      if (anItem->HasAttribute())
        continue;

      if (anItem->HasLabel() && anItem->GetLabel().IsEqual (aLabel))
      {
        aParentItem = anItem;
        aParentIndex = anIndex;
        break;
      }
    }
  }
  return aParentIndex;
}

// =======================================================================
// function : FindIndexByPath
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeModel::FindIndexByPath (const QStringList& theLabelEntries, const QString& theValue) const
{
  QModelIndex aFoundIndex;

  QModelIndex aRootIndex = index (0, 0);
  TreeModel_ItemBasePtr aRootItem = TreeModel_ModelBase::GetItemByIndex (aRootIndex); // application item
  // find document, where label of document item is equal to Root label
  for (int aDocItemId = 0, aNbDocItems = aRootItem->rowCount(); aDocItemId < aNbDocItems && !aFoundIndex.isValid(); aDocItemId++)
  {
    QModelIndex aParentIndex = index (aDocItemId, 0, aRootIndex);
    if (!aParentIndex.isValid()) // OCAF document for this document item is not found
      continue;
    if (theLabelEntries.size() == 0)
    {
      aFoundIndex = aParentIndex;
      break;
    }
    TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex);
    for (int aPathId = 1, aPathCount = theLabelEntries.size(); aPathId < aPathCount + 1; aPathId++)
    {
      QString anEntry;
      if (aPathId < aPathCount)
        anEntry = theLabelEntries[aPathId];
      else
        anEntry = theValue;

      bool aFoundEntry = false;
      for (int aChildId = 0, aNbChildren = aParentItem->rowCount(); aChildId < aNbChildren; aChildId++)
      {
        QModelIndex anIndex = index (aChildId, 0, aParentIndex);
        TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
        DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);

        if (aPathId == aPathCount && anItem->HasAttribute())
        {
          // processing attribute in theValue
          DFBrowser_ItemApplicationPtr aRootAppItem = itemDynamicCast<DFBrowser_ItemApplication>(m_pRootItem);
          QString anAttributeInfo = DFBrowser_Module::GetAttributeInfo (anItem->GetAttribute(), aRootAppItem->GetModule(),
                                                                        Qt::DisplayRole, 0).toString();
          if (anAttributeInfo == anEntry)
          {
            aParentItem = anItem;
            aParentIndex = anIndex;
            aFoundEntry = true;
            break;
          }
        }
        else if (anItem->HasLabel() &&
                 anEntry == QString (DFBrowserPane_Tools::GetEntry (anItem->GetLabel()).ToCString()))
        {
          aParentItem = anItem;
          aParentIndex = anIndex;
          aFoundEntry = true;
          break;
        }

      }
      if (!aFoundEntry) // an entry is not found on some level tree, find it in other documents
        break;
      else
        aFoundIndex = aParentIndex;
    }
  }
  return aFoundIndex;
}

// =======================================================================
// function : FindIndexByAttribute
// purpose :
// =======================================================================
QModelIndex DFBrowser_TreeModel::FindIndexByAttribute (Handle(TDF_Attribute) theAttribute) const
{
  QModelIndex aFoundIndex;
  const TDF_Label aLabel = theAttribute->Label();

  QModelIndex aParentIndex = FindIndex (aLabel);
  if (!aParentIndex.isValid())
    return aFoundIndex;

  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex);
  for (int aChildId = 0, aCount = aParentItem->rowCount(); aChildId < aCount; aChildId++)
  {
    QModelIndex anIndex = index (aChildId, 0, aParentIndex);
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    DFBrowser_ItemPtr anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);
    if (anItem->GetAttribute() == theAttribute)
    {
      aFoundIndex = anIndex;
      break;
    }
  }
  return aFoundIndex;
}

// =======================================================================
// function : ConvertToIndices
// purpose :
// =======================================================================
void DFBrowser_TreeModel::ConvertToIndices (const NCollection_List<TDF_Label>& theReferences,
                                            QModelIndexList& theIndices)
{
  for (NCollection_List<TDF_Label>::Iterator aLabelItr (theReferences); aLabelItr.More(); aLabelItr.Next())
    theIndices.append (FindIndex (aLabelItr.Value()));
}

// =======================================================================
// function : ConvertToIndices
// purpose :
// =======================================================================
void DFBrowser_TreeModel::ConvertToIndices (const NCollection_List<Handle(TDF_Attribute)>& theReferences,
                                            QModelIndexList& theIndices)
{
  for (NCollection_List<Handle(TDF_Attribute)>::Iterator anAttrItr (theReferences); anAttrItr.More(); anAttrItr.Next())
    theIndices.append(FindIndexByAttribute (anAttrItr.Value()));
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowser_TreeModel::data (const QModelIndex& theIndex, int theRole) const
{
  if (theRole == Qt::BackgroundRole && myHighlightedIndices.contains (theIndex))
    return DFBrowserPane_Tools::LightHighlightColor();
  return TreeModel_ModelBase::data (theIndex, theRole);
}
