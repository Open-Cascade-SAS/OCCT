// Created on: 1996-06-07
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <GeomConvert_CompBezierSurfacesToBSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColGeom_Array2OfBezierSurface.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::
GeomConvert_CompBezierSurfacesToBSplineSurface(const TColGeom_Array2OfBezierSurface& Beziers)
// ============================================================================
{
  Standard_Integer ii;
  myDone = Standard_True;
  // Choix des noeuds 
  myUKnots = new (TColStd_HArray1OfReal) (1, Beziers.ColLength()+1);
  for (ii=0; ii<myUKnots->Length(); ii++) { myUKnots->SetValue(ii+1, ii); }

  myVKnots = new (TColStd_HArray1OfReal) (1, Beziers.RowLength()+1);
  for (ii=0; ii<myVKnots->Length(); ii++) { myVKnots->SetValue(ii+1, ii); }

  // Calcul des Poles
  Perform(Beziers);
}


// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::
GeomConvert_CompBezierSurfacesToBSplineSurface(
      const TColGeom_Array2OfBezierSurface& Beziers,
      const Standard_Real Tolerance,
      const Standard_Boolean RemoveKnots)
// ============================================================================
{
  Standard_Integer ii, jj, multU=0, multV, minus;
  Standard_Boolean Ok;
  gp_Vec vec;
  Standard_Real V1, V2, V3, Ratio, L1, L2, Tol, val;
  gp_Pnt P1, P2, P3;
  Handle(Geom_Curve) FirstCurve, SecondCurve;

  myDone = Standard_True;

  // Choix des noeuds 

  myUKnots = new (TColStd_HArray1OfReal) (1, Beziers.ColLength()+1);
  myVKnots = new (TColStd_HArray1OfReal) (1, Beziers.RowLength()+1);

  // --> en U
  myUKnots->SetValue(1, 0);
  jj =  myVKnots->Length()/2;
  FirstCurve = Beziers(1, jj)->VIso(0.3);
  FirstCurve->D0(0, P1);
  FirstCurve->D0(0.5, P2);
  FirstCurve->D1(1, P3, vec);

  L1 = P1.Distance(P2) + P2.Distance(P3);
  myUKnots->SetValue(2, L1);

  V1 = vec.Magnitude();
  // si la Parametrisation est trop bizzare on garde la pseudo-longueur
  if ((V1 > 1000 * L1) || (V1 < L1 * 1.e-3))  V1 = L1;
    
  for (ii=2; ii<myUKnots->Length(); ii++) { 

    SecondCurve =  Beziers(ii, jj)->VIso(0.3);
    SecondCurve->D1(0, P1, vec);
    V2 = vec.Magnitude();
    SecondCurve->D0(0.5, P2);
    SecondCurve->D1(1, P3, vec);
    V3 = vec.Magnitude();
    L2 = P1.Distance(P2) + P2.Distance(P3);
    
    // On calcul le ratio, en evitant les cas tordus...
    if ((V2 > 1000 * L2) || (V2 < L2 * 1.e-3))  V2 = L2; 
    if ((V3 > 1000 * L2) || (V3 < L2 * 1.e-3))  V3 = L2;
    
    Ratio = 1;
    if ( (V1 > Precision::Confusion()) && (V2 > Precision::Confusion()) ) {
      Ratio = V2 / V1;
    }
    if ( (Ratio < Precision::Confusion()) 
	|| (Ratio > 1/Precision::Confusion()) ) {Ratio = 1;}
    
    // On en deduit un  nouveau noeud
    val =  myUKnots->Value(ii);
    myUKnots->SetValue(ii+1, val + Ratio*(val- myUKnots->Value(ii-1)) );

    // Et c'est reparti, pour un tour
    L1 = L2;
    V1 = V3;
    FirstCurve = SecondCurve;
  }

  // --> en V
  myVKnots->SetValue(1, 0);
  ii =  myUKnots->Length()/2;
  FirstCurve = Beziers(ii, 1)->UIso(0.3);
  FirstCurve->D0(0, P1);
  FirstCurve->D0(0.5, P2);
  FirstCurve->D1(1, P3, vec);

  L1 = P1.Distance(P2) + P2.Distance(P3);
  myVKnots->SetValue(2, L1);

  V1 = vec.Magnitude();
  // si la Parametrisation est trop bizzare on garde la pseudo-longueur
  if ((V1 > 1000 * L1) || (V1 < L1 * 1.e-3))  V1 = L1;
    
  for (jj=2; jj<myVKnots->Length(); jj++) { 

    SecondCurve =  Beziers(ii, jj)->UIso(0.3);
    SecondCurve->D1(0, P1, vec);
    V2 = vec.Magnitude();
    SecondCurve->D0(0.5, P2);
    SecondCurve->D1(1, P3, vec);
    V3 = vec.Magnitude();
    L2 = P1.Distance(P2) + P2.Distance(P3);
    
    // On calcul le ratio, en evitant les cas tordus...
    if ((V2 > 1000 * L2) || (V2 < L2 * 1.e-3))  V2 = L2; 
    if ((V3 > 1000 * L2) || (V3 < L2 * 1.e-3))  V3 = L2;
    
    Ratio = 1;
    if ( (V1 > Precision::Confusion()) && (V2 > Precision::Confusion()) ) {
      Ratio = V2 / V1;
    }
    if ( (Ratio < Precision::Confusion()) 
	|| (Ratio > 1/Precision::Confusion()) ) {Ratio = 1;}
    
    // On en deduit un  nouveau noeud
    val =  myVKnots->Value(jj);
    myVKnots->SetValue(jj+1, val + Ratio*(val-myVKnots->Value(jj-1)) );

    // Et c'est reparti, pour un tour
    L1 = L2;
    V1 = V3;
    FirstCurve = SecondCurve;
  }

  // Calcul des Poles
  Perform(Beziers);

  // Reduction de la multiplicite
  Handle(Geom_BSplineSurface) Surface = new (Geom_BSplineSurface)
         (myPoles->Array2(),  
          myUKnots->Array1(), 
          myVKnots->Array1(),  
          myUMults->Array1(), 
	  myVMults->Array1(),
          myUDegree,
          myVDegree);

  if (RemoveKnots) minus = 0;
  else             minus = 1;

  for (ii=myUKnots->Length()-1; ii>1; ii--) {
    Ok=Standard_True;
    Tol = Tolerance/2;
    multU = myUMults->Value(ii)-1;
    for  ( ; Ok && multU > minus; multU--, Tol/=2) {
      Ok = Surface->RemoveUKnot(ii, multU, Tol);
    }
  }

  for (ii=myVKnots->Length()-1; ii>1; ii--) {
    Ok=Standard_True;
    Tol = Tolerance/2;
    multV = myVMults->Value(ii)-1;
    for  ( ; Ok && multU > minus; multV--, Tol/=2) {
      Ok = Surface->RemoveVKnot(ii, multV, Tol);
    }
  } 

  // Les nouveaux champs sont arrivees ....
  myPoles = new (TColgp_HArray2OfPnt) (1, Surface->NbUPoles(), 1, Surface->NbVPoles());
  Surface->Poles( myPoles->ChangeArray2());

  myUMults = new (TColStd_HArray1OfInteger) (1, Surface->NbUKnots());
  myVMults = new (TColStd_HArray1OfInteger) (1, Surface->NbVKnots());
  myUKnots = new (TColStd_HArray1OfReal) (1, Surface->NbUKnots());
  myVKnots = new (TColStd_HArray1OfReal) (1, Surface->NbVKnots());
 

  Surface->UMultiplicities( myUMults->ChangeArray1());
  Surface->VMultiplicities( myVMults->ChangeArray1());
  Surface->UKnots( myUKnots->ChangeArray1());
  Surface->VKnots( myVKnots->ChangeArray1());
}

// ============================================================================
GeomConvert_CompBezierSurfacesToBSplineSurface::
GeomConvert_CompBezierSurfacesToBSplineSurface(
				 const TColGeom_Array2OfBezierSurface& Beziers, 
				 const TColStd_Array1OfReal& UKnots, 
				 const TColStd_Array1OfReal& VKnots, 
				 const GeomAbs_Shape UContinuity, 
				 const GeomAbs_Shape VContinuity,
				 const Standard_Real Tolerance)
// ============================================================================
{
  Standard_Integer decu=0, decv=0;
  Standard_Boolean Ok;

  myDone = Standard_True;

  // Recuperation des noeuds 
  myUKnots = new (TColStd_HArray1OfReal) (1, Beziers.ColLength()+1);
  myUKnots->ChangeArray1() =  UKnots;

  myVKnots = new (TColStd_HArray1OfReal) (1, Beziers.RowLength()+1);
  myVKnots->ChangeArray1() = VKnots;

  // Calcul des Poles
  Perform(Beziers);

  // Obtention des bonne continuitee

  switch ( UContinuity ) {
  case GeomAbs_C0 :
    decu = 0;
    break;
  case GeomAbs_C1 :
    decu = 1;
    break;
  case GeomAbs_C2 :
    decu = 2;
    break;
  case GeomAbs_C3 :
    decu = 3;
    break;
  default:
    throw Standard_ConstructionError(
     "GeomConvert_CompBezierSurfacesToBSpl:: UContinuity error");
  }
  
  switch ( VContinuity ) {
  case GeomAbs_C0 :
    decv = 0;
    break;
  case GeomAbs_C1 :
    decv = 1;
    break;
  case GeomAbs_C2 :
    decv = 2;
    break;
  case GeomAbs_C3 :
    decv = 3;
    break;
  default:
    throw Standard_ConstructionError(
     "GeomConvert_CompBezierSurfacesToBSpl:: VContinuity error");
  }

  
  if ( (decu>0) || (decv>0) ) {
 
    Standard_Integer ii;
    Standard_Integer multU = myUDegree - decu;
    Standard_ConstructionError_Raise_if( 
    ((multU <= 0) && (myUKnots->Length()>2)) , 
     "GeomConvert_CompBezierSurfacesToBSpl:: UContinuity or Udeg error");

    Standard_Integer multV = myVDegree - decv;
    Standard_ConstructionError_Raise_if(
    ((multV <= 0) && (myVKnots->Length()>2)) ,
     "GeomConvert_CompBezierSurfacesToBSpl:: VContinuity or Vdeg error");

    Handle(Geom_BSplineSurface) Surface = new (Geom_BSplineSurface)
         (myPoles->Array2(),  
          myUKnots->Array1(), 
          myVKnots->Array1(),  
          myUMults->Array1(), 
	  myVMults->Array1(),
          myUDegree,
          myVDegree); 


  if (decu>0) {
     for (ii=2; ii<myUKnots->Length(); ii++) {
        Ok = Surface->RemoveUKnot(ii, multU, Tolerance);
        if (!Ok) {myDone = Ok;}
      }
   }

  if (decv>0) {
     for (ii=2; ii<myVKnots->Length(); ii++) {
        Ok = Surface->RemoveVKnot(ii, multV, Tolerance);
        if (!Ok) {myDone = Ok;}
      }
   }

  // Les nouveaux champs sont arrivees ....
  myPoles = new (TColgp_HArray2OfPnt) (1, Surface->NbUPoles(), 1, Surface->NbVPoles());
  Surface->Poles( myPoles->ChangeArray2());
  Surface->UMultiplicities( myUMults->ChangeArray1());
  Surface->VMultiplicities( myVMults->ChangeArray1());  
  }
}


// ================================================================================
void GeomConvert_CompBezierSurfacesToBSplineSurface::Perform(
				     const TColGeom_Array2OfBezierSurface& Beziers)
// ================================================================================
{
  Standard_Integer IU, IV;

  // (1) Determination des degrees et si le resultat est rationnel.
  isrational = Standard_False;
  myUDegree = 1;
  myVDegree = 1;

  for (IU=Beziers.LowerRow(); IU <=Beziers.UpperRow(); IU++) {
    for (IV=Beziers.LowerCol(); IV <=Beziers.UpperCol(); IV++) {
      if (   Beziers(IU, IV)-> IsURational() 
          || Beziers(IU, IV)-> IsVRational()) { isrational = Standard_True;}

      myUDegree = ( Beziers(IU, IV)->UDegree() >  myUDegree ) ?
                    Beziers(IU, IV)->UDegree() :  myUDegree;

      myVDegree = ( Beziers(IU, IV)->VDegree() >  myVDegree ) ?
                    Beziers(IU, IV)->VDegree() :  myVDegree; 
    }
  }

  Standard_NotImplemented_Raise_if(isrational, 
            "CompBezierSurfacesToBSpl : rational !");


  // (2) Boucle sur les carreaux  -----------------------------

  Handle(Geom_BezierSurface) Patch;
  Standard_Integer  UIndex,  VIndex,  uindex,  vindex,  udeb,  vdeb;
  Standard_Integer  upol, vpol, ii;


  myPoles = new (TColgp_HArray2OfPnt) 
     ( 1,  (myUDegree+1)*Beziers.ColLength() - myUKnots->Length() + 2 ,
       1,  (myVDegree+1)*Beziers.RowLength() - myVKnots->Length() + 2 );

  for (IU=Beziers.LowerRow(); IU <=Beziers.UpperRow(); IU++) {
    UIndex = (IU-1)*myUDegree + 1;
    for (IV=Beziers.LowerCol(); IV <=Beziers.UpperCol(); IV++) {

      Patch = Beziers(IU, IV);
      VIndex = (IV-1)*myVDegree + 1;
           
    // (2.1) Augmentation du degree
      Patch->Increase(myUDegree, myVDegree);

    // (2.2) Poles a recopier
      if (IU==1) {udeb = 1;}
      else {udeb = 2;}
      if (IV==1) {vdeb = 1;}
      else {vdeb = 2;}

      uindex = UIndex + udeb -1;

      for (upol = udeb; upol <= myUDegree+1; upol++, uindex++ ) {
          vindex =  VIndex + vdeb - 1;
          for (vpol = vdeb; vpol <= myVDegree+1; vpol++, vindex++) {
	      myPoles->ChangeValue(uindex, vindex) =  Patch->Pole(upol, vpol);
	    }
	}

    // (2.3) Poles a sommer
      if (udeb==2) {
         vindex =  VIndex + vdeb - 1;
         for (vpol = vdeb; vpol <= myVDegree+1; vpol++, vindex++) {
	     myPoles->ChangeValue(UIndex, vindex).ChangeCoord() += 
                      Patch->Pole(1, vpol).Coord();
	    }
       }

      if (vdeb==2) {
         uindex =  UIndex + udeb - 1;
         for (upol = udeb; upol <= myUDegree+1; upol++, uindex++) {
	     myPoles->ChangeValue(uindex, VIndex).ChangeCoord() += 
                      Patch->Pole(upol, 1).Coord();
	    }
       }

      if (udeb==2 && vdeb==2) { 
	 myPoles->ChangeValue(UIndex, VIndex).ChangeCoord() += 
                              Patch->Pole(1, 1).Coord();
       }
    }
  }

  // (3) Elimination des redondances
  // car dans la boucle precedente on compte : 
  // - 2 fois les poles associes aux noeuds simples
  // - 4 fois les poles associes aux doubles noeuds (en U et V)

  // (3.1) Elimination en U
    for ( UIndex = myUDegree+1, ii=2; 
	  ii< myUKnots->Length(); 
	  ii++, UIndex+=myUDegree) {
       for (vpol = 1; vpol<=myPoles->UpperCol();  vpol++) {
	  myPoles->ChangeValue(UIndex, vpol).ChangeCoord() *= 0.5;
	}
     }       

  // (3.2) Elimination en V
    for ( VIndex = myVDegree+1, ii=2; 
	  ii< myVKnots->Length(); 
	  ii++, VIndex += myVDegree) {
       for (upol = 1; upol<=myPoles->UpperRow();  upol++) {
	  myPoles->ChangeValue(upol, VIndex).ChangeCoord() *= 0.5;
	}
     }

 // (4) Init des multiplicites
    myUMults = new (TColStd_HArray1OfInteger) (1, myUKnots->Length());
    myUMults->Init( myUDegree);
    myUMults->SetValue(1,                   myUDegree+1);
    myUMults->SetValue( myUMults->Upper(),  myUDegree+1);

    myVMults = new (TColStd_HArray1OfInteger) (1, myVKnots->Length());
    myVMults->Init( myVDegree);
    myVMults->SetValue(1,                  myVDegree+1);
    myVMults->SetValue(myVMults->Upper(),  myVDegree+1);
}

// ========================================================================
Standard_Boolean GeomConvert_CompBezierSurfacesToBSplineSurface::IsDone() const
// ========================================================================
{
 return myDone;
}

