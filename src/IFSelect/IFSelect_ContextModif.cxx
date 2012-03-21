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

#include <IFSelect_ContextModif.ixx>
#include <IFSelect_Selection.hxx>
#include <Message_Messenger.hxx>
#include <Message.hxx>
#include <Standard_NoSuchObject.hxx>


//=======================================================================
//function : IFSelect_ContextModif
//purpose  : 
//=======================================================================

IFSelect_ContextModif::IFSelect_ContextModif(const Interface_Graph& graph,
                                             const Interface_CopyTool& TC,
                                             const Standard_CString filename)
     : thegraf (graph,Standard_False) , thefile (filename) ,
       thelist (graph.Size(),' ')
{
  themap = TC.Control();  thesel  = Standard_False;  thecurr = thecurt = 0;
  Standard_Integer nb = thelist.Length();
  Handle(Standard_Transient) newent;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (themap->Search(graph.Entity(i),newent)) thelist.SetValue(i,'1');
  }
}


//=======================================================================
//function : IFSelect_ContextModif
//purpose  : 
//=======================================================================

IFSelect_ContextModif::IFSelect_ContextModif(const Interface_Graph& graph,
                                             const Standard_CString filename)
     : thegraf (graph,Standard_False) , thefile (filename) ,
       thelist (graph.Size(),' ')
{
  thesel  = Standard_False;  thecurr = thecurt = 0;
  Standard_Integer nb = thelist.Length();
  Handle(Standard_Transient) newent;
  for (Standard_Integer i = 1; i <= nb; i ++)  thelist.SetValue(i,'1');
}


//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::Select (Interface_EntityIterator& list)
{
  thesel = Standard_True;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) thelist.SetValue(i,' ');
  for (list.Start(); list.More(); list.Next()) {
    Handle(Standard_Transient) start,newent;
    start = list.Value();
    Standard_Integer num = thegraf.EntityNumber(start);
    if (num > nb || num < 0) num = 0;
    if (themap.IsNull() && num > 0)  thelist.SetValue(num,'1');
    else if (themap->Search (start,newent)) {
      if (num > 0) thelist.SetValue(num,'1');
    }
  }
}


//=======================================================================
//function : OriginalGraph
//purpose  : 
//=======================================================================

const Interface_Graph& IFSelect_ContextModif::OriginalGraph () const
{
  return thegraf;
}


//=======================================================================
//function : OriginalModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) IFSelect_ContextModif::OriginalModel() const
{
  return thegraf.Model();
}


//=======================================================================
//function : SetProtocol
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::SetProtocol(const Handle(Interface_Protocol)& prot)
{
  theprot = prot;
}


//=======================================================================
//function : Protocol
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) IFSelect_ContextModif::Protocol() const
{
  return theprot;
}


//=======================================================================
//function : HasFileName
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::HasFileName() const
{
  return (thefile.Length() > 0);
}


//=======================================================================
//function : FileName
//purpose  : 
//=======================================================================

Standard_CString IFSelect_ContextModif::FileName () const
{
  return thefile.ToCString();
}


//=======================================================================
//function : Control
//purpose  : 
//=======================================================================

Handle(Interface_CopyControl) IFSelect_ContextModif::Control () const
{
  return themap;
}


//=======================================================================
//function : IsForNone
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::IsForNone () const
{
  if (!thesel) return Standard_False;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thelist.Value(i) != ' ') return Standard_False;
  }
  return Standard_True;
}


//=======================================================================
//function : IsForAll
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::IsForAll  () const
{
  return (!thesel);
}


//=======================================================================
//function : IsTransferred
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::IsTransferred
  (const Handle(Standard_Transient)& ent) const
{
  if (themap.IsNull()) return Standard_True;
  Handle(Standard_Transient) newent;
  return themap->Search(ent,newent);
}


//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::IsSelected
  (const Handle(Standard_Transient)& ent) const
{
  //  Select a deja verifie "IsTransferred"
  Standard_Integer num = thegraf.EntityNumber(ent);
  if (num == 0) return Standard_False;
  return (thelist.Value(num) != ' ');
}


//=======================================================================
//function : SelectedOriginal
//purpose  : 
//=======================================================================

Interface_EntityIterator IFSelect_ContextModif::SelectedOriginal () const
{
  Interface_EntityIterator list;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thelist.Value(i) != ' ') list.GetOneItem (thegraf.Entity(i));
  }
  return list;
}


//=======================================================================
//function : SelectedResult
//purpose  : 
//=======================================================================

Interface_EntityIterator  IFSelect_ContextModif::SelectedResult () const
{
  Interface_EntityIterator list;
  Standard_Integer nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) newent;
    if (themap.IsNull()) newent = thegraf.Entity(i);
    else if (thelist.Value(i) != ' ') themap->Search (thegraf.Entity(i),newent);
    if (!newent.IsNull()) list.GetOneItem (newent);
  }
  return list;
}


//=======================================================================
//function : SelectedCount
//purpose  : 
//=======================================================================

Standard_Integer IFSelect_ContextModif::SelectedCount () const
{
  Standard_Integer nb = thelist.Length();
  Standard_Integer ns = 0;
  for (Standard_Integer i = 1; i <= nb; i ++)
    {  if (thelist.Value(i) != ' ') ns ++;  }
  return ns;
}


//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::Start ()
{
  thecurr = thecurt = 0;
  Next();
}


//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean IFSelect_ContextModif::More () const
{
  return (thecurr > 0);
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::Next()
{
  Standard_Integer nb = thelist.Length();
//  thecurr = thecurt;
//  if (thecurr <= 0 && thecurt >= 0) return;
  for (Standard_Integer i = thecurr+1; i <= nb; i ++) {
    if (thelist.Value(i) != ' ')  {  thecurr = i;  thecurt ++;  return;  }
  }
  thecurr = thecurt = 0;
}


//=======================================================================
//function : ValueOriginal
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_ContextModif::ValueOriginal () const
{
  if (thecurr <= 0) Standard_NoSuchObject::Raise("IFSelect_ContextModif");
  return thegraf.Entity(thecurr);
}


//=======================================================================
//function : ValueResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) IFSelect_ContextModif::ValueResult   () const
{
  if (thecurr <= 0) Standard_NoSuchObject::Raise("IFSelect_ContextModif");
  Handle(Standard_Transient) ent,newent;
  ent = thegraf.Entity(thecurr);
  if (themap.IsNull()) newent = ent;
  else themap->Search(ent,newent);
  return newent;
}


//=======================================================================
//function : TraceModifier
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::TraceModifier
  (const Handle(IFSelect_GeneralModifier)& modif)
{
  if (modif.IsNull()) return;

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout << "---   Run Modifier:" << endl;
  Handle(IFSelect_Selection) sel = modif->Selection();
  if (!sel.IsNull()) sout<<"      Selection:"<<sel->Label();
  else               sout<<"  (no Selection)";

//  on va simplement compter les entites
  Standard_Integer ne = 0, nb = thelist.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thelist.Value(i) != ' ') ne ++;
  }
  if (nb == ne) sout<<"  All Model ("<<nb<<" Entities)"<<endl;
  else          sout<<"  Entities,Total:"<<nb<<" Concerned:"<<ne<<endl;
}


//=======================================================================
//function : Trace
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::Trace (const Standard_CString mess)
{
//  Trace courante
  if (thecurr <= 0) return;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (ValueOriginal() == ValueResult())
    sout<<"--  ContextModif. Entity  n0 "<<thecurr<<endl;
  else
    sout<<"--  ContextModif. Entity in Original, n0 "<<thecurr<<" in Result, n0 "
      <<thecurt<<endl;
  if (mess[0] != '\0') sout<<"--  Message:"<<mess<<endl;
}


//=======================================================================
//function : AddCheck
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::AddCheck(const Handle(Interface_Check)& check)
{
  if (check->NbFails() + check->NbWarnings() == 0) return;
  const Handle(Standard_Transient)& ent = check->Entity();
  Standard_Integer num = thegraf.EntityNumber(ent);
  if (num == 0 && !ent.IsNull()) num = -1;  // force enregistrement
  thechek.Add(check,num);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::AddWarning(const Handle(Standard_Transient)& start,
                                       const Standard_CString mess,
                                       const Standard_CString orig)
{
  thechek.CCheck(thegraf.EntityNumber(start))->AddWarning(mess,orig);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void IFSelect_ContextModif::AddFail(const Handle(Standard_Transient)& start,
                                    const Standard_CString mess,
                                    const Standard_CString orig)
{
  thechek.CCheck(thegraf.EntityNumber(start))->AddFail(mess,orig);
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) IFSelect_ContextModif::CCheck(const Standard_Integer num)
{
  Handle(Interface_Check) ach = thechek.CCheck(num);
  if (num > 0 && num <= thegraf.Size()) ach->SetEntity(thegraf.Entity(num));
  return ach;
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check) IFSelect_ContextModif::CCheck
       (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = thegraf.EntityNumber(ent);
  if (num == 0) num = -1;    // force l enregistrement
  Handle(Interface_Check)& ach = thechek.CCheck(num);
  ach->SetEntity(ent);
  return ach;
}


//=======================================================================
//function : CheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator IFSelect_ContextModif::CheckList () const
{
  return thechek;
}
