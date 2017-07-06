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
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_Check,Standard_Transient)

// Un Check est cree au depart sans liste de message : celle ci est creee
// seulement si au moins une erreur doit y etre enregitree (Fail-Warning)
//=======================================================================
//function : Interface_Check
//purpose  : 
//=======================================================================
Interface_Check::Interface_Check()
{
}    // construit a vide


//=======================================================================
//function : Interface_Check
//purpose  : 
//=======================================================================

Interface_Check::Interface_Check(const Handle(Standard_Transient)& anentity)
{
  theent = anentity;
}

// ....      Ajout de message d Erreur vraie (Fail)


//=======================================================================
//function : SendFail
//purpose  : 
//=======================================================================

void Interface_Check::SendFail(const Message_Msg& amsg)
{
  AddFail (amsg);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Interface_Check::AddFail (const Handle(TCollection_HAsciiString)& mess)
{
  if (thefails.IsNull()) thefails = new TColStd_HSequenceOfHAsciiString();
  if (thefailo.IsNull()) thefailo = new TColStd_HSequenceOfHAsciiString();
  thefails->Append(mess);  thefailo->Append(mess);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Interface_Check::AddFail(const Handle(TCollection_HAsciiString)& mess,
                              const Handle(TCollection_HAsciiString)& orig)
{
  if (thefails.IsNull()) thefails = new TColStd_HSequenceOfHAsciiString();
  if (thefailo.IsNull()) thefailo = new TColStd_HSequenceOfHAsciiString();
  thefails->Append(mess);  thefailo->Append(orig);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Interface_Check::AddFail(const Standard_CString amess,
                              const Standard_CString orig)
{
  if (amess[0] == '\0') return;
  if (!orig || orig[0] == '\0') AddFail (new TCollection_HAsciiString(amess));
  else AddFail (new TCollection_HAsciiString(amess),
		new TCollection_HAsciiString(orig) );
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void Interface_Check::AddFail(const Message_Msg& amsg)
{
  if (amsg.IsEdited()) AddFail (new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())),
                                new TCollection_HAsciiString(TCollection_AsciiString(amsg.Original())));
  else AddFail (new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())));
}


//=======================================================================
//function : HasFailed
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Check::HasFailed () const
{
  return (!thefails.IsNull());
}


//=======================================================================
//function : NbFails
//purpose  : 
//=======================================================================

Standard_Integer Interface_Check::NbFails () const
{
  return (thefails.IsNull() ? 0 : thefails->Length());
}


//=======================================================================
//function : Fail
//purpose  : 
//=======================================================================

const Handle(TCollection_HAsciiString)& Interface_Check::Fail
       (const Standard_Integer num, const Standard_Boolean final) const
{
  if (thefails.IsNull()) throw Standard_OutOfRange();
  return (final ? thefails->Value(num) : thefailo->Value(num));
}


//=======================================================================
//function : CFail
//purpose  : 
//=======================================================================

Standard_CString Interface_Check::CFail(const Standard_Integer num,
                                        const Standard_Boolean final) const
{
  return Fail(num,final)->ToCString();
}


//=======================================================================
//function : Fails
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) Interface_Check::Fails
       (const Standard_Boolean final) const
{
  if (thefails.IsNull()) return new TColStd_HSequenceOfHAsciiString();
  return (final ? thefails : thefailo);
}

//  ....        Ajout de message de Warning


//=======================================================================
//function : SendWarning
//purpose  : 
//=======================================================================

void Interface_Check::SendWarning(const Message_Msg& amsg)
{
  AddWarning (amsg);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Interface_Check::AddWarning (const Handle(TCollection_HAsciiString)& mess)
{
  if (thewarns.IsNull()) thewarns = new TColStd_HSequenceOfHAsciiString();
  if (thewarno.IsNull()) thewarno = new TColStd_HSequenceOfHAsciiString();
  thewarns->Append(mess);  thewarno->Append(mess);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Interface_Check::AddWarning(const Handle(TCollection_HAsciiString)& mess,
                                 const Handle(TCollection_HAsciiString)& orig)
{
  if (thewarns.IsNull()) thewarns = new TColStd_HSequenceOfHAsciiString();
  if (thewarno.IsNull()) thewarno = new TColStd_HSequenceOfHAsciiString();
  thewarns->Append(mess);  thewarno->Append(orig);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Interface_Check::AddWarning(const Standard_CString amess,
                                 const Standard_CString orig)
{
  if (amess[0] == '\0') return;
  if (!orig || orig[0] == '\0') AddWarning (new TCollection_HAsciiString(amess));
  else AddWarning (new TCollection_HAsciiString(amess),
		   new TCollection_HAsciiString(orig) );
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void Interface_Check::AddWarning(const Message_Msg& amsg)
{
  if (amsg.IsEdited()) AddWarning (new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())),
                                   new TCollection_HAsciiString(TCollection_AsciiString(amsg.Original())));
  else AddWarning (new TCollection_HAsciiString(TCollection_AsciiString(amsg.Value())));
}


//=======================================================================
//function : HasWarnings
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Check::HasWarnings () const
{
  return (!thewarns.IsNull());
}


//=======================================================================
//function : NbWarnings
//purpose  : 
//=======================================================================

Standard_Integer Interface_Check::NbWarnings () const
{
  return (thewarns.IsNull() ? 0 : thewarns->Length());
}


//=======================================================================
//function : Warning
//purpose  : 
//=======================================================================

const Handle(TCollection_HAsciiString)& Interface_Check::Warning
       (const Standard_Integer num, const Standard_Boolean final) const
{
  if (thewarns.IsNull()) throw Standard_OutOfRange();
  return (final ? thewarns->Value(num) : thewarno->Value(num));
}


//=======================================================================
//function : CWarning
//purpose  : 
//=======================================================================

Standard_CString Interface_Check::CWarning(const Standard_Integer num,
                                           const Standard_Boolean final) const
{
  return Warning(num,final)->ToCString();
}


//=======================================================================
//function : Warnings
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) Interface_Check::Warnings
       (const Standard_Boolean final) const
{
  if (thewarns.IsNull()) return new TColStd_HSequenceOfHAsciiString();
  return (final ? thewarns : thewarno);
}


//  ....        Ajout de message d Info simple (not yet completed)


//=======================================================================
//function : SendMsg
//purpose  : 
//=======================================================================

void Interface_Check::SendMsg (const Message_Msg& amsg)
{
  Handle(TCollection_HAsciiString) mess =
    new TCollection_HAsciiString (amsg.Value());
  Handle(TCollection_HAsciiString) orig = mess;
  if (amsg.IsEdited()) orig = new TCollection_HAsciiString (amsg.Original());

  if (theinfos.IsNull()) theinfos = new TColStd_HSequenceOfHAsciiString();
  if (theinfoo.IsNull()) theinfoo = new TColStd_HSequenceOfHAsciiString();
  theinfos->Append(mess);  theinfoo->Append(mess);
}


//=======================================================================
//function : NbInfoMsgs
//purpose  : 
//=======================================================================

Standard_Integer Interface_Check::NbInfoMsgs () const
{
  return (theinfos.IsNull() ? 0 : theinfos->Length());
}


//=======================================================================
//function : InfoMsg
//purpose  : 
//=======================================================================

const Handle(TCollection_HAsciiString)& Interface_Check::InfoMsg
  (const Standard_Integer num, const Standard_Boolean final) const
{
  if (theinfos.IsNull()) throw Standard_OutOfRange();
  return (final ? theinfos->Value(num) : theinfoo->Value(num));
}


//=======================================================================
//function : CInfoMsg
//purpose  : 
//=======================================================================

Standard_CString Interface_Check::CInfoMsg
  (const Standard_Integer num, const Standard_Boolean final) const
{
  return InfoMsg(num,final)->ToCString();
}


//=======================================================================
//function : InfoMsgs
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) Interface_Check::InfoMsgs
  (const Standard_Boolean final) const
{
  if (theinfos.IsNull()) return new TColStd_HSequenceOfHAsciiString();
  return (final ? theinfos : theinfoo);
}


//    ....  Gestion generale


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Interface_CheckStatus Interface_Check::Status () const
{
  if (!thefails.IsNull()) return Interface_CheckFail;
  if (!thewarns.IsNull()) return Interface_CheckWarning;
  return Interface_CheckOK;
}


//=======================================================================
//function : Complies
//purpose  : 
//=======================================================================

Standard_Boolean  Interface_Check::Complies(const Interface_CheckStatus status) const
{
  Standard_Boolean now = thewarns.IsNull();
  Standard_Boolean nof = thefails.IsNull();
  switch (status) {
  case Interface_CheckOK      :  return ( nof &&  now);
  case Interface_CheckWarning :  return ( nof && !now);
  case Interface_CheckFail    :  return  !nof;
  case Interface_CheckAny     :  return Standard_True;
  case Interface_CheckMessage :  return (!nof || !now);
  case Interface_CheckNoFail  :  return   nof;
  default                     :  break;
  }
  return Standard_False;
}


//=======================================================================
//function : Complies
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Check::Complies(const Handle(TCollection_HAsciiString)& mess,
                                           const Standard_Integer incl,
                                           const Interface_CheckStatus status) const
{
  if (mess.IsNull()) return Standard_False;
  Standard_Integer lng = mess->Length();
  if (status == Interface_CheckWarning || status == Interface_CheckAny) {
    Standard_Integer i, nb = NbWarnings();
    for (i = nb; i > 0; i --) {
      Handle(TCollection_HAsciiString) ames = Warning(i);
      if (incl == 0)  {  if (mess->IsSameString (ames)) return Standard_True;  }
      if (incl <  0)  {  if (ames->Location(mess,1,ames->Length()) > 0) return Standard_True;  }
      if (incl <  0)  {  if (mess->Location(mess,1,lng) > 0) return Standard_True;  }
    }
  }
  if (status == Interface_CheckFail || status == Interface_CheckAny) {
    Standard_Integer i, nb = NbWarnings();
    for (i = nb; i > 0; i --) {
      Handle(TCollection_HAsciiString) ames = Warning(i);
      if (incl == 0)  {  if (mess->IsSameString (ames)) return Standard_True;  }
      if (incl <  0)  {  if (ames->Location(mess,1,ames->Length()) > 0) return Standard_True;  }
      if (incl <  0)  {  if (mess->Location(mess,1,lng) > 0) return Standard_True;  }
    }
  }
  return Standard_False;
}


//=======================================================================
//function : HasEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Check::HasEntity () const
{
  return (!theent.IsNull());
}


//=======================================================================
//function : Entity
//purpose  : 
//=======================================================================

const Handle(Standard_Transient)& Interface_Check::Entity () const
{
  return theent;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Interface_Check::Clear ()
{
  thefails.Nullify();  thefailo.Nullify();
  thewarns.Nullify();  thewarno.Nullify();
  theinfos.Nullify();  theinfoo.Nullify();
  theent.Nullify();
}


//=======================================================================
//function : ClearFails
//purpose  : 
//=======================================================================

void Interface_Check::ClearFails ()
{
  thefails.Nullify();
  thefailo.Nullify();
}


//=======================================================================
//function : ClearWarnings
//purpose  : 
//=======================================================================

void Interface_Check::ClearWarnings ()
{
  thewarns.Nullify();
  thewarno.Nullify();
}


//=======================================================================
//function : ClearInfoMsgs
//purpose  : 
//=======================================================================

void Interface_Check::ClearInfoMsgs ()
{
  theinfos.Nullify();
  theinfoo.Nullify();
}



//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean Interface_Check::Remove(const Handle(TCollection_HAsciiString)& mess,
                                         const Standard_Integer incl,
                                         const Interface_CheckStatus status)
{
  if (mess.IsNull()) return Standard_False;
  Standard_Boolean res = Standard_False;
  Standard_Integer lng = mess->Length();
  if (status == Interface_CheckWarning || status == Interface_CheckAny) {
    Standard_Integer i, nb = NbWarnings();
    for (i = nb; i > 0; i --) {
      Standard_Boolean rem = Standard_False;
      Handle(TCollection_HAsciiString) ames = Warning(i);
      if (incl == 0)  rem = mess->IsSameString (ames);
      if (incl <  0)  rem = (ames->Location(mess,1,ames->Length()) > 0);
      if (incl <  0)  rem = (mess->Location(mess,1,lng) > 0);
      if (rem)  {  thewarns->Remove(i);  thewarno->Remove(i);  res = rem;  }
    }
  }
  if (status == Interface_CheckFail || status == Interface_CheckAny) {
    Standard_Integer i, nb = NbWarnings();
    for (i = nb; i > 0; i --) {
      Standard_Boolean rem = Standard_False;
      Handle(TCollection_HAsciiString) ames = Warning(i);
      if (incl == 0)  rem = mess->IsSameString (ames);
      if (incl <  0)  rem = (ames->Location(mess,1,ames->Length()) > 0);
      if (incl <  0)  rem = (mess->Location(mess,1,lng) > 0);
      if (rem)  {  thefails->Remove(i);  thefailo->Remove(i);  res = rem;  }
    }
  }
  return res;
}


//=======================================================================
//function : Mend
//purpose  : 
//=======================================================================

Standard_Boolean  Interface_Check::Mend(const Standard_CString pref,
                                        const Standard_Integer num)
{
  Standard_Integer i, n1 = num, n2 = num;
  if (pref && pref[2] == '\0') {
    if (pref[0] == 'F' && pref[1] == 'M') return Mend ("Mended",num);
    if (pref[0] == 'C' && pref[1] == 'A') {  Clear();  return Standard_True;  }
    if (pref[0] == 'C' && pref[1] == 'W') {
      if (num == 0)  {  ClearWarnings();  return Standard_True;  }
      if (num < 0 || num > NbWarnings())  return Standard_False;
      thewarns->Remove(num);  thewarno->Remove(num);
      return Standard_True;
    }
    if (pref[0] == 'C' && pref[1] == 'F') {
      if (num == 0)  {  ClearFails();  return Standard_True;  }
      if (num < 0 || num > NbFails())  return Standard_False;
      thefails->Remove(num);  thefailo->Remove(num);
      return Standard_True;
    }
  }
  //  Cas courant
  if (num == 0)  {
    n1 = 1;  n2 = NbFails();
    for (i = n2; i >= n1; i --) Mend (pref,i);
    return Standard_True;
  }
  else if (num < 0 || num > NbFails()) return Standard_False;
  //  Un message
  Handle(TCollection_HAsciiString) strf = thefails->Value(num);
  Handle(TCollection_HAsciiString) stro = thefailo->Value(num);
  if (pref && pref[0] != '\0') {
    strf->Insert (1," : ");  strf->Insert (1,pref);
    if (stro != strf)  {     stro->Insert (1," : ");  stro->Insert (1,pref);  }
  }
  thefails->Remove(num);  thefailo->Remove(num);
  if (stro == strf)  AddFail (strf);
  else               AddFail (strf,stro);
  return Standard_True;
}


//=======================================================================
//function : SetEntity
//purpose  : 
//=======================================================================

void Interface_Check::SetEntity(const Handle(Standard_Transient)& anentity)
{
  theent = anentity;
}


//=======================================================================
//function : GetEntity
//purpose  : 
//=======================================================================

void Interface_Check::GetEntity(const Handle(Standard_Transient)& anentity)
{
  if (theent.IsNull()) SetEntity(anentity);
}

// .. GetMessages, reprend les messages en les cumulant aux siens propres
// .. GetAsWarning, reprend les messages en les cumulant et en les
//    considerant tous comme "Warning" . En outre, selon <failsonly>
//      failsonly True  : ne pas reprendre les Warnings originaux
//      failsonly False : les prendre aussi


//=======================================================================
//function : GetMessages
//purpose  : 
//=======================================================================

void Interface_Check::GetMessages (const Handle(Interface_Check)& other)
{
  Standard_Integer nb,i;
  if ( (nb = other->NbFails()) != 0) {
    if (thefails.IsNull()) thefails = new TColStd_HSequenceOfHAsciiString();
    if (thefailo.IsNull()) thefailo = new TColStd_HSequenceOfHAsciiString();
    for (i = 1; i <= nb; i ++) thefails->Append(other->Fail(i,Standard_True));
    for (i = 1; i <= nb; i ++) thefailo->Append(other->Fail(i,Standard_False));
  }
  if ( (nb = other->NbWarnings()) != 0) {
    if (thewarns.IsNull()) thewarns = new TColStd_HSequenceOfHAsciiString();
    if (thewarno.IsNull()) thewarno = new TColStd_HSequenceOfHAsciiString();
    for (i = 1; i <= nb; i ++) thewarns->Append(other->Warning(i,Standard_True));
    for (i = 1; i <= nb; i ++) thewarno->Append(other->Warning(i,Standard_False));
  }
  if ( (nb = other->NbInfoMsgs()) != 0) {
    if (theinfos.IsNull()) theinfos = new TColStd_HSequenceOfHAsciiString();
    if (theinfoo.IsNull()) theinfoo = new TColStd_HSequenceOfHAsciiString();
    for (i = 1; i <= nb; i ++) theinfos->Append(other->InfoMsg(i,Standard_True));
    for (i = 1; i <= nb; i ++) theinfoo->Append(other->InfoMsg(i,Standard_False));
  }
}


//=======================================================================
//function : GetAsWarning
//purpose  : 
//=======================================================================

void Interface_Check::GetAsWarning(const Handle(Interface_Check)& other,
                                   const Standard_Boolean failsonly)
{
  Standard_Integer nb,i;
  if ( (nb = other->NbFails()) != 0) {
    if (thewarns.IsNull()) thewarns = new TColStd_HSequenceOfHAsciiString();
    if (thewarno.IsNull()) thewarno = new TColStd_HSequenceOfHAsciiString();
    for (i = 1; i <= nb; i ++) thewarns->Append(other->Fail(i,Standard_True));
    for (i = 1; i <= nb; i ++) thewarno->Append(other->Fail(i,Standard_False));
  }
  if ( (nb = other->NbWarnings()) != 0 || !failsonly) {
    if (thewarns.IsNull()) thewarns = new TColStd_HSequenceOfHAsciiString();
    if (thewarno.IsNull()) thewarno = new TColStd_HSequenceOfHAsciiString();
    for (i = 1; i <= nb; i ++) thewarns->Append(other->Warning(i,Standard_True));
    for (i = 1; i <= nb; i ++) thewarno->Append(other->Warning(i,Standard_False));
  }
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void Interface_Check::Print(const Handle(Message_Messenger)& S, const Standard_Integer level,
                            const Standard_Integer final) const
{
  Standard_Integer j, nb = NbFails();

  if (level >= 1) {
    nb = NbFails();
//    if (nb > 0) S<<" -->   Fails    : "<<nb<<endl;
    for (j = 1; j <= nb; j ++) {
      if (final >= 0) 
	S<<CFail(j,Standard_True)<<"\n";
      else
	S<<CFail(j,Standard_False)<<"\n";
//      if (final == 0)
//	if (Fail(j,Standard_True) != Fail(j,Standard_False))
//	  S<<"[original:"<<CFail(j,Standard_False)<<"]\n";
    }
  }

  if (level >= 2) {
    nb = NbWarnings();
//    if (nb > 0) S<<" -->   Warnings : "<<nb<<endl;
    for (j = 1; j <= nb; j ++) {
      if (final >= 0)
	S<<CWarning(j,Standard_True)<<"\n";
      else
	S<<CWarning(j,Standard_False)<<"\n";
//      if (final == 0)
//	if (Warning(j,Standard_True) != Warning(j,Standard_False))
//	  S<<"[original:"<<CWarning(j,Standard_False)<<"]\n";
    }
  }

//   InfoMsg : regle causant (user message)
  if (level >= 0) {
    nb = NbInfoMsgs();
//    if (nb > 0) S<<" -->   Infos    : "<<nb<<endl;
    for (j = 1; j <= nb; j ++) {
      if (final >= 0) 
	S<<CInfoMsg(j,Standard_True)<<"\n";
      else
	S<<CInfoMsg(j,Standard_False)<<"\n";
//      if (final == 0)
//	if (InfoMsg(j,Standard_True) != InfoMsg(j,Standard_False))
//	  S<<"[original:"<<CInfoMsg(j,Standard_False)<<"]\n";
    }
  }

}


//=======================================================================
//function : Trace
//purpose  : 
//=======================================================================

void Interface_Check::Trace(const Standard_Integer level,
                            const Standard_Integer final) const
{
  Print (Message::DefaultMessenger(), level, final);
}
