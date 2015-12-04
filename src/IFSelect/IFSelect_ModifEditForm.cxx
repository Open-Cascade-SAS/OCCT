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
#include <IFSelect_EditForm.hxx>
#include <IFSelect_ModifEditForm.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ModifEditForm,IFSelect_Modifier)

IFSelect_ModifEditForm::IFSelect_ModifEditForm
  (const Handle(IFSelect_EditForm)& editform)
    : IFSelect_Modifier (Standard_False)    {  theedit = editform;  }

    Handle(IFSelect_EditForm)  IFSelect_ModifEditForm::EditForm () const
      {  return theedit;  }


    void  IFSelect_ModifEditForm::Perform
  (IFSelect_ContextModif& ctx,
   const Handle(Interface_InterfaceModel)& target,
   const Handle(Interface_Protocol)& /*protocol*/,
   Interface_CopyTool& /*TC*/) const
{
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    Standard_Boolean done = theedit->ApplyData(ctx.ValueResult(),target);
    if (done) ctx.Trace();
    else ctx.AddWarning (ctx.ValueResult(),"EditForm could not be applied");
  }
}

    TCollection_AsciiString  IFSelect_ModifEditForm::Label () const
{
  Standard_CString editlab = theedit->Label();
  TCollection_AsciiString lab ("Apply EditForm");
  if (editlab && editlab[0] != '\0') {
    lab.AssignCat (" : ");
    lab.AssignCat (editlab);
  }
  return lab;
}
