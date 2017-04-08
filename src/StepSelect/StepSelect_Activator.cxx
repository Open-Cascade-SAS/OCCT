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


#include <IFSelect_SessionPilot.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Macros.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <StepData_Plex.hxx>
#include <StepData_Simple.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <StepSelect_Activator.hxx>
#include <StepSelect_FloatFormat.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepSelect_Activator,IFSelect_Activator)

static int THE_StepSelect_Activator_initActivator = 0;

StepSelect_Activator::StepSelect_Activator ()
{
  if (THE_StepSelect_Activator_initActivator)
  {
    return;
  }

  THE_StepSelect_Activator_initActivator = 1;
//  Add ( 0,"load");
//  Add ( 0,"loadstep");    // homonyme
//  Add ( 1,"entity");
//  Add ( 2,"liststep");

//  AddSet (10,"steptype");

  Add    ( 1,"stepschema");
  AddSet (40,"floatformat");
}


IFSelect_ReturnStatus  StepSelect_Activator::Do
  (const Standard_Integer number,
   const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Word(1).ToCString();
  const Standard_CString arg2 = pilot->Word(2).ToCString();
//  const Standard_CString arg3 = pilot->Word(3).ToCString();

  switch (number) {

    case  1 : {   //        ****    StepSchema
      if (argc < 2) {
        cout<<"Identify an entity"<<endl;
        return IFSelect_RetError;
      }
      Standard_Integer num = pilot->Number(arg1);
      if (num <= 0) {
        cout<<"Not an entity : "<<arg2<<endl;
        return IFSelect_RetError;
      }
      Handle(Standard_Transient) ent = pilot->Session()->StartingEntity(num);
      DeclareAndCast(StepData_UndefinedEntity,und,ent);
      if (!und.IsNull()) {
	cout<<"Entity "<<arg2<<" : No Binding known"<<endl;
	return IFSelect_RetVoid;
      }
      DeclareAndCast(StepData_Simple,sim,ent);
      if (!sim.IsNull()) {
	cout<<"Entity "<<arg2<<" : Late Binding"<<endl;
	cout<<"Simple Type : "<<sim->StepType()<<endl;
	return IFSelect_RetVoid;
      }
      DeclareAndCast(StepData_Plex,plx,ent);
      if (!plx.IsNull()) {
	cout<<"Entity "<<arg2<<" : Late Binding"<<endl;
	cout<<"Complex Type"<<endl;
      }
//       reste Early Binding
      cout<<"Entity "<<arg2<<" : Early Binding"<<endl;
      cout<<"CDL Type : "<<ent->DynamicType()->Name()<<endl;
      return IFSelect_RetVoid;
    }

    case 40 : {   //        ****    FloatFormat
      char prem = ' ';
      if (argc < 2) prem = '?';
      else if (argc == 5) { cout<<"floatformat tout court donne les formes admises"<<endl; return IFSelect_RetError; }
      else prem = arg1[0];
      Standard_Boolean zerosup=Standard_False;
      Standard_Integer digits = 0;
      if      (prem == 'N' || prem == 'n') zerosup = Standard_False;
      else if (prem == 'Z' || prem == 'z') zerosup = Standard_True;
      else if (prem >= 48  && prem <= 57)  digits  = atoi(arg1);
      else {
	cout<<"floatformat digits, digits=nb de chiffres signifiants, ou\n"
	  <<  "floatformat NZ %mainformat [%rangeformat [Rmin Rmax]]\n"
	  <<"  NZ : N ou n pour Non-zero-suppress, Z ou z pour zero-suppress\n"
	  <<" %mainformat  : format principal type printf, ex,: %E\n"
	  <<" + optionnel  : format secondaire (flottants autour de 1.) :\n"
	  <<" %rangeformat Rmin Rmax : format type printf entre Rmin et Rmax\n"
	  <<" %rangeformat tout seul : format type printf entre 0.1 et 1000.\n"
	    <<flush;
	return (prem == '?' ? IFSelect_RetVoid : IFSelect_RetError);
      }
      Standard_Real Rmin=0., Rmax=0.;
      if (argc > 4) {
	Rmin = Atof(pilot->Word(4).ToCString());
	Rmax = Atof(pilot->Word(5).ToCString());
	if (Rmin <= 0 || Rmax <= 0) { cout<<"intervalle : donner reels > 0"<<endl; return IFSelect_RetError; }
      }
      Handle(StepSelect_FloatFormat) fm = new StepSelect_FloatFormat;
      if (argc == 2) fm->SetDefault(digits);
      else {
	fm->SetZeroSuppress(zerosup);
	fm->SetFormat (arg2);
	if      (argc == 4) fm->SetFormatForRange(pilot->Word(3).ToCString());
	else if (argc >= 6) fm->SetFormatForRange(pilot->Word(3).ToCString(),Rmin,Rmax);
	else                fm->SetFormatForRange("");
      }
      return pilot->RecordItem(fm);
    }

    default : break;
  }
  return IFSelect_RetVoid;

}


Standard_CString  StepSelect_Activator::Help
  (const Standard_Integer number) const
{
  switch (number) {

    case 40 : return "options... : cree FloatFormat ... floatformat tout court->help";
    default : break;
  }
  return "";
}
