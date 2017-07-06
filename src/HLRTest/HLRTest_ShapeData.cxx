// Created on: 1992-08-25
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Draw_Color.hxx>
#include <HLRTest_ShapeData.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRTest_ShapeData,Standard_Transient)

//=======================================================================
//function : HLRTest_ShapeData
//purpose  : 
//=======================================================================
HLRTest_ShapeData::HLRTest_ShapeData
  (const Draw_Color& CVis,
   const Draw_Color& COVis,
   const Draw_Color& CIVis,
   const Draw_Color& CHid,
   const Draw_Color& COHid,
   const Draw_Color& CIHid) :
  myVColor(CVis),myVOColor(COVis),myVIColor(CIVis),
  myHColor(CHid),myHOColor(COHid),myHIColor(CIHid)
{}

