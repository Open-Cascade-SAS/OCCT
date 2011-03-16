// File:	TopOpeBRepDS_Association.cxx
// Created:	Thu Sep  3 09:35:36 1998
// Author:	Yves FRICAUD
//		<yfr@claquox.paris1.matra-dtv.fr>


#include <TopOpeBRepDS_Association.ixx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>


//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

static Standard_Boolean Contains (const TopOpeBRepDS_ListOfInterference& LI,
				  const Handle(TopOpeBRepDS_Interference)& I)
{
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    if (I->HasSameGeometry(it.Value())) return 1;
  }
  return 0;
}


//=======================================================================
//function : TopOpeBRepDS_Association
//purpose  : 
//=======================================================================

TopOpeBRepDS_Association::TopOpeBRepDS_Association()
{
}


//=======================================================================
//function : Associate
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Association::Associate(const Handle(TopOpeBRepDS_Interference)& I,
					 const Handle(TopOpeBRepDS_Interference)& K) 
{
  if (!myMap.IsBound(I)) {
    TopOpeBRepDS_ListOfInterference empty;
    myMap.Bind(I,empty);
    myMap(I).Append(K);
  }
  else if (!Contains(myMap(I),K)) {
    myMap(I).Append(K);
  }
  if (!myMap.IsBound(K)) {
    TopOpeBRepDS_ListOfInterference empty;
    myMap.Bind(K,empty);
    myMap(K).Append(I);
  }
  else if (!Contains(myMap(K),I)) {
    myMap(K).Append(I);
  }
}


//=======================================================================
//function : Associate
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Association::Associate(const Handle(TopOpeBRepDS_Interference)& I,
					 const TopOpeBRepDS_ListOfInterference& LI) 
{  
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    Associate(I,it.Value());
  }
}

//=======================================================================
//function : HasAssociation
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Association::HasAssociation(const Handle(TopOpeBRepDS_Interference)& I) const
{
  return myMap.IsBound(I);
}


//=======================================================================
//function : Associated
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_Association::Associated
(const Handle(TopOpeBRepDS_Interference)& I)
{
  if (myMap.IsBound(I)) {
    return myMap.ChangeFind(I);
  }
  static TopOpeBRepDS_ListOfInterference empty;
  return empty;
}


//=======================================================================
//function : AreAssociated
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Association::AreAssociated(const Handle(TopOpeBRepDS_Interference)& I,
							 const Handle(TopOpeBRepDS_Interference)& K) const
{
  return (myMap.IsBound(I) && Contains(myMap(I),K));
}


