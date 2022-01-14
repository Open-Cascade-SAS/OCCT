// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _HeaderSection_Protocol_HeaderFile
#define _HeaderSection_Protocol_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_Protocol.hxx>
#include <Standard_Integer.hxx>


class HeaderSection_Protocol;
DEFINE_STANDARD_HANDLE(HeaderSection_Protocol, StepData_Protocol)

//! Protocol for HeaderSection Entities
//! It requires HeaderSection as a Resource
class HeaderSection_Protocol : public StepData_Protocol
{

public:

  
  Standard_EXPORT HeaderSection_Protocol();
  
  //! Returns a Case Number for each of the HeaderSection Entities
  Standard_EXPORT virtual Standard_Integer TypeNumber (const Handle(Standard_Type)& atype) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_CString SchemaName() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(HeaderSection_Protocol,StepData_Protocol)

protected:




private:




};







#endif // _HeaderSection_Protocol_HeaderFile
