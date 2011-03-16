// File:	HatchGen_Domain.cxx
// Created:	Wed Nov 10 15:33:27 1993
// Author:	Jean Marc LACHAUME
//		<jml@sdsun1>

#include <Standard_Stream.hxx>
#include <HatchGen_Domain.ixx>

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain () :
       myHasFirstPoint  (Standard_False) ,
       myHasSecondPoint (Standard_False)
{
}

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain (const HatchGen_PointOnHatching& P1,
				  const HatchGen_PointOnHatching& P2) :
       myHasFirstPoint  (Standard_True) ,
       myFirstPoint     (P1),
       myHasSecondPoint (Standard_True) ,
       mySecondPoint    (P2)
{
}

//=======================================================================
// Function : HatchGen_Domain
// Purpose  : Constructor.
//=======================================================================

HatchGen_Domain::HatchGen_Domain (const HatchGen_PointOnHatching& P,
				  const Standard_Boolean First)
{
  if (First) {
    myHasFirstPoint  = Standard_True ;
    myHasSecondPoint = Standard_False ;
    myFirstPoint     = P ;
  } else {
    myHasFirstPoint  = Standard_False ;
    myHasSecondPoint = Standard_True ;
    mySecondPoint    = P ;
  }
}


//=======================================================================
// Function : Dump
// Purpose  : Dump of the domain.
//=======================================================================

void HatchGen_Domain::Dump (const Standard_Integer Index) const
{
  cout << "=== Domain " ;
  if (Index > 0) {
    cout << "# " << setw(3) << Index << " " ;
  } else {
    cout << "======" ;
  }
  cout << "=============================" << endl ;

  if (myHasFirstPoint) {
    myFirstPoint.Dump (1) ;
  } else {
    cout << "    Has not a first point" << endl ;
  }

  if (myHasSecondPoint) {
    mySecondPoint.Dump (2) ;
  } else {
    cout << "    Has not a second point" << endl ;
  }

  cout << "==============================================" << endl ;
}
