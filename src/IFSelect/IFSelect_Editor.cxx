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


#include <IFSelect_EditForm.hxx>
#include <IFSelect_Editor.hxx>
#include <IFSelect_ListEditor.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_MSG.hxx>
#include <Interface_TypedValue.hxx>
#include <Message_Messenger.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <string.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Editor,Standard_Transient)

IFSelect_Editor::IFSelect_Editor (const Standard_Integer nbval)
    : thenbval (nbval) , themaxsh (0) , themaxco (0) , themaxla (0) ,
      thevalues (1,nbval) , theshorts (1,nbval) , themodes (1,nbval) ,
      thelists  (1,nbval)
      {  thelists.Init(-1);  }

    void  IFSelect_Editor::SetNbValues (const Standard_Integer nbval)
{
  if (nbval > thevalues.Upper()) throw Standard_OutOfRange("IFSelect_Editor:SetNbValues");
  thenbval = nbval;
}

    void  IFSelect_Editor::SetValue
  (const Standard_Integer num, const Handle(Interface_TypedValue)& typval,
   const Standard_CString shortname, const IFSelect_EditValue editmode)
{
  if (num < 1 || num > thenbval) return;
  TCollection_AsciiString shn (shortname);
  Standard_Integer lng = shn.Length();
  if (lng > 0) thenames.Bind (shortname,num);
  if (lng > themaxsh) themaxsh = lng;
  lng = (Standard_Integer) strlen (typval->Name());
  if (lng > themaxco) themaxco = lng;
  lng = (Standard_Integer) strlen (typval->Label());
  if (lng > themaxla) themaxla = lng;

  thenames.Bind (typval->Name(),num);
  Standard_Integer edm = (Standard_Integer) editmode;
  thevalues.SetValue (num,typval);
  theshorts.SetValue (num,shn);
  themodes.SetValue  (num,edm);
}

    void  IFSelect_Editor::SetList
  (const Standard_Integer num, const Standard_Integer max)
{
  if (num < 1 || num > thenbval) return;
  thelists.SetValue (num,max);
}

    Standard_Integer  IFSelect_Editor::NbValues () const
      {  return thenbval;  }

    Handle(Interface_TypedValue)  IFSelect_Editor::TypedValue
  (const Standard_Integer num) const
    {  return Handle(Interface_TypedValue)::DownCast(thevalues.Value(num));  }

Standard_Boolean  IFSelect_Editor::IsList  (const Standard_Integer num) const
{
  if (num < 1 || num > thenbval) return Standard_False;
  return (thelists.Value(num) >= 0);
}

Standard_Integer  IFSelect_Editor::MaxList (const Standard_Integer num) const
{
  if (num < 1 || num > thenbval) return -1;
  return thelists.Value(num);
}

    Standard_CString  IFSelect_Editor::Name
  (const Standard_Integer num, const Standard_Boolean isshort) const
{
  if (num < 1 || num > thenbval) return "";
  if (isshort) return theshorts.Value (num).ToCString();
  else return TypedValue (num)->Name();
}

    IFSelect_EditValue  IFSelect_Editor::EditMode
  (const Standard_Integer num) const
{
  if (num < 1 || num > thenbval) return IFSelect_EditDynamic;
  Standard_Integer edm = themodes.Value(num);
  return (IFSelect_EditValue) edm;
}

    void  IFSelect_Editor::PrintNames (const Handle(Message_Messenger)& S) const
{
  Standard_Integer i, nb = NbValues();
  S<<"****    Editor : "<<Label()<<endl;
  S<<"****    Nb Values = "<<nb<<"    ****    Names / Labels"<<endl;
  S<<" Num ";
  if (themaxsh > 0) S<<"Short"<<Interface_MSG::Blanks("Short",themaxsh)<<" ";
  S<<"Complete"<<Interface_MSG::Blanks("Complete",themaxco)<<"  Label"<<endl;

  for (i = 1; i <= nb; i ++) {
    Handle(Interface_TypedValue) tv = TypedValue(i);
    if (tv.IsNull()) continue;
    S<<Interface_MSG::Blanks(i,3)<<i<<" ";
    if (themaxsh > 0) {
      const TCollection_AsciiString& sho = theshorts(i);
      S<<sho<<Interface_MSG::Blanks(sho.ToCString(),themaxsh)<<" ";
    }
    S<<tv->Name()<<Interface_MSG::Blanks(tv->Name(),themaxco)<<"  "<<tv->Label()<<endl;
  }
}

    void  IFSelect_Editor::PrintDefs
  (const Handle(Message_Messenger)& S, const Standard_Boolean labels) const
{
  Standard_Integer i, nb = NbValues();
  S<<"****    Editor : "<<Label()<<endl;
  S<<"****    Nb Values = "<<nb<<"    ****    "<<(labels ? "Labels" : "Names")<<"  /  Definitions"<<endl;
  S<<" Num ";
  if (labels) S<<"Label"<<Interface_MSG::Blanks("Label",themaxla);
  else {
    if (themaxsh > 0) S<<"Short"<<Interface_MSG::Blanks("Short",themaxsh+1);
    S<<"Complete"<<Interface_MSG::Blanks("Complete",themaxco);
  }
  S<<"  Edit Mode  &  Definition"<<endl;

  for (i = 1; i <= nb; i ++) {
    Handle(Interface_TypedValue) tv = TypedValue(i);
    if (tv.IsNull()) continue;
    S<<" "<<Interface_MSG::Blanks(i,3)<<i<<" ";
    if (labels) S<<tv->Label()<<Interface_MSG::Blanks(tv->Label(),themaxla);
    else {
      if (themaxsh > 0) {
	const TCollection_AsciiString& sho = theshorts(i);
	S<<sho<<Interface_MSG::Blanks(sho.ToCString(),themaxsh)<<" ";
      }
      S<<tv->Name()<<Interface_MSG::Blanks(tv->Name(),themaxco);
    }

    S<<" ";
    Standard_Integer maxls = MaxList (i);
    if (maxls == 0) S<<" (List) ";
    else if (maxls > 0) S<<" (List <= "<<maxls<<" Items) ";
    else S<<" ";
    IFSelect_EditValue edm = EditMode (i);
    switch (edm) {
      case IFSelect_Optional      : S<<"Optional ";  break;
      case IFSelect_Editable      : S<<"Editable ";  break;
      case IFSelect_EditProtected : S<<"Protected";  break;
      case IFSelect_EditComputed  : S<<"Computed ";  break;
      case IFSelect_EditRead      : S<<"ReadOnly ";  break;
      case IFSelect_EditDynamic   : S<<"Dynamic  ";  break;
      default :                     S<<"?????????";  break;
    }

    S<<" "<<tv->Definition()<<endl;
  }
}


    Standard_Integer  IFSelect_Editor::MaxNameLength
  (const Standard_Integer what) const
{
  if (what == -1) return themaxsh;
  if (what ==  0) return themaxco;
  if (what ==  1) return themaxla;
  return 0;
}


    Standard_Integer  IFSelect_Editor::NameNumber
  (const Standard_CString name) const
{
  Standard_Integer res;
  if (thenames.Find(name,res))
    return res;
  res = atoi (name);  // si c est un entier, on tente le coup
  if (res < 1 || res > NbValues()) res = 0;
  return res;
}


    Handle(IFSelect_EditForm)  IFSelect_Editor::Form
  (const Standard_Boolean readonly, const Standard_Boolean undoable) const
{
  return new IFSelect_EditForm (this,readonly,undoable,Label().ToCString());
}

    Handle(IFSelect_ListEditor)  IFSelect_Editor::ListEditor
  (const Standard_Integer num) const
{
  Handle(IFSelect_ListEditor) led;
  Standard_Integer max = MaxList (num);
  if (max < 0) return led;
  led = new IFSelect_ListEditor (TypedValue(num),max);
  return led;
}

    Handle(TColStd_HSequenceOfHAsciiString)  IFSelect_Editor::ListValue
  (const Handle(IFSelect_EditForm)& /*form*/, const Standard_Integer /*num*/) const
{
  Handle(TColStd_HSequenceOfHAsciiString) list;
  return list;
}


    Standard_Boolean  IFSelect_Editor::Update
  (const Handle(IFSelect_EditForm)& /*form*/, const Standard_Integer /*num*/,
   const Handle(TCollection_HAsciiString)& /*newval*/,
   const Standard_Boolean /*enforce*/) const
      {  return Standard_True;  }

    Standard_Boolean  IFSelect_Editor::UpdateList
  (const Handle(IFSelect_EditForm)& /*form*/, const Standard_Integer /*num*/,
   const Handle(TColStd_HSequenceOfHAsciiString)& /*newval*/,
   const Standard_Boolean /*enforce*/) const
      {  return Standard_True;  }
