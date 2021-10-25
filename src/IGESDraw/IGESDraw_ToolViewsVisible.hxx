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

#ifndef _IGESDraw_ToolViewsVisible_HeaderFile
#define _IGESDraw_ToolViewsVisible_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class IGESDraw_ViewsVisible;
class IGESData_IGESReaderData;
class IGESData_ParamReader;
class IGESData_IGESWriter;
class Interface_EntityIterator;
class IGESData_DirChecker;
class Interface_ShareTool;
class Interface_Check;
class Interface_CopyTool;
class IGESData_IGESDumper;

//! Tool to work on a ViewsVisible. Called by various Modules
//! (ReadWriteModule, GeneralModule, SpecificModule)
class IGESDraw_ToolViewsVisible 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a ToolViewsVisible, ready to work
  Standard_EXPORT IGESDraw_ToolViewsVisible();
  
  //! Reads own parameters from file. <PR> gives access to them,
  //! <IR> detains parameter types and values
  Standard_EXPORT void ReadOwnParams (const Handle(IGESDraw_ViewsVisible)& ent, const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const;
  
  //! Writes own parameters to IGESWriter
  Standard_EXPORT void WriteOwnParams (const Handle(IGESDraw_ViewsVisible)& ent, IGESData_IGESWriter& IW) const;
  
  //! Lists the Entities shared by a ViewsVisible <ent>, from
  //! its specific (own) parameters shared not implied (the Views)
  Standard_EXPORT void OwnShared (const Handle(IGESDraw_ViewsVisible)& ent, Interface_EntityIterator& iter) const;
  
  //! Lists the Entities shared by a ViewsVisible <ent>, from
  //! its specific (own) implied parameters : the Displayed Entities
  Standard_EXPORT void OwnImplied (const Handle(IGESDraw_ViewsVisible)& ent, Interface_EntityIterator& iter) const;
  
  //! Returns specific DirChecker
  Standard_EXPORT IGESData_DirChecker DirChecker (const Handle(IGESDraw_ViewsVisible)& ent) const;
  
  //! Performs Specific Semantic Check
  Standard_EXPORT void OwnCheck (const Handle(IGESDraw_ViewsVisible)& ent, const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const;
  
  //! Copies Specific Parameters shared not implied, i.e. all but
  //! the Displayed Entities
  Standard_EXPORT void OwnCopy (const Handle(IGESDraw_ViewsVisible)& entfrom, const Handle(IGESDraw_ViewsVisible)& entto, Interface_CopyTool& TC) const;
  
  //! Copies Specific implied Parameters : the Displayed Entities
  //! which have already been copied
  Standard_EXPORT void OwnRenew (const Handle(IGESDraw_ViewsVisible)& entfrom, const Handle(IGESDraw_ViewsVisible)& entto, const Interface_CopyTool& TC) const;
  
  //! Clears specific implied parameters, which cause looping
  //! structures; required for deletion
  Standard_EXPORT void OwnWhenDelete (const Handle(IGESDraw_ViewsVisible)& ent) const;
  
  //! Dump of Specific Parameters
  Standard_EXPORT void OwnDump (const Handle(IGESDraw_ViewsVisible)& ent, const IGESData_IGESDumper& dumper, Standard_OStream& S, const Standard_Integer own) const;
  
  //! Sets automatic unambiguous Correction on a ViewsVisible
  //! (all displayed entities must refer to <ent> in directory part,
  //! else the list is cleared)
  Standard_EXPORT Standard_Boolean OwnCorrect (const Handle(IGESDraw_ViewsVisible)& ent) const;




protected:





private:





};







#endif // _IGESDraw_ToolViewsVisible_HeaderFile
