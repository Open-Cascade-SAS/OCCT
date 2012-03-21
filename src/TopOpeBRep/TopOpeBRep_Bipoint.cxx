// Created on: 1997-01-09
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TopOpeBRep_Bipoint.ixx>
#include <Standard_ProgramError.hxx>

TopOpeBRep_Bipoint::TopOpeBRep_Bipoint():myI1(0),myI2(0){}
TopOpeBRep_Bipoint::TopOpeBRep_Bipoint
(const Standard_Integer I1,const Standard_Integer I2):myI1(I1),myI2(I2){}
Standard_Integer TopOpeBRep_Bipoint::I1() const {
if(myI1<=0)Standard_ProgramError::Raise("TopOpeBRep_Bipoint I1=0");
return myI1;
}
Standard_Integer TopOpeBRep_Bipoint::I2() const {
if(myI2<=0)Standard_ProgramError::Raise("TopOpeBRep_Bipoint I2=0");
return myI2;
}
