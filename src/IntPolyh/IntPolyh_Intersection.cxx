// Created on: 1999-03-03
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//  modified by Edward AGAPOV (eap) Tue Jan 22 12:29:55 2002 (occ53)
//  Modified by skv - Thu Sep 25 18:24:29 2003 OCC567

#include <Adaptor3d_HSurface.hxx>
#include <IntPolyh_Couple.hxx>
#include <IntPolyh_Intersection.hxx>
#include <IntPolyh_MaillageAffinage.hxx>
#include <IntPolyh_SectionLine.hxx>
#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Triangle.hxx>
#include <NCollection_Map.hxx>
#include <IntPolyh_CoupleMapHasher.hxx>

Standard_Integer MYDISPLAY = 0;
Standard_Integer MYPRINT   = 0;

IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1,
                                             const Handle(Adaptor3d_HSurface)& S2)
{
  myNbSU1 = -1;
  myNbSV1 = -1;
  myNbSU2 = -1; 
  myNbSV2 = -1; 
  mySurf1 = S1;
  mySurf2 = S2;
  done = Standard_False;
  TSectionLines.Init(1000);
  TTangentZones.Init(10000);
  Perform();
}

IntPolyh_Intersection::IntPolyh_Intersection(const Handle(Adaptor3d_HSurface)& S1,
                                             const Standard_Integer NbSU1,
                                             const Standard_Integer NbSV1,
                                             const Handle(Adaptor3d_HSurface)& S2,
                                             const Standard_Integer NbSU2,
                                             const Standard_Integer NbSV2)
{
  myNbSU1 = NbSU1;
  myNbSV1 = NbSV1;
  myNbSU2 = NbSU2; 
  myNbSV2 = NbSV2; 
  mySurf1 = S1;
  mySurf2 = S2;
  done = Standard_False;
  TSectionLines.Init(1000);
  TTangentZones.Init(10000);
  Perform();
}

void IntPolyh_Intersection::Perform() { 

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

  isStdDone = PerformStd(aPMaillageStd,nbCouplesStd);

  // default interference done well, use it
  if(isStdDone && nbCouplesStd > 10) {
    aPMaillageStd->StartPointsChain(TSectionLines, TTangentZones);
  }
  // default interference done, but too few interferences foud;
  // use advanced interference
  else if(isStdDone && nbCouplesStd <= 10) {
    isAdvDone = PerformAdv(aPMaillageFF,aPMaillageFR,aPMaillageRF,aPMaillageRR,nbCouplesAdv);

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


Standard_Boolean IntPolyh_Intersection::IsDone() const {
  return(done);
}


Standard_Integer IntPolyh_Intersection::NbSectionLines() const { 
  return(nbsectionlines);
}


Standard_Integer IntPolyh_Intersection::NbPointsInLine(const Standard_Integer IndexLine) const { 
  
  return(TSectionLines[IndexLine-1].NbStartPoints());
}


Standard_Integer IntPolyh_Intersection::NbPointsInTangentZone(const Standard_Integer) const {   
  //-- IndexLine--;     (pas implemente) Attention : Tableaux de 0 a n-1 
  // eap
  // return(TTangentZones.NbTangentZones());
  return 1;
}


Standard_Integer IntPolyh_Intersection::NbTangentZones() const { 
  return(nbtangentzones);
}


void IntPolyh_Intersection::GetLinePoint(const Standard_Integer Indexl,
				 const Standard_Integer Indexp,
				 Standard_Real &x,
				 Standard_Real &y,
				 Standard_Real &z,
				 Standard_Real &u1,
				 Standard_Real &v1,
				 Standard_Real &u2,
				 Standard_Real &v2,
				 Standard_Real &incidence) const { 
  const IntPolyh_SectionLine  &msl=TSectionLines[Indexl-1];
  const IntPolyh_StartPoint   &sp=msl[Indexp-1];
  x=sp.X();
  y=sp.Y();
  z=sp.Z();
  u1=sp.U1();
  v1=sp.V1();
  u2=sp.U2();
  v2=sp.V2();
  incidence=sp.GetAngle();
}


void IntPolyh_Intersection::GetTangentZonePoint(const Standard_Integer Indexz,
					const Standard_Integer /*Indexp*/,
					Standard_Real &x,
					Standard_Real &y,
					Standard_Real &z,
					Standard_Real &u1,
					Standard_Real &v1,
					Standard_Real &u2,
					Standard_Real &v2) const { 
  //--   Indexz--;    tableaux C
  // eap
  //const IntPolyh_StartPoint   &sp=TTangentZones[Indexp-1];
  const IntPolyh_StartPoint   &sp=TTangentZones[Indexz-1];
  x=sp.X();
  y=sp.Y();
  z=sp.Z();
  u1=sp.U1();
  v1=sp.V1();
  u2=sp.U2();
  v2=sp.V2();
}

//  Modified by skv - Thu Sep 25 18:07:41 2003 OCC567 Begin
//=======================================================================
//function : PerformMaillage
//purpose  : Computes MaillageAffinage
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage
                 (const Standard_Boolean            isFirstFwd,
		  const Standard_Boolean            isSecondFwd,
		        IntPolyh_PMaillageAffinage &theMaillageS)
{
  if (myNbSU1 == -1)
    theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, mySurf2, MYPRINT);
  else
    theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, myNbSU1, myNbSV1,
						 mySurf2, myNbSU2, myNbSV2,
						 MYPRINT);

  theMaillageS->FillArrayOfPnt(1, isFirstFwd);
  theMaillageS->FillArrayOfPnt(2, isSecondFwd);
  
  
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
  if(FinTTC > 200 &&
     (FinTTC >= theMaillageS->GetArrayOfTriangles(1).NbItems() ||
      FinTTC >= theMaillageS->GetArrayOfTriangles(2).NbItems()) ) {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : PerformMaillage
//purpose  : Computes MaillageAffinage
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformMaillage(IntPolyh_PMaillageAffinage &theMaillageS)
{
  if (myNbSU1 == -1)
    theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, mySurf2, MYPRINT);
  else
    theMaillageS = new IntPolyh_MaillageAffinage(mySurf1, myNbSU1, myNbSV1,
						 mySurf2, myNbSU2, myNbSV2,
						 MYPRINT);

  theMaillageS->FillArrayOfPnt(1);
  theMaillageS->FillArrayOfPnt(2);
  
  
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

  return Standard_True;
}

//=======================================================================
//function : MergeCouples
//purpose  : This method analyzes the lists to find same couples.
//           If some are detected it leaves the couple in only one list
//           deleting from others.
//=======================================================================

void IntPolyh_Intersection::MergeCouples(IntPolyh_ListOfCouples &anArrayFF,
                                         IntPolyh_ListOfCouples &anArrayFR,
                                         IntPolyh_ListOfCouples &anArrayRF,
                                         IntPolyh_ListOfCouples &anArrayRR) const
{
  // Fence map to remove from the lists the duplicating elements.
  NCollection_Map<IntPolyh_Couple, IntPolyh_CoupleMapHasher> aFenceMap;
  //
  IntPolyh_ListOfCouples* pLists[4] = {&anArrayFF, &anArrayFR, &anArrayRF, &anArrayRR};
  for (Standard_Integer i = 0; i < 4; ++i) {
    IntPolyh_ListIteratorOfListOfCouples aIt(*pLists[i]);
    for (; aIt.More();) {
      if (!aFenceMap.Add(aIt.Value())) {
        pLists[i]->Remove(aIt);
        continue;
      }
      aIt.Next();
    }
  }
}

//=======================================================================
//function : PerformStd
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformStd(IntPolyh_PMaillageAffinage& MaillageS,
						   Standard_Integer&           NbCouples)
{
  Standard_Boolean isdone = PerformMaillage(MaillageS);
  NbCouples = (isdone) ? (MaillageS->GetCouples().Extent()) : 0;
  return isdone;
}

//=======================================================================
//function : PerformAdv
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Intersection::PerformAdv(IntPolyh_PMaillageAffinage& MaillageFF,
						   IntPolyh_PMaillageAffinage& MaillageFR,
						   IntPolyh_PMaillageAffinage& MaillageRF,
						   IntPolyh_PMaillageAffinage& MaillageRR,
						   Standard_Integer&           NbCouples)
{
  Standard_Boolean isdone = Standard_True;
  NbCouples = 0;

  if(!PerformMaillage(Standard_True,Standard_False,MaillageFR) ||
     !PerformMaillage(Standard_False,Standard_True,MaillageRF) ||
     !PerformMaillage(Standard_True,Standard_True,MaillageFF)  ||
     !PerformMaillage(Standard_False,Standard_False,MaillageRR) )
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
