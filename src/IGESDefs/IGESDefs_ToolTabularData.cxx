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
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_ToolTabularData.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

IGESDefs_ToolTabularData::IGESDefs_ToolTabularData ()    {  }


void IGESDefs_ToolTabularData::ReadOwnParams
  (const Handle(IGESDefs_TabularData)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{
  Standard_Integer nbProps;
  Standard_Integer propType;
  Standard_Integer nbDeps;
  Standard_Integer nbIndeps;
  Handle(TColStd_HArray1OfInteger) typesInd;
  Handle(TColStd_HArray1OfInteger) nbValuesInd;
  Handle(IGESBasic_HArray1OfHArray1OfReal) valuesInd;
  Handle(IGESBasic_HArray1OfHArray1OfReal) valuesDep;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer i;

  PR.ReadInteger(PR.Current(), "Number of Property values", nbProps); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInteger(PR.Current(), "Property type", propType); //szv#4:S4163:12Mar99 `st=` not needed

  Standard_Boolean st = PR.ReadInteger(PR.Current(), "No. of dependent variables", nbDeps);
  if (st && nbDeps > 0)
    valuesDep = new IGESBasic_HArray1OfHArray1OfReal(1, nbDeps);

  st = PR.ReadInteger(PR.Current(), "No. of Independent variables", nbIndeps);
  if (st && nbIndeps > 0)
    {
      valuesInd = new IGESBasic_HArray1OfHArray1OfReal(1, nbIndeps);
      typesInd = new TColStd_HArray1OfInteger(1, nbIndeps);
      nbValuesInd = new TColStd_HArray1OfInteger(1, nbIndeps);
    }

  PR.ReadInts(PR.CurrentList(nbIndeps),
	      "Type of independent variables", typesInd); //szv#4:S4163:12Mar99 `st=` not needed

  PR.ReadInts(PR.CurrentList(nbIndeps),
	      "No. of values of independent variables", nbValuesInd); //szv#4:S4163:12Mar99 `st=` not needed

  for (i=1; i<=nbIndeps; i++)
    {
      Handle(TColStd_HArray1OfReal) tarr;
      Standard_Integer nb = nbValuesInd->Value(i), j;
      if (nb > 0 )
	{
          tarr = new TColStd_HArray1OfReal(1, nb);
          for (j=1; j<= nb; j++)
	    {
              Standard_Real treal;
              PR.ReadReal(PR.Current(), "Value of independent variable",
			  treal); //szv#4:S4163:12Mar99 `st=` not needed
              tarr->SetValue(j, treal);
	    }
	}
      valuesInd->SetValue(i, tarr);
    }
// ??  for (i=1; i<=nbDeps; i++) {  }
//  Dependents : definition pas limpide, on accumule tout sur un seul
//  HArray1OfReal, mis en 1re position du HArray1OfHArray1OfReal
//  On y met tous les flottants qui restent
  Standard_Integer curnum = PR.CurrentNumber();
  Standard_Integer nbpars = PR.NbParams();
  Standard_Integer nbd = 0;
  for (i = curnum; i <= nbpars; i ++) {
    if (PR.ParamType(i) != Interface_ParamReal) break;
    nbd = i - curnum + 1;
  }
  Handle(TColStd_HArray1OfReal) somedeps;
  if (nbd > 0) somedeps = new TColStd_HArray1OfReal(1,nbd);
  for (i = 1; i <= nbd; i ++) {
    Standard_Real treal;
    PR.ReadReal(PR.Current(), "Value of dependent variable", treal); //szv#4:S4163:12Mar99 `st=` not needed
    somedeps->SetValue(i, treal);
  }
  if (nbDeps > 0) valuesDep->SetValue(1,somedeps);
  else PR.AddWarning("Some Real remain while no dependent vakue is defined");

  nbProps = PR.CurrentNumber() - 2;
/*  for (;;) {
    curnum = PR.CurrentNumber();
    if (curnum > PR.NbParams()) break;
    if (PR.ParamType(curnum) != Interface_ParamReal) break;
    PR.SetCurrentNumber (curnum+1);
  }  */
  PR.AddWarning("Don't know exactly how to read dependant values ...");
//  ??  a eclaircir
  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (nbProps, propType, typesInd, nbValuesInd, valuesInd, valuesDep);
}

void IGESDefs_ToolTabularData::WriteOwnParams
  (const Handle(IGESDefs_TabularData)& ent, IGESData_IGESWriter& IW) const
{
  Standard_Integer i, nbIndeps = ent->NbIndependents();
  Standard_Integer j, nbDeps = ent->NbDependents();
  IW.Send(ent->NbPropertyValues());
  IW.Send(ent->PropertyType());
  IW.Send(nbDeps);
  IW.Send(nbIndeps);
  for (i=1; i<=nbIndeps; i++)
    IW.Send(ent->TypeOfIndependents(i));
  for (i=1; i<=nbIndeps; i++)
    IW.Send(ent->NbValues(i));
  for (i=1; i<=nbIndeps; i++)
    for (j=1; j<=ent->NbValues(i); j++)
      IW.Send(ent->IndependentValue(i,j));
  // UNFINISHED
  if (nbDeps == 0) return;
  Handle(TColStd_HArray1OfReal) deps = ent->DependentValues(1);
  for (i = 1; i <= deps->Length(); i ++) IW.Send(deps->Value(i));
  /*
    for (i=1; i<=nbDeps; i++)
    for (j=1; j<= .. ->Value(i); j++)
    IW.Send(ent->DependentValue(i,j));
    */
}

void  IGESDefs_ToolTabularData::OwnShared
  (const Handle(IGESDefs_TabularData)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void IGESDefs_ToolTabularData::OwnCopy
  (const Handle(IGESDefs_TabularData)& another,
   const Handle(IGESDefs_TabularData)& ent, Interface_CopyTool& /* TC */) const
{
  Standard_Integer nbProps = another->NbPropertyValues();
  Standard_Integer propType = another->PropertyType();
  Standard_Integer nbDeps = another->NbDependents();
  Standard_Integer nbIndeps = another->NbIndependents();
  Handle(TColStd_HArray1OfInteger) typesInd = new
    TColStd_HArray1OfInteger(1, nbIndeps);
  Handle(TColStd_HArray1OfInteger) nbValuesInd = new
    TColStd_HArray1OfInteger(1, nbIndeps);
  Handle(IGESBasic_HArray1OfHArray1OfReal) valuesInd = new
    IGESBasic_HArray1OfHArray1OfReal(1, nbIndeps);
  Handle(IGESBasic_HArray1OfHArray1OfReal) valuesDep = new
    IGESBasic_HArray1OfHArray1OfReal(1, nbDeps);
  Standard_Integer i;
  for (i=1; i<=nbIndeps; i++)
    {
      Standard_Integer j, nval;
      typesInd->SetValue(i, another->TypeOfIndependents(i));
      nval = another->NbValues(i);
      nbValuesInd->SetValue(i, nval);
      Handle(TColStd_HArray1OfReal) tmparr = new
	TColStd_HArray1OfReal(1, nval);
      for (j=1; j<=nval; j++)
	tmparr->SetValue(j, another->IndependentValue(i, j));
      valuesInd->SetValue(i, tmparr);
    }
  // UNFINISHED
  /*
    for (i=1; i<=nbDeps; i++)
    {
    }
    */
  ent->Init(nbProps, propType, typesInd, nbValuesInd,
	    valuesInd, valuesDep);
}

IGESData_DirChecker IGESDefs_ToolTabularData::DirChecker
  (const Handle(IGESDefs_TabularData)& /* ent */ ) const
{
  IGESData_DirChecker DC(406, 11);
  DC.Structure  (IGESData_DefVoid);
  DC.LineFont   (IGESData_DefVoid);
  DC.LineWeight (IGESData_DefVoid);
  DC.Color      (IGESData_DefVoid);

  DC.BlankStatusIgnored ();
  DC.UseFlagIgnored ();
  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESDefs_ToolTabularData::OwnCheck
  (const Handle(IGESDefs_TabularData)& /* ent */,
   const Interface_ShareTool& , Handle(Interface_Check)& /* ach */) const
{
}

void IGESDefs_ToolTabularData::OwnDump
  (const Handle(IGESDefs_TabularData)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level) const
{
  Standard_Integer nbIndeps = ent->NbIndependents(); //szv#4:S4163:12Mar99 i unused
  Standard_Integer nbDeps = ent->NbDependents();

  S << "IGESDefs_TabularData" << endl;
  S << "No. of property values : " << ent->NbPropertyValues() << endl;
  S << "Property type : " << ent->PropertyType() << endl;
  S << "No. of Dependent variables    : " << nbDeps << endl;
  S << "No. of Independent variables  : " << nbIndeps << endl;
  S << "Type of independent variables : ";
  IGESData_DumpVals(S ,level,1, nbIndeps,ent->TypeOfIndependents);
  S << endl << "Number of values of independent variables : ";
  IGESData_DumpVals(S ,level,1, nbIndeps,ent->NbValues);
// ?? JAGGED ??
  S << endl << "Values of the independent variable : ";
  if (level < 5) S << " [ask level > 4]";
  else {
    for (Standard_Integer ind = 1; ind <= nbIndeps; ind ++) {
      S << endl << "[" << ind << "]:";
      Standard_Integer nbi = ent->NbValues(ind);
      for (Standard_Integer iv = 1; iv <= nbi; iv ++)
	S << " " << ent->IndependentValue(ind,iv);
    }
  }
//  IGESData_DumpVals(S ,level,1, nbIndeps,ent->IndependentValue);
  S << endl << "Values of the dependent variable : ";
//  IGESData_DumpVals(S ,level,1, nbDeps,ent->DependentValue);
  S << "  TO BE DONE";
  S << endl;
}
