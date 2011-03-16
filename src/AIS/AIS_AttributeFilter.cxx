// File:	AIS_AttributeFilter.cxx
// Created:	Tue Mar  4 18:13:59 1997
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <AIS_AttributeFilter.ixx>

#include <AIS_InteractiveObject.hxx>


AIS_AttributeFilter::AIS_AttributeFilter():
hasC(Standard_False),
hasW(Standard_False){}


AIS_AttributeFilter::AIS_AttributeFilter(const Quantity_NameOfColor aCol):
myCol(aCol),
hasC(Standard_True),
hasW(Standard_False){}


AIS_AttributeFilter::AIS_AttributeFilter(const Standard_Real aWid):
myWid(aWid),
hasC(Standard_False),
hasW(Standard_True){}


Standard_Boolean AIS_AttributeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& anObj) const 
{
  if (Handle(AIS_InteractiveObject)::DownCast(anObj->Selectable()).IsNull()) return Standard_False;
  Standard_Boolean okstat = Standard_True;

//#ifndef DEB
  Handle_SelectMgr_SelectableObject aSelectable = anObj->Selectable() ;
  if( hasC && ((Handle(AIS_InteractiveObject)&) aSelectable)->HasColor() )
//#else
//  if(hasC && ((Handle(AIS_InteractiveObject)&) anObj->Selectable())->HasColor())
//#endif
    okstat =  (myCol == ((Handle(AIS_InteractiveObject)&) anObj)->Color());

//#ifndef DEB
  aSelectable = anObj->Selectable() ;
  if( hasW && ((Handle(AIS_InteractiveObject)&) aSelectable)->HasWidth() )
//#else
//  if(hasW && ((Handle(AIS_InteractiveObject)&) anObj->Selectable())->HasWidth())
//#endif
    okstat =  (myWid == ((Handle(AIS_InteractiveObject)&) anObj)->Width()) && okstat;

  return okstat;
  
}
