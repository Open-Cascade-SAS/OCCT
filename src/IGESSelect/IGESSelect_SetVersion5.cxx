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


#include <IFSelect_ContextModif.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_SetVersion5.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SetVersion5,IGESSelect_ModelModifier)

IGESSelect_SetVersion5::IGESSelect_SetVersion5 ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_SetVersion5::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  IGESData_GlobalSection GS = target->GlobalSection();
  if (GS.IGESVersion() >= 9) return;
  GS.SetIGESVersion(9);
  GS.SetLastChangeDate ();
  target->SetGlobalSection(GS);
  Handle(Interface_Check) check = new Interface_Check;
  target->VerifyCheck(check);
  if (check->HasFailed()) ctx.CCheck()->GetMessages(check);
}


    TCollection_AsciiString  IGESSelect_SetVersion5::Label () const
{ return TCollection_AsciiString ("Update IGES Version in Global Section to 5.1"); }
