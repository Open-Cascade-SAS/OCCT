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

#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Message.hxx>
#include <Standard_Transient.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_StepDumper.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

#include <stdio.h>

// Constructor: Initialize STEP dumper with model, protocol, and output mode
// mode > 0 sets label mode to 2 for enhanced entity labeling
StepData_StepDumper::StepData_StepDumper(const Handle(StepData_StepModel)& amodel,
                                         const Handle(StepData_Protocol)&  protocol,
                                         const Standard_Integer            mode)
    : theslib(protocol),
      thewlib(protocol),
      thewriter(amodel)
{
  themodel = amodel;
  if (mode > 0)
    thewriter.LabelMode() = 2;
}

StepData_StepWriter& StepData_StepDumper::StepWriter()
{
  return thewriter;
}

// Main dump method: outputs entity information to stream with different levels of detail
// level <= 0: basic entity type and identifier information only
// level == 1: entity identifiers and basic structure
// level > 1:  complete entity data with all referenced entities
Standard_Boolean StepData_StepDumper::Dump(Standard_OStream&                 S,
                                           const Handle(Standard_Transient)& ent,
                                           const Standard_Integer            level)
{
  Standard_Integer        i, nb = themodel->NbEntities();
  TColStd_Array1OfInteger ids(0, nb); // Array to store entity identifiers
  ids.Init(0);
  Standard_Integer num  = themodel->Number(ent);     // Entity number in model
  Standard_Integer nlab = themodel->IdentLabel(ent); // Entity identifier label
  ids.SetValue(num, (nlab > 0 ? nlab : -1));

  if (level <= 0)
  {
    // Basic output: show entity number and type information only
    Handle(StepData_ReadWriteModule) module;
    Standard_Integer                 CN;
    if (num > 0)
      S << "#" << num << " = ";
    else
      S << "#??? = "; // Unknown entity number
    if (thewlib.Select(ent, module, CN))
    {
      if (module->IsComplex(CN))
      {
        TColStd_SequenceOfAsciiString listypes;
        if (!module->ComplexType(CN, listypes))
          S << "(Complex Type : ask level > 0) cdl = " << ent->DynamicType()->Name() << " (...);"
            << std::endl;
        else
        {
          Standard_Integer n = listypes.Length();
          for (i = 1; i <= n; i++)
            S << listypes.Value(i) << " (...)";
          S << std::endl;
        }
      }
      else
        S << module->StepType(CN) << " (...);" << std::endl;
    }
    else
      S << "(Unrecognized Type for protocol) cdl = " << ent->DynamicType()->Name() << " (...);"
        << std::endl;
    if (nlab > 0)
      S << "/*   Ident in file for " << num << " : #" << nlab << "   */" << std::endl;
  }

  else if (level == 1)
  {
    // Collect entity identifiers
    Handle(Standard_Transient)      anent;
    Handle(Interface_GeneralModule) module;
    Standard_Integer                CN;
    if (theslib.Select(ent, module, CN))
    {
      Interface_EntityIterator iter;
      module->FillSharedCase(CN, ent, iter);
      module->ListImpliedCase(CN, ent, iter); // accumulate entities
      for (; iter.More(); iter.Next())
      {
        anent = iter.Value();
        nlab  = themodel->IdentLabel(anent);
        ids.SetValue(themodel->Number(anent), (nlab > 0 ? nlab : -1));
      }
    }
    // Send entity to writer
    thewriter.SendEntity(num, thewlib);
    ////    thewriter.Print(S);
  }
  else
  {
    Handle(Standard_Transient) anent;
    // Debug output: dumping entity number
    // Send entities to writer
    TColStd_Array1OfInteger tab(0, nb);
    tab.Init(0);
    tab.SetValue(num, 1);
    Handle(Interface_GeneralModule) module;
    Standard_Integer                CN;
    if (theslib.Select(ent, module, CN))
    {
      Interface_EntityIterator iter;
      module->FillSharedCase(CN, ent, iter);
      module->ListImpliedCase(CN, ent, iter); // accumulate entities
      for (; iter.More(); iter.Next())
      {
        tab.SetValue(themodel->Number(iter.Value()), 1);
      }
    }
    for (i = 1; i <= nb; i++)
    {
      // Process entity identifiers list
      if (tab.Value(i) == 0)
        continue;
      anent = themodel->Value(i);
      thewriter.SendEntity(i, thewlib);
      if (theslib.Select(anent, module, CN))
      {
        Interface_EntityIterator iter;
        module->FillSharedCase(CN, anent, iter);
        module->ListImpliedCase(CN, anent, iter); // accumulate entities
        for (; iter.More(); iter.Next())
        {
          anent = iter.Value();
          nlab  = themodel->IdentLabel(anent);
          ids.SetValue(themodel->Number(anent), (nlab > 0 ? nlab : -1));
        }
      }
    }
    ////    thewriter.Print(S);
  }

  // Display entity identifiers if present
  // Count different types of identifiers: distinct (nbi), matching entity number (nbq),
  // without identifier (nbu), total entities with identifiers (nbe)
  Standard_Integer nbi = 0, nbe = 0, nbq = 0, nbu = 0;
  for (i = 1; i <= nb; i++)
  {
    nlab = ids.Value(i);
    if (nlab == 0)
      continue; // Skip entities without identifiers
    nbe++;      // Count entities with identifiers
    if (nlab < 0)
      nbu = 0; // Entities without proper identifier
    else if (nlab == i)
      nbq = 0; // Entities where identifier matches entity number
    else if (nlab > 0)
      nbi++; // Entities with distinct proper identifiers
  }
  if (nbe > 0)
  {
    // Debug statistics: displayed entities, matching identifiers, distinct identifiers
    if (nbu > 0)
    {
      S << " (no ident): ";
      for (i = 1; i <= nb; i++)
      {
        if (ids.Value(i) >= 0)
          continue;
        S << " #" << i;
      }
      S << std::endl;
    }
    if (nbq > 0)
    {
      S << " (ident = num): ";
      for (i = 1; i <= nb; i++)
      {
        if (ids.Value(i) == i)
          S << " #" << i;
      }
      S << std::endl;
    }
    if (nbi < 0)
    { // Display help format instead of individual num:#id entries
      Standard_Integer nbl = 0, nbr = 0, nbr0 = 0, nbc = 0;
      char             unid[30];
      // Alternative format: "#num	     #ident"
      S << " (proper ident):  num:#ident  num:#ident  ..." << std::endl;
      for (i = 1; i <= nb; i++)
      {
        if (ids.Value(i) <= 0 || ids.Value(i) == i)
          continue;
        Sprintf(unid, "%d:#%d", i, ids.Value(i));
        nbc = (Standard_Integer)strlen(unid);
        nbr = ((80 - nbc) % 4) + 2;
        nbl += nbc;
        if (nbl + nbr0 > 79)
        {
          nbl = nbc;
          S << std::endl;
        }
        else
        {
          nbl += nbr0;
          for (; nbr0 > 0; nbr0--)
            S << " ";
        }
        S << unid;
        nbr0 = nbr;

        // Alternative formatting approaches for entity identifiers:
        // - Line wrapping at 79 characters
        // - Grouped output with spacing
        // - Tabular format with entity ranks and STEP identifiers
      }
      if (nbl > 0)
        S << std::endl;
    }
    if (nbi > 0)
      S << "In dump, iii:#jjj means : entity rank iii has step ident #jjj" << std::endl;
    // Debug output: entity dumping information with level details
  }
  if (level > 0)
  {
    thewriter.Print(S);
  }
  return Standard_True;
}

Standard_Boolean StepData_StepDumper::Dump(Standard_OStream&      S,
                                           const Standard_Integer num,
                                           const Standard_Integer level)
{
  if (num <= 0 || num > themodel->NbEntities())
    return Standard_False;
  Handle(Standard_Transient) ent = themodel->Value(num);
  return Dump(S, ent, level);
}
