// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_VolumeElementPurposeMember_HeaderFile
#define _StepElement_VolumeElementPurposeMember_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepData_SelectNamed.hxx>
#include <Standard_CString.hxx>


class StepElement_VolumeElementPurposeMember;
DEFINE_STANDARD_HANDLE(StepElement_VolumeElementPurposeMember, StepData_SelectNamed)

//! Representation of member for  STEP SELECT type VolumeElementPurpose
class StepElement_VolumeElementPurposeMember : public StepData_SelectNamed
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_VolumeElementPurposeMember();
  
  //! Returns True if has name
  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;
  
  //! Returns set name
  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;
  
  //! Set name
  Standard_EXPORT virtual Standard_Boolean SetName (const Standard_CString name) Standard_OVERRIDE;
  
  //! Tells if the name of a SelectMember matches a given one;
  Standard_EXPORT virtual Standard_Boolean Matches (const Standard_CString name) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepElement_VolumeElementPurposeMember,StepData_SelectNamed)

protected:




private:


  Standard_Integer mycase;


};







#endif // _StepElement_VolumeElementPurposeMember_HeaderFile
