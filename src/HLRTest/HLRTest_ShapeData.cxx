// File:	HLRTest_ShapeData.cxx
// Created:	Tue Aug 25 10:43:05 1992
// Author:	Christophe MARION
//		<cma@sdsun2>

#include <HLRTest_ShapeData.ixx>

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

