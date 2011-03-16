#include <IGESSelect_SetVersion5.ixx>
#include <IGESData_GlobalSection.hxx>
#include <Interface_Check.hxx>



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
