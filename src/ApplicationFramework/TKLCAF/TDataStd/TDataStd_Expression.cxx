// Created on: 1997-12-16
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

#include <TDataStd_Expression.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Expression, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_Expression::GetID()
{
  static Standard_GUID TDataStd_ExpressionID("ce24146a-8e57-11d1-8953-080009dc4425");
  return TDataStd_ExpressionID;
}

//=================================================================================================

Handle(TDataStd_Expression) TDataStd_Expression::Set(const TDF_Label& L)
{
  Handle(TDataStd_Expression) A;
  if (!L.FindAttribute(TDataStd_Expression::GetID(), A))
  {
    A = new TDataStd_Expression();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_Expression::TDataStd_Expression() {}

//=================================================================================================

TCollection_ExtendedString TDataStd_Expression::Name() const
{
  return myExpression; // ->String();
}

//=================================================================================================

void TDataStd_Expression::SetExpression(const TCollection_ExtendedString& E)
{
  // OCC2932 correction
  if (myExpression == E)
    return;

  Backup();
  myExpression = E;
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_Expression::GetExpression() const
{
  return myExpression;
}

//=================================================================================================

TDF_AttributeList& TDataStd_Expression::GetVariables()
{
  return myVariables;
}

//=================================================================================================

const Standard_GUID& TDataStd_Expression::ID() const
{
  return GetID();
}

//=================================================================================================

void TDataStd_Expression::Restore(const Handle(TDF_Attribute)& With)
{
  Handle(TDataStd_Expression) EXPR = Handle(TDataStd_Expression)::DownCast(With);
  myExpression                     = EXPR->GetExpression();

  Handle(TDataStd_Variable) V;
  myVariables.Clear();
  for (TDF_ListIteratorOfAttributeList it(EXPR->GetVariables()); it.More(); it.Next())
  {
    V = Handle(TDataStd_Variable)::DownCast(it.Value());
    myVariables.Append(V);
  }
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_Expression::NewEmpty() const
{
  return new TDataStd_Expression();
}

//=================================================================================================

void TDataStd_Expression::Paste(const Handle(TDF_Attribute)&       Into,
                                const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_Expression) EXPR = Handle(TDataStd_Expression)::DownCast(Into);
  EXPR->SetExpression(myExpression);
  Handle(TDataStd_Variable) V1;
  for (TDF_ListIteratorOfAttributeList it(myVariables); it.More(); it.Next())
  {
    V1 = Handle(TDataStd_Variable)::DownCast(it.Value());
    Handle(TDF_Attribute) V2;
    RT->HasRelocation(V1, V2);
    EXPR->GetVariables().Append(V2);
  }
}

//=================================================================================================

Standard_OStream& TDataStd_Expression::Dump(Standard_OStream& anOS) const
{
  anOS << "Expression";
  return anOS;
}

//=================================================================================================

void TDataStd_Expression::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myExpression)

  for (TDF_AttributeList::Iterator aVariableIt(myVariables); aVariableIt.More(); aVariableIt.Next())
  {
    const Handle(TDF_Attribute)& anAttribute = aVariableIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anAttribute.get())
  }
}
