// Created on: 1992-02-11
// Created by: Christian CAILLET
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

#ifndef _StepData_StepReaderData_HeaderFile
#define _StepData_StepReaderData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <Interface_IndexedMapOfAsciiString.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <Standard_Integer.hxx>
#include <Interface_FileReaderData.hxx>
#include <Standard_CString.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <StepData_Logical.hxx>
class Interface_Check;
class TCollection_AsciiString;
class StepData_PDescr;
class Standard_Transient;
class StepData_SelectMember;
class StepData_Field;
class StepData_ESDescr;
class StepData_FieldList;
class StepData_SelectType;
class TCollection_HAsciiString;
class StepData_EnumTool;


class StepData_StepReaderData;
DEFINE_STANDARD_HANDLE(StepData_StepReaderData, Interface_FileReaderData)

//! Specific FileReaderData for Step
//! Contains litteral description of entities (for each one : type
//! as a string, ident, parameter list)
//! provides references evaluation, plus access to litteral data
//! and specific access methods (Boolean, XY, XYZ)
class StepData_StepReaderData : public Interface_FileReaderData
{

public:

  
  //! creates StepReaderData correctly dimensionned (necessary at
  //! creation time, because it contains arrays)
  //! nbheader is nb of records for Header, nbtotal for Header+Data
  //! and nbpar gives the total count of parameters
  Standard_EXPORT StepData_StepReaderData(const Standard_Integer nbheader, const Standard_Integer nbtotal, const Standard_Integer nbpar);
  
  //! Fills the fields of a record
  Standard_EXPORT void SetRecord (const Standard_Integer num, const Standard_CString ident, const Standard_CString type, const Standard_Integer nbpar);
  
  //! Fills the fields of a parameter of a record. This is a variant
  //! of AddParam, Adapted to STEP (optimized for specific values)
  Standard_EXPORT void AddStepParam (const Standard_Integer num, const Standard_CString aval, const Interface_ParamType atype, const Standard_Integer nument = 0);
  
  //! Returns Record Type
  Standard_EXPORT const TCollection_AsciiString& RecordType (const Standard_Integer num) const;
  
  //! Returns Record Type as a CString
  //! was C++ : return const
  Standard_EXPORT Standard_CString CType (const Standard_Integer num) const;
  
  //! Returns record identifier (Positive number)
  //! If returned ident is not positive : Sub-List or Scope mark
  Standard_EXPORT Standard_Integer RecordIdent (const Standard_Integer num) const;
  
  //! Returns SubList numero designated by a parameter (nump) in a
  //! record (num), or zero if the parameter does not exist or is
  //! not a SubList address. Zero too If aslast is True and nump
  //! is not for the last parameter
  Standard_EXPORT Standard_Integer SubListNumber (const Standard_Integer num, const Standard_Integer nump, const Standard_Boolean aslast) const;
  
  //! Returns True if <num> corresponds to a Complex Type Entity
  //! (as can be defined by ANDOR Express clause)
  Standard_EXPORT Standard_Boolean IsComplex (const Standard_Integer num) const;
  
  //! Returns the List of Types which correspond to a Complex Type
  //! Entity. If not Complex, there is just one Type in it
  //! For a SubList or a Scope mark, <types> remains empty
  Standard_EXPORT void ComplexType (const Standard_Integer num, TColStd_SequenceOfAsciiString& types) const;
  
  //! Returns the Next "Componant" for a Complex Type Entity, of
  //! which <num> is already a Componant (the first one or a next one)
  //! Returns 0 for a Simple Type or for the last Componant
  Standard_EXPORT Standard_Integer NextForComplex (const Standard_Integer num) const;
  
  //! Determines the first component which brings a given name, for
  //! a Complex Type Entity
  //! <num0> is the very first record of this entity
  //! <num> is given the last NextNamedForComplex, starts at zero
  //! it is returned as the newly found number
  //! Hence, in the normal case, NextNamedForComplex starts by num0
  //! if <num> is zero, else by NextForComplex(num)
  //! If the alphabetic order is not respected, it restarts from
  //! num0 and loops on NextForComplex until finding <name>
  //! In case of "non-alphabetic order", <ach> is filled with a
  //! Warning for this name
  //! In case of "not-found at all", <ach> is filled with a Fail,
  //! and <num> is returned as zero
  //!
  //! Returns True if alphabetic order, False else
  Standard_EXPORT Standard_Boolean NamedForComplex (const Standard_CString name, const Standard_Integer num0, Standard_Integer& num, Handle(Interface_Check)& ach) const;
  
  //! Determines the first component which brings a given name, or
  //! short name for a Complex Type Entity
  //! <num0> is the very first record of this entity
  //! <num> is given the last NextNamedForComplex, starts at zero
  //! it is returned as the newly found number
  //! Hence, in the normal case, NextNamedForComplex starts by num0
  //! if <num> is zero, else by NextForComplex(num)
  //! If the alphabetic order is not respected, it restarts from
  //! num0 and loops on NextForComplex until finding <name>
  //! In case of "non-alphabetic order", <ach> is filled with a
  //! Warning for this name
  //! In case of "not-found at all", <ach> is filled with a Fail,
  //! and <num> is returned as zero
  //!
  //! Returns True if alphabetic order, False else
  Standard_EXPORT Standard_Boolean NamedForComplex (const Standard_CString theName, const Standard_CString theShortName, const Standard_Integer num0, Standard_Integer& num, Handle(Interface_Check)& ach) const;

  //! Checks Count of Parameters of record <num> to equate <nbreq>
  //! If this Check is successful, returns True
  //! Else, fills <ach> with an Error Message then returns False
  //! <mess> is included in the Error message if given non empty
  Standard_EXPORT Standard_Boolean CheckNbParams (const Standard_Integer num, const Standard_Integer nbreq, Handle(Interface_Check)& ach, const Standard_CString mess = "") const;
  
  //! reads parameter <nump> of record <num> as a sub-list (may be
  //! typed, see ReadTypedParameter in this case)
  //! Returns True if OK. Else (not a LIST), returns false and
  //! feeds Check with appropriate check
  //! If <optional> is True and Param is not defined, returns True
  //! with <ach> not filled and <numsub> returned as 0
  //! Works with SubListNumber with <aslast> false (no specific case
  //! for last parameter)
  Standard_EXPORT Standard_Boolean ReadSubList (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Integer& numsub, const Standard_Boolean optional = Standard_False, const Standard_Integer lenmin = 0, const Standard_Integer lenmax = 0) const;
  
  //! reads the content of a sub-list into a transient :
  //! SelectNamed, or HArray1 of Integer,Real,String,Transient ...
  //! recursive call if list of list ...
  //! If a sub-list has mixed types, an HArray1OfTransient is
  //! produced, it may contain SelectMember
  //! Intended to be called by ReadField
  //! The returned status is : negative if failed, 0 if empty.
  //! Else the kind to be recorded in the field
  Standard_EXPORT Standard_Integer ReadSub (const Standard_Integer numsub, const Standard_CString mess, Handle(Interface_Check)& ach, const Handle(StepData_PDescr)& descr, Handle(Standard_Transient)& val) const;
  
  //! Reads parameter <nump> of record <num> into a SelectMember,
  //! self-sufficient (no Description needed)
  //! If <val> is already created, it will be filled, as possible
  //! And if reading does not match its own description, the result
  //! will be False
  //! If <val> is not it not yet created, it will be (SelectNamed)
  //! Usefull if a field is defined as a SelectMember, directly
  //! (SELECT with no Entity as member)
  //! But SelectType also manages SelectMember (for SELECT with
  //! some members as Entity, some other not)
  Standard_EXPORT Standard_Boolean ReadMember (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Handle(StepData_SelectMember)& val) const;
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean ReadMember (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Handle(T)& val) const
  {
    Handle(StepData_SelectMember) aVal = val;
    return ReadMember (num, nump, mess, ach, aVal) && ! (val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! reads parameter <nump> of record <num> into a Field,
  //! controlled by a Parameter Descriptor (PDescr), which controls
  //! its allowed type(s) and value
  //! <ach> is filled if the read parameter does not match its
  //! description (but the field is read anyway)
  //! If the description is not defined, no control is done
  //! Returns True when done
  Standard_EXPORT Standard_Boolean ReadField (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const Handle(StepData_PDescr)& descr, StepData_Field& fild) const;
  
  //! reads a list of fields controlled by an ESDescr
  Standard_EXPORT Standard_Boolean ReadList (const Standard_Integer num, Handle(Interface_Check)& ach, const Handle(StepData_ESDescr)& descr, StepData_FieldList& list) const;
  
  //! Reads parameter <nump> of record <num> into a Transient Value
  //! according to the type of the parameter :
  //! Named for Integer,Boolean,Logical,Enum,Real : SelectNamed
  //! Immediate Integer,Boolean,Logical,Enum,Real : SelectInt/Real
  //! Text  : HAsciiString
  //! Ident : the referenced Entity
  //! Sub-List not processed, see ReadSub
  //! This value is controlled by a Parameter Descriptor (PDescr),
  //! which controls its allowed type and value
  //! <ach> is filled if the read parameter does not match its
  //! description (the select is nevertheless created if possible)
  //!
  //! Warning : val is in out, hence it is possible to predefine a specific
  //! SelectMember then to fill it. If <val> is Null or if the
  //! result is not a SelectMember, val itself is returned a new ref
  //! For a Select with a Name, <val> must then be a SelectNamed
  Standard_EXPORT Standard_Boolean ReadAny (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const Handle(StepData_PDescr)& descr, Handle(Standard_Transient)& val) const;
  
  //! reads parameter <nump> of record <num> as a sub-list of
  //! two Reals X,Y. Returns True if OK. Else, returns false and
  //! feeds Check with appropriate Fails (parameter not a sub-list,
  //! not two Reals in the sub-list) composed with "mess" which
  //! gives the name of the parameter
  Standard_EXPORT Standard_Boolean ReadXY (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Real& X, Standard_Real& Y) const;
  
  //! reads parameter <nump> of record <num> as a sub-list of
  //! three Reals X,Y,Z. Return value and Check managed as by
  //! ReadXY (demands a sub-list of three Reals)
  Standard_EXPORT Standard_Boolean ReadXYZ (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
  //! reads parameter <nump> of record <num> as a single Real value.
  //! Return value and Check managed as by ReadXY (demands a Real)
  Standard_EXPORT Standard_Boolean ReadReal (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Real& val) const;
  
  //! Reads parameter <nump> of record <num> as a single Entity.
  //! Return value and Check managed as by ReadReal (demands a
  //! reference to an Entity). In Addition, demands read Entity
  //! to be Kind of a required Type <atype>.
  //! Remark that returned status is False and <ent> is Null if
  //! parameter is not an Entity, <ent> remains Not Null is parameter
  //! is an Entity but is not Kind of required type
  Standard_EXPORT Standard_Boolean ReadEntity (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const Handle(Standard_Type)& atype, Handle(Standard_Transient)& ent) const;
  
  //! Safe variant for arbitrary type of argument
  template <class T> 
  Standard_Boolean ReadEntity (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const Handle(Standard_Type)& atype, Handle(T)& ent) const
  {
    Handle(Standard_Transient) anEnt = ent;
    return ReadEntity (num, nump, mess, ach, atype, anEnt) && ! (ent = Handle(T)::DownCast(anEnt)).IsNull();
  }

  //! Same as above, but a SelectType checks Type Matching, and
  //! records the read Entity (see method Value from SelectType)
  Standard_EXPORT Standard_Boolean ReadEntity (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, StepData_SelectType& sel) const;
  
  //! reads parameter <nump> of record <num> as a single Integer.
  //! Return value & Check managed as by ReadXY (demands an Integer)
  Standard_EXPORT Standard_Boolean ReadInteger (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Integer& val) const;
  
  //! reads parameter <nump> of record <num> as a Boolean
  //! Return value and Check managed as by ReadReal (demands a
  //! Boolean enum, i.e. text ".T." for True or ".F." for False)
  Standard_EXPORT Standard_Boolean ReadBoolean (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Boolean& flag) const;
  
  //! reads parameter <nump> of record <num> as a Logical
  //! Return value and Check managed as by ReadBoolean (demands a
  //! Logical enum, i.e. text ".T.", ".F.", or ".U.")
  Standard_EXPORT Standard_Boolean ReadLogical (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, StepData_Logical& flag) const;
  
  //! reads parameter <nump> of record <num> as a String (text
  //! between quotes, quotes are removed by the Read operation)
  //! Return value and Check managed as by ReadXY (demands a String)
  Standard_EXPORT Standard_Boolean ReadString (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Handle(TCollection_HAsciiString)& val) const;
  
  Standard_EXPORT Standard_Boolean ReadEnumParam (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_CString& text) const;
  
  //! Fills a check with a fail message if enumeration value does
  //! match parameter definition
  //! Just a help to centralize message definitions
  Standard_EXPORT void FailEnumValue (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach) const;
  
  //! Reads parameter <nump> of record <num> as an Enumeration (text
  //! between dots) and converts it to an integer value, by an
  //! EnumTool. Returns True if OK, false if : this parameter is not
  //! enumeration, or is not recognized by the EnumTool (with fail)
  Standard_EXPORT Standard_Boolean ReadEnum (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const StepData_EnumTool& enumtool, Standard_Integer& val) const;
  
  //! Resolves a parameter which can be enclosed in a type def., as
  //! TYPE(val). The parameter must then be read normally according
  //! its type.  Parameter to be resolved is <nump> of record <num>
  //! <mustbetyped> True  demands a typed parameter
  //! <mustbetyped> False accepts a non-typed parameter as option
  //! mess and ach as usual
  //! <numr>,<numrp> are the resolved record and parameter numbers
  //! = num,nump if no type,  else numrp=1
  //! <typ> returns the recorded type, or empty string
  //! Remark : a non-typed list is considered as "non-typed"
  Standard_EXPORT Standard_Boolean ReadTypedParam (const Standard_Integer num, const Standard_Integer nump, const Standard_Boolean mustbetyped, const Standard_CString mess, Handle(Interface_Check)& ach, Standard_Integer& numr, Standard_Integer& numrp, TCollection_AsciiString& typ) const;
  
  //! Checks if parameter <nump> of record <num> is given as Derived
  //! If this Check is successful (i.e. Param = "*"), returns True
  //! Else, fills <ach> with a Message which contains <mess> and
  //! returns False. According to <errstat>, this message is Warning
  //! if errstat is False (Default), Fail if errstat is True
  Standard_EXPORT Standard_Boolean CheckDerived (const Standard_Integer num, const Standard_Integer nump, const Standard_CString mess, Handle(Interface_Check)& ach, const Standard_Boolean errstat = Standard_False) const;
  
  //! Returns total count of Entities (including Header)
  Standard_EXPORT virtual Standard_Integer NbEntities() const Standard_OVERRIDE;
  
  //! determines the first suitable record following a given one
  //! that is, skips SCOPE,ENDSCOPE and SUBLIST records
  //! Note : skips Header records, which are accessed separately
  Standard_EXPORT Standard_Integer FindNextRecord (const Standard_Integer num) const Standard_OVERRIDE;
  
  //! determines reference numbers in EntityNumber fields
  //! called by Prepare from StepReaderTool to prepare later using
  //! by a StepModel. This method is attached to StepReaderData
  //! because it needs a massive amount of data accesses to work
  //!
  //! If <withmap> is given False, the basic exploration algorithm
  //! is activated, otherwise a map is used as far as it is possible
  //! this option can be used only to test this algorithm
  Standard_EXPORT void SetEntityNumbers (const Standard_Boolean withmap = Standard_True);
  
  //! determine first suitable record of Header
  //! works as FindNextRecord, but treats only Header records
  Standard_EXPORT Standard_Integer FindNextHeaderRecord (const Standard_Integer num) const;
  
  //! Works as SetEntityNumbers but for Header : more simple because
  //! there are no Reference, only Sub-Lists
  Standard_EXPORT void PrepareHeader();
  
  //! Returns the Global Check. It can record Fail messages about
  //! Undefined References (detected by SetEntityNumbers)
  Standard_EXPORT const Handle(Interface_Check) GlobalCheck() const;




  DEFINE_STANDARD_RTTIEXT(StepData_StepReaderData,Interface_FileReaderData)

protected:




private:

  
  //! Searches for a Parameter of the record <num>, which refers to
  //! the Ident <id> (form #nnn). [Used by SetEntityNumbers]
  //! If found, returns its EntityNumber, else returns Zero.
  Standard_EXPORT Standard_Integer FindEntityNumber (const Standard_Integer num, const Standard_Integer id) const;

  TColStd_Array1OfInteger theidents;
  TColStd_Array1OfInteger thetypes;
  Interface_IndexedMapOfAsciiString thenametypes;
  TColStd_DataMapOfIntegerInteger themults;
  Standard_Integer thenbents;
  Standard_Integer thelastn;
  Standard_Integer thenbhead;
  Standard_Integer thenbscop;
  Handle(Interface_Check) thecheck;


};







#endif // _StepData_StepReaderData_HeaderFile
