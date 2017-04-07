// Created on: 2005-09-26
// Created by: Igor FEOKTISTOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Adaptor3d_HSurface.hxx>
#include <IntPolyh_ListOfCouples.hxx>
#include <IntPolyh_Couple.hxx>
#include <IntPolyh_Intersection.hxx>
#include <IntPolyh_MaillageAffinage.hxx>
#include <IntPolyh_PMaillageAffinage.hxx>
#include <IntPolyh_Triangle.hxx>

Standard_Integer MYPRINT1   = 0;


//=======================================================================
//function : IntPolyh_Intersection
//purpose  : 
//=======================================================================
IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1,
                                             const TColStd_Array1OfReal& Upars1,
                                             const TColStd_Array1OfReal& Vpars1,
                                             const Handle(Adaptor3d_HSurface)& S2,
                                             const TColStd_Array1OfReal& Upars2,
                                             const TColStd_Array1OfReal& Vpars2)
{
  myNbSU1 = Upars1.Length();
  myNbSV1 = Vpars1.Length();
  myNbSU2 = Upars2.Length(); 
  myNbSV2 = Vpars2.Length(); 
  mySurf1 = S1;
  mySurf2 = S2;
  done = Standard_False;
  TSectionLines.Init(1000);
  TTangentZones.Init(10000);
  Perform(Upars1, Vpars1, Upars2, Vpars2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void IntPolyh_Intersection::Perform(const TColStd_Array1OfReal& Upars1,
				    const TColStd_Array1OfReal& Vpars1,
				    const TColStd_Array1OfReal& Upars2,
				    const TColStd_Array1OfReal& Vpars2) { 

  done = Standard_True;

  Standard_Boolean isStdDone = Standard_False;
  Standard_Boolean isAdvDone = Standard_False;
  Standard_Integer nbCouplesStd = 0;
  Standard_Integer nbCouplesAdv = 0;
  
  
  IntPolyh_PMaillageAffinage aPMaillageStd = 0;
  IntPolyh_PMaillageAffinage aPMaillageFF = 0;
  IntPolyh_PMaillageAffinage aPMaillageFR = 0;
  IntPolyh_PMaillageAffinage aPMaillageRF = 0;
  IntPolyh_PMaillageAffinage aPMaillageRR = 0;

  isStdDone = PerformStd( Upars1, Vpars1, Upars2, Vpars2, 
                          aPMaillageStd,nbCouplesStd);

  // default interference done well, use it
  if(isStdDone && nbCouplesStd > 10) {
    aPMaillageStd->StartPointsChain(TSectionLines, TTangentZones);
  }
  // default interference done, but too few interferences foud;
  // use advanced interference
  else if(isStdDone && nbCouplesStd <= 10) {
    isAdvDone = PerformAdv( Upars1, Vpars1, Upars2, Vpars2,
                            aPMaillageFF,aPMaillageFR,aPMaillageRF,
                            aPMaillageRR,nbCouplesAdv);
      
    // advanced interference found
    if(isAdvDone && nbCouplesAdv > 0) {
      aPMaillageFF->StartPointsChain(TSectionLines,TTangentZones);
      aPMaillageFR->StartPointsChain(TSectionLines,TTangentZones);
      aPMaillageRF->StartPointsChain(TSectionLines,TTangentZones);
      aPMaillageRR->StartPointsChain(TSectionLines,TTangentZones);
    }
    else {
      // use result of default
      if(nbCouplesStd > 0)
        aPMaillageStd->StartPointsChain(TSectionLines, TTangentZones);
    }
  }
  // default interference faild, use advanced
  else {
//       isAdvDone = PerformAdv(aPMaillageFF,aPMaillageFR,aPMaillageRF,aPMaillageRR,nbCouplesAdv);
      
//       if(isAdvDone && nbCouplesAdv > 0) {cout << "4adv done, nbc: " << nbCouplesAdv << endl;
// 	aPMaillageFF->StartPointsChain(TSectionLines,TTangentZones);
// 	aPMaillageFR->StartPointsChain(TSectionLines,TTangentZones);
// 	aPMaillageRF->StartPointsChain(TSectionLines,TTangentZones);
// 	aPMaillageRR->StartPointsChain(TSectionLines,TTangentZones);
//       }
  }

  // accept result
  nbsectionlines = TSectionLines.NbItems();
  nbtangentzones = TTangentZones.NbItems();

  // clean up
  if(aPMaillageStd) delete aPMaillageStd;
  if(aPMaillageFF) delete aPMaillageFF;
  if(aPMaillageFR) delete aPMaillageFR;
  if(aPMaillageRF) delete aPMaillageRF;
  if(aPMaillageRR) delete aPMaillageRR;

  // verify
  if(!isStdDone && !isAdvDone)
    done = Standard_False;
}

//=======================================================================
//function : PerformMaillage
//purpose  : Computes MaillageAffinage
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage(const Standard_Boolean isFirstFwd,
							const Standard_Boolean isSecondFwd,
							const TColStd_Array1OfReal& Upars1,
							const TColStd_Array1OfReal& Vpars1,
							const TColStd_Array1OfReal& Upars2,
							const TColStd_Array1OfReal& Vpars2,
							IntPolyh_PMaillageAffinage &theMaillageS)
{
  theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, Upars1.Length(), Vpars1.Length(),
					       mySurf2, Upars2.Length(), Vpars2.Length(),
					       MYPRINT1);
  
  
  theMaillageS->FillArrayOfPnt(1, isFirstFwd, Upars1, Vpars1);
  theMaillageS->FillArrayOfPnt(2, isSecondFwd, Upars2, Vpars2);
  
  
  
  Standard_Real xx0,yy0,zz0,xx1,yy1,zz1;
  theMaillageS->CommonBox(theMaillageS->GetBox(1), theMaillageS->GetBox(2),
			  xx0, yy0, zz0, xx1, yy1, zz1);
  
  theMaillageS->FillArrayOfTriangles(1);
  theMaillageS->FillArrayOfTriangles(2);

  theMaillageS->FillArrayOfEdges(1);
  theMaillageS->FillArrayOfEdges(2);

  theMaillageS->TrianglesDeflectionsRefinementBSB();

  Standard_Integer FinTTC = theMaillageS->TriangleCompare();

  // if too many intersections, consider surfaces parallel (eap)
  // test for parallel surf
  if(FinTTC > 200) {
    const Standard_Real eps = .996; //~ cos of 5deg.
    IntPolyh_ListOfCouples& Couples = theMaillageS->GetCouples();

    Standard_Integer npara = 0;
    IntPolyh_ListIteratorOfListOfCouples aIt(Couples);
    for(; aIt.More(); aIt.Next()) {
      Standard_Real cosa = Abs(aIt.Value().Angle());
      if(cosa > eps) ++npara;
    }
    if(npara >= theMaillageS->GetArrayOfTriangles(1).NbItems() ||
       npara >= theMaillageS->GetArrayOfTriangles(2).NbItems() ) {
      return Standard_False;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : PerformMaillage
//purpose  : Computes MaillageAffinage
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage(const TColStd_Array1OfReal& Upars1,
							const TColStd_Array1OfReal& Vpars1,
							const TColStd_Array1OfReal& Upars2,
							const TColStd_Array1OfReal& Vpars2,
							IntPolyh_PMaillageAffinage &theMaillageS)
{
    
  theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, Upars1.Length(), Vpars1.Length(),
					       mySurf2, Upars2.Length(), Vpars2.Length(),
					       MYPRINT1);
  
  theMaillageS->FillArrayOfPnt(1, Upars1, Vpars1);
  theMaillageS->FillArrayOfPnt(2, Upars2, Vpars2);
  
  
  Standard_Real xx0,yy0,zz0,xx1,yy1,zz1;
  theMaillageS->CommonBox(theMaillageS->GetBox(1), theMaillageS->GetBox(2),
			  xx0, yy0, zz0, xx1, yy1, zz1);

  theMaillageS->FillArrayOfTriangles(1);
  theMaillageS->FillArrayOfTriangles(2);

  theMaillageS->FillArrayOfEdges(1);
  theMaillageS->FillArrayOfEdges(2);

  theMaillageS->TrianglesDeflectionsRefinementBSB();

  Standard_Integer FinTTC = theMaillageS->TriangleCompare();

  if( FinTTC == 0 ) {
    Standard_Boolean myZone = Standard_True;
    theMaillageS->SetEnlargeZone( myZone );
    theMaillageS->FillArrayOfPnt(1);
    theMaillageS->FillArrayOfPnt(2);
    theMaillageS->CommonBox(theMaillageS->GetBox(1), theMaillageS->GetBox(2),
			    xx0, yy0, zz0, xx1, yy1, zz1);
    theMaillageS->FillArrayOfTriangles(1);
    theMaillageS->FillArrayOfTriangles(2);
    theMaillageS->FillArrayOfEdges(1);
    theMaillageS->FillArrayOfEdges(2);
    theMaillageS->TrianglesDeflectionsRefinementBSB();
    FinTTC = theMaillageS->TriangleCompare();
    myZone = Standard_False;
    theMaillageS->SetEnlargeZone( myZone );
  }

  // if too many intersections, consider surfaces parallel (eap)
//IFV test for parallel surf
  if(FinTTC > 200) {
    const Standard_Real eps = .996; //~ cos of 5deg.
    IntPolyh_ListOfCouples& Couples = theMaillageS->GetCouples();

    Standard_Integer npara = 0;
    IntPolyh_ListIteratorOfListOfCouples aIt(Couples);
    for(; aIt.More(); aIt.Next()) {
      Standard_Real cosa = Abs(aIt.Value().Angle());
      if(cosa > eps) ++npara;
    }
    if(npara >= theMaillageS->GetArrayOfTriangles(1).NbItems() ||
       npara >= theMaillageS->GetArrayOfTriangles(2).NbItems() ) {
      return Standard_False;
    }
  }
  
  return Standard_True;
}

//=======================================================================
//function : PerformAdv
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformAdv(const TColStd_Array1OfReal& Upars1,
						   const TColStd_Array1OfReal& Vpars1,
						   const TColStd_Array1OfReal& Upars2,
						   const TColStd_Array1OfReal& Vpars2,
						   IntPolyh_PMaillageAffinage& MaillageFF,
						   IntPolyh_PMaillageAffinage& MaillageFR,
						   IntPolyh_PMaillageAffinage& MaillageRF,
						   IntPolyh_PMaillageAffinage& MaillageRR,
						   Standard_Integer&           NbCouples)
{
  Standard_Boolean isdone = Standard_True;
  NbCouples = 0;

  if(!PerformMaillage(Standard_True,Standard_False,
		      Upars1, Vpars1, Upars2, Vpars2, 
		      MaillageFR) ||
     !PerformMaillage(Standard_False,Standard_True,
		      Upars1, Vpars1, Upars2, Vpars2, 
		      MaillageRF) ||
     !PerformMaillage(Standard_True,Standard_True,
		      Upars1, Vpars1, Upars2, Vpars2, 
		      MaillageFF)  ||
     !PerformMaillage(Standard_False,Standard_False,
		      Upars1, Vpars1, Upars2, Vpars2, 
		      MaillageRR) )
    isdone = Standard_False; 

  if(isdone) {
    NbCouples = MaillageFF->GetCouples().Extent() +
      MaillageFR->GetCouples().Extent() +
	MaillageRF->GetCouples().Extent() +
	  MaillageRR->GetCouples().Extent();

    if(NbCouples > 0)
      MergeCouples(MaillageFF->GetCouples(),MaillageFR->GetCouples(),
		   MaillageRF->GetCouples(),MaillageRR->GetCouples());
  }
  return isdone;
}
//=======================================================================
//function : PerformStd
//purpose  : 
//=======================================================================

Standard_Boolean IntPolyh_Intersection::PerformStd(const TColStd_Array1OfReal& Upars1,
						   const TColStd_Array1OfReal& Vpars1,
						   const TColStd_Array1OfReal& Upars2,
						   const TColStd_Array1OfReal& Vpars2,
						   IntPolyh_PMaillageAffinage& MaillageS,
						   Standard_Integer&           NbCouples)
{
  Standard_Boolean isdone = PerformMaillage(Upars1, Vpars1, Upars2, Vpars2, 
					    MaillageS);
  NbCouples = (isdone) ? (MaillageS->GetCouples().Extent()) : 0;
  return isdone;
}
