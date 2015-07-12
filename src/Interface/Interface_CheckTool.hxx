// Created on: 1993-02-02
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

#ifndef _Interface_CheckTool_HeaderFile
#define _Interface_CheckTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_ShareTool.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Interface_GTool;
class Interface_CheckFailure;
class Interface_InterfaceModel;
class Interface_Protocol;
class Interface_Graph;
class Interface_HGraph;
class Standard_Transient;
class Interface_ShareTool;
class Interface_Check;
class Message_Messenger;
class Interface_CheckIterator;
class Interface_EntityIterator;


//! Performs Checks on Entities, using General Service Library and
//! Modules to work. Works on one Entity or on a complete Model
class Interface_CheckTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a CheckTool, by calling the General Service Library
  //! and Modules, selected through a Protocol, to work on a Model
  //! Moreover, Protocol recognizes Unknown Entities
  Standard_EXPORT Interface_CheckTool(const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol);
  
  //! Creates a CheckTool, by calling the General Service Library
  //! and Modules, selected through a Protocol, to work on a Model
  //! Protocol and so on are taken from the Model (its GTool)
  Standard_EXPORT Interface_CheckTool(const Handle(Interface_InterfaceModel)& model);
  
  //! Creates a CheckTool from a Graph. The Graph contains a Model
  //! which designates a Protocol: they are used to create ShareTool
  Standard_EXPORT Interface_CheckTool(const Interface_Graph& graph);
  
  Standard_EXPORT Interface_CheckTool(const Handle(Interface_HGraph)& hgraph);
  
  //! Fills as required a Check with the Error and Warning messages
  //! produced by Checking a given Entity.
  //! For an Erroneous or Corrected Entity : Check build at Analyse
  //! time; else, Check computed for Entity (Verify integrity), can
  //! use a Graph as required to control context
  Standard_EXPORT void FillCheck (const Handle(Standard_Transient)& ent, const Interface_ShareTool& sh, Handle(Interface_Check)& ach);
  
  //! Utility method which Prints the content of a Check
  Standard_EXPORT void Print (const Handle(Interface_Check)& ach, const Handle(Message_Messenger)& S) const;
  
  //! Simply Lists all the Checks and the Content (messages) and the
  //! Entity, if there is, of each Check
  //! (if all Checks are OK, nothing is Printed)
  Standard_EXPORT void Print (const Interface_CheckIterator& list, const Handle(Message_Messenger)& S) const;
  
  //! Returns the Check associated to an Entity identified by
  //! its Number in a Model.
  Standard_EXPORT Handle(Interface_Check) Check (const Standard_Integer num);
  
  //! Checks if any Error has been detected (CheckList not empty)
  //! Returns normally if none, raises exception if some exists.
  //! It reuses the last computations from other checking methods,
  //! unless the argument <resest> is given True
  Standard_EXPORT void CheckSuccess (const Standard_Boolean reset = Standard_False);
  
  //! Returns list of all "remarkable" informations, which include :
  //! - GlobalCheck, if not empty
  //! - Error Checks, for all Errors (Verify + Analyse)
  //! - also Corrected Entities
  //! - and Unknown Entities : for those, each Unknown Entity is
  //! associated to an empty Check (it is neither an Error nor a
  //! Correction, but a remarkable information)
  Standard_EXPORT Interface_CheckIterator CompleteCheckList();
  
  //! Returns list of all Errors detected
  //! Note that presence of Unknown Entities is not an error
  //! Cumulates : GlobalCheck if error +
  //! AnalyseCheckList + VerifyCheckList
  Standard_EXPORT Interface_CheckIterator CheckList();
  
  //! Returns list of errors dectected at Analyse time (syntactic)
  //! (note that GlobalCheck is not in this list)
  Standard_EXPORT Interface_CheckIterator AnalyseCheckList();
  
  //! Returns list of integrity constraints errors (semantic)
  //! (note that GlobalCheck is not in this list)
  Standard_EXPORT Interface_CheckIterator VerifyCheckList();
  
  //! Returns list of Corrections (includes GlobalCheck if corrected)
  Standard_EXPORT Interface_CheckIterator WarningCheckList();
  
  //! Returns list of Unknown Entities
  //! Note that Error and Erroneous Entities are not considered
  //! as Unknown
  Standard_EXPORT Interface_EntityIterator UnknownEntities();




protected:





private:



  Handle(Interface_GTool) thegtool;
  Interface_ShareTool theshare;
  Standard_Integer thestat;


};







#endif // _Interface_CheckTool_HeaderFile
