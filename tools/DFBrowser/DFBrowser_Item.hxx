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

#ifndef DFBrowser_Item_H
#define DFBrowser_Item_H

#include <inspector/DFBrowser_ItemBase.hxx>

#include <TDF_Attribute.hxx>
#include <Standard.hxx>
#include <Standard_GUID.hxx>

#include <QMap>
#include <QList>
#include <QVariant>

class DFBrowser_Item;
typedef QExplicitlySharedDataPointer<DFBrowser_Item> DFBrowser_ItemPtr;

//! \class DFBrowser_Item
//! \brief Declaration of the tree model root item.
class DFBrowser_Item : public DFBrowser_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  //! \return the pointer to the created item
  static DFBrowser_ItemPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return DFBrowser_ItemPtr (new DFBrowser_Item (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~DFBrowser_Item() Standard_OVERRIDE {};

  //! \return true if the attribute is set in the item, otherwise it is initialized by a label
  Standard_EXPORT bool HasAttribute() const;

  //! \return the item attribute
  Standard_EXPORT Handle(TDF_Attribute) GetAttribute() const;

  //! Finds int parent item attribute or label by this item row and store it in a field of this item.
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets the cached item values, set null attribute and calls reset of the parent class
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Init item and calls cachedValue() for the role
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT QVariant GetAttributeInfo(int theRole) const;

protected:

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Return data value for the role:
  //! - if content is label, calls DFBrowser_ItemBase;
  //! - if content is attribute, if the fole is extended display, asks additional info text or ask module about info
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  DFBrowser_Item (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : DFBrowser_ItemBase (theParent, theRow, theColumn) {}

  //! Initialize the current item. It creates a backup of the specific item information
  void initItem() const;

  //! Sets the item attribute
  //! \param theAttribute an item attribute
  void SetAttribute (Handle(TDF_Attribute) theAttribute);

private:

  Standard_GUID myAttributeGUID; //!< if attribute, stores GUID of the attibute because label can contain only one attribute of a kind
};

#endif
