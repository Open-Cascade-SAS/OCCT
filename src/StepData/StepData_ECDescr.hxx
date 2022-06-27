// Created on: 1997-05-09
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepData_ECDescr_HeaderFile
#define _StepData_ECDescr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <StepData_EDescr.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <Standard_CString.hxx>
class StepData_ESDescr;
class StepData_Described;


class StepData_ECDescr;
DEFINE_STANDARD_HANDLE(StepData_ECDescr, StepData_EDescr)

//! Describes a Complex Entity (Plex) as a list of Simple ones
class StepData_ECDescr : public StepData_EDescr
{

public:

  
  //! Creates an ECDescr, empty
  Standard_EXPORT StepData_ECDescr();
  
  //! Adds a member
  //! Warning : members are added in alphabetic order
  Standard_EXPORT void Add (const Handle(StepData_ESDescr)& member);
  
  //! Returns the count of members
  Standard_EXPORT Standard_Integer NbMembers() const;
  
  //! Returns a Member from its rank
  Standard_EXPORT Handle(StepData_ESDescr) Member (const Standard_Integer num) const;
  
  //! Returns the ordered list of types
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) TypeList() const;
  
  //! Tells if a ESDescr matches a step type : exact or super type
  Standard_EXPORT Standard_Boolean Matches (const Standard_CString steptype) const Standard_OVERRIDE;
  
  //! Returns True
  Standard_EXPORT Standard_Boolean IsComplex() const Standard_OVERRIDE;
  
  //! Creates a described entity (i.e. a complex one, made of one
  //! simple entity per member)
  Standard_EXPORT Handle(StepData_Described) NewEntity() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_ECDescr,StepData_EDescr)

protected:




private:


  TColStd_SequenceOfTransient thelist;


};







#endif // _StepData_ECDescr_HeaderFile
