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

// szv#4 S4163
// svv#1 11.01.00 : porting on DEC
// svv#2 21.02.00 : porting on SIL
// smh#14 17.03.2000 : FRA62479 Clearing of gtool.

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_GTool.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_SignType.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_InterfaceModel, Standard_Transient)

// An Interface Model is a closed set of interface Entities: each one
// is in a single model at a time; it has a number (Number) which allows to
// verify that an entity is indeed in a single model, to define very
// efficient Maps, to provide a numerical identifier
// It is able to be used in Graph processing
// STATICS : the TEMPLATES
static NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> atemp;

static const occ::handle<Standard_Type>& typerep()
{
  return STANDARD_TYPE(Interface_ReportEntity);
}

static const occ::handle<Interface_Check>& nulch()
{
  static occ::handle<Interface_Check> anulch = new Interface_Check;
  return anulch;
}

//=================================================================================================

Interface_InterfaceModel::Interface_InterfaceModel()
    : haschecksem(false),
      isdispatch(false)
{
  thecheckstx = new Interface_Check;
  thechecksem = new Interface_Check;
}

//=================================================================================================

void Interface_InterfaceModel::Destroy() // we do a minimum
{
  //   Less than Clear which, itself, is adapted to each standard
  ClearEntities();
  thecheckstx->Clear();
  thechecksem->Clear();
  thecategory.Nullify();
}

//=================================================================================================

void Interface_InterfaceModel::SetProtocol(const occ::handle<Interface_Protocol>& proto)
{
  thegtool = new Interface_GTool(proto);
}

//=================================================================================================

occ::handle<Interface_Protocol> Interface_InterfaceModel::Protocol() const
{
  occ::handle<Interface_Protocol> proto;
  if (!thegtool.IsNull())
    return thegtool->Protocol();
  return proto;
}

//=================================================================================================

void Interface_InterfaceModel::SetGTool(const occ::handle<Interface_GTool>& gtool)
{
  thegtool = gtool;
}

//=================================================================================================

occ::handle<Interface_GTool> Interface_InterfaceModel::GTool() const
{
  return thegtool;
}

//=================================================================================================

void Interface_InterfaceModel::Clear()
{
  ClearEntities();
  thecheckstx->Clear();
  thechecksem->Clear();
  ClearHeader();
  ClearLabels();
  thecategory.Nullify();
}

//=================================================================================================

bool& Interface_InterfaceModel::DispatchStatus()
{
  return isdispatch;
}

//=================================================================================================

void Interface_InterfaceModel::ClearEntities()
{
  thereports.Clear();
  therepch.Clear();
  haschecksem = false;

  if (!thegtool.IsNull())
  {
    // WhenDeleteCase is not applicable
    /*    occ::handle<Interface_GeneralModule> module;  int CN;
        int nb = NbEntities();
        for (int i = 1; i <= nb ; i ++) {
          occ::handle<Standard_Transient> anent = Value(i);
          if (thegtool->Select (anent,module,CN))
        module->WhenDeleteCase (CN,anent,isdispatch);
        }*/
    thegtool->ClearEntities(); // smh#14 FRA62479
  }
  isdispatch = false;
  theentities.Clear();
}

//  ....                ENTITY ACCESS                ....

//=================================================================================================

int Interface_InterfaceModel::NbEntities() const
{
  return theentities.Extent();
}

//=================================================================================================

bool Interface_InterfaceModel::Contains(const occ::handle<Standard_Transient>& anentity) const
{
  if (theentities.Contains(anentity))
    return true;
  occ::handle<Interface_ReportEntity> rep = occ::down_cast<Interface_ReportEntity>(anentity);
  if (!rep.IsNull())
    return Contains(rep->Concerned());
  return false;
}

//=================================================================================================

int Interface_InterfaceModel::Number(const occ::handle<Standard_Transient>& anentity) const
{
  if (anentity.IsNull())
    return 0;
  int num = theentities.FindIndex(anentity);
  if (num > 0)
    return num;
  if (anentity->IsKind(typerep()))
  {
    occ::handle<Interface_ReportEntity> rep = occ::down_cast<Interface_ReportEntity>(anentity);
    if (!rep.IsNull())
      return Number(rep->Concerned());
  }
  return 0;
}

/*
int Interface_InterfaceModel::DENumber
                 (const occ::handle<Standard_Transient>& anentity) const
{
  if (anentity.IsNull()) return 0;
  int num = theentities.FindIndex(anentity);
  if (num > 0) return (2*num-1);
  if (anentity->IsKind(typerep())) {
    occ::handle<Interface_ReportEntity> rep =
      occ::down_cast<Interface_ReportEntity>(anentity);
    if (!rep.IsNull()) return (Number(rep->Concerned())*2-1);
  }
  return 0;
}
*/

//  ..                Special Access (Report, etc...)                ..

//=================================================================================================

const occ::handle<Standard_Transient>& Interface_InterfaceModel::Value(const int num) const
{
  return theentities.FindKey(num);
}

//=================================================================================================

int Interface_InterfaceModel::NbTypes(const occ::handle<Standard_Transient>& ent) const
{
  if (Protocol().IsNull())
    return 1;
  return Protocol()->NbTypes(ent);
}

//=================================================================================================

occ::handle<Standard_Type> Interface_InterfaceModel::Type(
  const occ::handle<Standard_Transient>& ent,
  const int                              nt) const
{
  if (Protocol().IsNull())
    return ent->DynamicType();
  return Protocol()->Type(ent, nt);
}

//=================================================================================================

const char* Interface_InterfaceModel::TypeName(const occ::handle<Standard_Transient>& ent,
                                               const bool                             complet) const
{
  if (!thegtool.IsNull())
    return thegtool->SignValue(ent, this);
  const char* tn = ent->DynamicType()->Name();
  if (complet)
    return tn;
  return Interface_InterfaceModel::ClassName(tn);
}

//=================================================================================================

const char* Interface_InterfaceModel::ClassName(const char* typnam)
{
  return Interface_SignType::ClassName(typnam);
}

//=================================================================================================

Interface_DataState Interface_InterfaceModel::EntityState(const int num) const
{
  occ::handle<Interface_ReportEntity> rep;
  if (!thereports.IsBound(num))
  {
    if (!therepch.IsBound(num))
      return Interface_StateOK;
    rep = occ::down_cast<Interface_ReportEntity>(therepch.Find(num));
    if (rep->IsError())
      return Interface_DataFail;
    return Interface_DataWarning;
  }
  rep = occ::down_cast<Interface_ReportEntity>(thereports.Find(num));
  if (rep.IsNull())
    return Interface_StateUnknown;
  if (rep->IsUnknown())
    return Interface_StateUnknown;
  if (rep->HasNewContent())
    return Interface_StateUnloaded;
  if (rep->IsError())
    return Interface_LoadFail;

  if (!therepch.IsBound(num))
    return Interface_LoadWarning;
  rep = occ::down_cast<Interface_ReportEntity>(therepch.Find(num));
  if (rep->IsError())
    return Interface_DataFail;
  return Interface_DataWarning;
}

//=================================================================================================

bool Interface_InterfaceModel::IsReportEntity(const int num, const bool semantic) const
{
  return (semantic ? therepch.IsBound(num) : thereports.IsBound(num));
}

//=================================================================================================

occ::handle<Interface_ReportEntity> Interface_InterfaceModel::ReportEntity(
  const int  num,
  const bool semantic) const
{
  occ::handle<Interface_ReportEntity> rep;
  if (!IsReportEntity(num, semantic))
    return rep;
  if (semantic)
    rep = occ::down_cast<Interface_ReportEntity>(therepch.Find(num));
  else
    rep = occ::down_cast<Interface_ReportEntity>(thereports.Find(num));
  return rep;
}

//=================================================================================================

bool Interface_InterfaceModel::IsErrorEntity(const int num) const
{
  occ::handle<Interface_ReportEntity> rep = ReportEntity(num);
  if (rep.IsNull())
    return false;
  return rep->IsError();
}

//=================================================================================================

bool Interface_InterfaceModel::IsRedefinedContent(const int num) const
{
  occ::handle<Interface_ReportEntity> rep = ReportEntity(num);
  if (rep.IsNull())
    return false;
  return rep->HasNewContent();
}

//=================================================================================================

bool Interface_InterfaceModel::ClearReportEntity(const int num)
{
  if (!thereports.IsBound(num))
    return false;
  thereports.UnBind(num);
  return true;
}

//=================================================================================================

bool Interface_InterfaceModel::SetReportEntity(const int                                  num,
                                               const occ::handle<Interface_ReportEntity>& rep)
{
  int                             nm = num;
  occ::handle<Standard_Transient> ent;
  if (num > 0)
  {
    ent = Value(nm);
    if (!(ent == rep->Concerned()))
      throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  }
  else if (num < 0)
  {
    nm  = -num;
    ent = Value(nm);
    if (!(ent == rep->Concerned()))
      throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  }
  else
  {
    ent = rep->Concerned();
    nm  = Number(ent);
    if (nm == 0)
      throw Interface_InterfaceMismatch("InterfaceModel : SetReportEntity");
  }
  if (!thereports.IsBound(nm))
  {
    int maxrep = thereports.NbBuckets();
    if (thereports.Extent() > maxrep - 10)
      thereports.ReSize(maxrep * 3 / 2);
  }
  if (nm <= 0)
    return false;
  return thereports.Bind(nm, rep);
}

//=================================================================================================

bool Interface_InterfaceModel::AddReportEntity(const occ::handle<Interface_ReportEntity>& rep,
                                               const bool                                 semantic)
{
  if (rep.IsNull())
    return false;
  occ::handle<Standard_Transient> ent = rep->Concerned();
  if (ent.IsNull())
    return false;
  int num = Number(ent);
  if (num == 0)
    return false;
  if (semantic)
    return thereports.Bind(num, rep);
  else
    return therepch.Bind(num, rep);
}

//=================================================================================================

bool Interface_InterfaceModel::IsUnknownEntity(const int num) const
{
  occ::handle<Interface_ReportEntity> rep = ReportEntity(num);
  if (rep.IsNull())
    return false;
  return rep->IsUnknown();
}

//  ....              Checks semantiques                ....  //

//=================================================================================================

void Interface_InterfaceModel::FillSemanticChecks(const Interface_CheckIterator& checks,
                                                  const bool                     clear)
{
  if (!checks.Model().IsNull())
  {
    occ::handle<Standard_Transient> t1 = checks.Model();
    occ::handle<Standard_Transient> t2 = this;
    if (t2 != t1)
      return;
  }
  if (clear)
  {
    therepch.Clear();
    thechecksem->Clear();
  }
  int nb = 0;
  for (checks.Start(); checks.More(); checks.Next())
    nb++;
  therepch.ReSize(therepch.Extent() + nb + 2);
  for (checks.Start(); checks.More(); checks.Next())
  {
    const occ::handle<Interface_Check>& ach = checks.Value();
    int                                 num = checks.Number();
    //    global check : ok if SAME MODEL
    if (num == 0)
      thechecksem->GetMessages(ach);
    else
    {
      occ::handle<Standard_Transient>     ent = Value(num);
      occ::handle<Interface_ReportEntity> rep = new Interface_ReportEntity(ach, ent);
      therepch.Bind(num, rep);
    }
  }
  haschecksem = true;
}

//=================================================================================================

bool Interface_InterfaceModel::HasSemanticChecks() const
{
  return haschecksem;
}

//=================================================================================================

const occ::handle<Interface_Check>& Interface_InterfaceModel::Check(const int  num,
                                                                    const bool syntactic) const
{
  if (num == 0)
  {
    if (syntactic)
      return thecheckstx;
    else
      return thechecksem;
  }
  if (!(syntactic ? thereports.IsBound(num) : therepch.IsBound(num)))
    return nulch();
  occ::handle<Standard_Transient> trep;
  if (syntactic)
    trep = thereports.Find(num);
  else
    trep = therepch.Find(num);
  occ::handle<Interface_ReportEntity> rep = occ::down_cast<Interface_ReportEntity>(trep);
  if (rep.IsNull())
    return nulch();
  return rep->Check();
}

//  ....              Loading of Model data                ....  //

//=================================================================================================

void Interface_InterfaceModel::Reservate(const int nbent)
{
  if (nbent > theentities.NbBuckets())
    theentities.ReSize(nbent);
  if (nbent < -thereports.NbBuckets())
    thereports.ReSize(-nbent);
}

//=================================================================================================

void Interface_InterfaceModel::AddEntity(const occ::handle<Standard_Transient>& anentity)
{
  // int newnum; svv #2
  if (!anentity->IsKind(typerep()))
    theentities.Add(anentity);
  //  Report : Add Concerned, but note presence Report and its value
  else
  {
    occ::handle<Interface_ReportEntity> rep = occ::down_cast<Interface_ReportEntity>(anentity);
    AddEntity(rep->Concerned());
    int maxrep = thereports.NbBuckets();
    if (thereports.Extent() > maxrep - 10)
      thereports.ReSize(maxrep * 3 / 2);
    thereports.Bind(Number(rep->Concerned()), rep);
  }
}

//  AddWithRefs iterates on referenced Entities to load an Entity
//  completely, with everything it needs

//=================================================================================================

void Interface_InterfaceModel::AddWithRefs(const occ::handle<Standard_Transient>& anent,
                                           const occ::handle<Interface_Protocol>& proto,
                                           const int                              level,
                                           const bool                             listall)
{
  if (anent.IsNull())
    return;
  if (theentities.FindIndex(anent) != 0)
  {
    if (!listall)
      return;
  }
  Interface_GeneralLib lib(proto);
  AddWithRefs(anent, lib, level, listall);
  if (Protocol().IsNull() && !proto.IsNull())
    SetProtocol(proto);
}

//=================================================================================================

void Interface_InterfaceModel::AddWithRefs(const occ::handle<Standard_Transient>& anent,
                                           const int                              level,
                                           const bool                             listall)
{
  occ::handle<Interface_Protocol> proto = Protocol();
  if (proto.IsNull())
    throw Interface_InterfaceMismatch("InterfaceModel : AddWithRefs");
  AddWithRefs(anent, proto, level, listall);
}

//=================================================================================================

void Interface_InterfaceModel::AddWithRefs(const occ::handle<Standard_Transient>& anent,
                                           const Interface_GeneralLib&            lib,
                                           const int                              level,
                                           const bool                             listall)
{
  if (anent.IsNull())
    return;
  if (theentities.FindIndex(anent) != 0)
  {
    if (!listall)
      return;
  }
  else
    AddEntity(anent);

  Interface_EntityIterator             iter;
  occ::handle<Interface_GeneralModule> module;
  int                                  CN;
  if (lib.Select(anent, module, CN))
  {
    module->FillSharedCase(CN, anent, iter);
    //    FillShared simply : would suppose that the model is already ready
    //    or precisely, we are in the process of building it ...
    module->ListImpliedCase(CN, anent, iter);
  }
  int lev1 = level - 1;
  if (lev1 == 0)
    return; // level = 0 -> all levels; otherwise still n-1
  for (iter.Start(); iter.More(); iter.Next())
    AddWithRefs(iter.Value(), lib, lev1, listall);
}

//=================================================================================================

void Interface_InterfaceModel::ReplaceEntity(const int                              nument,
                                             const occ::handle<Standard_Transient>& anent)
{
  theentities.Substitute(nument, anent);
}

//  ReverseOrders allows better control of Entity numbering :
//  Often, files put the roots at the end, while AddWithRefs
//  puts them at the head.

//=================================================================================================

void Interface_InterfaceModel::ReverseOrders(const int after)
{
  int nb = NbEntities(); // int num; svv #2
  if (nb < 2 || after >= nb)
    return;
  NCollection_Array1<occ::handle<Standard_Transient>> ents(1, nb);
  int                                                 i; // svv #1
  for (i = 1; i <= nb; i++)
    ents.SetValue(i, theentities.FindKey(i));
  //    We will empty the Map, then reload it : in order until after
  //        in reverse order after
  theentities.Clear();
  Reservate(nb);
  for (i = 1; i <= after; i++)
    theentities.Add(ents(i)); // svv #2
  for (i = nb; i > after; i--)
    theentities.Add(ents(i));
  //    Will also have to take care of the Reports
  for (i = nb; i > after; i--)
  {
    int                             i2 = nb + after - i;
    occ::handle<Standard_Transient> rep1, rep2;
    if (thereports.IsBound(i))
      rep1 = thereports.Find(i);
    if (thereports.IsBound(i2))
      rep2 = thereports.Find(i2);
    if (!rep1.IsNull())
      thereports.Bind(i2, rep1);
    else
      thereports.UnBind(i2);
    if (!rep2.IsNull())
      thereports.Bind(i, rep2);
    else
      thereports.UnBind(i);
  }
}

//=================================================================================================

void Interface_InterfaceModel::ChangeOrder(
  const int oldnum,
  const int newnum,
  const int cnt) // szv#4:S4163:12Mar99 `count` hid one from this
{
  int nb = NbEntities();
  int i; //, num; svv #2
  if (nb < 2 || newnum >= nb || cnt <= 0)
    return;
  NCollection_Array1<occ::handle<Standard_Transient>> ents(1, nb);
  //  We will prepare the change
  int minum = (oldnum > newnum ? newnum : oldnum);
  int mxnum = (oldnum < newnum ? newnum : oldnum);
  int kount = (oldnum > newnum ? cnt : -cnt);
  if (cnt <= 0 || cnt > mxnum - minum)
    throw Interface_InterfaceMismatch("InterfaceModel : ChangeOrder, Overlap");
  for (i = 1; i < minum; i++)
    ents.SetValue(i, theentities.FindKey(i));
  for (i = mxnum + cnt; i <= nb; i++)
    ents.SetValue(i, theentities.FindKey(i));
  for (i = minum; i < mxnum; i++)
    ents.SetValue(i + kount, theentities.FindKey(i));
  for (i = oldnum; i < oldnum + cnt; i++)
    ents.SetValue(i + (newnum - oldnum), theentities.FindKey(i));

  theentities.Clear();
  Reservate(nb);
  for (i = 1; i <= nb; i++)
    theentities.Add(ents(i)); // svv #2

  int difnum = mxnum - minum;
  for (i = minum; i < minum + cnt; i++)
  {
    occ::handle<Standard_Transient> rep1, rep2;
    if (thereports.IsBound(i))
      rep1 = thereports.Find(i);
    if (thereports.IsBound(i + difnum))
      rep1 = thereports.Find(i + difnum);
    if (!rep1.IsNull())
      thereports.Bind(i + difnum, rep1);
    else
      thereports.UnBind(i + difnum);
    if (!rep2.IsNull())
      thereports.Bind(i, rep2);
    else
      thereports.UnBind(i);
  }
}

//  GetFromTransfer allows to recover a result prepared elsewhere
//  The Model starts at zero. Entities must be free (cf AddEntity)

//=================================================================================================

void Interface_InterfaceModel::GetFromTransfer(const Interface_EntityIterator& aniter)
{
  theentities.Clear();
  theentities.ReSize(aniter.NbEntities());
  for (aniter.Start(); aniter.More(); aniter.Next())
  {
    const occ::handle<Standard_Transient>& ent = aniter.Value();
    AddEntity(ent);
  }
}

//  ....                       Interrogations                        ....  //

//=================================================================================================

bool Interface_InterfaceModel::SetCategoryNumber(const int num, const int val)
{
  int i, nb = NbEntities();
  if (num < 1 || num > nb)
    return false;
  if (thecategory.IsNull())
    thecategory = new TCollection_HAsciiString(nb, ' ');
  else if (thecategory->Length() < nb)
  {
    occ::handle<TCollection_HAsciiString> c = new TCollection_HAsciiString(nb, ' ');
    for (i = thecategory->Length(); i > 0; i--)
      c->SetValue(i, thecategory->Value(i));
    thecategory = c;
  }
  char cval = (char)(val + 32);
  thecategory->SetValue(num, cval);
  return true;
}

//=================================================================================================

int Interface_InterfaceModel::CategoryNumber(const int num) const
{
  if (thecategory.IsNull())
    return 0;
  if (num < 1 || num > thecategory->Length())
    return 0;
  int val = thecategory->Value(num);
  return val - 32;
}

//=================================================================================================

void Interface_InterfaceModel::FillIterator(Interface_EntityIterator& iter) const
{
  int nb = NbEntities();
  for (int i = 1; i <= nb; i++)
    iter.GetOneItem(theentities.FindKey(i));
}

//=================================================================================================

Interface_EntityIterator Interface_InterfaceModel::Entities() const
{
  Interface_EntityIterator iter;
  FillIterator(iter);
  return iter;
}

//=================================================================================================

Interface_EntityIterator Interface_InterfaceModel::Reports(const bool semantic) const
{
  Interface_EntityIterator iter;
  if (semantic)
  {
    NCollection_DataMap<int, occ::handle<Standard_Transient>>::Iterator itmap(therepch);
    for (; itmap.More(); itmap.Next())
      iter.AddItem(itmap.Value());
  }
  else
  {
    NCollection_DataMap<int, occ::handle<Standard_Transient>>::Iterator itmap(thereports);
    for (; itmap.More(); itmap.Next())
      iter.AddItem(itmap.Value());
  }
  return iter;
}

//=================================================================================================

Interface_EntityIterator Interface_InterfaceModel::Redefineds() const
{
  Interface_EntityIterator                                            iter;
  NCollection_DataMap<int, occ::handle<Standard_Transient>>::Iterator itmap(thereports);
  for (; itmap.More(); itmap.Next())
  {
    occ::handle<Interface_ReportEntity> rep = occ::down_cast<Interface_ReportEntity>(itmap.Value());
    if (rep.IsNull())
      continue;
    if (!rep->HasNewContent())
      continue;
    iter.AddItem(rep);
  }
  return iter;
}

// #include <limits.h>
// #include <TColStd_MapTransientHasher.hxx>

//=================================================================================================

const occ::handle<Interface_Check>& Interface_InterfaceModel::GlobalCheck(
  const bool syntactic) const
{
  if (syntactic)
    return thecheckstx;
  else
    return thechecksem;
}

//=================================================================================================

void Interface_InterfaceModel::SetGlobalCheck(const occ::handle<Interface_Check>& ach)
{
  thecheckstx = ach;
}

//=================================================================================================

void Interface_InterfaceModel::VerifyCheck(occ::handle<Interface_Check>& /*ach*/) const {}

//=================================================================================================

void Interface_InterfaceModel::Print(const occ::handle<Standard_Transient>& ent,
                                     Standard_OStream&                      S,
                                     const int                              mode) const
{
  if (ent.IsNull())
  {
    S << "NULL";
    return;
  }
  int num = Number(ent);
  if (mode <= 0)
    S << num;
  if (mode == 0)
    S << ":";
  if (mode >= 0)
  {
    if (num > 0)
      PrintToLog(ent, S);
    //      PrintLabel (ent,S);
    else
      S << "??";
  }
}

//=================================================================================================

void Interface_InterfaceModel::PrintToLog(const occ::handle<Standard_Transient>& ent,
                                          Standard_OStream&                      S) const
{
  PrintLabel(ent, S);
}

//  ....                       TEMPLATES                        ....  //

//=================================================================================================

int Interface_InterfaceModel::NextNumberForLabel(const char* label,
                                                 const int   fromnum,
                                                 const bool  exact) const
{
  int                                   n    = NbEntities();
  occ::handle<TCollection_HAsciiString> labs = new TCollection_HAsciiString(label);
  int                                   lnb  = labs->Length();
  labs->LowerCase();

  int i; // svv #1
  for (i = fromnum + 1; i <= n; i++)
  {
    occ::handle<TCollection_HAsciiString> lab = StringLabel(Value(i));
    if (lab.IsNull())
      continue;
    if (exact)
    {
      if (lab->IsSameString(labs, false))
        return i;
    }
    else
    {
      if (lab->Length() < lnb)
        continue;
      lab->LowerCase();
      if (lab->SearchFromEnd(labs) == lab->Length() - lnb + 1)
        return i;
    }
  }

  //   In "non exact", we admit to receive the number between 1 and n
  if (exact)
    return 0;
  i = 0;
  if (labs->IsIntegerValue())
    i = atoi(labs->ToCString());
  if (i <= 0 || i > n)
    i = 0;
  return i;
}

//=================================================================================================

bool Interface_InterfaceModel::HasTemplate(const char* name)
{
  return atemp.IsBound(name);
}

//=================================================================================================

occ::handle<Interface_InterfaceModel> Interface_InterfaceModel::Template(const char* name)
{
  occ::handle<Interface_InterfaceModel> model, newmod;
  if (!HasTemplate(name))
    return model;
  model  = occ::down_cast<Interface_InterfaceModel>(atemp.ChangeFind(name));
  newmod = model->NewEmptyModel();
  newmod->GetFromAnother(model);
  return newmod;
}

//=================================================================================================

bool Interface_InterfaceModel::SetTemplate(const char*                                  name,
                                           const occ::handle<Interface_InterfaceModel>& model)
{
  return atemp.Bind(name, model);
}

//=================================================================================================

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Interface_InterfaceModel::
  ListTemplates()
{
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list =
    new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  if (atemp.IsEmpty())
    return list;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(
    atemp);
  for (; iter.More(); iter.Next())
  {
    list->Append(new TCollection_HAsciiString(iter.Key()));
  }
  return list;
}
