// Created on: 1997-03-04
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <AIS_AttributeFilter.hxx>
#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_AttributeFilter,SelectMgr_Filter)

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
  Handle(AIS_InteractiveObject) aSelectable (Handle(AIS_InteractiveObject)::DownCast (anObj->Selectable()));
  if (aSelectable.IsNull())
    return Standard_False;
  
  Standard_Boolean okstat = Standard_True;
  if( hasC && aSelectable->HasColor() )
  {
    Quantity_Color aColor;
    aSelectable->Color (aColor);
    okstat = (myCol == aColor.Name());
  }

  if( hasW && aSelectable->HasWidth() )
    okstat = (myWid == aSelectable->Width()) && okstat;

  return okstat;
}
