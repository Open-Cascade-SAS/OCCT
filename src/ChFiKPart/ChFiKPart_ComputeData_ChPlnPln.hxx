// Created on: 1995-04-28
// Created by: Stagiaire Flore Lautheanne
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef _ChFiKPart_ComputeData_ChPlnPln_HeaderFile
#define _ChFiKPart_ComputeData_ChPlnPln_HeaderFile


Standard_Boolean ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure& DStr,
				       const Handle(ChFiDS_SurfData)& Data, 
				       const gp_Pln& Pl1, 
				       const gp_Pln& Pl2, 
				       const TopAbs_Orientation Or1,
				       const TopAbs_Orientation Or2,
				       const Standard_Real Dis1, 
				       const Standard_Real Dis2, 
				       const gp_Lin& Spine, 
				       const Standard_Real First, 
				       const TopAbs_Orientation Of1);

#endif
