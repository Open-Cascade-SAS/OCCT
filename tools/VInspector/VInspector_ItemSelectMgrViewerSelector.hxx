// Created on: 2020-02-10
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

#ifndef VInspector_ItemSelectMgrViewerSelector_H
#define VInspector_ItemSelectMgrViewerSelector_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <SelectMgr_ViewerSelector.hxx>

class VInspector_ItemSelectMgrViewerSelector;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrViewerSelector>
  VInspector_ItemSelectMgrViewerSelectorPtr;

//! \class VInspector_ItemSelectMgrViewerSelector
//! Parent item is context properties, that corresponds to SelectMgr_ViewerSelector
class VInspector_ItemSelectMgrViewerSelector : public VInspector_ItemBase
{
public:
  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrViewerSelectorPtr CreateItem(TreeModel_ItemBasePtr theParent,
                                                              const int             theRow,
                                                              const int             theColumn)
  {
    return VInspector_ItemSelectMgrViewerSelectorPtr(
      new VInspector_ItemSelectMgrViewerSelector(theParent, theRow, theColumn));
  }

  //! Destructor
  virtual ~VInspector_ItemSelectMgrViewerSelector() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual const Handle(Standard_Transient)& Object() const Standard_OVERRIDE
  {
    initItem();
    return myViewerSelector;
  }

  //! Returns the current viewer, init item if it was not initialized yet
  //! \return interactive object
  Handle(SelectMgr_ViewerSelector) GetViewerSelector() const
  {
    return Handle(SelectMgr_ViewerSelector)::DownCast(Object());
  }

protected:
  //! Initializes the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue(const int theItemRole) const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream(Standard_OStream& theOStream) const Standard_OVERRIDE;

private:
  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemSelectMgrViewerSelector(TreeModel_ItemBasePtr theParent,
                                         const int             theRow,
                                         const int             theColumn)
      : VInspector_ItemBase(theParent, theRow, theColumn)
  {
  }

protected:
  Handle(SelectMgr_ViewerSelector) myViewerSelector; //!< the current viewer selector
};

#endif
