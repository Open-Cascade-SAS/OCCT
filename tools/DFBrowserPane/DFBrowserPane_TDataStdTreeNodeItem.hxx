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

#ifndef DFBrowserPane_TDataStdTreeNodeItem_H
#define DFBrowserPane_TDataStdTreeNodeItem_H

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard.hxx>
#include <TDF_Attribute.hxx>

#include <QList>
#include <QMap>
#include <QVariant>

class DFBrowserPane_TDataStdTreeNodeItem;
typedef QExplicitlySharedDataPointer<DFBrowserPane_TDataStdTreeNodeItem> DFBrowserPane_TDataStdTreeNodeItemPtr;

//! \class DFBrowserPane_TDataStdTreeNodeItem
//! An item connected to TDataStd_TreeNode attribute. Parent is NULL or tree node item.
//! Childrens are items for children of tree node attribute. 
class DFBrowserPane_TDataStdTreeNodeItem : public TreeModel_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  //! \return the pointer to the created item
  static DFBrowserPane_TDataStdTreeNodeItemPtr CreateItem (TreeModel_ItemBasePtr theParent,
                                                           const int theRow, const int theColumn)
  { return DFBrowserPane_TDataStdTreeNodeItemPtr (new DFBrowserPane_TDataStdTreeNodeItem (theParent, theRow, theColumn)); }

  //!Destructor
  virtual ~DFBrowserPane_TDataStdTreeNodeItem() Standard_OVERRIDE {};

  //! Store a current attribute
  //! \param theAttribute an attribute
  void SetAttribute (const Handle(TDF_Attribute)& theAttribute) { myAttribute = theAttribute; }

  //! Returns the current attribute
  //! \return an attribute
  Handle(TDF_Attribute) GetAttribute () const { initItem(); return myAttribute; }

  //! Set state if the attribute is current(corresponds to the selected attribute in tree)
  //! \param theCurrent boolean state
  void setCurrentAttribute (const bool theCurrent) { Reset(); myIsCurrentItem = theCurrent; }

  //! Returns child attribute of the current attribute
  //! \param theChildRow an index of a child attribute
  //! \returns an attribute
  Standard_EXPORT Handle(TDF_Attribute) getChildAttribute (const int theChildRow) const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets the cached item values. Throws down the initialized state of the item.
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Returns the data stored under the given role for the current item.
  //! \param theRole an enumeration value of role for data obtaining
  virtual QVariant initValue (const int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE { return getRowCount(); }

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

protected:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  DFBrowserPane_TDataStdTreeNodeItem(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : TreeModel_ItemBase (theParent, theRow, theColumn), myIsCurrentItem (false) {}

  //! Initialize the current item. It creates a backup of the specific item information
  void initItem() const;

  //! Returns number of children attributes, initializes item is necessary
  int getRowCount() const;

  //! Returns entry of the label of the current attribute tree node
  QString getName() const { return myLabelName; }

private:

  Handle(TDF_Attribute) myAttribute; //! current attribute in tree node hierarchy
  bool myIsCurrentItem; //! state whether this attribute is active in DFBrowser selected attribute in tree

  int myRowCount; //! cached value of rows count
  QString myLabelName; //! cached value of label name of the label of the current tree node attribute
};

#endif