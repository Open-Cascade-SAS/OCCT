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

#ifndef VInspector_ItemSelection_H
#define VInspector_ItemSelection_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <SelectMgr_Selection.hxx>

class VInspector_ItemSelection;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelection> VInspector_ItemSelectionPtr;

//! \class VInspector_ItemSelection
//! Item about SelectMgr_Selection.
//! Parent is presentable object item, children are sensitive entity items 
class VInspector_ItemSelection : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectionPtr CreateItem(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectionPtr (new VInspector_ItemSelection (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemSelection() {};

  //! \return current selection value
  Standard_EXPORT Handle(SelectMgr_Selection) getSelection() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initializes the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Initializes number of children
  //! \return integer value
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
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
  VInspector_ItemSelection(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

private:

  Handle(SelectMgr_Selection) mySelection; //!< the current selection
};

#endif
