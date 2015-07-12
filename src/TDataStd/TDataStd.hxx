// Created on: 1995-05-10
// Created by: Denis PASCAL
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TDataStd_HeaderFile
#define _TDataStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_IDList.hxx>
#include <Standard_OStream.hxx>
#include <TDataStd_RealEnum.hxx>
class TDataStd_Current;
class TDataStd_Name;
class TDataStd_Comment;
class TDataStd_Integer;
class TDataStd_IntegerArray;
class TDataStd_Real;
class TDataStd_RealArray;
class TDataStd_ExtStringArray;
class TDataStd_UAttribute;
class TDataStd_TreeNode;
class TDataStd_ChildNodeIterator;
class TDataStd_Directory;
class TDataStd_NoteBook;
class TDataStd_Expression;
class TDataStd_Relation;
class TDataStd_Variable;
class TDataStd_DeltaOnModificationOfIntArray;
class TDataStd_DeltaOnModificationOfRealArray;
class TDataStd_DeltaOnModificationOfExtStringArray;
class TDataStd_DeltaOnModificationOfIntPackedMap;
class TDataStd_DeltaOnModificationOfByteArray;
class TDataStd_Tick;
class TDataStd_AsciiString;
class TDataStd_IntPackedMap;
class TDataStd_IntegerList;
class TDataStd_RealList;
class TDataStd_ExtStringList;
class TDataStd_BooleanList;
class TDataStd_ReferenceList;
class TDataStd_BooleanArray;
class TDataStd_ReferenceArray;
class TDataStd_ByteArray;
class TDataStd_NamedData;
class TDataStd_HDataMapOfStringInteger;
class TDataStd_HDataMapOfStringReal;
class TDataStd_HDataMapOfStringString;
class TDataStd_HDataMapOfStringByte;
class TDataStd_HDataMapOfStringHArray1OfInteger;
class TDataStd_HDataMapOfStringHArray1OfReal;


//! This  package  defines   standard attributes for
//! modelling.
//! These allow you to create and modify labels
//! and attributes for many basic data types.
//! Standard topological and visualization
//! attributes have also been created.
//! To find an attribute attached to a specific label,
//! you use the GUID of the type of attribute you
//! are looking for. To do this, first find this
//! information using the method GetID as follows: Standard_GUID anID =
//! MyAttributeClass::GetID();
//! Then, use the method Find for the label as follows:
//! Standard_Boolean HasAttribute
//! =
//! aLabel.Find(anID,anAttribute);
//! Note
//! For information on the relations between this
//! component of OCAF and the others, refer to the OCAF User's Guide.
class TDataStd 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Appends to <anIDList> the list of the attributes
  //! IDs of this package. CAUTION: <anIDList> is NOT
  //! cleared before use.
  Standard_EXPORT static void IDList (TDF_IDList& anIDList);
  
  //! Prints the name of the real dimension <DIM> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TDataStd_RealEnum DIM, Standard_OStream& S);




protected:





private:




friend class TDataStd_Current;
friend class TDataStd_Name;
friend class TDataStd_Comment;
friend class TDataStd_Integer;
friend class TDataStd_IntegerArray;
friend class TDataStd_Real;
friend class TDataStd_RealArray;
friend class TDataStd_ExtStringArray;
friend class TDataStd_UAttribute;
friend class TDataStd_TreeNode;
friend class TDataStd_ChildNodeIterator;
friend class TDataStd_Directory;
friend class TDataStd_NoteBook;
friend class TDataStd_Expression;
friend class TDataStd_Relation;
friend class TDataStd_Variable;
friend class TDataStd_DeltaOnModificationOfIntArray;
friend class TDataStd_DeltaOnModificationOfRealArray;
friend class TDataStd_DeltaOnModificationOfExtStringArray;
friend class TDataStd_DeltaOnModificationOfIntPackedMap;
friend class TDataStd_DeltaOnModificationOfByteArray;
friend class TDataStd_Tick;
friend class TDataStd_AsciiString;
friend class TDataStd_IntPackedMap;
friend class TDataStd_IntegerList;
friend class TDataStd_RealList;
friend class TDataStd_ExtStringList;
friend class TDataStd_BooleanList;
friend class TDataStd_ReferenceList;
friend class TDataStd_BooleanArray;
friend class TDataStd_ReferenceArray;
friend class TDataStd_ByteArray;
friend class TDataStd_NamedData;
friend class TDataStd_HDataMapOfStringInteger;
friend class TDataStd_HDataMapOfStringReal;
friend class TDataStd_HDataMapOfStringString;
friend class TDataStd_HDataMapOfStringByte;
friend class TDataStd_HDataMapOfStringHArray1OfInteger;
friend class TDataStd_HDataMapOfStringHArray1OfReal;

};







#endif // _TDataStd_HeaderFile
