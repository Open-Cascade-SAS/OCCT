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

#ifndef _RWHeaderSection_GeneralModule_HeaderFile
#define _RWHeaderSection_GeneralModule_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_GeneralModule.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class Interface_EntityIterator;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;


class RWHeaderSection_GeneralModule;
DEFINE_STANDARD_HANDLE(RWHeaderSection_GeneralModule, StepData_GeneralModule)

//! Defines General Services for HeaderSection Entities
//! (Share,Check,Copy; Trace already inherited)
//! Depends (for case numbers) of Protocol from HeaderSection
class RWHeaderSection_GeneralModule : public StepData_GeneralModule
{

public:

  
  //! Creates a GeneralModule
  Standard_EXPORT RWHeaderSection_GeneralModule();
  
  //! Specific filling of the list of Entities shared by an Entity
  //! <ent>, according to a Case Number <CN> (provided by HeaderSection
  //! Protocol).
  Standard_EXPORT void FillSharedCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, Interface_EntityIterator& iter) const Standard_OVERRIDE;
  
  //! Specific Checking of an Entity <ent>
  Standard_EXPORT void CheckCase (const Standard_Integer CN, const Handle(Standard_Transient)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const Standard_OVERRIDE;
  
  //! Specific Copy ("Deep") from <entfrom> to <entto> (same type)
  //! by using a CopyTool which provides its working Map.
  //! Use method Transferred from CopyTool to work
  Standard_EXPORT void CopyCase (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom, const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean NewVoid (const Standard_Integer CN, Handle(Standard_Transient)& ent) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(RWHeaderSection_GeneralModule,StepData_GeneralModule)

protected:




private:




};







#endif // _RWHeaderSection_GeneralModule_HeaderFile
