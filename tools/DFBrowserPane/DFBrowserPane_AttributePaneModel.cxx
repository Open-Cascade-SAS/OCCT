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

#include <inspector/DFBrowserPane_AttributePaneModel.hxx>

#include <QApplication>
#include <QFont>
#include <QColor>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneModel::DFBrowserPane_AttributePaneModel (QObject* theParent)
: QAbstractTableModel (theParent), myOrientation (Qt::Vertical), myColumnCount (1)
{
  myItalicColumns.append (0);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPane_AttributePaneModel::Init (const QList<QVariant>& theValues)
{
  myValuesMap.clear();

  if (myOrientation == Qt::Vertical)
  {
    int aRows = theValues.size() / myColumnCount;
    QList<QVariant> aRowValues;
    int aValuesIndex = 0;
    for (int aRowId = 0; aRowId < aRows; aRowId++)
    {
      aRowValues.clear();
      for (int aColumnId = 0; aColumnId < myColumnCount; aColumnId++)
      {
        aRowValues.append (theValues[aValuesIndex]);
        aValuesIndex++;
      }
      myValuesMap[aRowId] = aRowValues;
    }
  }
  else {
    int aCols = theValues.size() / myColumnCount;
    QList<QVariant> aColValues;
    int aValuesIndex = 0;
    for (int aColumnId = 0; aColumnId < aCols; aColumnId++)
    {
      aColValues.clear();
      for (int aRowId = 0; aRowId < myColumnCount; aRowId++)
      {
        aColValues.append (theValues[aValuesIndex]);
        aValuesIndex++;
      }
      myValuesMap[aColumnId] = aColValues;
    }
  }
  emit layoutChanged();
}

// =======================================================================
// function : SetHeaderValues
// purpose :
// =======================================================================
void DFBrowserPane_AttributePaneModel::SetHeaderValues (const QList<QVariant>& theValues,
                                                        Qt::Orientation theOrientation)
{
  if (theOrientation == Qt::Horizontal)
    myHorizontalHeaderValues = theValues;
  else
    myVerticalHeaderValues = theValues;
}

// =======================================================================
// function : columnCount
// purpose :
// =======================================================================
int DFBrowserPane_AttributePaneModel::columnCount (const QModelIndex&/* theParent*/) const
{
  return myOrientation == Qt::Vertical ? myColumnCount : myValuesMap.size();
}

// =======================================================================
// function : rowCount
// purpose :
// =======================================================================
int DFBrowserPane_AttributePaneModel::rowCount (const QModelIndex&/* theParent*/) const
{
  return myOrientation == Qt::Vertical ? myValuesMap.size() : myColumnCount;
}

// =======================================================================
// function : data
// purpose :
// =======================================================================
QVariant DFBrowserPane_AttributePaneModel::data (const QModelIndex& theIndex, int theRole) const
{
  QVariant aValue;

  if (theRole == Qt::DisplayRole)
  {
    if (myOrientation == Qt::Vertical)
    {
      int aRowId = theIndex.row();
      QList<QVariant> aRowValues = myValuesMap[aRowId];
      aValue = aRowValues.at (theIndex.column());
    }
    else
    {
      int aColId = theIndex.column();
      QList<QVariant> aColValues = myValuesMap[aColId];
      aValue = aColValues.at (theIndex.row());
    }
  }
  if (myItalicColumns.contains (theIndex.column()) && theRole == Qt::FontRole)
  {
    QFont aFont = qApp->font();
    aFont.setItalic (true);
    return aFont;
  }
  if (myItalicColumns.contains (theIndex.column()) && theRole == Qt::ForegroundRole)
    return QColor (Qt::darkGray).darker(150);

  return aValue;
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant DFBrowserPane_AttributePaneModel::headerData (int theSection, Qt::Orientation theOrientation,
                                                       int theRole) const
{
  QVariant aValue = QAbstractTableModel::headerData (theSection, theOrientation, theRole);
  if (theRole == Qt::DisplayRole)
  {
    if (theOrientation == Qt::Horizontal)
    {
      if (!myHorizontalHeaderValues.empty() && theSection < myHorizontalHeaderValues.size())
        aValue = myHorizontalHeaderValues[theSection];
    }
    else
    { // vertical
      if (!myVerticalHeaderValues.empty() && theSection < myVerticalHeaderValues.size())
        aValue = myVerticalHeaderValues[theSection];
    }
  }
  return aValue;
}
