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

#ifndef _StepBasic_ApprovalRelationship_HeaderFile
#define _StepBasic_ApprovalRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepBasic_Approval;


class StepBasic_ApprovalRelationship;
DEFINE_STANDARD_HANDLE(StepBasic_ApprovalRelationship, Standard_Transient)


class StepBasic_ApprovalRelationship : public Standard_Transient
{

public:

  
  //! Returns a ApprovalRelationship
  Standard_EXPORT StepBasic_ApprovalRelationship();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepBasic_Approval)& aRelatingApproval, const Handle(StepBasic_Approval)& aRelatedApproval);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  Standard_EXPORT void SetRelatingApproval (const Handle(StepBasic_Approval)& aRelatingApproval);
  
  Standard_EXPORT Handle(StepBasic_Approval) RelatingApproval() const;
  
  Standard_EXPORT void SetRelatedApproval (const Handle(StepBasic_Approval)& aRelatedApproval);
  
  Standard_EXPORT Handle(StepBasic_Approval) RelatedApproval() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_ApprovalRelationship,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) description;
  Handle(StepBasic_Approval) relatingApproval;
  Handle(StepBasic_Approval) relatedApproval;


};







#endif // _StepBasic_ApprovalRelationship_HeaderFile
