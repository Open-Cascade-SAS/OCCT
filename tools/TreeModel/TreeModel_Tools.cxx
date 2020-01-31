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

#include <inspector/TreeModel_Tools.hxx>
#include <inspector/TreeModel_ModelBase.hxx>
#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_VisibilityState.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QHeaderView>
#include <QObject>
#include <QRegExp>
#include <QStringList>
#include <QStyle>
#include <QTreeView>
#include <Standard_WarningsRestore.hxx>

const int INFO_LENGHT = 60;

// =======================================================================
// function : ToString
// purpose :
// =======================================================================
QString TreeModel_Tools::ToString (const QByteArray& theValue)
{
  char aBuffer[8];
  QStringList aBytesList;
  for (int aByteId = 0; aByteId < theValue.size();  aByteId++)
  {
    ::sprintf (aBuffer, "#%02X", (unsigned char)theValue.at (aByteId));
    aBytesList.append (QString (aBuffer));
  }
  return QString ("@ByteArray[%1]").arg (aBytesList.join (" "));
}

// =======================================================================
// function : ToByteArray
// purpose :
// =======================================================================
QByteArray TreeModel_Tools::ToByteArray (const QString& theValue)
{
  QByteArray aStateArray;
  if (!theValue.startsWith ("@ByteArray[") || !theValue.endsWith (']'))
    return aStateArray;

  QString aValue = theValue.mid (11, theValue.size() - 12);
  QStringList lst = aValue.split (QRegExp ("[\\s|,]"), QString::SkipEmptyParts);
  for (QStringList::ConstIterator aByteId = lst.begin(); aByteId != lst.end(); ++aByteId)
  {
    int aBase = 10;
    QString aString = *aByteId;
    if (aString.startsWith ("#"))
    {
      aBase = 16;
      aString = aString.mid (1);
    }
    bool isOk = false;
    int aNum = aString.toInt (&isOk, aBase);
    if (!isOk || aNum < 0 || aNum > 255)
      continue;
    aStateArray.append ((char)aNum);
  }
  return aStateArray;
}

// =======================================================================
// function : SaveState
// purpose :
// =======================================================================
void TreeModel_Tools::SaveState (QTreeView* theTreeView, QMap<QString, QString>& theItems,
                                 const QString& thePrefix)
{
  QStringList aColumnWidths, aHiddenColumns;
  for (int aColumnId = 0; aColumnId < theTreeView->model()->columnCount(); aColumnId++)
  {
    if (theTreeView->isColumnHidden (aColumnId))
    {
      aHiddenColumns.append (QString::number (aColumnId));
      aColumnWidths.append (QString());
    }
    else
      aColumnWidths.append (QString::number (theTreeView->columnWidth (aColumnId)));
  }
  theItems[thePrefix + "columns_width"] = aColumnWidths.join (",");
  theItems[thePrefix + "columns_hidden"] = aHiddenColumns.join (",");
}

// =======================================================================
// function : RestoreState
// purpose :
// =======================================================================
bool TreeModel_Tools::RestoreState (QTreeView* theTreeView, const QString& theKey, const QString& theValue,
                                    const QString& thePrefix)
{
  if (theKey == thePrefix + "columns_width")
  {
    QStringList aValues = theValue.split (",");
    for (int aColumnId = 0; aColumnId < theTreeView->model()->columnCount() && aColumnId < aValues.size(); aColumnId++)
    {
      bool isOk;
      int aWidth = aValues.at (aColumnId).toInt (&isOk);
      if (isOk && !theTreeView->isColumnHidden (aColumnId)) // do not resize hidden columnsa
        theTreeView->setColumnWidth (aColumnId, aWidth);
    }
  }
  else if (theKey == thePrefix + "columns_hidden")
  {
    int aColumnSize = theTreeView->model()->columnCount();
    QStringList aValues = theValue.split (",", QString::SkipEmptyParts);
    QList<int> aColumnIds;
    for (int aValueId = 0; aValueId < aValues.size(); aValueId++)
    {
      if (aValueId < aColumnSize)
        aColumnIds.append (aValues.at (aValueId).toInt());
    }
    for (int aColumnId = 0; aColumnId < aColumnSize; aColumnId++)
    {
      theTreeView->setColumnHidden (aColumnId, aColumnIds.contains(aColumnId) == true);
    }
  }
  else
    return false;
  return true;
}

// =======================================================================
// function : SetDefaultHeaderSections
// purpose :
// =======================================================================
void TreeModel_Tools::SetDefaultHeaderSections(QTreeView* theTreeView)
{
  TreeModel_ModelBase* aTreeModel = dynamic_cast<TreeModel_ModelBase*> (theTreeView->model());

  for (int aColumnId = 0, aNbColumns = aTreeModel->columnCount(); aColumnId < aNbColumns; aColumnId++)
  {
    TreeModel_HeaderSection aSection = aTreeModel->GetHeaderItem (aColumnId);
    theTreeView->setColumnWidth (aColumnId, aSection.GetWidth());
    theTreeView->setColumnHidden (aColumnId, aSection.IsHidden());
  }
}

// =======================================================================
// function : UseVisibilityColumn
// purpose :
// =======================================================================
void TreeModel_Tools::UseVisibilityColumn (QTreeView* theTreeView, const bool theActive)
{
  QHeaderView* aHeader = theTreeView->header();
#if QT_VERSION < 0x050000
  aHeader->setResizeMode (TreeModel_ColumnType_Visibility, QHeaderView::Fixed);
#else
  aHeader->setSectionResizeMode (TreeModel_ColumnType_Visibility, QHeaderView::Fixed);
#endif
  aHeader->moveSection (TreeModel_ColumnType_Name, TreeModel_ColumnType_Visibility);

  TreeModel_ModelBase* aModel = dynamic_cast<TreeModel_ModelBase*> (theTreeView->model());
  aModel->SetHeaderItem (TreeModel_ColumnType_Visibility,
    TreeModel_HeaderSection ("Visibility", TreeModel_ModelBase::ColumnVisibilityWidth()));
  aModel->SetUseVisibilityColumn (theActive);
}

// =======================================================================
// function : GetTextWidth
// purpose :
// =======================================================================
int TreeModel_Tools::GetTextWidth (const QString& theText, QObject*)
{
  // TODO: find margins like QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, (QWidget*)theParent);
  int aTextMargin = 10;
  QFontMetrics aFontMetrics (QApplication::font());
  QRect aBoundingRect = aFontMetrics.boundingRect (theText);
  return qMax (aBoundingRect.width(), aFontMetrics.width (theText)) + aTextMargin * 2;
}

// =======================================================================
// function : CutString
// purpose :
// =======================================================================
QString TreeModel_Tools::CutString (const QString& theText, const int theWidth, const QString& theTail)
{
  if (theText.isEmpty())
    return theText;

  int aLength = theWidth < 0 ? INFO_LENGHT : theWidth - 3;

  int anIndex = theText.indexOf ('\n');
  if (anIndex > 0 && anIndex < aLength)
    aLength = anIndex;

  return aLength < theText.length() ? theText.mid (0, aLength) + theTail : theText;
}

// =======================================================================
// function : LightHighlightColor
// purpose :
// =======================================================================
QColor TreeModel_Tools::LightHighlightColor()
{
  QWidget aWidget;
  QPalette aPalette = aWidget.palette();
  return aPalette.highlight().color().lighter();
}

// =======================================================================
// function : SetExpandedTo
// purpose :
// =======================================================================
void TreeModel_Tools::SetExpandedTo (QTreeView* theTreeView, const QModelIndex& theIndex)
{
  QAbstractItemModel* aModel = theTreeView->model();

  QModelIndex aParent = aModel->parent (theIndex);
  while (aParent.isValid())
  {
    theTreeView->setExpanded (aParent, true);
    aParent = aModel->parent (aParent);
  }
}

// =======================================================================
// function : setExpanded
// purpose :
// =======================================================================
void TreeModel_Tools::SetExpanded (QTreeView* theTreeView, const QModelIndex& theIndex, const bool isExpanded,
                                   int& theLevels)
{
  bool isToExpand = theLevels == -1 || theLevels > 0;
  if (!isToExpand)
    return;

  theTreeView->setExpanded (theIndex, isExpanded);
  if (theLevels != -1)
    theLevels--;

  QAbstractItemModel* aModel = theTreeView->model();
  for (int aRowId = 0, aRows = aModel->rowCount (theIndex); aRowId < aRows; aRowId++)
  {
    int aLevels = theLevels;
    SetExpanded (theTreeView, aModel->index (aRowId, 0, theIndex), isExpanded, aLevels);
  }
}
