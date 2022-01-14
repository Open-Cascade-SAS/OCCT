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

#ifndef _Interface_CopyTool_HeaderFile
#define _Interface_CopyTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_BitMap.hxx>
#include <Interface_GeneralLib.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfInteger.hxx>
class Interface_InterfaceModel;
class Interface_CopyControl;
class Interface_CopyMap;
class Standard_Transient;
class Interface_GeneralModule;
class Interface_Protocol;
class Interface_EntityIterator;


//! Performs Deep Copies of sets of Entities
//! Allows to perform Copy of Interface Entities from a Model to
//! another one. Works by calling general services GetFromAnother
//! and GetImplied.
//! Uses a CopyMap to bind a unique Result to each Copied Entity
//!
//! It is possible to command Copies of Entities (and those they
//! reference) by call to the General Service Library, or to
//! enforce results for transfer of some Entities (calling Bind)
//!
//! A Same CopyTool can be used for several successive Copies from
//! the same Model : either by restarting from scratch (e.g. to
//! copy different parts of a starting Model to several Targets),
//! or incremental : in that case, it is possible to know what is
//! the content of the last increment (defined by last call to
//! ClearLastFlags  and queried by call to LastCopiedAfter)
//!
//! Works in two times : first, create the list of copied Entities
//! second, pushes them to a target Model (manages also Model's
//! Header) or returns the Result as an Iterator, as desired
//!
//! The core action (Copy) works by using ShallowCopy (method
//! attached to each class) and Copy from GeneralLib (itself using
//! dedicated tools). It can be redefined for specific actions.
class Interface_CopyTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a CopyTool adapted to work from a Model. Works
  //! with a General Service Library, given as an argument
  Standard_EXPORT Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel, const Interface_GeneralLib& lib);
  
  //! Same as above, but Library is defined through a Protocol
  Standard_EXPORT Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& protocol);
  
  //! Same as above, but works with the Active Protocol
  Standard_EXPORT Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel);
  
  //! Returns the Model on which the CopyTool works
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Changes the Map of Result for another one. This allows to work
  //! with a more sophisticated Mapping Control than the Standard
  //! one which is CopyMap (e.g. TransferProcess from Transfer)
  Standard_EXPORT void SetControl (const Handle(Interface_CopyControl)& othermap);
  
  //! Returns the object used for Control
  Standard_EXPORT Handle(Interface_CopyControl) Control() const;
  
  //! Clears Transfer List. Gets Ready to begin another Transfer
  Standard_EXPORT virtual void Clear();
  
  //! Creates the CounterPart of an Entity (by ShallowCopy), Binds
  //! it, then Copies the content of the former Entity to the other
  //! one (same Type), by call to the General Service Library
  //! It may command the Copy of Referenced Entities
  //! Then, its returns True.
  //!
  //! If <mapped> is True, the Map is used to store the Result
  //! Else, the Result is simply produced : it can be used to Copy
  //! internal sub-parts of Entities, which are not intended to be
  //! shared (Strings, Arrays, etc...)
  //! If <errstat> is True, this means that the Entity is recorded
  //! in the Model as Erroneous : in this case, the General Service
  //! for Deep Copy is not called (this could be dangerous) : hence
  //! the Counter-Part is produced but empty, it can be referenced.
  //!
  //! This method does nothing and returns False if the Protocol
  //! does not recognize <ent>.
  //! It basically makes a Deep Copy without changing the Types.
  //! It can be redefined for special uses.
  Standard_EXPORT virtual Standard_Boolean Copy (const Handle(Standard_Transient)& entfrom, Handle(Standard_Transient)& entto, const Standard_Boolean mapped, const Standard_Boolean errstat);
  
  //! Transfers one Entity, if not yet bound to a result
  //! Remark : For an Entity which is reported in the Starting Model,
  //! the ReportEntity will also be copied with its Content if it
  //! has one (at least ShallowCopy; Complete Copy if the Protocol
  //! recognizes the Content : see method Copy)
  Standard_EXPORT Handle(Standard_Transient) Transferred (const Handle(Standard_Transient)& ent);
  
  //! Defines a Result for the Transfer of a Starting object.
  //! Used by method Transferred (which performs a normal Copy),
  //! but can also be called to enforce a result : in the latter
  //! case, the enforced result must be compatible with the other
  //! Transfers which are performed
  Standard_EXPORT void Bind (const Handle(Standard_Transient)& ent, const Handle(Standard_Transient)& res);
  
  //! Search for the result of a Starting Object (i.e. an Entity)
  //! Returns True  if a  Result is Bound (and fills "result")
  //! Returns False if no result is Bound
  Standard_EXPORT Standard_Boolean Search (const Handle(Standard_Transient)& ent, Handle(Standard_Transient)& res) const;
  
  //! Clears LastFlags only. This allows to know what Entities are
  //! copied after its call (see method LastCopiedAfter). It can be
  //! used when copies are done by increments, which must be
  //! distinghished. ClearLastFlags is also called by Clear.
  Standard_EXPORT void ClearLastFlags();
  
  //! Returns an copied Entity and its Result which were operated
  //! after last call to ClearLastFlags. It returns the first
  //! "Last Copied Entity" which Number follows <numfrom>, Zero if
  //! none. It is used in a loop as follow :
  //! Integer num = 0;
  //! while ( (num = CopyTool.LastCopiedAfter(num,ent,res)) ) {
  //! .. Process Starting <ent> and its Result <res>
  //! }
  Standard_EXPORT Standard_Integer LastCopiedAfter (const Standard_Integer numfrom, Handle(Standard_Transient)& ent, Handle(Standard_Transient)& res) const;
  
  //! Transfers one Entity and records result into the Transfer List
  //! Calls method Transferred
  Standard_EXPORT void TransferEntity (const Handle(Standard_Transient)& ent);
  
  //! Renews the Implied References. These References do not involve
  //! Copying of referenced Entities. For such a Reference, if the
  //! Entity which defines it AND the referenced Entity are both
  //! copied, then this Reference is renewed. Else it is deleted in
  //! the copied Entities.
  //! Remark : this concerns only some specific references, such as
  //! "back pointers".
  Standard_EXPORT void RenewImpliedRefs();
  
  //! Fills a Model with the result of the transfer (TransferList)
  //! Commands copy of Header too, and calls RenewImpliedRefs
  Standard_EXPORT void FillModel (const Handle(Interface_InterfaceModel)& bmodel);
  
  //! Returns the complete list of copied Entities
  //! If <withreports> is given True, the entities which were
  //! reported in the Starting Model are replaced in the list
  //! by the copied ReportEntities
  Standard_EXPORT Interface_EntityIterator CompleteResult (const Standard_Boolean withreports = Standard_False) const;
  
  //! Returns the list of Root copied Entities (those which were
  //! asked for copy by the user of CopyTool, not by copying
  //! another Entity)
  Standard_EXPORT Interface_EntityIterator RootResult (const Standard_Boolean withreports = Standard_False) const;
  Standard_EXPORT virtual ~Interface_CopyTool();




protected:

  
  //! Creates a new void instance (just created) of the same class
  //! as <entfrom>. Uses the general service GeneralModule:NewVoid
  //! Returns True if OK (Recognize has succeeded), False else
  //! (in such a case, the standard method ShallowCopy is called
  //! to produce <ento> from <entfrom> : hence it is not void)
  //!
  //! No mapping is managed by this method
  Standard_EXPORT virtual Standard_Boolean NewVoid (const Handle(Standard_Transient)& entfrom, Handle(Standard_Transient)& entto);


  Interface_GeneralLib thelib;


private:

  
  //! Renews the Implied References of one already Copied Entity
  Standard_EXPORT virtual void Implied (const Handle(Standard_Transient)& entfrom, const Handle(Standard_Transient)& entto);


  Handle(Interface_InterfaceModel) themod;
  Handle(Interface_CopyControl) themap;
  Handle(Interface_CopyMap) therep;
  Interface_BitMap thelst;
  Standard_Integer thelev;
  TColStd_SequenceOfInteger therts;
  Standard_Boolean theimp;
  Handle(Standard_Transient) theent;
  Handle(Interface_GeneralModule) themdu;
  Standard_Integer theCN;


};







#endif // _Interface_CopyTool_HeaderFile
