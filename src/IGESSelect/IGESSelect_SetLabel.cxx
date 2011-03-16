#include <IGESSelect_SetLabel.ixx>
#include <Interface_Macros.hxx>
#include <IGESData_IGESEntity.hxx>
#include <TCollection_HAsciiString.hxx>

    IGESSelect_SetLabel::IGESSelect_SetLabel
  (const Standard_Integer mode, const Standard_Boolean enforce)
    : IGESSelect_ModelModifier (Standard_False) ,
      themode (mode) , theforce (enforce)    {  }

    void  IGESSelect_SetLabel::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
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
