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

//#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000 and YYYYMMDD.HHMMSS after Y2000)

#include <IFSelect_ContextModif.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_UpdateLastChange.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_UpdateLastChange,IGESSelect_ModelModifier)

IGESSelect_UpdateLastChange::IGESSelect_UpdateLastChange ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_UpdateLastChange::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  Standard_Integer jour,mois,annee,heure,minute,seconde,millisec,microsec;
  OSD_Process system;
  Quantity_Date ladate = system.SystemDate();
  ladate.Values (mois,jour,annee,heure,minute,seconde,millisec,microsec);

  IGESData_GlobalSection GS = target->GlobalSection();
  if (GS.IGESVersion() < 9) GS.SetIGESVersion(9);
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


    TCollection_AsciiString  IGESSelect_UpdateLastChange::Label () const
{ return TCollection_AsciiString ("Update Last Change Date in IGES Global Section"); }
