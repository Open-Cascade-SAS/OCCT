// Created on: 1998-02-23
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _MoniTool_TypedValue_HeaderFile
#define _MoniTool_TypedValue_HeaderFile

#include <Standard.hxx>

#include <MoniTool_ValueType.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfAsciiString.hxx>
#include <MoniTool_ValueInterpret.hxx>
#include <MoniTool_ValueSatisfies.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
class TCollection_HAsciiString;

class MoniTool_TypedValue;
DEFINE_STANDARD_HANDLE(MoniTool_TypedValue, Standard_Transient)

//! This class allows to dynamically manage .. typed values, i.e.
//! values which have an alphanumeric expression, but with
//! controls. Such as "must be an Integer" or "Enumerative Text"
//! etc
//!
//! Hence, a TypedValue brings a specification (type + constraints
//! if any) and a value. Its basic form is a string, it can be
//! specified as integer or real or enumerative string, then
//! queried as such.
//! Its string content, which is a Handle(HAsciiString) can be
//! shared by other data structures, hence gives a direct on line
//! access to its value.
class MoniTool_TypedValue : public Standard_Transient
{

public:

  
  //! Creates a TypedValue, with a name
  //!
  //! type gives the type of the parameter, default is free text
  //! Also available : Integer, Real, Enum, Entity (i.e. Object)
  //! More precise specifications, titles, can be given to the
  //! TypedValue once created
  //!
  //! init gives an initial value. If it is not given, the
  //! TypedValue begins as "not set", its value is empty
  Standard_EXPORT MoniTool_TypedValue(const Standard_CString name, const MoniTool_ValueType type = MoniTool_ValueText, const Standard_CString init = "");
  
  //! Creates a TypedValue from another one, by duplication
  Standard_EXPORT MoniTool_TypedValue(const Handle(MoniTool_TypedValue)& other);
  
  //! Access to internal data which have no other access
  Standard_EXPORT void Internals (MoniTool_ValueInterpret& interp, MoniTool_ValueSatisfies& satisf, Standard_CString& satisname, NCollection_DataMap<TCollection_AsciiString, Standard_Integer>& enums) const;
  
  //! Returns the name
  Standard_EXPORT Standard_CString Name() const;
  
  //! Returns the type of the value
  Standard_EXPORT MoniTool_ValueType ValueType() const;
  
  //! Returns the Definition
  //! By priority, the enforced one, else an automatic one, computed
  //! from the specification
  Standard_EXPORT TCollection_AsciiString Definition() const;
  
  //! Enforces a Definition
  Standard_EXPORT void SetDefinition (const Standard_CString deftext);
  
  //! Prints definition, specification, and actual status and value
  Standard_EXPORT virtual void Print (Standard_OStream& S) const;
  
  //! Prints only the Value
  Standard_EXPORT void PrintValue (Standard_OStream& S) const;
  
  //! Completes the definition of a TypedValue by command <initext>,
  //! once created with its type
  //! Returns True if done, False if could not be interpreted
  //! <initext> may be :
  //! imin ival : minimum value for an integer
  //! imax ival : maximum value for an integer
  //! rmin rval : minimum value for a real
  //! rmax rval : maximum value for a real
  //! unit name : name of unit
  //! ematch i  : enum from integer value i, match required
  //! enum   i  : enum from integer value i, match not required
  //! eval text : add an enumerative value (increments max by 1)
  //! eval ??   : add a non-authorised enum value (to be skipped)
  //! tmax   l  : maximum length for a text
  Standard_EXPORT Standard_Boolean AddDef (const Standard_CString initext);
  
  //! Sets a label, which can then be displayed
  Standard_EXPORT void SetLabel (const Standard_CString label);
  
  //! Returns the label, if set; else returns an empty string
  Standard_EXPORT Standard_CString Label() const;
  
  //! Sets a maximum length for a text (active only for a free text)
  Standard_EXPORT void SetMaxLength (const Standard_Integer max);
  
  //! Returns the maximum length, 0 if not set
  Standard_EXPORT Standard_Integer MaxLength() const;
  
  //! Sets an Integer limit (included) to <val>, the upper limit
  //! if <max> is True, the lower limit if <max> is False
  Standard_EXPORT void SetIntegerLimit (const Standard_Boolean max, const Standard_Integer val);
  
  //! Gives an Integer Limit (upper if <max> True, lower if <max>
  //! False). Returns True if this limit is defined, False else
  //! (in that case, gives the natural limit for Integer)
  Standard_EXPORT Standard_Boolean IntegerLimit (const Standard_Boolean max, Standard_Integer& val) const;
  
  //! Sets a Real limit (included) to <val>, the upper limit
  //! if <max> is True, the lower limit if <max> is False
  Standard_EXPORT void SetRealLimit (const Standard_Boolean max, const Standard_Real val);
  
  //! Gives an Real Limit (upper if <max> True, lower if <max>
  //! False). Returns True if this limit is defined, False else
  //! (in that case, gives the natural limit for Real)
  Standard_EXPORT Standard_Boolean RealLimit (const Standard_Boolean max, Standard_Real& val) const;
  
  //! Sets (Clears if <def> empty) a unit definition, as an equation
  //! of dimensions. TypedValue just records this definition, does
  //! not exploit it, to be done as required by user applications
  Standard_EXPORT void SetUnitDef (const Standard_CString def);
  
  //! Returns the recorded unit definition, empty if not set
  Standard_EXPORT Standard_CString UnitDef() const;
  
  //! For an enumeration, precises the starting value (default 0)
  //! and the match condition : if True (D), the string value must
  //! match the definition, else it may take another value : in that
  //! case, the Integer Value will be  Start - 1.
  //! (empty value remains allowed)
  Standard_EXPORT void StartEnum (const Standard_Integer start = 0, const Standard_Boolean match = Standard_True);
  
  //! Adds enumerative definitions. For more than 10, several calls
  Standard_EXPORT void AddEnum (const Standard_CString v1 = "", const Standard_CString v2 = "", const Standard_CString v3 = "", const Standard_CString v4 = "", const Standard_CString v5 = "", const Standard_CString v6 = "", const Standard_CString v7 = "", const Standard_CString v8 = "", const Standard_CString v9 = "", const Standard_CString v10 = "");
  
  //! Adds an enumeration definition, by its string and numeric
  //! values. If it is the first setting for this value, it is
  //! recorded as main value. Else, it is recognized as alternate
  //! string for this numeric value
  Standard_EXPORT void AddEnumValue (const Standard_CString val, const Standard_Integer num);
  
  //! Gives the Enum definitions : start value, end value, match
  //! status. Returns True for an Enum, False else.
  Standard_EXPORT Standard_Boolean EnumDef (Standard_Integer& startcase, Standard_Integer& endcase, Standard_Boolean& match) const;
  
  //! Returns the value of an enumerative definition, from its rank
  //! Empty string if out of range or not an Enum
  Standard_EXPORT Standard_CString EnumVal (const Standard_Integer num) const;
  
  //! Returns the case number which corresponds to a string value
  //! Works with main and additional values
  //! Returns (StartEnum - 1) if not OK, -1 if not an Enum
  Standard_EXPORT Standard_Integer EnumCase (const Standard_CString val) const;
  
  //! Sets type of which an Object TypedValue must be kind of
  //! Error for a TypedValue not an Object (Entity)
  Standard_EXPORT void SetObjectType (const Handle(Standard_Type)& typ);
  
  //! Returns the type of which an Object TypedValue must be kind of
  //! Default is Standard_Transient
  //! Null for a TypedValue not an Object
  Standard_EXPORT Handle(Standard_Type) ObjectType() const;
  
  //! Sets a specific Interpret function
  Standard_EXPORT void SetInterpret (const MoniTool_ValueInterpret func);
  
  //! Tells if a TypedValue has an Interpret
  Standard_EXPORT virtual Standard_Boolean HasInterpret() const;
  
  //! Sets a specific Satisfies function : it is added to the
  //! already defined criteria
  //! It must match the form :
  //! satisfies (val : HAsciiString) returns Boolean
  Standard_EXPORT void SetSatisfies (const MoniTool_ValueSatisfies func, const Standard_CString name);
  
  //! Returns name of specific satisfy, empty string if none
  Standard_EXPORT Standard_CString SatisfiesName() const;
  
  //! Returns True if the value is set (not empty/not null object)
  Standard_EXPORT Standard_Boolean IsSetValue() const;
  
  //! Returns the value, as a cstring. Empty if not set.
  Standard_EXPORT Standard_CString CStringValue() const;
  
  //! Returns the value, as a Handle (can then be shared)
  //! Null if not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) HStringValue() const;
  
  //! Interprets a value.
  //! <native> True  : returns a native value
  //! <native> False : returns a coded  value
  //! If the Interpret function is set, calls it
  //! Else, for an Enum, Native returns the Text, Coded returns
  //! the number
  //! STANDARD RETURNS : = hval means no specific interpretation
  //! Null means senseless
  //! Can also be redefined
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) Interpret (const Handle(TCollection_HAsciiString)& hval, const Standard_Boolean native) const;
  
  //! Returns True if a value statifies the specification
  //! (remark : does not apply to Entity : see ObjectType, for this
  //! type, the string is just a comment)
  Standard_EXPORT virtual Standard_Boolean Satisfies (const Handle(TCollection_HAsciiString)& hval) const;
  
  //! Clears the recorded Value : it is now unset
  Standard_EXPORT void ClearValue();
  
  //! Changes the value. The new one must satisfy the specification
  //! Returns False (and did not set) if the new value
  //! does not satisfy the specification
  //! Can be redefined to be managed (in a subclass)
  Standard_EXPORT virtual Standard_Boolean SetCStringValue (const Standard_CString val);
  
  //! Forces a new Handle for the Value
  //! It can be empty, else (if Type is not free Text), it must
  //! satisfy the specification.
  //! Not only the value is changed, but also the way it is shared
  //! Remark : for Type=Object, this value is not controlled, it can
  //! be set as a comment
  //! Returns False (and did not set) if the new value
  //! does not satisfy the specification
  //! Can be redefined to be managed (in a subclass)
  Standard_EXPORT virtual Standard_Boolean SetHStringValue (const Handle(TCollection_HAsciiString)& hval);
  
  //! Returns the value as integer, i.e. :
  //! For type = Integer, the integer itself; 0 if not set
  //! For type = Enum, the designated rank (see Enum definition)
  //! StartEnum - 1 if not set or not in the definition
  //! Else, returns 0
  Standard_EXPORT Standard_Integer IntegerValue() const;
  
  //! Changes the value as an integer, only for Integer or Enum
  Standard_EXPORT virtual Standard_Boolean SetIntegerValue (const Standard_Integer ival);
  
  //! Returns the value as real,  for a Real type TypedValue
  //! Else, returns 0.
  Standard_EXPORT Standard_Real RealValue() const;
  
  //! Changes the value as a real, only for Real
  Standard_EXPORT virtual Standard_Boolean SetRealValue (const Standard_Real rval);
  
  //! Returns the value as Transient Object, only for Object/Entity
  //! Remark that the "HString value" is IGNORED here
  //! Null if not set; remains to be casted
  Standard_EXPORT Handle(Standard_Transient) ObjectValue() const;
  
  //! Same as ObjectValue, but avoids DownCast : the receiving
  //! variable is directly loaded. It is assumed that it complies
  //! with the definition of ObjectType ! Otherwise, big trouble
  Standard_EXPORT void GetObjectValue (Handle(Standard_Transient)& val) const;
  
  //! Changes the value as Transient Object, only for Object/Entity
  //! Returns False if DynamicType does not satisfy ObjectType
  //! Can be redefined to be managed (in a subclass)
  Standard_EXPORT virtual Standard_Boolean SetObjectValue (const Handle(Standard_Transient)& obj);
  
  //! Returns the type name of the ObjectValue, or an empty string
  //! if not set
  Standard_EXPORT Standard_CString ObjectTypeName() const;
  
  //! Adds a TypedValue in the library.
  //! It is recorded then will be accessed by its Name
  //! Its Definition may be imposed, else it is computed as usual
  //! By default it will be accessed by its Definition (string)
  //! Returns True if done, False if tv is Null or brings no
  //! Definition or <def> not defined
  //!
  //! If a TypedValue was already recorded under this name, it is
  //! replaced
  Standard_EXPORT static Standard_Boolean AddLib (const Handle(MoniTool_TypedValue)& tv, const Standard_CString def = "");
  
  //! Returns the TypedValue bound with a given Name
  //! Null Handle if none recorded
  //! Warning : it is the original, not duplicated
  Standard_EXPORT static Handle(MoniTool_TypedValue) Lib (const Standard_CString def);
  
  //! Returns a COPY of the TypedValue bound with a given Name
  //! Null Handle if none recorded
  Standard_EXPORT static Handle(MoniTool_TypedValue) FromLib (const Standard_CString def);
  
  //! Returns the list of names of items of the Library of Types
  //! --    Library of TypedValue as Valued Parameters,    -- --
  //! accessed by parameter name
  //! for use by management of Static Parameters
  Standard_EXPORT static Handle(TColStd_HSequenceOfAsciiString) LibList();
  
  //! Returns a static value from its name, null if unknown
  Standard_EXPORT static Handle(MoniTool_TypedValue) StaticValue (const Standard_CString name);




  DEFINE_STANDARD_RTTIEXT(MoniTool_TypedValue,Standard_Transient)

protected:

  
  //! Gives the internal library of static values
  Standard_EXPORT static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& Stats();



private:


  TCollection_AsciiString thename;
  TCollection_AsciiString thedef;
  TCollection_AsciiString thelabel;
  MoniTool_ValueType thetype;
  Handle(Standard_Type) theotyp;
  Standard_Integer thelims;
  Standard_Integer themaxlen;
  Standard_Integer theintlow;
  Standard_Integer theintup;
  Standard_Real therealow;
  Standard_Real therealup;
  TCollection_AsciiString theunidef;
  Handle(TColStd_HArray1OfAsciiString) theenums;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> theeadds;
  MoniTool_ValueInterpret theinterp;
  MoniTool_ValueSatisfies thesatisf;
  TCollection_AsciiString thesatisn;
  Standard_Integer theival;
  Handle(TCollection_HAsciiString) thehval;
  Handle(Standard_Transient) theoval;


};







#endif // _MoniTool_TypedValue_HeaderFile
