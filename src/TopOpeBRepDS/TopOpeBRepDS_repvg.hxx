// Created on: 1998-02-14
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


#ifndef _TopOpeBRepDS_regpv_HeaderFile
#define _TopOpeBRepDS_regpv_HeaderFile

#include <TopOpeBRepDS_define.hxx>
Standard_EXPORT void FDS_repvg
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
Standard_EXPORT void FDS_repvg1
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
Standard_EXPORT void FDS_repvg2
(const TopOpeBRepDS_DataStructure& BDS,const Standard_Integer EIX,const TopOpeBRepDS_Kind GT,TopOpeBRepDS_ListOfInterference& LI,TopOpeBRepDS_ListOfInterference& reducedLI);
#endif
