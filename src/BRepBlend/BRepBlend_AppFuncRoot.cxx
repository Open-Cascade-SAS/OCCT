// File:	BRepBlend_AppFuncRoot.cxx
// Created:	Tue May 12 16:57:47 1998
// Author:	Philippe NOUAILLE
//		<pne@cleox.paris1.matra-dtv.fr>


#include <BRepBlend_AppFuncRoot.ixx>
#include <Blend_AppFunction.hxx>

#include <Blend_Point.hxx>
#include <BRepBlend_Line.hxx>

#include <math_FunctionSetRoot.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_HArray1OfVec2d.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

BRepBlend_AppFuncRoot::BRepBlend_AppFuncRoot(Handle(BRepBlend_Line)& Line,
					     Blend_AppFunction&      Func,
					     const Standard_Real     Tol3d,
					     const Standard_Real     Tol2d)
:myLine(Line),
 myFunc(&Func),
 myTolerance(1,Func.NbVariables()),
 X1(1,Func.NbVariables()),
 X2(1,Func.NbVariables()),
 XInit(1,Func.NbVariables()),
 Sol(1,Func.NbVariables())
{
  Standard_Integer NbPoles, NbKnots, Degree, NbPoles2d;
  Standard_Integer ii;
  
  //  Tolerances    
  Func.GetTolerance(myTolerance, Tol3d);
  Standard_Integer dim = Func.NbVariables();
  for (ii=1; ii<= dim; ii++) {
    if (myTolerance(ii)>Tol2d) { myTolerance(ii) = Tol2d;}
  }
  
  //  Tableaux
  Func.GetShape( NbPoles, NbKnots, Degree, NbPoles2d);
  
  // Calcul du BaryCentre (cas rationnel).
  if (Func.IsRational()) {
    Standard_Real Xmax =-1.e100, Xmin = 1.e100, 
    Ymax =-1.e100, Ymin = 1.e100, 
    Zmax =-1.e100, Zmin = 1.e100;
    Blend_Point P;
    for (ii=1; ii<=myLine->NbPoints(); ii++) {
      P = myLine->Point(ii);
      Xmax = Max ( Max(P.PointOnS1().X(), P.PointOnS2().X()), Xmax);
      Xmin = Min ( Min(P.PointOnS1().X(), P.PointOnS2().X()), Xmin);
      Ymax = Max ( Max(P.PointOnS1().Y(), P.PointOnS2().Y()), Ymax);
      Ymin = Min ( Min(P.PointOnS1().Y(), P.PointOnS2().Y()), Ymin);
      Zmax = Max ( Max(P.PointOnS1().Z(), P.PointOnS2().Z()), Zmax);
      Zmin = Min ( Min(P.PointOnS1().Z(), P.PointOnS2().Z()), Zmin);
      
      myBary.SetCoord((Xmax+Xmin)/2, (Ymax+Ymin)/2, (Zmax+Zmin)/2);
    }
  }
  else {myBary.SetCoord(0,0,0);}
}

//================================================================================ 
// Function: D0
// Purpose : Calcul de la section pour v = Param, si le calcul echoue on rend
//           Standard_False. 
//================================================================================
Standard_Boolean BRepBlend_AppFuncRoot::D0(const Standard_Real Param,
					   const Standard_Real First,
					   const Standard_Real Last,TColgp_Array1OfPnt& Poles,
					   TColgp_Array1OfPnt2d& Poles2d,TColStd_Array1OfReal& Weigths) 
{
  Standard_Boolean Ok=Standard_True;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Ok = SearchPoint( *Func, Param, myPnt);
  
  if (Ok) (*Func).Section(myPnt,
			  Poles,
			  Poles2d,
			  Weigths);
  return Ok;
}

//================================================================================ 
// Function: D1
// Purpose : Calcul de la derive partiel de la section par rapport a v
//           pour v = Param, si le calcul echoue on rend Standard_False.
//================================================================================ 
Standard_Boolean BRepBlend_AppFuncRoot::D1(const Standard_Real Param,
					   const Standard_Real First,
					   const Standard_Real Last,
					   TColgp_Array1OfPnt& Poles,
					   TColgp_Array1OfVec& DPoles,
					   TColgp_Array1OfPnt2d& Poles2d,
					   TColgp_Array1OfVec2d& DPoles2d,
					   TColStd_Array1OfReal& Weigths,
					   TColStd_Array1OfReal& DWeigths) 
{
  Standard_Boolean Ok=Standard_True;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  
  Ok = SearchPoint( *Func, Param, myPnt);
  
  if (Ok) {
    Ok = (*Func).Section(myPnt,
			 Poles, DPoles,
			 Poles2d, DPoles2d,
			 Weigths, DWeigths);
  }
  
  return Ok;
}

//=========================================================================== 
// Function: D2
// Purpose : Calcul de la derive et seconde partiel de la 
//           section par rapport a v.
//           Pour v = Param, si le calcul echoue on rend Standard_False.  
//=========================================================================== 
Standard_Boolean BRepBlend_AppFuncRoot::D2(const Standard_Real Param,
					   const Standard_Real First,
					   const Standard_Real Last,
					   TColgp_Array1OfPnt& Poles,
					   TColgp_Array1OfVec& DPoles,
					   TColgp_Array1OfVec& D2Poles,
					   TColgp_Array1OfPnt2d& Poles2d,
					   TColgp_Array1OfVec2d& DPoles2d,
					   TColgp_Array1OfVec2d& D2Poles2d,
					   TColStd_Array1OfReal& Weigths,
					   TColStd_Array1OfReal& DWeigths,
					   TColStd_Array1OfReal& D2Weigths) 
{
  Standard_Boolean Ok=Standard_True;
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  
  Ok = SearchPoint( *Func, Param, myPnt); 
  if (Ok) {
    Ok = (*Func).Section(myPnt,
			 Poles, DPoles, D2Poles,
			 Poles2d, DPoles2d, D2Poles2d,
			 Weigths, DWeigths, D2Weigths);
  }
  return Ok;
}

Standard_Integer BRepBlend_AppFuncRoot::Nb2dCurves() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Standard_Integer i,j,k,nbpol2d;
  (*Func).GetShape(i,j,k,nbpol2d);
  return nbpol2d;
}

void BRepBlend_AppFuncRoot::SectionShape(Standard_Integer& NbPoles,
					 Standard_Integer& NbKnots,
					 Standard_Integer& Degree) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Standard_Integer ii;
  (*Func).GetShape( NbPoles, NbKnots, Degree, ii);
}

void BRepBlend_AppFuncRoot::Knots(TColStd_Array1OfReal& TKnots) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc; 
  Func->Knots(TKnots);
}

void BRepBlend_AppFuncRoot::Mults(TColStd_Array1OfInteger& TMults) const
{  
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Mults(TMults);
}

Standard_Boolean BRepBlend_AppFuncRoot::IsRational() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  return  (*Func).IsRational();
}

Standard_Integer BRepBlend_AppFuncRoot::NbIntervals(const GeomAbs_Shape S) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  return  Func->NbIntervals(S);
}

void BRepBlend_AppFuncRoot::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Intervals(T, S);
}

void BRepBlend_AppFuncRoot::SetInterval(const Standard_Real First,const Standard_Real Last) 
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Set(First, Last);
}

void BRepBlend_AppFuncRoot::Resolution(const Standard_Integer Index,
				       const Standard_Real Tol,
				       Standard_Real& TolU,
				       Standard_Real& TolV) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->Resolution(Index,Tol,TolU,TolV);
}

void BRepBlend_AppFuncRoot::GetTolerance(const Standard_Real BoundTol,
					 const Standard_Real SurfTol,
					 const Standard_Real AngleTol,
					 TColStd_Array1OfReal& Tol3d) const
{
  Standard_Integer ii;
  math_Vector V3d(1, Tol3d.Length()), V1d(1, Tol3d.Length());
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc; 
  
  Func->GetTolerance(BoundTol, SurfTol, AngleTol, V3d, V1d); 
  for (ii=1; ii<=Tol3d.Length(); ii++) Tol3d(ii) = V3d(ii);
}

void BRepBlend_AppFuncRoot::SetTolerance(const Standard_Real Tol3d, 
					 const Standard_Real Tol2d)
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc; 
  Standard_Integer ii, dim = Func->NbVariables();
  Func->GetTolerance(myTolerance, Tol3d);
  for (ii=1; ii<=dim; ii++) {
    if (myTolerance(ii)>Tol2d) { myTolerance(ii) = Tol2d;}
  }
} 

gp_Pnt BRepBlend_AppFuncRoot::BarycentreOfSurf() const
{
  return myBary;
}

Standard_Real BRepBlend_AppFuncRoot::MaximalSection() const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc; 
  return Func->GetSectionSize(); 
}

void BRepBlend_AppFuncRoot::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const
{
  Blend_AppFunction* Func = (Blend_AppFunction*)myFunc;
  Func->GetMinimalWeight(Weigths);
}


//================================================================================ 
//
// Function : SearchPoint
//
// Purpose : Recherche du point solution au parametre Param (sur 2 Surfaces)
//
// Algorithme : 
//     1) On recheche une solution approximative a partir des Points dejas calcules
//     2) On Converge par une methode de type Newton
// 
// Causes possibles d'echecs : 
//        - Singularite sur les surfaces.
//        - Manquent d'information dans la "line"issue du cheminement. 
//            
//================================================================================  

Standard_Boolean BRepBlend_AppFuncRoot::SearchPoint(Blend_AppFunction& Func,
						    const Standard_Real Param,
						    Blend_Point& Pnt)
{
  Standard_Boolean Trouve;
  Standard_Integer dim = Func.NbVariables();
  // (1) Recherche d'un point d'init
  Standard_Integer I1=1, I2=myLine->NbPoints(), Index;
  Standard_Real t1, t2;
  
  //  (1.a) On verifie que c'est a l'interieur
  if (Param < myLine->Point(I1).Parameter()) {return Standard_False;}
  if (Param > myLine->Point(I2).Parameter()) {return Standard_False;}
  
  //  (1.b) On recheche l'intervalle
  Trouve = SearchLocation(Param, I1, I2, Index);
  
  //  (1.c) Si le point est dejas calcule on le recupere
  if (Trouve) {
    Pnt = myLine->Point(Index);
    Vec(XInit,Pnt);
  }
  else {
    //  (1.d) Intialisation par interpolation lineaire
    Pnt = myLine->Point(Index);
    Vec(X1,Pnt);
    t1 = Pnt.Parameter();

    Pnt = myLine->Point(Index+1);
    Vec(X2,Pnt);
    t2 = Pnt.Parameter();

    Standard_Real Parammt1 = (Param-t1) / (t2-t1);
    Standard_Real t2mParam = (t2-Param) / (t2-t1);
    for(Standard_Integer i = 1; i <= dim; i++){
      XInit(i) = X2(i) * Parammt1 + X1(i) * t2mParam;
    }
  }

  // (2) Calcul effectif de la solution ------------------------
  Func.Set(Param);
  Func.GetBounds(X1, X2);
  math_FunctionSetRoot rsnld(Func, myTolerance, 30);
  
  rsnld.Perform(Func, XInit, X1, X2);
  
  if (!rsnld.IsDone()) {
# ifdef DEB
    cout << "AppFunc : RNLD Not done en t = " <<  Param  << endl;
# endif 
    return Standard_False;
  }
  rsnld.Root(Sol);
  
  // (3) Stockage du point
  Point(Func,Param,Sol,Pnt);

  // (4) Insertion du point si le cacul semble long.
  if ((!Trouve)&&(rsnld.NbIterations()>3)) {
#ifdef DEB
    cout << "Evaluation en t = " <<  Param << "donne" << endl;
    rsnld.Dump(cout);
#endif
    myLine->InsertBefore(Index+1, Pnt);
  }
  return Standard_True;
}


//=============================================================================
//
// Function : SearchLocation
//
// Purpose : Recherche dichotomiqique du rang de l'intervalle parametrique contenant
//           Param dans la liste de points calcule (myline)
//           si le point de parametre Param est deja stocker dans la liste on rend
//           True et ParamIndex correspond au rang du Point.
//           La complexite de cet algorithme est de log(n)/log(2)
//================================================================================ 
Standard_Boolean BRepBlend_AppFuncRoot::SearchLocation(const Standard_Real Param,
						       const Standard_Integer FirstIndex,
						       const Standard_Integer LastIndex,
						       Standard_Integer& ParamIndex) const
{
  Standard_Integer Ideb = FirstIndex, Ifin =  LastIndex, Idemi;
  Standard_Real Valeur;
  
  Valeur = myLine->Point(Ideb).Parameter();
  if (Param == Valeur) {
    ParamIndex = Ideb;
    return Standard_True; 
  }
  
  Valeur = myLine->Point(Ifin).Parameter();
  if (Param == Valeur) {
    ParamIndex = Ifin;
    return Standard_True; 
  } 
  
  while ( Ideb+1 != Ifin) {
    Idemi = (Ideb+Ifin)/2;
    Valeur = myLine->Point(Idemi).Parameter();
    if (Valeur < Param) {Ideb = Idemi;}
    else { 
      if ( Valeur > Param) { Ifin = Idemi;}
      else                 { ParamIndex = Idemi;
			     return Standard_True;}
    }
  }
  
  ParamIndex = Ideb;
  return Standard_False;
}

