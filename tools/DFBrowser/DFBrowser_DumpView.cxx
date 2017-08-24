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

#include <inspector/DFBrowser_DumpView.hxx>

#include <inspector/DFBrowser_Item.hxx>
#include <inspector/DFBrowser_Window.hxx>
#include <inspector/DFBrowser_TreeLevelView.hxx>
#include <OSD_OpenFile.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <QAbstractItemModel>
#include <QDir>
#include <QFile>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QWidget>

// =======================================================================
// function : onSelectionChanged
// purpose :
// =======================================================================
void DFBrowser_DumpView::OnTreeViewSelectionChanged (const QItemSelection& theSelected,
                                                     const QItemSelection&)
{
  myTextEdit->setVisible (false);
  myTextEdit->clear();

  QModelIndexList aSelectedIndices = theSelected.indexes();
  QModelIndexList aFirstColumnSelectedIndices;
  for (QModelIndexList::const_iterator aSelIt = aSelectedIndices.begin(); aSelIt != aSelectedIndices.end(); aSelIt++)
  {
    QModelIndex anIndex = *aSelIt;
    if (anIndex.column() == 0)
      aFirstColumnSelectedIndices.append (anIndex);
  }
  if (aFirstColumnSelectedIndices.size() != 1)
    return;

  QString aDumpInfo;
  const QModelIndex& anIndex = aFirstColumnSelectedIndices.first();
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
  DFBrowser_ItemPtr anItem;
  if (anItemBase)
    anItem = itemDynamicCast<DFBrowser_Item> (anItemBase);

  if (!anItem)
    return;

  TCollection_AsciiString aFileName = DFBrowser_Window::TmpDirectory();
  aFileName += "/dfbrowser.txt";
  // print dump to file(not in a string stream because result might be too long)
  std::ofstream aFileStream;
  OSD_OpenStream(aFileStream, aFileName, ios::out);
  if (anItem->HasAttribute())
  {
    Handle(TDF_Attribute) anAttribute = anItem->GetAttribute();
    if (!anAttribute.IsNull())
      anAttribute->Dump(aFileStream);
  }
  else if (anItem->HasLabel())
    anItem->GetLabel().Dump(aFileStream);
  aFileStream.close();

  // read dumped file to fill view
  QFile aFile (aFileName.ToCString());
  if (!aFile.open (QIODevice::ReadOnly | QIODevice::Text))
      return;
  QTextStream aStream (&aFile);
  while (!aStream.atEnd())
  {
    aDumpInfo.append (QString ("%1\n").arg (aStream.readLine()));
  }
  aFile.close();
  QDir aDir;
  aDir.remove (aFileName.ToCString());
  if (!aDumpInfo.isEmpty())
  {
    myTextEdit->setVisible (true);
    myTextEdit->setPlainText (aDumpInfo);
  }
}
