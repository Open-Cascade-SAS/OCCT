// Created on: 1995-02-23
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Draw.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <TCollection_AsciiString.hxx>
#include <Units.hxx>
#include <Units_UnitsDictionary.hxx>
#include <Units_UnitSentence.hxx>
#include <UnitsAPI.hxx>

//=======================================================================
// function : parsing
// purpose  : parsing of unit's expression
//=======================================================================
static int parsing(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage : " << argv[0] << " string [nbiter]\n";
    return 1;
  }

  TCollection_AsciiString aStrTok(argv[1]);
  int                     nbIter = 1;
  if (argc > 2)
    nbIter = Draw::Atoi(argv[2]);
  UnitsAPI::SetLocalSystem();
  occ::handle<Units_Token> atoken;
  Units_UnitSentence       aUnitSent(aStrTok.ToCString());

  if (!aUnitSent.IsDone())
  {
    di << "can not create a sentence\n";
    return 1;
  }

  int i = 1;
  for (; i <= nbIter; i++)
  {
    aUnitSent.Analyse();
    // aUnitSent.Dump();
    occ::handle<NCollection_HSequence<occ::handle<Units_Token>>> aseq = aUnitSent.Sequence();
  }
  atoken = aUnitSent.Evaluate();
  di << "Token word : " << atoken->Word().ToCString() << "\n";
  return 0;
}

//=======================================================================
// function : unitsdico
// purpose  : dump dictionary of units
//=======================================================================
static int unitsdico(Draw_Interpretor& /* di */, int /*argc*/, const char** /*argv*/)
{
  UnitsAPI::SetLocalSystem();
  int mode = 2;
  Units::DictionaryOfUnits(false)->Dump(mode);
  return 0;
}

//=================================================================================================

static int converttoSI(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Invalid number of parameter, use: unitconvtoSI real string\n";
    return 1;
  }

  double      aData = Draw::Atof(argv[1]);
  const char* aUnit = argv[2];

  double res = UnitsAPI::AnyToSI(aData, aUnit);
  di << "result: " << res << "\n";

  return 0;
}

//=================================================================================================

static int converttoMDTV(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Invalid number of parameter, use: unitconvtoMDTV real string\n";
    return 1;
  }

  double      aData = Draw::Atof(argv[1]);
  const char* aUnit = argv[2];

  UnitsAPI::SetLocalSystem(UnitsAPI_MDTV);
  double res = UnitsAPI::AnyToLS(aData, aUnit);
  di << "result: " << res << "\n";

  return 0;
}

//=================================================================================================

static int unit(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 4)
  {
    di << Units::Convert(Draw::Atof(a[1]), a[2], a[3]);
    return 0;
  }
  else
    return 1;
}

//=================================================================================================

void Draw::UnitCommands(Draw_Interpretor& theCommands)
{
  static bool Done = false;
  if (Done)
    return;
  Done = true;

  const char* g = "DRAW Unit Commands";

  theCommands.Add("unitparsing", "unitparsing string [nbiter]", __FILE__, parsing, g);
  theCommands.Add("unitsdico", "unitsdico", __FILE__, unitsdico, g);
  theCommands.Add("unitconvtoSI", "unitconvtoSI real string", __FILE__, converttoSI, g);
  theCommands.Add("unitconvtoMDTV", "unitconvtoMDTV real string", __FILE__, converttoMDTV, g);
  theCommands.Add("unit", "unit value unitfrom unitto", __FILE__, unit, g);
}
