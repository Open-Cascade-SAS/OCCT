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

#include <TDataStd_Variable.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Variable, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_Variable::GetID()
{
  static Standard_GUID TDataStd_VariableID("ce241469-8e57-11d1-8953-080009dc4425");
  return TDataStd_VariableID;
}

//=================================================================================================

Handle(TDataStd_Variable) TDataStd_Variable::Set(const TDF_Label& L)
{
  Handle(TDataStd_Variable) A;
  if (!L.FindAttribute(TDataStd_Variable::GetID(), A))
  {
    A = new TDataStd_Variable();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_Variable::TDataStd_Variable()
    : isConstant(Standard_False),
      myUnit("SCALAR")
{
}

//=================================================================================================

void TDataStd_Variable::Name(const TCollection_ExtendedString& string)
{
  TDataStd_Name::Set(Label(), string);
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_Variable::Name() const
{
  Handle(TDataStd_Name) N;
  if (!Label().FindAttribute(TDataStd_Name::GetID(), N))
  {
    throw Standard_DomainError("TDataStd_Variable::Name : invalid model");
  }
  return N->Get();
}

//=================================================================================================

Standard_Boolean TDataStd_Variable::IsValued() const
{
  return (Label().IsAttribute(TDataStd_Real::GetID()));
}

//=================================================================================================

Handle(TDataStd_Real) TDataStd_Variable::Real() const
{
  Handle(TDataStd_Real) R;
  if (!Label().FindAttribute(TDataStd_Real::GetID(), R))
  {
    throw Standard_DomainError("TDataStd_Variable::Real : invalid model");
  }
  return R;
}

//=================================================================================================

void TDataStd_Variable::Set(const Standard_Real value) const
{
  Handle(TDataStd_Real) R = TDataStd_Real::Set(Label(), value);
}

//=================================================================================================

void TDataStd_Variable::Set(const Standard_Real value, const TDataStd_RealEnum dimension) const
{
  if (!IsValued())
  {
    Handle(TDataStd_Real)                 R = TDataStd_Real::Set(Label(), value);
    Standard_DISABLE_DEPRECATION_WARNINGS R->SetDimension(dimension);
    Standard_ENABLE_DEPRECATION_WARNINGS
  }
  else
  {
    Handle(TDataStd_Real) R = TDataStd_Real::Set(Label(), value);
  }
}

//=================================================================================================

Standard_Real TDataStd_Variable::Get() const
{
  Handle(TDataStd_Real) R;
  if (!Label().FindAttribute(TDataStd_Real::GetID(), R))
  {
    throw Standard_DomainError("TDataStd_Variable::Get : invalid model");
  }
  return R->Get();
}

//=================================================================================================

Standard_Boolean TDataStd_Variable::IsAssigned() const
{
  return (Label().IsAttribute(TDataStd_Expression::GetID()));
}

//=================================================================================================

Handle(TDataStd_Expression) TDataStd_Variable::Assign() const
{
  Handle(TDataStd_Expression) E = TDataStd_Expression::Set(Label());
  return E;
}

//=================================================================================================

void TDataStd_Variable::Desassign() const
{
  Handle(TDataStd_Expression) E;
  if (!Label().FindAttribute(TDataStd_Expression::GetID(), E))
  {
    throw Standard_DomainError("TDataStd_Variable::Deassign");
  }
  Label().ForgetAttribute(E);
}

//=================================================================================================

Handle(TDataStd_Expression) TDataStd_Variable::Expression() const
{
  Handle(TDataStd_Expression) E;
  if (!Label().FindAttribute(TDataStd_Expression::GetID(), E))
  {
    throw Standard_DomainError("TDataStd_Variable::GetExpression");
  }
  return E;
}

//=================================================================================================

Standard_Boolean TDataStd_Variable::IsCaptured() const
{
  return Real()->IsCaptured();
}

//=================================================================================================

Standard_Boolean TDataStd_Variable::IsConstant() const
{
  return isConstant;
}

//=================================================================================================

void TDataStd_Variable::Constant(const Standard_Boolean status)
{
  // OCC2932 correction
  if (isConstant == status)
    return;

  Backup();
  isConstant = status;
}

//=================================================================================================

void TDataStd_Variable::Unit(const TCollection_AsciiString& unit)
{
  // OCC2932 correction
  if (myUnit == unit)
    return;

  Backup();
  myUnit = unit;
}

//=================================================================================================

const TCollection_AsciiString& TDataStd_Variable::Unit() const
{
  return myUnit;
}

//=================================================================================================

const Standard_GUID& TDataStd_Variable::ID() const
{
  return GetID();
}

//=================================================================================================

void TDataStd_Variable::Restore(const Handle(TDF_Attribute)& With)
{
  Handle(TDataStd_Variable) V = Handle(TDataStd_Variable)::DownCast(With);
  isConstant                  = V->IsConstant();
  myUnit                      = V->Unit();
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_Variable::NewEmpty() const
{
  return new TDataStd_Variable();
}

//=================================================================================================

void TDataStd_Variable::Paste(const Handle(TDF_Attribute)& Into,
                              const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TDataStd_Variable) V = Handle(TDataStd_Variable)::DownCast(Into);
  V->Constant(isConstant);
  V->Unit(myUnit);
}

//=================================================================================================

void TDataStd_Variable::References(const Handle(TDF_DataSet)& DS) const
{
  Handle(TDataStd_Name) N;
  if (Label().FindAttribute(TDataStd_Name::GetID(), N))
  {
    DS->AddAttribute(N);
  }
}

//=================================================================================================

Standard_OStream& TDataStd_Variable::Dump(Standard_OStream& anOS) const
{
  anOS << "Variable";
  return anOS;
}

//=================================================================================================

void TDataStd_Variable::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isConstant)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myUnit)
}
