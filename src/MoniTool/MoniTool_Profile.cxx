// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <MoniTool_Profile.ixx>
#include <TCollection_HAsciiString.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <MoniTool_TypedValue.hxx>


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
