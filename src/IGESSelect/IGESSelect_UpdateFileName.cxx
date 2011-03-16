#include <IGESSelect_UpdateFileName.ixx>
#include <IGESData_GlobalSection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Check.hxx>


    IGESSelect_UpdateFileName::IGESSelect_UpdateFileName  ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_UpdateFileName::Performing
  (IFSelect_ContextModif& ctx, const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
{
  if (!ctx.HasFileName()) {
    ctx.CCheck(0)->AddWarning("New File Name unknown, former one is kept");
    return;
  }
  IGESData_GlobalSection GS = target->GlobalSection();
  GS.SetFileName (new TCollection_HAsciiString (ctx.FileName()) );
  target->SetGlobalSection(GS);
  Handle(Interface_Check) check = new Interface_Check;
  target->VerifyCheck(check);
  ctx.AddCheck(check);
}

    TCollection_AsciiString  IGESSelect_UpdateFileName::Label () const
{ return TCollection_AsciiString("Updates IGES File Name to new current one"); }
