// Created on: 1994-03-14
// Created by: Christian CAILLET
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

#ifndef _StepData_StepDumper_HeaderFile
#define _StepData_StepDumper_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_GeneralLib.hxx>
#include <StepData_WriterLib.hxx>
#include <StepData_StepWriter.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class StepData_StepModel;
class Interface_InterfaceMismatch;
class StepData_Protocol;
class StepData_StepWriter;
class Message_Messenger;
class Standard_Transient;


//! Provides a way to dump entities processed through STEP, with
//! these features :
//! - same form as for writing a STEP File (because it is clear
//! and compact enough, even if the names of the fields do not
//! appear) : thus, no additionnal resource is required
//! - possibility to look for an entity itself (only its Type or
//! with its content), an entity and it shared items (one level)
//! or all the entities its refers to, directly or recursively.
class StepData_StepDumper 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a StepDumper, able to work on a given StepModel
  //! (which defines the total scope for dumping entities) and
  //! a given Protocol from Step (which defines the authorized
  //! types to be dumped)
  //! <mode> commands what is to be displayed (number or label)
  //! 0 for number (and corresponding labels  are displayed apart)
  //! 1 for label  (and corresponding numbers are displayed apart)
  //! 2 for label without anymore
  Standard_EXPORT StepData_StepDumper(const Handle(StepData_StepModel)& amodel, const Handle(StepData_Protocol)& protocol, const Standard_Integer mode = 0);
  
  //! Gives an access to the tool which is used to work : this allow
  //! to acts on some parameters : Floating Format, Scopes ...
  Standard_EXPORT StepData_StepWriter& StepWriter();
  
  //! Dumps a Entity on an Messenger. Returns True if
  //! sucess, False, if the entity to dump has not been recognized
  //! by the Protocol. <level> can have one of these values :
  //! - 0 : prints the TYPE only, as known in STEP Files (StepType)
  //! If <ent> has not been regognized by the Protocol, or if its
  //! type is Complex, the StepType is replaced by the display of
  //! the cdl type. Complex Type are well processed by level 1.
  //! - 1 : dumps the entity, completely (whatever it has simple or
  //! complex type) but alone.
  //! - 2 : dumps the entity completely, plus the item its refers to
  //! at first level (a header message designates the starting
  //! entity of the dump) <Lists Shared and Implied>
  //! - 3 : dumps the entity and its refered items at any levels
  //!
  //! For levels 1,2,3, the numbers displayed (form #nnn) are the
  //! numbers of the corresponding entities in the Model
  Standard_EXPORT Standard_Boolean Dump (const Handle(Message_Messenger)& S, const Handle(Standard_Transient)& ent, const Standard_Integer level);
  
  //! Works as Dump with a Transient, but directly takes the
  //! entity designated by its number in the Model
  //! Returns False, also if <num> is out of range
  Standard_EXPORT Standard_Boolean Dump (const Handle(Message_Messenger)& S, const Standard_Integer num, const Standard_Integer level);




protected:





private:



  Handle(StepData_StepModel) themodel;
  Interface_GeneralLib theslib;
  StepData_WriterLib thewlib;
  StepData_StepWriter thewriter;


};







#endif // _StepData_StepDumper_HeaderFile
