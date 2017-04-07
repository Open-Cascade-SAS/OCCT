// Created on: 1999-03-08
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


#include <Adaptor3d_HSurface.hxx>
#include <IntPolyh_Point.hxx>

#include <stdio.h>

//=======================================================================
//function : Middle
//purpose  : 
//=======================================================================
void IntPolyh_Point::Middle(const Handle(Adaptor3d_HSurface)& MySurface,
				      const IntPolyh_Point & Point1, 
				      const IntPolyh_Point & Point2){
  myU = (Point1.U()+Point2.U())*0.5;
  myV = (Point1.V()+Point2.V())*0.5;
  
  gp_Pnt PtXYZ = (MySurface)->Value(myU, myV);

  myX=PtXYZ.X();
  myY=PtXYZ.Y(); 
  myZ=PtXYZ.Z();
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Add(const IntPolyh_Point &P1)const
{
  IntPolyh_Point res;
  //
  res.SetX(myX+P1.X());
  res.SetY(myY+P1.Y());
  res.SetZ(myZ+P1.Z());
  res.SetU(myU+P1.U());
  res.SetV(myV+P1.V());
  return res;
}  

//=======================================================================
//function : Sub
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Sub(const IntPolyh_Point &P1)const
{ 
  IntPolyh_Point res;
  //
  res.SetX(myX-P1.X());
  res.SetY(myY-P1.Y());
  res.SetZ(myZ-P1.Z());
  res.SetU(myU-P1.U());
  res.SetV(myV-P1.V());
  return res;
} 
//=======================================================================
//function : Divide
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Divide(const Standard_Real RR)const
{ 
  IntPolyh_Point res;
  //
  if (Abs(RR)>10.0e-20) {
    res.SetX(myX/RR);
    res.SetY(myY/RR);
    res.SetZ(myZ/RR);
    res.SetU(myU/RR);
    res.SetV(myV/RR);
  }
  else { 
   printf("Division par zero RR=%f\n",RR);
  }
  return res;
}
//=======================================================================
//function : Multiplication
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Multiplication(const Standard_Real RR)const
{ 
  IntPolyh_Point res;
  //
  res.SetX(myX*RR);
  res.SetY(myY*RR);
  res.SetZ(myZ*RR);
  res.SetU(myU*RR);
  res.SetV(myV*RR);
  return res;
}
//=======================================================================
//function : SquareModulus
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::SquareModulus()const
{
  Standard_Real res=myX*myX+myY*myY+myZ*myZ;
  return res;
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::SquareDistance(const IntPolyh_Point &P2)const
{
  Standard_Real res=(myX-P2.myX)*(myX-P2.myX)+(myY-P2.myY)*(myY-P2.myY)+(myZ-P2.myZ)*(myZ-P2.myZ);
  return res;
}
//=======================================================================
//function : Dot
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::Dot(const IntPolyh_Point &b ) const
{ 
  Standard_Real t=myX*b.myX+myY*b.myY+myZ*b.myZ;
  return t;
}
//=======================================================================
//function : Cross
//purpose  : 
//=======================================================================
void IntPolyh_Point::Cross(const IntPolyh_Point &a,const IntPolyh_Point &b){ 
  myX=a.myY*b.myZ-a.myZ*b.myY;
  myY=a.myZ*b.myX-a.myX*b.myZ;
  myZ=a.myX*b.myY-a.myY*b.myX;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void IntPolyh_Point::Dump() const
{ 
  printf("\nPoint : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg\n",myX,myY,myZ,myU,myV);
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void IntPolyh_Point::Dump(const Standard_Integer i) const
{ 
  printf("\nPoint(%3d) : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg poc=%3d\n",
	 i,myX,myY,myZ,myU,myV,myPOC);
}
