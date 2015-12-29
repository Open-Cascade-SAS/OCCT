// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepAP214_AutoDesignSecurityClassificationAssignment_HeaderFile
#define _StepAP214_AutoDesignSecurityClassificationAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_HArray1OfApproval.hxx>
#include <StepBasic_SecurityClassificationAssignment.hxx>
#include <Standard_Integer.hxx>
class StepBasic_SecurityClassification;
class StepBasic_Approval;


class StepAP214_AutoDesignSecurityClassificationAssignment;
DEFINE_STANDARD_HANDLE(StepAP214_AutoDesignSecurityClassificationAssignment, StepBasic_SecurityClassificationAssignment)


class StepAP214_AutoDesignSecurityClassificationAssignment : public StepBasic_SecurityClassificationAssignment
{

public:

  
  //! Returns a AutoDesignSecurityClassificationAssignment
  Standard_EXPORT StepAP214_AutoDesignSecurityClassificationAssignment();
  
  Standard_EXPORT void Init (const Handle(StepBasic_SecurityClassification)& aAssignedSecurityClassification, const Handle(StepBasic_HArray1OfApproval)& aItems);
  
  Standard_EXPORT void SetItems (const Handle(StepBasic_HArray1OfApproval)& aItems);
  
  Standard_EXPORT Handle(StepBasic_HArray1OfApproval) Items() const;
  
  Standard_EXPORT Handle(StepBasic_Approval) ItemsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbItems() const;




  DEFINE_STANDARD_RTTIEXT(StepAP214_AutoDesignSecurityClassificationAssignment,StepBasic_SecurityClassificationAssignment)

protected:




private:


  Handle(StepBasic_HArray1OfApproval) items;


};







#endif // _StepAP214_AutoDesignSecurityClassificationAssignment_HeaderFile
