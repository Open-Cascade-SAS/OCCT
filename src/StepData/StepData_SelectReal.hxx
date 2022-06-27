// Created on: 1996-12-16
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _StepData_SelectReal_HeaderFile
#define _StepData_SelectReal_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_SelectMember.hxx>
#include <Standard_Integer.hxx>


class StepData_SelectReal;
DEFINE_STANDARD_HANDLE(StepData_SelectReal, StepData_SelectMember)

//! A SelectReal is a SelectMember specialised for a basic real
//! type in a select which also accepts entities : this one has
//! NO NAME
//! For a named select, see SelectNamed
class StepData_SelectReal : public StepData_SelectMember
{

public:

  
  Standard_EXPORT StepData_SelectReal();
  
  Standard_EXPORT virtual Standard_Integer Kind() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Real Real() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetReal (const Standard_Real val) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_SelectReal,StepData_SelectMember)

protected:




private:


  Standard_Real theval;


};







#endif // _StepData_SelectReal_HeaderFile
