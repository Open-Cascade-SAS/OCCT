// Copyright: 	Matra-Datavision 1995
// File:	StdSelect_Prs.cxx
// Created:	Fri Mar 17 13:45:56 1995
// Author:	Robert COUBLANC
//		<rob>



#include <StdSelect_Prs.ixx>

StdSelect_Prs::
StdSelect_Prs(const Handle(Graphic3d_StructureManager)& aStructureManager):
Prs3d_Presentation(aStructureManager),
myManager(aStructureManager){}

