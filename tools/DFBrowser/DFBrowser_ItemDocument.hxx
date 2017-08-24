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

#ifndef DFBrowser_ItemDocument_H
#define DFBrowser_ItemDocument_H

#include <inspector/DFBrowser_ItemBase.hxx>

#include <Standard.hxx>
#include <TDocStd_Document.hxx>

class DFBrowser_ItemDocument;
typedef QExplicitlySharedDataPointer<DFBrowser_ItemDocument> DFBrowser_ItemDocumentPtr;

//! \class DFBrowser_ItemDocument
//! \brief Declaration of the tree model document item.
//! This item is connected to the OCAF document. Parent item is application, children are either labels or attributes
class DFBrowser_ItemDocument : public DFBrowser_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  //! \return the pointer to the created item
  static DFBrowser_ItemDocumentPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return DFBrowser_ItemDocumentPtr (new DFBrowser_ItemDocument (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~DFBrowser_ItemDocument() Standard_OVERRIDE {};

  //! Returns the current label
  //! \return a label
  Standard_EXPORT virtual TDF_Label GetLabel() const Standard_OVERRIDE;

  //! Sets the item document
  //! \param theLabel an object where the child items structure is found
  void setDocument (const Handle(TDocStd_Document)& theDocument) { myDocument = theDocument; }

  //! Returns true if the current document is not null
  //! \return a boolean value
  bool hasDocument() const { return !getDocument().IsNull(); }

  //! Returns the current document
  //! \return a label
  Standard_EXPORT const Handle(TDocStd_Document)& getDocument() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets the cached item values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //!
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

protected:

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  DFBrowser_ItemDocument(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : DFBrowser_ItemBase (theParent, theRow, theColumn) {}

private:

  Handle(TDocStd_Document) myDocument; //!< cached application document by the row index of the item
};

#endif
