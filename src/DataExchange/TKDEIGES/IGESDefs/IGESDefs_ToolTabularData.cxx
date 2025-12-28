// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// UNFINISHED
// The last field (ent->DependentValues()) not resolved. Queried to mdtv

#include <IGESBasic_HArray1OfHArray1OfReal.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_ToolTabularData.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IGESDefs_ToolTabularData::IGESDefs_ToolTabularData() = default;

void IGESDefs_ToolTabularData::ReadOwnParams(const occ::handle<IGESDefs_TabularData>& ent,
                                             const occ::handle<IGESData_IGESReaderData>& /* IR */,
                                             IGESData_ParamReader& PR) const
{
  int                                           nbProps;
  int                                           propType;
  int                                           nbDeps;
  int                                           nbIndeps;
  occ::handle<NCollection_HArray1<int>>         typesInd;
  occ::handle<NCollection_HArray1<int>>         nbValuesInd;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> valuesInd;
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> valuesDep;
  // bool st; //szv#4:S4163:12Mar99 moved down
  int i;

  // clang-format off
  PR.ReadInteger(PR.Current(), "Number of Property values", nbProps); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  PR.ReadInteger(PR.Current(), "Property type", propType); // szv#4:S4163:12Mar99 `st=` not needed

  bool st = PR.ReadInteger(PR.Current(), "No. of dependent variables", nbDeps);
  if (st && nbDeps > 0)
    valuesDep = new IGESBasic_HArray1OfHArray1OfReal(1, nbDeps);

  st = PR.ReadInteger(PR.Current(), "No. of Independent variables", nbIndeps);
  if (st && nbIndeps > 0)
  {
    valuesInd   = new IGESBasic_HArray1OfHArray1OfReal(1, nbIndeps);
    typesInd    = new NCollection_HArray1<int>(1, nbIndeps);
    nbValuesInd = new NCollection_HArray1<int>(1, nbIndeps);
  }

  PR.ReadInts(PR.CurrentList(nbIndeps),
              "Type of independent variables",
              typesInd); // szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInts(PR.CurrentList(nbIndeps),
              // clang-format off
	      "No. of values of independent variables", nbValuesInd); //szv#4:S4163:12Mar99 `st=` not needed
  // clang-format on

  for (i = 1; i <= nbIndeps; i++)
  {
    occ::handle<NCollection_HArray1<double>> tarr;
    int                                      nb = nbValuesInd->Value(i), j;
    if (nb > 0)
    {
      tarr = new NCollection_HArray1<double>(1, nb);
      for (j = 1; j <= nb; j++)
      {
        double treal;
        PR.ReadReal(PR.Current(),
                    "Value of independent variable",
                    treal); // szv#4:S4163:12Mar99 `st=` not needed
        tarr->SetValue(j, treal);
      }
    }
    valuesInd->SetValue(i, tarr);
  }
  // ??  for (i=1; i<=nbDeps; i++) {  }
  //  Dependents : definition not clear, we accumulate everything on a single
  //  HArray1OfReal, put in 1st position of the HArray1OfHArray1OfReal
  //  We put all the remaining floats there
  int curnum = PR.CurrentNumber();
  int nbpars = PR.NbParams();
  int nbd    = 0;
  for (i = curnum; i <= nbpars; i++)
  {
    if (PR.ParamType(i) != Interface_ParamReal)
      break;
    nbd = i - curnum + 1;
  }
  occ::handle<NCollection_HArray1<double>> somedeps;
  if (nbd > 0)
    somedeps = new NCollection_HArray1<double>(1, nbd);
  for (i = 1; i <= nbd; i++)
  {
    double treal;
    // clang-format off
    PR.ReadReal(PR.Current(), "Value of dependent variable", treal); //szv#4:S4163:12Mar99 `st=` not needed
    // clang-format on
    somedeps->SetValue(i, treal);
  }
  if (nbDeps > 0)
    valuesDep->SetValue(1, somedeps);
  else
    PR.AddWarning("Some Real remain while no dependent value is defined");

  nbProps = PR.CurrentNumber() - 2;
  /*  for (;;) {
      curnum = PR.CurrentNumber();
      if (curnum > PR.NbParams()) break;
      if (PR.ParamType(curnum) != Interface_ParamReal) break;
      PR.SetCurrentNumber (curnum+1);
    }  */
  PR.AddWarning("Don't know exactly how to read dependent values ...");
  //  ??  to clarify
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(), ent);
  ent->Init(nbProps, propType, typesInd, nbValuesInd, valuesInd, valuesDep);
}

void IGESDefs_ToolTabularData::WriteOwnParams(const occ::handle<IGESDefs_TabularData>& ent,
                                              IGESData_IGESWriter&                     IW) const
{
  int i, nbIndeps = ent->NbIndependents();
  int j, nbDeps   = ent->NbDependents();
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->PropertyType());
  IW.Send(nbDeps);
  IW.Send(nbIndeps);
  for (i = 1; i <= nbIndeps; i++)
    IW.Send(ent->TypeOfIndependents(i));
  for (i = 1; i <= nbIndeps; i++)
    IW.Send(ent->NbValues(i));
  for (i = 1; i <= nbIndeps; i++)
    for (j = 1; j <= ent->NbValues(i); j++)
      IW.Send(ent->IndependentValue(i, j));
  // UNFINISHED
  if (nbDeps == 0)
    return;
  occ::handle<NCollection_HArray1<double>> deps = ent->DependentValues(1);
  for (i = 1; i <= deps->Length(); i++)
    IW.Send(deps->Value(i));
  /*
    for (i=1; i<=nbDeps; i++)
    for (j=1; j<= .. ->Value(i); j++)
    IW.Send(ent->DependentValue(i,j));
    */
}

void IGESDefs_ToolTabularData::OwnShared(const occ::handle<IGESDefs_TabularData>& /* ent */,
                                         Interface_EntityIterator& /* iter */) const
{
}

void IGESDefs_ToolTabularData::OwnCopy(const occ::handle<IGESDefs_TabularData>& another,
                                       const occ::handle<IGESDefs_TabularData>& ent,
                                       Interface_CopyTool& /* TC */) const
{
  int                                   nbProps     = another->NbPropertyValues();
  int                                   propType    = another->PropertyType();
  int                                   nbDeps      = another->NbDependents();
  int                                   nbIndeps    = another->NbIndependents();
  occ::handle<NCollection_HArray1<int>> typesInd    = new NCollection_HArray1<int>(1, nbIndeps);
  occ::handle<NCollection_HArray1<int>> nbValuesInd = new NCollection_HArray1<int>(1, nbIndeps);
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> valuesInd =
    new IGESBasic_HArray1OfHArray1OfReal(1, nbIndeps);
  occ::handle<IGESBasic_HArray1OfHArray1OfReal> valuesDep =
    new IGESBasic_HArray1OfHArray1OfReal(1, nbDeps);
  int i;
  for (i = 1; i <= nbIndeps; i++)
  {
    int j, nval;
    typesInd->SetValue(i, another->TypeOfIndependents(i));
    nval = another->NbValues(i);
    nbValuesInd->SetValue(i, nval);
    occ::handle<NCollection_HArray1<double>> tmparr = new NCollection_HArray1<double>(1, nval);
    for (j = 1; j <= nval; j++)
      tmparr->SetValue(j, another->IndependentValue(i, j));
    valuesInd->SetValue(i, tmparr);
  }
  // UNFINISHED
  /*
    for (i=1; i<=nbDeps; i++)
    {
    }
    */
  ent->Init(nbProps, propType, typesInd, nbValuesInd, valuesInd, valuesDep);
}

IGESData_DirChecker IGESDefs_ToolTabularData::DirChecker(
  const occ::handle<IGESDefs_TabularData>& /* ent */) const
{
  IGESData_DirChecker DC(406, 11);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);

  DC.BlankStatusIgnored();
  DC.UseFlagIgnored();
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDefs_ToolTabularData::OwnCheck(const occ::handle<IGESDefs_TabularData>& /* ent */,
                                        const Interface_ShareTool&,
                                        occ::handle<Interface_Check>& /* ach */) const
{
}

void IGESDefs_ToolTabularData::OwnDump(const occ::handle<IGESDefs_TabularData>& ent,
                                       const IGESData_IGESDumper& /* dumper */,
                                       Standard_OStream& S,
                                       const int         level) const
{
  int nbIndeps = ent->NbIndependents(); // szv#4:S4163:12Mar99 i unused
  int nbDeps   = ent->NbDependents();

  S << "IGESDefs_TabularData\n"
    << "No. of property values : " << ent->NbPropertyValues() << "\n"
    << "Property type : " << ent->PropertyType() << "\n"
    << "No. of Dependent variables    : " << nbDeps << "\n"
    << "No. of Independent variables  : " << nbIndeps << "\n"
    << "Type of independent variables : ";
  IGESData_DumpVals(S, level, 1, nbIndeps, ent->TypeOfIndependents);
  S << "\nNumber of values of independent variables : ";
  IGESData_DumpVals(S, level, 1, nbIndeps, ent->NbValues);
  // ?? JAGGED ??
  S << std::endl << "Values of the independent variable : ";
  if (level < 5)
    S << " [ask level > 4]";
  else
  {
    for (int ind = 1; ind <= nbIndeps; ind++)
    {
      S << std::endl << "[" << ind << "]:";
      int nbi = ent->NbValues(ind);
      for (int iv = 1; iv <= nbi; iv++)
        S << " " << ent->IndependentValue(ind, iv);
    }
  }
  //  IGESData_DumpVals(aSender,level,1, nbIndeps,ent->IndependentValue);
  S << std::endl << "Values of the dependent variable : ";
  //  IGESData_DumpVals(aSender,level,1, nbDeps,ent->DependentValue);
  S << "  TO BE DONE" << std::endl;
}
