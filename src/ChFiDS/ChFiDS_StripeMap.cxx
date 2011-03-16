// File:	ChFiDS_StripeMap.cxx
// Created:	Thu Nov 18 11:51:01 1993
// Author:	Isabelle GRIGNON
//		<isg@zerox>


#include <ChFiDS_StripeMap.ixx>

//=======================================================================
//function : ChFiDS_StripeMap
//purpose  : 
//=======================================================================

ChFiDS_StripeMap::ChFiDS_StripeMap() 
{
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  ChFiDS_StripeMap::Add(const TopoDS_Vertex& V, 
			       const Handle(ChFiDS_Stripe)& F)
{
  Standard_Integer Index = mymap.FindIndex(V);
  if (Index==0) {
    ChFiDS_ListOfStripe Empty;
    Index = mymap.Add(V,Empty);
  }
  mymap(Index).Append(F);
}



//=======================================================================
//function : FindFromKey
//purpose  : 
//=======================================================================

const ChFiDS_ListOfStripe&  ChFiDS_StripeMap::FindFromKey(
						  const TopoDS_Vertex& V)const 
{
  return mymap.FindFromKey(V);
}



//=======================================================================
//function : FindFromIndex
//purpose  : 
//=======================================================================

const ChFiDS_ListOfStripe&  ChFiDS_StripeMap::
                               FindFromIndex(const Standard_Integer I)const 
{
  return mymap.FindFromIndex(I);
}

void ChFiDS_StripeMap::Clear()
{
  mymap.Clear();
}
