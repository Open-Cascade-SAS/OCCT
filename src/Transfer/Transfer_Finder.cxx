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

IMPLEMENT_STANDARD_RTTIEXT(Transfer_Finder,Standard_Transient)

void  Transfer_Finder::SetHashCode (const Standard_Integer code)
      {  thecode = code;  }

    Standard_Integer  Transfer_Finder::GetHashCode () const
      {  return thecode;  }

    Handle(Standard_Type)  Transfer_Finder::ValueType () const
      {  return DynamicType();  }

    Standard_CString  Transfer_Finder::ValueTypeName () const
      {  return "(finder)";  }


//  ####    ATTRIBUTES    ####


// Integer -> IntVal, Real -> Geom2d_CartesianPoint, CString -> HAsciiString

    void  Transfer_Finder::SetAttribute
  (const Standard_CString name, const Handle(Standard_Transient)& val)
{
  theattrib.Bind(name,val);
}

    Standard_Boolean  Transfer_Finder::RemoveAttribute
  (const Standard_CString name)
{
  if (theattrib.IsEmpty()) return Standard_False;
  return theattrib.UnBind (name);
}

    Standard_Boolean  Transfer_Finder::GetAttribute
  (const Standard_CString name, const Handle(Standard_Type)& type,
   Handle(Standard_Transient)& val) const
{
  if (theattrib.IsEmpty())  {  val.Nullify();  return Standard_False;  }
  if (!theattrib.Find(name, val))  {  val.Nullify();  return Standard_False;  }
  if (!val->IsKind(type))  {  val.Nullify();  return Standard_False;  }
  return Standard_True;
}

    Handle(Standard_Transient)  Transfer_Finder::Attribute
  (const Standard_CString name) const
{
  Handle(Standard_Transient) atr;
  if (theattrib.IsEmpty()) return atr;
  if (!theattrib.Find(name, atr)) atr.Nullify();
  return atr;
}

    Interface_ParamType  Transfer_Finder::AttributeType
  (const Standard_CString name) const
{
  Handle(Standard_Transient) atr = Attribute(name);
  if (atr.IsNull()) return Interface_ParamVoid;
  if (atr->DynamicType() == STANDARD_TYPE(Interface_IntVal))
    return Interface_ParamInteger;
  if (atr->DynamicType() == STANDARD_TYPE(Geom2d_CartesianPoint))
    return Interface_ParamReal;
  if (atr->DynamicType() == STANDARD_TYPE(TCollection_HAsciiString))
    return Interface_ParamText;
  return Interface_ParamIdent;
}


    void  Transfer_Finder::SetIntegerAttribute
  (const Standard_CString name, const Standard_Integer val)
{
  Handle(Interface_IntVal) ival = new Interface_IntVal;
  ival->CValue() = val;
  SetAttribute (name, ival);
}

    Standard_Boolean  Transfer_Finder::GetIntegerAttribute
  (const Standard_CString name, Standard_Integer& val) const
{
  Handle(Interface_IntVal) ival = Handle(Interface_IntVal)::DownCast
    (Attribute(name));
  if (ival.IsNull())  {  val = 0;  return Standard_False;  }
  val = ival->Value();
  return Standard_True;
}

    Standard_Integer  Transfer_Finder::IntegerAttribute
  (const Standard_CString name) const
{
  Handle(Interface_IntVal) ival = Handle(Interface_IntVal)::DownCast
    (Attribute(name));
  if (ival.IsNull()) return 0;
  return ival->Value();
}

    void  Transfer_Finder::SetRealAttribute
  (const Standard_CString name, const Standard_Real val)
{
  Handle(Geom2d_CartesianPoint) rval = new Geom2d_CartesianPoint (val,0);
  SetAttribute (name,rval);
}

    Standard_Boolean  Transfer_Finder::GetRealAttribute
  (const Standard_CString name, Standard_Real& val) const
{
  Handle(Geom2d_CartesianPoint) rval = Handle(Geom2d_CartesianPoint)::DownCast
    (Attribute(name));
  if (rval.IsNull())  {  val = 0.0;  return Standard_False;  }
  val = rval->X();
  return Standard_True;
}

    Standard_Real  Transfer_Finder::RealAttribute (const Standard_CString name) const
{
  Handle(Geom2d_CartesianPoint) rval = Handle(Geom2d_CartesianPoint)::DownCast
    (Attribute(name));
  if (rval.IsNull()) return 0;
  return rval->X();
}

    void  Transfer_Finder::SetStringAttribute
  (const Standard_CString name, const Standard_CString val)
{
  Handle(TCollection_HAsciiString) hval = new TCollection_HAsciiString (val);
  SetAttribute (name,hval);
}

    Standard_Boolean  Transfer_Finder::GetStringAttribute
  (const Standard_CString name, Standard_CString& val) const
{
  Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast
    (Attribute(name));
  if (hval.IsNull())  {  val = "";  return Standard_False;  }
  val = hval->ToCString();
  return Standard_True;
}

    Standard_CString  Transfer_Finder::StringAttribute (const Standard_CString name) const
{
  Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast
    (Attribute(name));
  if (hval.IsNull()) return "";
  return hval->ToCString();
}

NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& Transfer_Finder::AttrList ()
  {  return theattrib;  }

    void  Transfer_Finder::SameAttributes (const Handle(Transfer_Finder)& other)
      {  if (!other.IsNull()) theattrib = other->AttrList();  }

    void  Transfer_Finder::GetAttributes
  (const Handle(Transfer_Finder)& other,
   const Standard_CString fromname, const Standard_Boolean copied)
{
      if (other.IsNull()) return;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& list = other->AttrList();
  if (list.IsEmpty()) return;

  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator iter(list);
  for (; iter.More(); iter.Next()) {
    TCollection_AsciiString name = iter.Key();
    if (!name.StartsWith(fromname)) continue;
    Handle(Standard_Transient) atr = iter.Value();
    Handle(Standard_Transient) newatr = atr;

//    Copy ? according type
    if (copied) {
      Handle(Interface_IntVal) ival = Handle(Interface_IntVal)::DownCast(atr);
      if (!ival.IsNull()) {
	Standard_Integer intval = ival->Value();
	ival = new Interface_IntVal;
	ival->CValue() = intval;
	newatr = ival; 
      }
      Handle(Geom2d_CartesianPoint) rval = Handle(Geom2d_CartesianPoint)::DownCast(atr);
      if (!rval.IsNull()) {
	Standard_Real realval = rval->X();
	rval = new Geom2d_CartesianPoint (realval,0);
	newatr = rval;
      }
      Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast(atr);
      if (!hval.IsNull()) {
	Handle(TCollection_HAsciiString) strval = new TCollection_HAsciiString
	  (hval->ToCString());
	newatr = strval;
      }

    }

    theattrib.Bind(name,newatr);

  }
}
