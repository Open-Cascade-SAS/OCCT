// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/TreeModel_ItemStream.hxx>
#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Dump.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QFont>
#include <Standard_WarningsRestore.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TreeModel_ItemProperties, Standard_Transient)

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Init ()
{
  myRowValues.Clear();

  const Standard_SStream& aStream = Item()->Stream();

  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue> aValues;
  TCollection_AsciiString aStreamText = Standard_Dump::Text (aStream);
  Standard_Dump::SplitJson (aStreamText, aValues);

  TreeModel_ItemStreamPtr aStreamParent = itemDynamicCast<TreeModel_ItemStream>(Item());
  TCollection_AsciiString aKey;
  Standard_DumpValue aKeyValue;
  if (!aStreamParent)
  {
    const Handle(Standard_Transient)& anItemObject = Item()->Object();
    aKey = anItemObject.IsNull() ? "Dump" : anItemObject->DynamicType()->Name();
    aKeyValue = Standard_DumpValue (aStreamText, 1);

    myKey = aKey;
    myStreamValue = aKeyValue;
  }
  else
  {
    TCollection_AsciiString aValue;
    if (Item()->Parent())
    {
      const Handle(TreeModel_ItemProperties)& aParentProperties = Item()->Parent()->Properties();
      if (aParentProperties)
        aParentProperties->ChildStream (Item()->Row(), aKey, aKeyValue);
    }
    myKey = aKey;
    myStreamValue = aKeyValue;

    aValues.Clear();
    Standard_Dump::SplitJson (myStreamValue.myValue, aValues);
  }

  for (Standard_Integer anIndex = 1; anIndex <= aValues.Size(); anIndex++)
  {
    Standard_DumpValue aValue = aValues.FindFromIndex (anIndex);
    if (Standard_Dump::HasChildKey (aValue.myValue))
      myChildren.Add (aValues.FindKey (anIndex), aValue);
    else
    {
      TreeModel_RowValue aRowValue (aValue.myStartPosition, aValues.FindKey (anIndex).ToCString(), aValue.myValue.ToCString());
      myRowValues.Add (myRowValues.Size() + 1, aRowValue);
    }
  }
  if (myRowValues.Size() == 1)
  {
    Quantity_Color aColor;
    if (Convert_Tools::ConvertStreamToColor (aStream, aColor))
    {
      Standard_Real aRed, aGreen, aBlue;
      aColor.Values (aRed, aGreen, aBlue, Quantity_TOC_sRGB);

      int aDelta = 255;
      myRowValues.ChangeFromIndex (1).CustomValues.insert ((int)Qt::BackgroundRole, QColor((int)(aRed * aDelta),
        (int)(aGreen * aDelta), (int)(aBlue * aDelta)));
    }
  }
}

// =======================================================================
// function :  Reset
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Reset()
{
  myKey = "";
  myStreamValue = Standard_DumpValue();

  myChildren.Clear();
  myRowValues.Clear();
}

// =======================================================================
// function : RowCount
// purpose :
// =======================================================================
int TreeModel_ItemProperties::RowCount() const
{
  return RowValues().Size();
}

// =======================================================================
// function : Data
// purpose :
// =======================================================================
QVariant TreeModel_ItemProperties::Data (const int theRow, const int theColumn, int theRole) const
{
  if (theColumn == 1 && theRole == Qt::BackgroundRole)
  {
    const QMap<int, QVariant>& aCachedValues = RowValues().FindFromIndex (theRow + 1).CustomValues;
    if (aCachedValues.contains ((int)theRole))
      return aCachedValues[(int)theRole];
  }

  if (theRole == Qt::FontRole) // method name is in italic
  {
    if (Data(theRow, 0, Qt::DisplayRole).toString().contains("className"))
    {
      QFont aFont = qApp->font();
      aFont.setItalic (true);
      return aFont;
    }
  }
  if (theRole == Qt::ForegroundRole)
  {
    if (Data(theRow, 0, Qt::DisplayRole).toString().contains("className"))
      return QColor (Qt::darkGray).darker(150);
  }

  if (theRole == Qt::DisplayRole || theRole == Qt::ToolTipRole)
  {
    if (theColumn == 0) return RowValues().FindFromIndex (theRow + 1).Key;
    else if (theColumn == 1) return RowValues().FindFromIndex (theRow + 1).Value;
  }

  return QVariant();
}

// =======================================================================
// function : EditType
// purpose :
// =======================================================================
ViewControl_EditType TreeModel_ItemProperties::EditType (const int, const int theColumn) const
{
  if (theColumn == 0)
    return ViewControl_EditType_None;

  return ViewControl_EditType_Line;
}

// =======================================================================
// function : SetData
// purpose :
// =======================================================================
bool TreeModel_ItemProperties::SetData (const int /*theRow*/, const int theColumn, const QVariant& /*theValue*/, int)
{
  if (theColumn == 0)
    return false;
  return false;
}

// =======================================================================
// function : Presentations
// purpose :
// =======================================================================
void TreeModel_ItemProperties::Presentations (NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  const Standard_SStream& aStream = Item()->Stream();
  Convert_Tools::ConvertStreamToPresentations (aStream, 1, -1, thePresentations);
}

// =======================================================================
// function : TableFlags
// purpose :
// =======================================================================
Qt::ItemFlags TreeModel_ItemProperties::TableFlags (const int, const int theColumn) const
{
  Qt::ItemFlags aFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (theColumn == 1)
    aFlags = aFlags | Qt::ItemIsEditable;

  return aFlags;
}

// =======================================================================
// function : ChildStream
// purpose :
// =======================================================================
void TreeModel_ItemProperties::ChildStream (const int theRowId,
                                            TCollection_AsciiString& theKey,
                                            Standard_DumpValue& theValue) const
{
  if (myChildren.Size() <= theRowId)
    return;

  theKey = myChildren.FindKey (theRowId + 1);
  theValue = myChildren.FindFromIndex (theRowId + 1);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void TreeModel_ItemProperties::initItem() const
{
  if (!Item())
    return;
  if (Item()->IsInitialized())
    return;
  Item()->Init();
}
