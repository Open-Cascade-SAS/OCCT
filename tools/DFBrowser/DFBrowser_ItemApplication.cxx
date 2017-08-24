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

#include <inspector/DFBrowser_ItemApplication.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_ItemDocument.hxx>
#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>

#include <QObject>

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr DFBrowser_ItemApplication::createChild (int theRow, int theColumn)
{
  TreeModel_ItemBasePtr anItem = DFBrowser_ItemDocument::CreateItem (currentItem(), theRow, theColumn);
  DFBrowser_ItemBasePtr aBaseItem = itemDynamicCast<DFBrowser_ItemBase> (anItem);
  aBaseItem->SetModule (GetModule());

  return anItem;
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int DFBrowser_ItemApplication::initRowCount() const
{
  if (myApplication.IsNull())
    return 0;

  return myApplication->NbDocuments();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant DFBrowser_ItemApplication::initValue (const int theItemRole) const
{
  if (theItemRole == Qt::DisplayRole ||
      theItemRole == Qt::EditRole ||
      theItemRole == Qt::ToolTipRole ||
      theItemRole == DFBrowserPane_ItemRole_DisplayExtended ||
      theItemRole == DFBrowserPane_ItemRole_ToolTipExtended)
  {
    return "TDocStd_Application";
  }
  return QVariant();
}
