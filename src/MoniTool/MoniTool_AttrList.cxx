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


#include <MoniTool_AttrList.hxx>
#include <MoniTool_IntVal.hxx>
#include <MoniTool_RealVal.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

MoniTool_AttrList::MoniTool_AttrList ()    {  }

    MoniTool_AttrList::MoniTool_AttrList (const MoniTool_AttrList& other)
    : theattrib (other.AttrList())    {  }

//  ####    ATTRIBUTES    ####


// Integer -> IntVal, Real -> RealVal, CString -> HAsciiString

    void  MoniTool_AttrList::SetAttribute
  (const Standard_CString name, const Handle(Standard_Transient)& val)
{
  theattrib.Bind(name,val);
}

    Standard_Boolean  MoniTool_AttrList::RemoveAttribute
  (const Standard_CString name)
{
  if (theattrib.IsEmpty()) return Standard_False;
  return theattrib.UnBind(name);
}

    Standard_Boolean  MoniTool_AttrList::GetAttribute
  (const Standard_CString name, const Handle(Standard_Type)& type,
   Handle(Standard_Transient)& val) const
{
  if (theattrib.IsEmpty())  {  val.Nullify();  return Standard_False;  }
  if (!theattrib.Find(name, val)) { val.Nullify();  return Standard_False; }
  if (!val->IsKind(type))  {  val.Nullify();  return Standard_False;  }
  return Standard_True;
}

    Handle(Standard_Transient)  MoniTool_AttrList::Attribute
  (const Standard_CString name) const
{
  Handle(Standard_Transient) atr;
  if (theattrib.IsEmpty()) return atr;
  if (!theattrib.Find(name, atr))
    atr.Nullify();
  return atr;
}

    MoniTool_ValueType  MoniTool_AttrList::AttributeType
  (const Standard_CString name) const
{
  Handle(Standard_Transient) atr = Attribute(name);
  if (atr.IsNull()) return MoniTool_ValueVoid;
  if (atr->DynamicType() == STANDARD_TYPE(MoniTool_IntVal))
    return MoniTool_ValueInteger;
  if (atr->DynamicType() == STANDARD_TYPE(MoniTool_RealVal))
    return MoniTool_ValueReal;
  if (atr->DynamicType() == STANDARD_TYPE(TCollection_HAsciiString))
    return MoniTool_ValueText;
  return MoniTool_ValueIdent;
}


    void  MoniTool_AttrList::SetIntegerAttribute
  (const Standard_CString name, const Standard_Integer val)
{
  Handle(MoniTool_IntVal) ival = new MoniTool_IntVal;
  ival->CValue() = val;
  SetAttribute (name, ival);
}

    Standard_Boolean  MoniTool_AttrList::GetIntegerAttribute
  (const Standard_CString name, Standard_Integer& val) const
{
  Handle(MoniTool_IntVal) ival = Handle(MoniTool_IntVal)::DownCast
    (Attribute(name));
  if (ival.IsNull())  {  val = 0;  return Standard_False;  }
  val = ival->Value();
  return Standard_True;
}

    Standard_Integer  MoniTool_AttrList::IntegerAttribute
  (const Standard_CString name) const
{
  Handle(MoniTool_IntVal) ival = Handle(MoniTool_IntVal)::DownCast
    (Attribute(name));
  if (ival.IsNull()) return 0;
  return ival->Value();
}

    void  MoniTool_AttrList::SetRealAttribute
  (const Standard_CString name, const Standard_Real val)
{
  Handle(MoniTool_RealVal) rval = new MoniTool_RealVal;
  rval->CValue() = val;
  SetAttribute (name,rval);
}

    Standard_Boolean  MoniTool_AttrList::GetRealAttribute
  (const Standard_CString name, Standard_Real& val) const
{
  Handle(MoniTool_RealVal) rval = Handle(MoniTool_RealVal)::DownCast
    (Attribute(name));
  if (rval.IsNull())  {  val = 0.0;  return Standard_False;  }
  val = rval->Value();
  return Standard_True;
}

    Standard_Real  MoniTool_AttrList::RealAttribute (const Standard_CString name) const
{
  Handle(MoniTool_RealVal) rval = Handle(MoniTool_RealVal)::DownCast
    (Attribute(name));
  if (rval.IsNull()) return 0;
  return rval->Value();
}

    void  MoniTool_AttrList::SetStringAttribute
  (const Standard_CString name, const Standard_CString val)
{
  Handle(TCollection_HAsciiString) hval = new TCollection_HAsciiString (val);
  SetAttribute (name,hval);
}

    Standard_Boolean  MoniTool_AttrList::GetStringAttribute
  (const Standard_CString name, Standard_CString& val) const
{
  Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast
    (Attribute(name));
  if (hval.IsNull())  {  val = "";  return Standard_False;  }
  val = hval->ToCString();
  return Standard_True;
}

    Standard_CString  MoniTool_AttrList::StringAttribute (const Standard_CString name) const
{
  Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast
    (Attribute(name));
  if (hval.IsNull()) return "";
  return hval->ToCString();
}

   const NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& MoniTool_AttrList::AttrList () const
      {  return theattrib;  }

    void  MoniTool_AttrList::SameAttributes (const MoniTool_AttrList& other)
      {  theattrib = other.AttrList();  }

    void  MoniTool_AttrList::GetAttributes
  (const MoniTool_AttrList& other,
   const Standard_CString fromname, const Standard_Boolean copied)
{
  const NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& list = other.AttrList();
  if (list.IsEmpty()) return;

  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator iter(list);
  for (; iter.More(); iter.Next()) {
    TCollection_AsciiString name = iter.Key();
    if (!name.StartsWith(fromname))
      continue;
    Handle(Standard_Transient) atr = iter.Value();
    Handle(Standard_Transient) newatr = atr;

//    Copy ? according type
    if (copied) {
      Handle(MoniTool_IntVal) ival = Handle(MoniTool_IntVal)::DownCast(atr);
      if (!ival.IsNull()) {
	Standard_Integer intval = ival->Value();
	ival = new MoniTool_IntVal;
	ival->CValue() = intval;
	newatr = ival; 
      }
      Handle(MoniTool_RealVal) rval = Handle(MoniTool_RealVal)::DownCast(atr);
      if (!rval.IsNull()) {
	Standard_Real realval = rval->Value();
	rval = new MoniTool_RealVal;
	rval->CValue() = realval;
	newatr = rval;
      }
      Handle(TCollection_HAsciiString) hval = Handle(TCollection_HAsciiString)::DownCast(atr);
      if (!hval.IsNull()) {
	Handle(TCollection_HAsciiString) strval = new TCollection_HAsciiString
	  (hval->ToCString());
	newatr = strval;
      }

    }
    theattrib.Bind(name, newatr);
  }
}
