// File:	TopOpeBRepDS_ShapeData.cxx
// Created:	Mon Nov 13 12:56:30 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepDS_ShapeData.ixx>

//=======================================================================
//function : TopOpeBRepDS_ShapeData
//purpose  : 
//=======================================================================

TopOpeBRepDS_ShapeData::TopOpeBRepDS_ShapeData() :
  mySameDomainRef(0),
  mySameDomainOri(TopOpeBRepDS_UNSHGEOMETRY),
  mySameDomainInd(0),
  myOrientation(TopAbs_FORWARD),
  myOrientationDef(Standard_False),
  myAncestorRank(0),
  myKeep(Standard_True)
{
}

//=======================================================================
//function : Interferences
//purpose  : 
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_ShapeData::Interferences() const
{
  return myInterferences;
}

//=======================================================================
//function : ChangeInterferences
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_ShapeData::ChangeInterferences()
{
  return myInterferences;
}

//=======================================================================
//function : Keep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_ShapeData::Keep() const
{
  return myKeep;
}
//=======================================================================
//function : ChangeKeep
//purpose  : 
//=======================================================================

void TopOpeBRepDS_ShapeData::ChangeKeep(const Standard_Boolean b)
{
  myKeep = b;
}
