// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000 and YYYYMMDD.HHMMSS after Y2000)
#include <IGESSelect_UpdateCreationDate.ixx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <IGESData_GlobalSection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Check.hxx>



IGESSelect_UpdateCreationDate::IGESSelect_UpdateCreationDate ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_UpdateCreationDate::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  Standard_Integer jour,mois,annee,heure,minute,seconde,millisec,microsec;
  OSD_Process system;
  Quantity_Date ladate = system.SystemDate();
  ladate.Values (mois,jour,annee,heure,minute,seconde,millisec,microsec);

  IGESData_GlobalSection GS = target->GlobalSection();
  if (annee < 2000)
     //#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000)
    GS.SetDate (IGESData_GlobalSection::NewDateString
		(annee,mois,jour,heure,minute,seconde,0));
  else 
    //#65 rln 12.02.99 S4151 (explicitly force YYYYMMDD.HHMMSS after Y2000)
    GS.SetDate (IGESData_GlobalSection::NewDateString
		(annee,mois,jour,heure,minute,seconde, -1));
  target->SetGlobalSection(GS);
  Handle(Interface_Check) check = new Interface_Check;
  target->VerifyCheck(check);
  ctx.AddCheck(check);
}


    TCollection_AsciiString  IGESSelect_UpdateCreationDate::Label () const
{ return TCollection_AsciiString ("Update Creation Date in IGES Global Section"); }
