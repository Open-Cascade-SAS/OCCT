// File:	MFunction_FunctionStorageDriver.cxx
// Created:	Thu Jun 17 11:58:51 1999
// Author:	Vladislav ROMASHKO
//		<vro@flox.nnov.matra-dtv.fr>


#include <MFunction_FunctionStorageDriver.ixx>

#include <TFunction_Function.hxx>
#include <PFunction_Function.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_NoSuchObject.hxx>

MFunction_FunctionStorageDriver::MFunction_FunctionStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}

Standard_Integer MFunction_FunctionStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MFunction_FunctionStorageDriver::SourceType() const
{ return STANDARD_TYPE(TFunction_Function); }

Handle(PDF_Attribute) MFunction_FunctionStorageDriver::NewEmpty() const
{ return new PFunction_Function(); }

void MFunction_FunctionStorageDriver::Paste(const Handle(TDF_Attribute)&        Source,
					    const Handle(PDF_Attribute)&        Target,
//					    const Handle(MDF_SRelocationTable)& RelocTable) const
					    const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TFunction_Function) S = Handle(TFunction_Function)::DownCast (Source);
  Handle(PFunction_Function) T = Handle(PFunction_Function)::DownCast (Target);
  T->SetDriverGUID(S->GetDriverGUID());
  T->SetFailure(S->GetFailure());
}
