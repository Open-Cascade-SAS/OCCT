// Created on: 1992-02-11
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StepData_UndefinedEntity_HeaderFile
#define _StepData_UndefinedEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Interface_UndefinedContent;
class StepData_StepReaderData;
class Interface_Check;
class StepData_StepWriter;
class Interface_CopyTool;
class Interface_EntityIterator;


class StepData_UndefinedEntity;
DEFINE_STANDARD_HANDLE(StepData_UndefinedEntity, Standard_Transient)

//! Undefined entity specific to Step Interface, in which StepType
//! is defined at each instance, or is a SubList of another one
//! Uses an UndefinedContent, that from Interface is suitable.
//! Also an Entity defined by STEP can be "Complex Type" (see
//! ANDOR clause in Express).
class StepData_UndefinedEntity : public Standard_Transient
{

public:

  
  //! creates an Unknown entity
  Standard_EXPORT StepData_UndefinedEntity();
  
  //! Creates a SubList of an Unknown entity : it is an Unknown
  //! Entity with no Type, but flagged as "SUB" if issub is True
  Standard_EXPORT StepData_UndefinedEntity(const Standard_Boolean issub);
  
  //! Returns the UndefinedContent which brings the Parameters
  Standard_EXPORT Handle(Interface_UndefinedContent) UndefinedContent() const;
  
  //! Returns True if an Unndefined Entity is SubPart of another one
  Standard_EXPORT Standard_Boolean IsSub() const;
  
  //! Returns the "super-entity" of a sub-part
  Standard_EXPORT Handle(StepData_UndefinedEntity) Super() const;
  
  //! Returns True if <me> defines a Multiple Type Entity (see ANDOR)
  Standard_EXPORT Standard_Boolean IsComplex() const;
  
  //! For a Multiple Type Entity, returns the Next "Component"
  //! For more than two Types, iterative definition (Next->Next...)
  //! Returns a Null Handle for the end of the List
  Standard_EXPORT Handle(StepData_UndefinedEntity) Next() const;
  
  //! gives entity type, read from file
  //! For a Complex Type Entity, gives the first Type read, each
  //! "Next" gives its "partial" type
  //! was C++ : return const
  Standard_EXPORT Standard_CString StepType() const;
  
  //! reads data from StepReaderData (i.e. from file), by filling
  //! StepType and parameters stored in the UndefinedContent
  Standard_EXPORT void ReadRecord (const Handle(StepData_StepReaderData)& SR, const Standard_Integer num, Handle(Interface_Check)& ach);
  
  //! write data to StepWriter, taken from UndefinedContent
  Standard_EXPORT void WriteParams (StepData_StepWriter& SW) const;
  
  //! reads another UndefinedEntity from StepData
  Standard_EXPORT void GetFromAnother (const Handle(StepData_UndefinedEntity)& other, Interface_CopyTool& TC);
  
  //! Fills the list of shared entities
  Standard_EXPORT void FillShared (Interface_EntityIterator& list) const;




  DEFINE_STANDARD_RTTIEXT(StepData_UndefinedEntity,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) thetype;
  Handle(Interface_UndefinedContent) thecont;
  Standard_Boolean thesub;
  Handle(StepData_UndefinedEntity) thenext;


};







#endif // _StepData_UndefinedEntity_HeaderFile
