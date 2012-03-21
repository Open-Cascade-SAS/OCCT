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

#include <IFSelect_ModifEditForm.ixx>

IFSelect_ModifEditForm::IFSelect_ModifEditForm
  (const Handle(IFSelect_EditForm)& editform)
    : IFSelect_Modifier (Standard_False)    {  theedit = editform;  }

    Handle(IFSelect_EditForm)  IFSelect_ModifEditForm::EditForm () const
      {  return theedit;  }


    void  IFSelect_ModifEditForm::Perform
  (IFSelect_ContextModif& ctx,
   const Handle(Interface_InterfaceModel)& target,
   const Handle(Interface_Protocol)& protocol,
   Interface_CopyTool& TC) const
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
