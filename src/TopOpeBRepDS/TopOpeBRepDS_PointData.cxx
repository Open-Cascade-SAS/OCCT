// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRepDS_PointData.ixx>
#include <TopOpeBRepDS_define.hxx>

//=======================================================================
//function : TopOpeBRepDS_PointData
//purpose  : 
//=======================================================================
TopOpeBRepDS_PointData::TopOpeBRepDS_PointData()
: myS1(0),myS2(0)
{}

//=======================================================================
//function : TopOpeBRepDS_PointData
//purpose  : 
//=======================================================================
TopOpeBRepDS_PointData::TopOpeBRepDS_PointData(const TopOpeBRepDS_Point& P) 
: myPoint(P),myS1(0),myS2(0)
{}

//=======================================================================
//function : TopOpeBRepDS_PointData
//purpose  : 
//=======================================================================

TopOpeBRepDS_PointData::TopOpeBRepDS_PointData(const TopOpeBRepDS_Point& P,
					       const Standard_Integer I1,const Standard_Integer I2)
: myPoint(P),myS1(I1),myS2(I2)
{}

//=======================================================================
//function : SetShapes
//purpose  : 
//=======================================================================
void TopOpeBRepDS_PointData::SetShapes(const Standard_Integer I1,const Standard_Integer I2)
{
  myS1 = I1;myS2 = I2;
}

//=======================================================================
//function : GetShapes
//purpose  : 
//=======================================================================
void TopOpeBRepDS_PointData::GetShapes(Standard_Integer& I1,Standard_Integer& I2) const 
{
  I1 = myS1;I2 = myS2;
}
