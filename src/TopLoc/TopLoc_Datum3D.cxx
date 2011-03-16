// File:	TopLoc_Datum3D.cxx
// Created:	Fri Jan 25 11:15:48 1991
// Author:	Christophe MARION
//		<cma@topsn3>

#include <Standard_Stream.hxx>
#include <TopLoc_Datum3D.ixx>

//=======================================================================
//function : TopLoc_Datum3D
//purpose  : Identity
//=======================================================================

TopLoc_Datum3D::TopLoc_Datum3D () 
{
}

//=======================================================================
//function : TopLoc_Datum3D
//purpose  : 
//=======================================================================

TopLoc_Datum3D::TopLoc_Datum3D (const gp_Trsf& T) :
 myTrsf(T)
{
}

//=======================================================================
//function : ShallowDump
//purpose  : 
//=======================================================================

void  TopLoc_Datum3D::ShallowDump(Standard_OStream& S) const 
{
  S << " TopLoc_Datum3D " << (void*)this << endl;
  Standard_Integer i;
  gp_Trsf T = myTrsf;
  for (i = 1; i<=3; i++) {
    S<<"  ( "<<setw(10)<<T.Value(i,1);
    S<<","<<setw(10)<<T.Value(i,2);
    S<<","<<setw(10)<<T.Value(i,3);
    S<<","<<setw(10)<<T.Value(i,4);
    S<<")\n";
  }
  S << endl;
}
    
