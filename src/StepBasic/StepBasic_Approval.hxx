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

#ifndef _StepBasic_Approval_HeaderFile
#define _StepBasic_Approval_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class StepBasic_ApprovalStatus;
class TCollection_HAsciiString;


class StepBasic_Approval;
DEFINE_STANDARD_HANDLE(StepBasic_Approval, MMgt_TShared)


class StepBasic_Approval : public MMgt_TShared
{

public:

  
  //! Returns a Approval
  Standard_EXPORT StepBasic_Approval();
  
  Standard_EXPORT virtual void Init (const Handle(StepBasic_ApprovalStatus)& aStatus, const Handle(TCollection_HAsciiString)& aLevel);
  
  Standard_EXPORT void SetStatus (const Handle(StepBasic_ApprovalStatus)& aStatus);
  
  Standard_EXPORT Handle(StepBasic_ApprovalStatus) Status() const;
  
  Standard_EXPORT void SetLevel (const Handle(TCollection_HAsciiString)& aLevel);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Level() const;




  DEFINE_STANDARD_RTTI(StepBasic_Approval,MMgt_TShared)

protected:




private:


  Handle(StepBasic_ApprovalStatus) status;
  Handle(TCollection_HAsciiString) level;


};







#endif // _StepBasic_Approval_HeaderFile
