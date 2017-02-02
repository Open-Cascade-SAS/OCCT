// Created on: 1997-05-05
// Created by: Jerome LEMONIER
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

// Modification de l'interface
// Amelioration de l'aglo de remplissage
// 29-09-97 ; jct; correction du calcul des points doublons (tol 2d et non 3d)
// 16-10-97 ; jct; on remet un Raise au lieu du cout sous debug (sinon plantage dans k1fab)
// 03-11-97 ; jct; plus de reference a BRepAdaptor et BRepTools

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Approx_CurveOnSurface.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GeomPlate_HArray1OfSequenceOfReal.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_SequenceOfAij.hxx>
#include <GeomPlate_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Law_Interpol.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_FreeGtoCConstraint.hxx>
#include <Plate_GtoCConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_Plate.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_HCompProjectedCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColgp_SequenceOfVec.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>

#include <stdio.h>
// pour la verif G2
// pour les intersection entre les courbes
// projection
#ifdef DRAW
#include <DrawTrSurf.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw.hxx>
// 0 : Pas de display
// 1 : Display des Geometries et controle intermediaire
// 2 : Display du nombre de contrainte par courbe + Intersection
// 3 : Dump des contraintes dans Plate
static Standard_Integer NbPlan = 0;
//static Standard_Integer NbCurv2d = 0;
static Standard_Integer NbMark = 0;
static Standard_Integer NbProj = 0;
#endif

// pour mes tests
#ifdef OCCT_DEBUG
#include <OSD_Chronometer.hxx>
#include <Geom_Surface.hxx>
static Standard_Integer Affich=0;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//      =========================================================
//                   C O N S T R U C T E U R S
//      =========================================================
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

//---------------------------------------------------------
//    Constructeur compatible avec l'ancienne version
//---------------------------------------------------------
GeomPlate_BuildPlateSurface::GeomPlate_BuildPlateSurface ( 
		    const Handle(TColStd_HArray1OfInteger)& NPoints,
		    const Handle(GeomPlate_HArray1OfHCurve)& TabCurve,
		    const Handle(TColStd_HArray1OfInteger)& Tang,
		    const Standard_Integer Degree,
		    const Standard_Integer NbIter,
		    const Standard_Real Tol2d,
		    const Standard_Real Tol3d,
		    const Standard_Real TolAng,
		    const Standard_Real ,
		    const Standard_Boolean Anisotropie
) :
myAnisotropie(Anisotropie),
myDegree(Degree),
myNbIter(NbIter),
myProj(),
myTol2d(Tol2d),
myTol3d(Tol3d),
myTolAng(TolAng),
myNbBounds(0)
{ Standard_Integer NTCurve=TabCurve->Length();// Nombre de contraintes lineaires
  myNbPtsOnCur = 0; // Debrayage du calcul du nombre de points
                    // en fonction de la longueur
  myLinCont = new GeomPlate_HSequenceOfCurveConstraint;
  myPntCont = new GeomPlate_HSequenceOfPointConstraint;
  if (myNbIter<1)
    throw Standard_ConstructionError("GeomPlate :  Number of iteration must be >= 1");
    
  if (NTCurve==0) 
    throw Standard_ConstructionError("GeomPlate : the bounds Array is null");
  if (Tang->Length()==0) 
    throw Standard_ConstructionError("GeomPlate : the constraints Array is null");
  Standard_Integer nbp = 0;
  Standard_Integer i ;
  for ( i=1;i<=NTCurve;i++) 
    { nbp+=NPoints->Value(i);
    }
  if (nbp==0) 
    throw Standard_ConstructionError("GeomPlate : the resolution is impossible if the number of constraints points is 0");
  if (myDegree<2) 
    throw Standard_ConstructionError("GeomPlate ; the degree resolution must be upper of 2");
  // Remplissage des champs  passage de l'ancien constructeur au nouveau
  for(i=1;i<=NTCurve;i++) 
    { Handle(GeomPlate_CurveConstraint) Cont = new GeomPlate_CurveConstraint 
                                               ( TabCurve->Value(i),
						 Tang->Value(i),
						 NPoints->Value(i));
      myLinCont->Append(Cont);
    }
  mySurfInitIsGive=Standard_False;

  myIsLinear = Standard_True;
  myFree = Standard_False;
}

//------------------------------------------------------------------
// Constructeur avec surface d'init et degre de resolution de plate
//------------------------------------------------------------------
GeomPlate_BuildPlateSurface::GeomPlate_BuildPlateSurface ( 
                             const Handle(Geom_Surface)& Surf,
                             const Standard_Integer Degree,
			     const Standard_Integer NbPtsOnCur,
		             const Standard_Integer NbIter,
		             const Standard_Real Tol2d,
		             const Standard_Real Tol3d,
			     const Standard_Real TolAng,
			     const Standard_Real /*TolCurv*/,
			     const Standard_Boolean Anisotropie ) :
mySurfInit(Surf),
myAnisotropie(Anisotropie),
myDegree(Degree),
myNbPtsOnCur(NbPtsOnCur),
myNbIter(NbIter),
myProj(),
myTol2d(Tol2d),
myTol3d(Tol3d),
myTolAng(TolAng),
myNbBounds(0)
{   if (myNbIter<1)
    throw Standard_ConstructionError("GeomPlate :  Number of iteration must be >= 1");
 if (myDegree<2) 
     throw Standard_ConstructionError("GeomPlate : the degree resolution must be upper of 2");
   myLinCont = new GeomPlate_HSequenceOfCurveConstraint;
   myPntCont = new GeomPlate_HSequenceOfPointConstraint;
  mySurfInitIsGive=Standard_True;

  myIsLinear = Standard_True;
  myFree = Standard_False;
}


//---------------------------------------------------------
// Constructeur avec degre de resolution de plate
//---------------------------------------------------------
GeomPlate_BuildPlateSurface::GeomPlate_BuildPlateSurface (
                             const Standard_Integer Degree,
			     const Standard_Integer NbPtsOnCur,
                             const Standard_Integer NbIter,
                             const Standard_Real Tol2d,
                             const Standard_Real Tol3d,
                             const Standard_Real TolAng,
                             const Standard_Real /*TolCurv*/,
                             const Standard_Boolean Anisotropie ) :
myAnisotropie(Anisotropie),
myDegree(Degree),
myNbPtsOnCur(NbPtsOnCur),
myNbIter(NbIter),
myProj(),
myTol2d(Tol2d),
myTol3d(Tol3d),
myTolAng(TolAng),
myNbBounds(0)
{  if (myNbIter<1)
    throw Standard_ConstructionError("GeomPlate :  Number of iteration must be >= 1");
 if (myDegree<2) 
    throw Standard_ConstructionError("GeomPlate : the degree resolution must be upper of 2");
  myLinCont = new GeomPlate_HSequenceOfCurveConstraint;
  myPntCont = new GeomPlate_HSequenceOfPointConstraint;
  mySurfInitIsGive=Standard_False;

  myIsLinear = Standard_True;
  myFree = Standard_False;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//      =========================================================
//               M E T H O D E S  P U B L I Q U E S    
//      =========================================================
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
 


//-------------------------------------------------------------------------
// Fonction : TrierTab, Fonction interne, ne fait partie de la classe
//-------------------------------------------------------------------------
// Reordonne le tableau des permutations
// Apres l'appel a CourbeJointive l'ordre des courbes est modifie
// Ex : ordre init des courbe ==> A B C D E F
//      Dans TabInit on note ==> 1 2 3 4 5 6 
//      apres CourbeJointive ==> A E C B D F
//            TabInit ==> 1 5 3 2 4 6
//      apres TrierTab le Tableau contient ==> 1 4 3 5 2 6
// On peut ainsi acceder a la 2eme courbe en prenant TabInit[2] 
// c'est a dire la 4eme du tableau des courbes classees
//-------------------------------------------------------------------------
void TrierTab(Handle(TColStd_HArray1OfInteger)& Tab)
{ // Trie le tableau des permutations pour retrouver l'ordre initial
  Standard_Integer Nb=Tab->Length();
  TColStd_Array1OfInteger TabTri(1,Nb); 
  for (Standard_Integer i=1;i<=Nb;i++)
   TabTri.SetValue(Tab->Value(i),i);
  Tab->ChangeArray1()=TabTri;
}

//---------------------------------------------------------
// Fonction : ProjectCurve
//---------------------------------------------------------
Handle(Geom2d_Curve)  GeomPlate_BuildPlateSurface::ProjectCurve(const Handle(Adaptor3d_HCurve)& Curv) 
{ // Projection d'une courbe sur un plan
 Handle(Geom2d_Curve) Curve2d ;
 Handle(GeomAdaptor_HSurface) hsur = new GeomAdaptor_HSurface(mySurfInit);
 gp_Pnt2d P2d;

 ProjLib_CompProjectedCurve Projector(hsur, Curv, myTol3d/10, myTol3d/10);

 Standard_Real UdebCheck, UfinCheck, ProjUdeb, ProjUfin;
 UdebCheck = Curv->FirstParameter();
 UfinCheck = Curv->LastParameter();
 Projector.Bounds( 1, ProjUdeb, ProjUfin );

 if (Projector.NbCurves() != 1 ||
     Abs( UdebCheck -ProjUdeb ) > Precision::PConfusion() ||
     Abs( UfinCheck -ProjUfin ) > Precision::PConfusion())
   {
     if (Projector.IsSinglePnt(1, P2d))
       {
	 //solution ponctuelles
	 TColgp_Array1OfPnt2d poles(1, 2);
	 poles.Init(P2d);
	 Curve2d = new (Geom2d_BezierCurve) (poles);
       }
     else
       {
	 Curve2d.Nullify(); // Pas de solution continue
#ifdef OCCT_DEBUG
	 cout << "BuildPlateSurace :: Pas de projection continue" << endl;
#endif
       }
   }
 else {
   GeomAbs_Shape Continuity = GeomAbs_C1;
   Standard_Integer MaxDegree = 10, MaxSeg;
   Standard_Real Udeb, Ufin;
   Handle(ProjLib_HCompProjectedCurve) HProjector = 
     new ProjLib_HCompProjectedCurve();
   HProjector->Set(Projector);
   Projector.Bounds(1, Udeb, Ufin);
   
   MaxSeg = 20 + HProjector->NbIntervals(GeomAbs_C3);
   Approx_CurveOnSurface appr(HProjector, hsur, Udeb, Ufin, myTol3d,
			      Continuity, MaxDegree, MaxSeg, 
			      Standard_False, Standard_True);
   
   Curve2d = appr.Curve2d();
 }
#if DRAW
 if (Affich) {
   char name[256];
   sprintf(name,"proj_%d",++NbProj);
   DrawTrSurf::Set(name, Curve2d);
 }
#endif
 return Curve2d;
}
//---------------------------------------------------------
// Fonction : ProjectedCurve
//---------------------------------------------------------
Handle(Adaptor2d_HCurve2d)  GeomPlate_BuildPlateSurface::ProjectedCurve( Handle(Adaptor3d_HCurve)& Curv) 
{ // Projection d'une courbe sur la surface d'init

 Handle(GeomAdaptor_HSurface) hsur = new GeomAdaptor_HSurface(mySurfInit);

 ProjLib_CompProjectedCurve Projector(hsur, Curv, myTolU/10, myTolV/10);
 Handle(ProjLib_HCompProjectedCurve) HProjector = 
     new ProjLib_HCompProjectedCurve();

 if (Projector.NbCurves() != 1) {
     
     HProjector.Nullify(); // Pas de solution continue
#ifdef OCCT_DEBUG
     cout << "BuildPlateSurace :: Pas de projection continue" << endl;
#endif
   }
 else
   { 
     Standard_Real First1,Last1,First2,Last2;
     First1=Curv->FirstParameter();
     Last1 =Curv->LastParameter();
     Projector.Bounds(1,First2,Last2);

     if (Abs(First1-First2) <= Max(myTolU,myTolV) && 
         Abs(Last1-Last2) <= Max(myTolU,myTolV))
     {    

         HProjector->Set(Projector);
         HProjector = Handle(ProjLib_HCompProjectedCurve)::
	   DownCast(HProjector->Trim(First2,Last2,Precision::PConfusion()));
     }
     else
     {
         HProjector.Nullify(); // Pas de solution continue
#ifdef OCCT_DEBUG
         cout << "BuildPlateSurace :: Pas de projection complete" << endl;
#endif
     }
   }
   return HProjector;
}

//---------------------------------------------------------
// Fonction : ProjectPoint
//---------------------------------------------------------
// Projete une point sur la surface d'init
//---------------------------------------------------------
gp_Pnt2d GeomPlate_BuildPlateSurface::ProjectPoint(const gp_Pnt &p3d)
{ Extrema_POnSurf P;
  myProj.Perform(p3d);
  Standard_Integer nearest = 1;
  if( myProj.NbExt() > 1)  
  {
      Standard_Real dist2mini = myProj.SquareDistance(1);
      for (Standard_Integer i=2; i<=myProj.NbExt();i++)
      {
        if (myProj.SquareDistance(i) < dist2mini)
	  {
            dist2mini = myProj.SquareDistance(i);
            nearest = i;
          }
      }  
  }
  P=myProj.Point(nearest);
  Standard_Real u,v;
  P.Parameter(u,v);
  gp_Pnt2d p2d;
  p2d.SetCoord(u,v);

  return p2d;
}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//      =========================================================
//               M E T H O D E S  P U B L I Q U E S    
//      =========================================================
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//---------------------------------------------------------
// Fonction : Init
//---------------------------------------------------------
// Initialise les contraintes lineaires et ponctuelles
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::Init()
{ myLinCont->Clear();
  myPntCont->Clear();
  myPntCont = new GeomPlate_HSequenceOfPointConstraint;
  myLinCont = new GeomPlate_HSequenceOfCurveConstraint;
}

//---------------------------------------------------------
// Fonction : LoadInitSurface
//---------------------------------------------------------
// Charge la surface initiale
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::LoadInitSurface(const Handle(Geom_Surface)& Surf)
{ mySurfInit = Surf;
  mySurfInitIsGive=Standard_True;
}

//---------------------------------------------------------
//fonction : Add
//---------------------------------------------------------
//---------------------------------------------------------
// Ajout d'une contrainte lineaire
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
                  Add(const Handle(GeomPlate_CurveConstraint)& Cont)
{ myLinCont->Append(Cont);
}

void GeomPlate_BuildPlateSurface::SetNbBounds( const Standard_Integer NbBounds )
{
  myNbBounds = NbBounds;
}

//---------------------------------------------------------
//fonction : Add
//---------------------------------------------------------
//---------------------------------------------------------
// Ajout d'une contrainte ponctuelle
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
                  Add(const Handle(GeomPlate_PointConstraint)& Cont)
{ 
  myPntCont->Append(Cont);
}

//---------------------------------------------------------
//fonction : Perform
// Calcul la surface de remplissage avec les contraintes chargees
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::Perform()
{ 
#ifdef OCCT_DEBUG
  // Chronmetrage
  OSD_Chronometer Chrono;
  Chrono.Reset();
  Chrono.Start();
#endif

  if (myNbBounds == 0)
    myNbBounds = myLinCont->Length();

  myPlate.Init();
  //=====================================================================
  //Declaration des variables.
  //=====================================================================
  Standard_Integer NTLinCont = myLinCont->Length(), 
  NTPntCont = myPntCont->Length(), NbBoucle=0;
  // La variable  NTPoint peut etre enlevee
  Standard_Boolean Fini=Standard_True;
  if ((NTLinCont + NTPntCont) == 0)
  {
#ifdef OCCT_DEBUG
    cout << "WARNING : GeomPlate : The number of constraints is null." << endl;
#endif

    return;
  }

  //======================================================================   
  // Surface Initiale
  //======================================================================
  if (!mySurfInitIsGive)
    ComputeSurfInit();

  else {
   if (NTLinCont>=2)
	{ // Tableau des permutations pour conserver l'ordre initial voir TrierTab
	  myInitOrder = new TColStd_HArray1OfInteger(1,NTLinCont);
	  for (Standard_Integer l=1;l<=NTLinCont;l++)
	    myInitOrder->SetValue(l,l);
	  if (!CourbeJointive(myTol3d)) 
	    {//    throw Standard_Failure("Curves are not joined");
#ifdef OCCT_DEBUG
	      cout<<"WARNING : Courbes non jointives a "<<myTol3d<<" pres"<<endl;
#endif	  
	    }
	  TrierTab(myInitOrder); // Reordonne le tableau des permutations
	}
   else if(NTLinCont > 0)//Patch
     {
       mySense = new TColStd_HArray1OfInteger( 1, NTLinCont, 0 );
       myInitOrder = new TColStd_HArray1OfInteger( 1, NTLinCont, 1 );
     }
 }

  if (mySurfInit.IsNull())
  {
    return;
  }

  Standard_Real u1,v1,u2,v2;
  mySurfInit->Bounds(u1,v1,u2,v2);
  GeomAdaptor_Surface aSurfInit(mySurfInit);
  myTolU = aSurfInit.UResolution(myTol3d);
  myTolV = aSurfInit.VResolution(myTol3d);
  myProj.Initialize(aSurfInit,u1,v1,u2,v2,
		    myTolU,myTolV);

  //======================================================================
  // Projection des courbes
  //======================================================================
  Standard_Boolean Ok = Standard_True;
  for (Standard_Integer i = 1; i <= NTLinCont; i++)
    if(myLinCont->Value(i)->Curve2dOnSurf().IsNull())
      {
	Handle( Geom2d_Curve ) Curve2d = ProjectCurve( myLinCont->Value(i)->Curve3d() );
	if (Curve2d.IsNull())
	  {
	    Ok = Standard_False;
	    break;
	  }
	myLinCont->ChangeValue(i)->SetCurve2dOnSurf( Curve2d );
      }
  if (!Ok)
    {
      GeomPlate_MakeApprox App(myGeomPlateSurface, myTol3d, 
			       1, 3, 
			       15*myTol3d, 
			       -1, GeomAbs_C0,
			       1.3); 
      mySurfInit =  App.Surface();

      mySurfInit->Bounds(u1,v1,u2,v2);
      GeomAdaptor_Surface Surf(mySurfInit);
      myTolU = Surf.UResolution(myTol3d);
      myTolV = Surf.VResolution(myTol3d);
      myProj.Initialize(Surf,u1,v1,u2,v2,
			myTolU,myTolV);

      Ok = Standard_True;
      for (Standard_Integer i = 1; i <= NTLinCont; i++)
	{
	  Handle( Geom2d_Curve ) Curve2d = ProjectCurve( myLinCont->Value(i)->Curve3d() );
	  if (Curve2d.IsNull())
	    {
	      Ok = Standard_False;
	      break;
	    }
	  myLinCont->ChangeValue(i)->SetCurve2dOnSurf( Curve2d );
	}
      if (!Ok)
	{
	  mySurfInit = myPlanarSurfInit;

	  mySurfInit->Bounds(u1,v1,u2,v2);
	  GeomAdaptor_Surface SurfNew(mySurfInit);
	  myTolU = SurfNew.UResolution(myTol3d);
	  myTolV = SurfNew.VResolution(myTol3d);
	  myProj.Initialize(SurfNew,u1,v1,u2,v2,
			    myTolU,myTolV);

	  for (Standard_Integer i = 1; i <= NTLinCont; i++)
	    myLinCont->ChangeValue(i)->
	      SetCurve2dOnSurf(ProjectCurve( myLinCont->Value(i)->Curve3d() ) );
	}
      else { // Project the points
	for (Standard_Integer i=1; i<=NTPntCont; i++) { 
	  gp_Pnt P;
	  myPntCont->Value(i)->D0(P);
	  myPntCont->ChangeValue(i)->SetPnt2dOnSurf(ProjectPoint(P));
	}	
      }
    }

  //======================================================================
  // Projection des points
  //======================================================================
  for (Standard_Integer i=1;i<=NTPntCont;i++) {
    if (! myPntCont->Value(i)->HasPnt2dOnSurf()) {
      gp_Pnt P;
      myPntCont->Value(i)->D0(P);
      myPntCont->ChangeValue(i)->SetPnt2dOnSurf(ProjectPoint(P));
    }
  }

  //======================================================================
  // Nbre de point par courbe
  //======================================================================
  if ((NTLinCont !=0)&&(myNbPtsOnCur!=0)) 
    CalculNbPtsInit();

  //======================================================================
  // Gestion des incompatibilites entre courbes
  //======================================================================
  Handle(GeomPlate_HArray1OfSequenceOfReal) PntInter;
  Handle(GeomPlate_HArray1OfSequenceOfReal) PntG1G1;
  if (NTLinCont != 0) {
   PntInter = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
   PntG1G1 = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
   Intersect(PntInter,  PntG1G1);
  }

  //======================================================================
  // Boucle pour obtenir une meilleur surface
  //======================================================================

  myFree = !myIsLinear;

  do 
    {
#ifdef OCCT_DEBUG
      if (Affich && NbBoucle) {   
	cout<<"Resultats boucle"<< NbBoucle << endl;
	cout<<"DistMax="<<myG0Error<<endl;
	if (myG1Error!=0)
	  cout<<"AngleMax="<<myG1Error<<endl; 
	if (myG2Error!=0)
	  cout<<"CourbMax="<<myG2Error<<endl;
      }
#endif
      NbBoucle++;
      if (NTLinCont!=0)
	{ //====================================================================
	  // Calcul le nombre de point total et le maximum de points par courbe
	  //====================================================================
	  Standard_Integer NPointMax=0;
	  for (Standard_Integer i=1;i<=NTLinCont;i++) 
	    if ((myLinCont->Value(i)->NbPoints())>NPointMax)
	      NPointMax=(Standard_Integer )( myLinCont->Value(i)->NbPoints()); 
	  //====================================================================
	  // Discretisation des courbes
	  //====================================================================
	  Discretise(PntInter,  PntG1G1);  
	  //====================================================================
	  //Preparation des points de contrainte pour plate
	  //====================================================================
	  LoadCurve( NbBoucle );
	  if ( myPntCont->Length() != 0)
	    LoadPoint( NbBoucle );
	  //====================================================================
	  //Resolution de la surface
	  //====================================================================
	  myPlate.SolveTI(myDegree, ComputeAnisotropie());
          if (!myPlate.IsDone())
          {
#ifdef OCCT_DEBUG
            cout << "WARNING : GeomPlate : abort calcul of Plate." << endl;
#endif

            return;
          }

          myGeomPlateSurface = new GeomPlate_Surface(mySurfInit,myPlate);
	  Standard_Real Umin,Umax,Vmin,Vmax; 
          myPlate.UVBox(Umin,Umax,Vmin,Vmax);
	  myGeomPlateSurface->SetBounds(Umin,Umax,Vmin,Vmax);

	  Fini = VerifSurface(NbBoucle);
	  if ((NbBoucle >= myNbIter)&&(!Fini))
	    { 
#ifdef OCCT_DEBUG
	      cout<<"Warning objectif non atteint"<<endl;
#endif
	      Fini = Standard_True;
	    }

	  if ((NTPntCont != 0)&&(Fini))
	    { Standard_Real di,an,cu;
	      VerifPoints(di,an,cu);
	    }
	}
      else
	{ LoadPoint( NbBoucle );
	  //====================================================================
	  //Resolution de la surface
	  //====================================================================
	  myPlate.SolveTI(myDegree, ComputeAnisotropie());
          if (!myPlate.IsDone())
          {
#ifdef OCCT_DEBUG
            cout << "WARNING : GeomPlate : abort calcul of Plate." << endl;
#endif
            return;
          }

          myGeomPlateSurface = new GeomPlate_Surface(mySurfInit,myPlate);
	  Standard_Real Umin,Umax,Vmin,Vmax; 
          myPlate.UVBox(Umin,Umax,Vmin,Vmax);
	  myGeomPlateSurface->SetBounds(Umin,Umax,Vmin,Vmax);
	  Fini = Standard_True;
          Standard_Real di,an,cu;
          VerifPoints(di,an,cu);
	}
    } while (!Fini); // Fin boucle pour meilleur surface
#ifdef OCCT_DEBUG
  if (NTLinCont != 0)
    { cout<<"======== Resultats globaux ==========="<<endl;
      cout<<"DistMax="<<myG0Error<<endl;
      if (myG1Error!=0)
	cout<<"AngleMax="<<myG1Error<<endl; 
      if (myG2Error!=0)
	cout<<"CourbMax="<<myG2Error<<endl;
    }  
  Chrono.Stop();
  Standard_Real Tps;
  Chrono.Show(Tps);
  cout<<"*** FIN DE GEOMPLATE ***"<<endl;
  cout<<"Temps de calcul  : "<<Tps<<endl;
  cout<<"Nombre de boucle : "<<NbBoucle<<endl;
#endif
}  

//---------------------------------------------------------
// fonction : EcartContraintesMIL
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
EcartContraintesMil  ( const Standard_Integer c,
		      Handle(TColStd_HArray1OfReal)& d,
		      Handle(TColStd_HArray1OfReal)& an,
		      Handle(TColStd_HArray1OfReal)& courb)
{ 
  Standard_Integer NbPt=myParCont->Value(c).Length();
  Standard_Real U;
  if (NbPt<3)
    NbPt=4;
  else 
    NbPt=myParCont->Value(c).Length();
  gp_Vec v1i, v1f, v2i, v2f, v3i, v3f;
  gp_Pnt Pi, Pf;
  gp_Pnt2d P2d;Standard_Integer i;
  Handle(GeomPlate_CurveConstraint) LinCont = myLinCont->Value(c);
  switch (LinCont->Order())
    { case 0 :
	for (i=1; i<NbPt; i++)
	  { U = ( myParCont->Value(c).Value(i) + 
                    myParCont->Value(c).Value(i+1) )/2;
	    LinCont->D0(U, Pi);
	    if (!LinCont->ProjectedCurve().IsNull())
              P2d = LinCont->ProjectedCurve()->Value(U);
	    else 
            { if (!LinCont->Curve2dOnSurf().IsNull())
	         P2d = LinCont->Curve2dOnSurf()->Value(U); 
              else
	         P2d = ProjectPoint(Pi); 
            }
	    myGeomPlateSurface->D0( P2d.Coord(1), P2d.Coord(2), Pf); 
	    an->Init(0);
	    courb->Init(0);
	    d->ChangeValue(i) = Pf.Distance(Pi);
	  }
	break;
      case 1 :
	for (i=1; i<NbPt; i++)
	  { U = ( myParCont->Value(c).Value(i) + 
		 myParCont->Value(c).Value(i+1) )/2; 
	    LinCont->D1(U, Pi, v1i, v2i);
	    if (!LinCont->ProjectedCurve().IsNull())
              P2d = LinCont->ProjectedCurve()->Value(U);   
	    else 
            { if (!LinCont->Curve2dOnSurf().IsNull())
	         P2d = LinCont->Curve2dOnSurf()->Value(U); 
              else
	         P2d = ProjectPoint(Pi); 
            }
	    myGeomPlateSurface->D1( P2d.Coord(1), P2d.Coord(2), Pf, v1f, v2f); 
	    d->ChangeValue(i) = Pf.Distance(Pi);
	    v3i = v1i^v2i; v3f=v1f^v2f;
	    Standard_Real angle=v3f.Angle(v3i);
	    if (angle>(M_PI/2))
	      an->ChangeValue(i) = M_PI -angle;
	    else
	      an->ChangeValue(i) = angle;
	    courb->Init(0);
	  }
	break;
      case 2 :
	Handle(Geom_Surface) Splate (myGeomPlateSurface);
	LocalAnalysis_SurfaceContinuity CG2;
	for (i=1; i<NbPt; i++)
	  { U = ( myParCont->Value(c).Value(i) + 
		 myParCont->Value(c).Value(i+1) )/2; 
	    LinCont->D0(U, Pi);
            if (!LinCont->ProjectedCurve().IsNull())
                P2d = LinCont->ProjectedCurve()->Value(U);
	    else 
            { if (!LinCont->Curve2dOnSurf().IsNull())
	          P2d = LinCont->Curve2dOnSurf()->Value(U); 
              else
	          P2d = ProjectPoint(Pi); 
            }
	    GeomLProp_SLProps Prop (Splate, P2d.Coord(1), P2d.Coord(2), 
				    2, 0.001);
	    CG2.ComputeAnalysis(Prop, myLinCont->Value(c)->LPropSurf(U),
				GeomAbs_G2);
            d->ChangeValue(i)=CG2.C0Value();
	    an->ChangeValue(i)=CG2.G1Angle();
	    courb->ChangeValue(i)=CG2.G2CurvatureGap();
	  }
	break;
      }
}



//---------------------------------------------------------
// fonction : Disc2dContour
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
                  Disc2dContour ( const Standard_Integer /*nbp*/,
                                  TColgp_SequenceOfXY& Seq2d)
{
#ifdef OCCT_DEBUG
  if (Seq2d.Length()!=4)
    cout<<"nbp doit etre egal a 4 pour Disc2dContour"<<endl;
#endif
  //  initialisation
  Seq2d.Clear();
  
  //  echantillonnage en "cosinus" + 3 points sur chaque intervalle
  Standard_Integer NTCurve = myLinCont->Length();
  Standard_Integer NTPntCont = myPntCont->Length();
  gp_Pnt2d P2d;
  gp_XY UV;
  gp_Pnt PP;
  Standard_Real u1,v1,u2,v2;
  Standard_Integer i ;

  mySurfInit->Bounds(u1,v1,u2,v2);
  GeomAdaptor_Surface Surf(mySurfInit);
  myProj.Initialize(Surf,u1,v1,u2,v2,
		    myTolU,myTolV);

  for( i=1; i<=NTPntCont; i++) 
    if (myPntCont->Value(i)->Order()!=-1) 
      { P2d = myPntCont->Value(i)->Pnt2dOnSurf();
	UV.SetX(P2d.Coord(1));
	UV.SetY(P2d.Coord(2));
	Seq2d.Append(UV);
      }
  for(i=1; i<=NTCurve; i++) 
    {
   Handle(GeomPlate_CurveConstraint) LinCont = myLinCont->Value(i);
   if (LinCont->Order()!=-1) 
     { Standard_Integer NbPt=myParCont->Value(i).Length();
       // premier point de contrainte (j=0)
       if (!LinCont->ProjectedCurve().IsNull())
	   P2d = LinCont->ProjectedCurve()->Value(myParCont->Value(i).Value(1));

       else 
       {   if (!LinCont->Curve2dOnSurf().IsNull())
	      P2d = LinCont->Curve2dOnSurf()->Value(myParCont->Value(i).Value(1));

           else 
           {
              LinCont->D0(myParCont->Value(i).Value(1),PP);
	      P2d = ProjectPoint(PP);
           }
       }

       UV.SetX(P2d.Coord(1));
       UV.SetY(P2d.Coord(2));
       Seq2d.Append(UV);
       for (Standard_Integer j=2; j<NbPt; j++)
	 { Standard_Real Uj=myParCont->Value(i).Value(j), 
	   Ujp1=myParCont->Value(i).Value(j+1);
           if (!LinCont->ProjectedCurve().IsNull())
	       P2d = LinCont->ProjectedCurve()->Value((Ujp1+3*Uj)/4);          

           else 
           { if (!LinCont->Curve2dOnSurf().IsNull())
	         P2d = LinCont->Curve2dOnSurf()->Value((Ujp1+3*Uj)/4);

             else 
             {
               LinCont->D0((Ujp1+3*Uj)/4,PP);
	       P2d = ProjectPoint(PP);
             }
           }
	   UV.SetX(P2d.Coord(1));
	   UV.SetY(P2d.Coord(2));
	   Seq2d.Append(UV);
	   // point milieu precedent
           if (!LinCont->ProjectedCurve().IsNull())
	       P2d = LinCont->ProjectedCurve()->Value((Ujp1+Uj)/2);           

           else 
           {  if (!LinCont->Curve2dOnSurf().IsNull())
	         P2d = LinCont->Curve2dOnSurf()->Value((Ujp1+Uj)/2);

              else 
              {
                 LinCont->D0((Ujp1+Uj)/2,PP);
	         P2d = ProjectPoint(PP);
              }
           }

	   UV.SetX(P2d.Coord(1));
	   UV.SetY(P2d.Coord(2));
	   Seq2d.Append(UV);
	   // point 3/4 precedent
           if (!LinCont->ProjectedCurve().IsNull())
	       P2d = LinCont->ProjectedCurve()->Value((3*Ujp1+Uj)/4);

           else 
           {   if (!LinCont->Curve2dOnSurf().IsNull())
	          P2d = LinCont->Curve2dOnSurf()->Value((3*Ujp1+Uj)/4);

               else 
               {
                  LinCont->D0((3*Ujp1+Uj)/4,PP);
	          P2d = ProjectPoint(PP);
                }
           }

	   UV.SetX(P2d.Coord(1));
	   UV.SetY(P2d.Coord(2));
	   Seq2d.Append(UV);
	   // point de contrainte courant
           if (!LinCont->ProjectedCurve().IsNull())
	       P2d = LinCont->ProjectedCurve()->Value(Ujp1);

           else 
           {   if (!LinCont->Curve2dOnSurf().IsNull())
	          P2d = LinCont->Curve2dOnSurf()->Value(Ujp1);

               else 
               {
                  LinCont->D0(Ujp1,PP);
	          P2d = ProjectPoint(PP);
               }
           }

	   UV.SetX(P2d.Coord(1));
	   UV.SetY(P2d.Coord(2));
	   Seq2d.Append(UV);
	 }
     }
   }
}

//---------------------------------------------------------
// fonction : Disc3dContour
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
Disc3dContour (const Standard_Integer /*nbp*/,
               const Standard_Integer iordre,
               TColgp_SequenceOfXYZ& Seq3d)
{
#ifdef OCCT_DEBUG
  if (Seq3d.Length()!=4)
    cout<<"nbp doit etre egal a 4 pour Disc3dContour"<<endl;
  if (iordre!=0&&iordre!=1)
    cout<<"iordre incorrect pour Disc3dContour"<<endl;
#endif
  //  initialisation
  Seq3d.Clear();
  //  echantillonnage en "cosinus" + 3 points sur chaque intervalle
  Standard_Real u1,v1,u2,v2;
  mySurfInit->Bounds(u1,v1,u2,v2);
  GeomAdaptor_Surface Surf(mySurfInit);
  myProj.Initialize(Surf,u1,v1,u2,v2,
		    Surf.UResolution(myTol3d),
		    Surf.VResolution(myTol3d));
  Standard_Integer NTCurve = myLinCont->Length();
  Standard_Integer NTPntCont = myPntCont->Length();
//  gp_Pnt2d P2d;
  gp_Pnt P3d;
  gp_Vec v1h,v2h,v3h;
  gp_XYZ Pos;
  Standard_Integer i ;

  for( i=1; i<=NTPntCont; i++) 
    if (myPntCont->Value(i)->Order()!=-1) 
     { if (iordre==0) 
	 { myPntCont->Value(i)->D0(P3d);
	   Pos.SetX(P3d.X());
	   Pos.SetY(P3d.Y());
	   Pos.SetZ(P3d.Z());
	   Seq3d.Append(Pos);
	 }
       else {
	 myPntCont->Value(i)->D1(P3d,v1h,v2h);
	 v3h=v1h^v2h;
	 Pos.SetX(v3h.X());
	 Pos.SetY(v3h.Y());
	 Pos.SetZ(v3h.Z());
	 Seq3d.Append(Pos);
       }
     }
  
  for(i=1; i<=NTCurve; i++) 
    if (myLinCont->Value(i)->Order()!=-1) 
      
      { Standard_Integer NbPt=myParCont->Value(i).Length();;
	// premier point de contrainte (j=0)
	//  Standard_Integer NbPt=myParCont->Length();
	if (iordre==0) {
	  myLinCont->Value(i)->D0(myParCont->Value(i).Value(1),P3d);
	  Pos.SetX(P3d.X());
	  Pos.SetY(P3d.Y());
	  Pos.SetZ(P3d.Z());
	  Seq3d.Append(Pos);
	}
	else {
	  myLinCont->Value(i)->D1(myParCont->Value(i).Value(1),P3d,v1h,v2h);
	  v3h=v1h^v2h;
	  Pos.SetX(v3h.X());
	  Pos.SetY(v3h.Y());
	  Pos.SetZ(v3h.Z());
	  Seq3d.Append(Pos);
	}
	
	for (Standard_Integer j=2; j<NbPt; j++)
	  {  Standard_Real Uj=myParCont->Value(i).Value(j), 
	     Ujp1=myParCont->Value(i).Value(j+1);
	     if (iordre==0) {
	       // point 1/4 precedent
	       myLinCont->Value(i)->D0((Ujp1+3*Uj)/4,P3d);
	       Pos.SetX(P3d.X());
	       Pos.SetY(P3d.Y());
	       Pos.SetZ(P3d.Z());
	       Seq3d.Append(Pos);
	       // point milieu precedent
	       myLinCont->Value(i)->D0((Ujp1+Uj)/2,P3d);
	       Pos.SetX(P3d.X());
	       Pos.SetY(P3d.Y());
	       Pos.SetZ(P3d.Z());
	       Seq3d.Append(Pos);
	       // point 3/4 precedent
	       myLinCont->Value(i)->D0((3*Ujp1+Uj)/4,P3d);
	       Pos.SetX(P3d.X());
	       Pos.SetY(P3d.Y());
	       Pos.SetZ(P3d.Z());
	       Seq3d.Append(Pos);
	       // point de contrainte courant
	       myLinCont->Value(i)->D0(Ujp1,P3d);
	       Pos.SetX(P3d.X());
	       Pos.SetY(P3d.Y());
	       Pos.SetZ(P3d.Z());
	       Seq3d.Append(Pos);
	     }
	     else {
	       // point 1/4 precedent
	       myLinCont->Value(i)->D1((Ujp1+3*Uj)/4,P3d,v1h,v2h);
	       v3h=v1h^v2h;
	       Pos.SetX(v3h.X());
	       Pos.SetY(v3h.Y());
	       Pos.SetZ(v3h.Z());
	       Seq3d.Append(Pos);
	       // point milieu precedent
	       myLinCont->Value(i)->D1((Ujp1+Uj)/2,P3d,v1h,v2h);
	       v3h=v1h^v2h;
	       Pos.SetX(v3h.X());
	       Pos.SetY(v3h.Y());
	       Pos.SetZ(v3h.Z());
	       Seq3d.Append(Pos);
	       // point 3/4 precedent
	       myLinCont->Value(i)->D1((3*Ujp1+Uj)/4,P3d,v1h,v2h);
	       v3h=v1h^v2h;
	       Pos.SetX(v3h.X());
	       Pos.SetY(v3h.Y());
	       Pos.SetZ(v3h.Z());
	       Seq3d.Append(Pos);
	       // point de contrainte courant
	       myLinCont->Value(i)->D1(Ujp1,P3d,v1h,v2h);
	       v3h=v1h^v2h;
	       Pos.SetX(v3h.X());
	       Pos.SetY(v3h.Y());
	       Pos.SetZ(v3h.Z());
	       Seq3d.Append(Pos);
	     }
	   }
      }
  
}


//---------------------------------------------------------
// fonction : IsDone
//---------------------------------------------------------
Standard_Boolean GeomPlate_BuildPlateSurface::IsDone() const
{ return myPlate.IsDone();
}



//---------------------------------------------------------
// fonction : Surface
//---------------------------------------------------------
Handle(GeomPlate_Surface) GeomPlate_BuildPlateSurface::Surface() const
{ return myGeomPlateSurface ;
}

//---------------------------------------------------------
// fonction : SurfInit
//---------------------------------------------------------
Handle(Geom_Surface) GeomPlate_BuildPlateSurface::SurfInit() const
{ return mySurfInit ;
}


//---------------------------------------------------------
// fonction : Sense
//---------------------------------------------------------
Handle(TColStd_HArray1OfInteger) GeomPlate_BuildPlateSurface::Sense() const
{ Standard_Integer NTCurve = myLinCont->Length();
  Handle(TColStd_HArray1OfInteger) Sens = new TColStd_HArray1OfInteger(1,
								       NTCurve);
  for (Standard_Integer i=1; i<=NTCurve; i++)
    Sens->SetValue(i,mySense->Value(myInitOrder->Value(i)));
  return Sens;
}



//---------------------------------------------------------
// fonction : Curve2d
//---------------------------------------------------------
Handle(TColGeom2d_HArray1OfCurve) GeomPlate_BuildPlateSurface::Curves2d() const
{ Standard_Integer NTCurve = myLinCont->Length();
  Handle(TColGeom2d_HArray1OfCurve) C2dfin = 
    new TColGeom2d_HArray1OfCurve(1,NTCurve);

  for (Standard_Integer i=1; i<=NTCurve; i++)
    C2dfin->SetValue(i,myLinCont->Value(myInitOrder->Value(i))->Curve2dOnSurf());
  return C2dfin;
  
}


//---------------------------------------------------------
//fonction : Order
//---------------------------------------------------------
Handle(TColStd_HArray1OfInteger) GeomPlate_BuildPlateSurface::Order() const
{ Handle(TColStd_HArray1OfInteger) result=
    new TColStd_HArray1OfInteger(1,myLinCont->Length());
  for (Standard_Integer i=1;i<=myLinCont->Length();i++)
   result->SetValue(myInitOrder->Value(i),i);
  return result;
}


//---------------------------------------------------------
// fonction : G0Error
//---------------------------------------------------------
Standard_Real GeomPlate_BuildPlateSurface::G0Error() const
{ return myG0Error;
}

//---------------------------------------------------------
// fonction : G1Error
//---------------------------------------------------------
Standard_Real GeomPlate_BuildPlateSurface::G1Error() const
{ return myG1Error;
}

//---------------------------------------------------------
// fonction : G2Error
//---------------------------------------------------------
Standard_Real GeomPlate_BuildPlateSurface::G2Error() const
{ return myG2Error;
}

//=======================================================================
//function : G0Error
//purpose  : 
//=======================================================================

Standard_Real GeomPlate_BuildPlateSurface::G0Error( const Standard_Integer Index )
{
  Handle( TColStd_HArray1OfReal ) tdistance = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tangle = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tcurvature = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  EcartContraintesMil( Index, tdistance, tangle, tcurvature );
  Standard_Real MaxDistance = 0.;
  for (Standard_Integer i = 1; i <= myNbPtsOnCur; i++)
    if (tdistance->Value(i) > MaxDistance)
      MaxDistance = tdistance->Value(i);
  return MaxDistance;
}

//=======================================================================
//function : G1Error
//purpose  : 
//=======================================================================

Standard_Real GeomPlate_BuildPlateSurface::G1Error( const Standard_Integer Index )
{
  Handle( TColStd_HArray1OfReal ) tdistance = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tangle = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tcurvature = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  EcartContraintesMil( Index, tdistance, tangle, tcurvature );
  Standard_Real MaxAngle = 0.;
  for (Standard_Integer i = 1; i <= myNbPtsOnCur; i++)
    if (tangle->Value(i) > MaxAngle)
      MaxAngle = tangle->Value(i);
  return MaxAngle;
}

//=======================================================================
//function : G2Error
//purpose  : 
//=======================================================================

Standard_Real GeomPlate_BuildPlateSurface::G2Error( const Standard_Integer Index )
{
  Handle( TColStd_HArray1OfReal ) tdistance = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tangle = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  Handle( TColStd_HArray1OfReal ) tcurvature = new TColStd_HArray1OfReal( 1, myNbPtsOnCur );
  EcartContraintesMil( Index, tdistance, tangle, tcurvature );
  Standard_Real MaxCurvature = 0.;
  for (Standard_Integer i = 1; i <= myNbPtsOnCur; i++)
    if (tcurvature->Value(i) > MaxCurvature)
      MaxCurvature = tcurvature->Value(i);
  return MaxCurvature;
}

Handle(GeomPlate_CurveConstraint) GeomPlate_BuildPlateSurface::CurveConstraint( const Standard_Integer order) const
{
  return myLinCont->Value(order);
}
Handle(GeomPlate_PointConstraint) GeomPlate_BuildPlateSurface::PointConstraint( const Standard_Integer order) const
{
  return myPntCont->Value(order);
}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//      =========================================================
//                  M E T H O D E S  P R I V E S    
//      =========================================================
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

//=======================================================================
// function : CourbeJointive
// purpose  : Effectue un chainage des courbes pour pouvoir calculer 
//            la surface d'init avec la methode du flux maxi.
//            Retourne vrai si il s'agit d'un contour ferme.
//=======================================================================

Standard_Boolean GeomPlate_BuildPlateSurface::
                                  CourbeJointive(const Standard_Real tolerance) 
{ Standard_Integer nbf = myLinCont->Length();
  Standard_Real Ufinal1,Uinit1,Ufinal2,Uinit2;
  mySense = new TColStd_HArray1OfInteger(1,nbf,0);
  Standard_Boolean result=Standard_True;
  Standard_Integer j=1,i;
  gp_Pnt P1,P2;

  while (j <= (myNbBounds-1))
    {
      Standard_Integer a=0;
      i=j+1;
      if (i > myNbBounds) 
	{ result = Standard_False;
	  a=2;
	}
      while (a<1)
	{ if (i > myNbBounds) 
	    { result = Standard_False;
	      a=2;
	    }
	else
	  {
	    Uinit1=myLinCont->Value(j)->FirstParameter();
	    Ufinal1=myLinCont->Value(j)->LastParameter();
	    Uinit2=myLinCont->Value(i)->FirstParameter();
	    Ufinal2=myLinCont->Value(i)->LastParameter();
	    if (mySense->Value(j)==1)
	      Ufinal1=Uinit1;
	    myLinCont->Value(j)->D0(Ufinal1,P1);
	    myLinCont->Value(i)->D0(Uinit2,P2);
	    if (P1.Distance(P2)<tolerance)
	      { if (i!=j+1) { 
		Handle(GeomPlate_CurveConstraint) tampon = myLinCont->Value(j+1);
		myLinCont->SetValue(j+1,myLinCont->Value(i));
		myLinCont->SetValue(i,tampon);
		Standard_Integer Tmp=myInitOrder->Value(j+1);
		//Voir fonction TrierTab pour le fonctionnement de myInitOrder 
		myInitOrder->SetValue(j+1,myInitOrder->Value(i));  
		myInitOrder->SetValue(i,Tmp);
		
		
	      };
	      a=2;
		mySense->SetValue(j+1,0);
		
	      }
	  else
	    { myLinCont->Value(i)->D0(Ufinal2,P2);
	      
	      if (P1.Distance(P2)<tolerance)
		{ if (i!=j+1) {
		  Handle(GeomPlate_CurveConstraint) tampon = 
		    myLinCont->Value(j+1);
		  myLinCont->SetValue(j+1,myLinCont->Value(i));
		  myLinCont->SetValue(i,tampon);
		  Standard_Integer Tmp=myInitOrder->Value(j+1);
		  //Voir fonction TrierTab pour le fonctionnement de myInitOrder 
		  myInitOrder->SetValue(j+1,myInitOrder->Value(i));
		  myInitOrder->SetValue(i,Tmp);
		};
		  a=2;
 		  mySense->SetValue(j+1,1);
		}
	    }
	  }
	  i++;
	}
      j++;
    }
  Uinit1=myLinCont->Value( myNbBounds )->FirstParameter();
  Ufinal1=myLinCont->Value( myNbBounds )->LastParameter();
  Uinit2=myLinCont->Value(1)->FirstParameter();
  Ufinal2=myLinCont->Value(1)->LastParameter(); 
  myLinCont->Value( myNbBounds )->D0(Ufinal1,P1);
  myLinCont->Value(1)->D0(Uinit2,P2);
  if  ((mySense->Value( myNbBounds )==0)
       &&(P1.Distance(P2)<tolerance))
    {
      return ((Standard_True)&&(result));
    }
  myLinCont->Value( myNbBounds )->D0(Uinit1,P1);
  if  ((mySense->Value( myNbBounds )==1)
       &&(P1.Distance(P2)<tolerance))
    {
      return ((Standard_True)&&(result));
    }
  else return Standard_False;
}


//-------------------------------------------------------------------------
// fonction : ComputeSurfInit
//-------------------------------------------------------------------------
// Calcul la surface d'init soit par la methode du flux maxi soit par
// la methode du plan d'inertie si le contour n'est pas ferme ou si
// il y a des contraintes ponctuelles
//-------------------------------------------------------------------------

void GeomPlate_BuildPlateSurface::ComputeSurfInit()
{
  Standard_Integer nopt=2, popt=2, Np=1;
  Standard_Boolean isHalfSpace = Standard_True;
  Standard_Real LinTol = 0.001, AngTol = 0.001; //AngTol = 0.0001; //LinTol = 0.0001
  
  // Option pour le calcul du plan initial
  Standard_Integer NTLinCont = myLinCont->Length(), NTPntCont = myPntCont->Length();
  // Tableau des permutation pour conserver l'ordre initial voir TrierTab
  if (NTLinCont != 0) {
    myInitOrder = new TColStd_HArray1OfInteger(1,NTLinCont);
    for (Standard_Integer i = 1; i <= NTLinCont; i++)
      myInitOrder->SetValue( i, i );
  }

  Standard_Boolean CourbeJoint = (NTLinCont != 0) && CourbeJointive (myTol3d);
  if (CourbeJoint && IsOrderG1())
    {
      nopt = 3;
      // Tableau contenant le nuage de point pour le calcul du plan
      Standard_Integer  NbPoint = 20, Discr = NbPoint/4, pnum = 0;
      Handle( TColgp_HArray1OfPnt ) Pts = new TColgp_HArray1OfPnt( 1, (NbPoint+1)*NTLinCont+NTPntCont );
      TColgp_SequenceOfVec Vecs, NewVecs;
      GeomPlate_SequenceOfAij Aset;
      Standard_Real Uinit, Ufinal, Uif;
      gp_Vec LastVec;
      Standard_Integer i ;
      for ( i = 1; i <= NTLinCont; i++)
	{ 
	  Standard_Integer Order = myLinCont->Value(i)->Order();
	  
	  NewVecs.Clear();
	  
	  Uinit = myLinCont->Value(i)->FirstParameter();
	  Ufinal = myLinCont->Value(i)->LastParameter();
	  Uif = Ufinal - Uinit;
	  if (mySense->Value(i) == 1)
	    { 
	      Uinit = Ufinal;
	      Uif = -Uif;
	    }
	  
	  gp_Vec Vec1, Vec2, Normal;
	  Standard_Boolean ToReverse = Standard_False;
	  if (i > 1 && Order >= GeomAbs_G1)
	    {
	      gp_Pnt P;
	      myLinCont->Value(i)->D1( Uinit, P, Vec1, Vec2 );
	      Normal = Vec1 ^ Vec2;
	      if (LastVec.IsOpposite( Normal, AngTol ))
		ToReverse = Standard_True;
	    }
	  
	  for (Standard_Integer j = 0; j <= NbPoint; j++)
	    { // Nombre de point par courbe = 20
	      // Repartition lineaire
	      Standard_Real Inter = j*Uif/(NbPoint);
	      if (Order < GeomAbs_G1 || j % Discr != 0)
		myLinCont->Value(i)->D0( Uinit+Inter, Pts->ChangeValue(++pnum) );
	      else
		{
		  myLinCont->Value(i)->D1( Uinit+Inter, Pts->ChangeValue(++pnum), Vec1, Vec2 );
		  Normal = Vec1 ^ Vec2;
		  Normal.Normalize();
		  if (ToReverse)
		    Normal.Reverse();
		  Standard_Boolean isNew = Standard_True;
                  Standard_Integer k ;
		  for ( k = 1; k <= Vecs.Length(); k++)
		    if (Vecs(k).IsEqual( Normal, LinTol, AngTol ))
		      {
			isNew = Standard_False;
			break;
		      }
		  if (isNew)
		    for (k = 1; k <= NewVecs.Length(); k++)
		      if (NewVecs(k).IsEqual( Normal, LinTol, AngTol ))
			{
			  isNew = Standard_False;
			  break;
			}
		  if (isNew)
		    NewVecs.Append( Normal );
		}
	    }
	  if (Order >= GeomAbs_G1)
	    {
	      isHalfSpace = GeomPlate_BuildAveragePlane::HalfSpace( NewVecs, Vecs, Aset, LinTol, AngTol );
	      if (! isHalfSpace)
		break;
	      LastVec = Normal;
	    }
	} //for (i = 1; i <= NTLinCont; i++)
      
      if (isHalfSpace)
	{
	  for (i = 1; i <= NTPntCont; i++)
	    { 
	      Standard_Integer Order = myPntCont->Value(i)->Order();
	      
	      NewVecs.Clear();
	      gp_Vec Vec1, Vec2, Normal;
	      if (Order < GeomAbs_G1)
		myPntCont->Value(i)->D0( Pts->ChangeValue(++pnum) );
	      else
		{
		  myPntCont->Value(i)->D1( Pts->ChangeValue(++pnum), Vec1, Vec2 );
		  Normal = Vec1 ^ Vec2;
		  Normal.Normalize();
		  Standard_Boolean isNew = Standard_True;
		  for (Standard_Integer k = 1; k <= Vecs.Length(); k++)
		    if (Vecs(k).IsEqual( Normal, LinTol, AngTol ))
		      {
			isNew = Standard_False;
			break;
		      }
		  if (isNew)
		    {
		      NewVecs.Append( Normal );
		      isHalfSpace = GeomPlate_BuildAveragePlane::HalfSpace( NewVecs, Vecs, Aset, LinTol, AngTol );
		      if (! isHalfSpace)
			{
			  NewVecs(1).Reverse();
			  isHalfSpace = GeomPlate_BuildAveragePlane::HalfSpace( NewVecs, Vecs, Aset, LinTol, AngTol );
			}
		      if (! isHalfSpace)
			break;
		    }
		}
	    } //for (i = 1; i <= NTPntCont; i++)
	  
	  if (isHalfSpace)
	    {
	      Standard_Boolean NullExist = Standard_True;
	      while (NullExist)
		{
		  NullExist = Standard_False;
		  for (i = 1; i <= Vecs.Length(); i++)
		    if (Vecs(i).SquareMagnitude() == 0.)
		      {
			NullExist = Standard_True;
			Vecs.Remove(i);
			break;
		      }
		}
	      GeomPlate_BuildAveragePlane BAP( Vecs, Pts );
	      Standard_Real u1,u2,v1,v2;
	      BAP.MinMaxBox(u1,u2,v1,v2);
	      // On agrandit le bazar pour les projections
	      Standard_Real du = u2 - u1;
	      Standard_Real dv = v2 - v1;
	      u1 -= du; u2 += du; v1 -= dv; v2 += dv;
	      mySurfInit = new Geom_RectangularTrimmedSurface( BAP.Plane(), u1,u2,v1,v2 );
	    }
	} //if (isHalfSpace)
      if (!isHalfSpace)
	{
#ifdef OCCT_DEBUG
	  cout<<endl<<"Normals are not in half space"<<endl<<endl;
#endif
	  myIsLinear = Standard_False;
	  nopt = 2;
	}
    } //if (NTLinCont != 0 && (CourbeJoint = CourbeJointive( myTol3d )) && IsOrderG1())

  if (NTLinCont != 0)
    TrierTab( myInitOrder ); // Reordonne le tableau des permutations
  
  if (nopt != 3)
    {
      if ( NTPntCont != 0)
	nopt = 1;  //Calcul par la methode du plan d'inertie
      else if (!CourbeJoint || NTLinCont != myNbBounds)
	{//    throw Standard_Failure("Curves are not joined");
#ifdef OCCT_DEBUG	    
	  cout<<"WARNING : Courbes non jointives a "<<myTol3d<<" pres"<<endl;
#endif	  
	  nopt = 1;
	}

      Standard_Real LenT=0;
      Standard_Integer Npt=0;
      Standard_Integer NTPoint=20*NTLinCont;
      Standard_Integer i ;
      for ( i=1;i<=NTLinCont;i++) 
	LenT+=myLinCont->Value(i)->Length();
      for (i=1;i<=NTLinCont;i++) 
	{ Standard_Integer NbPoint= (Standard_Integer )( NTPoint*(myLinCont->Value(i)->Length())/LenT);
	  if (NbPoint<10)
	    NbPoint=10;
	  Npt+=NbPoint;
	}
      // Tableau contenant le nuage de point pour le calcul du plan
      Handle(TColgp_HArray1OfPnt) Pts = new TColgp_HArray1OfPnt(1,20*NTLinCont+NTPntCont);
      Standard_Integer  NbPoint=20;
      Standard_Real Uinit,Ufinal, Uif;
      for ( i=1;i<=NTLinCont;i++)
	{ Uinit=myLinCont->Value(i)->FirstParameter();
	  Ufinal=myLinCont->Value(i)->LastParameter();
	  Uif=Ufinal-Uinit;
	  if (mySense->Value(i) == 1)
	    { Uinit = Ufinal;
	      Uif = -Uif;
	    }
	  for (Standard_Integer j=0; j<NbPoint; j++)
	    { // Nombre de point par courbe = 20
	      // Repartition lineaire
	      Standard_Real Inter=j*Uif/(NbPoint);
	      gp_Pnt P;
	      myLinCont->Value(i)->D0(Uinit+Inter,P); 
	      Pts->SetValue(Np++,P);
	    }
	}
      for (i=1;i<=NTPntCont;i++)
	{ gp_Pnt P;
	  myPntCont->Value(i)->D0(P); 
	  Pts->SetValue(Np++,P);
	}
      if (!CourbeJoint)
	myNbBounds = 0;
      GeomPlate_BuildAveragePlane BAP( Pts, NbPoint*myNbBounds, myTol3d/1000, popt, nopt );
      if (!BAP.IsPlane())
      {
#ifdef OCCT_DEBUG
        cout << "WARNING : GeomPlate : the initial surface is not a plane." << endl;
#endif

        return;
      }
      Standard_Real u1,u2,v1,v2;
      BAP.MinMaxBox(u1,u2,v1,v2);
      // On agrandit le bazar pour les projections
      Standard_Real du = u2 - u1;
      Standard_Real dv = v2 - v1;
      u1 -= du; u2 += du; v1 -= dv; v2 += dv;
      mySurfInit = new Geom_RectangularTrimmedSurface(BAP.Plane(),u1,u2,v1,v2);
    } //if (nopt != 3)

  //Comparing metrics of curves and projected curves
  if (NTLinCont != 0 && myIsLinear)
    {
      Handle( Geom_Surface ) InitPlane = 
	(Handle( Geom_RectangularTrimmedSurface )::DownCast(mySurfInit))->BasisSurface();
      
      Standard_Real Ratio = 0., R1 = 2., R2 = 0.6; //R1 = 3, R2 = 0.5;//R1 = 1.4, R2 = 0.8; //R1 = 5., R2 = 0.2; 
      Handle( GeomAdaptor_HSurface ) hsur = 
	new GeomAdaptor_HSurface( InitPlane );
      Standard_Integer NbPoint = 20;
//      gp_Pnt P;
//      gp_Vec DerC, DerCproj, DU, DV;
//      gp_Pnt2d P2d;
//      gp_Vec2d DProj;
      

      for (Standard_Integer i = 1; i <= NTLinCont && myIsLinear; i++)
	{
	  Standard_Real FirstPar = myLinCont->Value(i)->FirstParameter();
	  Standard_Real LastPar  = myLinCont->Value(i)->LastParameter();
	  Standard_Real Uif = (LastPar - FirstPar)/(NbPoint);
	  
	  Handle( Adaptor3d_HCurve ) Curve = myLinCont->Value(i)->Curve3d();
	  ProjLib_CompProjectedCurve Projector( hsur, Curve, myTol3d, myTol3d );
	  Handle( ProjLib_HCompProjectedCurve ) ProjCurve = 
	    new ProjLib_HCompProjectedCurve();
	  ProjCurve->Set( Projector );
          Adaptor3d_CurveOnSurface AProj(ProjCurve, hsur);
	  
	  gp_Pnt P;
	  gp_Vec DerC, DerCproj;
	  for (Standard_Integer j = 1 ; j < NbPoint && myIsLinear; j++)
	    {
	      Standard_Real Inter = FirstPar + j*Uif;
	      Curve->D1(Inter, P, DerC);
	      AProj.D1(Inter, P, DerCproj);      
	      
	      Standard_Real A1 = DerC.Magnitude();
	      Standard_Real A2 = DerCproj.Magnitude();
	      if (A2 <= 1.e-20) {
		Ratio = 1.e20;
	      } 
	      else {
		Ratio = A1 / A2;
	      }
	      if (Ratio > R1 || Ratio < R2) 
		{
		  myIsLinear = Standard_False;
		  break;
		}
	    }
	}
#ifdef OCCT_DEBUG
      if (! myIsLinear)
	cout <<"Metrics are too different :"<< Ratio<<endl;
#endif
//      myIsLinear =  Standard_True; // !!
    } //comparing metrics of curves and projected curves


  if (! myIsLinear)
    {
      myPlanarSurfInit = mySurfInit;
#if DRAW
      if (Affich) {
	char name[256];
	sprintf(name,"planinit_%d",NbPlan+1);
	DrawTrSurf::Set(name, mySurfInit);
      }
#endif
      Standard_Real u1,v1,u2,v2;
      mySurfInit->Bounds(u1,v1,u2,v2);
      GeomAdaptor_Surface Surf(mySurfInit);
      myTolU = Surf.UResolution(myTol3d);
      myTolV = Surf.VResolution(myTol3d);
      myProj.Initialize(Surf,u1,v1,u2,v2,
			myTolU,myTolV);

      //======================================================================
      // Projection des courbes
      //======================================================================
      Standard_Integer i;
      for (i = 1; i <= NTLinCont; i++)
	if (myLinCont->Value(i)->Curve2dOnSurf().IsNull())
	  myLinCont->ChangeValue(i)->SetCurve2dOnSurf(ProjectCurve(myLinCont->Value(i)->Curve3d()));

      //======================================================================
      // Projection des points
      //======================================================================
      for (i = 1; i<=NTPntCont; i++)
	{ gp_Pnt P;
	  myPntCont->Value(i)->D0(P);
	  if (!myPntCont->Value(i)->HasPnt2dOnSurf())  
	    myPntCont->ChangeValue(i)->SetPnt2dOnSurf(ProjectPoint(P));
	}

      //======================================================================
      // Nbre de point par courbe
      //======================================================================
      if ((NTLinCont !=0)&&(myNbPtsOnCur!=0)) 
	CalculNbPtsInit();

      //======================================================================
      // Gestion des incompatibilites entre courbes
      //======================================================================
      Handle(GeomPlate_HArray1OfSequenceOfReal) PntInter;
      Handle(GeomPlate_HArray1OfSequenceOfReal) PntG1G1;
      if (NTLinCont != 0)
	{
	  PntInter = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
	  PntG1G1 = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
	  Intersect(PntInter,  PntG1G1);
	}

      //====================================================================
      // Discretisation des courbes
      //====================================================================
      Discretise(PntInter,  PntG1G1);  
      //====================================================================
      //Preparation des points de contrainte pour plate
      //====================================================================
      LoadCurve( 0, 0 );
      if (myPntCont->Length() != 0)
	LoadPoint( 0, 0 );
      //====================================================================
      //Resolution de la surface
      //====================================================================
      myPlate.SolveTI(2, ComputeAnisotropie());
      if (!myPlate.IsDone())
      {
#ifdef OCCT_DEBUG
        cout << "WARNING : GeomPlate : abort calcul of Plate." << endl;
#endif
        return;
      }

      myGeomPlateSurface = new GeomPlate_Surface( mySurfInit, myPlate );

      GeomPlate_MakeApprox App(myGeomPlateSurface, myTol3d, 
			       1, 3, 
			       15*myTol3d, 
			       -1, GeomAbs_C0); 
      mySurfInit =  App.Surface();
 
      mySurfInitIsGive = Standard_True;
      myPlate.Init(); // Reset

      for (i=1;i<=NTLinCont;i++)
	{
	  Handle( Geom2d_Curve ) NullCurve;
	  NullCurve.Nullify();
	  myLinCont->ChangeValue(i)->SetCurve2dOnSurf( NullCurve);
	}
    }

#if DRAW
  if (Affich) {
    char name[256];
    sprintf(name,"surfinit_%d",++NbPlan);
    DrawTrSurf::Set(name, mySurfInit);
  }
#endif
}

//---------------------------------------------------------
// fonction : Intersect
//---------------------------------------------------------
// Recherche les intersections entre les courbe 2d 
// Si intersection et compatible( dans le cas G1-G1)  
// enleve le point sur une des deux courbes
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
Intersect(Handle(GeomPlate_HArray1OfSequenceOfReal)& PntInter,
	  Handle(GeomPlate_HArray1OfSequenceOfReal)& PntG1G1)
{
  Standard_Integer NTLinCont = myLinCont->Length();
  Geom2dInt_GInter Intersection;
  Geom2dAdaptor_Curve Ci, Cj;
  IntRes2d_IntersectionPoint int2d;
  gp_Pnt P1,P2;
  gp_Pnt2d P2d;
  gp_Vec2d V2d;
//  if (!mySurfInitIsGive)
  for (Standard_Integer i=1;i<=NTLinCont;i++)
    {
      //Standard_Real NbPnt_i=myLinCont->Value(i)->NbPoints();
      // Cherche l'intersection avec chaque courbe y compris elle meme.
      Ci.Load(myLinCont->Value(i)->Curve2dOnSurf());
      for(Standard_Integer j=i; j<=NTLinCont; j++)
	{
	  Cj.Load(myLinCont->Value(j)->Curve2dOnSurf());
	  if (i==j)
	    Intersection.Perform(Ci, myTol2d*10, myTol2d*10); 
	  else
	    Intersection.Perform(Ci, Cj, myTol2d*10, myTol2d*10);
	    
	  if (!Intersection.IsEmpty())
	    { // il existe une intersection
	      Standard_Integer nbpt = Intersection.NbPoints();
	      // nombre de point d'intersection
	      for (Standard_Integer k = 1; k <= nbpt; k++) 
		{ int2d = Intersection.Point(k);
		  myLinCont->Value(i)->D0(int2d.ParamOnFirst(),P1);
		  myLinCont->Value(j)->D0(int2d.ParamOnSecond(),P2);
#ifdef OCCT_DEBUG
		  if (Affich> 1)
		    {
		      cout << " Intersection "<< k << " entre " << i 
			<< " &" << j << endl;
		      cout <<"  Distance = "<<  P1.Distance(P2) << endl;
		    }
#endif
		  if (P1.Distance( P2 ) < myTol3d)
		    { // L'intersection 2d correspond a des points 3d proches
		      // On note l'intervalle dans lequel il faudra enlever 
		      // un point pour eviter les doublons ce qui fait une 
		      // erreur dans plate. le point sur la courbe i est enleve 
		      // on conserve celui de la courbe j
		      // la longueur de l'intervalle est une longueur 2d 
		      // correspondant en 3d a myTol3d
		      Standard_Real tolint = Ci.Resolution(myTol3d);
		      Ci.D1(int2d.ParamOnFirst(),P2d, V2d);
		      Standard_Real aux = V2d.Magnitude();
		      if (aux > 1.e-7)
			{
			  aux = myTol3d/aux;
			  if (aux > 100*tolint) tolint*=100;
			  else tolint = aux;
			} 
		      else tolint*=100;
		      
		      PntInter->ChangeValue(i).Append( int2d.ParamOnFirst() - tolint);
		      PntInter->ChangeValue(i).Append( int2d.ParamOnFirst() + tolint);
		      // Si G1-G1
		      if ( (myLinCont->Value(i)->Order()==1)
			  &&(myLinCont->Value(j)->Order()==1))
			{ gp_Vec v11,v12,v13,v14,v15,v16,v21,v22,v23,v24,v25,v26;
			  myLinCont->Value(i)->D2( int2d.ParamOnFirst(), P1, v11, v12, v13, v14, v15 );
			  myLinCont->Value(j)->D2( int2d.ParamOnSecond(), P2, v21, v22, v23, v24, v25 ); 
			  v16=v11^v12;v26=v21^v22;
			  Standard_Real ant=v16.Angle(v26);	 
			  if (ant>(M_PI/2))
			    ant= M_PI -ant;
			  if ((Abs(v16*v15-v16*v25)>(myTol3d/1000))
			      ||(Abs(ant)>myTol3d/1000))  
			    // Pas compatible ==> on enleve une zone en 
			    // contrainte G1 correspondant
			    // a une tolerance 3D de 0.01
			    { Standard_Real coin;
			      Standard_Real Tol= 100 * myTol3d;
			      Standard_Real A1;
			      gp_Pnt2d P1temp,P2temp;
			      gp_Vec2d V1,V2;
			      myLinCont->Value(i)->Curve2dOnSurf()->D1( int2d.ParamOnFirst(), P1temp, V1);
			      myLinCont->Value(j)->Curve2dOnSurf()->D1( int2d.ParamOnSecond(), P2temp, V2);
			      A1 = V1.Angle(V2);
			      if (A1>(M_PI/2))
				A1= M_PI - A1;
			      if (Abs(Abs(A1)-M_PI)<myTolAng) Tol = 100000 * myTol3d;
#ifdef OCCT_DEBUG
			      if (Affich) cout <<"Angle entre Courbe "<<i<<","<<j
				<<" "<<Abs(Abs(A1)-M_PI)<<endl;
#endif
			      
			      coin = Ci.Resolution(Tol);
			      Standard_Real Par1=int2d.ParamOnFirst()-coin,
			      Par2=int2d.ParamOnFirst()+coin;
			      // Stockage de l'intervalle pour la courbe i
			      PntG1G1->ChangeValue(i).Append(Par1);
			      PntG1G1->ChangeValue(i).Append(Par2);
			      coin = Cj.Resolution(Tol);
			      Par1=int2d.ParamOnSecond()-coin;
			      Par2=int2d.ParamOnSecond()+coin;
			      // Stockage de l'intervalle pour la courbe j
			      PntG1G1->ChangeValue(j).Append(Par1);
			      PntG1G1->ChangeValue(j).Append(Par2);
			    }	
			}
		      // Si G0-G1
		      if ((myLinCont->Value(i)->Order()==0 && myLinCont->Value(j)->Order()==1) ||
			  (myLinCont->Value(i)->Order()==1 && myLinCont->Value(j)->Order()==0))
			{
			  gp_Vec vec, vecU, vecV, N;
			  if (myLinCont->Value(i)->Order() == 0)
			    {
			      Handle( Adaptor3d_HCurve ) theCurve = myLinCont->Value(i)->Curve3d();
			      theCurve->D1( int2d.ParamOnFirst(), P1, vec );
			      myLinCont->Value(j)->D1( int2d.ParamOnSecond(), P2, vecU, vecV );
			    }
			  else
			    {
			      Handle( Adaptor3d_HCurve ) theCurve = myLinCont->Value(j)->Curve3d();
			      theCurve->D1( int2d.ParamOnSecond(), P2, vec );
			      myLinCont->Value(i)->D1( int2d.ParamOnFirst(), P1, vecU, vecV );
			    }
			  N = vecU ^ vecV;
			  Standard_Real Angle = vec.Angle( N );
			  Angle = Abs( M_PI/2-Angle ); 
			  if (Angle > myTolAng/10.) //????????? //if (Abs( scal ) > myTol3d/100)
			    { // Pas compatible ==> on enleve une zone en 
			      // contrainte G0 et G1 correspondant
			      // a une tolerance 3D de 0.01
			      Standard_Real coin;
			      Standard_Real Tol= 100 * myTol3d;
			      Standard_Real A1;
			      gp_Pnt2d P1temp,P2temp;
			      gp_Vec2d V1,V2;
			      myLinCont->Value(i)->Curve2dOnSurf()->D1( int2d.ParamOnFirst(), P1temp, V1);
			      myLinCont->Value(j)->Curve2dOnSurf()->D1( int2d.ParamOnSecond(), P2temp, V2);
			      A1 = V1.Angle( V2 );
			      if (A1 > M_PI/2)
				A1= M_PI - A1;
			      if (Abs(Abs(A1) - M_PI) < myTolAng) Tol = 100000 * myTol3d;
#ifdef OCCT_DEBUG
			      if (Affich) cout <<"Angle entre Courbe "<<i<<","<<j
				<<" "<<Abs(Abs(A1)-M_PI)<<endl;
#endif
			      if (myLinCont->Value(i)->Order() == 1)
				{
				  coin = Ci.Resolution(Tol);
				  coin *=  Angle / myTolAng * 10.;
#ifdef OCCT_DEBUG
				  cout<<endl<<"coin = "<<coin<<endl;
#endif
				  Standard_Real Par1 = int2d.ParamOnFirst() - coin;
				  Standard_Real Par2 = int2d.ParamOnFirst() + coin;
				  // Stockage de l'intervalle pour la courbe i
				  PntG1G1->ChangeValue(i).Append( Par1 );
				  PntG1G1->ChangeValue(i).Append( Par2 );
				}
			      else
				{
				  coin = Cj.Resolution(Tol);
				  coin *= Angle / myTolAng * 10.;
#ifdef OCCT_DEBUG
				  cout<<endl<<"coin = "<<coin<<endl;
#endif
				  Standard_Real Par1 = int2d.ParamOnSecond() - coin;
				  Standard_Real Par2 = int2d.ParamOnSecond() + coin;
				  // Stockage de l'intervalle pour la courbe j
				  PntG1G1->ChangeValue(j).Append( Par1 );
				  PntG1G1->ChangeValue(j).Append( Par2 );
				}
			    }	
			}
		    } //if (P1.Distance( P2 ) < myTol3d)
		  else { 
		    //L'intersection 2d correspond a des points 3d eloigne
		    // On note l'intervalle dans lequel il faudra enlever 
		    // des points pour eviter les doublons ce qui fait une 
		    // erreur dans plate. le point sur la courbe i est enleve 
		    // on conserve celui de la courbe j.
		    // la longueur de l'intervalle est une longueur 2d 
		    // correspondant a la distance des points en 3d a myTol3d  
		    Standard_Real tolint, Dist;
		    Dist = P1.Distance(P2);
		    tolint = Ci.Resolution(Dist);
		    PntInter->ChangeValue(i).Append( int2d.ParamOnFirst() - tolint);
		    PntInter->ChangeValue(i).Append( int2d.ParamOnFirst() + tolint);
		    if (j!=i)
		      {
			tolint = Cj.Resolution(Dist);
			PntInter->ChangeValue(j).
			  Append( int2d.ParamOnSecond() - tolint);
			PntInter->ChangeValue(j).
			  Append( int2d.ParamOnSecond() + tolint);
		      }		       
		    
#ifdef OCCT_DEBUG
		    cout<<"Attention: Deux points 3d ont la meme projection dist="
		      <<Dist<<endl;
#endif	
#ifdef DRAW
		    if (Affich > 1)
		      {
			Handle(Draw_Marker3D) mark = new (Draw_Marker3D)(P1, Draw_X, Draw_vert);
                        char name[256];
			sprintf(name,"mark_%d",++NbMark);
			Draw::Set(name, mark); 
		      }
#endif	
		  }
		}
	    }    
	}
    }
}

//---------------------------------------------------------
// fonction : Discretise
//---------------------------------------------------------
// Discretise les courbes selon le parametre de celle-ci
// le tableau des sequences Parcont contiendera tous les 
// parametres des points sur les courbes 
// Le champ myPlateCont contiendera les parametre des points envoye a plate
// il excluera les points en double et les zones imcompatibles.
// La premiere partie correspond a la verfication de la compatibilite
// et a la supprssion des points en double.
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
Discretise(const Handle(GeomPlate_HArray1OfSequenceOfReal)& PntInter,
	   const Handle(GeomPlate_HArray1OfSequenceOfReal)& PntG1G1) 
{ 
  Standard_Integer NTLinCont = myLinCont->Length();
  Standard_Boolean ACR;
  Handle(Geom2d_Curve) C2d; 
  Geom2dAdaptor_Curve AC2d;
//  Handle(Adaptor_HCurve2d) HC2d;
  Handle(Law_Interpol) acrlaw = new (Law_Interpol) ();
  myPlateCont = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
  myParCont = new GeomPlate_HArray1OfSequenceOfReal(1,NTLinCont);
 

  //===========================================================================
  // Construction du tableau contenant les parametres des  points de contrainte 
  //=========================================================================== 
  Standard_Real  Uinit, Ufinal,  Length2d=0, Inter;
  Standard_Real   CurLength;
  Standard_Integer NbPnt_i, NbPtInter, NbPtG1G1;
  Handle(GeomPlate_CurveConstraint) LinCont;
  
  for (Standard_Integer i=1;i<=NTLinCont;i++) {
    LinCont = myLinCont->Value(i);
    Uinit=LinCont->FirstParameter();
    Ufinal=LinCont->LastParameter();
//    HC2d=LinCont->ProjectedCurve();
//    if(HC2d.IsNull())
//    ACR = (!HC2d.IsNull() || !C2d.IsNull());
    C2d= LinCont->Curve2dOnSurf();
    ACR =  (!C2d.IsNull());
    if (ACR) {
      // On Construit une loi proche de l'abscisse curviligne
      if(!C2d.IsNull()) AC2d.Load(C2d);
//      AC2d.Load(LinCont->Curve2dOnSurf());
      Standard_Integer ii, Nbint = 20;
      Standard_Real U;
      TColgp_Array1OfPnt2d tabP2d(1,  Nbint+1);
      tabP2d(1).SetY(Uinit);
      tabP2d(1).SetX(0.);
      tabP2d(Nbint+1).SetY(Ufinal);
/*      if (!HC2d.IsNull())
       
          Length2d = GCPnts_AbscissaPoint::Length(HC2d->Curve2d(), Uinit, Ufinal); 
      else*/
      Length2d = GCPnts_AbscissaPoint::Length(AC2d, Uinit, Ufinal);
        
      tabP2d(Nbint+1).SetX(Length2d);
      for (ii = 2;  ii<= Nbint; ii++) {
	U = Uinit + (Ufinal-Uinit)*((1-Cos((ii-1)*M_PI/(Nbint)))/2);
	tabP2d(ii).SetY(U);
/*        if (!HC2d.IsNull()) {
             Standard_Real L = GCPnts_AbscissaPoint::Length(HC2d->Curve2d(), Uinit, U);
             tabP2d(ii).SetX(L);
	   }
        else*/
 	     tabP2d(ii).SetX(GCPnts_AbscissaPoint::Length(AC2d, Uinit, U)); 
      }
      acrlaw->Set(tabP2d);
    }

    NbPnt_i = (Standard_Integer )( LinCont->NbPoints());
    NbPtInter= PntInter->Value(i).Length();
    NbPtG1G1= PntG1G1->Value(i).Length();

#ifdef OCCT_DEBUG
    if (Affich > 1) {
      cout << "Courbe : " << i << endl;
      cout << "  NbPnt, NbPtInter, NbPtG1G1 :" << NbPnt_i << ", " 
           << NbPtInter << ", " << NbPtG1G1 << endl;
    }
#endif
    for (Standard_Integer j=1; j<=NbPnt_i; j++)  
    { 
      // repartition des points en cosinus selon l'ACR 2d
      // Afin d'eviter les points d'acumulation dans le 2d
      //Inter=Uinit+(Uif)*((-cos(M_PI*((j-1)/(NbPnt_i-1)))+1)/2);
      if (j==NbPnt_i)
        Inter=Ufinal;//pour parer au bug sur sun
      else if (ACR) {
        CurLength = Length2d*(1-Cos((j-1)*M_PI/(NbPnt_i-1)))/2;
        Inter =  acrlaw->Value(CurLength);
      }
      else {
        Inter=Uinit+(Ufinal-Uinit)*((1-Cos((j-1)*M_PI/(NbPnt_i-1)))/2);
      }
      myParCont->ChangeValue(i).Append(Inter);// on ajoute le point
      if (NbPtInter!=0) 
      { 
        for(Standard_Integer l=1;l<=NbPtInter;l+=2) 
        {
          //on cherche si le point Inter est dans l'intervalle 
          //PntInter[i] PntInter[i+1]
          //auquelle cas il ne faudrait pas le stocker (pb de doublons) 
          if ((Inter>PntInter->Value(i).Value(l))
            &&(Inter<PntInter->Value(i).Value(l+1)))
          { 
            l=NbPtInter+2; 
            // pour sortir de la boucle sans stocker le point	 
          }
          else
          {
            if (l+1>=NbPtInter) {
              // on a parcouru tout le tableau : Le point 
              // n'appartient pas a un interval point commun 
              if (NbPtG1G1!=0) 
              {
                // est qu'il existe un intervalle incompatible
                for(Standard_Integer k=1;k<=NbPtG1G1;k+=2)
                { 
                  if ((Inter>PntG1G1->Value(i).Value(k))
                    &&(Inter<PntG1G1->Value(i).Value(k+1)))
                  { 
                    k=NbPtG1G1+2; // pour sortir de la boucle
                    // Ajouter les points de contrainte G0
                    gp_Pnt P3d,PP,Pdif;
                    gp_Pnt2d P2d;
                        
                    AC2d.D0(Inter, P2d);
                    LinCont->D0(Inter,P3d);
                    mySurfInit->D0(P2d.Coord(1),P2d.Coord(2),PP);
                    Pdif.SetCoord(-PP.Coord(1)+P3d.Coord(1),
                                  -PP.Coord(2)+P3d.Coord(2),
                                  -PP.Coord(3)+P3d.Coord(3));
                    Plate_PinpointConstraint PC(P2d.XY(),Pdif.XYZ(),0,0);
                    myPlate.Load(PC);
                  }
                  else // le point n'appartient pas a un interval G1
                  {
                    if (k+1>=NbPtG1G1) 
                    {
                      myPlateCont->ChangeValue(i).Append(Inter);
                      // on ajoute le point
                    }
                  }
                }
              }
              else
              {
                myPlateCont->ChangeValue(i).Append(Inter);
                // on ajoute le point
              }
            }
          }
        }
      }
      else
      {
        if (NbPtG1G1!=0) // est qu'il existe un intervalle incompatible
        {
          for(Standard_Integer k=1;k<=NbPtG1G1;k+=2)
          {
            if ((Inter>PntG1G1->Value(i).Value(k))
              &&(Inter<PntG1G1->Value(i).Value(k+1)))
            {
              k=NbPtG1G1+2; // pour sortir de la boucle
              // Ajouter les points de contrainte G0
              gp_Pnt P3d,PP,Pdif;
              gp_Pnt2d P2d;
              
              AC2d.D0(Inter, P2d);
              LinCont->D0(Inter,P3d);
              mySurfInit->D0(P2d.Coord(1),P2d.Coord(2),PP);
              Pdif.SetCoord(-PP.Coord(1)+P3d.Coord(1),
                -PP.Coord(2)+P3d.Coord(2),
                -PP.Coord(3)+P3d.Coord(3));
              Plate_PinpointConstraint PC(P2d.XY(),Pdif.XYZ(),0,0);
              myPlate.Load(PC);

            }
            else // le point n'appartient pas a un intervalle G1
            {
              if (k+1>=NbPtG1G1) 
              {
                myPlateCont->ChangeValue(i).Append(Inter);
                // on ajoute le point
              }
            }
          }
        }
        else
        {
          if (  ( (!mySurfInitIsGive)
                  &&(Geom2dAdaptor_Curve(LinCont->Curve2dOnSurf()).GetType()!=GeomAbs_Circle))
             || ( (j>1) &&(j<NbPnt_i))) //on enleve les extremites
            myPlateCont->ChangeValue(i).Append(Inter);// on ajoute le point
        }
      }
    }
  } 
}
//---------------------------------------------------------
// fonction : CalculNbPtsInit
//---------------------------------------------------------
// Calcul du nombre de points par courbe en fonction de la longueur
// pour la premiere iteration
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::CalculNbPtsInit ()
{
  Standard_Real LenT=0;
  Standard_Integer NTLinCont=myLinCont->Length();
  Standard_Integer NTPoint=(Standard_Integer )( myNbPtsOnCur*NTLinCont);
  Standard_Integer i ;

  for ( i=1;i<=NTLinCont;i++) 
    LenT+=myLinCont->Value(i)->Length();
  for ( i=1;i<=NTLinCont;i++) 
    { Standard_Integer Cont=myLinCont->Value(i)->Order();
      switch(Cont)
	{ case 0 : // Cas G0 *1.2
	    myLinCont->ChangeValue(i)->SetNbPoints( 
						   Standard_Integer(1.2*NTPoint*(myLinCont->Value(i)->Length())/LenT)); 
	    break;
	  case 1 : // Cas G1 *1
	    myLinCont->ChangeValue(i)->SetNbPoints(
				 Standard_Integer(NTPoint*(myLinCont->Value(i)->Length())/LenT)); 
	    break;
	  case 2 : // Cas G2 *0.7
	    myLinCont->ChangeValue(i)->SetNbPoints( 
			      Standard_Integer(0.7*NTPoint*(myLinCont->Value(i)->Length())/LenT));
	    break;
	  } 
      if (myLinCont->Value(i)->NbPoints()<3)
	myLinCont->ChangeValue(i)->SetNbPoints(3);
    }
}
//---------------------------------------------------------
// fonction : LoadCurve
//---------------------------------------------------------
// A partir du tableau myParCont on charge tous les points notes dans plate
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::LoadCurve(const Standard_Integer NbBoucle,
					    const Standard_Integer OrderMax )
{ 
  gp_Pnt P3d,Pdif,PP;
  gp_Pnt2d P2d;
  Standard_Integer NTLinCont=myLinCont->Length(), i, j;
  Standard_Integer  Tang, Nt;

  
  for (i=1; i<=NTLinCont; i++){
    Handle(GeomPlate_CurveConstraint) CC = myLinCont->Value(i);
    if (CC ->Order()!=-1) {
      Tang = Min(CC->Order(), OrderMax);
      Nt = myPlateCont->Value(i).Length();
      if (Tang!=-1)
	for (j=1; j<=Nt; j++) {// Chargement des points G0 sur les frontieres
	  CC ->D0(myPlateCont->Value(i).Value(j),P3d);
	  if (!CC->ProjectedCurve().IsNull())
	    P2d = CC->ProjectedCurve()->Value(myPlateCont->Value(i).Value(j));
	  
	  else {  
	    if (!CC->Curve2dOnSurf().IsNull())
	      P2d = CC->Curve2dOnSurf()->Value(myPlateCont->Value(i).Value(j));
	    else 
	      P2d = ProjectPoint(P3d);
	  }
	  mySurfInit->D0(P2d.Coord(1),P2d.Coord(2),PP);
	  Pdif.SetCoord(-PP.Coord(1)+P3d.Coord(1),
			-PP.Coord(2)+P3d.Coord(2),
			-PP.Coord(3)+P3d.Coord(3));
	  Plate_PinpointConstraint PC(P2d.XY(),Pdif.XYZ(),0,0);
	  myPlate.Load(PC);

	      // Chargement des points G1 
	  if (Tang==1) { // ==1
	    gp_Vec  V1,V2,V3,V4;
	    CC->D1( myPlateCont->Value(i).Value(j), PP, V1, V2 );
	    mySurfInit->D1( P2d.Coord(1), P2d.Coord(2), PP, V3, V4 );
	    
	    Plate_D1 D1final(V1.XYZ(),V2.XYZ());
	    Plate_D1 D1init(V3.XYZ(),V4.XYZ());
	    if (! myFree)
	      {
		Plate_GtoCConstraint GCC( P2d.XY(), D1init, D1final );
		myPlate.Load(GCC);
	      }
	    else if (NbBoucle == 1)
	      {
		Plate_FreeGtoCConstraint FreeGCC( P2d.XY(), D1init, D1final );
		myPlate.Load( FreeGCC );
	      }
	    else {
	      gp_Vec DU, DV, Normal, DerPlateU, DerPlateV;

	      Normal = V1^V2;
	      //Normal.Normalize();
	      Standard_Real norm = Normal.Magnitude();
	      if (norm > 1.e-12) Normal /= norm;
	      DerPlateU = myPrevPlate.EvaluateDerivative( P2d.XY(), 1, 0 );
	      DerPlateV = myPrevPlate.EvaluateDerivative( P2d.XY(), 0, 1 );

	      DU.SetLinearForm(-(V3 + DerPlateU).Dot(Normal), Normal, DerPlateU);
	      DV.SetLinearForm(-(V4 + DerPlateV).Dot(Normal), Normal, DerPlateV);
	      Plate_PinpointConstraint PinU( P2d.XY(), DU.XYZ(), 1, 0 );
	      Plate_PinpointConstraint PinV( P2d.XY(), DV.XYZ(), 0, 1 );
	      myPlate.Load( PinU );
	      myPlate.Load( PinV );
	    }
		}
	      // Chargement des points G2
	      if (Tang==2) // ==2
		{ gp_Vec  V1,V2,V3,V4,V5,V6,V7,V8,V9,V10;
		  CC->D2(myPlateCont->Value(i).Value(j),
				      PP,V1,V2,V5,V6,V7);
		  mySurfInit->D2(P2d.Coord(1),P2d.Coord(2),PP,V3,V4,V8,V9,V10);

		  Plate_D1 D1final(V1.XYZ(),V2.XYZ());
		  Plate_D1 D1init(V3.XYZ(),V4.XYZ());
		  Plate_D2 D2final (V5.XYZ(),V6.XYZ(),V7.XYZ());
		  Plate_D2 D2init (V8.XYZ(),V9.XYZ(),V10.XYZ());
//		  if (! myFree)
//		    {
		      Plate_GtoCConstraint GCC(P2d.XY(),D1init,D1final,D2init,D2final);
		      myPlate.Load(GCC);
//		    }
//		  else // Good but too expansive
//		    {
//		      Plate_FreeGtoCConstraint FreeGCC( P2d.XY(), 
//		            D1init, D1final, D2init, D2final );
//		      myPlate.Load( FreeGCC );
//		    }

		}   
	    }
	  }
  }
}  
  

//---------------------------------------------------------
//fonction : LoadPoint
//---------------------------------------------------------
//void GeomPlate_BuildPlateSurface::LoadPoint(const Standard_Integer NbBoucle, 
void GeomPlate_BuildPlateSurface::LoadPoint(const Standard_Integer , 
					    const Standard_Integer OrderMax)
{ 
  gp_Pnt P3d,Pdif,PP;
  gp_Pnt2d P2d;
  Standard_Integer NTPntCont=myPntCont->Length();
  Standard_Integer Tang, i;
//  gp_Vec  V1,V2,V3,V4,V5,V6,V7,V8,V9,V10;
 
  // Chargement des points de contraintes ponctuel
  for (i=1;i<=NTPntCont;i++) { 
    myPntCont->Value(i)->D0(P3d);
    P2d = myPntCont->Value(i)->Pnt2dOnSurf();
    mySurfInit->D0(P2d.Coord(1),P2d.Coord(2),PP);
    Pdif.SetCoord(-PP.Coord(1)+P3d.Coord(1),
		  -PP.Coord(2)+P3d.Coord(2),
		  -PP.Coord(3)+P3d.Coord(3));
    Plate_PinpointConstraint PC(P2d.XY(),Pdif.XYZ(),0,0);
    myPlate.Load(PC);
    Tang = Min(myPntCont->Value(i)->Order(), OrderMax);
    if (Tang==1) {// ==1
      gp_Vec  V1,V2,V3,V4;
      myPntCont->Value(i)->D1(PP,V1,V2);
      mySurfInit->D1(P2d.Coord(1),P2d.Coord(2),PP,V3,V4);
      Plate_D1 D1final(V1.XYZ(),V2.XYZ());
      Plate_D1 D1init(V3.XYZ(),V4.XYZ());
      if (! myFree)
	{
	  Plate_GtoCConstraint GCC( P2d.XY(), D1init, D1final );
	  myPlate.Load( GCC );
	}
      else
	{
	  Plate_FreeGtoCConstraint FreeGCC( P2d.XY(), D1init, D1final );
	  myPlate.Load( FreeGCC );
	}
    }
    // Chargement des points G2 GeomPlate_PlateG0Criterion 
    if (Tang==2) // ==2
      { gp_Vec  V1,V2,V3,V4,V5,V6,V7,V8,V9,V10;
	myPntCont->Value(i)->D2(PP,V1,V2,V5,V6,V7);
//	gp_Vec Tv2 = V1^V2;
	mySurfInit->D2(P2d.Coord(1),P2d.Coord(2),PP,V3,V4,V8,V9,V10);
	Plate_D1 D1final(V1.XYZ(),V2.XYZ());
	Plate_D1 D1init(V3.XYZ(),V4.XYZ());
	Plate_D2 D2final (V5.XYZ(),V6.XYZ(),V7.XYZ());
	Plate_D2 D2init (V8.XYZ(),V9.XYZ(),V10.XYZ());
//	if (! myFree)
//	  {
	    Plate_GtoCConstraint GCC( P2d.XY(), D1init, D1final, D2init, D2final );
	    myPlate.Load( GCC );
//	  }
//	else // Good but too expansive
//	  {
//	    Plate_FreeGtoCConstraint FreeGCC( P2d.XY(), D1init, D1final, D2init//, D2final );
//	    myPlate.Load( FreeGCC );
//	  }
      }   
  }

}
//---------------------------------------------------------
//fonction : VerifSurface
//---------------------------------------------------------
Standard_Boolean GeomPlate_BuildPlateSurface::
VerifSurface(const Standard_Integer NbBoucle)
{
  //======================================================================
  //    Calcul des erreurs 
  //======================================================================
  Standard_Integer NTLinCont=myLinCont->Length();
  Standard_Boolean Result=Standard_True;   

  // variable pour les calculs d erreur
  myG0Error=0,myG1Error =0, myG2Error=0;

  for (Standard_Integer i=1;i<=NTLinCont;i++) {
    Handle(GeomPlate_CurveConstraint) LinCont;
    LinCont = myLinCont->Value(i);
    if (LinCont->Order()!=-1) {
      Standard_Integer NbPts_i = myParCont->Value(i).Length();
      if (NbPts_i<3)
	NbPts_i=4;
      Handle(TColStd_HArray1OfReal) tdist = 
	new TColStd_HArray1OfReal(1,NbPts_i-1);      
      Handle(TColStd_HArray1OfReal) tang = 
	new TColStd_HArray1OfReal(1,NbPts_i-1);
      Handle(TColStd_HArray1OfReal) tcourb = 
	new TColStd_HArray1OfReal(1,NbPts_i-1);

      EcartContraintesMil (i,tdist,tang,tcourb);

      Standard_Real diffDistMax=0,SdiffDist=0;
      Standard_Real diffAngMax=0,SdiffAng=0;
      Standard_Integer NdiffDist=0,NdiffAng=0;

   
      for (Standard_Integer j=1;j<NbPts_i;j++)
	{ if (tdist->Value(j)>myG0Error)
	    myG0Error=tdist->Value(j);
	  if (tang->Value(j)>myG1Error)
	    myG1Error=tang->Value(j);
	  if (tcourb->Value(j)>myG2Error)
	    myG2Error=tcourb->Value(j);
	  Standard_Real U;
	  if (myParCont->Value(i).Length()>3)
	    U=(myParCont->Value(i).Value(j)+myParCont->Value(i).Value(j+1))/2;
	  else 
	    U=LinCont->FirstParameter()+
	      ( LinCont->LastParameter()-LinCont->FirstParameter())*(j-1)/(NbPts_i-2);
	  Standard_Real diffDist=tdist->Value(j)-LinCont->G0Criterion(U),diffAng;
	  if (LinCont->Order()>0)
	    diffAng=tang->Value(j)-LinCont->G1Criterion(U);
	  else diffAng=0;
	  // recherche de la variation de l'erreur maxi et calcul de la moyenne
	  if (diffDist>0) {
            diffDist = diffDist/LinCont->G0Criterion(U);
	    if (diffDist>diffDistMax)
	      diffDistMax = diffDist;
	    SdiffDist+=diffDist;
	    NdiffDist++;
#if DRAW
	    if ((Affich) && (NbBoucle == myNbIter)) {
	      gp_Pnt P;
	      gp_Pnt2d P2d;
	      LinCont->D0(U,P);
	      Handle(Draw_Marker3D) mark = 
		new (Draw_Marker3D)(P, Draw_X, Draw_orange);
              char name[256];
	      sprintf(name,"mark_%d",++NbMark);
	      Draw::Set(name, mark);
	      if (!LinCont->ProjectedCurve().IsNull())
                P2d = LinCont->ProjectedCurve()->Value(U);
	      else 
              {  if (!LinCont->Curve2dOnSurf().IsNull())
		    P2d = LinCont->Curve2dOnSurf()->Value(U); 
                 else    
		    P2d = ProjectPoint(P);
              }
	      sprintf(name,"mark2d_%d",++NbMark);
	      Handle(Draw_Marker2D) mark2d = 
		new (Draw_Marker2D)(P2d, Draw_X, Draw_orange);
	      Draw::Set(name, mark2d);
	    }
#endif
	  }
          else 
            if ((diffAng>0)&&(LinCont->Order()==1)) {
	      diffAng=diffAng/myLinCont->Value(i)->G1Criterion(U);
	      if (diffAng>diffAngMax)
		diffAngMax = diffAng;
	      SdiffAng+=diffAng;
	      NdiffAng++;
#if DRAW
	      if ((Affich) && (NbBoucle == myNbIter)) {
		gp_Pnt P;
		LinCont->D0(U,P);	
		Handle(Draw_Marker3D) mark = 
		  new Draw_Marker3D(P, Draw_X, Draw_or);
                char name[256];
		sprintf(name,"mark_%d",++NbMark);
		Draw::Set(name, mark);
	      }
#endif
	    }	  
        }

	if (NdiffDist>0) {// au moins un point n'est pas acceptable en G0
	  Standard_Real Coef;
	  if(LinCont->Order()== 0)
	    Coef = 0.6*Log(diffDistMax+7.4);                     
	  //7.4 correspond au calcul du coef mini = 1.2 donc e^1.2/0.6
	  else
	    Coef = Log(diffDistMax+3.3);                         
	  //3.3 correspond au calcul du coef mini = 1.2 donc e^1.2
          if (Coef>3) 
	    Coef=3;                                
	  //experimentalement apres le coef devient mauvais pour les cas L
	  if ((NbBoucle>1)&&(diffDistMax>2))
	    { Coef=1.6;
	    }

	  if (LinCont->NbPoints()>=Floor(LinCont->NbPoints()*Coef))
	    Coef=2;// pour assurer une augmentation du nombre de points

	  LinCont->SetNbPoints(Standard_Integer(LinCont->NbPoints() * Coef));
	  Result=Standard_False;	    	
	}
     else
	if (NdiffAng>0) // au moins 1 points ne sont pas accepable en G1
	  { Standard_Real Coef=1.5;
	    if ((LinCont->NbPoints()+1)>=Floor(LinCont->NbPoints()*Coef))
	      Coef=2;

	    LinCont->SetNbPoints(Standard_Integer(LinCont->NbPoints()*Coef )) ;
	    Result=Standard_False;
 	  }
    }
  } 
  if (!Result)
    {
      if (myFree && NbBoucle == 1)
	myPrevPlate = myPlate;
      myPlate.Init();
    }  
  return Result;     
}



//---------------------------------------------------------
//fonction : VerifPoint
//---------------------------------------------------------
void GeomPlate_BuildPlateSurface::
               VerifPoints (Standard_Real& Dist, 
			    Standard_Real& Ang, 
			    Standard_Real& Curv) const
{ Standard_Integer NTPntCont=myPntCont->Length();
  gp_Pnt Pi, Pf;
  gp_Pnt2d P2d;
  gp_Vec v1i, v1f, v2i, v2f, v3i, v3f;
  Ang=0;Dist=0,Curv=0;
  Handle(GeomPlate_PointConstraint) PntCont;
  for (Standard_Integer i=1;i<=NTPntCont;i++) {
    PntCont = myPntCont->Value(i);
    switch (PntCont->Order())
      { case 0 :
	  P2d = PntCont->Pnt2dOnSurf();
	  PntCont->D0(Pi);
	  myGeomPlateSurface->D0( P2d.Coord(1), P2d.Coord(2), Pf); 
	  Dist = Pf.Distance(Pi);
	  break;
	case 1 :
	  PntCont->D1(Pi, v1i, v2i);
	  P2d = PntCont->Pnt2dOnSurf();
	  myGeomPlateSurface->D1( P2d.Coord(1), P2d.Coord(2), Pf, v1f, v2f); 
	  Dist = Pf.Distance(Pi);
	  v3i = v1i^v2i; v3f=v1f^v2f;
	  Ang=v3f.Angle(v3i);
	  if (Ang>(M_PI/2))
	    Ang = M_PI -Ang;
	  break;
	case 2 :
	  Handle(Geom_Surface) Splate (myGeomPlateSurface);
	  LocalAnalysis_SurfaceContinuity CG2;
	  P2d = PntCont->Pnt2dOnSurf();
	  GeomLProp_SLProps Prop (Splate, P2d.Coord(1), P2d.Coord(2), 
				  2, 0.001);
	  CG2.ComputeAnalysis(Prop, PntCont->LPropSurf(),GeomAbs_G2);
	  Dist=CG2.C0Value();
	  Ang=CG2.G1Angle();
	  Curv=CG2.G2CurvatureGap();
	  break;
	}
  }
}

Standard_Real GeomPlate_BuildPlateSurface::ComputeAnisotropie() const
{
  if (myAnisotropie)
    {
      //Temporary
      return 1.0;
    }
  else return 1.0;
}

Standard_Boolean GeomPlate_BuildPlateSurface::IsOrderG1() const
{
  Standard_Boolean result = Standard_True;
  for (Standard_Integer i = 1; i <= myLinCont->Length(); i++)
    if (myLinCont->Value(i)->Order() < 1)
      {
	result = Standard_False;
	break;
      }
  return result;
}
