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

#ifndef _Transfer_Finder_HeaderFile
#define _Transfer_Finder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_Real.hxx>
#include <TCollection_AsciiString.hxx>



class Transfer_Finder;
DEFINE_STANDARD_HANDLE(Transfer_Finder, Standard_Transient)

//! a Finder allows to map any kind of object as a Key for a Map.
//! This works by defining, for a Hash Code, that of the real Key,
//! not of the Finder which acts only as an intermediate.
//! When a Map asks for the HashCode of a Finder, this one returns
//! the code it has determined at creation time
class Transfer_Finder : public Standard_Transient
{

public:

  
  //! Returns the HashCode which has been stored by SetHashCode
  //! (remark that HashCode could be deferred then be defined by
  //! sub-classes, the result is the same)
  Standard_EXPORT Standard_Integer GetHashCode() const;
  
  //! Specific testof equality : to be defined by each sub-class,
  //! must be False if Finders have not the same true Type, else
  //! their contents must be compared
  Standard_EXPORT virtual Standard_Boolean Equates (const Handle(Transfer_Finder)& other) const = 0;
  
  //! Returns the Type of the Value. By default, returns the
  //! DynamicType of <me>, but can be redefined
  Standard_EXPORT virtual Handle(Standard_Type) ValueType() const;
  
  //! Returns the name of the Type of the Value. Default is name
  //! of ValueType, unless it is for a non-handled object
  Standard_EXPORT virtual Standard_CString ValueTypeName() const;
  
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
  
  //! Returns an attribute from its name. Null Handle if not recorded
  //! (whatever Transient, Integer, Real ...)
  Standard_EXPORT Handle(Standard_Transient) Attribute (const Standard_CString name) const;
  
  //! Returns the type of an attribute :
  //! ParamInt , ParamReal , ParamText (String) , ParamIdent (any)
  //! or ParamVoid (not recorded)
  Standard_EXPORT Interface_ParamType AttributeType (const Standard_CString name) const;
  
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
  Standard_EXPORT NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& AttrList();
  
  //! Gets the list of attributes from <other>, as such, i.e.
  //! not copied : attributes are shared, any attribute edited,
  //! added, or removed in <other> is also in <me> and vice versa
  //! The former list of attributes of <me> is dropped
  Standard_EXPORT void SameAttributes (const Handle(Transfer_Finder)& other);
  
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
  Standard_EXPORT void GetAttributes (const Handle(Transfer_Finder)& other, const Standard_CString fromname = "", const Standard_Boolean copied = Standard_True);




  DEFINE_STANDARD_RTTIEXT(Transfer_Finder,Standard_Transient)

protected:

  
  //! Stores the HashCode which corresponds to the Value given to
  //! create the Mapper
  Standard_EXPORT void SetHashCode (const Standard_Integer code);



private:


  Standard_Integer thecode;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> theattrib;


};







#endif // _Transfer_Finder_HeaderFile
