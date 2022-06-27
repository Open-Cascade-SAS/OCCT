// Created on: 1994-09-14
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

#ifndef _StepSelect_WorkLibrary_HeaderFile
#define _StepSelect_WorkLibrary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_WorkLibrary.hxx>
class Interface_InterfaceModel;
class Interface_Protocol;
class IFSelect_ContextWrite;
class Interface_EntityIterator;
class Interface_CopyTool;

class StepSelect_WorkLibrary;
DEFINE_STANDARD_HANDLE(StepSelect_WorkLibrary, IFSelect_WorkLibrary)

//! Performs Read and Write a STEP File with a STEP Model
//! Following the protocols, Copy may be implemented or not
class StepSelect_WorkLibrary : public IFSelect_WorkLibrary
{

public:

  
  //! Creates a STEP WorkLibrary
  //! <copymode> precises whether Copy is implemented or not
  Standard_EXPORT StepSelect_WorkLibrary(const Standard_Boolean copymode = Standard_True);
  
  //! Selects a mode to dump entities
  //! 0 (D) : prints numbers, then displays table number/label
  //! 1 : prints labels, then displays table label/number
  //! 2 : prints labels onky
  Standard_EXPORT void SetDumpLabel (const Standard_Integer mode);
  
  //! Reads a STEP File and returns a STEP Model (into <mod>),
  //! or lets <mod> "Null" in case of Error
  //! Returns 0 if OK, 1 if Read Error, -1 if File not opened
  Standard_EXPORT Standard_Integer ReadFile (const Standard_CString name, Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol) const Standard_OVERRIDE;

  //! Reads a STEP File from stream and returns a STEP Model (into <mod>),
  //! or lets <mod> "Null" in case of Error
  //! Returns 0 if OK, 1 if Read Error, -1 if File not opened
  Standard_EXPORT Standard_Integer ReadStream(const Standard_CString theName,
                                              std::istream& theIStream, 
                                              Handle(Interface_InterfaceModel)& model,
                                              const Handle(Interface_Protocol)& protocol) const Standard_OVERRIDE;

  //! Writes a File from a STEP Model
  //! Returns False (and writes no file) if <ctx> does not bring a
  //! STEP Model
  Standard_EXPORT Standard_Boolean WriteFile (IFSelect_ContextWrite& ctx) const Standard_OVERRIDE;
  
  //! Performs the copy of entities from an original model to a new
  //! one. Works according <copymode> :
  //! if True, standard copy is run
  //! else nothing is done and returned value is False
  Standard_EXPORT virtual Standard_Boolean CopyModel (const Handle(Interface_InterfaceModel)& original, const Handle(Interface_InterfaceModel)& newmodel, const Interface_EntityIterator& list, Interface_CopyTool& TC) const Standard_OVERRIDE;
  
  //! Dumps an entity under STEP form, i.e. as a part of a Step file
  //! Works with a StepDumper.
  //! Level 0 just displays type; level 1 displays the entity itself
  //! and level 2 displays the entity plus its shared ones (one
  //! sub-level : immediately shared entities)
  Standard_EXPORT virtual void DumpEntity (const Handle(Interface_InterfaceModel)& model, const Handle(Interface_Protocol)& protocol, const Handle(Standard_Transient)& entity, Standard_OStream& S, const Standard_Integer level) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StepSelect_WorkLibrary,IFSelect_WorkLibrary)

protected:




private:


  Standard_Boolean thecopymode;
  Standard_Integer thelabmode;


};







#endif // _StepSelect_WorkLibrary_HeaderFile
