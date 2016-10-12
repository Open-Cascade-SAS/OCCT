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

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Interface_DataMapIteratorOfDataMapOfTransientInteger.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <StepData_Described.hxx>
#include <StepData_ECDescr.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_UndefinedEntity.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(StepData_Protocol,Interface_Protocol)

//  Le Protocol de base reconnait UnknownEntity
//static TCollection_AsciiString  thename("(DEFAULT)");
static Standard_CString thename = "(DEFAULT)";

StepData_Protocol::StepData_Protocol ()
{
}

Standard_Integer  StepData_Protocol::NbResources () const
{
  return 0;
}


Handle(Interface_Protocol) StepData_Protocol::Resource
  (const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) nulproto;  
  return nulproto;
}


Standard_Integer  StepData_Protocol::CaseNumber
  (const Handle(Standard_Transient)& obj) const
{
  if (obj.IsNull()) return 0;
  Standard_Integer num = TypeNumber (obj->DynamicType());
  if (num > 0) return num;
  Handle(StepData_Described) dc = Handle(StepData_Described)::DownCast(obj);
  if (dc.IsNull()) return 0;
  return DescrNumber (dc->Description());
}


Standard_Integer  StepData_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(StepData_UndefinedEntity)) return 1;
  return 0;
}


Standard_CString StepData_Protocol::SchemaName () const
{
  return thename;
}


Handle(Interface_InterfaceModel)  StepData_Protocol::NewModel () const 
{
  return new StepData_StepModel;
}


Standard_Boolean  StepData_Protocol::IsSuitableModel
  (const Handle(Interface_InterfaceModel)& model) const 
{
  return model->IsKind(STANDARD_TYPE(StepData_StepModel));
}


Handle(Standard_Transient)  StepData_Protocol::UnknownEntity () const
{
  return new StepData_UndefinedEntity;
}


Standard_Boolean  StepData_Protocol::IsUnknownEntity
  (const Handle(Standard_Transient)& ent) const 
{
  if (!ent.IsNull())
    return ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity));
  return Standard_False;
}


//  ####    Description pour LateBinding

Standard_Integer  StepData_Protocol::DescrNumber
  (const Handle(StepData_EDescr)& adescr) const
{
  if (thedscnum.IsBound(adescr)) return thedscnum.Find(adescr);
  return 0;
}


void  StepData_Protocol::AddDescr
  (const Handle(StepData_EDescr)& adescr, const Standard_Integer CN)
{
  Handle(StepData_ESDescr) sd = Handle(StepData_ESDescr)::DownCast(adescr);
  thedscnum.Bind (adescr,CN);

//  Simple : memorisee selon son nom
//  sinon que faire ? on memorise selon le numero passe en alpha-num ...
//   (temporaire)
  if (!sd.IsNull()) thedscnam.Bind(sd->TypeName(),sd);
  char fonom[10];
  sprintf(fonom,"%d",CN);
  thedscnam.Bind(fonom,adescr);
}


Standard_Boolean  StepData_Protocol::HasDescr () const
{
  return !thedscnam.IsEmpty();
}


Handle(StepData_EDescr)  StepData_Protocol::Descr
  (const Standard_Integer num) const
{
  Handle(StepData_EDescr) dsc;
  if (thedscnam.IsEmpty()) return dsc;
  char fonom[10];
  sprintf(fonom,"%d",num);
  Handle(Standard_Transient) aTDsc;
  if (thedscnam.Find(fonom, aTDsc))
    dsc = Handle(StepData_EDescr)::DownCast(aTDsc);
  else
    dsc.Nullify();
  return dsc;
}


Handle(StepData_EDescr)  StepData_Protocol::Descr
  (const Standard_CString name, const Standard_Boolean anylevel) const
{
  Handle(StepData_EDescr) sd;
  if (!thedscnam.IsEmpty()) {
    Handle(Standard_Transient) aTSd;
    if (thedscnam.Find(name, aTSd))
      return Handle(StepData_EDescr)::DownCast(aTSd);
  }
  if (!anylevel) return sd;

  Standard_Integer i, nb = NbResources();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Protocol) sp = Handle(StepData_Protocol)::DownCast(Resource(i));
    if (sp.IsNull()) continue;
    sd = sp->Descr (name,anylevel);
    if (!sd.IsNull()) return sd;
  }
  return sd;
}


Handle(StepData_ESDescr)  StepData_Protocol::ESDescr
  (const Standard_CString name, const Standard_Boolean anylevel) const
{
  return Handle(StepData_ESDescr)::DownCast(Descr(name,anylevel));
}


Handle(StepData_ECDescr)  StepData_Protocol::ECDescr
  (const TColStd_SequenceOfAsciiString& names, const Standard_Boolean anylevel) const
{
  Standard_Integer i, nb = names.Length();
  Handle(StepData_ECDescr) cd;
  Interface_DataMapIteratorOfDataMapOfTransientInteger iter(thedscnum);
  for (; iter.More(); iter.Next()) {
    cd = Handle(StepData_ECDescr)::DownCast (iter.Key());
    if (cd.IsNull()) continue;
    if (cd->NbMembers() != nb) continue;
    Standard_Boolean ok = Standard_True;
    for (i = 1; i <= nb; i ++) {
      if (!names(i).IsEqual (cd->Member(i)->TypeName())) { ok = Standard_False; break; }
    }
    if (ok) return cd;
  }
  cd.Nullify();
  if (!anylevel) return cd;

  nb = NbResources();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Protocol) sp = Handle(StepData_Protocol)::DownCast(Resource(i));
    if (sp.IsNull()) continue;
    cd = sp->ECDescr (names,anylevel);
    if (!cd.IsNull()) return cd;
  }
  return cd;
}


void  StepData_Protocol::AddPDescr
  (const Handle(StepData_PDescr)& pdescr)
{
  thepdescr.Bind(pdescr->Name(),pdescr);
}


Handle(StepData_PDescr)  StepData_Protocol::PDescr
  (const Standard_CString name, const Standard_Boolean anylevel) const
{
  Handle(StepData_PDescr) sd;
  if (!thepdescr.IsEmpty()) {
    Handle(Standard_Transient) aTSd;
    if (thepdescr.Find(name, aTSd))
      return Handle(StepData_PDescr)::DownCast(aTSd);
  }
  if (!anylevel) return sd;

  Standard_Integer i, nb = NbResources();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Protocol) sp = Handle(StepData_Protocol)::DownCast(Resource(i));
    if (sp.IsNull()) continue;
    sd = sp->PDescr (name,anylevel);
    if (!sd.IsNull()) return sd;
  }
  return sd;
}


void  StepData_Protocol::AddBasicDescr
  (const Handle(StepData_ESDescr)& esdescr)
{
  thedscbas.Bind(esdescr->TypeName(),esdescr);
}


Handle(StepData_EDescr)  StepData_Protocol::BasicDescr
  (const Standard_CString name, const Standard_Boolean anylevel) const
{
  Handle(StepData_EDescr) sd;
  if (!thedscbas.IsEmpty()) {
    Handle(Standard_Transient) aTSd;
    if (thedscbas.Find(name, aTSd))
      return Handle(StepData_EDescr)::DownCast(aTSd);
  }
  if (!anylevel) return sd;

  Standard_Integer i, nb = NbResources();
  for (i = 1; i <= nb; i ++) {
    Handle(StepData_Protocol) sp = Handle(StepData_Protocol)::DownCast(Resource(i));
    if (sp.IsNull()) continue;
    sd = sp->BasicDescr (name,anylevel);
    if (!sd.IsNull()) return sd;
  }
  return sd;
}
