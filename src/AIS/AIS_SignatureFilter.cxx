// File:	AIS_SignatureFilter.cxx
// Created:	Tue Mar  4 17:31:50 1997
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <AIS_SignatureFilter.ixx>

#include <AIS_InteractiveObject.hxx>

AIS_SignatureFilter::AIS_SignatureFilter(const AIS_KindOfInteractive TheKind,
					 const Standard_Integer      TheSignature):
AIS_TypeFilter(TheKind),
mySig(TheSignature){}

Standard_Boolean AIS_SignatureFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anObj) const 
{
  Handle(AIS_InteractiveObject) IO = Handle(AIS_InteractiveObject)::DownCast(anObj->Selectable());
  if(IO.IsNull()) 
    return Standard_False;
  
  return
    (IO->Signature()==mySig &&
     IO->Type()==myKind);
}
