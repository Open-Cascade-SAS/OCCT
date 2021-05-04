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

#ifndef _TDF_HeaderFile
#define _TDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class Standard_GUID;
class TCollection_ExtendedString;
class TDF_Data;
class TDF_Label;
class TDF_Attribute;
class TDF_TagSource;
class TDF_Reference;
class TDF_ClosureMode;
class TDF_ClosureTool;
class TDF_CopyTool;
class TDF_CopyLabel;
class TDF_ComparisonTool;
class TDF_Transaction;
class TDF_Delta;
class TDF_AttributeDelta;
class TDF_DeltaOnAddition;
class TDF_DeltaOnForget;
class TDF_DeltaOnResume;
class TDF_DeltaOnRemoval;
class TDF_DeltaOnModification;
class TDF_DefaultDeltaOnRemoval;
class TDF_DefaultDeltaOnModification;
class TDF_ChildIterator;
class TDF_ChildIDIterator;
class TDF_DataSet;
class TDF_RelocationTable;
class TDF_Tool;
class TDF_LabelMapHasher;
class TDF_IDFilter;


//! This package provides data framework for binding
//! features and data structures.
//!
//! The feature structure is a tree used to bind
//! semantic information about each feature together.
//!
//! The only one concrete   attribute defined in  this
//! package is the TagSource attribute.This attribute
//! is used for  random creation of child labels under
//! a given label. Tags are randomly delivered.
class TDF 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns ID "00000000-0000-0000-0000-000000000000",
  //! sometimes used as null ID.
  Standard_EXPORT static const Standard_GUID& LowestID();
  
  //! Returns ID "ffffffff-ffff-ffff-ffff-ffffffffffff".
  Standard_EXPORT static const Standard_GUID& UppestID();
  
  //! Sets link between GUID and ProgID in hidden DataMap
  Standard_EXPORT static void AddLinkGUIDToProgID (const Standard_GUID& ID, const TCollection_ExtendedString& ProgID);
  
  //! Returns True if there is GUID for given <ProgID> then GUID is returned in <ID>
  Standard_EXPORT static Standard_Boolean GUIDFromProgID (const TCollection_ExtendedString& ProgID, Standard_GUID& ID);
  
  //! Returns True if there is ProgID for given <ID> then ProgID is returned in <ProgID>
  Standard_EXPORT static Standard_Boolean ProgIDFromGUID (const Standard_GUID& ID, TCollection_ExtendedString& ProgID);




protected:





private:




friend class TDF_Data;
friend class TDF_Label;
friend class TDF_Attribute;
friend class TDF_TagSource;
friend class TDF_Reference;
friend class TDF_ClosureMode;
friend class TDF_ClosureTool;
friend class TDF_CopyTool;
friend class TDF_CopyLabel;
friend class TDF_ComparisonTool;
friend class TDF_Transaction;
friend class TDF_Delta;
friend class TDF_AttributeDelta;
friend class TDF_DeltaOnAddition;
friend class TDF_DeltaOnForget;
friend class TDF_DeltaOnResume;
friend class TDF_DeltaOnRemoval;
friend class TDF_DeltaOnModification;
friend class TDF_DefaultDeltaOnRemoval;
friend class TDF_DefaultDeltaOnModification;
friend class TDF_ChildIterator;
friend class TDF_ChildIDIterator;
friend class TDF_DataSet;
friend class TDF_RelocationTable;
friend class TDF_Tool;
friend class TDF_LabelMapHasher;
friend class TDF_IDFilter;

};







#endif // _TDF_HeaderFile
