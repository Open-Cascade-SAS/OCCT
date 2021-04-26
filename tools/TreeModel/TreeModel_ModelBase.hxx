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
#include <inspector/TreeModel_HeaderSection.hxx>

#include <NCollection_List.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QAbstractItemModel>
#include <QExplicitlySharedDataPointer>
#include <QMap>
#include <QModelIndex>
#include <QVariant>
#include <QVector>
#include <Standard_WarningsRestore.hxx>

class TreeModel_VisibilityState;

//! \class TreeModel_ModelBase
//! \brief Implementation of the tree item based model of QAbstractItemModel.
//! The TreeModel_ModelBase class defines the abstract model realization through the base item architecture.
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

  //! Creates model columns and root items.
  //! Default columns are: [0] - Name, [1] - Visibility, [2] - Row
  Standard_EXPORT virtual void InitColumns();

  //! Returns the item shared pointer by the model index
  //! if it is in the index internal pointer
  //! @param theIndex a model index
  Standard_EXPORT static TreeModel_ItemBasePtr GetItemByIndex (const QModelIndex& theIndex);

  //! Resets the model items content. Calls the same method of the root item.
  //! It leads to reset of all child/sub child items.
  Standard_EXPORT virtual void Reset();

  //! Returns the model root item.
  //! It is realized for OCAFBrowser
  TreeModel_ItemBasePtr RootItem (const int theColumn) const { return myRootItems[theColumn]; }

  //! Emits the layoutChanged signal from outside of this class
  Standard_EXPORT void EmitLayoutChanged();

  //! Emits the dataChanged signal from outside of this class
  Standard_EXPORT void EmitDataChanged (const QModelIndex& theTopLeft, const QModelIndex& theBottomRight,
                                        const QVector<int>& theRoles = QVector<int>(), const bool isResetItem = true);

  //! Sets state whether visibility column (0) is used in the model
  //! \param theState state
  void SetUseVisibilityColumn (const bool theState) { m_pUseVisibilityColumn = theState; }

  //! Returns state whether visibility column (0) is used in the model
  //! \param theState state
  bool IsUseVisibilityColumn() const { return m_pUseVisibilityColumn; }

  //! Fills visibility state checker
  //! \param theController the checker interface
  void SetVisibilityState (TreeModel_VisibilityState* theController) { myVisibilityState = theController; }

  //! Returns visibility state checker
  //! \return the checker interface
  TreeModel_VisibilityState* GetVisibilityState () const { return myVisibilityState; }

  //! Returns true if the tree view model contains highlighted items. This highlight is set manually.
  bool HasHighlighted() { return !myHighlightedIndices.isEmpty(); }

  //! Sets items of the indices highlighted in the model.
  //! \param theIndices a list of tree model indices
  void SetHighlighted (const QModelIndexList& theIndices = QModelIndexList()) { myHighlightedIndices = theIndices; }

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
                                               int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning
  //! the number of children of parent.
  //! \param theParent a parent model index
  //! \return the number of rows
  Standard_EXPORT virtual int rowCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE;

  //! Returns header item, that can be modified
  //! \param theColumnId a column index
  //! \return header section value
  TreeModel_HeaderSection* ChangeHeaderItem (const int theColumnId) { return &myHeaderValues[theColumnId]; }

  //! Returns count of columns in the model
  //! \param theParent an index of the parent item
  //! \return integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return myHeaderValues.size(); }

  //! Returns default value of the visibility column
  //! \return integer value
  static int ColumnVisibilityWidth() { return 20; }

  //! Returns selected items in the cell of given orientation.
  //! \param theIndices a container of selected indices
  //! \param theCellId column index if orientation is horizontal, row index otherwise
  //! \param theOrientation an orientation to apply the cell index
  //! \return model indices from the list
  Standard_EXPORT static QModelIndexList Selected (const QModelIndexList& theIndices, const int theCellId,
                                                   const Qt::Orientation theOrientation = Qt::Horizontal);

  //! Returns single selected item in the cell of given orientation. If the orientation is Horizontal,
  //! in the cell id column, one row should be selected.
  //! \param theIndices a container of selected indices
  //! \param theCellId column index if orientation is horizontal, row index otherwise
  //! \param theOrientation an orientation to apply the cell index
  //! \return model index from the list
  Standard_EXPORT static QModelIndex SingleSelected (const QModelIndexList& theIndices, const int theCellId,
                                                     const Qt::Orientation theOrientation = Qt::Horizontal);

  //! Returns selected tree model items for indices.
  //! \param theIndices a container of selected indices
  //! \return model items from the list
  Standard_EXPORT static QList<TreeModel_ItemBasePtr> SelectedItems (const QModelIndexList& theIndices);

  //! Returns presentations of sub items
  //! \param theIndices a container of selected indices
  //! \thePresentations [out] container of presentations
  Standard_EXPORT static void SubItemsPresentations (const QModelIndexList& theIndices,
                                                     NCollection_List<Handle(Standard_Transient)>& thePresentations);

protected:
  //! Creates root item
  //! \param theColumnId index of a column
  virtual TreeModel_ItemBasePtr createRootItem (const int theColumnId) = 0;

  //! Sets header properties item.
  //! \param theColumnId a column index
  //! \param theSection a section value
  Standard_EXPORT void setHeaderItem (const int theColumnId, const TreeModel_HeaderSection& theSection);

  //! Converts the item shared pointer to void* type
  //! \param theItem
  //!  \return an item pointer
  Standard_EXPORT static void* getIndexValue (const TreeModel_ItemBasePtr& theItem);

  //! Returns presentations of sub items. Recursive method to get presentations of all children
  //! \param theItem an item to get own presentations and presentations of children
  //! \thePresentations [out] container of presentations found
  static void subItemsPresentations (const TreeModel_ItemBasePtr& theItem,
                                    NCollection_List<Handle(Standard_Transient)>& thePresentations);

private:
  //! Creates root item
  //! \param theColumnId index of a column
  Standard_EXPORT void createRoot (const int theColumnId);

protected:

  QMap<int, TreeModel_ItemBasePtr> myRootItems; //!< container of root items, for each column own root item
  QMap<int, TreeModel_HeaderSection> myHeaderValues; //!< header values
  //!< model subclass. The model is fulfilled by this item content

  bool m_pUseVisibilityColumn; //!< the state whether column=0 is reserved for Visibility state
  TreeModel_VisibilityState* myVisibilityState; //!< the interface of item visibility
  QIcon myVisibleIcon; //!< icon of visible state
  QIcon myInvisibleIcon; //!< icon of invisible state

  QModelIndexList myHighlightedIndices; //!< tree model indices that should be visualized as highlighted
};

#endif
