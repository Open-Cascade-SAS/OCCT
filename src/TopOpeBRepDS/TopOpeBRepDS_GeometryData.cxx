// File:	TopOpeBRepDS_GeometryData.cxx
// Created:	Thu May 26 15:55:07 1994
// Author:	Jean Yves LEBEY
//		<jyl@fuegox>

#include <TopOpeBRepDS_GeometryData.ixx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>

//=======================================================================
//function : TopOpeBRepDS_GeometryData
//purpose  : 
//=======================================================================

TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData()
{
}

//modified by NIZNHY-PKV Tue Oct 30 09:25:59 2001 f
//=======================================================================
//function : TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData
//purpose  : 
//=======================================================================
TopOpeBRepDS_GeometryData::TopOpeBRepDS_GeometryData(const TopOpeBRepDS_GeometryData& Other)
{
  Assign(Other);
}
//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================
void TopOpeBRepDS_GeometryData::Assign(const TopOpeBRepDS_GeometryData& Other)
{
  myInterferences.Clear();

  TopOpeBRepDS_ListIteratorOfListOfInterference anIt(Other.myInterferences);
  for (; anIt.More(); anIt.Next()) {
    myInterferences.Append(anIt.Value());
  }
}
//modified by NIZNHY-PKV Tue Oct 30 09:25:49 2001 t

//=======================================================================
//function : Interferences
//purpose  : 
//=======================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GeometryData::Interferences() const 
{
  return myInterferences;
}

//=======================================================================
//function : ChangeInterferences
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_GeometryData::ChangeInterferences() 
{
  return myInterferences;
}

//=======================================================================
//function : AddInterference
//purpose  : 
//=======================================================================

void TopOpeBRepDS_GeometryData::AddInterference(const Handle(TopOpeBRepDS_Interference)& I)
{
  myInterferences.Append(I);
}
