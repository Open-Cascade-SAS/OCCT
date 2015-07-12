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
#include <MoniTool_Profile.hxx>
#include <MoniTool_TypedValue.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

static Standard_Boolean IsCurrent (const Standard_CString name)
{  return (name[0] == '.' && name[1] == '\0');  }

    MoniTool_Profile::MoniTool_Profile ()
{
  theopts  = new Dico_DictionaryOfTransient;
  theconfs = new Dico_DictionaryOfTransient;
//    Current
  Handle(Dico_DictionaryOfTransient) conf = new Dico_DictionaryOfTransient;
  theconfs->SetItem (".",conf);
  thecurname.AssignCat(".");
  thecurconf = conf;
}

    void  MoniTool_Profile::AddOption
  (const Handle(MoniTool_Option)& option,
   const Standard_CString name)
{
  if (option.IsNull()) return;
  if (name[0] == '\0') theopts->SetItem (option->Name().ToCString(),option);
  else theopts->SetItem (name,option);
}

    Handle(MoniTool_Option)  MoniTool_Profile::Option
  (const Standard_CString name) const
{
  Handle(MoniTool_Option) opt;
  if (!theopts->GetItem (name,opt,Standard_True)) opt.Nullify();
  return opt;
}

    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_Profile::OptionList () const
{
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  Dico_IteratorOfDictionaryOfTransient iter(theopts);
  for (; iter.More(); iter.Next())  list->Append (iter.Name());
  return list;
}

    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_Profile::TypedValueList () const
{
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  Dico_IteratorOfDictionaryOfTransient iter(theopts);
  for (; iter.More(); iter.Next()) {
    Handle(MoniTool_Option) opt = Handle(MoniTool_Option)::DownCast(iter.Value());
    if (!opt->TypedValue().IsNull())  list->Append (iter.Name());
  }
  return list;
}

    void  MoniTool_Profile::NewConf (const Standard_CString name)
{
  if (IsCurrent (name)) return;
  Handle(Dico_DictionaryOfTransient) conf = new Dico_DictionaryOfTransient;
  theconfs->SetItem (name,conf);
//  thecurname.Clear();  thecurname.AssignCat (name);
//  thecurconf = conf;
}

    void  MoniTool_Profile::AddConf (const Standard_CString name)
{
  if (IsCurrent (name)) return;
  Handle(Dico_DictionaryOfTransient) conf;
  if (theconfs->GetItem (name,conf,Standard_False)) return;
  conf = new Dico_DictionaryOfTransient;
  theconfs->SetItem (name,conf);
//  thecurname.Clear();  thecurname.AssignCat (name);
//  thecurconf = conf;
}

    Standard_Boolean  MoniTool_Profile::HasConf (const Standard_CString name) const
      {  return theconfs->HasItem (name,Standard_False);  }

    Handle(TColStd_HSequenceOfAsciiString)  MoniTool_Profile::ConfList () const
{ 
  Handle(TColStd_HSequenceOfAsciiString) list = new TColStd_HSequenceOfAsciiString();
  Dico_IteratorOfDictionaryOfTransient iter(theconfs);
  for (; iter.More(); iter.Next())  list->Append (iter.Name());
  return list;
}

    Handle(Dico_DictionaryOfTransient)  MoniTool_Profile::Conf
  (const Standard_CString name) const
{
  Handle(Dico_DictionaryOfTransient) conf;
  if (!theconfs->GetItem (name,conf,Standard_False)) conf.Nullify();
  return conf;
}


    Standard_Boolean  MoniTool_Profile::ClearConf
  (const Standard_CString confname)
{
  Handle(Dico_DictionaryOfTransient) conf;
  if (!theconfs->GetItem (confname,conf,Standard_False)) return Standard_False;
  conf->Clear();
  return Standard_True;
}

    Standard_Boolean  MoniTool_Profile::AddFromOtherConf
  (const Standard_CString confname, const Standard_CString otherconf)
{
  Handle(Dico_DictionaryOfTransient) conf, other;
  if (!theconfs->GetItem (confname,conf,Standard_False)) return Standard_False;
  if (!theconfs->GetItem (otherconf,other,Standard_False)) return Standard_False;
  if (conf == other) return Standard_True;
  Dico_IteratorOfDictionaryOfTransient iter(other);
  for (; iter.More(); iter.Next())    conf->SetItem (iter.Name(),iter.Value());
  return Standard_True;
}

    Standard_Boolean  MoniTool_Profile::SetFromCurrent
  (const Standard_CString confname)
{
  Handle(Dico_DictionaryOfTransient) conf;
  if (!theconfs->GetItem (confname,conf,Standard_False)) return Standard_False;
  Dico_IteratorOfDictionaryOfTransient iter(theopts);
  for (; iter.More(); iter.Next()) {
    TCollection_AsciiString name = iter.Name();
    TCollection_AsciiString cn   = CaseName (name.ToCString());
    AddSwitch (name.ToCString(), cn.ToCString() );
  }
  return Standard_True;
}


    Standard_Boolean  MoniTool_Profile::AddSwitch
  (const Standard_CString confname,
   const Standard_CString optname,
   const Standard_CString casename)
{
  Handle(Dico_DictionaryOfTransient) conf = Conf (confname);
  Handle(MoniTool_Option) opt = Option (optname);
  if (conf.IsNull() || opt.IsNull()) return Standard_False;
  Handle(TCollection_HAsciiString) sw = new TCollection_HAsciiString(casename);
  if (casename[0] == '\0') sw = new TCollection_HAsciiString (opt->CaseName());
  Handle(Standard_Transient) val;
  if (!opt->Item (sw->ToCString(),val)) return Standard_False;
  conf->SetItem (optname,sw);
  return Standard_True;
}

    Standard_Boolean  MoniTool_Profile::RemoveSwitch
  (const Standard_CString confname,
   const Standard_CString optname)
{
  Handle(Dico_DictionaryOfTransient) conf = Conf (confname);
  Handle(MoniTool_Option) opt = Option (optname);
  if (conf.IsNull() || opt.IsNull()) return Standard_False;
  conf->RemoveItem (optname,Standard_False,Standard_False);
  return Standard_True;
}


    void  MoniTool_Profile::SwitchList
  (const Standard_CString confname,
   Handle(TColStd_HSequenceOfAsciiString)& optlist,
   Handle(TColStd_HSequenceOfAsciiString)& caselist) const
{
  optlist  = new TColStd_HSequenceOfAsciiString();
  caselist = new TColStd_HSequenceOfAsciiString();
  Handle(Dico_DictionaryOfTransient) conf = Conf (confname);
  if (conf.IsNull()) return;
  Dico_IteratorOfDictionaryOfTransient iter (conf);
  for (; iter.More(); iter.Next()) {
    TCollection_AsciiString optname = iter.Name();
    Handle(TCollection_HAsciiString) cn =
      Handle(TCollection_HAsciiString)::DownCast(iter.Value());
    TCollection_AsciiString casename(cn->ToCString());
    optlist->Append(optname);
    caselist->Append(casename);
  }
}


    Standard_Boolean  MoniTool_Profile::SetCurrent
  (const Standard_CString confname)
{
  if (!AddFromOtherConf (".",confname)) return Standard_False;
  thecurname.Clear();  thecurname.AssignCat (confname);
  return Standard_True;
}

    void  MoniTool_Profile::RecordCurrent ()
{
  Dico_IteratorOfDictionaryOfTransient iter(theconfs);
  for (; iter.More(); iter.Next()) {
    Handle(MoniTool_Option) opt = Option (iter.Name().ToCString());
    Handle(TCollection_HAsciiString) val =
      Handle(TCollection_HAsciiString)::DownCast (iter.Value());
    if (!val.IsNull())  opt->Switch (val->ToCString());
  }
}

    const TCollection_AsciiString&  MoniTool_Profile::Current () const
      {  return thecurname;  }

    TCollection_AsciiString  MoniTool_Profile::CaseName
  (const Standard_CString optname, const Standard_Boolean proper) const
{
  TCollection_AsciiString cn;
  Handle(TCollection_HAsciiString) sw;
  if (thecurconf->GetItem (optname,sw,Standard_True)) {
    cn.AssignCat (sw->ToCString());
    return cn;
  }
  if (proper) return cn;
  Handle(MoniTool_Option) opt = Option (optname);
  if (opt.IsNull()) return cn;
  return opt->CaseName();
}

    Handle(Standard_Transient)  MoniTool_Profile::CaseValue
  (const Standard_CString optname) const
{
  Handle(Standard_Transient) val;
  Handle(MoniTool_Option) opt = Option (optname);
  if (opt.IsNull()) return val;

  Handle(TCollection_HAsciiString) sw;
  if (!thecurconf->GetItem (optname,sw,Standard_True)) sw.Nullify();

  if (sw.IsNull()) return opt->CaseValue();
  if (!opt->Item (sw->ToCString(),val)) val.Nullify();
  return val;
}


    Standard_Boolean  MoniTool_Profile::Value
  (const Standard_CString optname,
   Handle(Standard_Transient)& val) const
{
  Handle(MoniTool_Option) opt = Option (optname);
  if (opt.IsNull()) return Standard_False;

  Handle(TCollection_HAsciiString) sw;
  if (!thecurconf->GetItem (optname,sw,Standard_True)) sw.Nullify();

  if (sw.IsNull()) {  opt->Value(val);  return Standard_True;  }
  return opt->Item (sw->ToCString(),val);
}


    void  MoniTool_Profile::SetTypedValues
  (const Standard_Boolean proper, const Standard_CString name) const
{
  Dico_IteratorOfDictionaryOfTransient iter(theopts,name);
  for (; iter.More(); iter.Next()) {
    Handle(MoniTool_Option) opt = Handle(MoniTool_Option)::DownCast(iter.Value());
    Handle(MoniTool_TypedValue) tv = opt->TypedValue();
    if (tv.IsNull())  continue;

//    On recherche la valeur: en conf courante, sinon dans l option (cf proper)

    Handle(TCollection_HAsciiString) sw, val;
    if (!thecurconf->GetItem (name,sw,Standard_True)) sw.Nullify();
    if (!sw.IsNull()) {
      if (!opt->Item (sw->ToCString(),val)) val.Nullify();
    }
    if (val.IsNull() && !proper) opt->Value(val);

//    On applique
    if (!val.IsNull()) tv->SetHStringValue (val);

  }
}

    void  MoniTool_Profile::SetFast (const Standard_CString confname)
{
  Standard_Boolean cur = (confname[0] == '\0');
  thefastval = new Dico_DictionaryOfTransient;
  Dico_IteratorOfDictionaryOfTransient iter(theopts);
  for (; iter.More(); iter.Next()) {
    Handle(MoniTool_Option) opt = Handle(MoniTool_Option)::DownCast(iter.Value());
    if (opt.IsNull()) continue;
    Standard_Boolean iaopt = Standard_True;
    TCollection_AsciiString optnam = iter.Name();
    Standard_CString optname = optnam.ToCString();
    Handle(Standard_Transient) val;

    if (cur) {
      Handle(TCollection_HAsciiString) sw;
      if (!thecurconf->GetItem (optname,sw,Standard_True)) sw.Nullify();

      if (sw.IsNull()) opt->Value(val);
      else iaopt = opt->Item (sw->ToCString(),val);
    }
    else iaopt = opt->Item (confname,val);

//   Now, recording
    if (iaopt) thefastval->SetItem (optname,val);
  }
}


    void  MoniTool_Profile::ClearFast ()
      {  thefastval.Nullify(); }


    Standard_Boolean  MoniTool_Profile::FastValue
  (const Standard_CString optname,
   Handle(Standard_Transient)& val) const
{
  if (!thefastval.IsNull()) {
    if (thefastval->GetItem (optname,val,Standard_True)) return Standard_True;
  }
  return Value (optname,val);
}
