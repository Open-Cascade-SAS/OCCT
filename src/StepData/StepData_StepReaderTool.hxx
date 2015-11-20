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

#ifndef _StepData_StepReaderTool_HeaderFile
#define _StepData_StepReaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class StepData_FileRecognizer;
class StepData_StepReaderData;
class StepData_Protocol;
class Interface_Check;
class Standard_Transient;
class Interface_InterfaceModel;


//! Specific FileReaderTool for Step; works with FileReaderData
//! provides references evaluation, plus access to litteral data
//! and specific methods defined by FileReaderTool
//! Remarks : works with a ReaderLib to load Entities
class StepData_StepReaderTool  : public Interface_FileReaderTool
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates StepReaderTool to work with a StepReaderData according
  //! to a Step Protocol. Defines the ReaderLib at this time
  Standard_EXPORT StepData_StepReaderTool(const Handle(StepData_StepReaderData)& reader, const Handle(StepData_Protocol)& protocol);
  
  //! Bounds empty entities to records, uses default Recognition
  //! provided by ReaderLib and ReaderModule. Also calls computation
  //! of references (SetEntityNumbers from StepReaderData)
  //! Works only on data entities (skips header)
  //! <optimize> given False allows to test some internal algorithms
  //! which are normally avoided (see also StepReaderData)
  Standard_EXPORT void Prepare (const Standard_Boolean optimize = Standard_True);
  
  //! Bounds empty entities to records, works with a specific
  //! FileRecognizer, stored and later used in Recognize
  //! Works only on data entities (skips header)
  //! <optimize : same as above
  Standard_EXPORT void Prepare (const Handle(StepData_FileRecognizer)& reco, const Standard_Boolean optimize = Standard_True);
  
  //! recognizes records, by asking either ReaderLib (default) or
  //! FileRecognizer (if defined) to do so. <ach> is to call
  //! RecognizeByLib
  Standard_EXPORT Standard_Boolean Recognize (const Standard_Integer num, Handle(Interface_Check)& ach, Handle(Standard_Transient)& ent) Standard_OVERRIDE;
  
  //! bounds empty entities and sub-lists to header records
  //! works like Prepare + SetEntityNumbers, but for header
  //! (N.B.: in Header, no Ident and no reference)
  //! FileRecognizer is to specify Entities which are allowed to be
  //! defined in the Header (not every type can be)
  Standard_EXPORT void PrepareHeader (const Handle(StepData_FileRecognizer)& reco);
  
  //! fills model's header; that is, gives to it Header entities
  //! and commands their loading. Also fills StepModel's Global
  //! Check from StepReaderData's GlobalCheck
  Standard_EXPORT void BeginRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;
  
  //! fills an entity, given record no; works by using a ReaderLib
  //! to load each entity, which must be a Transient
  //! Actually, returned value is True if no fail, False else
  Standard_EXPORT Standard_Boolean AnalyseRecord (const Standard_Integer num, const Handle(Standard_Transient)& anent, Handle(Interface_Check)& acheck) Standard_OVERRIDE;
  
  //! Ends file reading after reading all the entities
  //! Here, it binds in the model, Idents to Entities (for checks)
  Standard_EXPORT virtual void EndRead (const Handle(Interface_InterfaceModel)& amodel) Standard_OVERRIDE;




protected:





private:



  Handle(StepData_FileRecognizer) thereco;
  Interface_GeneralLib theglib;
  Interface_ReaderLib therlib;


};







#endif // _StepData_StepReaderTool_HeaderFile
