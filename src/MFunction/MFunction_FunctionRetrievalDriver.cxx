// File:	MFunction_FunctionRetrievalDriver.cxx
// Created:	Thu Jun 17 12:11:20 1999
// Author:	Vladislav ROMASHKO
//		<vro@flox.nnov.matra-dtv.fr>


#include <MFunction_FunctionRetrievalDriver.ixx>

#include <PFunction_Function.hxx>
#include <TFunction_Function.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_NoSuchObject.hxx>

MFunction_FunctionRetrievalDriver::MFunction_FunctionRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}

Standard_Integer MFunction_FunctionRetrievalDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MFunction_FunctionRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PFunction_Function);  }

Handle(TDF_Attribute) MFunction_FunctionRetrievalDriver::NewEmpty() const
{ return new TFunction_Function(); }

void MFunction_FunctionRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
					       const Handle(TDF_Attribute)&        Target,
//					       const Handle(MDF_RRelocationTable)& RelocTable) const
					       const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PFunction_Function) S = Handle(PFunction_Function)::DownCast(Source);
  Handle(TFunction_Function) T = Handle(TFunction_Function)::DownCast(Target);
  T->SetDriverGUID(S->GetDriverGUID());
  T->SetFailure(S->GetFailure()); 
}
