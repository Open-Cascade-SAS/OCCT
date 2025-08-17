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
#include <Interface_CheckFailure.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_GTool.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

#ifdef _WIN32
  #include <OSD_Exception.hxx>
#else
  #include <OSD_Signal.hxx>
#endif
#include <stdio.h>

static int errh = 1;

static void raisecheck(Standard_Failure& theException, Handle(Interface_Check)& ach)
{
  char mess[100];
  Sprintf(mess, "** Exception Raised during Check : %s **", theException.DynamicType()->Name());
  ach->AddFail(mess);
#ifdef _WIN32
  if (theException.IsKind(STANDARD_TYPE(OSD_Exception)))
  {
#else
  if (theException.IsKind(STANDARD_TYPE(OSD_Signal)))
  {
#endif
    theException.SetMessageString("System Signal received, check interrupt");
    throw theException;
  }
}

//  thestat : avoids CheckSuccess redoing a previously done calculation :
//  bit value 1 : Verify done, value 4 : and there are errors
//  bit value 2 : Analysis done, value 8 : and there are errors

//=================================================================================================

Interface_CheckTool::Interface_CheckTool(const Handle(Interface_InterfaceModel)& model,
                                         const Handle(Interface_Protocol)&       protocol)
    : thegtool(new Interface_GTool(protocol, model->NbEntities())),
      theshare(model, protocol)
{
  thestat = 0;
}

//=================================================================================================

Interface_CheckTool::Interface_CheckTool(const Handle(Interface_InterfaceModel)& model)
    : thegtool(model->GTool()),
      theshare(model, model->GTool())
{
  thestat = 0;
  thegtool->Reservate(model->NbEntities());
}

//=================================================================================================

Interface_CheckTool::Interface_CheckTool(const Interface_Graph& graph)
    : thegtool(graph.Model()->GTool()),
      theshare(graph)
{
}

//=================================================================================================

Interface_CheckTool::Interface_CheckTool(const Handle(Interface_HGraph)& hgraph)
    : thegtool(hgraph->Graph().Model()->GTool()),
      theshare(hgraph)
{
}

//=================================================================================================

void Interface_CheckTool::FillCheck(const Handle(Standard_Transient)& ent,
                                    const Interface_ShareTool&        sh,
                                    Handle(Interface_Check)&          ach)
{
  Handle(Interface_GeneralModule) module;
  Standard_Integer                CN;
  if (thegtool->Select(ent, module, CN))
  {
    //    Without try/catch (done by caller, avoids try/catch in loop)
    if (!errh)
    {
      module->CheckCase(CN, ent, sh, ach);
      return;
    }
    //    With try/catch
    try
    {
      OCC_CATCH_SIGNALS
      module->CheckCase(CN, ent, sh, ach);
    }
    catch (Standard_Failure& anException)
    {
      raisecheck(anException, ach);
    }
  }
  else
  {
    DeclareAndCast(Interface_ReportEntity, rep, ent);
    if (rep.IsNull())
      return;
    ach = rep->Check();
  }
  if (theshare.Graph().HasShareErrors(ent))
    ach->AddFail("** Shared Items unknown from the containing Model");
}

//=================================================================================================

void Interface_CheckTool::Print(const Handle(Interface_Check)& ach, Standard_OStream& S) const
{
  Standard_Integer i, nb;
  nb = ach->NbFails();
  if (nb > 0)
    S << " Fail Messages : " << nb << " :\n";
  for (i = 1; i <= nb; i++)
  {
    S << ach->Fail(i)->String() << "\n";
  }
  nb = ach->NbWarnings();
  if (nb > 0)
    S << " Warning Messages : " << nb << " :\n";
  for (i = 1; i <= nb; i++)
  {
    S << ach->Warning(i)->String() << "\n";
  }
}

//=================================================================================================

void Interface_CheckTool::Print(const Interface_CheckIterator& list, Standard_OStream& S) const
{
  Handle(Interface_InterfaceModel) model = theshare.Model();
  list.Print(S, model, Standard_False);
}

//  ....                General Check on a Model                ....

// Check: An Entity of a Model, designated by its rank

//=================================================================================================

Handle(Interface_Check) Interface_CheckTool::Check(const Standard_Integer num)
{
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Handle(Standard_Transient)       ent   = model->Value(num);
  Handle(Interface_Check) ach = new Interface_Check(ent); // not filtered by "Warning": as is
  errh                        = 1;
  FillCheck(ent, theshare, ach);
  return ach;
}

//  CheckSuccess: test passes-doesn't pass, on CheckList(Fail) of Entities

//=================================================================================================

void Interface_CheckTool::CheckSuccess(const Standard_Boolean reset)
{
  if (reset)
    thestat = 0;
  if (thestat > 3)
    throw Interface_CheckFailure // already tested with error
      ("Interface Model : Global Check");
  Handle(Interface_InterfaceModel) model = theshare.Model();
  if (model->GlobalCheck()->NbFails() > 0)
    throw Interface_CheckFailure("Interface Model : Global Check");
  Handle(Interface_Check) modchk = new Interface_Check;
  model->VerifyCheck(modchk);
  if (!model->Protocol().IsNull())
    model->Protocol()->GlobalCheck(theshare.Graph(), modchk);
  if (modchk->HasFailed())
    throw Interface_CheckFailure("Interface Model : Verify Check");
  if (thestat == 3)
    return; // everything tested and it passes

  errh                = 0; // No try/catch, because we precisely raise
  Standard_Integer nb = model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (model->IsErrorEntity(i))
      throw Interface_CheckFailure("Interface Model : an Entity is recorded as Erroneous");
    Handle(Standard_Transient) ent = model->Value(i);
    if (thestat & 1)
    {
      if (!model->IsErrorEntity(i))
        continue; // already verify, remains analyse
    }
    if (thestat & 2)
    {
      if (model->IsErrorEntity(i))
        continue; // already analyse, remains verify
    }

    Handle(Interface_Check) ach = new Interface_Check(ent);
    FillCheck(ent, theshare, ach);
    if (ach->HasFailed())
      throw Interface_CheckFailure("Interface Model : Check on an Entity has Failed");
  }
}

//  CompleteCheckList: All Tests: GlobalCheck, Analyse-Verify in Fail or in
//  Warning; plus the Unknown Entities (by empty Check)

//=================================================================================================

Interface_CheckIterator Interface_CheckTool::CompleteCheckList()
{
  thestat                                = 3;
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_CheckIterator          res;
  res.SetModel(model);
  Handle(Interface_Check) globch = model->GlobalCheck(); // GlobalCheck Statique
  if (!model->Protocol().IsNull())
    model->Protocol()->GlobalCheck(theshare.Graph(), globch);
  model->VerifyCheck(globch); // GlobalCheck Dynamique
  if (globch->HasFailed() || globch->HasWarnings())
    res.Add(globch, 0);
  if (globch->HasFailed())
    thestat |= 12;

  Standard_Integer i = 0, n0 = 1, nb = model->NbEntities();
  errh = 0;
  while (n0 <= nb)
  {
    Handle(Interface_Check)    ach = new Interface_Check;
    Handle(Standard_Transient) ent;
    try
    {
      OCC_CATCH_SIGNALS
      for (i = n0; i <= nb; i++)
      {
        ach->Clear();
        ent = model->Value(i);
        ach->SetEntity(ent);
        if (model->IsReportEntity(i))
        {
          ach = model->ReportEntity(i)->Check(); // INCLUT Unknown
          if (ach->HasFailed())                  // FAIL : no semantic Check
          {
            res.Add(ach, i);
            ach = new Interface_Check;
            thestat |= 12;
            continue;
          }
        }
        if (!model->HasSemanticChecks())
          FillCheck(ent, theshare, ach);
        else
          ach->GetMessages(model->Check(i, Standard_False));
        if (ach->HasFailed() || ach->HasWarnings())
        {
          res.Add(ach, i);
          ach = new Interface_Check;
          if (ach->HasFailed())
            thestat |= 12;
        }
      }
      n0 = nb + 1;
    }
    catch (Standard_Failure& anException)
    {
      n0 = i + 1;
      raisecheck(anException, ach);
      res.Add(ach, i);
      thestat |= 12;
    }
  }
  return res;
}

//  CheckList: Check Fail on Entities, in Analysis (Read time) or Verify

//=================================================================================================

Interface_CheckIterator Interface_CheckTool::CheckList()
{
  thestat                                = 3;
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_CheckIterator          res;
  res.SetModel(model);
  Standard_Integer        i = 0, n0 = 1, nb = model->NbEntities();
  Handle(Interface_Check) globch = model->GlobalCheck();
  if (!model->Protocol().IsNull())
    model->Protocol()->GlobalCheck(theshare.Graph(), globch);
  model->VerifyCheck(globch);
  if (globch->HasFailed())
  {
    thestat |= 12;
    res.Add(globch, 0);
  }

  errh = 0;
  while (n0 <= nb)
  {
    Handle(Interface_Check)    ach = new Interface_Check;
    Handle(Standard_Transient) ent;
    try
    {
      OCC_CATCH_SIGNALS
      for (i = n0; i <= nb; i++)
      {
        if (model->IsReportEntity(i))
        {
          ach = model->ReportEntity(i)->Check();
          if (ach->HasFailed())
          {
            thestat |= 12;
            res.Add(ach, i);
          }
        }
        else
        {
          ent = model->Value(i);
          ach->Clear();
          ach->SetEntity(ent);
          if (!model->HasSemanticChecks())
            FillCheck(ent, theshare, ach);
          else
            ach = model->Check(i, Standard_False);
          if (ach->HasFailed())
          {
            thestat |= 12;
            res.Add(ach, i);
          }
        }
      }
      n0 = nb + 1;
    }
    catch (Standard_Failure& anException)
    {
      n0 = i + 1;
      raisecheck(anException, ach);
      res.Add(ach, i);
      thestat |= 12;
    }
  }
  return res;
}

//  AnalyseCheckList: Fail during loading of Entities (Read time)

//=================================================================================================

Interface_CheckIterator Interface_CheckTool::AnalyseCheckList()
{
  thestat                                = 2;
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_CheckIterator          res;
  res.SetModel(model);
  Standard_Integer i = 0, n0 = 1, nb = model->NbEntities();

  errh = 0;
  while (n0 <= nb)
  {
    Handle(Interface_Check) ach = new Interface_Check;
    try
    {
      OCC_CATCH_SIGNALS
      for (i = n0; i <= nb; i++)
      {
        if (!model->IsReportEntity(i))
          continue;
        Handle(Interface_ReportEntity) rep = model->ReportEntity(i);
        ach                                = rep->Check();
        if (ach->HasFailed() || ach->HasWarnings())
        {
          thestat |= 8;
          res.Add(ach, i);
        }
      }
      n0 = nb + 1;
    }
    catch (Standard_Failure& anException)
    {
      n0 = i + 1;
      raisecheck(anException, ach);
      res.Add(ach, i);
      thestat |= 8;
    }
  }
  return res;
}

//  VerifyCheckList: Fail/Warning on Analysis (Entities loaded OK. Valid?)

//=================================================================================================

Interface_CheckIterator Interface_CheckTool::VerifyCheckList()
{
  thestat                                = 1;
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_CheckIterator          res;
  res.SetModel(model);
  Standard_Integer i = 0, n0 = 1, nb = model->NbEntities();

  errh = 0;
  while (n0 <= nb)
  {
    Handle(Standard_Transient) ent;
    Handle(Interface_Check)    ach = new Interface_Check;
    try
    {
      OCC_CATCH_SIGNALS
      for (i = n0; i <= nb; i++)
      {
        if (model->IsErrorEntity(i))
          continue;
        ent = model->Value(i);
        ach->Clear();
        ach->SetEntity(ent);
        if (!model->HasSemanticChecks())
          FillCheck(ent, theshare, ach);
        else
          ach = model->Check(i, Standard_False);
        if (ach->HasFailed() || ach->HasWarnings())
        {
          thestat |= 4;
          res.Add(ach, i);
        }
      }
      n0 = nb + 1;
    }
    catch (Standard_Failure& anException)
    {
      n0 = i + 1;
      raisecheck(anException, ach);
      res.Add(ach, i);
      thestat |= 4;
    }
  }
  return res;
}

//  Warnings on Entities (Read time or after)

//=================================================================================================

Interface_CheckIterator Interface_CheckTool::WarningCheckList()
{
  thestat                                = 3;
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_CheckIterator          res;
  res.SetModel(model);
  Standard_Integer i = 0, n0 = 1, nb = model->NbEntities();

  errh = 0;
  while (n0 <= nb)
  {
    Handle(Interface_Check)    ach = new Interface_Check;
    Handle(Standard_Transient) ent;
    try
    {
      OCC_CATCH_SIGNALS
      for (i = n0; i <= nb; i++)
      {
        ach->Clear();
        ach->SetEntity(ent);
        if (model->IsReportEntity(i))
        {
          Handle(Interface_ReportEntity) rep = model->ReportEntity(i);
          if (rep->IsError())
          {
            thestat |= 12;
            continue;
          }
          ach = rep->Check();
        }
        ent = model->Value(i);
        if (!model->HasSemanticChecks())
          FillCheck(ent, theshare, ach);
        else
          ach = model->Check(i, Standard_False);
        if (ach->HasFailed())
          thestat |= 12;
        else if (ach->HasWarnings())
          res.Add(ach, i);
      }
      n0 = nb + 1;
    }
    catch (Standard_Failure& anException)
    {
      n0 = i + 1;
      raisecheck(anException, ach);
      res.Add(ach, i);
      thestat |= 12;
    }
  }

  return res;
}

//=================================================================================================

Interface_EntityIterator Interface_CheckTool::UnknownEntities()
{
  Handle(Interface_InterfaceModel) model = theshare.Model();
  Interface_EntityIterator         res;
  Standard_Integer                 nb = model->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    if (model->IsUnknownEntity(i))
      res.GetOneItem(model->Value(i));
  }
  return res;
}
