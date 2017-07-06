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

//szv#4 S4163  
//svv#1 11.01.00 : porting on DEC
//svv#2 21.02.00 : porting on SIL
//smh#14 17.03.2000 : FRA62479 Clearing of gtool.

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_SignType.hxx>
#include <Message_Messenger.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerTransient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_InterfaceModel, Standard_Transient)

// Un Modele d`Interface est un ensemble ferme d`Entites d`interface : chacune
// est dans un seul modele a la fois; elle y a un numero (Number) qui permet de
// verifier qu`une entite est bien dans un seul modele, de definir des Map tres
// performantes, de fournir un identifieur numerique
// Il est a meme d`etre utilise dans des traitements de Graphe
// STATICS : les TEMPLATES
static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> atemp;

static const Handle(Standard_Type)& typerep()
{
  static  Handle(Standard_Type) tr = STANDARD_TYPE(Interface_ReportEntity);
  return tr;
}


static const Handle(Interface_Check)& nulch()
{
  static Handle(Interface_Check) anulch = new Interface_Check;
  return anulch;
}


//=======================================================================
//function : Interface_InterfaceModel
//purpose  : 
//=======================================================================

Interface_InterfaceModel::Interface_InterfaceModel ()
     : haschecksem (Standard_False), isdispatch (Standard_False)
{
  thecheckstx = new Interface_Check;
  thechecksem = new Interface_Check;
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::Destroy ()  // on fait un mimumum
{
//   Moins que Clear que, lui, est adapte a chaque norme
  ClearEntities();
  thecheckstx->Clear();
  thechecksem->Clear();
  thecategory.Nullify();
}


//=======================================================================
//function : SetProtocol
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::SetProtocol(const Handle(Interface_Protocol)& proto)
{
  thegtool = new Interface_GTool(proto);
}


//=======================================================================
//function : Protocol
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) Interface_InterfaceModel::Protocol () const
{
  Handle(Interface_Protocol) proto;
  if (!thegtool.IsNull()) return thegtool->Protocol();
  return proto;
}


//=======================================================================
//function : SetGTool
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::SetGTool(const Handle(Interface_GTool)& gtool)
{
  thegtool = gtool;
}


//=======================================================================
//function : GTool
//purpose  : 
//=======================================================================

Handle(Interface_GTool) Interface_InterfaceModel::GTool () const
{
  return thegtool;
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::Clear ()
{
  ClearEntities();
  thecheckstx->Clear();
  thechecksem->Clear();
  ClearHeader();
  ClearLabels();
  thecategory.Nullify();
}


//=======================================================================
//function : DispatchStatus
//purpose  : 
//=======================================================================

Standard_Boolean& Interface_InterfaceModel::DispatchStatus ()
{
  return isdispatch;
}


//=======================================================================
//function : ClearEntities
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::ClearEntities ()
{
  thereports.Clear();
  therepch.Clear();
  haschecksem = Standard_False;

  if (!thegtool.IsNull()) {
// WhenDeleteCase is not applicable    
/*    Handle(Interface_GeneralModule) module;  Standard_Integer CN;
    Standard_Integer nb = NbEntities();
    for (Standard_Integer i = 1; i <= nb ; i ++) {
      Handle(Standard_Transient) anent = Value(i);
      if (thegtool->Select (anent,module,CN))
	module->WhenDeleteCase (CN,anent,isdispatch);
    }*/
    thegtool->ClearEntities(); //smh#14 FRA62479
  }
  isdispatch = Standard_False;
  theentities.Clear();
}


//  ....                ACCES AUX ENTITES                ....


//=======================================================================
//function : NbEntities
//purpose  : 
//=======================================================================

Standard_Integer Interface_InterfaceModel::NbEntities () const
{
  return theentities.Extent();
}


//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::Contains
  (const Handle(Standard_Transient)& anentity) const
{
  if (theentities.Contains(anentity)) return Standard_True;
  Handle(Interface_ReportEntity) rep =
    Handle(Interface_ReportEntity)::DownCast(anentity);
  if (!rep.IsNull()) return Contains(rep->Concerned());
  return Standard_False;
}


//=======================================================================
//function : Number
//purpose  : 
//=======================================================================

Standard_Integer Interface_InterfaceModel::Number
  (const Handle(Standard_Transient)& anentity) const
{
  if (anentity.IsNull()) return 0;
  Standard_Integer num = theentities.FindIndex(anentity);
  if (num > 0) return num;
  if (anentity->IsKind(typerep())) {
    Handle(Interface_ReportEntity) rep =
      Handle(Interface_ReportEntity)::DownCast(anentity);
    if (!rep.IsNull()) return Number(rep->Concerned());
  }
  return 0;
}

/*
Standard_Integer Interface_InterfaceModel::DENumber
                 (const Handle(Standard_Transient)& anentity) const
{
  if (anentity.IsNull()) return 0;
  Standard_Integer num = theentities.FindIndex(anentity);
  if (num > 0) return (2*num-1);
  if (anentity->IsKind(typerep())) {
    Handle(Interface_ReportEntity) rep =
      Handle(Interface_ReportEntity)::DownCast(anentity);
    if (!rep.IsNull()) return (Number(rep->Concerned())*2-1);
  }
  return 0;
}
*/

//  ..                Acces Speciaux (Report, etc...)                ..


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(Standard_Transient)& Interface_InterfaceModel::Value
       (const Standard_Integer num) const
{
  return theentities.FindKey(num);
}


//=======================================================================
//function : NbTypes
//purpose  : 
//=======================================================================

Standard_Integer Interface_InterfaceModel::NbTypes
  (const Handle(Standard_Transient)& ent) const
{
  if (Protocol().IsNull()) return 1;
  return  Protocol()->NbTypes(ent);
}


//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

Handle(Standard_Type) Interface_InterfaceModel::Type
  (const Handle(Standard_Transient)& ent, const Standard_Integer nt) const
{
  if (Protocol().IsNull()) return ent->DynamicType();
  return  Protocol()->Type(ent,nt);
}


//=======================================================================
//function : TypeName
//purpose  : 
//=======================================================================

Standard_CString Interface_InterfaceModel::TypeName
  (const Handle(Standard_Transient)& ent, const Standard_Boolean complet) const
{
  if (!thegtool.IsNull()) return thegtool->SignValue (ent,this);
  Standard_CString tn = ent->DynamicType()->Name();
  if (complet) return tn;
  return Interface_InterfaceModel::ClassName(tn);
}


//=======================================================================
//function : ClassName
//purpose  : 
//=======================================================================

Standard_CString Interface_InterfaceModel::ClassName(const Standard_CString typnam)
{
  return Interface_SignType::ClassName (typnam);
}


//=======================================================================
//function : EntityState
//purpose  : 
//=======================================================================

Interface_DataState Interface_InterfaceModel::EntityState
  (const Standard_Integer num) const
{
  Handle(Interface_ReportEntity) rep;
  if (!thereports.IsBound(num)) {
    if (!therepch.IsBound(num)) return Interface_StateOK;
    rep = Handle(Interface_ReportEntity)::DownCast(therepch.Find(num));
    if (rep->IsError()) return Interface_DataFail;
    return Interface_DataWarning;
  }
  rep = Handle(Interface_ReportEntity)::DownCast(thereports.Find(num));
  if (rep.IsNull()) return Interface_StateUnknown;
  if (rep->IsUnknown()) return Interface_StateUnknown;
  if (rep->HasNewContent()) return Interface_StateUnloaded;
  if (rep->IsError()) return Interface_LoadFail;

  if (!therepch.IsBound(num)) return Interface_LoadWarning;
  rep = Handle(Interface_ReportEntity)::DownCast(therepch.Find(num));
  if (rep->IsError()) return Interface_DataFail;
  return Interface_DataWarning;
}


//=======================================================================
//function : IsReportEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::IsReportEntity
  (const Standard_Integer num, const Standard_Boolean semantic) const
{
  return (semantic ? therepch.IsBound(num) : thereports.IsBound(num));
}


//=======================================================================
//function : ReportEntity
//purpose  : 
//=======================================================================

Handle(Interface_ReportEntity) Interface_InterfaceModel::ReportEntity
       (const Standard_Integer num, const Standard_Boolean semantic) const
{
  Handle(Interface_ReportEntity) rep;
  if (!IsReportEntity(num,semantic)) return rep;
  if (semantic) rep = Handle(Interface_ReportEntity)::DownCast(therepch.Find(num));
  else rep = Handle(Interface_ReportEntity)::DownCast(thereports.Find(num));
  return rep;
}


//=======================================================================
//function : IsErrorEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::IsErrorEntity
  (const Standard_Integer num) const
{
  Handle(Interface_ReportEntity) rep = ReportEntity(num);
  if (rep.IsNull()) return Standard_False;
  return rep->IsError();
}


//=======================================================================
//function : IsRedefinedContent
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::IsRedefinedContent
  (const Standard_Integer num) const
{
  Handle(Interface_ReportEntity) rep = ReportEntity(num);
  if (rep.IsNull()) return Standard_False;
  return rep->HasNewContent();
}


//=======================================================================
//function : ClearReportEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::ClearReportEntity
  (const Standard_Integer num)
{
  if (!thereports.IsBound(num)) return Standard_False;
  thereports.UnBind (num);
  return Standard_True;
}


//=======================================================================
//function : SetReportEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::SetReportEntity
  (const Standard_Integer num, const Handle(Interface_ReportEntity)& rep)
{
  Standard_Integer nm = num;
  Handle(Standard_Transient) ent;
  if (num > 0) {
    ent = Value(nm);
    if (! (ent == rep->Concerned()) ) throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  } else if (num < 0) {
    nm = -num;
    ent = Value(nm);
    if (! (ent == rep->Concerned()) ) throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  } else {
    ent = rep->Concerned();
    nm = Number (ent);
    if (nm == 0)  throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  }
  if (!thereports.IsBound(nm)) {
    Standard_Integer maxrep = thereports.NbBuckets();
    if (thereports.Extent() > maxrep - 10) thereports.ReSize(maxrep*3/2);
  }
  if (nm <= 0) return Standard_False;
  return thereports.Bind (nm,rep);
}


//=======================================================================
//function : AddReportEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::AddReportEntity
  (const Handle(Interface_ReportEntity)& rep, const Standard_Boolean semantic)
{
  if (rep.IsNull()) return Standard_False;
  Handle(Standard_Transient) ent = rep->Concerned();
  if (ent.IsNull()) return Standard_False;
  Standard_Integer num = Number(ent);
  if (num == 0) return Standard_False;
  if (semantic) return thereports.Bind (num,rep);
  else          return therepch.Bind (num,rep);
}


//=======================================================================
//function : IsUnknownEntity
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::IsUnknownEntity
  (const Standard_Integer num) const
{
  Handle(Interface_ReportEntity) rep = ReportEntity(num);
  if (rep.IsNull()) return Standard_False;
  return rep->IsUnknown();
}


//  ....              Checks semantiques                ....  //


//=======================================================================
//function : FillSemanticChecks
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::FillSemanticChecks
  (const Interface_CheckIterator& checks, const Standard_Boolean clear)
{
  if (!checks.Model().IsNull()) {
    Handle(Standard_Transient) t1 = checks.Model();
    Handle(Standard_Transient) t2 = this;
    if (t2 != t1) return;
  }
  if (clear) {  therepch.Clear();  thechecksem->Clear();  }
  Standard_Integer nb = 0;
  for (checks.Start(); checks.More(); checks.Next())  nb ++;
  therepch.ReSize (therepch.Extent() + nb + 2);
  for (checks.Start(); checks.More(); checks.Next()) {
    const Handle(Interface_Check) ach = checks.Value();
    Standard_Integer num = checks.Number();
//    global check : ok si MEME MODELE
    if (num == 0) thechecksem->GetMessages(ach);
    else {
      Handle(Standard_Transient) ent = Value(num);
      Handle(Interface_ReportEntity) rep = new Interface_ReportEntity(ach,ent);
      therepch.Bind (num,rep);
    }
  }
  haschecksem = Standard_True;
}


//=======================================================================
//function : HasSemanticChecks
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::HasSemanticChecks () const
{
  return haschecksem;
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_InterfaceModel::Check
  (const Standard_Integer num, const Standard_Boolean syntactic) const
{
  if (num == 0) {
    if (syntactic) return thecheckstx;
    else return thechecksem;
  }
  if (! (syntactic ? thereports.IsBound(num) : therepch.IsBound(num)) )
    return nulch();
  Handle(Standard_Transient) trep;
  if (syntactic) trep = thereports.Find(num);
  else trep = therepch.Find(num);
  Handle(Interface_ReportEntity) rep = Handle(Interface_ReportEntity)::DownCast(trep);
  if (rep.IsNull()) return nulch();
  return rep->Check();
}


//  ....              Chargement des donnees du Modele                ....  //


//=======================================================================
//function : Reservate
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::Reservate (const Standard_Integer nbent)
{
  if (nbent > theentities.NbBuckets()) theentities.ReSize (nbent);
  if (nbent < -thereports.NbBuckets()) thereports.ReSize (-nbent);
}


//=======================================================================
//function : AddEntity
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::AddEntity(const Handle(Standard_Transient)& anentity)
{
  //Standard_Integer newnum; svv #2
  if (!anentity->IsKind(typerep())) theentities.Add(anentity);
//  Report : Ajouter Concerned, mais noter presence Report et sa valeur
  else {
    Handle(Interface_ReportEntity) rep =
      Handle(Interface_ReportEntity)::DownCast(anentity);
    AddEntity(rep->Concerned());
    Standard_Integer maxrep = thereports.NbBuckets();
    if (thereports.Extent() > maxrep - 10) thereports.ReSize(maxrep*3/2);
    thereports.Bind (Number(rep->Concerned()),rep);
  }
}


//  AddWithRefs itere sur les Entities referencees pour charger une Entite
//  au complet, avec tout ce dont elle a besoin


//=======================================================================
//function : AddWithRefs
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::AddWithRefs(const Handle(Standard_Transient)& anent,
                                           const Handle(Interface_Protocol)& proto,
                                           const Standard_Integer level,
                                           const Standard_Boolean listall)
{
  if (anent.IsNull()) return;
  if (theentities.FindIndex(anent) != 0) {
    if (!listall) return;
  }
  Interface_GeneralLib lib(proto);
  AddWithRefs (anent,lib,level,listall);
  if (Protocol().IsNull() && !proto.IsNull()) SetProtocol(proto);
}


//=======================================================================
//function : AddWithRefs
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::AddWithRefs(const Handle(Standard_Transient)& anent,
                                           const Standard_Integer level,
                                           const Standard_Boolean listall)
{
  Handle(Interface_Protocol) proto = Protocol();
  if (proto.IsNull()) throw Interface_InterfaceMismatch("InterfaceModel : AddWithRefs");
  AddWithRefs (anent,proto,level,listall);
}


//=======================================================================
//function : AddWithRefs
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::AddWithRefs(const Handle(Standard_Transient)& anent,
                                           const Interface_GeneralLib& lib,
                                           const Standard_Integer level,
                                           const Standard_Boolean listall)
{
  if (anent.IsNull()) return;
  if (theentities.FindIndex(anent) != 0) {
    if (!listall) return;
  }
  else AddEntity(anent);

  Interface_EntityIterator iter;
  Handle(Interface_GeneralModule) module;  Standard_Integer CN;
  if (lib.Select (anent,module,CN)) {
    module->FillSharedCase  (CN,anent,iter);
//    FillShared tout court : supposerait que le modele soit deja pret
//    or justement, on est en train de le construire ...
    module->ListImpliedCase (CN,anent,iter);
  }
  Standard_Integer lev1 = level-1;
  if (lev1 == 0) return;  // level = 0 -> tous niveaux; sinon encore n-1
  for (iter.Start(); iter.More(); iter.Next())
    AddWithRefs(iter.Value(),lib,lev1,listall);
}


//=======================================================================
//function : ReplaceEntity
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::ReplaceEntity(const Standard_Integer nument,
                                             const Handle(Standard_Transient)& anent)
{
  theentities.Substitute(nument,anent);
}

//  ReverseOrders permet de mieux controler la numeration des Entites :
//  Souvent, les fichiers mettent les racines en fin, tandis que AddWithRefs
//  les met en tete.


//=======================================================================
//function : ReverseOrders
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::ReverseOrders (const Standard_Integer after)
{
  Standard_Integer nb = NbEntities();  //Standard_Integer num; svv #2
  if (nb < 2 || after >= nb) return;
  TColStd_Array1OfTransient ents(1,nb);
  Standard_Integer i; // svv #1
  for (i = 1; i <= nb; i ++)
    ents.SetValue (i, theentities.FindKey(i));
//    On va vider la Map, puis la recharger : dans l ordre jusqua after
//        en ordre inverse apres
  theentities.Clear();
  Reservate (nb);
  for (i = 1;  i <= after; i ++) theentities.Add (ents(i));// svv #2
  for (i = nb; i >  after; i --) theentities.Add (ents(i));
//    Faudra aussi s occuper des Reports
  for (i = nb; i >  after; i --) {
    Standard_Integer i2 = nb+after-i;
    Handle(Standard_Transient) rep1,rep2;
    if (thereports.IsBound(i))  rep1 = thereports.Find(i);
    if (thereports.IsBound(i2)) rep2 = thereports.Find(i2);
    if (!rep1.IsNull()) thereports.Bind (i2,rep1);
    else                thereports.UnBind (i2);
    if (!rep2.IsNull()) thereports.Bind (i,rep2);
    else                thereports.UnBind (i);
  }
}


//=======================================================================
//function : ChangeOrder
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::ChangeOrder(const Standard_Integer oldnum,
                                           const Standard_Integer newnum,
                                           const Standard_Integer cnt) //szv#4:S4163:12Mar99 `count` hid one from this
{
  Standard_Integer nb = NbEntities();  Standard_Integer i; //, num; svv #2 
  if (nb < 2 || newnum >= nb || cnt<= 0) return;
  TColStd_Array1OfTransient ents(1,nb);
  //  On va preparer le changement
  Standard_Integer minum  = (oldnum > newnum ? newnum : oldnum);
  Standard_Integer mxnum  = (oldnum < newnum ? newnum : oldnum);
  Standard_Integer kount  = (oldnum > newnum ? cnt  : -cnt);
  if (cnt <= 0 || cnt > mxnum - minum) throw Interface_InterfaceMismatch("InterfaceModel : ChangeOrder, Overlap");
  for (i = 1; i < minum; i ++)  ents.SetValue (i,theentities.FindKey(i));
  for (i = mxnum+cnt; i <= nb; i ++) ents.SetValue (i,theentities.FindKey(i));
  for (i = minum; i < mxnum; i ++)
    ents.SetValue( i + kount, theentities.FindKey(i) );
  for (i = oldnum; i < oldnum+cnt; i ++)
    ents.SetValue( i + (newnum-oldnum), theentities.FindKey(i) );

  theentities.Clear();
  Reservate (nb);
  for (i = 1;  i <= nb; i ++)  theentities.Add (ents(i)); // svv #2

  Standard_Integer difnum = mxnum - minum;
  for (i = minum; i < minum+cnt; i ++) {
    Handle(Standard_Transient) rep1, rep2;
    if (thereports.IsBound(i)) rep1 = thereports.Find(i);
    if (thereports.IsBound(i+difnum)) rep1 = thereports.Find(i+difnum);
    if (!rep1.IsNull()) thereports.Bind (i+difnum,rep1);
    else                thereports.UnBind (i+difnum);
    if (!rep2.IsNull()) thereports.Bind (i,rep2);
    else                thereports.UnBind (i);
  }
}


//  GetFromTransfer permet de recuperer un resultat prepare par ailleurs
//  Le Modele demarre a zero. Les entites doivent etre libres (cf AddEntity)


//=======================================================================
//function : GetFromTransfer
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::GetFromTransfer
  (const Interface_EntityIterator& aniter)
{
  theentities.Clear();  theentities.ReSize (aniter.NbEntities());
  for (aniter.Start(); aniter.More(); aniter.Next()) {
    Handle(Standard_Transient) ent = aniter.Value();    AddEntity(ent);
  }
}


//  ....                       Interrogations                        ....  //


//=======================================================================
//function : SetCategoryNumber
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::SetCategoryNumber
  (const Standard_Integer num, const Standard_Integer val)
{
  Standard_Integer i,nb = NbEntities();
  if (num < 1 || num > nb) return Standard_False;
  if (thecategory.IsNull()) thecategory = new TCollection_HAsciiString(nb,' ');
  else if (thecategory->Length() < nb) {
    Handle(TCollection_HAsciiString) c =  new TCollection_HAsciiString(nb,' ');
    for (i = thecategory->Length(); i > 0; i --)
      c->SetValue(i,thecategory->Value(i));
    thecategory = c;
  }
  Standard_Character cval = (Standard_Character)(val + 32);
  thecategory->SetValue(num,cval);
  return Standard_True;
}


//=======================================================================
//function : CategoryNumber
//purpose  : 
//=======================================================================

Standard_Integer Interface_InterfaceModel::CategoryNumber
  (const Standard_Integer num) const
{
  if (thecategory.IsNull()) return 0;
  if (num < 1 || num > thecategory->Length()) return 0;
  Standard_Integer val = thecategory->Value(num);
  return val-32;
}


//=======================================================================
//function : FillIterator
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::FillIterator(Interface_EntityIterator& iter) const
{
  Standard_Integer nb = NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++)
    iter.GetOneItem (theentities.FindKey(i));
}


//=======================================================================
//function : Entities
//purpose  : 
//=======================================================================

Interface_EntityIterator Interface_InterfaceModel::Entities () const
{
  Interface_EntityIterator iter;
  FillIterator(iter);
  return iter;
}


//=======================================================================
//function : Reports
//purpose  : 
//=======================================================================

Interface_EntityIterator Interface_InterfaceModel::Reports
  (const Standard_Boolean semantic) const
{
  Interface_EntityIterator iter;
  if (semantic) {
    TColStd_DataMapIteratorOfDataMapOfIntegerTransient itmap (therepch);
    for (; itmap.More(); itmap.Next()) iter.AddItem (itmap.Value());
  } else {
    TColStd_DataMapIteratorOfDataMapOfIntegerTransient itmap (thereports);
    for (; itmap.More(); itmap.Next()) iter.AddItem (itmap.Value());
  }
  return iter;
}


//=======================================================================
//function : Redefineds
//purpose  : 
//=======================================================================

Interface_EntityIterator Interface_InterfaceModel::Redefineds () const
{
  Interface_EntityIterator iter;
  TColStd_DataMapIteratorOfDataMapOfIntegerTransient itmap (thereports);
  for (; itmap.More(); itmap.Next()) {
    Handle(Interface_ReportEntity) rep =
      Handle(Interface_ReportEntity)::DownCast(itmap.Value());
    if (rep.IsNull()) continue;
    if (!rep->HasNewContent()) continue;
    iter.AddItem (rep);
  }
  return iter;
}

//#include <limits.h>
//#include <TColStd_MapTransientHasher.hxx>


//=======================================================================
//function : GlobalCheck
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_InterfaceModel::GlobalCheck
  (const Standard_Boolean syntactic) const
{
  if (syntactic) return thecheckstx;
  else return thechecksem;
}


//=======================================================================
//function : SetGlobalCheck
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::SetGlobalCheck(const Handle(Interface_Check)& ach)
{
  thecheckstx = ach;
}


//=======================================================================
//function : VerifyCheck
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::VerifyCheck (Handle(Interface_Check)& /*ach*/) const
{
}


//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::Print(const Handle(Standard_Transient)& ent,
                                     const Handle(Message_Messenger)& S,
                                     const Standard_Integer mode) const
{ 
  if (ent.IsNull())  {  S<<"NULL";  return;  }
  Standard_Integer num = Number(ent);
  if (mode <= 0) S<<num;
  if (mode == 0) S<<":";
  if (mode >= 0) {
    if (num > 0) 
      PrintToLog(ent,S);
//      PrintLabel (ent,S);
    else S<<"??";
  }
}


//=======================================================================
//function : PrintToLog
//purpose  : 
//=======================================================================

void Interface_InterfaceModel::PrintToLog(const Handle(Standard_Transient)& ent,
                                          const Handle(Message_Messenger)& S) const
{
  PrintLabel (ent,S);
}


//  ....                       TEMPLATES                        ....  //


//=======================================================================
//function : NextNumberForLabel
//purpose  : 
//=======================================================================

Standard_Integer Interface_InterfaceModel::NextNumberForLabel
  (const Standard_CString label, const Standard_Integer fromnum,
   const Standard_Boolean exact) const
{
  Standard_Integer n = NbEntities();
  Handle(TCollection_HAsciiString) labs = new TCollection_HAsciiString(label);
  Standard_Integer lnb = labs->Length();
  labs->LowerCase();

  Standard_Integer i; // svv #1
  for (i = fromnum+1; i <= n; i ++) {
    Handle(TCollection_HAsciiString) lab = StringLabel (Value(i));
    if (lab.IsNull()) continue;
    if (exact) {
      if (lab->IsSameString(labs,Standard_False)) return i;
    } else {
      if (lab->Length() < lnb) continue;
      lab->LowerCase();
      if (lab->SearchFromEnd(labs) == lab->Length() - lnb + 1) return i;
    }
  }

//   En "non exact", on admet de recevoir le numero entre 1 et n
  if (exact) return 0;
  i = 0;
  if (labs->IsIntegerValue()) i = atoi (labs->ToCString());
  if (i <= 0 || i > n) i = 0;
  return i;
}


//=======================================================================
//function : HasTemplate
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::HasTemplate
  (const Standard_CString name)
{
  return atemp.IsBound(name);
}


//=======================================================================
//function : Template
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Interface_InterfaceModel::Template
       (const Standard_CString name)
{
  Handle(Interface_InterfaceModel) model,newmod;
  if (!HasTemplate(name)) return model;
  model = Handle(Interface_InterfaceModel)::DownCast(atemp.ChangeFind(name));
  newmod = model->NewEmptyModel();
  newmod->GetFromAnother (model);
  return newmod;
}


//=======================================================================
//function : SetTemplate
//purpose  : 
//=======================================================================

Standard_Boolean Interface_InterfaceModel::SetTemplate
  (const Standard_CString name, const Handle(Interface_InterfaceModel)& model)
{
  return atemp.Bind(name, model);
}


//=======================================================================
//function : ListTemplates
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfHAsciiString) Interface_InterfaceModel::ListTemplates ()
{
  Handle(TColStd_HSequenceOfHAsciiString) list = new
    TColStd_HSequenceOfHAsciiString();
  if (atemp.IsEmpty()) return list;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator iter(atemp);
  for (; iter.More(); iter.Next()) {
    list->Append (new TCollection_HAsciiString (iter.Key()) );
  }
  return list;
}
