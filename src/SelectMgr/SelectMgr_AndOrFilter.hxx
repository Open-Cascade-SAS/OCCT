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

#ifndef _SelectMgr_AndOrFilter_HeaderFile
#define _SelectMgr_AndOrFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_NMapOfTransient.hxx>
#include <Standard_Boolean.hxx>
#include <SelectMgr_CompositionFilter.hxx>
#include <SelectMgr_FilterType.hxx>

DEFINE_STANDARD_HANDLE(SelectMgr_AndOrFilter, SelectMgr_CompositionFilter)

//! A framework to define an OR or AND selection filter.
//! To use an AND selection filter call SetUseOrFilter with False parameter.
//! By default the OR selection filter is used.
class SelectMgr_AndOrFilter : public SelectMgr_CompositionFilter
{

public:

  //! Constructs an empty selection filter.
  Standard_EXPORT SelectMgr_AndOrFilter (const SelectMgr_FilterType theFilterType);

  //! Indicates that the selected Interactive Object passes the filter.
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& theObj) const Standard_OVERRIDE;

  //! Disable selection of specified objects.
  Standard_EXPORT void SetDisabledObjects (const Handle(Graphic3d_NMapOfTransient)& theObjects);

  //! @return a selection filter type (@sa SelectMgr_FilterType).
  SelectMgr_FilterType FilterType() const { return myFilterType; }

  //! Sets a selection filter type.
  //! SelectMgr_FilterType_OR selection filter is used be default.
  //! @param theFilterType the filter type.
  void SetFilterType (const SelectMgr_FilterType theFilterType) { myFilterType = theFilterType; }

  DEFINE_STANDARD_RTTIEXT(SelectMgr_AndOrFilter, SelectMgr_CompositionFilter)

private:

  Handle(Graphic3d_NMapOfTransient) myDisabledObjects; //!< disabled objects.
                                                       //!  Selection isn't applied to these objects.
  SelectMgr_FilterType myFilterType; //!< selection filter type. SelectMgr_TypeFilter_OR by default.
};

#endif // _SelectMgr_AndOrFilter_HeaderFile
