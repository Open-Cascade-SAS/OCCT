// Created on: 1998-06-15
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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



#ifndef _TopOpeBRepDS_samdom_HeaderFile
#define _TopOpeBRepDS_samdom_HeaderFile

#include <TopOpeBRepDS_define.hxx>

Standard_EXPORT void FDSSDM_prepare(const Handle(TopOpeBRepDS_HDataStructure)&);
Standard_EXPORT void FDSSDM_makes1s2(const TopoDS_Shape& S,TopTools_ListOfShape& L1,TopTools_ListOfShape& L2);
Standard_EXPORT void FDSSDM_s1s2makesordor(const TopTools_ListOfShape& L1,const TopTools_ListOfShape& L2,const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO);
Standard_EXPORT Standard_Boolean FDSSDM_hass1s2(const TopoDS_Shape& S);
Standard_EXPORT void FDSSDM_s1s2(const TopoDS_Shape& S,TopTools_ListOfShape& LS1,TopTools_ListOfShape& LS2);
Standard_EXPORT void FDSSDM_sordor(const TopoDS_Shape& S,TopTools_ListOfShape& LSO,TopTools_ListOfShape& LDO);
Standard_EXPORT Standard_Boolean  FDSSDM_contains(const TopoDS_Shape& S,const TopTools_ListOfShape& L);
Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,const Standard_Integer I1,const Standard_Integer I2,TopTools_ListOfShape& Lou);
Standard_EXPORT void FDSSDM_copylist(const TopTools_ListOfShape& Lin,TopTools_ListOfShape& Lou);

#endif
