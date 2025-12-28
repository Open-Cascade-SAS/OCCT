// Created on: 1997-01-13
// Created by: VAUTHIER Jean-Claude
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

#include <QADNaming.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>

#include <DDF.hxx>

#include <TNaming.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_Map.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>

// #ifdef _MSC_VER
#include <cstdio>

// #endif

// POP : first Wrong Declaration : now it is correct
//       second not used
// extern void QADNaming_BuildMap(NCollection_Map<TDF_Label>& Updated, const TDF_Label& Lab);

//=================================================================================================

static int Ascendants(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;

  char name[100];

  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;

  if (!DDF::GetDF(a[1], ND))
    return 1;
  //  ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
    return 1;

  int T;

  if (n > 3)
    T = Draw::Atoi(a[3]);
  else
    T = ND->Transaction();

  // TNaming_OldShapeIterator it (S, T, US);
  TNaming_OldShapeIterator it(S, T, ND->Root());
  int                      i = 0;
  TCollection_AsciiString  entry;
  for (; it.More(); it.Next())
  {
    S = it.Shape();
    Sprintf(name, "%s_%s_%d", a[2], "old", i++);
    DBRep::Set(name, it.Shape());
    TDF_Label Label = it.Label();
    TDF_Tool::Entry(Label, entry);
    di << entry.ToCString() << "\n";
  }
  return 0;
}

//=================================================================================================

static int Descendants(Draw_Interpretor& di, int n, const char** a)

{
  if (n < 3)
    return 1;

  char                  name[100];
  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;
  if (!DDF::GetDF(a[1], ND))
    return 1;
  //  ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
    return 1;

  int T;

  if (n > 3)
    T = Draw::Atoi(a[3]);
  else
    T = ND->Transaction();

  TNaming_NewShapeIterator it(S, T, ND->Root());
  int                      i = 0;
  TCollection_AsciiString  entry;
  for (; it.More(); it.Next())
  {
    S = it.Shape();
    Sprintf(name, "%s_%s_%d", a[2], "new", i++);
    DBRep::Set(name, it.Shape());
    TDF_Label Label = it.Label();
    TDF_Tool::Entry(Label, entry);
    di << entry.ToCString() << "\n";
  }

  return 0;
}

//=================================================================================================

static int Getentry(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;
  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;

  if (!DDF::GetDF(a[1], ND))
    return 1;
  //  ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull())
  {
    di << "No shape selected\n";
    di << 0;
    return 0;
  }
  int                     aStatus = 0;
  TCollection_AsciiString Name    = QADNaming::GetEntry(S, ND, aStatus);
  if (aStatus == 0)
  {
    di << "E_NoName";
  }
  else
  {
    di << Name.ToCString();
    if (aStatus == 2)
    {
      di << "Several shapes have the same name\n";
    }
  }
  return 0;
}

//=======================================================================
// function : NamedShape
// purpose  : retrieve label of Primitive or a Generated shape
//=======================================================================

static int NamedShape(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;
  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;

  if (!DDF::GetDF(a[1], ND))
    return 1;
  TopoDS_Shape SS = DBRep::Get(a[2]);
  if (SS.IsNull())
  {
    di << "No shape selected\n";
    di << 0;
    return 0;
  }

  occ::handle<TNaming_NamedShape> NS = TNaming_Tool::NamedShape(SS, ND->Root());

  if (NS.IsNull())
  {
    di << "E_NoName";
    return 0;
  }
  TCollection_AsciiString Name;
  TDF_Tool::Entry(NS->Label(), Name);
  di << Name.ToCString();
  return 0;
}

//=================================================================================================

static int Currentshape(Draw_Interpretor&, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<TDF_Data> ND;
  if (!DDF::GetDF(a[1], ND))
    return 1;

  const char*  LabelName = a[2];
  TopoDS_Shape S         = QADNaming::CurrentShape(LabelName, ND);
  if (!S.IsNull())
  {
    if (n == 4)
      DBRep::Set(a[3], S);
    else
      DBRep::Set(a[2], S);
    return 0;
  }
  return 0;
}

//=================================================================================================

static int Initialshape(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 4)
    return 1;

  occ::handle<TDF_Data> ND;

  if (!DDF::GetDF(a[1], ND))
    return 1;

  TopoDS_Shape NS = DBRep::Get(a[2]);
  if (NS.IsNull())
    return 1;

  NCollection_List<TDF_Label> Labels;
  TopoDS_Shape                S = TNaming_Tool::InitialShape(NS, ND->Root(), Labels);
  if (!S.IsNull())
  {
    DBRep::Set(a[3], S);
  }
  NCollection_List<TDF_Label>::Iterator itL(Labels);

  TCollection_AsciiString entry;
  if (itL.More())
  {
    TDF_Tool::Entry(itL.Value(), entry);
    di << entry.ToCString();
    itL.Next();
  }
  for (; itL.More(); itL.Next())
  {
    TDF_Tool::Entry(itL.Value(), entry);
    di << " , " << entry.ToCString();
  }
  di << ".\n";
  return 0;
}

//=================================================================================================

static int Exploreshape(Draw_Interpretor& di, int n, const char** a)
{
  char name[100];

  if (n < 4)
    return 1;
  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;

  if (!DDF::GetDF(a[1], ND))
    return 1;
  //  ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  int Trans = ND->Transaction();
  if (n == 5)
  {
    Trans = (int)Draw::Atof(a[4]);
  }

  TDF_Label Lab;
  DDF::FindLabel(ND, a[2], Lab);
  occ::handle<TNaming_NamedShape> NS;
  if (!Lab.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    di << "No shape\n";
    return 0;
  }

  // TNaming::Print(NS->Evolution(),std::cout);
  Standard_SStream aSStream;
  TNaming::Print(NS->Evolution(), aSStream);
  di << aSStream;

  int NbShapes = 1;

  for (TNaming_Iterator itL(Lab, Trans); itL.More(); itL.Next())
  {
    if (!itL.OldShape().IsNull())
    {
      Sprintf(name, "%s%s_%d", "old", a[3], NbShapes);
      DBRep::Set(name, itL.OldShape());
    }
    if (!itL.NewShape().IsNull())
    {
      Sprintf(name, "%s_%d", a[3], NbShapes);
      DBRep::Set(name, itL.NewShape());
    }
    NbShapes++;
  }
  di << "\n";
  if (NbShapes == 0)
  {
    di << "No shape\n";
  }

  return 0;
}

//=======================================================================
// function : GeneratedShape
// purpose  : Generatedshape df shape Generationentry [drawname]
//=======================================================================

static int Generatedshape(Draw_Interpretor& di, int nb, const char** arg)
{
  TopoDS_Shape                    S;
  occ::handle<TNaming_NamedShape> A;
  if (nb >= 4)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    TopoDS_Shape                    Gen = DBRep::Get(arg[2]);
    occ::handle<TNaming_NamedShape> Generation;
    if (!DDF::Find(DF, arg[3], TNaming_NamedShape::GetID(), Generation))
      return 1;
    S = TNaming_Tool::GeneratedShape(Gen, Generation);
    if (!S.IsNull())
    {
      if (nb == 4)
        DBRep::Set(arg[4], S);
      else
        DBRep::Set(arg[3], S);
      return 0;
    }
  }
  di << "GetShape : Error\n";
  return 1;
}

//=================================================================================================

static int Getshape(Draw_Interpretor& di, int nb, const char** arg)
{
  TopoDS_Shape                    s;
  occ::handle<TNaming_NamedShape> A;
  if (nb >= 3)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    if (!DDF::Find(DF, arg[2], TNaming_NamedShape::GetID(), A))
      return 1;
    s = TNaming_Tool::GetShape(A);
    if (!s.IsNull())
    {
      if (nb == 4)
        DBRep::Set(arg[3], s);
      else
        DBRep::Set(arg[2], s);
      return 0;
    }
  }
  di << "DDataStd_GetShape : Error\n";
  return 1;
}

//=================================================================================================

static int Collect(Draw_Interpretor& di, int nb, const char** arg)
{
  NCollection_Map<occ::handle<TNaming_NamedShape>> MNS;
  occ::handle<TNaming_NamedShape>                  A;
  bool                                             OnlyModif = true;

  if (nb >= 3)
  {
    occ::handle<TDF_Data> DF;
    if (!DDF::GetDF(arg[1], DF))
      return 1;
    if (!DDF::Find(DF, arg[2], TNaming_NamedShape::GetID(), A))
      return 1;
    if (nb >= 4)
    {
      OnlyModif = Draw::Atoi(arg[3]) != 0;
    }
    TNaming_Tool::Collect(A, MNS, OnlyModif);
    for (NCollection_Map<occ::handle<TNaming_NamedShape>>::Iterator it(MNS); it.More(); it.Next())
    {
      TCollection_AsciiString Name;
      TDF_Tool::Entry(it.Key()->Label(), Name);
      di << Name.ToCString() << " ";
    }
  }
  return 1;
}

//=======================================================================
// function : GetCreationEntry
// purpose  : retrieve label of Primitive or a Generated shape
//=======================================================================

static int Getcreationentry(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 3)
    return 1;
  occ::handle<TDF_Data> ND;
  //  occ::handle<TNaming_UsedShapes> US;

  if (!DDF::GetDF(a[1], ND))
    return 1;
  //  ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  TopoDS_Shape SS = DBRep::Get(a[2]);

  if (SS.IsNull())
  {
    di << "No shape selected\n";
    di << 0;
    return 0;
  }

  NCollection_List<TDF_Label> Labels;
  TopoDS_Shape                S = TNaming_Tool::InitialShape(SS, ND->Root(), Labels);

  if (S.IsNull())
  {
    di << "E_NoName";
    return 0;
  }
  int                     aStatus = 0;
  TCollection_AsciiString Name    = QADNaming::GetEntry(S, ND, aStatus);
  if (aStatus == 0)
  {
    di << "E_NoName";
  }
  else
  {
    di << Name.ToCString();
    if (aStatus == 2)
    {
      di << "Several shapes have the same name\n";
    }
  }
  return 0;
}

//=================================================================================================

void QADNaming::BasicCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  const char* g = "Naming data commands";

  // Exploration
  theCommands.Add("Ascendants", "Ascendants df shape [trans]", __FILE__, Ascendants, g);

  theCommands.Add("Descendants", "Descendants  df shape [trans]", __FILE__, Descendants, g);

  theCommands.Add("ExploreShape", "ExploreShape df entry res [trans]", __FILE__, Exploreshape, g);

  theCommands.Add("GetEntry", "GetEntry df shape", __FILE__, Getentry, g);

  theCommands.Add("GetCreationEntry", "GetCreationEntry df shape", __FILE__, Getcreationentry, g);

  theCommands.Add("NamedShape", "NamedShape df shape", __FILE__, NamedShape, g);

  theCommands.Add("InitialShape", "InitialShape df shape res", __FILE__, Initialshape, g);

  theCommands.Add("CurrentShape", "Currentshape df entry [drawname]", __FILE__, Currentshape, g);

  theCommands.Add("GetShape", "GetShape df entry [drawname]", __FILE__, Getshape, g);

  theCommands.Add("Collect", "Collect  df entry [onlymodif 0/1]", __FILE__, Collect, g);

  theCommands.Add("GeneratedShape",
                  "Generatedshape df shape Generationentry [drawname]",
                  __FILE__,
                  Generatedshape,
                  g);
}
