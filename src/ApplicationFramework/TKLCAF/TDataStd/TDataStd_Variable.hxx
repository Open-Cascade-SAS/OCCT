// Created on: 1997-12-10
// Created by: Denis PASCAL
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

#ifndef _TDataStd_Variable_HeaderFile
#define _TDataStd_Variable_HeaderFile

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Real.hxx>
#include <TDataStd_RealEnum.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class TCollection_ExtendedString;
class TDataStd_Real;
class TDataStd_Expression;
class TDF_RelocationTable;
class TDF_DataSet;

class TDataStd_Variable;
DEFINE_STANDARD_HANDLE(TDataStd_Variable, TDF_Attribute)

//! Variable attribute.
//! ==================
//!
//! * A variable is  associated to a TDataStd_Real (which
//! contains its    current  value) and  a   TDataStd_Name
//! attribute (which  contains  its name).  It  contains a
//! constant flag, and a Unit
//!
//! * An  expression may  be assigned  to a variable.   In
//! thatcase the expression  is handled by the  associated
//! Expression Attribute  and the Variable returns True to
//! the method <IsAssigned>.
class TDataStd_Variable : public TDF_Attribute
{

public:
  //! class methods
  //! =============
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Find, or create, a  Variable attribute.
  //! Real methods
  //! ============
  Standard_EXPORT static Handle(TDataStd_Variable) Set(const TDF_Label& label);

  Standard_EXPORT TDataStd_Variable();

  //! set or change the name  of the variable, in myUnknown
  //! and my associated Name attribute.
  Standard_EXPORT void Name(const TCollection_ExtendedString& string);

  //! returns    string   stored  in   the  associated  Name
  //! attribute.
  Standard_EXPORT const TCollection_ExtendedString& Name() const;

  //! retrieve or create  the associated real attribute  and
  //! set the  value  <value>.
  Standard_EXPORT void Set(const Standard_Real value) const;

  //! Obsolete method that will be removed in next versions.
  //! The dimension argument is not supported in the persistence mechanism.
  Standard_DEPRECATED("TDataStd_Variable::Set(value, dimension) is deprecated. Please use "
                      "TDataStd_Variable::Set(value) instead.")
  Standard_EXPORT void Set(const Standard_Real value, const TDataStd_RealEnum dimension) const;

  //! returns True if a Real attribute is associated.
  Standard_EXPORT Standard_Boolean IsValued() const;

  //! returns value stored in associated Real attribute.
  Standard_EXPORT Standard_Real Get() const;

  //! returns associated Real attribute.
  Standard_EXPORT Handle(TDataStd_Real) Real() const;

  //! returns True if an Expression attribute is associated.
  //! create(if doesn't exist), set and returns the assigned
  //! expression attribute.
  Standard_EXPORT Standard_Boolean IsAssigned() const;

  //! create(if  doesn't exist)  and  returns  the  assigned
  //! expression  attribute. fill it after.
  Standard_EXPORT Handle(TDataStd_Expression) Assign() const;

  //! if <me> is  assigned delete the associated  expression
  //! attribute.
  Standard_EXPORT void Desassign() const;

  //! if <me>  is  assigned, returns  associated  Expression
  //! attribute.
  Standard_EXPORT Handle(TDataStd_Expression) Expression() const;

  //! shortcut for <Real()->IsCaptured()>
  Standard_EXPORT Standard_Boolean IsCaptured() const;

  //! A constant value is not modified by regeneration.
  Standard_EXPORT Standard_Boolean IsConstant() const;

  Standard_EXPORT void Unit(const TCollection_AsciiString& unit);

  //! to read/write fields
  //! ===================
  Standard_EXPORT const TCollection_AsciiString& Unit() const;

  //! if  <status> is   True, this  variable  will not   be
  //! modified by the solver.
  Standard_EXPORT void Constant(const Standard_Boolean status);

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& With) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  //! to export reference to the associated Name attribute.
  Standard_EXPORT virtual void References(const Handle(TDF_DataSet)& DS) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataStd_Variable, TDF_Attribute)

protected:
private:
  Standard_Boolean        isConstant;
  TCollection_AsciiString myUnit;
};

#endif // _TDataStd_Variable_HeaderFile
