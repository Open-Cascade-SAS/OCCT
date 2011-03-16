#include <IGESSelect_ComputeStatus.ixx>
#include <IGESData_Protocol.hxx>
#include <IGESData_BasicEditor.hxx>
#include <Interface_Check.hxx>

#include <Interface_Macros.hxx>

    IGESSelect_ComputeStatus::IGESSelect_ComputeStatus ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_ComputeStatus::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  DeclareAndCast(IGESData_Protocol,protocol,ctx.Protocol());
  if (protocol.IsNull()) {
    ctx.CCheck()->AddFail("IGES Compute Status, Protocol incorrect");
    return;
  }

  IGESData_BasicEditor corrector (target,protocol);
  corrector.ComputeStatus();
}


    TCollection_AsciiString  IGESSelect_ComputeStatus::Label () const
{
  return TCollection_AsciiString ("Compute Status of IGES Entities (Directory Part)");
}
