// File:	AIS_TypeFilter.cxx
// Created:	Thu Mar  6 16:59:32 1997
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <AIS_TypeFilter.ixx>
#include <AIS_InteractiveObject.hxx>

AIS_TypeFilter::AIS_TypeFilter(const AIS_KindOfInteractive TheKind):
myKind(TheKind){}

Standard_Boolean AIS_TypeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anObj) const 
{
  if(Handle(AIS_InteractiveObject)::DownCast(anObj->Selectable()).IsNull()) 
    return Standard_False;
  
//#ifndef DEB
  Handle_SelectMgr_SelectableObject aSelectableObject = anObj->Selectable() ;
  return ((Handle(AIS_InteractiveObject)&) aSelectableObject)->Type()== myKind;
//#else
//  return ((Handle(AIS_InteractiveObject)&)anObj->Selectable())->Type()== myKind;
//#endif
}
