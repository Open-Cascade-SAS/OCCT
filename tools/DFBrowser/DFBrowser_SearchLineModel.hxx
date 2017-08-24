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

#ifndef DFBrowser_SearchLineModel_H
#define DFBrowser_SearchLineModel_H

#include <inspector/DFBrowser_SearchItemInfo.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <QVariant>

class QObject;
class DFBrowser_Module;

//! \class DFBrowser_SearchLineModel
//! Table view model filled by OCAF labels and attributes. It contain information in the next form:
//! <document index> into <document values container>
//! <document values container> is key(attribute name or label entry) into item information
//! The model contains 3 columns:
//! - 0: information text
//! - 1: icon size is [80, 80]
//! - 2: information text that contains united path
class DFBrowser_SearchLineModel : public QAbstractTableModel
{
public:

  //! Constructor
  Standard_EXPORT DFBrowser_SearchLineModel (QObject* theParent, DFBrowser_Module* theModule);

  //! Destructor
  virtual ~DFBrowser_SearchLineModel() Standard_OVERRIDE {}

  //! Separator as attribute name is divided from the label entry in information text
  static QString SplitSeparator() { return ": "; }

  //! Returns the current module
  DFBrowser_Module* GetModule() const { return myModule; }

  //! Fills internal containers by the OCAF values. Updates number of rows value
  //! \param theDocumentValues container of document index to container of entry/attribute name to item information
  //! \param theDocumentInfoValues container of a document index to entry/attribute name
  Standard_EXPORT void SetValues (const QMap<int, QMap<QString, DFBrowser_SearchItemInfo > >& theDocumentValues,
                                  const QMap<int, QStringList>& theDocumentInfoValues);

  //! Clears cache of values
  Standard_EXPORT void ClearValues();

  //! Returns path to attribute or label element of OCAF by the index.
  //! \param theIndex a tree model index
  Standard_EXPORT QStringList GetPath (const QModelIndex& theIndex) const;

  //! Returns value(attribute name or label entry) of OCAF by the index
  Standard_EXPORT QString GetValue (const QModelIndex& theIndex) const;

  //! Emits the layoutChanged signal from outside of this class
  Standard_EXPORT void EmitLayoutChanged();

  //! Creates new model index
  //! \param theRow the index row position
  //! \param theColummn the index column position
  //! \param theParent the parent index
  //! \return the model index
  Standard_EXPORT virtual QModelIndex index (int theRow, int theColumn,
                                             const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns the data stored under the given role for the item referred to by the index.
  //! \param theIndex a model index
  //! \param theRole an enumeration value of role for data obtaining

  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE;
  //! Returns the number of rows under the given parent.
  //! \param theParent a parent model index
  //! \return the number of rows

  virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myRowCount; }
  //! Returns the number of columns for the children of the given parent.
  //! \param theParent a parent model index
  //! \return the number of columns

  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 3; }

protected:

  //! Returns document index by a row index in table model
  //! \param theRow a row index of a QModelIndex
  //! \param theRowInDocument an output index, to obtain information from myDocumentInfoValues
  int getDocumentId (const int theRow, int& theRowInDocument) const;

private:

  DFBrowser_Module* myModule; //!< the current module
   //! a document index to container of entry/attribute name to item information
  QMap<int, QMap<QString, DFBrowser_SearchItemInfo> > myAdditionalValues;
  QMap<int, QStringList> myDocumentInfoValues; //!< a document index to entry/attribute name
  int myRowCount; //!< number of rows in the model: summ of row counts in all documents
};
#endif
