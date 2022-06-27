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

#ifndef _StepData_Simple_HeaderFile
#define _StepData_Simple_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_FieldListN.hxx>
#include <StepData_Described.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
class StepData_ESDescr;
class StepData_Field;
class Interface_Check;
class Interface_EntityIterator;


class StepData_Simple;
DEFINE_STANDARD_HANDLE(StepData_Simple, StepData_Described)

//! A Simple Entity is defined by a type (which can heve super
//! types) and a list of parameters
class StepData_Simple : public StepData_Described
{

public:

  
  //! Creates a Simple Entity
  Standard_EXPORT StepData_Simple(const Handle(StepData_ESDescr)& descr);
  
  //! Returns description, as for simple
  Standard_EXPORT Handle(StepData_ESDescr) ESDescr() const;
  
  //! Returns the recorded StepType (TypeName of its ESDescr)
  Standard_EXPORT Standard_CString StepType() const;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsComplex() const Standard_OVERRIDE;
  
  //! Tells if a step type is matched by <me>
  //! For a Simple Entity : own type or super type
  //! For a Complex Entity : one of the members
  Standard_EXPORT Standard_Boolean Matches (const Standard_CString steptype) const Standard_OVERRIDE;
  
  //! Returns a Simple Entity which matches with a Type in <me> :
  //! For a Simple Entity : me if it matches, else a null handle
  //! For a Complex Entity : the member which matches, else null
  Standard_EXPORT Handle(StepData_Simple) As (const Standard_CString steptype) const Standard_OVERRIDE;
  
  //! Tells if a Field brings a given name
  Standard_EXPORT Standard_Boolean HasField (const Standard_CString name) const Standard_OVERRIDE;
  
  //! Returns a Field from its name; read-only
  Standard_EXPORT const StepData_Field& Field (const Standard_CString name) const Standard_OVERRIDE;
  
  //! Returns a Field from its name; read or write
  Standard_EXPORT StepData_Field& CField (const Standard_CString name) Standard_OVERRIDE;
  
  //! Returns the count of fields
  Standard_EXPORT Standard_Integer NbFields() const;
  
  //! Returns a field from its rank, for read-only use
  Standard_EXPORT const StepData_Field& FieldNum (const Standard_Integer num) const;
  
  //! Returns a field from its rank, in order to modify it
  Standard_EXPORT StepData_Field& CFieldNum (const Standard_Integer num);
  
  //! Returns the entire field list, read-only
  Standard_EXPORT const StepData_FieldListN& Fields() const;
  
  //! Returns the entire field list, read or write
  Standard_EXPORT StepData_FieldListN& CFields();
  
  //! Fills a Check by using its Description
  Standard_EXPORT void Check (Handle(Interface_Check)& ach) const Standard_OVERRIDE;
  
  //! Fills an EntityIterator with entities shared by <me>
  Standard_EXPORT void Shared (Interface_EntityIterator& list) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_Simple,StepData_Described)

protected:




private:


  StepData_FieldListN thefields;


};







#endif // _StepData_Simple_HeaderFile
