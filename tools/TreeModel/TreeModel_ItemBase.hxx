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

#ifndef TreeModel_ItemBase_H
#define TreeModel_ItemBase_H

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Handle.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

#include <inspector/TreeModel_ItemRole.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QExplicitlySharedDataPointer>
#include <QHash>
#include <QMap>
#include <QModelIndex>
#include <QObject>
#include <QPair>
#include <QSharedData>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class TreeModel_ItemBase;
class TreeModel_ItemProperties;

typedef QExplicitlySharedDataPointer<TreeModel_ItemBase> TreeModel_ItemBasePtr;

//! \class TreeModel_ItemBase
//! \brief Declaration of an abstract interface of model item.
//!
//! The TreeModel_ItemBase class defines the standard interface that model items must use
//! to be able to provide the model information in the tree view architecture.
//! It is not supposed to be instantiated directly. Instead, you should subclass it to
//! create new items.
//!
//! The goal of the item is to be an data container of a custom model, based on the
//! QAbstractItemModel. It provides the items architecture in order to realize the model
//! functionality to find a parent model index by a child index and vice versa.
//!
//! The item should be created by the model and is saved in the internal pointer of the
//! QModelIndex. Only model knows when the index is removed/created. By this cause,
//! the item is wrapped in the QExplicitlySharedDataPointer. It is a counter on the pointer
//! and if there is no index that refers to the item, it is removed automatically. So,
//! there is no necessity to remove the item manually.
//!
//! The item knows a pointer to the parent item and its position into.
//! Some methods of the item should be realized to fill the item content.
//! These are: the children count, a child creation and a child data.
//!
//! The best way of the item using is to request the content of the item from some
//! information object without the caching it. But it can be very expensive realisation,
//! because method data, for example, is called by the viewer repaint, in other words,
//! constantly.
//!
//! It is possible to cache some information in the item. Do not give it through the item
//! constructor. Realize method Init() to save the values in the item internal fields.
//! If the information model is changed, call Reset() for this item, or the item's parent.
//! It leads the item to non initialized state and by the next get of the item content,
//! call Init() method to fulfill the item content again.
class TreeModel_ItemBase : public QSharedData
{
public:

  //! Destructor
  virtual ~TreeModel_ItemBase() {}

  //! Gets whether the item is already initialized.The initialized state is thrown down
  //! by the reset method and get back after the method Init().
  //!  \return if the item is initialized
  bool IsInitialized() const { return m_bInitialized; }

  //! Sets the item internal initialized state to the true. If the item has internal values,
  //! there should be initialized here.
  Standard_EXPORT virtual void Init();

  //! Returns data object of the item.
  //! \return object
  Standard_EXPORT virtual const Handle(Standard_Transient)& Object() const;

  //! Resets the item and the child items content. Sets the initialized state to false.
  //! If the item has internal values, they should be reset here.
  Standard_EXPORT virtual void Reset();

  //! Resets the item cached value for the parameter role.
  //! \param theRole an item role
  Standard_EXPORT virtual void Reset(int theRole);

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  const Standard_SStream& Stream() const { return myStream; }

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  virtual bool SetStream (const Standard_SStream& theSStream, Standard_Integer& theStartPos,
                          Standard_Integer& theLastPos) const
  { (void)theSStream; (void)theStartPos; (void)theLastPos; return false; }

  //! Gets the parent of the item, or TreeModel_ItemBasePtr() if it has no parent.
  //! \return pointer to the item
  TreeModel_ItemBasePtr Parent() const { return m_pParent; };

  //! Gets the row of the item in the parent
  //! \return the row position
  int Row() const { return m_iRow; }

  //! Gets the column of the item in the parent
  //! \return the column position
  int Column() const { return m_iColumn; }

  //! Gets a child tree item in the given position. Find an item in the children hash.
  //! Creates a new child item, if there is no a cached item in the given position and
  //! if the flag about the creation is true.
  //! \param theRow the row of the child item
  //! \param theColumn the column of the child item
  //! \param isToCreate the flag whether the item should be created if it is not created yet
  //! \return the child item or TreeModel_ItemBasePtr() if it does not exist
  Standard_EXPORT TreeModel_ItemBasePtr Child (int theRow, int theColumn, const bool isToCreate = true);

  //! Sets a custom value for the role in an internal cache
  //! \param theValue a value
  //! \param theRole a value role
  void SetCustomData(const QVariant& theValue, int theRole) { myCachedValues.insert (theRole, theValue); }

  //! Returns the data stored under the given role for the current item
  //! \param theIndex the item model index
  //! \param theRole the item model role
  virtual QVariant data (const QModelIndex& theIndex, int theRole = Qt::DisplayRole) const
  { (void)theIndex; return cachedValue(theRole); }

  //! Returns number of rows where the children are
  //! \return the row count
  int rowCount() const { return cachedValue(TreeModel_ItemRole_RowCountRole).toInt(); }

  //! Returns the item properties
  const Handle(TreeModel_ItemProperties)& Properties() const { return myProperties; }

  //! Updates item by the item properties value
  virtual void StoreItemProperties (const int theRow, const int theColumn, const QVariant& theValue)
    { (void)theRow, (void)theColumn; (void)theValue; }

  //! Returns presentation of the item to be visualized in the view
  //! \thePresentations [out] container of presentation handles
  Standard_EXPORT virtual void Presentations (NCollection_List<Handle(Standard_Transient)>& thePresentations);
protected:

  //! \param theParent the parent item
  //! \param theRow the item row position in the parent item
  //! \param theColumn the item column position in the parent item
  Standard_EXPORT TreeModel_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

  //! Initializes the current item. It creates a backup of the specific item information
  virtual void initItem() const {}

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn)
  { (void)theRow; (void)theColumn; return TreeModel_ItemBasePtr(); }

  //! Wraps the current item by shared pointer
  //! \return the shared pointer to the current item
  Standard_EXPORT const TreeModel_ItemBasePtr currentItem();

  //! Returns the cached value for the role. Init the value if it is requested the first time
  //! By default, it calls initRowCount(TreeModel_ItemRole_RowCountRole) or initValue for the item role
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT QVariant cachedValue (const int theItemRole) const;

  //! \return number of children. It should be reimplemented in child
  virtual int initRowCount() const = 0;

  //! \return number of children. It should be reimplemented in child
  Standard_EXPORT int initStreamRowCount();

  //! Returns data value for the role. It should be reimplemented in child
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  virtual void initStream (Standard_OStream& theOStream) const { (void)theOStream; }

protected:
  Handle(TreeModel_ItemProperties) myProperties; //!< the properties
  int m_iStreamChildren; //!< the count of stream items
  Standard_SStream myStream; //!< stream value

private:

  typedef QHash< QPair<int, int>, TreeModel_ItemBasePtr > PositionToItemHash;
  PositionToItemHash m_ChildItems; //!< the hash of item children

  mutable QMap<int, QVariant> myCachedValues; //!< cached values, should be cleared by reset
  TreeModel_ItemBasePtr m_pParent; //!< the parent item
  int m_iRow;          //!< the item row position in the parent item
  int m_iColumn;       //!< the item column position in the parent item
  bool m_bInitialized; //!< the state whether the item content is already initialized
};

//! Returns an explicitly shared pointer to the pointer held by other, using a
//! dynamic cast to type X to obtain an internal pointer of the appropriate type.
//! If the dynamic_cast fails, the object returned will be null.
//! Example of using: 
//! TreeModel_ItemBase* aParent;
//! TreeModel_CustomItemPtr aParentItem = itemDynamicCast<TreeModel_CustomItem>(aParent);
//! \param theItem a source item
//! \return a converted item
template <class X, class T> QExplicitlySharedDataPointer<X> itemDynamicCast (const QExplicitlySharedDataPointer<T>& theItem)
{
  X* ptr = dynamic_cast<X*> (theItem.data());

  QExplicitlySharedDataPointer<X> result;
  result = ptr;

  return result;
}

#endif
