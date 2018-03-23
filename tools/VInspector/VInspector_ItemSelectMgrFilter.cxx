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

#include <inspector/VInspector_ItemSelectMgrFilter.hxx>
#include <inspector/VInspector_ItemFolderObject.hxx>
#include <inspector/VInspector_Tools.hxx>

#include <SelectMgr_AndFilter.hxx>
#include <SelectMgr_OrFilter.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QIcon>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant VInspector_ItemSelectMgrFilter::initValue (int theItemRole) const
{
  if (theItemRole == Qt::DisplayRole || theItemRole == Qt::ToolTipRole)
  {
    Handle(SelectMgr_Filter) aFilter = GetFilter();
    switch (Column())
    {
      case 0: return theItemRole == Qt::ToolTipRole ? QVariant ("")
                                                    : QVariant (aFilter->DynamicType()->Name());
      case 1: return rowCount() > 0 ? QVariant (rowCount()) : QVariant();
      case 2: return VInspector_Tools::GetPointerInfo (aFilter, true).ToCString();
      default: break;
    }
  }
  if (Column() == 0 && theItemRole == Qt::DecorationRole)
  {
    Handle(SelectMgr_Filter) aFilter = GetFilter();
    if (aFilter->IsKind (STANDARD_TYPE (SelectMgr_AndFilter))) return QIcon (":/icons/treeview_and.png");
    if (aFilter->IsKind (STANDARD_TYPE (SelectMgr_OrFilter))) return QIcon (":/icons/treeview_or.png");
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int VInspector_ItemSelectMgrFilter::initRowCount() const
{
  Handle(SelectMgr_Filter) aFilter = GetFilter();
  if (aFilter->IsKind (STANDARD_TYPE (SelectMgr_CompositionFilter)))
  {
    Handle(SelectMgr_CompositionFilter) aCompositionFilter = Handle(SelectMgr_CompositionFilter)::DownCast (aFilter);
    return aCompositionFilter->StoredFilters().Size();
  }
  return 0;
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ItemSelectMgrFilter::createChild (int theRow, int theColumn)
{
  return VInspector_ItemSelectMgrFilter::CreateItem(currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrFilter::Init()
{
  Handle(AIS_InteractiveContext) aContext = GetContext();

  Handle(SelectMgr_Filter) aFilter;

  Handle(SelectMgr_CompositionFilter) aParentCompositeFilter;
  VInspector_ItemSelectMgrFilterPtr aParentSelectMgrItem = itemDynamicCast<VInspector_ItemSelectMgrFilter> (Parent());
  if (aParentSelectMgrItem)
    aParentCompositeFilter = Handle(SelectMgr_CompositionFilter)::DownCast (aParentSelectMgrItem->GetFilter());

  int aRowId = Row();
  const SelectMgr_ListOfFilter& aFilters = !aParentCompositeFilter.IsNull() ? aParentCompositeFilter->StoredFilters()
    : aContext->Filters();
  int aCurrentIndex = 0;
  for (SelectMgr_ListOfFilter::Iterator aFiltersIt (aFilters); aFiltersIt.More(); aFiltersIt.Next(), aCurrentIndex++)
  {
    if (aCurrentIndex != aRowId)
      continue;
    aFilter = aFiltersIt.Value();
    break;
  }
  setFilter (aFilter);
  TreeModel_ItemBase::Init(); // to use getFilter() without circling initialization
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrFilter::Reset()
{
  VInspector_ItemBase::Reset();

  setFilter (NULL);
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void VInspector_ItemSelectMgrFilter::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<VInspector_ItemSelectMgrFilter*>(this)->Init();
}

// =======================================================================
// function : GetInteractiveObject
// purpose :
// =======================================================================
Handle(SelectMgr_Filter) VInspector_ItemSelectMgrFilter::GetFilter() const
{
  initItem();
  return myFilter;
}
