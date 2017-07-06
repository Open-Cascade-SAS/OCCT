// Created by: Christian CAILLET <cky@phobox> 
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

#ifndef _Interface_InterfaceModel_HeaderFile
#define _Interface_InterfaceModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_IndexedMapOfTransient.hxx>
#include <TColStd_DataMapOfIntegerTransient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <Interface_DataState.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
class Interface_Check;
class TCollection_HAsciiString;
class Interface_GTool;
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Interface_InterfaceMismatch;
class Interface_Protocol;
class Standard_Transient;
class Interface_ReportEntity;
class Interface_CheckIterator;
class Interface_GeneralLib;
class Interface_EntityIterator;
class Message_Messenger;


class Interface_InterfaceModel;
DEFINE_STANDARD_HANDLE(Interface_InterfaceModel, Standard_Transient)

//! Defines an (Indexed) Set of data corresponding to a complete
//! Transfer by a File Interface, i.e. File Header and Transient
//! Entities (Objects) contained in a File. Contained Entities are
//! identified in the Model by unique and consecutive Numbers.
//!
//! In addition, a Model can attach to each entity, a specific
//! Label according to the norm (e.g. Name for VDA, #ident for
//! Step ...), intended to be output on a string or a stream
//! (remark : labels are not obliged to be unique)
//!
//! InterfaceModel itself is not Transient, it is intended to
//! work on a set of Transient Data. The services offered are
//! basic Listing and Identification operations on Transient
//! Entities, storage of Error Reports, Copying.
//!
//! Moreovere, it is possible to define and use templates. These
//! are empty Models, from which copies can be obtained in order
//! to be filled with effective data. This allows to record
//! standard definitions for headers, avoiding to recreate them
//! for each sendings, and assuring customisation of produced
//! files for a given site.
//! A template is attached to a name. It is possible to define a
//! template from another one (get it, edit it then record it
//! under another name).
//!
//! See also Graph, ShareTool, CheckTool for more
class Interface_InterfaceModel : public Standard_Transient
{

public:

  
  //! Clears the list of entities (service WhenDelete)
  Standard_EXPORT void Destroy();
~Interface_InterfaceModel()
{
  Destroy();
}
  
  //! Sets a Protocol for this Model
  //! It is also set by a call to AddWithRefs with Protocol
  //! It is used for : DumpHeader (as required), ClearEntities ...
  Standard_EXPORT void SetProtocol (const Handle(Interface_Protocol)& proto);
  
  //! Returns the Protocol which has been set by SetProtocol, or
  //! AddWithRefs with Protocol
  Standard_EXPORT virtual Handle(Interface_Protocol) Protocol() const;
  
  //! Sets a GTool for this model, which already defines a Protocol
  Standard_EXPORT void SetGTool (const Handle(Interface_GTool)& gtool);
  
  //! Returns the GTool, set by SetProtocol or by SetGTool
  Standard_EXPORT Handle(Interface_GTool) GTool() const;
  
  //! Returns the Dispatch Status, either for get or set
  //! A Model which is produced from Dispatch may share entities
  //! with the original (according to the Protocol), hence these
  //! non-copied entities should not be deleted
  Standard_EXPORT Standard_Boolean& DispatchStatus();
  
  //! Erases contained data; used when a Model is copied to others :
  //! the new copied ones begin from clear
  //! Clear calls specific method ClearHeader (see below)
  Standard_EXPORT virtual void Clear();
  
  //! Clears the entities; uses the general service WhenDelete, in
  //! addition to the standard Memory Manager; can be redefined
  Standard_EXPORT virtual void ClearEntities();
  
  //! Erases informations about labels, if any : specific to each
  //! norm
  Standard_EXPORT virtual void ClearLabels() = 0;
  
  //! Clears Model's header : specific to each norm
  Standard_EXPORT virtual void ClearHeader() = 0;
  
  //! Returns count of contained Entities
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! Returns True if a Model contains an Entity (for a ReportEntity,
  //! looks for the ReportEntity itself AND its Concerned Entity)
  Standard_EXPORT Standard_Boolean Contains (const Handle(Standard_Transient)& anentity) const;
  
  //! Returns the Number of an Entity in the Model if it contains it.
  //! Else returns 0. For a ReportEntity, looks at Concerned Entity.
  //! Returns the Directory entry   Number of  an Entity in
  //! the  Model if it contains it.   Else returns  0.  For a
  //! ReportEntity, looks at Concerned Entity.
  Standard_EXPORT Standard_Integer Number (const Handle(Standard_Transient)& anentity) const;
  
  //! Returns an Entity identified by its number in the Model
  //! Each sub-class of InterfaceModel can define its own method
  //! Entity to return its specific class of Entity (e.g. for VDA,
  //! VDAModel returns a VDAEntity), working by calling Value
  //! Remark : For a Reported Entity, (Erroneous, Corrected, Unknown), this
  //! method returns this Reported Entity.
  //! See ReportEntity for other questions.
  Standard_EXPORT const Handle(Standard_Transient)& Value (const Standard_Integer num) const;
  
  //! Returns the count of DISTINCT types under which an entity may
  //! be processed. Defined by the Protocol, which gives default as
  //! 1 (dynamic Type).
  Standard_EXPORT Standard_Integer NbTypes (const Handle(Standard_Transient)& ent) const;
  
  //! Returns a type, given its rank : defined by the Protocol
  //! (by default, the first one)
  Standard_EXPORT Handle(Standard_Type) Type (const Handle(Standard_Transient)& ent, const Standard_Integer num = 1) const;
  
  //! Returns the type name of an entity, from the list of types
  //! (one or more ...)
  //! <complete> True (D) gives the complete type, else packages are
  //! removed
  //! WARNING : buffered, to be immediately copied or printed
  Standard_EXPORT Standard_CString TypeName (const Handle(Standard_Transient)& ent, const Standard_Boolean complete = Standard_True) const;
  
  //! From a CDL Type Name, returns the Class part (package dropped)
  //! WARNING : buffered, to be immediately copied or printed
  Standard_EXPORT static Standard_CString ClassName (const Standard_CString typnam);
  
  //! Returns the State of an entity, given its number
  Standard_EXPORT Interface_DataState EntityState (const Standard_Integer num) const;
  
  //! Returns True if <num> identifies a ReportEntity in the Model
  //! Hence, ReportEntity can be called.
  //!
  //! By default, queries main report, if <semantic> is True, it
  //! queries report for semantic check
  //!
  //! Remember that a Report Entity can be defined for an Unknown
  //! Entity, or a Corrected or Erroneous (at read time) Entity.
  //! The ReportEntity is defined before call to method AddEntity.
  Standard_EXPORT Standard_Boolean IsReportEntity (const Standard_Integer num, const Standard_Boolean semantic = Standard_False) const;
  
  //! Returns a ReportEntity identified by its number in the Model,
  //! or a Null Handle If <num> does not identify a ReportEntity.
  //!
  //! By default, queries main report, if <semantic> is True, it
  //! queries report for semantic check
  Standard_EXPORT Handle(Interface_ReportEntity) ReportEntity (const Standard_Integer num, const Standard_Boolean semantic = Standard_False) const;
  
  //! Returns True if <num> identifies an Error Entity : in this
  //! case, a ReportEntity brings Fail Messages and possibly an
  //! "undefined" Content, see IsRedefinedEntity
  Standard_EXPORT Standard_Boolean IsErrorEntity (const Standard_Integer num) const;
  
  //! Returns True if <num> identifies an Entity which content is
  //! redefined through a ReportEntity (i.e. with literal data only)
  //! This happens when an entity is syntactically erroneous in the
  //! way that its basic content remains empty.
  //! For more details (such as content itself), see ReportEntity
  Standard_EXPORT Standard_Boolean IsRedefinedContent (const Standard_Integer num) const;
  
  //! Removes the ReportEntity attached to Entity <num>. Returns
  //! True if done, False if no ReportEntity was attached to <num>.
  //! Warning : the caller must assume that this clearing is meaningfull
  Standard_EXPORT Standard_Boolean ClearReportEntity (const Standard_Integer num);
  
  //! Sets or Replaces a ReportEntity for the Entity <num>. Returns
  //! True if Report is replaced, False if it has been replaced
  //! Warning : the caller must assume that this setting is meaningfull
  Standard_EXPORT Standard_Boolean SetReportEntity (const Standard_Integer num, const Handle(Interface_ReportEntity)& rep);
  
  //! Adds a ReportEntity as such. Returns False if the concerned
  //! entity is not recorded in the Model
  //! Else, adds it into, either the main report list or the
  //! list for semantic checks, then returns True
  Standard_EXPORT Standard_Boolean AddReportEntity (const Handle(Interface_ReportEntity)& rep, const Standard_Boolean semantic = Standard_False);
  
  //! Returns True if <num> identifies an Unknown Entity : in this
  //! case, a ReportEntity with no Check Messages designates it.
  Standard_EXPORT Standard_Boolean IsUnknownEntity (const Standard_Integer num) const;
  
  //! Fills the list of semantic checks.
  //! This list is computed (by CheckTool). Hence, it can be stored
  //! in the model for later queries
  //! <clear> True (D) : new list replaces
  //! <clear> False    : new list is cumulated
  Standard_EXPORT void FillSemanticChecks (const Interface_CheckIterator& checks, const Standard_Boolean clear = Standard_True);
  
  //! Returns True if semantic checks have been filled
  Standard_EXPORT Standard_Boolean HasSemanticChecks() const;
  
  //! Returns the check attached to an entity, designated by its
  //! Number. 0 for global check
  //! <semantic> True  : recorded semantic check
  //! <semantic> False : recorded syntactic check (see ReportEntity)
  //! If no check is recorded for <num>, returns an empty Check
  Standard_EXPORT const Handle(Interface_Check)& Check (const Standard_Integer num, const Standard_Boolean syntactic) const;
  
  //! Does a reservation for the List of Entities (for optimized
  //! storage management). If it is not called, storage management
  //! can be less efficient. <nbent> is the expected count of
  //! Entities to store
  Standard_EXPORT virtual void Reservate (const Standard_Integer nbent);
  
  //! Internal method for adding an Entity. Used by file reading
  //! (defined by each Interface) and Transfer tools. It adds the
  //! entity required to be added, not its refs : see AddWithRefs.
  //! If <anentity> is a ReportEntity, it is added to the list of
  //! Reports, its Concerned Entity (Erroneous or Corrected, else
  //! Unknown) is added to the list of Entities.
  //! That is, the ReportEntity must be created before Adding
  Standard_EXPORT virtual void AddEntity (const Handle(Standard_Transient)& anentity);
  
  //! Adds to the Model, an Entity with all its References, as they
  //! are defined by General Services FillShared and ListImplied.
  //! Process is recursive (any sub-levels) if <level> = 0 (Default)
  //! Else, adds sub-entities until the required sub-level.
  //! Especially, if <level> = 1, adds immediate subs and that's all
  //!
  //! If <listall> is False (Default), an entity (<anentity> itself
  //! or one of its subs at any level) which is already recorded in
  //! the Model is not analysed, only the newly added ones are.
  //! If <listall> is True, all items are analysed (this allows to
  //! ensure the consistency of an adding made by steps)
  Standard_EXPORT void AddWithRefs (const Handle(Standard_Transient)& anent, const Handle(Interface_Protocol)& proto, const Standard_Integer level = 0, const Standard_Boolean listall = Standard_False);
  
  //! Same as above, but works with the Protocol of the Model
  Standard_EXPORT void AddWithRefs (const Handle(Standard_Transient)& anent, const Standard_Integer level = 0, const Standard_Boolean listall = Standard_False);
  
  //! Same as above, but works with an already created GeneralLib
  Standard_EXPORT void AddWithRefs (const Handle(Standard_Transient)& anent, const Interface_GeneralLib& lib, const Standard_Integer level = 0, const Standard_Boolean listall = Standard_False);
  
  //! Replace Entity with Number=nument on other entity - "anent"
  Standard_EXPORT void ReplaceEntity (const Standard_Integer nument, const Handle(Standard_Transient)& anent);
  
  //! Reverses the Numbers of the Entities, between <after> and the
  //! total count of Entities. Thus, the entities :
  //! 1,2 ... after, after+1 ... nb-1, nb  become numbered as :
  //! 1,2 ... after, nb, nb-1 ... after+1
  //! By default (after = 0) the whole list of Entities is reversed
  Standard_EXPORT void ReverseOrders (const Standard_Integer after = 0);
  
  //! Changes the Numbers of some Entities : <oldnum> is moved to
  //! <newnum>, same for <count> entities. Thus :
  //! 1,2 ... newnum-1 newnum ... oldnum .. oldnum+count oldnum+count+1 .. gives
  //! 1,2 ... newnum-1 oldnum .. oldnum+count newnum ... oldnum+count+1
  //! (can be seen as a circular permutation)
  Standard_EXPORT void ChangeOrder (const Standard_Integer oldnum, const Standard_Integer newnum, const Standard_Integer count = 1);
  
  //! Gets contents from an EntityIterator, prepared by a
  //! Transfer tool (e.g TransferCopy). Starts from clear
  Standard_EXPORT void GetFromTransfer (const Interface_EntityIterator& aniter);
  
  //! Gets header (data specific of a defined Interface) from
  //! another InterfaceModel; called from TransferCopy
  Standard_EXPORT virtual void GetFromAnother (const Handle(Interface_InterfaceModel)& other) = 0;
  
  //! Returns a New Empty Model, same type as <me> (whatever its
  //! Type); called to Copy parts a Model into other ones, then
  //! followed by a call to GetFromAnother (Header) then filling
  //! with specified Entities, themselves copied
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewEmptyModel() const = 0;
  
  //! Records a category number for an entity number
  //! Returns True when done, False if <num> is out of range
  Standard_EXPORT Standard_Boolean SetCategoryNumber (const Standard_Integer num, const Standard_Integer val);
  
  //! Returns the recorded category number for a given entity number
  //! 0 if none was defined for this entity
  Standard_EXPORT Standard_Integer CategoryNumber (const Standard_Integer num) const;
  
  //! Allows an EntityIterator to get a list of Entities
  Standard_EXPORT void FillIterator (Interface_EntityIterator& iter) const;
  
  //! Returns the list of all Entities, as an Iterator on Entities
  //! (the Entities themselves, not the Reports)
  Standard_EXPORT Interface_EntityIterator Entities() const;
  
  //! Returns the list of all ReportEntities, i.e. data about
  //! Entities read with Error or Warning informations
  //! (each item has to be casted to Report Entity then it can be
  //! queried for Concerned Entity, Content, Check ...)
  //! By default, returns the main reports, is <semantic> is True it
  //! returns the list for sematic checks
  Standard_EXPORT Interface_EntityIterator Reports (const Standard_Boolean semantic = Standard_False) const;
  
  //! Returns the list of ReportEntities which redefine data
  //! (generally, if concerned entity is "Error", a literal content
  //! is added to it : this is a "redefined entity"
  Standard_EXPORT Interface_EntityIterator Redefineds() const;
  
  //! Returns the GlobalCheck, which memorizes messages global to
  //! the file (not specific to an Entity), especially Header
  Standard_EXPORT const Handle(Interface_Check)& GlobalCheck (const Standard_Boolean syntactic = Standard_True) const;
  
  //! Allows to modify GlobalCheck, after getting then completing it
  //! Remark : it is SYNTACTIC check. Semantics, see FillChecks
  Standard_EXPORT void SetGlobalCheck (const Handle(Interface_Check)& ach);
  
  //! Minimum Semantic Global Check on data in model (header)
  //! Can only check basic Data. See also GlobalCheck from Protocol
  //! for a check which takes the Graph into account
  //! Default does nothing, can be redefined
  Standard_EXPORT virtual void VerifyCheck (Handle(Interface_Check)& ach) const;
  
  //! Dumps Header in a short, easy to read, form, onto a Stream
  //! <level> allows to print more or less parts of the header,
  //! if necessary. 0 for basic print
  Standard_EXPORT virtual void DumpHeader (const Handle(Message_Messenger)& S, const Standard_Integer level = 0) const = 0;
  
  //! Prints identification of a given entity in <me>, in order to
  //! be printed in a list or phrase
  //! <mode> < 0 : prints only its number
  //! <mode> = 1 : just calls PrintLabel
  //! <mode> = 0 (D) : prints its number plus '/' plus PrintLabel
  //! If <ent> == <me>, simply prints "Global"
  //! If <ent> is unknown, prints "??/its type"
  Standard_EXPORT void Print (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& s, const Standard_Integer mode = 0) const;
  
  //! Prints label specific to each norm, for a given entity.
  //! Must only print label itself, in order to be included in a
  //! phrase. Can call the result of StringLabel, but not obliged.
  Standard_EXPORT virtual void PrintLabel (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const = 0;
  
  //! Prints label specific to each norm in log format, for
  //! a given entity.
  //! By default, just calls PrintLabel, can be redefined
  Standard_EXPORT virtual void PrintToLog (const Handle(Standard_Transient)& ent, const Handle(Message_Messenger)& S) const;
  
  //! Returns a string with the label attached to a given entity.
  //! Warning : While this string may be edited on the spot, if it is a read
  //! field, the returned value must be copied before.
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) StringLabel (const Handle(Standard_Transient)& ent) const = 0;
  
  //! Searches a label which matches with one entity.
  //! Begins from <lastnum>+1 (default:1) and scans the entities
  //! until <NbEntities>. For the first which matches <label>,
  //! this method returns its Number. Returns 0 if nothing found
  //! Can be called recursively (labels are not specified as unique)
  //! <exact> : if True (default), exact match is required
  //! else, checks the END of entity label
  //!
  //! This method is virtual, hence it can be redefined for a more
  //! efficient search (if exact is true).
  Standard_EXPORT virtual Standard_Integer NextNumberForLabel (const Standard_CString label, const Standard_Integer lastnum = 0, const Standard_Boolean exact = Standard_True) const;
  
  //! Returns true if a template is attached to a given name
  Standard_EXPORT static Standard_Boolean HasTemplate (const Standard_CString name);
  
  //! Returns the template model attached to a name, or a Null Handle
  Standard_EXPORT static Handle(Interface_InterfaceModel) Template (const Standard_CString name);
  
  //! Records a new template model with a name. If the name was
  //! already recorded, the corresponding template is replaced by
  //! the new one. Then, WARNING : test HasTemplate to avoid
  //! surprises
  Standard_EXPORT static Standard_Boolean SetTemplate (const Standard_CString name, const Handle(Interface_InterfaceModel)& model);
  
  //! Returns the complete list of names attached to template models
  Standard_EXPORT static Handle(TColStd_HSequenceOfHAsciiString) ListTemplates();



  DEFINE_STANDARD_RTTIEXT(Interface_InterfaceModel,Standard_Transient)

protected:

  
  //! Defines empty InterfaceModel, ready to be filled
  Standard_EXPORT Interface_InterfaceModel();



private:


  TColStd_IndexedMapOfTransient theentities;
  TColStd_DataMapOfIntegerTransient thereports;
  TColStd_DataMapOfIntegerTransient therepch;
  Handle(Interface_Check) thecheckstx;
  Handle(Interface_Check) thechecksem;
  Standard_Boolean haschecksem;
  Standard_Boolean isdispatch;
  Handle(TCollection_HAsciiString) thecategory;
  Handle(Interface_GTool) thegtool;


};







#endif // _Interface_InterfaceModel_HeaderFile
