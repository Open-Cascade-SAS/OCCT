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

#ifndef DFBrowser_ItemApplication_H
#define DFBrowser_ItemApplication_H

#include <inspector/DFBrowser_ItemBase.hxx>

#include <TDocStd_Application.hxx>

class DFBrowser_ItemApplication;
typedef QExplicitlySharedDataPointer<DFBrowser_ItemApplication> DFBrowser_ItemApplicationPtr;

//! \class DFBrowser_ItemApplication
//! \brief Declaration of the tree model root item.
//! This item is connected to the main label of the document.
class DFBrowser_ItemApplication : public DFBrowser_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static DFBrowser_ItemApplicationPtr CreateItem (TreeModel_ItemBasePtr theParent)
  { return DFBrowser_ItemApplicationPtr (new DFBrowser_ItemApplication (theParent)); }

  //! Destructor
  virtual ~DFBrowser_ItemApplication() Standard_OVERRIDE {};

  //! Sets the item label
  //! \param theLabel an object where the child items structure is found
  void SetApplication (const Handle(TDocStd_Application)& theApplication) { myApplication = theApplication; }

  //! Returns the current label
  //! \return a label
  const Handle(TDocStd_Application)& GetApplication() const { return myApplication; }

protected:

  //! Returns number of documents if the application is not NULL
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns fixed item text or empty.
  //! \param theItemRole a value role
  //! \return the value
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
  DFBrowser_ItemApplication(TreeModel_ItemBasePtr theParent) : DFBrowser_ItemBase(theParent, 0, 0) {}

private:

  Handle(TDocStd_Application) myApplication; //!<! OCAF application
};

#endif
