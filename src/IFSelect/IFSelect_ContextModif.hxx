// Created on: 1994-06-08
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

#ifndef _IFSelect_ContextModif_HeaderFile
#define _IFSelect_ContextModif_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_CheckIterator.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class Interface_Protocol;
class Interface_CopyControl;
class Standard_NoSuchObject;
class Interface_Graph;
class Interface_CopyTool;
class Interface_EntityIterator;
class Interface_InterfaceModel;
class Standard_Transient;
class IFSelect_GeneralModifier;
class Interface_Check;
class Interface_CheckIterator;


//! This class gathers various informations used by Model Modifiers
//! apart from the target model itself, and the CopyTool which
//! must be passed directly.
//!
//! These informations report to original data : model, entities,
//! and the selection list if there is one : it allows to query
//! about such or such starting entity, or result entity, or
//! iterate on selection list ...
//! Also data useful for file output are available (because some
//! Modifiers concern models produced for file output).
//!
//! Furthermore, in return, ContextModif can record Checks, either
//! one for all, or one for each Entity. It supports trace too.
class IFSelect_ContextModif 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Prepares a ContextModif with these informations :
  //! - the graph established from original model (target passed
  //! directly to Modifier)
  //! - the CopyTool which detains the CopyControl, which maps
  //! starting (in original) and result (in target) entities
  //! - an optional file name (for file output)
  //!
  //! Such a ContextModif is considered to be applied on all
  //! transferred entities (no filter active)
  Standard_EXPORT IFSelect_ContextModif(const Interface_Graph& graph, const Interface_CopyTool& TC, const Standard_CString filename = "");
  
  //! Prepares a ContextModif with these informations :
  //! - the graph established from original model (target passed
  //! directly to Modifier)
  //! - an optional file name (for file output)
  //! Here, no CopyControl, hence all entities are considered equal
  //! as starting and result
  //!
  //! Such a ContextModif is considered to be applied on all
  //! transferred entities (no filter active)
  Standard_EXPORT IFSelect_ContextModif(const Interface_Graph& graph, const Standard_CString filename = "");
  
  //! This method requires ContextModif to be applied with a filter.
  //! If a ModelModifier is defined with a Selection criterium,
  //! the result of this Selection is used as a filter :
  //! - if none of its items has been transferred, the modification
  //! does not apply at all
  //! - else, the Modifier can query for what entities were selected
  //! and what are their results
  //! - if this method is not called before working, the Modifier
  //! has to work on the whole Model
  Standard_EXPORT void Select (Interface_EntityIterator& list);
  
  //! Returns the original Graph (compared to OriginalModel, it
  //! gives more query capabilitites)
  Standard_EXPORT const Interface_Graph& OriginalGraph() const;
  
  //! Returns the original model
  Standard_EXPORT Handle(Interface_InterfaceModel) OriginalModel() const;
  
  //! Allows to transmit a Protocol as part of a ContextModif
  Standard_EXPORT void SetProtocol (const Handle(Interface_Protocol)& proto);
  
  //! Returns the Protocol (Null if not set)
  Standard_EXPORT Handle(Interface_Protocol) Protocol() const;
  
  //! Returns True if a non empty file name has been defined
  Standard_EXPORT Standard_Boolean HasFileName() const;
  
  //! Returns File Name (can be empty)
  Standard_EXPORT Standard_CString FileName() const;
  
  //! Returns the map for a direct use, if required
  Standard_EXPORT Handle(Interface_CopyControl) Control() const;
  
  //! Returns True if Select has determined that a Modifier may not
  //! be run (filter defined and empty)
  Standard_EXPORT Standard_Boolean IsForNone() const;
  
  //! Returns True if no filter is defined : a Modifier has to work
  //! on all entities of the resulting (target) model
  Standard_EXPORT Standard_Boolean IsForAll() const;
  
  //! Returns True if a starting item has been transferred
  Standard_EXPORT Standard_Boolean IsTransferred (const Handle(Standard_Transient)& ent) const;
  
  //! Returns True if a starting item has been transferred and selected
  Standard_EXPORT Standard_Boolean IsSelected (const Handle(Standard_Transient)& ent) const;
  
  //! Returns True if a starting entity has been transferred, and
  //! the result is in <res>. Returns False else
  //! (direct call to the map)
  Standard_EXPORT Standard_Boolean Search (const Handle(Standard_Transient)& ent, Handle(Standard_Transient)& res) const;
  
  //! Returns the list of original selected items.
  //! See also the iteration
  Standard_EXPORT Interface_EntityIterator SelectedOriginal() const;
  
  //! Returns the list of resulting counterparts of selected items.
  //! See also the iteration
  Standard_EXPORT Interface_EntityIterator SelectedResult() const;
  
  //! Returns the count of selected and transferred items
  Standard_EXPORT Standard_Integer SelectedCount() const;
  
  //! Starts an iteration on selected items. It takes into account
  //! IsForAll/IsForNone, by really iterating on all selected items.
  Standard_EXPORT void Start();
  
  //! Returns True until the iteration has finished
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Advances the iteration
  Standard_EXPORT void Next();
  
  //! Returns the current selected item in the original model
  Standard_EXPORT Handle(Standard_Transient) ValueOriginal() const;
  
  //! Returns the result counterpart of current selected item
  //! (in the target model)
  Standard_EXPORT Handle(Standard_Transient) ValueResult() const;
  
  //! Traces the application of a Modifier. Works with default trace
  //! File and Level. Fills the trace if default trace level is at
  //! least 1. Traces the Modifier (its Label) and its Selection if
  //! there is one (its Label).
  //! To be called after Select (because status IsForAll is printed)
  //! Worths to trace a global modification. See also Trace below
  Standard_EXPORT void TraceModifier (const Handle(IFSelect_GeneralModifier)& modif);
  
  //! Traces the modification of the current entity (see above,
  //! ValueOriginal and ValueResult) for default trace level >= 2.
  //! To be called on each indivudual entity really modified
  //! <mess> is an optionnal additional message
  Standard_EXPORT void Trace (const Standard_CString mess = "");
  
  //! Adds a Check to the CheckList. If it is empty, nothing is done
  //! If it concerns an Entity from the Original Model (by SetEntity)
  //! to which another Check is attached, it is merged to it.
  //! Else, it is added or merged as to GlobalCheck.
  Standard_EXPORT void AddCheck (const Handle(Interface_Check)& check);
  
  //! Adds a Warning Message for an Entity from the original Model
  //! If <start> is not an Entity from the original model (e.g. the
  //! model itself) this message is added to Global Check.
  Standard_EXPORT void AddWarning (const Handle(Standard_Transient)& start, const Standard_CString mess, const Standard_CString orig = "");
  
  //! Adds a Fail Message for an Entity from the original Model
  //! If <start> is not an Entity from the original model (e.g. the
  //! model itself) this message is added to Global Check.
  Standard_EXPORT void AddFail (const Handle(Standard_Transient)& start, const Standard_CString mess, const Standard_CString orig = "");
  
  //! Returns a Check given an Entity number (in the original Model)
  //! by default a Global Check. Creates it the first time.
  //! It can then be acknowledged on the spot, in condition that the
  //! caller works by reference ("Interface_Check& check = ...")
  Standard_EXPORT Handle(Interface_Check) CCheck (const Standard_Integer num = 0);
  
  //! Returns a Check attached to an Entity from the original Model
  //! It can then be acknowledged on the spot, in condition that the
  //! caller works by reference ("Interface_Check& check = ...")
  Standard_EXPORT Handle(Interface_Check) CCheck (const Handle(Standard_Transient)& start);
  
  //! Returns the complete CheckList
  Standard_EXPORT Interface_CheckIterator CheckList() const;




protected:





private:



  Interface_Graph thegraf;
  Handle(Interface_Protocol) theprot;
  Handle(Interface_CopyControl) themap;
  TCollection_AsciiString thefile;
  TCollection_AsciiString thelist;
  Interface_CheckIterator thechek;
  Standard_Boolean thesel;
  Standard_Integer thecurr;
  Standard_Integer thecurt;


};







#endif // _IFSelect_ContextModif_HeaderFile
