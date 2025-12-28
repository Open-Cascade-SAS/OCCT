// Created on: 1997-10-20
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DNaming.hxx>

#include <DDF.hxx>

#include <TNaming_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TNaming.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_Map.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Selector.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>

#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <stdio.h>

//=================================================================================================

static void Display(const char* Name, const TopoDS_Shape& S)
{
  // char* name = Name;
  static int nbIsos  = 2;
  static double    size    = 100.;
  static int discret = 30;

  occ::handle<DBRep_DrawableShape> D =
    new DBRep_DrawableShape(S, Draw_jaune, Draw_vert, Draw_bleu, Draw_rouge, size, nbIsos, discret);
  Draw::Set(Name, D);
}

//=================================================================================================

// static void DumpNaming (const occ::handle<TNaming_Naming>& naming)
static void DumpNaming(const occ::handle<TNaming_Naming>& naming, Draw_Interpretor& di)
{
  TCollection_AsciiString Entry;
  const TNaming_Name&     AName = naming->GetName();
  // TNaming::Print(AName.Type(),std::cout);
  Standard_SStream aStream1;
  TNaming::Print(AName.Type(), aStream1);
  di << aStream1;
  di << " ";
  // TopAbs::Print(AName.ShapeType(),std::cout);
  Standard_SStream aStream2;
  TopAbs::Print(AName.ShapeType(), aStream2);
  di << aStream2;
  const NCollection_List<occ::handle<TNaming_NamedShape>>& NSS = AName.Arguments();
  for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(NSS); it.More(); it.Next())
  {
    TDF_Tool::Entry(it.Value()->Label(), Entry);
    di << " " << Entry.ToCString();
  }
  if (!AName.StopNamedShape().IsNull())
  {
    TDF_Tool::Entry(AName.StopNamedShape()->Label(), Entry);
    di << " Stop " << Entry.ToCString();
  }
}

//=======================================================================
// function : SelectShape ou SelectGeometry
// purpose  : "Select DF entry shape [context [orient]]",
//=======================================================================

static int DNaming_Select(Draw_Interpretor& di, int n, const char** a)
{
  if (n > 3)
  {
    bool geometry = !(strcmp(a[0], "SelectGeometry"));
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    DDF::AddLabel(DF, a[2], L);
    TNaming_Selector SL(L);
    if (n == 4)
    {
      TopoDS_Shape S = DBRep::Get(a[3], TopAbs_SHAPE);
      SL.Select(S, geometry);
    }
    if (n > 4)
    {
      bool Orient(false);
      if (n == 6)
        Orient = (Draw::Atoi(a[5]) != 0);
      TopoDS_Shape S = DBRep::Get(a[3], TopAbs_SHAPE);
      TopoDS_Shape C = DBRep::Get(a[4], TopAbs_SHAPE);
      SL.Select(S, C, geometry, Orient);
    }
    return 0;
  }
  di << "DNaming_Select : Error\n";
  return 1;
}

// #define DEB_SELN 1
//=================================================================================================

bool FillValidMap(const TDF_Label& theLabel, NCollection_Map<TDF_Label>& theValidMap)
{
  bool extRefFound = false;
  NCollection_Map<occ::handle<TDF_Attribute>> anExtMap;
#ifdef OCCT_DEBUG_SELN
  TCollection_AsciiString entr1;
  TDF_Tool::Entry(theLabel, entr1);
  std::cout << "\tNaming Attribute at = " << entr1 << std::endl;
#endif
  TDF_ChildIterator itr(theLabel, true);
  for (; itr.More(); itr.Next())
  {
    const TDF_Label&       aLabel = itr.Value();
    occ::handle<TNaming_Naming> aNaming;
    if (!aLabel.IsNull())
      aLabel.FindAttribute(TNaming_Naming::GetID(), aNaming);
    if (aNaming.IsNull())
      continue;
#ifdef OCCT_DEBUG_SELN
    TDF_Tool::Entry(aLabel, entr1);
    std::cout << "\tNaming Attribute at = " << entr1 << std::endl;
#endif
    TDF_Tool::OutReferences(aLabel, anExtMap);
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator attMItr(anExtMap); attMItr.More(); attMItr.Next())
    {
      const occ::handle<TDF_Attribute>& att = attMItr.Key();
#ifdef OCCT_DEBUG_SELN
      TDF_Tool::Entry(att->Label(), entr1);
      std::cout << "## References attribute dynamic type = " << att->DynamicType()
                << " at Label = " << entr1 << std::endl;
#endif
      if (att->Label().IsDifferent(aLabel) && !att->Label().IsDescendant(theLabel))
      {
        theValidMap.Add(att->Label());
        occ::handle<TNaming_NamedShape> aNS;
        att->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS);
        if (!aNS.IsNull())
          TNaming_NamingTool::BuildDescendants(aNS, theValidMap);
        extRefFound = true;
      }
    }
  }
  return extRefFound;
}

//=======================================================================
// function : SolveSelection
// purpose  : "SolveSelection DF entry",
//=======================================================================

static int DNaming_SolveSelection(Draw_Interpretor& di,
                                               int  n,
                                               const char**      a)
{
  if (n == 3)
  {
    char             name[100];
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    DDF::AddLabel(DF, a[2], L);

    occ::handle<TNaming_Naming> naming;
    if (!L.FindAttribute(TNaming_Naming::GetID(), naming))
    {
      std::cout << "DNaming__SolveSelection  : not a selection" << std::endl;
      return 1;
    }
    NCollection_Map<TDF_Label> aValidMap;
    if (!FillValidMap(L, aValidMap))
      di << "Valid map is empty\n";
#ifdef OCCT_DEBUG_SELN
    std::cout << "== Valid Label map ==" << std::endl;
    for (NCollection_Map<TDF_Label>::Iterator mapItr(aValidMap); mapItr.More(); mapItr.Next())
    {
      const TDF_Label& aLab = mapItr.Key();

      TCollection_AsciiString entr1;
      TDF_Tool::Entry(aLab, entr1);
      std::cout << "  Label = " << entr1 << std::endl;
    }
#endif

    TNaming_Selector SL(L);
    bool isSolved = SL.Solve(aValidMap);
    if (!isSolved)
      di << "!!! Solver is failed\n";
    TopoDS_Shape Res = TNaming_Tool::CurrentShape(SL.NamedShape());
    Sprintf(name, "%s_%s", "new", a[2]);
    Display(name, Res);
    return 0;
  }
  di << "DNaming_SolveSelection : Error\n";
  return 1;
}

//=======================================================================
// function : DumpSelection
// purpose  : DumpSelection DF entry (R)"
//=======================================================================
static int DNaming_DumpSelection(Draw_Interpretor& di,
                                              int  n,
                                              const char**      a)
{
  if (n == 3 || n == 4)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    if (!DDF::FindLabel(DF, a[2], L))
      return 1;
    occ::handle<TNaming_Naming> naming;
    if (!L.FindAttribute(TNaming_Naming::GetID(), naming))
    {
      di << "DNaming_DumpSelection : not a selection\n";
      return 1;
    }
    DumpNaming(naming, di);
    di << "\n";
    if (n == 4)
    {
      int        depth    = L.Depth();
      int        curdepth = 0;
      TCollection_AsciiString Entry;
      TDF_ChildIterator       it(naming->Label(), true);
      for (; it.More(); it.Next())
      {
        if (it.Value().FindAttribute(TNaming_Naming::GetID(), naming))
        {
          curdepth = (naming->Label().Depth() - depth);
          for (int i = 1; i <= curdepth; i++)
            di << " ";
          TDF_Tool::Entry(naming->Label(), Entry);
          di << Entry.ToCString() << " ";
          DumpNaming(naming, di);
          di << "\n";
        }
      }
    }
    return 0;
  }
  di << "DNaming_DumpSelection : Error\n";
  return 1;
}

//=======================================================================
// function : ArgsSelection
// purpose  : ArgsSelection DF entry"
//=======================================================================
static int DNaming_ArgsSelection(Draw_Interpretor& di,
                                              int  n,
                                              const char**      a)
{
  if (n == 3)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    if (!DDF::FindLabel(DF, a[2], L))
      return 1;
    occ::handle<TNaming_Naming> naming;
    if (!L.FindAttribute(TNaming_Naming::GetID(), naming))
    {
      di << "DNaming_DumpSelection : not a selection\n";
      return 1;
    }
    TCollection_AsciiString Entry;
    TNaming_Selector        SL(L);
    di << " Selection Arguments : ";
    NCollection_Map<occ::handle<TDF_Attribute>> args;
    SL.Arguments(args);
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator it(args); it.More(); it.Next())
    {
      TDF_Tool::Entry(it.Key()->Label(), Entry);
      di << Entry.ToCString() << " ";
    }
    di << "\n";
    return 0;
  }
  di << "DNaming_ArgsSelection : Error\n";
  return 1;
}

//=================================================================================================

static void CollectAttachment(const TDF_Label&              root,
                              const occ::handle<TNaming_Naming>& naming,
                              NCollection_Map<occ::handle<TNaming_NamedShape>>&      attachment)
{
  NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator itarg;
  const NCollection_List<occ::handle<TNaming_NamedShape>>&        args = naming->GetName().Arguments();
  for (itarg.Initialize(args); itarg.More(); itarg.Next())
  {
    if (!itarg.Value()->Label().IsDescendant(root))
      attachment.Add(itarg.Value());
  }
  occ::handle<TNaming_Naming> subnaming;
  for (TDF_ChildIterator it(naming->Label(), true); it.More(); it.Next())
  {
    if (it.Value().FindAttribute(TNaming_Naming::GetID(), subnaming))
    {
      const NCollection_List<occ::handle<TNaming_NamedShape>>& subargs = subnaming->GetName().Arguments();
      for (itarg.Initialize(subargs); itarg.More(); itarg.Next())
      {
        if (!itarg.Value()->Label().IsDescendant(root))
          attachment.Add(itarg.Value());
      }
    }
  }
}

//=======================================================================
// function : Attachment
// purpose  : Attachment DF entry"
//=======================================================================

static int DNaming_Attachment(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 3)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(a[1], DF))
      return 1;
    TDF_Label L;
    if (!DDF::FindLabel(DF, a[2], L))
      return 1;
    occ::handle<TNaming_Naming>  naming;
    NCollection_Map<occ::handle<TNaming_NamedShape>> attachment;
    if (L.FindAttribute(TNaming_Naming::GetID(), naming))
    {
      CollectAttachment(L, naming, attachment);
    }
    else
    {
      for (TDF_ChildIterator it(L, true); it.More(); it.Next())
      {
        if (it.Value().FindAttribute(TNaming_Naming::GetID(), naming))
        {
          CollectAttachment(L, naming, attachment);
          it.NextBrother();
        }
      }
    }
    TCollection_AsciiString Entry;
    TDF_Tool::Entry(L, Entry);
    di << " Attachment of " << Entry.ToCString();
    di << "\n";
    for (NCollection_Map<occ::handle<TNaming_NamedShape>>::Iterator ita(attachment); ita.More(); ita.Next())
    {
      TDF_Tool::Entry(ita.Key()->Label(), Entry);
      di << Entry.ToCString() << " ";
    }
    di << "\n";
    return 0;
  }
  di << "DNaming_Attachment : Error\n";
  return 1;
}

//=================================================================================================

void DNaming::SelectionCommands(Draw_Interpretor& theCommands)
{

  static bool done = false;
  if (done)
    return;
  done = true;

  const char* g = "Naming data commands";

  theCommands.Add("SelectShape",
                  "SelectShape DF entry shape [context [Orient]]",
                  __FILE__,
                  DNaming_Select,
                  g);

  theCommands.Add("SelectGeometry",
                  "SelectGeometry DF entry shape [context]",
                  __FILE__,
                  DNaming_Select,
                  g);

  theCommands.Add("DumpSelection", "DumpSelected DF entry", __FILE__, DNaming_DumpSelection, g);

  theCommands.Add("ArgsSelection", "ArgsSelection DF entry", __FILE__, DNaming_ArgsSelection, g);

  theCommands.Add("SolveSelection", "DumpSelection DF entry", __FILE__, DNaming_SolveSelection, g);

  theCommands.Add("Attachment", "Attachment DF entry", __FILE__, DNaming_Attachment, g);
}
