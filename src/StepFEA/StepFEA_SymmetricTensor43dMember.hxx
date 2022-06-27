// Created on: 2003-01-14
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_SymmetricTensor43dMember_HeaderFile
#define _StepFEA_SymmetricTensor43dMember_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepData_SelectArrReal.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>


class StepFEA_SymmetricTensor43dMember;
DEFINE_STANDARD_HANDLE(StepFEA_SymmetricTensor43dMember, StepData_SelectArrReal)

//! Representation of member for  STEP SELECT type SymmetricTensor43d
class StepFEA_SymmetricTensor43dMember : public StepData_SelectArrReal
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_SymmetricTensor43dMember();
  
  //! Returns True if has name
  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;
  
  //! Returns set name
  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;
  
  //! Set name
  Standard_EXPORT virtual Standard_Boolean SetName (const Standard_CString name) Standard_OVERRIDE;
  
  //! Tells if the name of a SelectMember matches a given one;
  Standard_EXPORT virtual Standard_Boolean Matches (const Standard_CString name) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepFEA_SymmetricTensor43dMember,StepData_SelectArrReal)

protected:




private:


  Standard_Integer mycase;


};







#endif // _StepFEA_SymmetricTensor43dMember_HeaderFile
