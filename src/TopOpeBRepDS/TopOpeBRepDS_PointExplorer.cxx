// File:	TopOpeBRepDS_PointExplorer.cxx
// Created:	Fri Dec  8 19:38:41 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <TopOpeBRepDS_PointExplorer.ixx>
#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))

//=======================================================================
//function : TopOpeBRepDS_PointExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_PointExplorer::TopOpeBRepDS_PointExplorer() 
: myIndex(1),
  myMax(0),
  myDS(NULL),
  myFound(Standard_False),
  myFindKeep(Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_PointExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_PointExplorer::TopOpeBRepDS_PointExplorer
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{ 
  Init(DS,FindKeep);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Init
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{
  myIndex = 1; 
  myMax = DS.NbPoints();
  myDS = (TopOpeBRepDS_DataStructure*)&DS;
  myFindKeep = FindKeep;
  Find();
}


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax) {
    if (myFindKeep) {
      myFound = IsPointKeep(myIndex);
    }
    else {
      myFound = IsPoint(myIndex);
    }
    if (myFound) break;
    else myIndex++;
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::More() const
{
  return myFound;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_PointExplorer::Next()
{
  myIndex++;
  Find();
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Point& TopOpeBRepDS_PointExplorer::Point()const
{
  if ( myFound ) {
    return MYDS.Point(myIndex);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : IsPoint
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::IsPoint
(const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myPoints.IsBound(I);
  return b;
}

//=======================================================================
//function : IsPointKeep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_PointExplorer::IsPointKeep
(const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myPoints.IsBound(I);
  if (b) b = MYDS.Point(I).Keep();
  return b;
}


//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Point& TopOpeBRepDS_PointExplorer::Point
   (const Standard_Integer I)const
{
  if ( IsPoint(I) ) {
    return MYDS.Point(I);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : NbPoint
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_PointExplorer::NbPoint()
{
  myIndex = 1; myMax = MYDS.NbPoints();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next() ) n++;
  return n;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_PointExplorer::Index()const
{
  return myIndex;
}
