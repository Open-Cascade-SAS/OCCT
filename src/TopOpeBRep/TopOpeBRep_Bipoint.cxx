// Created on: 1997-01-09
// Created by: Jean Yves LEBEY
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


#include <Standard_ProgramError.hxx>
#include <TopOpeBRep_Bipoint.hxx>

TopOpeBRep_Bipoint::TopOpeBRep_Bipoint():myI1(0),myI2(0){}
TopOpeBRep_Bipoint::TopOpeBRep_Bipoint
(const Standard_Integer I1,const Standard_Integer I2):myI1(I1),myI2(I2){}
Standard_Integer TopOpeBRep_Bipoint::I1() const {
if(myI1<=0)throw Standard_ProgramError("TopOpeBRep_Bipoint I1=0");
return myI1;
}
Standard_Integer TopOpeBRep_Bipoint::I2() const {
if(myI2<=0)throw Standard_ProgramError("TopOpeBRep_Bipoint I2=0");
return myI2;
}
