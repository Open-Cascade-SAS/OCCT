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

#include <inspector/DFBrowser_ItemDocument.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemApplication.hxx>
#include <inspector/DFBrowser_ItemDocument.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>

#include <QIcon>
#include <QLabel>
#include <QObject>

// =======================================================================
// function : GetLabel
// purpose :
// =======================================================================
TDF_Label DFBrowser_ItemDocument::GetLabel() const
{
  TDF_Label aLabel;
  if (myDocument.IsNull())
    getDocument();

  if (!myDocument.IsNull())
    aLabel = myDocument->Main().Root();

  if (!aLabel.IsNull())
    aLabel = aLabel.Root();

  return aLabel;
}

// =======================================================================
// function : getDocument
// purpose :
// =======================================================================
const Handle(TDocStd_Document)& DFBrowser_ItemDocument::getDocument() const
{
  initItem();
  return myDocument;
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant DFBrowser_ItemDocument::initValue (const int theItemRole) const
{
  if (theItemRole == Qt::DisplayRole ||
      theItemRole == Qt::EditRole ||
      theItemRole == DFBrowserPane_ItemRole_DisplayExtended ||
      theItemRole == DFBrowserPane_ItemRole_ToolTipExtended)
    return DFBrowser_Tools::GetLabelInfo (GetLabel());
  if (theItemRole == Qt::DecorationRole)
    return DFBrowser_Tools::GetLabelIcon (GetLabel());

  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr DFBrowser_ItemDocument::createChild (int theRow, int theColumn)
{
  TreeModel_ItemBasePtr anItem = DFBrowser_Item::CreateItem (currentItem(), theRow, theColumn);
  DFBrowser_ItemBasePtr aBaseItem = itemDynamicCast<DFBrowser_ItemBase> (anItem);
  aBaseItem->SetModule (GetModule());

  return anItem;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowser_ItemDocument::Init()
{
  DFBrowser_ItemApplicationPtr aParentItem = itemDynamicCast<DFBrowser_ItemApplication> (Parent());
  if (!aParentItem)
    return;

  const Handle(TDocStd_Application)& anApplication = aParentItem->GetApplication();
  // items can exist only by items with not empty label
  if (anApplication.IsNull())
    return;

  int aRowId = Row();

  int aDocumentId = -1;
  for (Standard_Integer aDocId = 1, aNbDoc = anApplication->NbDocuments(); aDocId <= aNbDoc && aDocumentId < 0; aDocId++)
  {
    if (aDocId - 1 == aRowId)
      aDocumentId = aDocId;
  }
  if (aDocumentId > 0)
  {
    Handle(TDocStd_Document) aDocument;
    anApplication->GetDocument (aDocumentId, aDocument);
    setDocument (aDocument);
  }
  else
    setDocument (Handle(TDocStd_Document)());

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : reset
// purpose :
// =======================================================================
void DFBrowser_ItemDocument::Reset()
{
  Handle(TDocStd_Document) aDocument;
  setDocument (aDocument);

  DFBrowser_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void DFBrowser_ItemDocument::initItem() const
{
  if (IsInitialized())
    return;

  const_cast<DFBrowser_ItemDocument*>(this)->Init();
}
