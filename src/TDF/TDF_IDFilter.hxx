// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TDF_IDFilter_HeaderFile
#define _TDF_IDFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TDF_IDMap.hxx>
#include <TDF_IDList.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Attribute;


//! This class offers filtering services around an ID list.
class TDF_IDFilter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an ID/attribute filter based on an ID
  //! list. The default mode is "ignore all but...".
  //!
  //! This filter has 2 working mode: keep and ignore.
  //!
  //! Ignore/Exclusive mode: all IDs are ignored except
  //! these set to be kept, using Keep(). Of course, it
  //! is possible set an kept ID to be ignored using
  //! Ignore().
  //!
  //! Keep/Inclusive mode: all IDs are kept except these
  //! set to be ignored, using Ignore(). Of course, it
  //! is possible set an ignored ID to be kept using
  //! Keep().
  Standard_EXPORT TDF_IDFilter(const Standard_Boolean ignoreMode = Standard_True);
  
  //! The list of ID is cleared and the filter mode is
  //! set to ignore mode if <keep> is true; false
  //! otherwise.
  Standard_EXPORT void IgnoreAll (const Standard_Boolean ignore);
  
  //! Returns true is the mode is set to "ignore all
  //! but...".
    Standard_Boolean IgnoreAll() const;
  
  //! An attribute with <anID> as ID is to be kept and
  //! the filter will answer true to the question
  //! IsKept(<anID>).
  Standard_EXPORT void Keep (const Standard_GUID& anID);
  
  //! Attributes with ID owned by <anIDList> are to be kept and
  //! the filter will answer true to the question
  //! IsKept(<anID>) with ID from <anIDList>.
  Standard_EXPORT void Keep (const TDF_IDList& anIDList);
  
  //! An attribute with <anID> as ID is to be ignored and
  //! the filter will answer false to the question
  //! IsKept(<anID>).
  Standard_EXPORT void Ignore (const Standard_GUID& anID);
  
  //! Attributes with ID owned by <anIDList> are to be
  //! ignored and the filter will answer false to the
  //! question IsKept(<anID>) with ID from <anIDList>.
  Standard_EXPORT void Ignore (const TDF_IDList& anIDList);
  
  //! Returns true if the ID is to be kept.
    Standard_Boolean IsKept (const Standard_GUID& anID) const;
  
  //! Returns true if the attribute is to be kept.
    Standard_Boolean IsKept (const Handle(TDF_Attribute)& anAtt) const;
  
  //! Returns true if the ID is to be ignored.
    Standard_Boolean IsIgnored (const Standard_GUID& anID) const;
  
  //! Returns true if the attribute is to be ignored.
    Standard_Boolean IsIgnored (const Handle(TDF_Attribute)& anAtt) const;
  
  //! Copies the list of ID to be kept or ignored in
  //! <anIDList>. <anIDList> is cleared before use.
  Standard_EXPORT void IDList (TDF_IDList& anIDList) const;
  
  //! Copies into <me> the contents of
  //! <fromFilter>. <me> is cleared before copy.
  Standard_EXPORT void Copy (const TDF_IDFilter& fromFilter);
  
  //! Writes the contents of <me> to <OS>.
  Standard_EXPORT void Dump (Standard_OStream& anOS) const;

  //! Assignment
  void Assign (const TDF_IDFilter& theFilter)
  {
    myIgnore = theFilter.myIgnore;
    myIDMap  = theFilter.myIDMap;
  }

private:

  //! Private, to forbid implicit or hidden accesses to
  //! the copy constructor.
  TDF_IDFilter(const TDF_IDFilter& aFilter);
  TDF_IDFilter& operator= (const TDF_IDFilter& theOther);

private:

  Standard_Boolean myIgnore;
  TDF_IDMap myIDMap;

};


#include <TDF_IDFilter.lxx>





#endif // _TDF_IDFilter_HeaderFile
