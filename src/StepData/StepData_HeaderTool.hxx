// Created on: 1993-07-23
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _StepData_HeaderTool_HeaderFile
#define _StepData_HeaderTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class StepData_StepReaderData;
class TCollection_AsciiString;
class StepData_Protocol;
class StepData_FileProtocol;


//! HeaderTool exploits data from Header to build a Protocol :
//! it uses the Header Entity FileSchema to do this.
//! It builds a Protocol from the Global List of Protocols
//! stored in the Library ReaderLib
class StepData_HeaderTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a HeaderTool from data read from a File. Computes the
  //! list of Schema Names. The Protocol will be computed later
  //! (because there are several options)
  Standard_EXPORT StepData_HeaderTool(const Handle(StepData_StepReaderData)& data);
  
  //! Creates a HeaderTool directly from a list of Schema Names
  Standard_EXPORT StepData_HeaderTool(const TColStd_SequenceOfAsciiString& names);
  
  //! Returns the count of SchemaNames
  Standard_EXPORT Standard_Integer NbSchemaNames() const;
  
  //! Returns a SchemaName, given its rank
  Standard_EXPORT const TCollection_AsciiString& SchemaName (const Standard_Integer num) const;
  
  //! Returns the Protocol which corresponds to a Schema Name
  //! Returns a Null Handle if this Schema Name is attached to no
  //! Protocol recorded in the Global List of ReaderLib
  Standard_EXPORT Handle(StepData_Protocol) NamedProtocol (const TCollection_AsciiString& name) const;
  
  //! Fills a FileProtocol with the list of Protocols attached to
  //! the list of Schema Names. It can remain empty ...
  Standard_EXPORT void Build (const Handle(StepData_FileProtocol)& protocol);
  
  //! Returns a Protocol computed from the list of Schema Names :
  //! - a Null Handle if no SchemaName has been recognized (or list
  //! empty)
  //! - a single Protocol if only one SchemaName has been recognized
  //! - a FileProtocol with its componants if several SchemaNames
  //! have been recognized
  Standard_EXPORT Handle(StepData_Protocol) Protocol();
  
  //! Returns True if either Build or Protocol has been called
  //! If it is False, Ignored and NbIgnored should not be called
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the count of ignored SchemaNames (0 if all were OK)
  Standard_EXPORT Standard_Integer NbIgnoreds() const;
  
  //! Returns an ignored SchemaName, given its rank in the list of
  //! Ignored SchemaNames (not in the total list)
  Standard_EXPORT const TCollection_AsciiString& Ignored (const Standard_Integer num) const;
  
  //! Sends the state of the HeaderTool in a comprehensive way,
  //! to an output stream
  Standard_EXPORT void Print (Standard_OStream& S) const;




protected:





private:



  TColStd_SequenceOfAsciiString thenames;
  Standard_Boolean thedone;
  TColStd_SequenceOfAsciiString theignored;


};







#endif // _StepData_HeaderTool_HeaderFile
