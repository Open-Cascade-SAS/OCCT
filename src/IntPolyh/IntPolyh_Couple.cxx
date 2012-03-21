// Created on: 1999-04-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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



#include <IntPolyh_Couple.ixx>
#include <stdio.h>

IntPolyh_Couple::IntPolyh_Couple() : t1(-1),t2(-1),ia(0),angle(-2.0) {  } 

IntPolyh_Couple::IntPolyh_Couple(const Standard_Integer a,const Standard_Integer b) : t1(a),t2(b),ia(0),angle(-2.0) {  }

Standard_Integer IntPolyh_Couple::FirstValue()  const{ return(t1);}
Standard_Integer IntPolyh_Couple::SecondValue() const{ return(t2);}
Standard_Integer IntPolyh_Couple::AnalyseFlagValue() const{ return(ia);}
Standard_Real IntPolyh_Couple::AngleValue() const{ return(angle);}

void IntPolyh_Couple::SetCoupleValue(const Standard_Integer a,const Standard_Integer b) 
{ t1=a; t2=b; } 
void IntPolyh_Couple::SetAnalyseFlag(const Standard_Integer iiaa) { ia=iiaa; }    
void IntPolyh_Couple::SetAngleValue(const Standard_Real ang) { angle=ang; }    

void IntPolyh_Couple::Dump(const Standard_Integer i) const{ 
  printf("\nCouple(%3d) : %5d %5d %3d %5f\n",i,t1,t2,ia,angle);
}




