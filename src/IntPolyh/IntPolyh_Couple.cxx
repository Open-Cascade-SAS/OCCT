// Created on: 1999-04-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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


#include <IntPolyh_Couple.hxx>

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




