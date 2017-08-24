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

#include <inspector/DFBrowser_SearchLineModel.hxx>

#include <inspector/DFBrowser_Module.hxx>
#include <inspector/DFBrowser_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePane.hxx>
#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <QDir>
#include <QIcon>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowser_SearchLineModel::DFBrowser_SearchLineModel (QObject* theParent, DFBrowser_Module* theModule)
: QAbstractTableModel (theParent), myModule (theModule), myRowCount (0)
{
}

// =======================================================================
// function : SetValues
// purpose :
// =======================================================================
void DFBrowser_SearchLineModel::SetValues (const QMap<int, QMap<QString, DFBrowser_SearchItemInfo > >& theDocumentValues,
                                           const QMap<int, QStringList>& theDocumentInfoValues)
{
  myAdditionalValues = theDocumentValues;
  myDocumentInfoValues = theDocumentInfoValues;

  myRowCount = 0;
  for (QMap<int, QStringList>::const_iterator aValuesIt = myDocumentInfoValues.begin();
       aValuesIt != myDocumentInfoValues.end(); aValuesIt++)
    myRowCount += aValuesIt.value().size();
}

// =======================================================================
// function : ClearValues
// purpose :
// =======================================================================
void DFBrowser_SearchLineModel::ClearValues()
{
  myAdditionalValues.clear();
  myDocumentInfoValues.clear();
  myRowCount = 0;
}

// =======================================================================
// function : GetPath
// purpose :
// =======================================================================
QStringList DFBrowser_SearchLineModel::GetPath (const QModelIndex& theIndex) const
{
  int aRowInDocument;
  int aDocumentId = getDocumentId (theIndex.row(), aRowInDocument);
  if (aDocumentId < 0)
    return QStringList();

  const QMap<QString, DFBrowser_SearchItemInfo>& anAdditionalValues = myAdditionalValues[aDocumentId];
  const QStringList& anInfoValues = myDocumentInfoValues[aDocumentId];

  return anAdditionalValues[anInfoValues[aRowInDocument] ].Path();
}

// =======================================================================
// function : GetValue
// purpose :
// =======================================================================
QString DFBrowser_SearchLineModel::GetValue (const QModelIndex& theIndex) const
{
  int aRowInDocument;
  int aDocumentId = getDocumentId (theIndex.row(), aRowInDocument);
  if (aDocumentId < 0)
    return QString();
  QString aValue = myDocumentInfoValues[aDocumentId][aRowInDocument];
  return aValue.mid (0, aValue.indexOf (SplitSeparator()));
}

// =======================================================================
// function : index
// purpose :
// =======================================================================
QModelIndex DFBrowser_SearchLineModel::index (int theRow, int theColumn, const QModelIndex& theParent) const
{
  if (!hasIndex (theRow, theColumn, theParent))
    return QModelIndex();
  return createIndex (theRow, theColumn);
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowser_SearchLineModel::data (const QModelIndex& theIndex, int theRole) const
{
  switch (theIndex.column())
  {
    case 0:
    {
      if (theRole == Qt::DisplayRole || theRole == Qt::EditRole || theRole == Qt::ToolTipRole)
      {
        int aRowInDocument;
        int aDocumentId = getDocumentId (theIndex.row(), aRowInDocument);
        if (aDocumentId < 0)
          return QVariant();
        return myDocumentInfoValues[aDocumentId][aRowInDocument];
      }
      break;
    }
    case 1:
    {
      if (theRole == Qt::DecorationRole)
      {
        int aRowInDocument;
        int aDocumentId = getDocumentId (theIndex.row(), aRowInDocument);
        if (aDocumentId < 0)
          return QVariant();
        QString anInfoValue = myDocumentInfoValues[aDocumentId][aRowInDocument];
        QVariant anIcon = myAdditionalValues[aDocumentId][anInfoValue].Icon();
        if (anIcon.isNull())
        {
          QString anAttributeName = anInfoValue.mid (0, anInfoValue.indexOf (SplitSeparator()));
          anIcon = DFBrowser_Module::GetAttributeInfo (anAttributeName.toUtf8().data(),
                                                       myModule, DFBrowserPane_ItemRole_Decoration_40x40, 0);
        }
        else
          anIcon = DFBrowser_Tools::GetLabelIcon (TDF_Label(), false);
        return anIcon;
      }
      if (theRole == Qt::SizeHintRole) return QSize (80, 80);
      break;
    }
    case 2:
    {
      if (theRole == Qt::DisplayRole || theRole == Qt::EditRole)
      {
        int aRowInDocument;
        int aDocumentId = getDocumentId (theIndex.row(), aRowInDocument);
        if (aDocumentId < 0)
          return QVariant();
        QString anInfoValue = myDocumentInfoValues[aDocumentId][aRowInDocument];
        return myAdditionalValues[aDocumentId][anInfoValue].PathUnited();
      }
      break;
    }
    default:
      break;
  }
  return QVariant();
}

// =======================================================================
// function : emitLayoutChanged
// purpose :
// =======================================================================
void DFBrowser_SearchLineModel::EmitLayoutChanged()
{
  emit layoutChanged();
}

// =======================================================================
// function : getDocumentId
// purpose :
// =======================================================================
int DFBrowser_SearchLineModel::getDocumentId (const int theRow, int& theRowInDocument) const
{
  theRowInDocument = 0;
  int aDocumentId = -1;

  int aCurrentRow = theRow;
  for (int aValueId = 0, aSize = myDocumentInfoValues.size(); aValueId < aSize; aValueId++)
  {
    int aValueIndex = aValueId+1;
    if (!myDocumentInfoValues.contains (aValueIndex))
      continue;
    QStringList aValues = myDocumentInfoValues[aValueIndex];
    int aValuesSize = aValues.size();
    if (aCurrentRow < aValuesSize)
    {
      aDocumentId = aValueIndex;
      theRowInDocument= aCurrentRow;
    }
    else
      aCurrentRow = aCurrentRow - aValuesSize;
  }
  return aDocumentId;
}
