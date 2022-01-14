// Created on: 1994-11-04
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

#ifndef _MoniTool_AttrList_HeaderFile
#define _MoniTool_AttrList_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <MoniTool_ValueType.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! a AttrList allows to record a list of attributes as Transients
//! which can be edited, changed ...
//! Each one is identified by a name
class MoniTool_AttrList 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an AttrList, empty
  Standard_EXPORT MoniTool_AttrList();
  
  //! Creates an AttrList from another one, definitions are shared
  //! (calls SameAttributes)
  Standard_EXPORT MoniTool_AttrList(const MoniTool_AttrList& other);
  
  //! Adds an attribute with a given name (replaces the former one
  //! with the same name if already exists)
  Standard_EXPORT void SetAttribute (const Standard_CString name, const Handle(Standard_Transient)& val);
  
  //! Removes an attribute
  //! Returns True when done, False if this attribute did not exist
  Standard_EXPORT Standard_Boolean RemoveAttribute (const Standard_CString name);
  
  //! Returns an attribute from its name, filtered by a type
  //! If no attribute has this name, or if it is not kind of this
  //! type, <val> is Null and returned value is False
  //! Else, it is True
  Standard_EXPORT Standard_Boolean GetAttribute (const Standard_CString name, const Handle(Standard_Type)& type, Handle(Standard_Transient)& val) const;
  
  //! Returns an attribute from its name. Null Handle if not
  //! recorded         (whatever Transient, Integer, Real ...)
  //! Integer is recorded as IntVal
  //! Real is recorded as RealVal
  //! Text is recorded as HAsciiString
  Standard_EXPORT Handle(Standard_Transient) Attribute (const Standard_CString name) const;
  
  //! Returns the type of an attribute :
  //! ValueInt , ValueReal , ValueText (String) , ValueIdent (any)
  //! or ValueVoid (not recorded)
  Standard_EXPORT MoniTool_ValueType AttributeType (const Standard_CString name) const;
  
  //! Adds an integer value for an attribute
  Standard_EXPORT void SetIntegerAttribute (const Standard_CString name, const Standard_Integer val);
  
  //! Returns an attribute from its name, as integer
  //! If no attribute has this name, or not an integer,
  //! <val> is 0 and returned value is False
  //! Else, it is True
  Standard_EXPORT Standard_Boolean GetIntegerAttribute (const Standard_CString name, Standard_Integer& val) const;
  
  //! Returns an integer attribute from its name. 0 if not recorded
  Standard_EXPORT Standard_Integer IntegerAttribute (const Standard_CString name) const;
  
  //! Adds a real value for an attribute
  Standard_EXPORT void SetRealAttribute (const Standard_CString name, const Standard_Real val);
  
  //! Returns an attribute from its name, as real
  //! If no attribute has this name, or not a real
  //! <val> is 0.0 and returned value is False
  //! Else, it is True
  Standard_EXPORT Standard_Boolean GetRealAttribute (const Standard_CString name, Standard_Real& val) const;
  
  //! Returns a real attribute from its name. 0.0 if not recorded
  Standard_EXPORT Standard_Real RealAttribute (const Standard_CString name) const;
  
  //! Adds a String value for an attribute
  Standard_EXPORT void SetStringAttribute (const Standard_CString name, const Standard_CString val);
  
  //! Returns an attribute from its name, as String
  //! If no attribute has this name, or not a String
  //! <val> is 0.0 and returned value is False
  //! Else, it is True
  Standard_EXPORT Standard_Boolean GetStringAttribute (const Standard_CString name, Standard_CString& val) const;
  
  //! Returns a String attribute from its name. "" if not recorded
  Standard_EXPORT Standard_CString StringAttribute (const Standard_CString name) const;
  
  //! Returns the exhaustive list of attributes
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& AttrList() const;
  
  //! Gets the list of attributes from <other>, as such, i.e.
  //! not copied : attributes are shared, any attribute edited,
  //! added, or removed in <other> is also in <me> and vice versa
  //! The former list of attributes of <me> is dropped
  Standard_EXPORT void SameAttributes (const MoniTool_AttrList& other);
  
  //! Gets the list of attributes from <other>, by copying it
  //! By default, considers all the attributes from <other>
  //! If <fromname> is given, considers only the attributes with
  //! name beginning by <fromname>
  //!
  //! For each attribute, if <copied> is True (D), its value is also
  //! copied if it is a basic type (Integer,Real,String), else it
  //! remains shared between <other> and <me>
  //!
  //! These new attributes are added to the existing ones in <me>,
  //! in case of same name, they replace the existing ones
  Standard_EXPORT void GetAttributes (const MoniTool_AttrList& other, const Standard_CString fromname = "", const Standard_Boolean copied = Standard_True);




protected:





private:



  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> theattrib;


};







#endif // _MoniTool_AttrList_HeaderFile
