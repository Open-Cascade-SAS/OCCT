// Created on: 1997-01-03
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

#ifndef _StepData_FreeFormEntity_HeaderFile
#define _StepData_FreeFormEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_HArray1OfField.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <Standard_Integer.hxx>
class StepData_Field;


class StepData_FreeFormEntity;
DEFINE_STANDARD_HANDLE(StepData_FreeFormEntity, Standard_Transient)

//! A Free Form Entity allows to record any kind of STEP
//! parameters, in any way of typing
//! It is implemented with an array of fields
//! A Complex entity can be defined, as a chain of FreeFormEntity
//! (see Next and As)
class StepData_FreeFormEntity : public Standard_Transient
{

public:

  
  //! Creates a FreeFormEntity, with no field, no type
  Standard_EXPORT StepData_FreeFormEntity();
  
  //! Sets the type of an entity
  //! For a complex one, the type of this member
  Standard_EXPORT void SetStepType (const Standard_CString typenam);
  
  //! Returns the recorded StepType
  //! For a complex one, the type of this member
  Standard_EXPORT Standard_CString StepType() const;
  
  //! Sets a next member, in order to define or complete a Complex
  //! entity
  //! If <last> is True (D), this next will be set as last of list
  //! Else, it is inserted just as next of <me>
  //! If <next> is Null, Next is cleared
  Standard_EXPORT void SetNext (const Handle(StepData_FreeFormEntity)& next, const Standard_Boolean last = Standard_True);
  
  //! Returns the next member of a Complex entity
  //! (remark : the last member has none)
  Standard_EXPORT Handle(StepData_FreeFormEntity) Next() const;
  
  //! Returns True if a FreeFormEntity is Complex (i.e. has Next)
  Standard_EXPORT Standard_Boolean IsComplex() const;
  
  //! Returns the member of a FreeFormEntity of which the type name
  //! is given (exact match, no sub-type)
  Standard_EXPORT Handle(StepData_FreeFormEntity) Typed (const Standard_CString typenam) const;
  
  //! Returns the list of types (one type for a simple entity),
  //! as is (non reordered)
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) TypeList() const;
  
  //! Reorders a Complex entity if required, i.e. if member types
  //! are not in alphabetic order
  //! Returns False if nothing done (order was OK or simple entity),
  //! True plus modified <ent> if <ent> has been reordered
  Standard_EXPORT static Standard_Boolean Reorder (Handle(StepData_FreeFormEntity)& ent);
  
  //! Sets a count of Fields, from scratch
  Standard_EXPORT void SetNbFields (const Standard_Integer nb);
  
  //! Returns the count of fields
  Standard_EXPORT Standard_Integer NbFields() const;
  
  //! Returns a field from its rank, for read-only use
  Standard_EXPORT const StepData_Field& Field (const Standard_Integer num) const;
  
  //! Returns a field from its rank, in order to modify it
  Standard_EXPORT StepData_Field& CField (const Standard_Integer num);




  DEFINE_STANDARD_RTTIEXT(StepData_FreeFormEntity,Standard_Transient)

protected:




private:


  TCollection_AsciiString thetype;
  Handle(StepData_HArray1OfField) thefields;
  Handle(StepData_FreeFormEntity) thenext;


};







#endif // _StepData_FreeFormEntity_HeaderFile
