// Created on: 1993-07-07
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1993-1999 Matra Datavision
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

// Version:	
//pmn 24/09/96 Ajout du prolongement de courbe.
//              jct 15/04/97 Ajout du prolongement de surface.
//              jct 24/04/97 simplification ou suppression de calculs
//                           inutiles dans ExtendSurfByLength
//                           correction de Tbord et Continuity=0 accepte
//                           correction du calcul de lambda et appel a
//                           TangExtendToConstraint avec lambmin au lieu de 1.
//                           correction du passage Sr rat --> BSp nD
//              xab 26/06/97 treatement partiel anulation des derivees 
//                           partiels du denonimateur des Surfaces BSplines Rationnelles
//                           dans le cas de valeurs proportionnelles des denominateurs
//                           en umin umax et/ou vmin vmax.
//              pmn 4/07/97  Gestion de la continuite dans BuildCurve3d (PRO9097)
//              xab 10/07/97 on revient en arriere sur l'ajout du 26/06/97
//              pmn 26/09/97 Ajout des parametres d'approx dans BuildCurve3d
//              xab 29/09/97 on reintegre l'ajout du 26/06/97
//              pmn 31/10/97 Ajoute AdjustExtremity
//              jct 26/11/98 blindage dans ExtendSurf qd NTgte = 0 (CTS21288)
//              jct 19/01/99 traitement de la periodicite dans ExtendSurf
// Design:	 
// Warning:      None	
// References:   None	
// Language:     C++2.0	
// Purpose:	
// Declarations:	

#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_GHCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib.hxx>
#include <GeomLib_DenominatorMultiplier.hxx>
#include <GeomLib_DenominatorMultiplierPtr.hxx>
#include <GeomLib_LogSample.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <GeomLib_PolyFunc.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_Vec.hxx>
#include <Hermit.hxx>
#include <math.hxx>
#include <math_FunctionAllRoots.hxx>
#include <math_FunctionSample.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
//
static Standard_Boolean CompareWeightPoles(const TColgp_Array1OfPnt& thePoles1, 
                                           const TColStd_Array1OfReal* const theW1,
                                           const TColgp_Array1OfPnt& thePoles2,
                                           const TColStd_Array1OfReal* const theW2,
                                           const Standard_Real theTol);

//=======================================================================
//function : ComputeLambda
//purpose  : Calcul le facteur lambda qui minimise la variation de vittesse
//           sur une interpolation d'hermite d'ordre (i,0)
//=======================================================================
static void ComputeLambda(const math_Matrix& Constraint,
			  const math_Matrix& Hermit,
			  const Standard_Real Length,
			  Standard_Real& Lambda )
{
  Standard_Integer size = Hermit.RowNumber();
  Standard_Integer Continuity = size-2;
  Standard_Integer ii, jj, ip, pp;

  //Minimization
  math_Matrix HDer(1, size-1, 1, size);
  for (jj=1; jj<=size; jj++) {
    for (ii=1; ii<size;ii++) {
      HDer(ii, jj) = ii*Hermit(jj, ii+1);
    }
  }

  math_Vector V(1, size);
  math_Vector Vec1(1, Constraint.RowNumber());
  math_Vector Vec2(1, Constraint.RowNumber());
  math_Vector Vec3(1, Constraint.RowNumber()); 
  math_Vector Vec4(1, Constraint.RowNumber());  

  Standard_Real * polynome = &HDer(1,1);
  Standard_Real * valhder =  &V(1);
  Vec2 =  Constraint.Col(2);
  Vec2 /= Length;
  Standard_Real t,  squared1 = Vec2.Norm2(), GW;
//  math_Matrix Vec(1, Constraint.RowNumber(), 1, size-1);
//  gp_Vec Vfirst(p0.XYZ()), Vlast(Point.XYZ());
//  TColgp_Array1OfVec Der(2, 4);
//  Der(2) = d1; Der(3) = d2; Der(4) = d3;

  Standard_Integer GOrdre = 4 + 4*Continuity, 
                   DDim=Continuity*(Continuity+2);
  math_Vector GaussP(1, GOrdre), GaussW(1, GOrdre), 
              pol2(1, 2*Continuity+1), 
              pol4(1, 4*Continuity+1);
  math::GaussPoints(GOrdre, GaussP);
  math::GaussWeights (GOrdre, GaussW);
  pol4.Init(0.);

  for (ip=1; ip<=GOrdre; ip++) {
    t = (GaussP(ip)+1.)/2;
    GW = GaussW(ip);
    PLib::NoDerivativeEvalPolynomial(t ,  Continuity, Continuity+2, DDim,
				     polynome[0], valhder[0]);
    V /= Length; //Normalisation   

    //                      i
    // C'(t) = SUM Vi*Lambda 
    Vec1 = Constraint.Col(1);
    Vec1 *= V(1);
    Vec1 += V(size)*Constraint.Col(size);
    Vec2 = Constraint.Col(2);
    Vec2 *= V(2);
    if (Continuity > 1) {
      Vec3 = Constraint.Col(3);
      Vec3 *= V(3);
      if (Continuity > 2) {
	Vec4 = Constraint.Col(4);
	Vec4 *= V(4);  
      }
    }
    
    
    //   2          2
    // C'(t) - C'(0)

    pol2(1) = Vec1.Norm2();
    pol2(2) = 2*(Vec1.Multiplied(Vec2));
    pol2(3) = Vec2.Norm2() - squared1;
    if (Continuity>1) { 
      pol2(3) += 2*(Vec1.Multiplied(Vec3));
      pol2(4) =  2*(Vec2.Multiplied(Vec3));
      pol2(5) =  Vec3.Norm2();
      if (Continuity>2) {
	pol2(4)+= 2*(Vec1.Multiplied(Vec4));
	pol2(5)+= 2*(Vec2.Multiplied(Vec4));
	pol2(6) = 2*(Vec3.Multiplied(Vec4));
	pol2(7) = Vec4.Norm2();
      }
    }

    //                     2      2  2
    // Integrale de ( C'(t) - C'(0) )
    for (ii=1; ii<=pol2.Length(); ii++) {
      pp = ii;
      for(jj=1; jj<ii; jj++, pp++) {
	pol4(pp) += 2*GW*pol2(ii)*pol2(jj);
      }
      pol4(2*ii-1) += GW*Pow(pol2(ii), 2);
    }
  }

  Standard_Real EMin, E;
  PLib::NoDerivativeEvalPolynomial(Lambda , pol4.Length()-1, 1, 
				   pol4.Length()-1,
				   pol4(1), EMin); 

  if (EMin > Precision::Confusion()) {
    // Recheche des extrema de la fonction
    GeomLib_PolyFunc FF(pol4);
    GeomLib_LogSample S(Lambda/1000, 50*Lambda, 100);
    math_FunctionAllRoots Solve(FF, S, Precision::Confusion(), 
				Precision::Confusion()*(Length+1),
				1.e-15);
    if (Solve.IsDone()) {
      for (ii=1; ii<=Solve.NbPoints(); ii++) {
	t = Solve.GetPoint(ii);
	PLib::NoDerivativeEvalPolynomial(t , pol4.Length()-1, 1, 
					 pol4.Length()-1,
					 pol4(1), E);
	if (E < EMin) {
	  Lambda = t;
	  EMin = E;
	}
      }
    }
  }
}

#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_Curve.hxx>
//=======================================================================
//function : RemovePointsFromArray
//purpose  : 
//=======================================================================

void GeomLib::RemovePointsFromArray(const Standard_Integer NumPoints,
				    const TColStd_Array1OfReal& InParameters,
				    Handle(TColStd_HArray1OfReal)& OutParameters) 
{
 Standard_Integer ii,
   jj,
   add_one_point,
   loc_num_points,
   num_points,
   index ;
 Standard_Real delta,
   current_parameter ;

   loc_num_points = Max(0,NumPoints-2) ;
   delta = InParameters(InParameters.Upper()) - InParameters(InParameters.Lower()) ;
   delta /= (Standard_Real) (loc_num_points + 1) ;
   num_points = 1 ;
   current_parameter = InParameters(InParameters.Lower()) + delta * 0.5e0 ;
   ii = InParameters.Lower() + 1 ;
   for (jj = 0 ; ii < InParameters.Upper() && jj < NumPoints ; jj++) {
     add_one_point = 0 ;
     while ( ii < InParameters.Upper() && InParameters(ii) < current_parameter) {
       ii += 1 ;
       add_one_point = 1 ;
     }
     num_points += add_one_point ;
     current_parameter += delta ;
   }
   if (NumPoints <= 2) {
     num_points = 2 ;
   }
   index = 2 ;
   current_parameter = InParameters(InParameters.Lower()) + delta * 0.5e0 ;
   OutParameters = 
     new TColStd_HArray1OfReal(1,num_points) ;
   OutParameters->ChangeArray1()(1) = InParameters(InParameters.Lower()) ;
   ii = InParameters.Lower() + 1 ;
   for (jj = 0 ; ii < InParameters.Upper() && jj < NumPoints ; jj++) {
     add_one_point = 0 ;
     while (ii < InParameters.Upper() && InParameters(ii) < current_parameter) {
       ii += 1 ;
       add_one_point = 1 ;
     }
     if (add_one_point && index <= num_points) {
       OutParameters->ChangeArray1()(index) = InParameters(ii-1) ;
       index += 1 ;
     }
     current_parameter += delta ;
   }
   OutParameters->ChangeArray1()(num_points) = InParameters(InParameters.Upper()) ;
}
//=======================================================================
//function : DensifyArray1OfReal
//purpose  : 
//=======================================================================

void GeomLib::DensifyArray1OfReal(const Standard_Integer MinNumPoints,
				  const TColStd_Array1OfReal& InParameters,
				  Handle(TColStd_HArray1OfReal)& OutParameters) 
{
 Standard_Integer ii,
   in_order,
   num_points,
   num_parameters_to_add,
   index ;
 Standard_Real delta,
   current_parameter ;

 in_order = 1 ;
 if (MinNumPoints > InParameters.Length()) {

   //
   // checks the paramaters are in increasing order
   // 
   for (ii = InParameters.Lower() ; ii < InParameters.Upper() ; ii++) {
     if (InParameters(ii) > InParameters(ii+1)) {
       in_order = 0 ;
       break ;
     }
   }
   if (in_order) {
     num_parameters_to_add = MinNumPoints - InParameters.Length()  ;
     delta = InParameters(InParameters.Upper()) - InParameters(InParameters.Lower()) ;
     delta /= (Standard_Real) (num_parameters_to_add + 1) ;
     num_points = MinNumPoints ;
     OutParameters = 
       new TColStd_HArray1OfReal(1,num_points) ;
     index = 1 ;
     current_parameter = InParameters(InParameters.Lower()) ;
     OutParameters->ChangeArray1()(index) = current_parameter ;
     index += 1 ;
     current_parameter += delta ; 
     for (ii = InParameters.Lower() + 1 ; index <= num_points && ii <= InParameters.Upper() ; ii++) {
       while (current_parameter < InParameters(ii) && index <= num_points) {
	 OutParameters->ChangeArray1()(index) = current_parameter ;
	 index += 1 ;
	 current_parameter += delta ;
       }
       if (index <= num_points) { 
	 OutParameters->ChangeArray1()(index) = InParameters(ii) ;
       }
       index += 1 ;
     }
     //
     // beware of roundoff !
     //
     OutParameters->ChangeArray1()(num_points) = InParameters(InParameters.Upper()) ;
   }
   else {
     index = 1 ;
     num_points = InParameters.Length() ;
     OutParameters = 
       new TColStd_HArray1OfReal(1,num_points) ;
     for (ii = InParameters.Lower()  ; ii <= InParameters.Upper() ; ii++) {
       OutParameters->ChangeArray1()(index) = InParameters(ii) ;
       index += 1 ;
     }
   }
 }
 else {
   index = 1 ;
   num_points = InParameters.Length() ;
   OutParameters = 
     new TColStd_HArray1OfReal(1,num_points) ;
   for (ii = InParameters.Lower()  ; ii <= InParameters.Upper() ; ii++) {
     OutParameters->ChangeArray1()(index) = InParameters(ii) ;
     index += 1 ;
   }
 }
}

//=======================================================================
//function : FuseIntervals
//purpose  : 
//=======================================================================
void GeomLib::FuseIntervals(const  TColStd_Array1OfReal& I1,
			    const  TColStd_Array1OfReal& I2,
			    TColStd_SequenceOfReal&  Seq,
			    const Standard_Real  Epspar) 
{
 Standard_Integer ind1=1, ind2=1;
 Standard_Real    v1, v2;
// Initialisations : les IND1 et IND2 pointent sur le 1er element
// de chacune des 2 tables a traiter.INDS pointe sur le dernier
// element cree de TABSOR


//--- On remplit TABSOR en parcourant TABLE1 et TABLE2 simultanement ---
//------------------ en eliminant les occurrences multiples ------------

 while ((ind1<=I1.Upper()) && (ind2<=I2.Upper())) {
      v1 = I1(ind1);
      v2 = I2(ind2);
      if (Abs(v1-v2)<= Epspar) {
// Ici les elements de I1 et I2 conviennent .
         Seq.Append((v1+v2)/2);
	 ind1++;
         ind2++;
       }
      else if (v1 < v2) {
	// Ici l' element de I1 convient.
         Seq.Append(v1);
         ind1++;
       }
      else {
// Ici l' element de TABLE2 convient.
	 Seq.Append(v2);
	 ind2++;
       }
    }

  if (ind1>I1.Upper()) { 
//----- Ici I1 est epuise, on complete avec la fin de TABLE2 -------

    for (; ind2<=I2.Upper(); ind2++) {
      Seq.Append(I2(ind2));
    }
  }

  if (ind2>I2.Upper()) { 
//----- Ici I2 est epuise, on complete avec la fin de I1 -------
    for (; ind1<=I1.Upper(); ind1++) {
      Seq.Append(I1(ind1));
    }
  } 
}


//=======================================================================
//function : EvalMaxParametricDistance
//purpose  : 
//=======================================================================

void GeomLib::EvalMaxParametricDistance(const Adaptor3d_Curve& ACurve,
			       const Adaptor3d_Curve& AReferenceCurve,
//			       const Standard_Real  Tolerance,
			       const Standard_Real  ,
			       const TColStd_Array1OfReal& Parameters,
			       Standard_Real& MaxDistance) 
{
  Standard_Integer ii ;

  Standard_Real max_squared = 0.0e0,
//    tolerance_squared,
    local_distance_squared ;

//  tolerance_squared = Tolerance * Tolerance ;
  gp_Pnt Point1 ;
  gp_Pnt Point2 ;
  for (ii = Parameters.Lower() ; ii <= Parameters.Upper() ; ii++) {
    ACurve.D0(Parameters(ii),
	      Point1) ;
    AReferenceCurve.D0(Parameters(ii),
		       Point2) ;
    local_distance_squared =
      Point1.SquareDistance (Point2) ;
    max_squared = Max(max_squared,local_distance_squared) ;
  }
  if (max_squared > 0.0e0) {
    MaxDistance = sqrt(max_squared) ;
  }
  else {
    MaxDistance = 0.0e0 ;
  }
  
}
//=======================================================================
//function : EvalMaxDistanceAlongParameter
//purpose  : 
//=======================================================================

void GeomLib::EvalMaxDistanceAlongParameter(const Adaptor3d_Curve& ACurve,
			       const Adaptor3d_Curve& AReferenceCurve,
			       const Standard_Real  Tolerance,
			       const TColStd_Array1OfReal& Parameters,
			       Standard_Real& MaxDistance) 
{
  Standard_Integer ii ;
  Standard_Real max_squared = 0.0e0,
    tolerance_squared = Tolerance * Tolerance,
    other_parameter,
    para_tolerance,
    local_distance_squared ;
  gp_Pnt Point1 ;
  gp_Pnt Point2 ;



  para_tolerance = 
    AReferenceCurve.Resolution(Tolerance) ;
  other_parameter = Parameters(Parameters.Lower()) ;
  ACurve.D0(other_parameter,
	    Point1) ;
  Extrema_LocateExtPC a_projector(Point1,
				  AReferenceCurve,
				  other_parameter,
				  para_tolerance) ;
  for (ii = Parameters.Lower() ; ii <= Parameters.Upper() ; ii++) {
    ACurve.D0(Parameters(ii),
	      Point1) ;
    AReferenceCurve.D0(Parameters(ii),
		       Point2) ;
    local_distance_squared =
      Point1.SquareDistance (Point2) ;
    
    local_distance_squared =
      Point1.SquareDistance (Point2) ;
    
    
    if (local_distance_squared > tolerance_squared) {
      
      
      a_projector.Perform(Point1,
			  other_parameter) ;
      if (a_projector.IsDone()) {
	other_parameter =
	  a_projector.Point().Parameter() ;
	AReferenceCurve.D0(other_parameter,
			   Point2) ;
	local_distance_squared =
	  Point1.SquareDistance (Point2) ;
      }
      else {
	local_distance_squared = 0.0e0 ;
	other_parameter = Parameters(ii) ;
      }
    }
    else {
      other_parameter = Parameters(ii) ;
    }
    
    
    max_squared = Max(max_squared,local_distance_squared) ;
  }
  if (max_squared > tolerance_squared) {
    MaxDistance = sqrt(max_squared) ;
  }
  else {
    MaxDistance = Tolerance ;
  }
}



// Aliases:	

// Global data definitions:	

// Methods :


//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomLib::To3d (const gp_Ax2&               Position,
				  const Handle(Geom2d_Curve)& Curve2d  ) {
  Handle(Geom_Curve) Curve3d;
  Handle(Standard_Type) KindOfCurve = Curve2d->DynamicType();

  if (KindOfCurve == STANDARD_TYPE (Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve) Ct =
      Handle(Geom2d_TrimmedCurve)::DownCast(Curve2d);
    Standard_Real U1 = Ct->FirstParameter ();
    Standard_Real U2 = Ct->LastParameter  ();
    Handle(Geom2d_Curve) CBasis2d = Ct->BasisCurve();
    Handle(Geom_Curve) CC = GeomLib::To3d(Position, CBasis2d);
    Curve3d = new Geom_TrimmedCurve (CC, U1, U2);
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_OffsetCurve)) {
    Handle(Geom2d_OffsetCurve) Co =
      Handle(Geom2d_OffsetCurve)::DownCast(Curve2d);
    Standard_Real Offset = Co->Offset();
    Handle(Geom2d_Curve) CBasis2d = Co->BasisCurve();
    Handle(Geom_Curve) CC = GeomLib::To3d(Position, CBasis2d);
    Curve3d = new Geom_OffsetCurve (CC, Offset, Position.Direction());
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_BezierCurve)) {
    Handle(Geom2d_BezierCurve) CBez2d = 
      Handle(Geom2d_BezierCurve)::DownCast (Curve2d);
    Standard_Integer Nbpoles = CBez2d->NbPoles ();
    TColgp_Array1OfPnt2d Poles2d (1, Nbpoles);
    CBez2d->Poles (Poles2d);
    TColgp_Array1OfPnt Poles3d (1, Nbpoles);
    for (Standard_Integer i = 1; i <= Nbpoles; i++) {
      Poles3d (i) = ElCLib::To3d (Position, Poles2d (i));
    }
    Handle(Geom_BezierCurve) CBez3d;
    if (CBez2d->IsRational()) {
      TColStd_Array1OfReal TheWeights (1, Nbpoles);
      CBez2d->Weights (TheWeights);
      CBez3d = new Geom_BezierCurve (Poles3d, TheWeights);
    }
    else {
      CBez3d = new Geom_BezierCurve (Poles3d);
    }
    Curve3d = CBez3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_BSplineCurve)) {
    Handle(Geom2d_BSplineCurve) CBSpl2d =
      Handle(Geom2d_BSplineCurve)::DownCast (Curve2d);
    Standard_Integer Nbpoles   = CBSpl2d->NbPoles ();
    Standard_Integer Nbknots   = CBSpl2d->NbKnots ();
    Standard_Integer TheDegree = CBSpl2d->Degree ();
    Standard_Boolean IsPeriodic = CBSpl2d->IsPeriodic();
    TColgp_Array1OfPnt2d Poles2d (1, Nbpoles);
    CBSpl2d->Poles (Poles2d);
    TColgp_Array1OfPnt Poles3d (1, Nbpoles);
    for (Standard_Integer i = 1; i <= Nbpoles; i++) {
      Poles3d (i) = ElCLib::To3d (Position, Poles2d (i));
    }
    TColStd_Array1OfReal    TheKnots (1, Nbknots);
    TColStd_Array1OfInteger TheMults (1, Nbknots);
    CBSpl2d->Knots (TheKnots);
    CBSpl2d->Multiplicities (TheMults);
    Handle(Geom_BSplineCurve) CBSpl3d;
    if (CBSpl2d->IsRational()) {
      TColStd_Array1OfReal TheWeights (1, Nbpoles);
      CBSpl2d->Weights (TheWeights);
      CBSpl3d = new Geom_BSplineCurve (Poles3d, TheWeights, TheKnots, TheMults, TheDegree, IsPeriodic);
    }
    else {
      CBSpl3d = new Geom_BSplineCurve (Poles3d, TheKnots, TheMults, TheDegree, IsPeriodic);
    }
    Curve3d = CBSpl3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_Line)) {
    Handle(Geom2d_Line) Line2d = Handle(Geom2d_Line)::DownCast (Curve2d);
    gp_Lin2d L2d = Line2d->Lin2d();
    gp_Lin   L3d = ElCLib::To3d (Position, L2d);
    Handle(Geom_Line) GeomL3d = new Geom_Line (L3d);
    Curve3d = GeomL3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_Circle)) {
    Handle(Geom2d_Circle) Circle2d = 
      Handle(Geom2d_Circle)::DownCast (Curve2d);
    gp_Circ2d C2d = Circle2d->Circ2d();
    gp_Circ   C3d = ElCLib::To3d (Position, C2d);
    Handle(Geom_Circle) GeomC3d = new Geom_Circle (C3d);
    Curve3d = GeomC3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_Ellipse)) {
    Handle(Geom2d_Ellipse) Ellipse2d =
      Handle(Geom2d_Ellipse)::DownCast (Curve2d);
    gp_Elips2d E2d = Ellipse2d->Elips2d ();
    gp_Elips   E3d = ElCLib::To3d (Position, E2d);
    Handle(Geom_Ellipse) GeomE3d = new Geom_Ellipse (E3d);
    Curve3d = GeomE3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_Parabola)) {
    Handle(Geom2d_Parabola) Parabola2d =
      Handle(Geom2d_Parabola)::DownCast (Curve2d);
    gp_Parab2d Prb2d = Parabola2d->Parab2d ();
    gp_Parab   Prb3d = ElCLib::To3d (Position, Prb2d);
    Handle(Geom_Parabola) GeomPrb3d = new Geom_Parabola (Prb3d);
    Curve3d = GeomPrb3d;
  }
  else if (KindOfCurve == STANDARD_TYPE (Geom2d_Hyperbola)) {
    Handle(Geom2d_Hyperbola) Hyperbola2d =
      Handle(Geom2d_Hyperbola)::DownCast (Curve2d);
    gp_Hypr2d H2d = Hyperbola2d->Hypr2d ();
    gp_Hypr   H3d = ElCLib::To3d (Position, H2d);
    Handle(Geom_Hyperbola) GeomH3d = new Geom_Hyperbola (H3d);
    Curve3d = GeomH3d;
  }
  else {
    throw Standard_NotImplemented();
  }
  
  return Curve3d;
}



//=======================================================================
//function : GTransform
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomLib::GTransform(const Handle(Geom2d_Curve)& Curve, 
					 const gp_GTrsf2d&           GTrsf)
{
  gp_TrsfForm Form = GTrsf.Form();
  
  if ( Form != gp_Other) {
    
    // Alors, la GTrsf est en fait une Trsf. 
    // La geometrie des courbes sera alors inchangee.

    Handle(Geom2d_Curve) C = 
      Handle(Geom2d_Curve)::DownCast(Curve->Transformed(GTrsf.Trsf2d()));
    return C;
  }
  else { 
    
    // Alors, la GTrsf est une other Transformation.
    // La geometrie des courbes est alors changee, et les conics devront
    // etre converties en BSplines.
    
    Handle(Standard_Type) TheType = Curve->DynamicType();
    
    if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      
      // On va recurer sur la BasisCurve
      
      Handle(Geom2d_TrimmedCurve) C = 
	Handle(Geom2d_TrimmedCurve)::DownCast(Curve->Copy());
      
      Handle(Standard_Type) TheBasisType = (C->BasisCurve())->DynamicType();
      
      if (TheBasisType == STANDARD_TYPE(Geom2d_BSplineCurve) ||
	  TheBasisType == STANDARD_TYPE(Geom2d_BezierCurve)    ) {
	
	// Dans ces cas le parametrage est conserve sur la courbe transformee
	// on peut donc la trimmer avec les parametres de la courbe de base.
	
	Standard_Real U1 = C->FirstParameter();
	Standard_Real U2 = C->LastParameter();
	
	Handle(Geom2d_TrimmedCurve) result = 
	  new Geom2d_TrimmedCurve(GTransform(C->BasisCurve(), GTrsf), U1,U2);
	return result;
      }
      else if ( TheBasisType == STANDARD_TYPE(Geom2d_Line)) {
	
	// Dans ce cas, le parametrage n`est plus conserve.
	// Il faut recalculer les parametres de Trimming sur la courbe 
	// resultante. ( Calcul par projection ( ElCLib) des points debut 
	// et fin transformes)
	
	Handle(Geom2d_Line) L = 
	  Handle(Geom2d_Line)::DownCast(GTransform(C->BasisCurve(), GTrsf));
	gp_Lin2d Lin = L->Lin2d();
	
	gp_Pnt2d P1 = C->StartPoint();
	gp_Pnt2d P2 = C->EndPoint();
	P1.SetXY(GTrsf.Transformed(P1.XY()));
	P2.SetXY(GTrsf.Transformed(P2.XY()));
	Standard_Real U1 = ElCLib::Parameter(Lin,P1);
	Standard_Real U2 = ElCLib::Parameter(Lin,P2);
	
	Handle(Geom2d_TrimmedCurve) result = 
	  new Geom2d_TrimmedCurve(L,U1,U2);
	return result;
      }
      else if (TheBasisType == STANDARD_TYPE(Geom2d_Circle)   ||
	       TheBasisType == STANDARD_TYPE(Geom2d_Ellipse)  ||
	       TheBasisType == STANDARD_TYPE(Geom2d_Parabola) ||
	       TheBasisType == STANDARD_TYPE(Geom2d_Hyperbola)  ) {
	
	// Dans ces cas, la geometrie de la courbe n`est pas conservee
	// on la convertir en BSpline avant de lui appliquer la Trsf.
	
	Handle(Geom2d_BSplineCurve) BS = 
	  Geom2dConvert::CurveToBSplineCurve(C);
	return GTransform(BS,GTrsf);
      }
      else {
	
	// La transformee d`une OffsetCurve vaut ????? Sais pas faire !! 
	
	Handle(Geom2d_Curve) dummy;
	return dummy;
      }
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Line)) {
      
      Handle(Geom2d_Line) L = 
	Handle(Geom2d_Line)::DownCast(Curve->Copy());
      gp_Lin2d Lin = L->Lin2d();
      gp_Pnt2d P  = Lin.Location();
      gp_Pnt2d PP = L->Value(10.); // pourquoi pas !!
      P.SetXY(GTrsf.Transformed(P.XY()));
      PP.SetXY(GTrsf.Transformed(PP.XY()));
      L->SetLocation(P);
      gp_Vec2d V(P,PP);
      L->SetDirection(gp_Dir2d(V));
      return L;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BezierCurve)) {
      
      // Les GTrsf etant des operation lineaires, la transformee d`une courbe
      // a poles est la courbe dont les poles sont la transformee des poles
      // de la courbe de base.
      
      Handle(Geom2d_BezierCurve) C = 
	Handle(Geom2d_BezierCurve)::DownCast(Curve->Copy());
      Standard_Integer NbPoles = C->NbPoles();
      TColgp_Array1OfPnt2d Poles(1,NbPoles);
      C->Poles(Poles);
      for ( Standard_Integer i = 1; i <= NbPoles; i++) {
	Poles(i).SetXY(GTrsf.Transformed(Poles(i).XY()));
	C->SetPole(i,Poles(i));
      }
      return C;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_BSplineCurve)) {
      
      // Voir commentaire pour les Bezier.
      
      Handle(Geom2d_BSplineCurve) C = 
	Handle(Geom2d_BSplineCurve)::DownCast(Curve->Copy());
      Standard_Integer NbPoles = C->NbPoles();
      TColgp_Array1OfPnt2d Poles(1,NbPoles);
      C->Poles(Poles);
      for ( Standard_Integer i = 1; i <= NbPoles; i++) {
	Poles(i).SetXY(GTrsf.Transformed(Poles(i).XY()));
	C->SetPole(i,Poles(i));
      }
      return C;
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Circle) ||
	      TheType == STANDARD_TYPE(Geom2d_Ellipse)  ) {
      
      // Dans ces cas, la geometrie de la courbe n`est pas conservee
      // on la convertir en BSpline avant de lui appliquer la Trsf.
      
      Handle(Geom2d_BSplineCurve) C = 
	Geom2dConvert::CurveToBSplineCurve(Curve);
      return GTransform(C, GTrsf);
    }
    else if ( TheType == STANDARD_TYPE(Geom2d_Parabola)   ||
	      TheType == STANDARD_TYPE(Geom2d_Hyperbola)  ||
	      TheType == STANDARD_TYPE(Geom2d_OffsetCurve)  ) {
      
      // On ne sait pas faire : return a null Handle;
      
      Handle(Geom2d_Curve) dummy;
      return dummy;
    }
  }

  Handle(Geom2d_Curve) WNT__; // portage Windows.
  return WNT__;
}


//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================
void GeomLib::SameRange(const Standard_Real         Tolerance,
			const Handle(Geom2d_Curve)& CurvePtr,
			const Standard_Real         FirstOnCurve,
			const Standard_Real         LastOnCurve,
			const Standard_Real         RequestedFirst,
			const Standard_Real         RequestedLast,
			      Handle(Geom2d_Curve)& NewCurvePtr) 
{
  if(CurvePtr.IsNull()) throw Standard_Failure();
  if (Abs(LastOnCurve - RequestedLast) <= Tolerance &&
    Abs(FirstOnCurve - RequestedFirst) <= Tolerance)
  { 
      NewCurvePtr = CurvePtr;
      return;
  }

  // the parametrisation lentgh  must at least be the same.
  if (Abs(LastOnCurve - FirstOnCurve - RequestedLast + RequestedFirst) 
      <= Tolerance)
  { 
    if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Line)))
    {
      Handle(Geom2d_Line) Line =
        Handle(Geom2d_Line)::DownCast(CurvePtr->Copy());
      Standard_Real dU = FirstOnCurve - RequestedFirst;
      gp_Dir2d D = Line->Direction() ;
      Line->Translate(dU * gp_Vec2d(D));
      NewCurvePtr = Line;
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_Circle)))
    {
      gp_Trsf2d Trsf;
      NewCurvePtr = Handle(Geom2d_Curve)::DownCast(CurvePtr->Copy()); 
      Handle(Geom2d_Circle) Circ = 
        Handle(Geom2d_Circle)::DownCast(NewCurvePtr);
      gp_Pnt2d P = Circ->Location();
      Standard_Real dU;
      if (Circ->Circ2d().IsDirect()) {
        dU = FirstOnCurve - RequestedFirst;
      }
      else {
        dU = RequestedFirst - FirstOnCurve;
      }
      Trsf.SetRotation(P,dU);
      NewCurvePtr->Transform(Trsf) ;
    }
    else if (CurvePtr->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) 
    {
      Handle(Geom2d_TrimmedCurve) TC = 
        Handle(Geom2d_TrimmedCurve)::DownCast(CurvePtr);
      GeomLib::SameRange(Tolerance,
        TC->BasisCurve(),
        FirstOnCurve  , LastOnCurve,
        RequestedFirst, RequestedLast,
        NewCurvePtr);
      NewCurvePtr = new Geom2d_TrimmedCurve( NewCurvePtr, RequestedFirst, RequestedLast );
    }
    //
    //  attention a des problemes de limitation : utiliser le MEME test que dans
    //  Geom2d_TrimmedCurve::SetTrim car sinon comme on risque de relimite sur 
    //  RequestedFirst et RequestedLast on aura un probleme
    //
    // 
    else if (Abs(LastOnCurve - FirstOnCurve) > Precision::PConfusion() ||
             Abs(RequestedLast + RequestedFirst) > Precision::PConfusion())
    {

      Handle(Geom2d_TrimmedCurve) TC =
        new Geom2d_TrimmedCurve(CurvePtr,FirstOnCurve,LastOnCurve);

      Handle(Geom2d_BSplineCurve) BS =
        Geom2dConvert::CurveToBSplineCurve(TC);
      TColStd_Array1OfReal Knots(1,BS->NbKnots());
      BS->Knots(Knots);

      BSplCLib::Reparametrize(RequestedFirst,RequestedLast,Knots);

      BS->SetKnots(Knots);
      NewCurvePtr = BS;
    }
  }
  else 
  { // On segmente le resultat
    Handle(Geom2d_TrimmedCurve) TC;
    Handle(Geom2d_Curve) aCCheck = CurvePtr;

    if(aCCheck->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      aCCheck = Handle(Geom2d_TrimmedCurve)::DownCast(aCCheck)->BasisCurve();
    }

    if(aCCheck->IsPeriodic())
    {
      TC = new Geom2d_TrimmedCurve( CurvePtr, FirstOnCurve, LastOnCurve );
    }
    else
    {
      const Standard_Real Udeb = Max(CurvePtr->FirstParameter(), FirstOnCurve);
      const Standard_Real Ufin = Min(CurvePtr->LastParameter(), LastOnCurve);

      TC = new Geom2d_TrimmedCurve( CurvePtr, Udeb, Ufin );
    }

    //
    Handle(Geom2d_BSplineCurve) BS =
      Geom2dConvert::CurveToBSplineCurve(TC);
    TColStd_Array1OfReal Knots(1,BS->NbKnots());
    BS->Knots(Knots);

    BSplCLib::Reparametrize(RequestedFirst,RequestedLast,Knots);

    BS->SetKnots(Knots);
    NewCurvePtr = BS;
  }
}

//=======================================================================
//class : GeomLib_CurveOnSurfaceEvaluator
//purpose: The evaluator for the Curve 3D building
//=======================================================================

class GeomLib_CurveOnSurfaceEvaluator : public AdvApprox_EvaluatorFunction
{
 public:
  GeomLib_CurveOnSurfaceEvaluator (Adaptor3d_CurveOnSurface& theCurveOnSurface,
                                   Standard_Real theFirst, Standard_Real theLast)
    : CurveOnSurface(theCurveOnSurface), FirstParam(theFirst), LastParam(theLast) {}
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Adaptor3d_CurveOnSurface& CurveOnSurface;
  Standard_Real FirstParam;
  Standard_Real LastParam; 

  Handle(Adaptor3d_HCurve) TrimCurve;
};

void GeomLib_CurveOnSurfaceEvaluator::Evaluate (Standard_Integer *,/*Dimension*/
                                                Standard_Real     DebutFin[2],
                                                Standard_Real    *Parameter,
                                                Standard_Integer *DerivativeRequest,
                                                Standard_Real    *Result,// [Dimension]
                                                Standard_Integer *ReturnCode)
{
  gp_Pnt Point;

  //Gestion des positionnements gauche / droite
  if ((DebutFin[0] != FirstParam) || (DebutFin[1] != LastParam)) 
    { 
      TrimCurve = CurveOnSurface.Trim(DebutFin[0], DebutFin[1], Precision::PConfusion());
      FirstParam = DebutFin[0];
      LastParam  = DebutFin[1];
    }

  //Positionemment
  if (*DerivativeRequest == 0)
    {
     TrimCurve->D0((*Parameter), Point) ;
   
     for (Standard_Integer ii = 0 ; ii < 3 ; ii++)
       Result[ii] = Point.Coord(ii + 1);
   }
  if (*DerivativeRequest == 1) 
    {
      gp_Vec Vector;
      TrimCurve->D1((*Parameter), Point, Vector);
      for (Standard_Integer ii = 0 ; ii < 3 ; ii++)
        Result[ii] = Vector.Coord(ii + 1) ;
    }
  if (*DerivativeRequest == 2) 
    {
      gp_Vec Vector, VecBis;
      TrimCurve->D2((*Parameter), Point, VecBis, Vector);
      for (Standard_Integer ii = 0 ; ii < 3 ; ii++)
        Result[ii] = Vector.Coord(ii + 1) ;
    }
  ReturnCode[0] = 0;
}

//=======================================================================
//function : BuildCurve3d
//purpose  : 
//=======================================================================

void GeomLib::BuildCurve3d(const Standard_Real           Tolerance,
			   Adaptor3d_CurveOnSurface&       Curve, 
			   const Standard_Real           FirstParameter,
			   const Standard_Real           LastParameter,
			   Handle(Geom_Curve)&            NewCurvePtr, 
			   Standard_Real&                MaxDeviation,
			   Standard_Real&                AverageDeviation,
			   const GeomAbs_Shape           Continuity,
			   const Standard_Integer        MaxDegree,
			   const Standard_Integer        MaxSegment) 

{
   

  Standard_Integer curve_not_computed = 1 ;
  MaxDeviation     = 0.0e0 ;
  AverageDeviation = 0.0e0 ;
  Handle(GeomAdaptor_HSurface) geom_adaptor_surface_ptr (Handle(GeomAdaptor_HSurface)::DownCast(Curve.GetSurface()) );
  Handle(Geom2dAdaptor_HCurve) geom_adaptor_curve_ptr (Handle(Geom2dAdaptor_HCurve)::DownCast(Curve.GetCurve()) );
   
  if (! geom_adaptor_curve_ptr.IsNull() &&
      ! geom_adaptor_surface_ptr.IsNull()) {
     Handle(Geom_Plane) P ;
     const GeomAdaptor_Surface  &   geom_surface =
       * (GeomAdaptor_Surface *) &geom_adaptor_surface_ptr->Surface() ;

    Handle(Geom_RectangularTrimmedSurface) RT = 
      Handle(Geom_RectangularTrimmedSurface)::
	DownCast(geom_surface.Surface());
    if ( RT.IsNull()) {
      P = Handle(Geom_Plane)::DownCast(geom_surface.Surface());
    }
    else {
      P = Handle(Geom_Plane)::DownCast(RT->BasisSurface());
    }

   
    if (! P.IsNull()) {
      // compute the 3d curve
      gp_Ax2 axes = P->Position().Ax2();
      const Geom2dAdaptor_Curve & geom2d_curve =
	* (Geom2dAdaptor_Curve *) & geom_adaptor_curve_ptr->Curve2d() ;
      NewCurvePtr = 
	GeomLib::To3d(axes,
		      geom2d_curve.Curve());
     curve_not_computed = 0 ;
      
    }
  }
  if (curve_not_computed) {

      //
      // Entree
      //
    Handle(TColStd_HArray1OfReal)   Tolerance1DPtr,Tolerance2DPtr; 
    Handle(TColStd_HArray1OfReal) Tolerance3DPtr =
      new TColStd_HArray1OfReal(1,1) ;
    Tolerance3DPtr->SetValue(1,Tolerance);

     // Recherche des discontinuitees
     Standard_Integer NbIntervalC2 = Curve.NbIntervals(GeomAbs_C2);
     TColStd_Array1OfReal Param_de_decoupeC2 (1, NbIntervalC2+1);
     Curve.Intervals(Param_de_decoupeC2, GeomAbs_C2);
     
     Standard_Integer NbIntervalC3 = Curve.NbIntervals(GeomAbs_C3);
     TColStd_Array1OfReal Param_de_decoupeC3 (1, NbIntervalC3+1);
     Curve.Intervals(Param_de_decoupeC3, GeomAbs_C3);

     // Note extension of the parameteric range  
     // Pour forcer le Trim au premier appel de l'evaluateur
     GeomLib_CurveOnSurfaceEvaluator ev (Curve, FirstParameter - 1., LastParameter  + 1.);
                                         
     // Approximation avec decoupe preferentiel
     AdvApprox_PrefAndRec Preferentiel(Param_de_decoupeC2,
				       Param_de_decoupeC3);
     AdvApprox_ApproxAFunction  anApproximator(0,
					      0,
					      1,
					      Tolerance1DPtr,
					      Tolerance2DPtr,
					      Tolerance3DPtr,
					      FirstParameter,
					      LastParameter,
					      Continuity,
					      MaxDegree,  
					      MaxSegment,
					      ev,
//					      CurveOnSurfaceEvaluator,
					      Preferentiel) ;
    
    if (anApproximator.HasResult()) {
      GeomLib_MakeCurvefromApprox 
	aCurveBuilder(anApproximator) ;    

      Handle(Geom_BSplineCurve) aCurvePtr = 
	aCurveBuilder.Curve(1) ;
      // On rend les resultats de l'approx
      MaxDeviation = anApproximator.MaxError(3,1) ;
      AverageDeviation = anApproximator.AverageError(3,1) ;
      NewCurvePtr = aCurvePtr ;
    }
  }  
 }

//=======================================================================
//function :  AdjustExtremity
//purpose  : 
//=======================================================================

void GeomLib::AdjustExtremity(Handle(Geom_BoundedCurve)& Curve, 
			      const gp_Pnt& P1,
			      const gp_Pnt& P2,
			      const gp_Vec& T1,
			      const gp_Vec& T2)
{
// il faut Convertir l'entree (en preservant si possible le parametrage)
  Handle(Geom_BSplineCurve) aIn, aDef;  
  aIn = GeomConvert::CurveToBSplineCurve(Curve, Convert_QuasiAngular);

  Standard_Integer ii, jj;
  gp_Pnt P;
  gp_Vec V, Vtan, DV;
  TColgp_Array1OfPnt PolesDef(1,4), Coeffs(1,4);
  TColStd_Array1OfReal FK(1, 8);
  TColStd_Array1OfReal Ti(1, 4);
  TColStd_Array1OfInteger Contact(1, 4);

  Ti(1) = Ti(2) = aIn->FirstParameter();
  Ti(3) = Ti(4) = aIn->LastParameter();
  Contact(1) =  Contact(3) = 0;
  Contact(2) =  Contact(4) = 1;
  for (ii=1; ii<=4; ii++) {
    FK(ii) = aIn->FirstParameter();
    FK(ii) = aIn->LastParameter();
  }

  // Calculs des contraintes de deformations
  aIn->D1(Ti(1), P, V);
  PolesDef(1).ChangeCoord() = P1.XYZ()-P.XYZ();
  Vtan = T1;
  Vtan.Normalize();
  DV = Vtan * (Vtan * V) - V;
  PolesDef(2).ChangeCoord() = (Ti(4)-Ti(1))*DV.XYZ();

  aIn->D1(Ti(4), P, V);
  PolesDef(3).ChangeCoord() = P2.XYZ()-P.XYZ();
  Vtan = T2;
  Vtan.Normalize();
  DV = Vtan * (Vtan * V) - V;
  PolesDef(4).ChangeCoord() = (Ti(4)-Ti(1))* DV.XYZ();
 
  // Interpolation des contraintes
  math_Matrix Mat(1, 4, 1, 4);
  if (!PLib::HermiteCoefficients(0., 1., 1, 1, Mat)) 
    throw Standard_ConstructionError();

  for (jj=1; jj<=4; jj++) {
    gp_XYZ aux(0.,0.,0.);
    for (ii=1; ii<=4; ii++) {
      aux.SetLinearForm(Mat(ii,jj), PolesDef(ii).XYZ(), aux);
    }
    Coeffs(jj).SetXYZ(aux);
  }

  PLib::CoefficientsPoles(Coeffs, PLib::NoWeights(),
			  PolesDef,  PLib::NoWeights());

  // Ajout de la deformation
  TColStd_Array1OfReal K(1, 2);
  TColStd_Array1OfInteger M(1, 2);
  K(1) = Ti(1);
  K(2) = Ti(4);
  M.Init(4);

  aDef = new (Geom_BSplineCurve) (PolesDef, K, M, 3);
  if (aIn->Degree() < 3) aIn->IncreaseDegree(3);
  else aDef->IncreaseDegree(aIn->Degree());

  for (ii=2; ii<aIn->NbKnots(); ii++) {
    aDef->InsertKnot(aIn->Knot(ii), aIn->Multiplicity(ii));
  }

  if (aDef->NbPoles() != aIn->NbPoles()) 
    throw Standard_ConstructionError("Inconsistent poles's number");

  for (ii=1; ii<=aDef->NbPoles(); ii++) {
    P = aIn->Pole(ii);
    P.ChangeCoord() += aDef->Pole(ii).XYZ();
    aIn->SetPole(ii, P);
  }
  Curve = aIn;
}
//=======================================================================
//function : ExtendCurveToPoint
//purpose  : 
//=======================================================================

void GeomLib::ExtendCurveToPoint(Handle(Geom_BoundedCurve)& Curve, 
				 const gp_Pnt& Point,
				 const Standard_Integer Continuity,
				 const Standard_Boolean After)
{
  if(Continuity < 1 || Continuity > 3) return;
  Standard_Integer size = Continuity + 2;
  Standard_Real Ubord, Tol=1.e-6;
  math_Matrix  MatCoefs(1,size, 1,size);
  Standard_Real Lambda, L1;
  Standard_Integer ii, jj;
  gp_Vec d1, d2, d3;
  gp_Pnt p0;
// il faut Convertir l'entree (en preservant si possible le parametrage)
  GeomConvert_CompCurveToBSplineCurve Concat(Curve, Convert_QuasiAngular);

// Les contraintes de constructions
  TColgp_Array1OfXYZ Cont(1,size);
  if (After) {
     Ubord = Curve->LastParameter();
    
   }
  else {
     Ubord = Curve->FirstParameter(); 
   }
  PLib::HermiteCoefficients(0, 1,           // Les Bornes
			    Continuity, 0,  // Les Ordres de contraintes
			    MatCoefs);

  Curve->D3(Ubord, p0, d1, d2, d3);
  if (!After) { // Inversion du parametrage
    d1 *= -1;
    d3 *= -1;
  }
  
  L1 = p0.Distance(Point);
  if (L1 > Tol) {
    // Lambda est le ratio qu'il faut appliquer a la derive de la courbe
    // pour obtenir la derive du prolongement (fixe arbitrairement a la
    // longueur du segment bout de la courbe - point cible.
    // On essai d'avoir sur le prolongement la vitesse moyenne que l'on
    // a sur la courbe.
    gp_Vec daux;
    gp_Pnt pp;
    Standard_Real f= Curve->FirstParameter(), t, dt, norm; 
    dt = (Curve->LastParameter()-f)/9;
    norm = d1.Magnitude();
    for (ii=1, t=f+dt; ii<=8; ii++, t+=dt) {
      Curve->D1(t, pp, daux);
      norm += daux.Magnitude();
    }
    norm /= 9;
    dt = d1.Magnitude() / norm;
    if ((dt<1.5) && (dt>0.75)) { // Le bord est dans la moyenne on le garde
      Lambda = ((Standard_Real)1) / Max (d1.Magnitude() / L1, Tol);
    }
    else {
      Lambda = ((Standard_Real)1) / Max (norm / L1, Tol);
    }
  }
  else {
    return; // Pas d'extension
  }

  // Optimisation du Lambda
  math_Matrix Cons(1, 3, 1, size);
  Cons(1,1) = p0.X();  Cons(2,1) = p0.Y(); Cons(3,1) = p0.Z();
  Cons(1,2) = d1.X();  Cons(2,2) = d1.Y(); Cons(3,2) = d1.Z();
  Cons(1,size) = Point.X();  Cons(2,size) = Point.Y(); Cons(3,size) = Point.Z();
  if (Continuity >= 2) {
     Cons(1,3) = d2.X();  Cons(2,3) = d2.Y(); Cons(3,3) = d2.Z(); 
  }
  if (Continuity >= 3) {
     Cons(1,4) = d3.X();  Cons(2,4) = d3.Y(); Cons(3,4) = d3.Z(); 
  }
  ComputeLambda(Cons, MatCoefs, L1, Lambda);

  // Construction dans la Base Polynomiale
  Cont(1) = p0.XYZ();
  Cont(2) = d1.XYZ() * Lambda;
  if(Continuity >= 2) Cont(3) = d2.XYZ() * Pow(Lambda,2);
  if(Continuity >= 3) Cont(4) = d3.XYZ() * Pow(Lambda,3);
  Cont(size) = Point.XYZ();
    

  TColgp_Array1OfPnt ExtrapPoles(1, size);
  TColgp_Array1OfPnt ExtraCoeffs(1, size);

  gp_Pnt PNull(0.,0.,0.);
  ExtraCoeffs.Init(PNull);
  for (ii=1; ii<=size; ii++) {
    for (jj=1; jj<=size; jj++) {
      ExtraCoeffs(jj).ChangeCoord() += MatCoefs(ii,jj)*Cont(ii);
    }
  }

  // Convertion Dans la Base de Bernstein
  PLib::CoefficientsPoles(ExtraCoeffs,  PLib::NoWeights(),
			  ExtrapPoles,  PLib::NoWeights());
  
  Handle(Geom_BezierCurve) Bezier = new (Geom_BezierCurve) (ExtrapPoles);

  Standard_Real dist = ExtrapPoles(1).Distance(p0);
  Standard_Boolean Ok;
  Tol += dist;

  // Concatenation
  Ok = Concat.Add(Bezier, Tol, After);
  if (!Ok) throw Standard_ConstructionError("ExtendCurveToPoint");
  
  Curve =  Concat.BSplineCurve();
}


//=======================================================================
//function : ExtendKPart
//purpose  : Extension par longueur des surfaces cannonique
//=======================================================================
static Standard_Boolean 
ExtendKPart(Handle(Geom_RectangularTrimmedSurface)& Surface, 
	    const Standard_Real Length,
	    const Standard_Boolean InU,
	    const Standard_Boolean After)
{

  if  (Surface.IsNull()) return Standard_False;

  Standard_Boolean Ok=Standard_True;
  Standard_Real Uf, Ul, Vf, Vl;
  Handle(Geom_Surface) Support = Surface->BasisSurface();
  GeomAbs_SurfaceType Type;

  Surface->Bounds(Uf, Ul, Vf, Vl);
  GeomAdaptor_Surface AS(Surface);
  Type = AS.GetType();

  if (InU) {
    switch(Type) {
    case GeomAbs_Plane :
      {
	if (After) Ul+=Length;
	else       Uf-=Length;
	Surface = new (Geom_RectangularTrimmedSurface)
	  (Support, Uf, Ul, Vf, Vl);
	break;
      }

    default:
      Ok = Standard_False;
    }
  }
  else {
    switch(Type) {
    case GeomAbs_Plane :
    case GeomAbs_Cylinder :
    case GeomAbs_SurfaceOfExtrusion :
      {
	if (After) Vl+=Length;
	else       Vf-=Length;
	Surface = new (Geom_RectangularTrimmedSurface)
	  (Support, Uf, Ul, Vf, Vl);
	break;
      }    
    default:
      Ok = Standard_False;
    }
  }

  return Ok;
}

//=======================================================================
//function : ExtendSurfByLength
//purpose  : 
//=======================================================================
void GeomLib::ExtendSurfByLength(Handle(Geom_BoundedSurface)& Surface, 
				 const Standard_Real Length,
				 const Standard_Integer Continuity,
				 const Standard_Boolean InU,
				 const Standard_Boolean After)
{
  if(Continuity < 0 || Continuity > 3) return;
  Standard_Integer Cont = Continuity;

  // Kpart ?
  Handle(Geom_RectangularTrimmedSurface) TS = 
    Handle(Geom_RectangularTrimmedSurface)::DownCast (Surface);
  if (ExtendKPart(TS,Length, InU, After) ) {
    Surface = TS;
    return;
  }

//  format BSplineSurface avec un degre suffisant pour la continuite voulue
  Handle(Geom_BSplineSurface) BS = 
    Handle(Geom_BSplineSurface)::DownCast (Surface);
  if (BS.IsNull()) {
    //BS = GeomConvert::SurfaceToBSplineSurface(Surface);
    Standard_Real Tol = Precision::Confusion(); //1.e-4;
    GeomAbs_Shape UCont = GeomAbs_C1, VCont = GeomAbs_C1;
    Standard_Integer degU = 14, degV = 14;
    Standard_Integer nmax = 16;
    Standard_Integer thePrec = 1; 
    const Handle(Geom_Surface)& aSurf = Surface; // to resolve ambiguity
    GeomConvert_ApproxSurface theApprox(aSurf,Tol,UCont,VCont,degU,degV,nmax,thePrec);
    if (theApprox.HasResult())
      BS = theApprox.Surface();
    else
      BS = GeomConvert::SurfaceToBSplineSurface(Surface);
  }
  if (InU&&(BS->UDegree()<Continuity+1)) 
    BS->IncreaseDegree(Continuity+1,BS->VDegree());      
  if (!InU&&(BS->VDegree()<Continuity+1))
    BS->IncreaseDegree(BS->UDegree(),Continuity+1);      

  // si BS etait periodique dans le sens de l'extension, elle ne le sera plus
  if ( (InU&&(BS->IsUPeriodic())) || (!InU&&(BS->IsVPeriodic())) ) {
    Standard_Real U0,U1,V0,V1;
    BS->Bounds(U0,U1,V0,V1);
    BS->Segment(U0,U1,V0,V1);
  }     


// IFV Fix OCC bug 0022694 - wrong result extrapolating rational surfaces
//   Standard_Boolean rational = ( InU && BS->IsURational() ) 
//                                   || ( !InU && BS->IsVRational() ) ;
  Standard_Boolean rational = (BS->IsURational() ||  BS->IsVRational());
  Standard_Boolean NullWeight;
   Standard_Real EpsW = 10*Precision::PConfusion();
  Standard_Integer gap = 3;
  if ( rational ) gap++;


        
  Standard_Integer Cdeg = 0, Cdim = 0, NbP = 0, Ksize = 0, Psize = 1;
  Standard_Integer ii, jj, ipole, Kount;  
  Standard_Real Tbord, lambmin=Length;
  Standard_Real * Padr = NULL;
  Standard_Boolean Ok;
  Handle(TColStd_HArray1OfReal)  FKnots, Point, lambda, Tgte, Poles;

  


  for (Kount=0, Ok=Standard_False; Kount<=2 && !Ok; Kount++) {
    //  transformation de la surface en une BSpline non rationnelle a une variable
    //  de degre UDegree ou VDegree et de dimension 3 ou 4 x NbVpoles ou NbUpoles
    //  le nombre de poles egal a NbUpoles ou NbVpoles
    //  ATTENTION : dans le cas rationnel, un point de coordonnees (x,y,z)
    //              et de poids w devient un point de coordonnees (wx, wy, wz, w )
  

    if (InU) {
      Cdeg = BS->UDegree();
      NbP = BS->NbUPoles();
      Cdim = BS->NbVPoles() * gap;
    }
    else {
      Cdeg = BS->VDegree();
      NbP = BS->NbVPoles();
      Cdim = BS->NbUPoles() * gap;
    }

    //  les noeuds plats
    Ksize = NbP + Cdeg + 1;
    FKnots = new (TColStd_HArray1OfReal) (1,Ksize);
    if (InU) 
      BS->UKnotSequence(FKnots->ChangeArray1());
    else 
      BS->VKnotSequence(FKnots->ChangeArray1());

    //  le parametre du noeud de raccord
    if (After)
      Tbord = FKnots->Value(FKnots->Upper()-Cdeg);
    else
      Tbord = FKnots->Value(FKnots->Lower()+Cdeg);

    //  les poles
    Psize = Cdim * NbP;
    Poles = new (TColStd_HArray1OfReal) (1,Psize);

    if (InU) {
      for (ii=1,ipole=1; ii<=NbP; ii++) {
	for (jj=1;jj<=BS->NbVPoles();jj++) {
	  Poles->SetValue(ipole,   BS->Pole(ii,jj).X());
	  Poles->SetValue(ipole+1, BS->Pole(ii,jj).Y());
	  Poles->SetValue(ipole+2, BS->Pole(ii,jj).Z());
	  if (rational) Poles->SetValue(ipole+3, BS->Weight(ii,jj));
	  ipole+=gap;
	}
      }
    }
    else {
      for (jj=1,ipole=1; jj<=NbP; jj++) {
	for (ii=1;ii<=BS->NbUPoles();ii++) {
	  Poles->SetValue(ipole,   BS->Pole(ii,jj).X());
	  Poles->SetValue(ipole+1, BS->Pole(ii,jj).Y());
	  Poles->SetValue(ipole+2, BS->Pole(ii,jj).Z());
	  if (rational) Poles->SetValue(ipole+3, BS->Weight(ii,jj));
	  ipole+=gap;
	}
      }
    }
    Padr = (Standard_Real *) &Poles->ChangeValue(1);

    //  calcul du point de raccord et de la tangente
    Point = new (TColStd_HArray1OfReal)(1,Cdim);
    Tgte  = new (TColStd_HArray1OfReal)(1,Cdim);
    lambda = new (TColStd_HArray1OfReal)(1,Cdim);

    Standard_Boolean  periodic_flag = Standard_False ;
    Standard_Integer extrap_mode[2], derivative_request = Max(Continuity,1);
    extrap_mode[0] = extrap_mode[1] = Cdeg;
    TColStd_Array1OfReal  Result(1, Cdim * (derivative_request+1)) ; 
    
    TColStd_Array1OfReal& tgte = Tgte->ChangeArray1();
    TColStd_Array1OfReal& point = Point->ChangeArray1();
    TColStd_Array1OfReal& lamb = lambda->ChangeArray1();

    Standard_Real * Radr = (Standard_Real *) &Result(1) ;

    BSplCLib::Eval(Tbord,periodic_flag,derivative_request,extrap_mode[0],
		   Cdeg,FKnots->Array1(),Cdim,*Padr,*Radr);
    Ok = Standard_True;
    for (ii=1;ii<=Cdim;ii++) {
      point(ii) = Result(ii);
      tgte(ii) = Result(ii+Cdim);
    }
  
    //  calcul de la contrainte a atteindre

    gp_Vec CurT, OldT;
  
    Standard_Real NTgte, val, Tgtol = 1.e-12, OldN = 0.0;
    if (rational) {
      for (ii=gap;ii<=Cdim;ii+=gap) {
	tgte(ii) = 0.;
      }
      for (ii=gap;ii<=Cdim;ii+=gap) {
	CurT.SetCoord(tgte(ii-3),tgte(ii-2), tgte(ii-1)); 
	NTgte=CurT.Magnitude();
	if (NTgte>Tgtol) {
	  val =  Length/NTgte;
	  // Attentions aux Cas ou le segment donne par les poles 
	  // est oppose au sens de la derive
	  // Exemple: Certaine portions de tore.
	  if ( (OldN > Tgtol) && (CurT.Angle(OldT) > 2)) {
	    Ok = Standard_False;
	  }

	  lamb(ii-1) = lamb(ii-2) = lamb(ii-3) = val;
	  lamb(ii) = 0.;
	  lambmin = Min(lambmin, val);
	}
	else {
	  lamb(ii-1) = lamb(ii-2) = lamb(ii-3) = 0.;
	  lamb(ii) = 0.;
	}
	OldT = CurT;
	OldN = NTgte;
      }
    }
    else {
      for (ii=gap;ii<=Cdim;ii+=gap) {
	CurT.SetCoord(tgte(ii-2),tgte(ii-1), tgte(ii)); 
	NTgte=CurT.Magnitude();
	if (NTgte>Tgtol) {
	  val =  Length/NTgte;
	  // Attentions aux Cas ou le segment donne par les poles 
	  // est oppose au sens de la derive
	  // Exemple: Certaine portion de tore.
	  if ( (OldN > Tgtol) && (CurT.Angle(OldT) > 2)) {
	     Ok = Standard_False;
	  }
	  lamb(ii) = lamb(ii-1) = lamb(ii-2) = val;
	  lambmin = Min(lambmin, val);
	}
	else {
	  lamb(ii) =lamb(ii-1) = lamb(ii-2) = 0.;
	}
	OldT = CurT;
	OldN = NTgte;
      }
    }
    if (!Ok && Kount<2) {
      // On augmente le degre de l'iso bord afin de rapprocher les poles de la surface
      // Et on ressaye
      if (InU) BS->IncreaseDegree(BS->UDegree(), BS->VDegree()+2);
      else     BS->IncreaseDegree(BS->UDegree()+2, BS->VDegree());
    }
  }


  TColStd_Array1OfReal ConstraintPoint(1,Cdim);
  if (After) {
    for (ii=1;ii<=Cdim;ii++) {
      ConstraintPoint(ii) = Point->Value(ii) + lambda->Value(ii)*Tgte->Value(ii);
    }
  }
  else {
    for (ii=1;ii<=Cdim;ii++) {
      ConstraintPoint(ii) = Point->Value(ii) - lambda->Value(ii)*Tgte->Value(ii);
    }
  }

//  cas particulier du rationnel
  if (rational) {
    for (ipole=1;ipole<=Psize;ipole+=gap) {
      Poles->ChangeValue(ipole) *= Poles->Value(ipole+3);
      Poles->ChangeValue(ipole+1) *= Poles->Value(ipole+3);
      Poles->ChangeValue(ipole+2) *= Poles->Value(ipole+3);
    }
    for (ii=1;ii<=Cdim;ii+=gap) {
      ConstraintPoint(ii) *= ConstraintPoint(ii+3);
      ConstraintPoint(ii+1) *= ConstraintPoint(ii+3);
      ConstraintPoint(ii+2) *= ConstraintPoint(ii+3);
    }
  }
  
//  tableaux necessaires pour l'extension
  Standard_Integer Ksize2 = Ksize+Cdeg, NbPoles, NbKnots = 0;
  TColStd_Array1OfReal  FK(1, Ksize2) ; 
  Standard_Real * FKRadr = &FK(1);

  Standard_Integer Psize2 = Psize+Cdeg*Cdim;
  TColStd_Array1OfReal  PRes(1, Psize2) ; 
  Standard_Real * PRadr = &PRes(1);
  Standard_Real ww;
  Standard_Boolean ExtOk = Standard_False;
  Handle(TColgp_HArray2OfPnt) NewPoles;
  Handle(TColStd_HArray2OfReal) NewWeights;


  for (Kount=1; Kount<=5 && !ExtOk; Kount++) {
    //  extension
    BSplCLib::TangExtendToConstraint(FKnots->Array1(),
				     lambmin,NbP,*Padr,
				     Cdim,Cdeg,
				     ConstraintPoint, Cont, After,
				     NbPoles, NbKnots,*FKRadr, *PRadr);

    //  recopie des poles du resultat sous forme de points 3D et de poids
    Standard_Integer NU, NV, indice ;
    if (InU) {
      NU = NbPoles;
      NV = BS->NbVPoles();
    }
    else {
      NU = BS->NbUPoles();
      NV = NbPoles;
    }

    NewPoles = new (TColgp_HArray2OfPnt)(1,NU,1,NV);
    TColgp_Array2OfPnt& NewP = NewPoles->ChangeArray2();
    NewWeights = new (TColStd_HArray2OfReal) (1,NU,1,NV);
    TColStd_Array2OfReal& NewW = NewWeights->ChangeArray2();

    if (!rational) NewW.Init(1.);
    NullWeight= Standard_False;

    if (InU) {
      for (ii=1; ii<=NU && !NullWeight; ii++) {
	for (jj=1; jj<=NV && !NullWeight; jj++) {
	  indice = 1+(ii-1)*Cdim+(jj-1)*gap;
	  NewP(ii,jj).SetCoord(1,PRes(indice));
	  NewP(ii,jj).SetCoord(2,PRes(indice+1));
	  NewP(ii,jj).SetCoord(3,PRes(indice+2));
	  if (rational) {
	    ww =  PRes(indice+3);
	    if (Abs(ww - 1.0) < EpsW)
	      ww = 1.0;
	    if (ww < EpsW) {
	      NullWeight = Standard_True;
	    }
	    else {
	      NewW(ii,jj) = ww;
	      NewP(ii,jj).ChangeCoord() /= ww;
	    }
	  }
	}
      }
    }
    else {
      for (jj=1; jj<=NV && !NullWeight; jj++) {
	for (ii=1; ii<=NU && !NullWeight; ii++) {
	  indice = 1+(ii-1)*gap+(jj-1)*Cdim;
	  NewP(ii,jj).SetCoord(1,PRes(indice));
	  NewP(ii,jj).SetCoord(2,PRes(indice+1));
	  NewP(ii,jj).SetCoord(3,PRes(indice+2));
	  if (rational) {
	    ww =  PRes(indice+3);
	    if (Abs(ww - 1.0) < EpsW)
	      ww = 1.0;
	    if (ww < EpsW) {
	      NullWeight = Standard_True;
	    }
	    else {
	      NewW(ii,jj) = ww;
	      NewP(ii,jj).ChangeCoord() /= ww;
	    }
	  }
	}
      }
    }

    if (NullWeight) {
#ifdef OCCT_DEBUG
      cout << "Echec de l'Extension rationnelle" << endl;    
#endif
      lambmin /= 3.;
      NullWeight = Standard_False;
    }
    else {
      ExtOk = Standard_True;
    }
  }
    

// recopie des noeuds plats sous forme de noeuds avec leurs multiplicites
// calcul des degres du resultat
  Standard_Integer Usize = BS->NbUKnots(), Vsize = BS->NbVKnots(), UDeg, VDeg;
  if (InU) 
    Usize++;
  else
    Vsize++;
  TColStd_Array1OfReal UKnots(1,Usize);
  TColStd_Array1OfReal VKnots(1,Vsize);
  TColStd_Array1OfInteger UMults(1,Usize);
  TColStd_Array1OfInteger VMults(1,Vsize);
  TColStd_Array1OfReal FKRes(1, NbKnots);

  for (ii=1; ii<=NbKnots; ii++)
     FKRes(ii) = FK(ii);

  if (InU) {
    BSplCLib::Knots(FKRes, UKnots, UMults);
    UDeg = Cdeg;
    UMults(Usize) = UDeg+1; // Petite verrue utile quand la continuite 
                             // n'est pas ok.
    BS->VKnots(VKnots);
    BS->VMultiplicities(VMults);
    VDeg = BS->VDegree();
  }
  else {
    BSplCLib::Knots(FKRes, VKnots, VMults);
    VDeg = Cdeg;
    VMults(Vsize) = VDeg+1;
    BS->UKnots(UKnots);
    BS->UMultiplicities(UMults);
    UDeg = BS->UDegree();
  }

//  construction de la surface BSpline resultat
  Handle(Geom_BSplineSurface) Res = 
    new (Geom_BSplineSurface) (NewPoles->Array2(),
			       NewWeights->Array2(),
			       UKnots,VKnots,
			       UMults,VMults,
			       UDeg,VDeg,
			       BS->IsUPeriodic(),
			       BS->IsVPeriodic());
  Surface = Res;
}

//=======================================================================
//function : Inertia
//purpose  : 
//=======================================================================
void GeomLib::Inertia(const TColgp_Array1OfPnt& Points,
		      gp_Pnt& Bary,
		      gp_Dir& XDir,
		      gp_Dir& YDir,
		      Standard_Real& Xgap,
		      Standard_Real& Ygap,
		      Standard_Real& Zgap)
{
  gp_XYZ GB(0., 0., 0.), Diff;
//  gp_Vec A,B,C,D;

  Standard_Integer i,nb=Points.Length();
  GB.SetCoord(0.,0.,0.);
  for (i=1; i<=nb; i++) 
      GB += Points(i).XYZ();

  GB /= nb;

  math_Matrix M (1, 3, 1, 3);
  M.Init(0.);
  for (i=1; i<=nb; i++) {
    Diff.SetLinearForm(-1, Points(i).XYZ(), GB);
    M(1,1) += Diff.X() *  Diff.X();
    M(2,2) += Diff.Y() *  Diff.Y();
    M(3,3) += Diff.Z() *  Diff.Z();
    M(1,2) += Diff.X() *  Diff.Y();
    M(1,3) += Diff.X() *  Diff.Z();
    M(2,3) += Diff.Y() *  Diff.Z();
  }

  M(2,1)=M(1,2) ;
  M(3,1)=M(1,3) ;
  M(3,2)=M(2,3) ;

  M /= nb;

  math_Jacobi J(M);
  if (!J.IsDone()) {
#ifdef OCCT_DEBUG
    cout << "Erreur dans Jacobbi" << endl;
    M.Dump(cout);
#endif
  }

  Standard_Real n1,n2,n3;

  n1=J.Value(1);
  n2=J.Value(2);
  n3=J.Value(3);

  Standard_Real r1 = Min(Min(n1,n2),n3), r2;
  Standard_Integer m1, m2, m3;
  if (r1==n1) {
    m1 = 1;
    r2 = Min(n2,n3);
    if (r2==n2) {
      m2 = 2;
      m3 = 3;
    }
    else {
      m2 = 3;
      m3 = 2;
    }
  }
  else {
    if (r1==n2) {
      m1 = 2 ;
      r2 = Min(n1,n3);
      if (r2==n1) {
	m2 = 1;
	m3 = 3;
      }
      else {
	m2 = 3;
	m3 = 1;
      }
    }
    else {
      m1 = 3 ;
      r2 = Min(n1,n2);
      if (r2==n1) {
	m2 = 1;
	m3 = 2;
      }
      else {
	m2 = 2;
	m3 = 1;
      }
    }
  }

  math_Vector V2(1,3),V3(1,3);
  J.Vector(m2,V2);
  J.Vector(m3,V3);
  
  Bary.SetXYZ(GB);
  XDir.SetCoord(V3(1),V3(2),V3(3));
  YDir.SetCoord(V2(1),V2(2),V2(3));

  Zgap = sqrt(Abs(J.Value(m1)));
  Ygap = sqrt(Abs(J.Value(m2)));
  Xgap = sqrt(Abs(J.Value(m3)));
}
//=======================================================================
//function : AxeOfInertia
//purpose  : 
//=======================================================================
void GeomLib::AxeOfInertia(const TColgp_Array1OfPnt& Points,
			   gp_Ax2& Axe,
			   Standard_Boolean& IsSingular,
			   const Standard_Real Tol)
{
  gp_Pnt Bary;
  gp_Dir OX,OY,OZ;
  Standard_Real gx, gy, gz;

  GeomLib::Inertia(Points, Bary, OX, OY, gx, gy, gz);
  
  if (gy*Points.Length()<=Tol) {
    gp_Ax2 axe (Bary, OX);
    OY = axe.XDirection();
    IsSingular = Standard_True;
  }
  else {
    IsSingular = Standard_False;
  }

  OZ = OX^OY;
  gp_Ax2 TheAxe(Bary, OZ, OX);
  Axe = TheAxe;
}

//=======================================================================
//function : CanBeTreated
//purpose  : indicates if the surface can be treated(if the conditions are
//           filled) and need to be treated(if the surface hasn't been yet
//           treated or if the surface is rationnal and non periodic)
//=======================================================================

static Standard_Boolean CanBeTreated(Handle(Geom_BSplineSurface)& BSurf)
     
{Standard_Integer i;
 Standard_Real    lambda;                                    //proportionnality coefficient
 Standard_Boolean AlreadyTreated=Standard_True;
 
 if (!BSurf->IsURational()||(BSurf->IsUPeriodic()))
   return Standard_False;
 else {
   lambda=(BSurf->Weight(1,1)/BSurf->Weight(BSurf->NbUPoles(),1));
   for (i=1;i<=BSurf->NbVPoles();i++)      //test of the proportionnality of the denominator on the boundaries
     if ((BSurf->Weight(1,i)/(lambda*BSurf->Weight(BSurf->NbUPoles(),i))<(1-Precision::Confusion()))||
	 (BSurf->Weight(1,i)/(lambda*BSurf->Weight(BSurf->NbUPoles(),i))>(1+Precision::Confusion())))
       return Standard_False;
   i=1;
   while ((AlreadyTreated) && (i<=BSurf->NbVPoles())){        //tests if the surface has already been treated
     if (((BSurf->Weight(1,i)/(BSurf->Weight(2,i)))<(1-Precision::Confusion()))||
	 ((BSurf->Weight(1,i)/(BSurf->Weight(2,i)))>(1+Precision::Confusion()))||
	 ((BSurf->Weight(BSurf->NbUPoles()-1,i)/(BSurf->Weight(BSurf->NbUPoles(),i)))<(1-Precision::Confusion()))||
	 ((BSurf->Weight(BSurf->NbUPoles()-1,i)/(BSurf->Weight(BSurf->NbUPoles(),i)))>(1+Precision::Confusion())))
       AlreadyTreated=Standard_False;
     i++;
   }
   if (AlreadyTreated)
     return Standard_False;
 }
 return Standard_True;  
}

//=======================================================================
//class   : law_evaluator
//purpose : usefull to estimate the value of a function of 2 variables
//=======================================================================

class law_evaluator : public BSplSLib_EvaluatorFunction
{

public:

  law_evaluator (const GeomLib_DenominatorMultiplierPtr theDenominatorPtr)
  : myDenominator (theDenominatorPtr) {}

  virtual void Evaluate (const Standard_Integer theDerivativeRequest,
                         const Standard_Real    theUParameter,
                         const Standard_Real    theVParameter,
                         Standard_Real&         theResult,
                         Standard_Integer&      theErrorCode) const
  {
    if ((myDenominator != NULL) && (theDerivativeRequest == 0))
    {
      theResult = myDenominator->Value (theUParameter, theVParameter);
      theErrorCode = 0;
    }
    else
    {
      theErrorCode = 1;
    }
  }

private:

  GeomLib_DenominatorMultiplierPtr myDenominator;

};
 
//=======================================================================
//function : CheckIfKnotExists
//purpose  : true if the knot already exists in the knot sequence
//=======================================================================

static Standard_Boolean CheckIfKnotExists(const TColStd_Array1OfReal&           surface_knots,
					  const Standard_Real                   knot)

{Standard_Integer    i;
 for (i=1;i<=surface_knots.Length();i++)
   if ((surface_knots(i)-Precision::Confusion()<=knot)&&(surface_knots(i)+Precision::Confusion()>=knot))
     return Standard_True;
 return Standard_False;
}

//=======================================================================
//function : AddAKnot
//purpose  : add a knot and its multiplicity to the knot sequence. This knot
//           will be C2 and the degree is increased of deltasurface_degree 
//=======================================================================

static void AddAKnot(const TColStd_Array1OfReal&           knots,
		     const TColStd_Array1OfInteger&        mults,
		     const Standard_Real                   knotinserted,
		     const Standard_Integer                deltasurface_degree,
		     const Standard_Integer                finalsurfacedegree,
		     Handle(TColStd_HArray1OfReal) &       newknots,
		     Handle(TColStd_HArray1OfInteger) &    newmults)

{Standard_Integer      i;

 newknots=new TColStd_HArray1OfReal(1,knots.Length()+1);
 newmults=new TColStd_HArray1OfInteger(1,knots.Length()+1); 
 i=1;
 while (knots(i)<knotinserted){
   newknots->SetValue(i,knots(i));
   newmults->SetValue(i,mults(i)+deltasurface_degree);
   i++;
 }
 newknots->SetValue(i,knotinserted);                        //insertion of the new knot
 newmults->SetValue(i,finalsurfacedegree-2);
 i++;
 while (i<=newknots->Length()){
   newknots->SetValue(i,knots(i-1));
   newmults->SetValue(i,mults(i-1)+deltasurface_degree);
   i++;
 }
}

//=======================================================================
//function : Sort
//purpose  : give the new flat knots(u or v) of the surface 
//=======================================================================

static void BuildFlatKnot(const TColStd_Array1OfReal&           surface_knots,    
		 const TColStd_Array1OfInteger&        surface_mults,    
		 const Standard_Integer                deltasurface_degree, 
		 const Standard_Integer                finalsurface_degree, 
		 const Standard_Real                   knotmin,
		 const Standard_Real                   knotmax,
		 Handle(TColStd_HArray1OfReal)&        ResultKnots,
		 Handle(TColStd_HArray1OfInteger)&     ResultMults)
		 
{
  Standard_Integer  i;
 
 if (CheckIfKnotExists(surface_knots,knotmin) &&
     CheckIfKnotExists(surface_knots,knotmax)){
   ResultKnots=new TColStd_HArray1OfReal(1,surface_knots.Length());
   ResultMults=new TColStd_HArray1OfInteger(1,surface_knots.Length());
   for (i=1;i<=surface_knots.Length();i++){
     ResultKnots->SetValue(i,surface_knots(i));
     ResultMults->SetValue(i,surface_mults(i)+deltasurface_degree);
   }
 }
 else{
   if ((CheckIfKnotExists(surface_knots,knotmin))&&(!CheckIfKnotExists(surface_knots,knotmax)))
     AddAKnot(surface_knots,surface_mults,knotmax,deltasurface_degree,finalsurface_degree,ResultKnots,ResultMults);
   else{
     if ((!CheckIfKnotExists(surface_knots,knotmin))&&(CheckIfKnotExists(surface_knots,knotmax)))
       AddAKnot(surface_knots,surface_mults,knotmin,deltasurface_degree,finalsurface_degree,ResultKnots,ResultMults);
     else{
       if ((!CheckIfKnotExists(surface_knots,knotmin))&&(!CheckIfKnotExists(surface_knots,knotmax))&&
	   (knotmin==knotmax)){
	 AddAKnot(surface_knots,surface_mults,knotmin,deltasurface_degree,finalsurface_degree,ResultKnots,ResultMults);
       }
       else{
	 Handle(TColStd_HArray1OfReal)      IntermedKnots;
	 Handle(TColStd_HArray1OfInteger)   IntermedMults;
	 AddAKnot(surface_knots,surface_mults,knotmin,deltasurface_degree,finalsurface_degree,IntermedKnots,IntermedMults);
	 AddAKnot(IntermedKnots->ChangeArray1(),IntermedMults->ChangeArray1(),knotmax,0,finalsurface_degree,ResultKnots,ResultMults);
       }
     }
   }
 }   
}

//=======================================================================
//function : FunctionMultiply 
//purpose  : multiply the surface BSurf by a(u,v) (law_evaluator) on its
//           numerator and denominator
//=======================================================================

static void FunctionMultiply(Handle(Geom_BSplineSurface)&          BSurf,
			     const Standard_Real                   knotmin,
			     const Standard_Real                   knotmax)
     
{TColStd_Array1OfReal      surface_u_knots(1,BSurf->NbUKnots()) ;
 TColStd_Array1OfInteger   surface_u_mults(1,BSurf->NbUKnots()) ;
 TColStd_Array1OfReal      surface_v_knots(1,BSurf->NbVKnots()) ;
 TColStd_Array1OfInteger   surface_v_mults(1,BSurf->NbVKnots()) ;
 TColgp_Array2OfPnt        surface_poles(1,BSurf->NbUPoles(),
					 1,BSurf->NbVPoles()) ;
 TColStd_Array2OfReal      surface_weights(1,BSurf->NbUPoles(),
					   1,BSurf->NbVPoles()) ;
 Standard_Integer          i,j,k,status,new_num_u_poles,new_num_v_poles,length=0;
 Handle(TColStd_HArray1OfReal)     newuknots,newvknots;
 Handle(TColStd_HArray1OfInteger)  newumults,newvmults;

 BSurf->UKnots(surface_u_knots) ;
 BSurf->UMultiplicities(surface_u_mults) ;
 BSurf->VKnots(surface_v_knots) ;
 BSurf->VMultiplicities(surface_v_mults) ;
 BSurf->Poles(surface_poles) ;
 BSurf->Weights(surface_weights) ;

 TColStd_Array1OfReal    Knots(1,2); 
 TColStd_Array1OfInteger Mults(1,2);
 Handle(TColStd_HArray1OfReal)      NewKnots;
 Handle(TColStd_HArray1OfInteger)   NewMults;
 
 Knots(1)=0;
 Knots(2)=1;
 Mults(1)=4;
 Mults(2)=4;
 BuildFlatKnot(Knots,Mults,0,3,knotmin,knotmax,NewKnots,NewMults);

 for (i=1;i<=NewMults->Length();i++)
   length+=NewMults->Value(i);
 TColStd_Array1OfReal       FlatKnots(1,length);
 BSplCLib::KnotSequence(NewKnots->ChangeArray1(),NewMults->ChangeArray1(),FlatKnots);

 GeomLib_DenominatorMultiplier aDenominator (BSurf, FlatKnots);

 BuildFlatKnot(surface_u_knots,
	       surface_u_mults,
	       3,
	       BSurf->UDegree()+3,
	       knotmin,
	       knotmax,
	       newuknots,
	       newumults);
 BuildFlatKnot(surface_v_knots,
	       surface_v_mults,
	       BSurf->VDegree(),
	       2*(BSurf->VDegree()),
	       1.0,
	       0.0,
	       newvknots,
	       newvmults);
 length=0;
 for (i=1;i<=newumults->Length();i++)
   length+=newumults->Value(i);
 new_num_u_poles=(length-BSurf->UDegree()-3-1);
 TColStd_Array1OfReal       newuflatknots(1,length);
 length=0;
 for (i=1;i<=newvmults->Length();i++)
   length+=newvmults->Value(i);
 new_num_v_poles=(length-2*BSurf->VDegree()-1);
 TColStd_Array1OfReal       newvflatknots(1,length);

 TColgp_Array2OfPnt        NewNumerator(1,new_num_u_poles,1,new_num_v_poles);
 TColStd_Array2OfReal      NewDenominator(1,new_num_u_poles,1,new_num_v_poles);
 
 BSplCLib::KnotSequence(newuknots->ChangeArray1(),newumults->ChangeArray1(),newuflatknots);
 BSplCLib::KnotSequence(newvknots->ChangeArray1(),newvmults->ChangeArray1(),newvflatknots);
//POP pour WNT
 law_evaluator ev (&aDenominator);
// BSplSLib::FunctionMultiply(law_evaluator,               //multiplication
 BSplSLib::FunctionMultiply(ev,               //multiplication
			    BSurf->UDegree(),
			    BSurf->VDegree(),
			    surface_u_knots,
			    surface_v_knots,
			    &surface_u_mults,
			    &surface_v_mults,
			    surface_poles,
			    &surface_weights,
			    newuflatknots,
			    newvflatknots,
			    BSurf->UDegree()+3,
			    2*(BSurf->VDegree()),
			    NewNumerator,
			    NewDenominator,
			    status);
 if (status!=0)
   throw Standard_ConstructionError("GeomLib Multiplication Error") ;
 for (i = 1 ; i <= new_num_u_poles ; i++) {
      for (j = 1 ; j <= new_num_v_poles ; j++) {
	for (k = 1 ; k <= 3 ; k++) {
	  NewNumerator(i,j).SetCoord(k,NewNumerator(i,j).Coord(k)/NewDenominator(i,j)) ;
	}
      }
    }
 BSurf= new Geom_BSplineSurface(NewNumerator,                  
				NewDenominator,
				newuknots->ChangeArray1(),
				newvknots->ChangeArray1(),
				newumults->ChangeArray1(),
				newvmults->ChangeArray1(),
				BSurf->UDegree()+3,
				2*(BSurf->VDegree()) );		    
}

//=======================================================================
//function : CancelDenominatorDerivative1D
//purpose  : cancel the denominator derivative in one direction
//=======================================================================

static void CancelDenominatorDerivative1D(Handle(Geom_BSplineSurface) & BSurf)
     
{Standard_Integer            i,j;
 Standard_Real               uknotmin=1.0,uknotmax=0.0,
                             x,y,
                             startu_value,
                             endu_value;
 TColStd_Array1OfReal        BSurf_u_knots(1,BSurf->NbUKnots()) ;

 startu_value=BSurf->UKnot(1);
 endu_value=BSurf->UKnot(BSurf->NbUKnots());
 BSurf->UKnots(BSurf_u_knots) ;
 BSplCLib::Reparametrize(0.0,1.0,BSurf_u_knots);
 BSurf->SetUKnots(BSurf_u_knots);                             //reparametrisation of the surface
 Handle(Geom_BSplineCurve) BCurve;
 TColStd_Array1OfReal      BCurveWeights(1,BSurf->NbUPoles());
 TColgp_Array1OfPnt        BCurvePoles(1,BSurf->NbUPoles());
 TColStd_Array1OfReal      BCurveKnots(1,BSurf->NbUKnots());
 TColStd_Array1OfInteger   BCurveMults(1,BSurf->NbUKnots());

 if (CanBeTreated(BSurf)){
   for (i=1;i<=BSurf->NbVPoles();i++){  //loop on each pole function
     x=1.0;y=0.0;
     for (j=1;j<=BSurf->NbUPoles();j++){
       BCurveWeights(j)=BSurf->Weight(j,i);
       BCurvePoles(j)=BSurf->Pole(j,i);
     }
     BSurf->UKnots(BCurveKnots);
     BSurf->UMultiplicities(BCurveMults);
     BCurve = new Geom_BSplineCurve(BCurvePoles, //building of a pole function 
				    BCurveWeights,
				    BCurveKnots,
				    BCurveMults,
				    BSurf->UDegree());
     Hermit::Solutionbis(BCurve,x,y,Precision::Confusion(),Precision::Confusion()); 
     if (x<uknotmin)
       uknotmin=x;    //uknotmin,uknotmax:extremal knots
     if ((x!=1.0)&&(x>uknotmax))
       uknotmax=x;
     if ((y!=0.0)&&(y<uknotmin))
       uknotmin=y;
     if (y>uknotmax)
       uknotmax=y;
   }
  
   FunctionMultiply(BSurf,uknotmin,uknotmax);                 //multiplication

   BSurf->UKnots(BSurf_u_knots) ;
   BSplCLib::Reparametrize(startu_value,endu_value,BSurf_u_knots);
   BSurf->SetUKnots(BSurf_u_knots);
 }
}

//=======================================================================
//function : CancelDenominatorDerivative
//purpose  : 
//=======================================================================

void GeomLib::CancelDenominatorDerivative(Handle(Geom_BSplineSurface)         & BSurf,
					  const Standard_Boolean              udirection,
					  const Standard_Boolean              vdirection)

{if (udirection && !vdirection)
   CancelDenominatorDerivative1D(BSurf);
 else{
   if (!udirection && vdirection) {
     BSurf->ExchangeUV();
     CancelDenominatorDerivative1D(BSurf);
     BSurf->ExchangeUV();
   }
   else{
     if (udirection && vdirection){                            //optimize the treatment
       if (BSurf->UDegree()<=BSurf->VDegree()){
	 CancelDenominatorDerivative1D(BSurf);
	 BSurf->ExchangeUV();
	 CancelDenominatorDerivative1D(BSurf);
	 BSurf->ExchangeUV();
       }
       else{
	 BSurf->ExchangeUV();
	 CancelDenominatorDerivative1D(BSurf);
	 BSurf->ExchangeUV();
	 CancelDenominatorDerivative1D(BSurf);
       }
     }
   }
 }
}

//=======================================================================
//function : NormEstim
//purpose  : 
//=======================================================================

Standard_Integer GeomLib::NormEstim(const Handle(Geom_Surface)& S, 
				    const gp_Pnt2d& UV, 
				    const Standard_Real Tol, gp_Dir& N) 
{
  gp_Vec DU, DV;
  gp_Pnt DummyPnt;
  Standard_Real aTol2 = Square(Tol);

  S->D1(UV.X(), UV.Y(), DummyPnt, DU, DV);

  Standard_Real MDU = DU.SquareMagnitude(), MDV = DV.SquareMagnitude();

  if(MDU >= aTol2 && MDV >= aTol2) {
    gp_Vec Norm = DU^DV;
    Standard_Real Magn = Norm.SquareMagnitude();
    if(Magn < aTol2) return 3;

    //Magn = sqrt(Magn);
    N.SetXYZ(Norm.XYZ());

    return 0;
  }
  else {
    gp_Vec D2U, D2V, D2UV;
    Standard_Boolean isDone;
    CSLib_NormalStatus aStatus;
    gp_Dir aNormal;

    S->D2(UV.X(), UV.Y(), DummyPnt, DU, DV, D2U, D2V, D2UV);
    CSLib::Normal(DU, DV, D2U, D2V, D2UV, Tol, isDone, aStatus, aNormal);

    if (isDone) {
     Standard_Real Umin, Umax, Vmin, Vmax;
     Standard_Real step = 1.0e-5;
     Standard_Real eps = 1.0e-16;
     Standard_Real sign = -1.0;
     
     S->Bounds(Umin, Umax, Vmin, Vmax);

     // check for cone apex singularity point
     if ((UV.Y() > Vmin + step) && (UV.Y() < Vmax - step))
     {
       gp_Dir aNormal1, aNormal2;
       Standard_Real aConeSingularityAngleEps = 1.0e-4;
       S->D1(UV.X(), UV.Y() - sign * step, DummyPnt, DU, DV);
       if ((DU.XYZ().SquareModulus() > eps) && (DV.XYZ().SquareModulus() > eps)) {
         aNormal1 = DU^DV;
         S->D1(UV.X(), UV.Y() + sign * step, DummyPnt, DU, DV);
         if ((DU.XYZ().SquareModulus() > eps) && (DV.XYZ().SquareModulus() > eps)) {
           aNormal2 = DU^DV;
           if (aNormal1.IsOpposite(aNormal2, aConeSingularityAngleEps))
             return 2;
         }
       }
     }

     // Along V
     if(MDU < aTol2 && MDV >= aTol2) {
       if ((Vmax - UV.Y()) > (UV.Y() - Vmin))
         sign = 1.0;
       S->D1(UV.X(), UV.Y() + sign * step, DummyPnt, DU, DV);
       gp_Vec Norm = DU^DV;
       if (Norm.SquareMagnitude() < eps) {
         Standard_Real sign1 = -1.0;
         if ((Umax - UV.X()) > (UV.X() - Umin))
           sign1 = 1.0;
         S->D1(UV.X() + sign1 * step, UV.Y() + sign * step, DummyPnt, DU, DV);
         Norm = DU^DV;
       }
       if ((Norm.SquareMagnitude() >= eps) && (Norm.Dot(aNormal) < 0.0))
         aNormal.Reverse();
     }

     // Along U
     if(MDV < aTol2 && MDU >= aTol2) {
       if ((Umax - UV.X()) > (UV.X() - Umin))
         sign = 1.0;
       S->D1(UV.X() + sign * step, UV.Y(), DummyPnt, DU, DV);
       gp_Vec Norm = DU^DV;
       if (Norm.SquareMagnitude() < eps) {
         Standard_Real sign1 = -1.0;
         if ((Vmax - UV.Y()) > (UV.Y() - Vmin))
           sign1 = 1.0;
         S->D1(UV.X() + sign * step, UV.Y() + sign1 * step, DummyPnt, DU, DV);
         Norm = DU^DV;
       }
       if ((Norm.SquareMagnitude() >= eps) && (Norm.Dot(aNormal) < 0.0))
         aNormal.Reverse();
     }

      // quasysingular
      if ((aStatus == CSLib_D1NuIsNull) || (aStatus == CSLib_D1NvIsNull) || 
          (aStatus == CSLib_D1NuIsParallelD1Nv)) {
            N.SetXYZ(aNormal.XYZ());
            return 1;
      }
      // conical
      if (aStatus == CSLib_InfinityOfSolutions)
          return 2;
    }
    // computation is impossible
    else {
      // conical
      if (aStatus == CSLib_D1NIsNull) {
        return 2;
      }
      return 3;
    }
  }
  return 3;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================
void GeomLib::IsClosed (const Handle(Geom_Surface)& S, 
                        const Standard_Real Tol,
                        Standard_Boolean& isUClosed, Standard_Boolean& isVClosed) 
{
  isUClosed = Standard_False;
  isVClosed = Standard_False;
  //
  GeomAdaptor_Surface aGAS(S);
  GeomAbs_SurfaceType aSType = aGAS.GetType();
  //
  Standard_Real u1, u2, v1, v2;
  u1 = aGAS.FirstUParameter();
  u2 = aGAS.LastUParameter();
  v1 = aGAS.FirstVParameter();
  v2 = aGAS.LastVParameter();
  //
  Standard_Real Tol2 = Tol * Tol;
  switch (aSType)
  {
    case GeomAbs_Plane:
    {
      return;
    }
    case GeomAbs_SurfaceOfExtrusion:
    {
      if (Precision::IsInfinite(u1) || Precision::IsInfinite(u2)) {
        // not closed
        return;
      }
    }
    case GeomAbs_Cylinder:
    {
      if(Precision::IsInfinite(v1))
        v1 = 0.;
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Cone:
    {
      //find v with maximal distance from axis
      if(!(Precision::IsInfinite(v1) || Precision::IsInfinite(v2)))
      {
        gp_Cone aCone = aGAS.Cone();
        gp_Pnt anApex = aCone.Apex();
        gp_Pnt P1 = aGAS.Value(u1, v1);
        gp_Pnt P2 = aGAS.Value(u1, v2);
        if(P2.SquareDistance(anApex) > P1.SquareDistance(anApex))
        {
          v1 = v2;
        }
      }
      else
      {
        v1 = 0.;
      }
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Sphere:
    {
      //find v with maximal distance from axis
      if(v1*v2 <= 0.)
      {
        v1 = 0.;
      }
      else
      {
        if(v1 < 0.)
        {
          v1 = v2;
        }
      }
      gp_Pnt p1 = aGAS.Value(u1, v1);
      gp_Pnt p2 = aGAS.Value(u2, v1);
      isUClosed = p1.SquareDistance(p2) <= Tol2;
      return;
    }
    case GeomAbs_Torus:
    {
      Standard_Real ures = aGAS.UResolution(Tol);
      Standard_Real vres = aGAS.VResolution(Tol);
      //
      isUClosed = (u2 - u1) >= aGAS.UPeriod() - ures;
      isVClosed = (v2 - v1) >= aGAS.VPeriod() - vres;
      return;
    }
    case GeomAbs_BSplineSurface:
    {
      Handle(Geom_BSplineSurface) aBSpl = aGAS.BSpline();
      isUClosed = GeomLib::IsBSplUClosed(aBSpl, u1, u2, Tol);
      isVClosed = GeomLib::IsBSplVClosed(aBSpl, v1, v2, Tol);
      return;
    }
    case GeomAbs_BezierSurface:
    {
      Handle(Geom_BezierSurface) aBz = aGAS.Bezier();
      isUClosed = GeomLib::IsBzUClosed(aBz, u1, u2, Tol);
      isVClosed = GeomLib::IsBzVClosed(aBz, v1, v2, Tol);
      return;
    }
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface:
    {
      Standard_Integer nbp = 23;
      if(Precision::IsInfinite(v1))
      {
        v1 = Sign(1., v1);
      }
      if(Precision::IsInfinite(v2))
      {
        v2 = Sign(1., v2);
      }
      //
      if(aSType == GeomAbs_OffsetSurface ||
         aSType == GeomAbs_OtherSurface)
      {
        if(Precision::IsInfinite(u1))
        {
          u1 = Sign(1., u1);
        }
        if(Precision::IsInfinite(u2))
        {
          u2 = Sign(1., u2);
        }
      }
      isUClosed = Standard_True;
      Standard_Real dt = (v2 - v1) / (nbp - 1);
      Standard_Real res = Max(aGAS.UResolution(Tol), Precision::PConfusion());
      if(dt <= res)
      {
        nbp = RealToInt((v2 - v1) /(2.*res)) + 1;
        nbp = Max(nbp, 2);
        dt = (v2 - v1) / (nbp - 1);
      }
      Standard_Real t;
      Standard_Integer i;
      for(i = 0; i < nbp;  ++i)
      {
        t = (i == nbp-1 ? v2 : v1 + i * dt);
        gp_Pnt p1 = aGAS.Value(u1, t);
        gp_Pnt p2 = aGAS.Value(u2, t);
        if(p1.SquareDistance(p2) > Tol2)
        {
          isUClosed = Standard_False;
          break;
        }
      }
      // 
      nbp = 23;
      isVClosed = Standard_True;
      dt = (u2 - u1) / (nbp - 1);
      res = Max(aGAS.VResolution(Tol), Precision::PConfusion());
      if(dt <= res)
      {
        nbp = RealToInt((u2 - u1) /(2.*res)) + 1;
        nbp = Max(nbp, 2);
        dt = (u2 - u1) / (nbp - 1);
      }
      for(i = 0; i < nbp;  ++i)
      {
        t = (i == nbp-1 ? u2 : u1 + i * dt);
        gp_Pnt p1 = aGAS.Value(t, v1);
        gp_Pnt p2 = aGAS.Value(t, v2);
        if(p1.SquareDistance(p2) > Tol2)
        {
          isVClosed = Standard_False;
          break;
        }
      }
      return;
    }
    default:
    {
      return;
    }
  }
}

//=======================================================================
//function : IsBSplUClosed
//purpose  : 
//=======================================================================
Standard_Boolean GeomLib::IsBSplUClosed (const Handle(Geom_BSplineSurface)& S, 
                                         const Standard_Real U1,
                                         const Standard_Real U2,
                                         const Standard_Real Tol) 
{   
  Handle(Geom_Curve) aCUF = S->UIso( U1 );
  Handle(Geom_Curve) aCUL = S->UIso( U2 );
  if(aCUF.IsNull() || aCUL.IsNull())
    return Standard_False;
  Standard_Real Tol2 = 2.*Tol;
  Handle(Geom_BSplineCurve) aBsF = Handle(Geom_BSplineCurve)::DownCast(aCUF);
  Handle(Geom_BSplineCurve) aBsL = Handle(Geom_BSplineCurve)::DownCast(aCUL);
  const TColgp_Array1OfPnt& aPF = aBsF->Poles();
  const TColgp_Array1OfPnt& aPL = aBsL->Poles();
  const TColStd_Array1OfReal* WF = aBsF->Weights();
  const TColStd_Array1OfReal* WL = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}

//=======================================================================
//function : IsBSplVClosed
//purpose  : 
//=======================================================================
Standard_Boolean GeomLib::IsBSplVClosed (const Handle(Geom_BSplineSurface)& S, 
                                         const Standard_Real V1,
                                         const Standard_Real V2,
                                         const Standard_Real Tol) 
{
  Handle(Geom_Curve) aCVF = S->VIso( V1 );
  Handle(Geom_Curve) aCVL = S->VIso( V2 );
  if(aCVF.IsNull() || aCVL.IsNull())
    return Standard_False;
  Standard_Real Tol2 = 2.*Tol;
  Handle(Geom_BSplineCurve) aBsF = Handle(Geom_BSplineCurve)::DownCast(aCVF);
  Handle(Geom_BSplineCurve) aBsL = Handle(Geom_BSplineCurve)::DownCast(aCVL);
  const TColgp_Array1OfPnt& aPF = aBsF->Poles();
  const TColgp_Array1OfPnt& aPL = aBsL->Poles();
  const TColStd_Array1OfReal* WF = aBsF->Weights();
  const TColStd_Array1OfReal* WL = aBsL->Weights();
  return CompareWeightPoles(aPF, WF, aPL, WL, Tol2);
}
//=======================================================================
//function : IsBzUClosed
//purpose  : 
//=======================================================================
Standard_Boolean GeomLib::IsBzUClosed (const Handle(Geom_BezierSurface)& S, 
                                       const Standard_Real U1,
                                       const Standard_Real U2,
                                       const Standard_Real Tol) 
{   
  Handle(Geom_Curve) aCUF = S->UIso( U1 );
  Handle(Geom_Curve) aCUL = S->UIso( U2 );
  if(aCUF.IsNull() || aCUL.IsNull())
    return Standard_False;
  Standard_Real Tol2 = 2.*Tol;
  Handle(Geom_BezierCurve) aBzF = Handle(Geom_BezierCurve)::DownCast(aCUF);
  Handle(Geom_BezierCurve) aBzL = Handle(Geom_BezierCurve)::DownCast(aCUL);
  const TColgp_Array1OfPnt& aPF = aBzF->Poles();
  const TColgp_Array1OfPnt& aPL = aBzL->Poles();
  //
  return CompareWeightPoles(aPF, 0, aPL, 0, Tol2);
}

//=======================================================================
//function : IsBzVClosed
//purpose  : 
//=======================================================================
Standard_Boolean GeomLib::IsBzVClosed (const Handle(Geom_BezierSurface)& S, 
                                       const Standard_Real V1,
                                       const Standard_Real V2,
                                       const Standard_Real Tol) 
{
  Handle(Geom_Curve) aCVF = S->VIso( V1 );
  Handle(Geom_Curve) aCVL = S->VIso( V2 );
  if(aCVF.IsNull() || aCVL.IsNull())
    return Standard_False;
  Standard_Real Tol2 = 2.*Tol;
  Handle(Geom_BezierCurve) aBzF = Handle(Geom_BezierCurve)::DownCast(aCVF);
  Handle(Geom_BezierCurve) aBzL = Handle(Geom_BezierCurve)::DownCast(aCVL);
  const TColgp_Array1OfPnt& aPF = aBzF->Poles();
  const TColgp_Array1OfPnt& aPL = aBzL->Poles();
  //
  return CompareWeightPoles(aPF, 0, aPL, 0, Tol2);
}

//=======================================================================
//function : CompareWeightPoles
//purpose  : Checks if thePoles1(i)*theW1(i) is equal to thePoles2(i)*theW2(i)
//            with tolerance theTol.
//           It is necessary for not rational B-splines and Bezier curves
//            to set theW1 and theW2 adresses to zero.
//=======================================================================
static Standard_Boolean CompareWeightPoles(const TColgp_Array1OfPnt& thePoles1, 
                                           const TColStd_Array1OfReal* const theW1,
                                           const TColgp_Array1OfPnt& thePoles2,
                                           const TColStd_Array1OfReal* const theW2,
                                           const Standard_Real theTol)
{
  if(thePoles1.Length() != thePoles2.Length())
  {
    return Standard_False;
  }
  //
  Standard_Integer i = 1;
  for( i = 1 ; i <= thePoles1.Length(); i++ )
  {
    const Standard_Real aW1 = (theW1 == 0) ? 1.0 : theW1->Value(i);
    const Standard_Real aW2 = (theW2 == 0) ? 1.0 : theW2->Value(i);

    gp_XYZ aPole1 = thePoles1.Value(i).XYZ() * aW1;
    gp_XYZ aPole2 = thePoles2.Value(i).XYZ() * aW2;
    if(!aPole1.IsEqual(aPole2, theTol))
      return Standard_False;
  }
  //
  return Standard_True;
}
