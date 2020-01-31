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

#ifndef TreeModel_ItemProperties_H
#define TreeModel_ItemProperties_H

#include <Standard.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>

#include <TCollection_AsciiString.hxx>

#include <inspector/TreeModel_ItemBase.hxx>
#include <inspector/ViewControl_EditType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QModelIndexList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

//! \class TreeModel_ItemProperties
//! Class to manipulate properties of tree item. The properties are organized in table structure
class TreeModel_ItemProperties : public Standard_Transient
{
  //! enum defined the dimension type
  enum TreeModel_DimType
  {
    TreeModel_DimType_Rows,   //!< defines number of rows
    TreeModel_DimType_Columns //!< defines number of columns
  };

  //! container of values in a row of property table
  struct TreeModel_RowValue
  {
    TreeModel_RowValue (const Standard_Integer theValueStartPosition, const QVariant& theKey, QVariant theValue)
    : ValueStartPosition (theValueStartPosition), Key (theKey), Value (theValue) {}

    Standard_Integer ValueStartPosition; //!< start position of the key
    QVariant Key; //!< value in the first column
    QVariant Value; //!< value in the second column
    QMap<int, QVariant> CustomValues; //!< custom values, e.g. key is Background, value is a defined color
  };

public:
  //! Constructor
  TreeModel_ItemProperties() {}

  //! Destructor
  ~TreeModel_ItemProperties() {}

  //! Sets the current item
  void SetItem (const TreeModel_ItemBasePtr& theItem) { myItem = theItem; }

  //! Returns the current item
  TreeModel_ItemBasePtr Item() const { return myItem; }

  //! Fills internal containers by item stream values
  Standard_EXPORT void Init();

  //! If the item has internal values, there should be reseted here.
  Standard_EXPORT virtual void Reset();

  //! Returns number of table rows
  //! \return an integer value
  Standard_EXPORT int RowCount() const;

  //! Returns number of table columns
  //! \return an integer value
  int ColumnCount() const { return 2; }

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value interpreted depending on the given role
  Standard_EXPORT QVariant Data (const int theRow, const int theColumn, int theRole = Qt::DisplayRole) const;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  Standard_EXPORT ViewControl_EditType EditType (const int theRow, const int theColumn) const;

  //! Sets content of the model index for the given role, it is applied to internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return true if the value is changed
  Standard_EXPORT virtual bool SetData (const int theRow, const int theColumn, const QVariant& theValue,
                                        int theRole = Qt::DisplayRole);

  //! Returns presentation of the attribute to be visualized in the view
  //! \thePresentations [out] container of presentation handles to be visualized
  Standard_EXPORT virtual void Presentations (NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable.
  //! Additional flag for the column 1 is Qt::ItemIsEditable.
  //! \param theIndex a model index
  //! \return flags
  Standard_EXPORT virtual Qt::ItemFlags TableFlags (const int theRow, const int theColumn) const;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT void ChildStream (const int theRowId,
                                    TCollection_AsciiString& theKey,
                                    Standard_DumpValue& theValue) const;

  //! Returns data object of the item.
  //! \return object key
  const TCollection_AsciiString& Key() const { return myKey; }

  //! Returns stream value of the item.
  //! \return value
  const TCollection_AsciiString& StreamValue() const { return myStreamValue.myValue; }

  //! Returns children stream values
  //const NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& Values() const { initItem(); return myValues; }

  //! Returns children stream values
  const NCollection_IndexedDataMap<Standard_Integer, TreeModel_RowValue>& RowValues() const { initItem(); return myRowValues; }

  //! Returns children stream values
  const NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& Children() const { initItem(); return myChildren; }

  DEFINE_STANDARD_RTTIEXT (TreeModel_ItemProperties, Standard_Transient)

protected:
  //! Initializes the current item. It creates a backup of the specific item information
  Standard_EXPORT void initItem() const;

private:
  TreeModel_ItemBasePtr myItem; //!< current item

  TCollection_AsciiString myKey; //!< the item key
  Standard_DumpValue myStreamValue; //!< the stream value
  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue> myChildren; //!< the children
  NCollection_IndexedDataMap<Standard_Integer, TreeModel_RowValue> myRowValues; //!< the values
};

#endif