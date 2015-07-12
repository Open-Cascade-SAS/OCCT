// Created on: 1993-10-14
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

#ifndef _IGESSolid_ToolShell_HeaderFile
#define _IGESSolid_ToolShell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class Standard_DomainError;
class IGESSolid_Shell;
class IGESData_IGESReaderData;
class IGESData_ParamReader;
class IGESData_IGESWriter;
class Interface_EntityIterator;
class IGESData_DirChecker;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;
class IGESData_IGESDumper;
class Message_Messenger;


//! Tool to work on a Shell. Called by various Modules
//! (ReadWriteModule, GeneralModule, SpecificModule)
class IGESSolid_ToolShell 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a ToolShell, ready to work
  Standard_EXPORT IGESSolid_ToolShell();
  
  //! Reads own parameters from file. <PR> gives access to them,
  //! <IR> detains parameter types and values
  Standard_EXPORT void ReadOwnParams (const Handle(IGESSolid_Shell)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const;
  
  //! Writes own parameters to IGESWriter
  Standard_EXPORT void WriteOwnParams (const Handle(IGESSolid_Shell)& ent, IGESData_IGESWriter& IW) const;
  
  //! Lists the Entities shared by a Shell <ent>, from
  //! its specific (own) parameters
  Standard_EXPORT void OwnShared (const Handle(IGESSolid_Shell)& ent, Interface_EntityIterator& iter) const;
  
  //! Returns specific DirChecker
  Standard_EXPORT IGESData_DirChecker DirChecker (const Handle(IGESSolid_Shell)& ent) const;
  
  //! Performs Specific Semantic Check
  Standard_EXPORT void OwnCheck (const Handle(IGESSolid_Shell)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const;
  
  //! Copies Specific Parameters
  Standard_EXPORT void OwnCopy (const Handle(IGESSolid_Shell)& entfrom, const Handle(IGESSolid_Shell)& entto, Interface_CopyTool& TC) const;
  
  //! Dump of Specific Parameters
  Standard_EXPORT void OwnDump (const Handle(IGESSolid_Shell)& ent, const IGESData_IGESDumper& dumper, const Handle(Message_Messenger)& S, const Standard_Integer own) const;




protected:





private:





};







#endif // _IGESSolid_ToolShell_HeaderFile
