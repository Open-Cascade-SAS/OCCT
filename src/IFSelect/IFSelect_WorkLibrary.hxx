// Created on: 1993-01-08
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

#ifndef _IFSelect_WorkLibrary_HeaderFile
#define _IFSelect_WorkLibrary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class Interface_InterfaceModel;
class Interface_Protocol;
class IFSelect_ContextWrite;
class Interface_EntityIterator;
class Interface_CopyTool;

class IFSelect_WorkLibrary;
DEFINE_STANDARD_HANDLE(IFSelect_WorkLibrary, Standard_Transient)

//! This class defines the (empty) frame which can be used to
//! enrich a XSTEP set with new capabilities
//! In particular, a specific WorkLibrary must give the way for
//! Reading a File into a Model, and Writing a Model to a File
//! Thus, it is possible to define several Work Libraries for each
//! norm, but recommended to define one general class for each one :
//! this general class will define the Read and Write methods.
//!
//! Also a Dump service is provided, it can produce, according the
//! norm, either a parcel of a file for an entity, or any other
//! kind of information relevant for the norm,
class IFSelect_WorkLibrary : public Standard_Transient
{

public:

  
  //! Gives the way to Read a File and transfer it to a Model
  //! <mod> is the resulting Model, which has to be created by this
  //! method. In case of error, <mod> must be returned Null
  //! Return value is a status with free values.
  //! Simply, 0 is for "Execution OK"
  //! The Protocol can be used to work (e.g. create the Model, read
  //! and recognize the Entities)
  Standard_EXPORT virtual Standard_Integer ReadFile (const Standard_CString name, Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol) const = 0;
  
  //! Interface to read a data from the specified stream.
  //! @param model is the resulting Model, which has to be created by this method. 
  //!        In case of error, model must be returned Null
  //! Return value is a status: 0 - OK, 1 - read failure, -1 - stream failure.
  //! 
  //! Default implementation returns 1 (error).
  Standard_EXPORT virtual Standard_Integer ReadStream (const Standard_CString theName, std::istream& theIStream, 
                                                       Handle(Interface_InterfaceModel)& model, 
                                                       const Handle(Interface_Protocol)& protocol) const;

  //! Gives the way to Write a File from a Model.
  //! <ctx> contains all necessary information : the model, the
  //! protocol, the file name, and the list of File Modifiers to be
  //! applied, also with restricted list of selected entities for
  //! each one, if required.
  //! In return, it brings the produced check-list
  //!
  //! The WorkLibrary has to query <applied> to get then run the
  //! ContextWrite by looping like this (example) :
  //! for (numap = 1; numap <= ctx.NbModifiers(); numap ++) {
  //! ctx.SetModifier (numap);
  //! cast ctx.FileModifier()  to specific type -> variable filemod
  //! if (!filemod.IsNull()) filemod->Perform (ctx,writer);
  //! filemod then works with ctx. It can, either act on the
  //! model itself (for instance on its header), or iterate
  //! on selected entities (Start/Next/More/Value)
  //! it can call AddFail or AddWarning, as necessary
  //! }
  Standard_EXPORT virtual Standard_Boolean WriteFile (IFSelect_ContextWrite& ctx) const = 0;
  
  //! Performs the copy of entities from an original model to a new
  //! one. It must also copy headers if any. Returns True when done.
  //! The provided default works by copying the individual entities
  //! designated in the list, by using the general service class
  //! CopyTool.
  //! It can be redefined for a norm which, either implements Copy
  //! by another way (do not forget to Bind each copied result with
  //! its original entity in TC) and returns True, or does not know
  //! how to copy and returns False
  Standard_EXPORT virtual Standard_Boolean CopyModel (const Handle(Interface_InterfaceModel)& original, const Handle(Interface_InterfaceModel)& newmodel, const Interface_EntityIterator& list, Interface_CopyTool& TC) const;
  
  //! Gives the way of dumping an entity under a form comprehensive
  //! for each norm. <model> helps to identify, number ... entities.
  //! <level> is to be interpreted for each norm (because of the
  //! formats which can be very different)
  Standard_EXPORT virtual void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, Standard_OStream& S, const Standard_Integer level) const = 0;
  
  //! Calls deferred DumpEntity with the recorded default level
  Standard_EXPORT void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, Standard_OStream& S) const;
  
  //! Records a default level and a maximum value for level
  //! level for DumpEntity can go between 0 and <max>
  //! default value will be <def>
  Standard_EXPORT void SetDumpLevels (const Standard_Integer def, const Standard_Integer max);
  
  //! Returns the recorded default and maximum dump levels
  //! If none was recorded, max is returned negative, def as zero
  Standard_EXPORT void DumpLevels (Standard_Integer& def, Standard_Integer& max) const;
  
  //! Records a short line of help for a level (0 - max)
  Standard_EXPORT void SetDumpHelp (const Standard_Integer level, const Standard_CString help);
  
  //! Returns the help line recorded for <level>, or an empty string
  Standard_EXPORT Standard_CString DumpHelp (const Standard_Integer level) const;




  DEFINE_STANDARD_RTTIEXT(IFSelect_WorkLibrary,Standard_Transient)

protected:

  
  //! Required to initialise fields
  Standard_EXPORT IFSelect_WorkLibrary();



private:


  Standard_Integer thelevdef;
  Handle(Interface_HArray1OfHAsciiString) thelevhlp;


};







#endif // _IFSelect_WorkLibrary_HeaderFile
