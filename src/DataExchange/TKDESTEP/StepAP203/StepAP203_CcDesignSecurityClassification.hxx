// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepAP203_CcDesignSecurityClassification_HeaderFile
#define _StepAP203_CcDesignSecurityClassification_HeaderFile

#include <Standard.hxx>

#include <StepAP203_ClassifiedItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_SecurityClassificationAssignment.hxx>
class StepBasic_SecurityClassification;

//! Representation of STEP entity CcDesignSecurityClassification
class StepAP203_CcDesignSecurityClassification : public StepBasic_SecurityClassificationAssignment
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignSecurityClassification();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepBasic_SecurityClassification>&
      aSecurityClassificationAssignment_AssignedSecurityClassification,
    const occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>>& aItems);

  //! Returns field Items
  Standard_EXPORT occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>> Items() const;

  //! Set field Items
  Standard_EXPORT void SetItems(
    const occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>>& Items);

  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignSecurityClassification,
                          StepBasic_SecurityClassificationAssignment)

private:
  occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>> theItems;
};

#endif // _StepAP203_CcDesignSecurityClassification_HeaderFile
