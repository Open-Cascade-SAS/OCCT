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


#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <MoniTool_Option.hxx>
#include <MoniTool_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

MoniTool_Option::MoniTool_Option
  (const Handle(Standard_Type)& atype, const Standard_CString aname)
    : thename (aname) , thetype (atype) 
      {  theitems = new Dico_DictionaryOfTransient();  }

    MoniTool_Option::MoniTool_Option
  (const Handle(MoniTool_TypedValue)& aval, const Standard_CString aname)
    : thename (aname) , 
      thetype (STANDARD_TYPE(TCollection_HAsciiString)) ,
      thevalue (aval)  
      {  theitems = new Dico_DictionaryOfTransient();
         if (thename.Length() == 0) thename.AssignCat (aval->Name());  }

    MoniTool_Option::MoniTool_Option
  (const Handle(MoniTool_Option)& other, const Standard_CString aname)
    : thename (aname) ,
      thetype (other->Type()) , 
      thevalue (other->TypedValue()) ,
      theitems (other->Items())
{
  if (thename.Length() == 0) thename.AssignCat (other->Name());
  thecase.Clear();  thecase.AssignCat (other->CaseName());
  other->Value(theval);
}

    Standard_Boolean  MoniTool_Option::Add
  (const Standard_CString name, const Handle(Standard_Transient)& val)
{
  if (val.IsNull()) return Standard_False;
  if (!val->IsKind (thetype)) return Standard_False;
  if (!thevalue.IsNull()) {
    Handle(TCollection_HAsciiString) str = Handle(TCollection_HAsciiString)::DownCast(val);
    if (str.IsNull()) return Standard_False;
    if (!thevalue->Satisfies(str)) return Standard_False;
  }
  theitems->SetItem (name,val);
  thecase.Clear();  thecase.AssignCat (name);  // switch mis a jour par defaut
  theval = val;
  return Standard_True;
}

    Standard_Boolean  MoniTool_Option::AddBasic
  (const Standard_CString name, const Standard_CString val)
{
  if (thevalue.IsNull()) return Standard_False;
  Handle(TCollection_HAsciiString) str;
  if (val && val[0] != '\0') str = new TCollection_HAsciiString (val);
  else str = new TCollection_HAsciiString (name);
  return Add (name,str);
}


    void  MoniTool_Option::Duplicate ()
{
  Handle(Dico_DictionaryOfTransient) items = new Dico_DictionaryOfTransient();
  Dico_IteratorOfDictionaryOfTransient iter(theitems);
  for (; iter.More(); iter.Next())
    items->SetItem (iter.Name().ToCString(),iter.Value());
  theitems = items;
}

    const TCollection_AsciiString&  MoniTool_Option::Name () const
      {  return thename;  }

    Handle(Standard_Type)  MoniTool_Option::Type () const
      {  return thetype;  }

    Handle(MoniTool_TypedValue)  MoniTool_Option::TypedValue () const
      {  return thevalue;  }

    Handle(Dico_DictionaryOfTransient)  MoniTool_Option::Items () const
      {  return theitems;  }

    Standard_Boolean  MoniTool_Option::Item
  (const Standard_CString name, Handle(Standard_Transient)& val) const
{
  Handle(Standard_Transient) v;
  if (!theitems->GetItem (name,v)) return Standard_False;
  val = v;
  return Standard_True;
}


    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_Option::ItemList () const
{
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  Dico_IteratorOfDictionaryOfTransient iter(theitems);
  for (; iter.More(); iter.Next())  list->Append (iter.Name());
  return list;
}

    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_Option::Aliases
  (const Standard_CString name, const Standard_Boolean exact) const
{
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  Handle(Standard_Transient) v;
  if (!theitems->GetItem (name,v, exact)) return list;

  Dico_IteratorOfDictionaryOfTransient iter(theitems);
  for (; iter.More(); iter.Next()) {
    if (iter.Value() != v) continue;
    TCollection_AsciiString itname = iter.Name();
    if (!itname.IsEqual(name)) list->Append (itname);
  }
  return list;
}

//    ####    SWITCH ACTIONS    ####

    Standard_Boolean  MoniTool_Option::Switch (const Standard_CString name)
{
  Handle(Standard_Transient) val;
  if (!theitems->GetItem (name,val)) return Standard_False;
  thecase.Clear();  thecase.AssignCat (name);
  theval = val;
  return Standard_True;
}

    const TCollection_AsciiString&  MoniTool_Option::CaseName () const
      {  return thecase;  }

    Handle(Standard_Transient)  MoniTool_Option::CaseValue () const
      {  return theval;  }

    void  MoniTool_Option::Value (Handle(Standard_Transient)& val) const
      {  if (!theval.IsNull()) val = theval;  }
