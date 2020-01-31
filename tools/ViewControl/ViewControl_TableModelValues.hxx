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

#ifndef ViewControl_TableModelValues_H
#define ViewControl_TableModelValues_H

#include <Standard.hxx>

#include <inspector/TreeModel_HeaderSection.hxx>
#include <inspector/TreeModel_ItemProperties.hxx>
#include <inspector/ViewControl_EditType.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>
#include <QModelIndexList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

//! \class ViewControl_TableModelValues
//! \brief This is an interace for ViewControl_TableModel to give real values of the model
//! It should be filled or redefined.
class ViewControl_TableModelValues
{
public:

  //! Constructor
  ViewControl_TableModelValues (const Qt::Orientation& theOrientation = Qt::Vertical) { SetOrientation (theOrientation); }

  //! Destructor
  virtual ~ViewControl_TableModelValues() {}

  //! Returns item table properties builder
  Handle(TreeModel_ItemProperties) Properties() const { return myProperties; }

  //! Sets item table properties builder
  void SetProperties (const Handle(TreeModel_ItemProperties)& theProperties) { myProperties = theProperties; }

  //! Sets direction of the values applying, whether it should be placed by rows or by columns
  //! \param theOrientation if horizontal, the values are applied by rows, otherwise by columns
  void SetOrientation (const Qt::Orientation& theOrientation) { myOrientation = theOrientation; }

  //! Fills the model header values for orientation.
  //! \param theValues a container of header text values
  //! \param theOrientation an orientation of header
  void SetHeaderValues (const QList<TreeModel_HeaderSection>& theValues, const Qt::Orientation theOrientation)
  { myHeaderValues.insert (theOrientation, theValues); }

  //! Returns whether the column is hidden by default
  //! \param theColumnId a column index
  //! \return header section values container
  TreeModel_HeaderSection HeaderItem (const Qt::Orientation theOrientation, const int theColumnId) const
  { return myHeaderValues.contains(theOrientation) ? myHeaderValues[theOrientation][theColumnId] : TreeModel_HeaderSection(); }

  //! Stores information about table view header visibility
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  void SetHeaderVisible (const Qt::Orientation theOrientation, const bool theVisibility)
  { myVisibleHeader.insert (theOrientation, theVisibility); }

  //! Stores information about table view header visibility
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  bool IsHeaderVisible (const Qt::Orientation theOrientation) const
  { return myVisibleHeader.contains(theOrientation) ? myVisibleHeader[theOrientation] : true; }

  //! Get default section size if defined
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  bool DefaultSectionSize (const Qt::Orientation theOrientation, int& theSectionSize)
  {
    theSectionSize = myDefaultSectionSize.contains (theOrientation) ? myDefaultSectionSize[theOrientation] : -1;
    return myDefaultSectionSize.contains (theOrientation);
  }

  //! Set default section size if defined
  //! \param theOrientation an orientation of header
  //! \param theVisibility if true, header is visible
  void SetDefaultSectionSize (const Qt::Orientation theOrientation, const int& theSectionSize)
  { myDefaultSectionSize.insert(theOrientation, theSectionSize); }

  //! Returns number of columns, size of header values
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int ColumnCount (const QModelIndex& theParent = QModelIndex()) const;

  //! Returns number of rows, depending on orientation: myColumnCount or size of values container
  //! \param theParent an index of the parent item
  //! \return an integer value
  Standard_EXPORT virtual int RowCount (const QModelIndex& theParent = QModelIndex()) const;

  //! Returns content of the model index for the given role, it is obtained from internal container of values
  //! It returns value only for DisplayRole.
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant Data (const int theRow, const int theColumn, int theRole = Qt::DisplayRole) const;

  //! Sets content of the model index for the given role, it is applied to internal container of values
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param theRole a view role
  //! \return true if the value is changed
  Standard_EXPORT virtual bool SetData (const int theRow, const int theColumn, const QVariant& theValue,
                                        int theRole = Qt::DisplayRole);

  //! Returns content of the model index for the given role, it is obtainer from internal container of header values
  //! It returns value only for DisplayRole.
  //! \param theSection an index of value in the container 
  //! \param theIndex a model index
  //! \param theRole a view role
  //! \return value intepreted depending on the given role
  Standard_EXPORT virtual QVariant HeaderData (int theSection, Qt::Orientation theOrientation, int theRole = Qt::DisplayRole) const;

  //! Returns flags for the item: ItemIsEnabled | Qt::ItemIsSelectable
  //! \param theIndex a model index
  //! \return flags
  Standard_EXPORT virtual Qt::ItemFlags Flags (const QModelIndex& theIndex) const;

  //! Returns type of edit control for the model index. By default, it is an empty control
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \return edit type
  Standard_EXPORT virtual ViewControl_EditType EditType (const int theRow, const int theColumn) const;

  //! Returns default color for editable cell
  //! \return color value
  static QColor EditCellColor() { return QColor (Qt::darkBlue); }

protected:
  //! Returns true if the header item is italic of the parameter index
  //! \param theRow a model index row
  //! \param theColumn a model index column
  //! \param boolean value
  bool isItalicHeader (const int theRow, const int theColumn) const;

protected:

  Qt::Orientation myOrientation; //!< orientation how the values should fill the current table view
  QMap<Qt::Orientation, QList<TreeModel_HeaderSection> > myHeaderValues; //!< table header values
  QMap<Qt::Orientation, bool> myVisibleHeader; //!< table header visibility
  QMap<Qt::Orientation, int> myDefaultSectionSize; //!< table section default size

  Handle(TreeModel_ItemProperties) myProperties; //!< item properties
};

#endif
