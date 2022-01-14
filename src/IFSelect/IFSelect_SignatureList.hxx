// Created on: 1994-10-27
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

#ifndef _IFSelect_SignatureList_HeaderFile
#define _IFSelect_SignatureList_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <IFSelect_PrintCount.hxx>
#include <NCollection_IndexedDataMap.hxx>
class TCollection_HAsciiString;
class Interface_InterfaceModel;


class IFSelect_SignatureList;
DEFINE_STANDARD_HANDLE(IFSelect_SignatureList, Standard_Transient)

//! A SignatureList is given as result from a Counter (any kind)
//! It gives access to a list of signatures, with counts, and
//! optionally with list of corresponding entities
//!
//! It can also be used only to give a signature, through SignOnly
//! Mode. This can be useful for a specific counter (used in a
//! Selection), while it remains better to use a Signature
//! whenever possible
class IFSelect_SignatureList : public Standard_Transient
{

public:

  
  //! Creates a SignatureList. If <withlist> is True, entities will
  //! be not only counted per signature, but also listed.
  Standard_EXPORT IFSelect_SignatureList(const Standard_Boolean withlist = Standard_False);
  
  //! Changes the record-list status. The list is not cleared but
  //! its use changes
  Standard_EXPORT void SetList (const Standard_Boolean withlist);
  
  //! Returns modifiable the SignOnly Mode
  //! If False (D), the counter normally counts
  //! If True, the counting work is turned off, Add only fills the
  //! LastValue, which can be used as signature, when a counter
  //! works from data which are not available from a Signature
  Standard_EXPORT Standard_Boolean& ModeSignOnly();
  
  Standard_EXPORT virtual void Clear();
  
  //! Adds an entity with its signature, i.e. :
  //! - counts an item more for <sign>
  //! - if record-list status is set, records the entity
  //! Accepts a null entity (the signature is then for the global
  //! model). But if the string is empty, counts a Null item.
  //!
  //! If SignOnly Mode is set, this work is replaced by just
  //! setting LastValue
  Standard_EXPORT void Add (const Handle(Standard_Transient)& ent, const Standard_CString sign);
  
  //! Returns the last value recorded by Add (only if SignMode set)
  //! Cleared by Clear or Init
  Standard_EXPORT Standard_CString LastValue() const;
  
  //! Aknowledges the list in once. Name identifies the Signature
  Standard_EXPORT void Init (const Standard_CString name, const NCollection_IndexedDataMap<TCollection_AsciiString, Standard_Integer>& count, const NCollection_IndexedDataMap<TCollection_AsciiString, Handle(Standard_Transient)>& list, const Standard_Integer nbnuls);
  
  //! Returns the list of signatures, as a sequence of strings
  //! (but without their respective counts). It is ordered.
  //! By default, for all the signatures.
  //! If <root> is given non empty, for the signatures which
  //! begin by <root>
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) List (const Standard_CString root = "") const;
  
  //! Returns True if the list of Entities is aknowledged, else
  //! the method Entities will always return a Null Handle
  Standard_EXPORT Standard_Boolean HasEntities() const;
  
  //! Returns the count of null entities
  Standard_EXPORT Standard_Integer NbNulls() const;
  
  //! Returns the number of times a signature was counted,
  //! 0 if it has not been recorded at all
  Standard_EXPORT Standard_Integer NbTimes (const Standard_CString sign) const;
  
  //! Returns the list of entities attached to a signature
  //! It is empty if <sign> has not been recorded
  //! It is a Null Handle if the list of entities is not known
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Entities (const Standard_CString sign) const;
  
  //! Defines a name for a SignatureList (used to print it)
  Standard_EXPORT void SetName (const Standard_CString name);
  
  //! Returns the recorded Name.
  //! Remark : default is "..." (no SetName called)
  Standard_EXPORT virtual Standard_CString Name() const;
  
  //! Prints the counts of items (not the list)
  Standard_EXPORT virtual void PrintCount (Standard_OStream& S) const;
  
  //! Prints the lists of items, if they are present (else, prints
  //! a message "no list available")
  //! Uses <model> to determine for each entity to be listed, its
  //! number, and its specific identifier (by PrintLabel)
  //! <mod> gives a mode for printing :
  //! - CountByItem : just count (as PrintCount)
  //! - ShortByItem : minimum i.e. count plus 5 first entity numbers
  //! - ShortByItem(D) complete list of entity numbers (0: "Global")
  //! - EntitiesByItem : list of (entity number/PrintLabel from the model)
  //! other modes are ignored
  Standard_EXPORT virtual void PrintList (Standard_OStream& S, const Handle(Interface_InterfaceModel)& model, const IFSelect_PrintCount mod = IFSelect_ListByItem) const;
  
  //! Prints a summary
  //! Item which has the greatest count of entities
  //! For items which are numeric values : their count, maximum,
  //! minimum values, cumul, average
  Standard_EXPORT virtual void PrintSum (Standard_OStream& S) const;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SignatureList,Standard_Transient)

protected:




private:


  Standard_Boolean thesignonly;
  Standard_Boolean thelistat;
  Standard_Integer thenbnuls;
  Handle(TCollection_HAsciiString) thename;
  TCollection_AsciiString thelastval;
  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_Integer> thedicount;
  NCollection_IndexedDataMap<TCollection_AsciiString, Handle(Standard_Transient)> thediclist;


};







#endif // _IFSelect_SignatureList_HeaderFile
