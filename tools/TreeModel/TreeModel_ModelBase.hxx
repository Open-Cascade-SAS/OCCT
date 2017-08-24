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

#ifndef TreeModel_ModelBase_H
#define TreeModel_ModelBase_H

#include <Standard.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

#include <QAbstractItemModel>
#include <QExplicitlySharedDataPointer>
#include <QModelIndex>
#include <QVariant>

//! \class TreeModel_ModelBase
//! \brief Implementation of the tree item based model of QAbstractItemModel.
//! The TreeModel_ModelBase class defines the abstract model realization throught the base item architecture.
//! By the model index creation, a base item is created and attached to the index.
//! Each item contains an iformation about the item parent, position in the parent and
//! the item's children. So, it is possible to get the model index relation from the item.
class TreeModel_ModelBase : public QAbstractItemModel
{
public:

  //! Constructor
  //! \param theParent the parent object
  Standard_EXPORT TreeModel_ModelBase (QObject* theParent = 0);

  //! Destructor
  virtual ~TreeModel_ModelBase() {}

  //! Returns the item shared pointer by the model index
  //! if it is in the index internal pointer
  //! @param theIndex a model index
  Standard_EXPORT static TreeModel_ItemBasePtr GetItemByIndex (const QModelIndex& theIndex);

  //! Resets the model items content. Calls the same method of the root item.
  //! It leads to reset of all child/sub child items.
  Standard_EXPORT virtual void Reset();

  //! Returns the model root item.
  //! It is realized for OCAFBrowser
  virtual TreeModel_ItemBasePtr RootItem (const int theColumn) const { (void)theColumn; return m_pRootItem; }

  //! Emits the layoutChanged signal from outside of this class
  Standard_EXPORT void EmitLayoutChanged();

  //! Returns the index of the item in the model specified by the given row, column and parent index.
  //! Saves an internal pointer at the createIndex. This pointer is a shared pointer to the class,
  //! that realizes a base item interface. If the parent is invalid, a root item is used, otherwise a new item
  //! is created by the pointer item saved the parent model index
  //! \param theRow the index row position
  //! \param theColummn the index column position
  //! \param theParent the parent index
  //! \return the model index
  Standard_EXPORT virtual QModelIndex index (int theRow, int theColumn,
                                             const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns the data stored under the given role for the item referred to by the index.
  //! \param theIndex a model index
  //! \param theRole an enumeration value of role for data obtaining
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns the parent index by the child index. Founds the item, saved in the index;
  //! obtains the parent item by the item. Create a new index by the item and containing it.
  //! \param theIndex a model index
  Standard_EXPORT virtual QModelIndex parent (const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Returns the item flags for the given index. The base class implementation returns a combination of flags that
  //! enables the item (ItemIsEnabled) and allows it to be selected (ItemIsSelectable)
  //! \param theIndex the model index
  //! \return Qt flag combination
  Standard_EXPORT virtual Qt::ItemFlags flags (const QModelIndex& theIndex) const Standard_OVERRIDE;

  //! Returns the header data for the given role and section in the header with the specified orientation.
  //! \param theSection the header section. For horizontal headers - column number, for vertical headers - row number.
  //! \param theOrientation a header orientation
  //! \param theRole a data role
  //! \return the header data
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole) const Standard_OVERRIDE
  { (void)theSection, (void)theOrientation; (void)theRole; return QVariant(); }

  //! Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning
  //! the number of children of parent.
  //! \param theParent a parent model index
  //! \return the number of rows
  Standard_EXPORT virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns the number of columns for the children of the given parent.
  //! \param theParent a parent model index
  //! \return the number of columns
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 1; }

protected:

  //! Converts the item shared pointer to void* type
  //! \param theItem
  //!  \return an item pointer
  Standard_EXPORT static void* getIndexValue (const TreeModel_ItemBasePtr& theItem);

protected:

  TreeModel_ItemBasePtr m_pRootItem; //!< the model root item. It should be created in the
  //!< model subclass. The model is fulfilled by this item content
};

#endif
