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
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_EditForm,Standard_Transient)

IFSelect_EditForm::IFSelect_EditForm
  (const Handle(IFSelect_Editor)& editor,
   const Standard_Boolean readonly, const Standard_Boolean undoable,
   const Standard_CString label)
    : thecomplete (Standard_True) , 
      theloaded (Standard_False) ,
      thekeepst (Standard_False)  ,
      thelabel (label) ,
      thenums (0,1) ,
      theorigs  (0, (undoable ? editor->NbValues() : 0) ) ,
      themodifs (0, (readonly ? 0 : editor->NbValues()) ) ,
      thestatus (0, (readonly ? 0 : editor->NbValues()) ) ,
      theeditor (editor) , 
      thetouched (0)    {  }

    IFSelect_EditForm::IFSelect_EditForm
  (const Handle(IFSelect_Editor)& editor,
   const TColStd_SequenceOfInteger& nums,
   const Standard_Boolean readonly, const Standard_Boolean undoable,
   const Standard_CString label)
    : thecomplete (Standard_False) , 
      theloaded (Standard_False) ,
      thekeepst (Standard_False)   , 
      thelabel (label) , 
      thenums (0,nums.Length()) ,
      theorigs  (0, (undoable ? nums.Length() : 0) ) ,
      themodifs (0, (readonly ? 0 : nums.Length()) ) ,
      thestatus (0, (readonly ? 0 : nums.Length()) ) ,
      theeditor (editor) , 
      thetouched (0)
{
  Standard_Integer i,nb = nums.Length();
  for (i = 1; i <= nb; i ++) thenums.SetValue (i,nums.Value(i));
}

    Standard_Boolean& IFSelect_EditForm::EditKeepStatus ()
      {  return thekeepst;  }

    Standard_CString  IFSelect_EditForm::Label () const
      {  return thelabel.ToCString();  }

    Standard_Boolean  IFSelect_EditForm::IsLoaded () const
      {  return theloaded;  }

    void  IFSelect_EditForm::ClearData ()
      {  theent.Nullify();  themodel.Nullify();  theloaded = Standard_False;  }

    void  IFSelect_EditForm::SetData
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model)
      {  theent = ent;  themodel = model;  }

    void  IFSelect_EditForm::SetEntity
  (const Handle(Standard_Transient)& ent)
      {  theent = ent;  }

    void  IFSelect_EditForm::SetModel
  (const Handle(Interface_InterfaceModel)& model)
      {  themodel = model;  }

    Handle(Standard_Transient)  IFSelect_EditForm::Entity () const
      {  return theent;  }

    Handle(Interface_InterfaceModel)  IFSelect_EditForm::Model () const
      {  return themodel;  }

    Handle(IFSelect_Editor)  IFSelect_EditForm::Editor () const
      {  return theeditor;  }

    Standard_Boolean  IFSelect_EditForm::IsComplete () const
      {  return thecomplete;  }

    Standard_Integer  IFSelect_EditForm::NbValues
  (const Standard_Boolean editable) const
{
  if (!editable || thecomplete) return theeditor->NbValues();
  return thenums.Upper();
}

    Standard_Integer  IFSelect_EditForm::NumberFromRank
  (const Standard_Integer rank) const
{
  if (thecomplete) return rank;
  if (rank < 1 || rank > thenums.Upper()) return 0;
  return thenums.Value(rank);
}

    Standard_Integer  IFSelect_EditForm::RankFromNumber
  (const Standard_Integer num) const
{
  if (thecomplete) return num;
  Standard_Integer i, n = thenums.Upper();
  for (i = 1; i <= n; i ++) {
    if (thenums.Value(i) == num) return i;
  }
  return 0;
}

    Standard_Integer  IFSelect_EditForm::NameNumber
  (const Standard_CString name) const
{
  Standard_Integer res = theeditor->NameNumber(name);
  if (thecomplete || res == 0) return res;
//   Sinon, chercher res dans thenums
  Standard_Integer i, nb = thenums.Length();
  for (i = 1; i <= nb; i ++) {
    if (res == thenums.Value(i)) return res;
  }
  return -res;
}

    Standard_Integer  IFSelect_EditForm::NameRank
  (const Standard_CString name) const
{
  Standard_Integer res = theeditor->NameNumber(name);
  if (thecomplete || res == 0) return res;
//   Sinon, chercher res dans thenums
  Standard_Integer i, nb = thenums.Length();
  for (i = 1; i <= nb; i ++) {
    if (res == thenums.Value(i)) return i;
  }
  return 0;
}


    void  IFSelect_EditForm::LoadDefault ()
{
  theloaded = Standard_True;
  thetouched = 0;
  Standard_Integer i,nb = theorigs.Upper();
  if (nb == 0) return;
  for (i = 1; i <= nb; i ++) {
    Standard_Integer num = NumberFromRank(i);
    if (num == 0) continue;
    Handle(TCollection_HAsciiString) str = theeditor->StringValue (this,num);
    theorigs.SetValue (i,str);
  }
}


    Standard_Boolean  IFSelect_EditForm::LoadData
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model)
{
  thetouched = 0;
  if (!theeditor->Load (this,ent,model)) return Standard_False;
  SetData (ent,model);
  theloaded = Standard_True;
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::LoadEntity
  (const Handle(Standard_Transient)& ent)
{
  thetouched = 0;
  Handle(Interface_InterfaceModel) model;
  if (!theeditor->Load (this,ent,model)) return Standard_False;
  SetEntity (ent);
  theloaded = Standard_True;
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::LoadModel
  (const Handle(Interface_InterfaceModel)& model)
{
  thetouched = 0;
  Handle(Standard_Transient) ent;
  if (!theeditor->Load (this,ent,model)) return Standard_False;
  SetData (ent,model);
  theloaded = Standard_True;
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::LoadData ()
{
  thetouched = 0;
  Handle(Interface_InterfaceModel) model;
  Handle(Standard_Transient) ent;
  if (!theeditor->Load (this,ent,model)) return Standard_False;
  theloaded = Standard_True;
  return Standard_True;
}


//  ########    VALUES    ########

    Handle(IFSelect_ListEditor)  IFSelect_EditForm::ListEditor
  (const Standard_Integer num) const
{
  Standard_Integer n = RankFromNumber(num);
  Handle(IFSelect_ListEditor) led;
  if (n <= 0 || n > theorigs.Upper()) return led;
  if (!theeditor->IsList(n)) return led;
  led = theeditor->ListEditor (num);
  Handle(TColStd_HSequenceOfHAsciiString) lis = theeditor->ListValue(this,num);
  led->LoadModel (themodel);
  led->LoadValues (lis);
  return led;
}

    void  IFSelect_EditForm::LoadValue
  (const Standard_Integer num, const Handle(TCollection_HAsciiString)& val)
{
  Standard_Integer n = RankFromNumber(num);
  if (n <= 0 || n > theorigs.Upper()) return;
  theorigs.SetValue (n,val);
}

    void  IFSelect_EditForm::LoadList
  (const Standard_Integer num, const Handle(TColStd_HSequenceOfHAsciiString)& list)
{
  Standard_Integer n = RankFromNumber(num);
  if (n <= 0 || n > theorigs.Upper()) return;
  theorigs.SetValue (n,list);
}


    Handle(TCollection_HAsciiString)  IFSelect_EditForm::OriginalValue
  (const Standard_Integer num) const
{
  Standard_Integer n = RankFromNumber(num);
  Handle(TCollection_HAsciiString) val;
  if (theorigs.Upper() == 0) return  theeditor->StringValue (this,num);
  else return Handle(TCollection_HAsciiString)::DownCast(theorigs.Value(n));
}

    Handle(TColStd_HSequenceOfHAsciiString)  IFSelect_EditForm::OriginalList
  (const Standard_Integer num) const
{
  Standard_Integer n = RankFromNumber(num);
  Handle(TColStd_HSequenceOfHAsciiString) list;
  if (theorigs.Upper() == 0) return  theeditor->ListValue (this,num);
  else return Handle(TColStd_HSequenceOfHAsciiString)::DownCast(theorigs.Value(n));
}

    Handle(TCollection_HAsciiString)  IFSelect_EditForm::EditedValue
  (const Standard_Integer num) const
{
  if (themodifs.Upper() == 0) return OriginalValue(num);
  if (!IsModified(num)) return OriginalValue(num);
  Standard_Integer n = RankFromNumber(num);
  return Handle(TCollection_HAsciiString)::DownCast(themodifs.Value(n));
}

    Handle(TColStd_HSequenceOfHAsciiString)  IFSelect_EditForm::EditedList
  (const Standard_Integer num) const
{
  if (themodifs.Upper() == 0) return OriginalList(num);
  if (!IsModified(num)) return OriginalList(num);
  Standard_Integer n = RankFromNumber(num);
  return Handle(TColStd_HSequenceOfHAsciiString)::DownCast(themodifs.Value(n));
}


    Standard_Boolean  IFSelect_EditForm::IsModified
  (const Standard_Integer num) const
{
  if (thestatus.Upper() == 0) return Standard_False;
  Standard_Integer n = RankFromNumber(num);
  return (thestatus.Value(n) != 0);
}

    Standard_Boolean  IFSelect_EditForm::IsTouched
  (const Standard_Integer num) const
{
  if (thestatus.Upper() == 0) return Standard_False;
  Standard_Integer n = RankFromNumber(num);
  return (thestatus.Value(n) == 2);
}

    Standard_Boolean  IFSelect_EditForm::Modify
  (const Standard_Integer num, const Handle(TCollection_HAsciiString)& newval,
   const Standard_Boolean enforce)
{
//  Peut-on editer
  thetouched = 0;
  if (themodifs.Upper() == 0) return Standard_False;
  Standard_Integer tnum = RankFromNumber(num);
  if (tnum == 0) return Standard_False;
  IFSelect_EditValue acc = theeditor->EditMode (num);
  if (newval.IsNull() && acc != IFSelect_Optional) return Standard_False;
  if (!enforce && (acc == IFSelect_EditProtected || acc == IFSelect_EditComputed)) return Standard_False;

//  Satisfies ?
  Handle(Interface_TypedValue) typval = theeditor->TypedValue(num);
  if (!typval->Satisfies(newval)) return Standard_False;
  Interface_ParamType pty = typval->Type();
  if (pty == Interface_ParamIdent && !newval.IsNull()) {
    if (themodel.IsNull()) return Standard_False;
    if (themodel->NextNumberForLabel(newval->ToCString(),0,Standard_False) <= 0)
      return Standard_False;
  }

//  Update ?
  if (!theeditor->Update(this,num,newval,enforce)) return Standard_False;

  thestatus.SetValue (tnum,1);
  themodifs.SetValue (tnum,newval);
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::ModifyList
  (const Standard_Integer num, const Handle(IFSelect_ListEditor)& edited,
   const Standard_Boolean enforce)
{
//  Faut-il prendre
  if (edited.IsNull()) return Standard_False;
  if (!edited->IsTouched()) return Standard_False;
  Handle(TColStd_HSequenceOfHAsciiString) newlist = edited->EditedValues();

//  Peut-on editer
  thetouched = 0;
  if (themodifs.Upper() == 0) return Standard_False;
  Standard_Integer tnum = RankFromNumber(num);
  if (tnum == 0) return Standard_False;
  IFSelect_EditValue acc = theeditor->EditMode (num);
  if (acc == IFSelect_EditRead || acc == IFSelect_EditDynamic) return Standard_False;
  if (newlist.IsNull() && acc != IFSelect_Optional) return Standard_False;
  if (!enforce && (acc == IFSelect_EditProtected || acc == IFSelect_EditComputed)) return Standard_False;

//  Update ?
  if (!theeditor->UpdateList(this,num,newlist,enforce)) return Standard_False;

  thestatus.SetValue (tnum,1);
  themodifs.SetValue (tnum,newlist);
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::ModifyListValue
  (const Standard_Integer num, const Handle(TColStd_HSequenceOfHAsciiString)& list,
   const Standard_Boolean enforce)
{
  Handle(IFSelect_ListEditor) led = ListEditor (num);
  if (led.IsNull()) return Standard_False;
  if (!led->LoadEdited(list)) return Standard_False;
  return ModifyList (num,led,enforce);
}


    Standard_Boolean  IFSelect_EditForm::Touch
  (const Standard_Integer num, const Handle(TCollection_HAsciiString)& newval)
{
  if (themodifs.Upper() == 0) return Standard_False;
  Standard_Integer tnum = RankFromNumber(num);
  if (tnum == 0) return Standard_False;

  thestatus.SetValue (tnum,2);
  themodifs.SetValue (tnum,newval);
  thetouched ++;
  return Standard_True;
}

    Standard_Boolean  IFSelect_EditForm::TouchList
  (const Standard_Integer num, const Handle(TColStd_HSequenceOfHAsciiString)& newlist)
{
  if (themodifs.Upper() == 0) return Standard_False;
  Standard_Integer tnum = RankFromNumber(num);
  if (tnum == 0) return Standard_False;

  thestatus.SetValue (tnum,2);
  themodifs.SetValue (tnum,newlist);
  thetouched ++;
  return Standard_True;
}


    void  IFSelect_EditForm::ClearEdit (const Standard_Integer num)
{
  Standard_Integer i, nb = thestatus.Upper();
  if (num == 0) {
    for (i = 1; i <= nb; i ++)   thestatus.SetValue (i,0);
  } else {
    Standard_Integer tnum = RankFromNumber(num);
    if (tnum > 0 && num <= nb) thestatus.SetValue (tnum,0);
  }
}


    void  IFSelect_EditForm::PrintDefs (const Handle(Message_Messenger)& S) const
{
  Standard_Integer iv, nbv = NbValues(Standard_True);
  S<<"***** EditForm,  Label : "<<Label()<<endl;
  if (IsComplete()) S<<"Complete, "<<nbv<<" Values"<<endl;
  else {
    S<<"Extraction on "<<nbv<<" Values : (extracted<-editor)"<<endl;
    for (iv = 1; iv <= nbv; iv ++) S<<"  "<<iv<<"<-"<<NumberFromRank(iv);
    S<<endl;
  }
  S<<"*****"<<endl;
}


static void PrintList
  (const Handle(TColStd_HSequenceOfHAsciiString)& list,
   const Handle(Message_Messenger)& S, const Standard_Boolean alsolist)
{
  if (list.IsNull())  {  S<<"(NULL LIST)"<<endl;  return;  }

  Standard_Integer i,nb = list->Length();
  S<<"(List : "<<nb<<" Items)"<<endl;
  if (!alsolist) return;

  for (i = 1; i <= nb; i ++) {
    Handle(TCollection_HAsciiString) str = list->Value(i);
    S<<"  ["<<i<<"]	"<< (str.IsNull() ? "(NULL)" : str->ToCString())<<endl;
  }
}

    void  IFSelect_EditForm::PrintValues
  (const Handle(Message_Messenger)& S, const Standard_Integer what,
   const Standard_Boolean names, const Standard_Boolean alsolist) const
{
  Standard_Integer iv, nbv = NbValues(Standard_True);
  S<<  "****************************************************"<<endl;
  S<<"*****  "<<Label()<<Interface_MSG::Blanks(Label(),40)<<"*****"<<endl;
  S<<"*****                                          *****"<<endl;
  if (!theloaded)
    S<<"*****         Values are NOT loaded            *****"<<endl;

  else {
//  Donnees sur lesquelles on a travaille
    if (themodel.IsNull()) {
      if (theent.IsNull()) S<<"*****  No loaded data";
      else S<<"*****  No loaded Model. Loaded object : type "<<theent->DynamicType()->Name();
    } else {
      if (theent.IsNull()) S<<"*****  No loaded entity";
      else { S<<"*****  Loaded entity : "; themodel->PrintLabel (theent,S); }
    }
  }
  S<<endl<<"****************************************************"<<endl<<endl;

//  Affichage des valeurs
  Standard_Boolean nams = names;
  Standard_Integer maxnam = theeditor->MaxNameLength (names ? 0 : -1);
  if (maxnam == 0) { maxnam = theeditor->MaxNameLength (0); nams = Standard_True; }
  Standard_Integer nbmod = 0;
  if (what != 0) S<<"Mod N0 Name               Value"<<endl;
  else S<<" N0 Name               Value"<<endl;

  for (iv = 1; iv <= nbv; iv ++) {
    Standard_Integer jv = NumberFromRank(iv);
    Standard_CString name = theeditor->Name(jv,!nams);

//     Original ou Final
    if (what != 0) {
      Handle(TCollection_HAsciiString) str;
      if (IsModified(jv)) S<<"* ";
      else S<<"  ";
      S <<Interface_MSG::Blanks(iv,3)<<iv<<" "
	<<name<<Interface_MSG::Blanks(name,maxnam)<<"  ";

      if (theeditor->IsList(jv)) {
	Handle(TColStd_HSequenceOfHAsciiString) list;
	if (what < 0) list = OriginalList (jv);
	if (what > 0) list = EditedList (jv);
	PrintList (list,S,alsolist);
	continue;
      }

      if (what < 0) str = OriginalValue (jv);
      if (what > 0) str = EditedValue (jv);

      S << (str.IsNull() ? "(NULL)" : str->ToCString()) <<endl;

//    Modified only
    } else {
      if (!IsModified(jv)) continue;
      nbmod ++;
      if (theeditor->IsList(jv)) {
	Handle(TColStd_HSequenceOfHAsciiString) list= OriginalList (jv);
	S <<Interface_MSG::Blanks(iv,3)<<iv<<" "
	  <<name<<Interface_MSG::Blanks(name,maxnam)<<" ORIG:";
	PrintList (list,S,alsolist);

	list = EditedList (jv);
	S<<Interface_MSG::Blanks("",maxnam+5)<<"MOD :";
	PrintList (list,S,alsolist);

	continue;
      }

      Handle(TCollection_HAsciiString) str = OriginalValue (jv);
      S <<Interface_MSG::Blanks(iv,3)<<iv<<" "
	<<name<<Interface_MSG::Blanks(name,maxnam)<<" ORIG:"
	<< (str.IsNull() ? "(NULL)" : str->ToCString()) <<endl;
      str = EditedValue (jv);
      S<<Interface_MSG::Blanks("",maxnam+4)<<" MOD :"<< (str.IsNull() ? "(NULL)" : str->ToCString()) <<endl;
    }
  }
  if (what == 0) S<<"On "<<nbv<<" Values, "<<nbmod<<" Modified"<<endl;
}


    Standard_Boolean  IFSelect_EditForm::Apply ()
{
  Standard_Boolean stat = ApplyData(theent,themodel);
  if (stat && !thekeepst) ClearEdit();
  return stat;
}


    Standard_Boolean  IFSelect_EditForm::Recognize () const
      {  return theeditor->Recognize(this);  }

    Standard_Boolean  IFSelect_EditForm::ApplyData
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model)
      {  return theeditor->Apply (this,ent,model);  }


    Standard_Boolean  IFSelect_EditForm::Undo ()
{
  if (thestatus.Upper() == 0 || theorigs.Upper() == 0) return Standard_False;
  Standard_Integer i, nb = thestatus.Upper();
  for (i = 1; i <= nb; i ++)  {
    if (thestatus.Value (i) != 0) themodifs.SetValue (i,theorigs.Value(i));
  }
  return Apply ();
}
