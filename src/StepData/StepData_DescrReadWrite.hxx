// Created on: 1997-05-21
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

#ifndef _StepData_DescrReadWrite_HeaderFile
#define _StepData_DescrReadWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_ReadWriteModule.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_Boolean.hxx>
class StepData_Protocol;
class TCollection_AsciiString;
class StepData_StepReaderData;
class Interface_Check;
class Standard_Transient;
class StepData_StepWriter;


class StepData_DescrReadWrite;
DEFINE_STANDARD_HANDLE(StepData_DescrReadWrite, StepData_ReadWriteModule)


class StepData_DescrReadWrite : public StepData_ReadWriteModule
{

public:

  
  Standard_EXPORT StepData_DescrReadWrite(const Handle(StepData_Protocol)& proto);
  
  Standard_EXPORT Standard_Integer CaseStep (const TCollection_AsciiString& atype) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer CaseStep (const TColStd_SequenceOfAsciiString& types) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsComplex (const Standard_Integer CN) const Standard_OVERRIDE;
  
  Standard_EXPORT const TCollection_AsciiString& StepType (const Standard_Integer CN) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean ComplexType (const Standard_Integer CN, TColStd_SequenceOfAsciiString& types) const Standard_OVERRIDE;
  
  Standard_EXPORT void ReadStep (const Standard_Integer CN, const Handle(StepData_StepReaderData)& data, const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;
  
  Standard_EXPORT void WriteStep (const Standard_Integer CN, StepData_StepWriter& SW, const Handle(Standard_Transient)& ent) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_DescrReadWrite,StepData_ReadWriteModule)

protected:




private:


  Handle(StepData_Protocol) theproto;


};







#endif // _StepData_DescrReadWrite_HeaderFile
