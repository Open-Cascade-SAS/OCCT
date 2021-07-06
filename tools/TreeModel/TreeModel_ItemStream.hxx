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

#ifndef TreeModel_ItemStream_H
#define TreeModel_ItemStream_H

#include <Standard.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

class TreeModel_ItemProperties;
class TreeModel_ItemStream;

typedef QExplicitlySharedDataPointer<TreeModel_ItemStream> TreeModel_ItemStreamPtr;

//! \class TreeModel_ItemStream
//! Parent item, that corresponds to AIS_InteractiveContext
//! Children of the item are:
//! - "Property" item to show context attributes such as selection filters and drawer properties
//! - presentation items to show all interactive elements displayed/erased in the context
class TreeModel_ItemStream : public TreeModel_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static TreeModel_ItemStreamPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return TreeModel_ItemStreamPtr (new TreeModel_ItemStream (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~TreeModel_ItemStream() {}

  //! Sets the item internal initialized state to the true. If the item has internal values,
  //! there should be initialized here.
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets the item and the child items content. Sets the initialized state to false.
  //! If the item has internal values, they should be reset here.
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE { initItem(); return 0; }

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Stores values of the item properties into the item object
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \param theValue the cell value
  Standard_EXPORT virtual void StoreItemProperties (const int theRow, const int theColumn, const QVariant& theValue) Standard_OVERRIDE;

protected:
  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

  //! Initializes the current item. It creates a backup of the specific item information
  //! Does nothing as context has been already set into item
  Standard_EXPORT virtual void initItem() const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  Standard_EXPORT virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! \param theParent a parent item
  //! \param theRow the item row position in the parent item
  //! \param theColumn the item column position in the parent item
  Standard_EXPORT TreeModel_ItemStream(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);
};

#endif
