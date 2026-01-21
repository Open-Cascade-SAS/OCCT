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

#include <Geom2d_CartesianPoint.hxx>
#include <Interface_IntVal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Transfer_Finder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_Finder, Standard_Transient)

void Transfer_Finder::SetHashCode(const size_t code)
{
  thecode = code;
}

size_t Transfer_Finder::GetHashCode() const
{
  return thecode;
}

occ::handle<Standard_Type> Transfer_Finder::ValueType() const
{
  return DynamicType();
}

const char* Transfer_Finder::ValueTypeName() const
{
  return "(finder)";
}

//  ####    ATTRIBUTES    ####

// Integer -> IntVal, Real -> Geom2d_CartesianPoint, CString -> HAsciiString

void Transfer_Finder::SetAttribute(const char* const                      name,
                                   const occ::handle<Standard_Transient>& val)
{
  theattrib.Bind(name, val);
}

bool Transfer_Finder::RemoveAttribute(const char* const name)
{
  if (theattrib.IsEmpty())
    return false;
  return theattrib.UnBind(name);
}

bool Transfer_Finder::GetAttribute(const char* const                 name,
                                   const occ::handle<Standard_Type>& type,
                                   occ::handle<Standard_Transient>&  val) const
{
  if (theattrib.IsEmpty())
  {
    val.Nullify();
    return false;
  }
  if (!theattrib.Find(name, val))
  {
    val.Nullify();
    return false;
  }
  if (!val->IsKind(type))
  {
    val.Nullify();
    return false;
  }
  return true;
}

occ::handle<Standard_Transient> Transfer_Finder::Attribute(const char* const name) const
{
  occ::handle<Standard_Transient> atr;
  if (theattrib.IsEmpty())
    return atr;
  if (!theattrib.Find(name, atr))
    atr.Nullify();
  return atr;
}

Interface_ParamType Transfer_Finder::AttributeType(const char* const name) const
{
  occ::handle<Standard_Transient> atr = Attribute(name);
  if (atr.IsNull())
    return Interface_ParamVoid;
  if (atr->DynamicType() == STANDARD_TYPE(Interface_IntVal))
    return Interface_ParamInteger;
  if (atr->DynamicType() == STANDARD_TYPE(Geom2d_CartesianPoint))
    return Interface_ParamReal;
  if (atr->DynamicType() == STANDARD_TYPE(TCollection_HAsciiString))
    return Interface_ParamText;
  return Interface_ParamIdent;
}

void Transfer_Finder::SetIntegerAttribute(const char* const name, const int val)
{
  occ::handle<Interface_IntVal> ival = new Interface_IntVal;
  ival->CValue()                     = val;
  SetAttribute(name, ival);
}

bool Transfer_Finder::GetIntegerAttribute(const char* const name, int& val) const
{
  occ::handle<Interface_IntVal> ival = occ::down_cast<Interface_IntVal>(Attribute(name));
  if (ival.IsNull())
  {
    val = 0;
    return false;
  }
  val = ival->Value();
  return true;
}

int Transfer_Finder::IntegerAttribute(const char* const name) const
{
  occ::handle<Interface_IntVal> ival = occ::down_cast<Interface_IntVal>(Attribute(name));
  if (ival.IsNull())
    return 0;
  return ival->Value();
}

void Transfer_Finder::SetRealAttribute(const char* const name, const double val)
{
  occ::handle<Geom2d_CartesianPoint> rval = new Geom2d_CartesianPoint(val, 0);
  SetAttribute(name, rval);
}

bool Transfer_Finder::GetRealAttribute(const char* const name, double& val) const
{
  occ::handle<Geom2d_CartesianPoint> rval = occ::down_cast<Geom2d_CartesianPoint>(Attribute(name));
  if (rval.IsNull())
  {
    val = 0.0;
    return false;
  }
  val = rval->X();
  return true;
}

double Transfer_Finder::RealAttribute(const char* const name) const
{
  occ::handle<Geom2d_CartesianPoint> rval = occ::down_cast<Geom2d_CartesianPoint>(Attribute(name));
  if (rval.IsNull())
    return 0;
  return rval->X();
}

void Transfer_Finder::SetStringAttribute(const char* const name, const char* const val)
{
  occ::handle<TCollection_HAsciiString> hval = new TCollection_HAsciiString(val);
  SetAttribute(name, hval);
}

bool Transfer_Finder::GetStringAttribute(const char* const name, const char*& val) const
{
  occ::handle<TCollection_HAsciiString> hval =
    occ::down_cast<TCollection_HAsciiString>(Attribute(name));
  if (hval.IsNull())
  {
    val = "";
    return false;
  }
  val = hval->ToCString();
  return true;
}

const char* Transfer_Finder::StringAttribute(const char* const name) const
{
  occ::handle<TCollection_HAsciiString> hval =
    occ::down_cast<TCollection_HAsciiString>(Attribute(name));
  if (hval.IsNull())
    return "";
  return hval->ToCString();
}

NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& Transfer_Finder::
  AttrList()
{
  return theattrib;
}

void Transfer_Finder::SameAttributes(const occ::handle<Transfer_Finder>& other)
{
  if (!other.IsNull())
    theattrib = other->AttrList();
}

void Transfer_Finder::GetAttributes(const occ::handle<Transfer_Finder>& other,
                                    const char* const                   fromname,
                                    const bool                          copied)
{
  if (other.IsNull())
    return;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& list =
    other->AttrList();
  if (list.IsEmpty())
    return;

  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(
    list);
  for (; iter.More(); iter.Next())
  {
    const TCollection_AsciiString& name = iter.Key();
    if (!name.StartsWith(fromname))
      continue;
    const occ::handle<Standard_Transient>& atr    = iter.Value();
    occ::handle<Standard_Transient>        newatr = atr;

    //    Copy ? according type
    if (copied)
    {
      occ::handle<Interface_IntVal> ival = occ::down_cast<Interface_IntVal>(atr);
      if (!ival.IsNull())
      {
        int intval     = ival->Value();
        ival           = new Interface_IntVal;
        ival->CValue() = intval;
        newatr         = ival;
      }
      occ::handle<Geom2d_CartesianPoint> rval = occ::down_cast<Geom2d_CartesianPoint>(atr);
      if (!rval.IsNull())
      {
        double realval = rval->X();
        rval           = new Geom2d_CartesianPoint(realval, 0);
        newatr         = rval;
      }
      occ::handle<TCollection_HAsciiString> hval = occ::down_cast<TCollection_HAsciiString>(atr);
      if (!hval.IsNull())
      {
        occ::handle<TCollection_HAsciiString> strval =
          new TCollection_HAsciiString(hval->ToCString());
        newatr = strval;
      }
    }

    theattrib.Bind(name, newatr);
  }
}
