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

#ifndef _StepData_DescrGeneral_HeaderFile
#define _StepData_DescrGeneral_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_GeneralModule.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class StepData_Protocol;
class Standard_Transient;
class Interface_EntityIterator;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;


class StepData_DescrGeneral;
DEFINE_STANDARD_HANDLE(StepData_DescrGeneral, StepData_GeneralModule)

//! Works with a Protocol by considering its entity descriptions
class StepData_DescrGeneral : public StepData_GeneralModule
{

public:

  
  Standard_EXPORT StepData_DescrGeneral(const Handle(StepData_Protocol)& proto);
  
  Standard_EXPORT void FillSharedCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const Standard_OVERRIDE;
  
  Standard_EXPORT void CheckCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const Standard_OVERRIDE;
  
  Standard_EXPORT void CopyCase (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom, const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean NewVoid (const Standard_Integer CN, Handle(Standard_Transient)& ent) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepData_DescrGeneral,StepData_GeneralModule)

protected:




private:


  Handle(StepData_Protocol) theproto;


};







#endif // _StepData_DescrGeneral_HeaderFile
