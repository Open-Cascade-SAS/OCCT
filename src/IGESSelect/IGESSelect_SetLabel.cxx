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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_SetLabel.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SetLabel,IGESSelect_ModelModifier)

IGESSelect_SetLabel::IGESSelect_SetLabel
  (const Standard_Integer mode, const Standard_Boolean enforce)
    : IGESSelect_ModelModifier (Standard_False) ,
      themode (mode) , theforce (enforce)    {  }

    void  IGESSelect_SetLabel::Performing(IFSelect_ContextModif& ctx,
                                          const Handle(IGESData_IGESModel)& target,
                                          Interface_CopyTool& /*TC*/) const
{
  Handle(TCollection_HAsciiString) lab;
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,iges,ctx.ValueResult());
    if (iges.IsNull()) continue;
    if (themode == 0)  {  iges->SetLabel(lab); continue;  }

// mode = 1 : mettre DEnnn , nnn est le DE Number
    lab = iges->ShortLabel();
    if (theforce) lab.Nullify();
    if (!lab.IsNull()) {
      if (lab->Length() > 2) {
	if (lab->Value(1) == 'D' && lab->Value(2) == 'E' &&
	    atoi( &(lab->ToCString())[2] ) > 0)
	  lab.Nullify();
      }
    }
//    Si lab nul : le recalculer
    if (lab.IsNull()) {
      lab = new TCollection_HAsciiString(target->Number(iges)*2-1);
      lab->Insert (1,"DE");
    }
    iges->SetLabel(lab);
  }
}

    TCollection_AsciiString  IGESSelect_SetLabel::Label () const
{
  TCollection_AsciiString lab;
  if (themode == 0) lab.AssignCat ("Clear Short Label");
  if (themode == 1) lab.AssignCat ("Set Short Label to DE Number");
  if (theforce) lab.AssignCat (" (enforced)");
  return lab;
}
