// File:	TopOpeBRepDS_SurfaceExplorer.cxx
// Created:	Thu Oct 17 13:31:32 1996
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#include <TopOpeBRepDS_SurfaceExplorer.ixx>
#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))

//=======================================================================
//function : TopOpeBRepDS_SurfaceExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer() 
: myIndex(1),myMax(0),myDS(NULL),myFound(Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_SurfaceExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceExplorer::TopOpeBRepDS_SurfaceExplorer
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{ 
  Init(DS,FindKeep);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Init
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{
  myIndex = 1; 
  myMax = DS.NbSurfaces();
  myDS = (TopOpeBRepDS_DataStructure*)&DS;
  myFindKeep = FindKeep;
  Find();
}


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax) {
    if (myFindKeep) {
      myFound = IsSurfaceKeep(myIndex);
    }
    else {
      myFound = IsSurface(myIndex);
    }
    if (myFound) break;
    else myIndex++;
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::More() const
{
  return myFound;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_SurfaceExplorer::Next()
{
  myIndex++;
  Find();
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface()const
{
  if ( myFound ) {
    return MYDS.Surface(myIndex);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : IsSurface
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::IsSurface
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.mySurfaces.IsBound(I);
  return b;
}

//=======================================================================
//function : IsSurfaceKeep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_SurfaceExplorer::IsSurfaceKeep
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.mySurfaces.IsBound(I);
  if (b) b = MYDS.Surface(I).Keep();
  return b;
}

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Surface& TopOpeBRepDS_SurfaceExplorer::Surface
   (const Standard_Integer I)const
{
  if ( IsSurface(I) ) {
    return MYDS.Surface(I);
  }
  else {
    return myEmpty;
  }
}

//=======================================================================
//function : NbSurface
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_SurfaceExplorer::NbSurface()
{
  myIndex = 1; myMax = MYDS.NbSurfaces();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next() ) n++;
  return n;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_SurfaceExplorer::Index()const
{
  return myIndex;
}
