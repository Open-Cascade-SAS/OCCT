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

#include <Interface_CopyControl.hxx>
#include <Interface_CopyMap.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

// Remember that a richer version of CopyTool exists: it is
// TransferDispatch (package Transfer). This class offers much more
// possibilities (parameterization of actions, Mapping management ...)
// But the principle (transfer in 2 passes) remains the same, namely:
// Pass 1 normal: the entities to transfer are designated, they involve
// their true sub-references
// Pass 2: once all transfers are done, the "Imply" relations are
// set, for the designated entities AND WHICH HAVE ALSO BEEN TRANSFERRED, the
// relation is continued (no new Share)
//  #####################################################################
//  ....                        CONSTRUCTORS                        ....
Interface_CopyTool::Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel,
                                       const Interface_GeneralLib&             lib)
    : thelib(lib),
      thelst(amodel->NbEntities())
{
  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap(amodel);
  therep = new Interface_CopyMap(amodel);
  thelev = 0;
  theimp = Standard_False;
}

Interface_CopyTool::Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel,
                                       const Handle(Interface_Protocol)&       protocol)
    : thelib(protocol),
      thelst(amodel->NbEntities())
{
  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap(amodel);
  therep = new Interface_CopyMap(amodel);
  thelev = 0;
  theimp = Standard_False;
}

Interface_CopyTool::Interface_CopyTool(const Handle(Interface_InterfaceModel)& amodel)
    : thelib(Interface_Protocol::Active()),
      thelst(amodel->NbEntities())
{
  if (Interface_Protocol::Active().IsNull())
    throw Interface_InterfaceError("Interface CopyTool : Create with Active Protocol undefined");

  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap(amodel);
  therep = new Interface_CopyMap(amodel);
  thelev = 0;
  theimp = Standard_False;
}

Handle(Interface_InterfaceModel) Interface_CopyTool::Model() const
{
  return themod;
}

void Interface_CopyTool::SetControl(const Handle(Interface_CopyControl)& othermap)
{
  themap = othermap;
}

Handle(Interface_CopyControl) Interface_CopyTool::Control() const
{
  return themap;
}

//  #####################################################################
//  ....                    Individual Actions                    ....

void Interface_CopyTool::Clear()
{
  themap->Clear();
  therep->Clear();
  thelev = 0;
  theimp = Standard_False;
  therts.Clear();
  ClearLastFlags();
}

Standard_Boolean Interface_CopyTool::NewVoid(const Handle(Standard_Transient)& entfrom,
                                             Handle(Standard_Transient)&       entto)
{
  if (entfrom == theent)
  {
    if (themdu.IsNull())
      return Standard_False;
    return themdu->NewVoid(theCN, entto);
  }
  theent               = entfrom;
  Standard_Boolean res = thelib.Select(entfrom, themdu, theCN);
  if (res)
    res = themdu->NewVoid(theCN, entto);
  if (!res)
    res = themdu->NewCopiedCase(theCN, entfrom, entto, *this);
  //  if (!res) entto = entfrom->ShallowCopy();   sorry, nothing more possible
  return res;
}

Standard_Boolean Interface_CopyTool::Copy(const Handle(Standard_Transient)& entfrom,
                                          Handle(Standard_Transient)&       entto,
                                          const Standard_Boolean            mapped,
                                          const Standard_Boolean            errstat)
{
  Standard_Boolean res = Standard_True;
  if (entfrom == theent)
  {
    if (themdu.IsNull())
      res = Standard_False;
  }
  else
  {
    theent = entfrom;
    res    = thelib.Select(entfrom, themdu, theCN);
  }
  if (!res)
  {
    //  Built-in :
    if (entfrom.IsNull())
      return res;
    if (entfrom->DynamicType() == STANDARD_TYPE(TCollection_HAsciiString))
    {
      entto = new TCollection_HAsciiString(
        Handle(TCollection_HAsciiString)::DownCast(entfrom)->ToCString());
      res = Standard_True;
    }
    return res;
  }
  //  Create the empty Entity (NewVoid), the Copy remains to be done
  res = NewVoid(entfrom, entto);
  if (mapped)
    themap->Bind(entfrom, entto); // Map before continuing ...

  //  Now, perform the Copy (depending on case; if ShallowCopy is not enough:
  //  it is <themdu> who decides)

  //    An Entity in Error is not copied (no sense and it's risky ...)
  //    However, it is "Copied Empty (NewVoid)" so referenceable
  if (!errstat)
    themdu->CopyCase(theCN, entfrom, entto, *this);
  return res;
}

void Interface_CopyTool::Implied(const Handle(Standard_Transient)& entfrom,
                                 const Handle(Standard_Transient)& entto)
{
  Handle(Interface_GeneralModule) module;
  Standard_Integer                CN;
  if (thelib.Select(entfrom, module, CN))
    module->RenewImpliedCase(CN, entfrom, entto, *this);
}

//  ....                Feeding the Map                ....

Handle(Standard_Transient) Interface_CopyTool::Transferred(const Handle(Standard_Transient)& ent)
{
  Handle(Standard_Transient) res;
  if (ent.IsNull())
    return res; // Copy of a Null : very simple ...
  Standard_Integer nument = themod->Number(ent);

  //  <nument> == 0 -> May be a non-shared sub-part ...
  //  We accept but we protect against a loop
  if (nument == 0 && thelev > 100)
    throw Interface_InterfaceError(
      "CopyTool : Transferred, Entity is not contained in Starting Model");
  if (!themap->Search(ent, res))
  { // already transferred ? if not, do it

    //  We perform the Copy (finally, we try)
    //  In case of failure, nothing is recorded
    if (!Copy(ent, res, (nument != 0), themod->IsRedefinedContent(nument)))
      return res;

    thelev++;
    if (nument != 0)
      thelst.SetTrue(nument);
    Handle(Interface_ReportEntity) rep;
    if (nument != 0)
      rep = themod->ReportEntity(nument);
    if (!rep.IsNull())
    {
      //  WARNING WARNING, if ReportEntity : Also copy Content and remake a
      //  ReportEntity with the initial terms
      if (rep->IsUnknown())
        therep->Bind(ent, new Interface_ReportEntity(res));
      else
      {
        Handle(Standard_Transient) contfrom, contto;
        contfrom                             = rep->Content();
        Handle(Interface_ReportEntity) repto = new Interface_ReportEntity(rep->Check(), res);
        if (!contfrom.IsNull())
        {
          if (contfrom == ent)
            contto = res;
          else
            Copy(contfrom, contto, themod->Contains(contfrom), Standard_False);
          repto->SetContent(contto);
        }
        therep->Bind(ent, repto);
      }
    }
    //    Manage the nesting level (0 = root of transfer)
    thelev--;
  }
  if (thelev == 0 && nument > 0)
    therts.Append(nument);
  return res;
}

void Interface_CopyTool::Bind(const Handle(Standard_Transient)& ent,
                              const Handle(Standard_Transient)& res)
{
  Standard_Integer num = themod->Number(ent);
  themap->Bind(ent, res);
  thelst.SetTrue(num);
}

Standard_Boolean Interface_CopyTool::Search(const Handle(Standard_Transient)& ent,
                                            Handle(Standard_Transient)&       res) const
{
  return themap->Search(ent, res);
}

//  ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##
//                              LastFlag

void Interface_CopyTool::ClearLastFlags()
{
  thelst.Init(Standard_False);
}

Standard_Integer Interface_CopyTool::LastCopiedAfter(const Standard_Integer      numfrom,
                                                     Handle(Standard_Transient)& ent,
                                                     Handle(Standard_Transient)& res) const
{
  Standard_Integer nb = thelst.Length();
  for (Standard_Integer num = numfrom + 1; num <= nb; num++)
  {
    if (thelst.Value(num))
    {
      ent = themod->Value(num);
      if (themap->Search(ent, res))
        return num;
    }
  }
  return 0;
}

//  #########################################################################
//  ....                        General Actions                        ....

void Interface_CopyTool::TransferEntity(const Handle(Standard_Transient)& ent)
{
  Handle(Standard_Transient) res = Transferred(ent);
}

void Interface_CopyTool::RenewImpliedRefs()
{
  if (theimp)
    return; // already done
  theimp = Standard_True;

  //  Transfer Pass 2 : recovery of non "Share" relations (but "Imply")
  //  i.e. concerning entities that may or may not have been transferred
  //  (And that the 1st pass did not copy but left as Null)
  //  N.B. : we should forbid commanding new transfers ...

  Standard_Integer nb = themod->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Standard_Transient) ent = themod->Value(i);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent, res))
      continue; // entity not transferred
                //    Renewal of "Imply" references.  Warning, do not copy if not loaded
    Handle(Standard_Transient) aRep;
    if (!therep->Search(ent, aRep))
    {
      Implied(ent, res);
    }
    else
    {
      Handle(Interface_ReportEntity) rep = Handle(Interface_ReportEntity)::DownCast(aRep);
      if (!rep.IsNull() && !rep->HasNewContent())
        Implied(ent, res);
    }
  }
}

void Interface_CopyTool::FillModel(const Handle(Interface_InterfaceModel)& bmodel)
{
  //  Preparatory work concerning the models
  //  We start : this involves the Header
  bmodel->Clear();
  bmodel->GetFromAnother(themod);

  //  Transfer Pass 1 : We take the Entities previously copied
  Interface_EntityIterator list = CompleteResult(Standard_True);
  bmodel->GetFromTransfer(list);

  //  Transfer Pass 2 : recovery of non "Share" relations (but "Imply")
  RenewImpliedRefs();
}

Interface_EntityIterator Interface_CopyTool::CompleteResult(
  const Standard_Boolean withreports) const
{
  Interface_EntityIterator iter;
  Standard_Integer         nb = themod->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Standard_Transient) ent = themod->Value(i);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent, res))
      continue;
    if (withreports)
    {
      Handle(Standard_Transient) rep;
      if (therep->Search(ent, rep))
        res = rep;
    }
    iter.GetOneItem(res);
  }
  return iter;
}

Interface_EntityIterator Interface_CopyTool::RootResult(const Standard_Boolean withreports) const
{
  Interface_EntityIterator iter;
  Standard_Integer         nb = therts.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Standard_Integer           j   = therts.Value(i);
    Handle(Standard_Transient) ent = themod->Value(j);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent, res))
      continue;
    if (withreports)
    {
      Handle(Standard_Transient) rep;
      if (therep->Search(ent, rep))
        res = rep;
    }
    iter.GetOneItem(res);
  }
  return iter;
}

//=================================================================================================

Interface_CopyTool::~Interface_CopyTool() {}
