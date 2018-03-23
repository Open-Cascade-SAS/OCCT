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

#ifndef VInspector_ItemSelectMgrFilter_H
#define VInspector_ItemSelectMgrFilter_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <SelectMgr_Filter.hxx>

class QItemSelectionModel;

class VInspector_ItemSelectMgrFilter;
typedef QExplicitlySharedDataPointer<VInspector_ItemSelectMgrFilter> VInspector_ItemSelectMgrFilterPtr;

//! \class VInspector_ItemSelectMgrFilter
//! Item presents information about SelectMgr_Filter.
//! Parent is item folder, children are sub filter if the filter is a composition filter.
class VInspector_ItemSelectMgrFilter : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemSelectMgrFilterPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemSelectMgrFilterPtr (new VInspector_ItemSelectMgrFilter (theParent, theRow, theColumn)); }
  //! Destructor
  virtual ~VInspector_ItemSelectMgrFilter() Standard_OVERRIDE {};

  //! Returns the current filter, init item if it was not initialized yet
  //! \return filter object
  Standard_EXPORT Handle(SelectMgr_Filter) GetFilter() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of item selected
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Set filter into the current item
  //! \param theFilter a filter
  void setFilter (Handle(SelectMgr_Filter) theFilter) { myFilter = theFilter; }

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemSelectMgrFilter (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase (theParent, theRow, theColumn) {}

protected:

  Handle(SelectMgr_Filter) myFilter; //!< the current filter
};

#endif
