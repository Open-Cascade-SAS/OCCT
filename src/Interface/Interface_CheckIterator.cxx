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


#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntVal.hxx>
#include <Message_Messenger.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

static const Handle(Interface_Check)& nulcheck ()
{
  static Handle(Interface_Check) nulch = new Interface_Check;
  return nulch;
}


//=======================================================================
//function : Interface_CheckIterator
//purpose  : 
//=======================================================================

Interface_CheckIterator::Interface_CheckIterator ()
{
  Clear();
}


//=======================================================================
//function : Interface_CheckIterator
//purpose  : 
//=======================================================================

Interface_CheckIterator::Interface_CheckIterator(const Standard_CString name)
     : thename (name)
{
  Clear();
}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void Interface_CheckIterator::SetName (const Standard_CString name)
{
  thename.Clear();
  if (name[0] != '\0') thename.AssignCat(name);
}


//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString Interface_CheckIterator::Name () const
{
  return thename.ToCString();
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================

void Interface_CheckIterator::SetModel(const Handle(Interface_InterfaceModel)& model)
{
  themod = model;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Interface_CheckIterator::Model() const
{
  return themod;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Clear ()
{
  thelist = new Interface_HSequenceOfCheck();
  thenums = new TColStd_HSequenceOfInteger();
  thecurr = new Interface_IntVal;
  thecurr->CValue() = 1;
}


//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Merge (Interface_CheckIterator& other)
{
  themod = other.Model();
  for (other.Start(); other.More(); other.Next())
    Add (other.Value(),other.Number());
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Add(const Handle(Interface_Check)& ach,
                                  const Standard_Integer num)
{
  //  Add <meme num que le dernier> -> cumul des Checks
  if (ach->NbWarnings() + ach->NbFails() == 0) return;
  Standard_Integer nm = num;
  if (num <= 0 && ach->HasEntity()) {
    if (!themod.IsNull()) {
      nm = themod->Number (ach->Entity());
      if (nm <= 0) nm = -1;
    }
    else nm = -1;
  }
  if (nm >= 0 && nm <= - (thecurr->Value()) ) {
    Standard_Integer i , numpos = 0 , nb = thelist->Length();
    for (i = nb; i > 0; i --)
      if (thenums->Value(i) == nm) {  numpos = i; break;  }
    if (numpos > 0 && nm >= 0) {
      Handle(Interface_Check) lch = thelist->ChangeValue(numpos);
      lch->GetMessages (ach);
    }
    //  Cas normal : on ajoute en fin de liste
    else  {  thelist->Append(ach);  thenums->Append(nm);  }
  }
  //  Pas encore vu passe : inutile de chercher
  else  {  thelist->Append(ach);  thenums->Append(nm);  thecurr->CValue() = -nm;  }
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_CheckIterator::Check
       (const Standard_Integer num) const
{
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    if (num == thenums->Value(i)) return thelist->Value(i);
  }
  return nulcheck();
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_CheckIterator::Check
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num = -1;
  if (!themod.IsNull()) num = themod->Number(ent);
  if (num > 0) return Check(num);

  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    if (ent == thelist->Value(i)->Entity()) return thelist->Value(i);
  }
  return nulcheck();
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check)& Interface_CheckIterator::CCheck
       (const Standard_Integer num)
{
  Standard_Integer i, nb = thenums->Length();
  for (i = 1; i <= nb; i ++) {
    if (num == thenums->Value(i)) return thelist->ChangeValue(i);
  }
  Handle(Interface_Check) ach = new Interface_Check;
  thelist->Append(ach);  thenums->Append(num);
  return thelist->ChangeValue (thelist->Length());
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check)& Interface_CheckIterator::CCheck
       (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = -1;
  if (!themod.IsNull()) num = themod->Number(ent);
  if (num > 0) return CCheck(num);

  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    if (ent == thelist->Value(i)->Entity()) return thelist->ChangeValue(i);
  }

  Handle(Interface_Check) ach = new Interface_Check;
  thelist->Append(ach);  thenums->Append(num);
  return thelist->ChangeValue (thelist->Length());
}


//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean Interface_CheckIterator::IsEmpty
  (const Standard_Boolean failsonly) const
{
  if (thelist->IsEmpty()) return Standard_True;
  if (!failsonly) return Standard_False;
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    if (thelist->Value(i)->HasFailed()) return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Interface_CheckStatus Interface_CheckIterator::Status () const
{
  Interface_CheckStatus stat = Interface_CheckOK;
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    const Handle(Interface_Check) ach = thelist->Value(i);
    if (ach->HasFailed()) return Interface_CheckFail;
    if (ach->NbWarnings() > 0) stat = Interface_CheckWarning;
  }
  return stat;
}


//=======================================================================
//function : Complies
//purpose  :
//=======================================================================

Standard_Boolean Interface_CheckIterator::Complies
  (const Interface_CheckStatus stat) const
{
  Standard_Boolean res = (stat == Interface_CheckNoFail);
  Standard_Integer nb = thelist->Length();
  for (Standard_Integer i = 1; i <= nb; ++i)
  {
    const Handle(Interface_Check) ach = thelist->Value(i);
    Standard_Integer nbf = ach->NbFails(), nbw = ach->NbWarnings();
    switch (stat)
    {
      case Interface_CheckOK:
      {
        if (nbf + nbw > 0)
        {
          return Standard_False;
        }
        break;
      }
      case Interface_CheckWarning:
      {
        if (nbf > 0)
        {
          return Standard_False;
        }
        if (nbw > 0)
        {
          res  = Standard_True;
        }
        break;
      }
      case Interface_CheckFail:
      {
        if (nbf > 0)
        {
          return Standard_True;
        }
        break;
      }
      case Interface_CheckAny:
      {
        return Standard_True;
      }
      case Interface_CheckMessage:
      {
        if (nbf + nbw > 0)
        {
          return Standard_True;
        }
        break;
      }
      case Interface_CheckNoFail:
      {
        if (nbf > 0)
        {
          return Standard_False;
        }
        break;
      }
      default:
        break;
    }
  }
  return res;
}


//=======================================================================
//function : Extract
//purpose  : 
//=======================================================================

Interface_CheckIterator Interface_CheckIterator::Extract
  (const Interface_CheckStatus stat) const
{
  Interface_CheckIterator res;
  res.SetModel (themod);  res.SetName (thename.ToCString());
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    const Handle(Interface_Check) ach = thelist->Value(i);
    Standard_Integer nbf = ach->NbFails(), nbw = ach->NbWarnings();
    Standard_Boolean prend = Standard_False;
    switch (stat) {
    case Interface_CheckOK      : prend = (nbf + nbw == 0);       break;
    case Interface_CheckWarning : prend = (nbf == 0 && nbw > 0);  break;
    case Interface_CheckFail    : prend = (nbf >  0);             break;
    case Interface_CheckAny     : prend = Standard_True;          break;
    case Interface_CheckMessage : prend = (nbf + nbw >  0);       break;
    case Interface_CheckNoFail  : prend = (nbf == 0);             break;
    default : break;
    }
    if (prend) res.Add (ach,thenums->Value(i));
  }
  return res;
}


//=======================================================================
//function : Extract
//purpose  : 
//=======================================================================

Interface_CheckIterator Interface_CheckIterator::Extract
  (const Standard_CString mess,
   const Standard_Integer incl, const Interface_CheckStatus stat) const
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (mess);
  Interface_CheckIterator res;
  res.SetModel (themod);  res.SetName (thename.ToCString());
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    const Handle(Interface_Check) ach = thelist->Value(i);
    if (ach->Complies(str,incl,stat)) res.Add (ach,thenums->Value(i));
  }
  return res;
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean Interface_CheckIterator::Remove(const Standard_CString mess,
                                                 const Standard_Integer incl,
                                                 const Interface_CheckStatus stat)
{
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (mess);
  Standard_Boolean res = Standard_False;
  Standard_Integer i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    Handle(Interface_Check) ach = thelist->ChangeValue(i);
    if (ach->Remove (str,incl,stat)) res = Standard_True;
  }
  return res;
}


//=======================================================================
//function : Checkeds
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) Interface_CheckIterator::Checkeds
  (const Standard_Boolean failsonly, const Standard_Boolean global) const
{
  Handle(TColStd_HSequenceOfTransient) list;
  if (themod.IsNull()) return list;
  list = new TColStd_HSequenceOfTransient();
  Standard_Integer num, i, nb = thelist->Length();
  for (i = 1; i <= nb; i ++) {
    const Handle(Interface_Check) chk = thelist->Value(i);
    if (failsonly && !chk->HasFailed()) continue;
    if (chk->NbWarnings() == 0) continue;
    num = thenums->Value(i);
    if (num == 0 && global) list->Append (themod);
    else if (num > 0) list->Append (themod->Value(num));
  }
  return list;
}


//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Start () const
{
  thecurr->CValue() = 1;
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean Interface_CheckIterator::More () const
{
  if (thecurr->Value() < 0) thecurr->CValue() = 1;
  return (thecurr->Value() <= thelist->Length());
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Next () const
{
  if (thecurr->Value() < 0) thecurr->CValue() = 1;
  thecurr->CValue() ++;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_CheckIterator::Value () const 
{
  if (thecurr->Value() > thelist->Length()) throw Standard_NoSuchObject("Interface Check Iterator : Value");
  return thelist->Value(thecurr->Value());
}


//=======================================================================
//function : Number
//purpose  : 
//=======================================================================

Standard_Integer Interface_CheckIterator::Number () const 
{
  if (thecurr->Value() > thenums->Length()) throw Standard_NoSuchObject("Interface Check Iterator : Value");
  return thenums->Value(thecurr->Value());
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Print(const Handle(Message_Messenger)& S,
                                    const Standard_Boolean failsonly,
                                    const Standard_Integer final) const
{
  Print (S,themod,failsonly,final);
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Print(const Handle(Message_Messenger)& S,
                                    const Handle(Interface_InterfaceModel)& model,
                                    const Standard_Boolean failsonly,
                                    const Standard_Integer /*final*/) const
{
  Standard_Boolean titre = Standard_False;
  /*Standard_CString mesnum;
    Standard_CString mesnum0 = ":";
    Standard_CString mesnum1 = " (original):";
    Standard_CString mesnum2 = " (computed):";    */
  Standard_Integer i, nbch = 0, nb = thelist->Length();//,j; svv #2
  Standard_Boolean yamod = !model.IsNull();
  for (i = 1; i <= nb; i ++) {
    const Handle(Interface_Check) ach = thelist->Value(i);
    Standard_Integer nbw = 0, nbf = ach->NbFails();
    if (!failsonly)  nbw = ach->NbWarnings();
    if (nbf + nbw == 0) continue;
    Handle(Standard_Transient) ent = ach->Entity();
    Standard_Integer nm0 = thenums->Value(i);
    Standard_Boolean entnul = ent.IsNull();
    Standard_Integer num = nm0;
    if (nm0 <= 0 && !entnul && yamod) num = model->Number(ent);
    if (nm0 <= 0 && entnul) num = -1;    // Global
//  mesnum = mesnum0;
//    if (yamod) mesnum = (nm0 > 0 ? mesnum1 : mesnum2);

    if (!titre)        S<<" **  " << Name() << "  **"<<endl;
    titre = Standard_True;
    S<<"Check:"; if(nb > 9 && i < 10) S<<" "; if (nb > 99 && i < 100) S<<" ";
    S<<i;
    if      (num <  0) S<<" -- Global Check"<<endl;
    else if (num == 0) S<<" -- Entity n0 ??:";
    else {
      if (yamod) { S<<" -- Entity (n0:id) "; model->Print (ent,S); }
      else       S<<" -- Entity n0 "<<num;
//      S<<" -- Entity n0 "<<num<<mesnum;
//      if (yamod) model->PrintLabel(ent,S);
    }
    if      (num >= 0 &&  entnul) S<<" (unknown Type)"<<endl;
    else if (num >= 0 && !entnul) {
      if (yamod) S<<"   Type:"<<model->TypeName(ent)<<endl;
      else       S<<"   Type:"<<ent->DynamicType()->Name()<<endl;
    }

    nbch ++;
    ach->Print (S, (failsonly ? 1 : 3));
  }
//  if (nbch > 0)  S<<" ----  Checks : "<<nbch<<"  ----"<<endl;
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void Interface_CheckIterator::Destroy ()
{
  thecurr.Nullify();
}    // redevient standard
