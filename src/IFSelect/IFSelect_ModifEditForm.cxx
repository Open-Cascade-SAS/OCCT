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
