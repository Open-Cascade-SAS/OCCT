// File:	TopOpeBRepDS_PointData.cxx
// Created:	Wed Jun 23 19:39:16 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

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
