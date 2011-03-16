#include <IGESSelect_AutoCorrect.ixx>
#include <IGESData_Protocol.hxx>
#include <IGESData_BasicEditor.hxx>
#include <Interface_Check.hxx>

#include <Interface_Macros.hxx>




    IGESSelect_AutoCorrect::IGESSelect_AutoCorrect ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_AutoCorrect::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  DeclareAndCast(IGESData_Protocol,protocol,ctx.Protocol());
  if (protocol.IsNull()) {
    ctx.CCheck()->AddFail("IGES Auto Correct, not called with Protocol");
    return;
  }

  IGESData_BasicEditor corrector (target,protocol);
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,ent,ctx.ValueResult());
    Standard_Boolean done = corrector.AutoCorrect (ent);
    if (done) ctx.Trace();
  }
}


    TCollection_AsciiString  IGESSelect_AutoCorrect::Label () const
{
  return TCollection_AsciiString ("Auto-Correction of IGES Entities");
}
