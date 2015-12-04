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

#ifndef _StepAP203_CcDesignCertification_HeaderFile
#define _StepAP203_CcDesignCertification_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepAP203_HArray1OfCertifiedItem.hxx>
#include <StepBasic_CertificationAssignment.hxx>
class StepBasic_Certification;


class StepAP203_CcDesignCertification;
DEFINE_STANDARD_HANDLE(StepAP203_CcDesignCertification, StepBasic_CertificationAssignment)

//! Representation of STEP entity CcDesignCertification
class StepAP203_CcDesignCertification : public StepBasic_CertificationAssignment
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepAP203_CcDesignCertification();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepBasic_Certification)& aCertificationAssignment_AssignedCertification, const Handle(StepAP203_HArray1OfCertifiedItem)& aItems);
  
  //! Returns field Items
  Standard_EXPORT Handle(StepAP203_HArray1OfCertifiedItem) Items() const;
  
  //! Set field Items
  Standard_EXPORT void SetItems (const Handle(StepAP203_HArray1OfCertifiedItem)& Items);




  DEFINE_STANDARD_RTTIEXT(StepAP203_CcDesignCertification,StepBasic_CertificationAssignment)

protected:




private:


  Handle(StepAP203_HArray1OfCertifiedItem) theItems;


};







#endif // _StepAP203_CcDesignCertification_HeaderFile
