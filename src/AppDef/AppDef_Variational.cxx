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

// Jeannine PANCIATICI le 06/06/96
// Igor FEOKTISTOV 14/12/98 - correction of Approximate() and Init().
// Approximation d une MultiLine de points decrite par le tool MLineTool.
// avec criteres variationnels

#include <AppDef_MultiLine.hxx>
#include <AppDef_SmoothCriterion.hxx>
#include <AppDef_Variational.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <FEmTool_Assembly.hxx>
#include <FEmTool_Curve.hxx>
#include <gp_VectorWithNullMagnitude.hxx>
#include <math_Matrix.hxx>
#include <PLib_Base.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
#define No_Standard_ConstructionError

#include <Standard_Stream.hxx>

#include <AppParCurves.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_HArray1OfConstraintCouple.hxx>
#include <AppParCurves_Array1OfMultiPoint.hxx>
#include <AppParCurves_MultiPoint.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppDef_LinearCriteria.hxx>
#include <Convert_CompPolynomialToPoles.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_SStream.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Precision.hxx>
#include <AppDef_MyLineTool.hxx>

#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <FEmTool_Assembly.hxx>
#include <FEmTool_AssemblyTable.hxx>
#include <FEmTool_Curve.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <PLib_Base.hxx>
#include <PLib_JacobiPolynomial.hxx>
#include <PLib_HermitJacobi.hxx>
#include <FEmTool_HAssemblyTable.hxx>

// Add this line:
#include <algorithm>

#if defined(_MSC_VER)
# include <stdio.h>
# include <stdarg.h>
#endif  /* _MSC_VER */

//
//=======================================================================
//function : AppDef_Variational
//purpose  : Initialization of   the   fields.
//=======================================================================
//
AppDef_Variational::AppDef_Variational(const AppDef_MultiLine& SSP,
                                       const Standard_Integer FirstPoint, 
                                       const Standard_Integer LastPoint,
                                       const Handle(AppParCurves_HArray1OfConstraintCouple)& TheConstraints,
                                       const Standard_Integer MaxDegree,
                                       const Standard_Integer MaxSegment,
                                       const GeomAbs_Shape Continuity, 
                                       const Standard_Boolean WithMinMax, 
                                       const Standard_Boolean WithCutting, 
                                       const Standard_Real Tolerance, 
                                       const Standard_Integer NbIterations):
mySSP(SSP),
myFirstPoint(FirstPoint),
myLastPoint(LastPoint),
myConstraints(TheConstraints),
myMaxDegree(MaxDegree),
myMaxSegment(MaxSegment),
myNbIterations(NbIterations),
myTolerance(Tolerance),
myContinuity(Continuity),
myWithMinMax(WithMinMax),
myWithCutting(WithCutting)
{
  // Verifications:
  if (myMaxDegree < 1) throw Standard_DomainError();
  myMaxDegree = Min (30, myMaxDegree);
  //
  if (myMaxSegment < 1) throw Standard_DomainError();
  //
  if (myWithMinMax != 0 && myWithMinMax !=1 ) throw Standard_DomainError();
  if (myWithCutting != 0 && myWithCutting !=1 ) throw Standard_DomainError();
  //
  myIsOverConstr = Standard_False;
  myIsCreated    = Standard_False;
  myIsDone       = Standard_False;
  switch (myContinuity) {
    case GeomAbs_C0:
      myNivCont=0;
      break ;
    case GeomAbs_C1: 
      myNivCont=1;
      break ;
    case GeomAbs_C2:  
      myNivCont=2;
      break ;
    default:
      throw Standard_ConstructionError();
  }
  //
  myNbP2d = AppDef_MyLineTool::NbP2d(SSP); 
  myNbP3d = AppDef_MyLineTool::NbP3d(SSP);
  myDimension = 2 * myNbP2d + 3* myNbP3d ;
  //
  myPercent[0]=0.4;
  myPercent[1]=0.2;
  myPercent[2]=0.4;
  myKnots= new TColStd_HArray1OfReal(1,2);
  myKnots->SetValue(1,0.);
  myKnots->SetValue(2,1.);

  //  Declaration
  //  
  mySmoothCriterion = new AppDef_LinearCriteria(mySSP, myFirstPoint, myLastPoint);
  myParameters = new TColStd_HArray1OfReal(myFirstPoint, myLastPoint);
  myNbPoints=myLastPoint-myFirstPoint+1;
  if (myNbPoints <= 0) throw Standard_ConstructionError();
  // 
  myTabPoints= new TColStd_HArray1OfReal(1,myDimension*myNbPoints);
  //
  //  Table of Points initialization
  //
  Standard_Integer ipoint,jp2d,jp3d,index;
  TColgp_Array1OfPnt TabP3d(1, Max(1,myNbP3d));
  TColgp_Array1OfPnt2d TabP2d(1, Max(1,myNbP2d));    
  gp_Pnt2d P2d;
  gp_Pnt P3d;
  index=1;

  for ( ipoint = myFirstPoint ; ipoint <= myLastPoint ; ipoint++)
  {

    if(myNbP2d !=0 && myNbP3d ==0 ) {
      AppDef_MyLineTool::Value(mySSP,ipoint,TabP2d);

      for ( jp2d = 1 ; jp2d <= myNbP2d ;jp2d++)
      {  P2d = TabP2d.Value(jp2d);

      myTabPoints->SetValue(index++,P2d.X());
      myTabPoints->SetValue(index++,P2d.Y());
      }
    } 
    if(myNbP3d !=0 && myNbP2d == 0 ) {
      AppDef_MyLineTool::Value(mySSP,ipoint,TabP3d);

      for ( jp3d = 1 ; jp3d <= myNbP3d ; jp3d++)

      {  P3d=TabP3d.Value(jp3d);

      myTabPoints->SetValue(index++,P3d.X());
      myTabPoints->SetValue(index++,P3d.Y()); 
      myTabPoints->SetValue(index++,P3d.Z());

      }
    }
    if(myNbP3d !=0 && myNbP2d != 0 ) {
      AppDef_MyLineTool::Value(mySSP,ipoint,TabP3d,TabP2d);

      for ( jp3d = 1 ; jp3d <= myNbP3d ; jp3d++)

      {  P3d=TabP3d.Value(jp3d);

      myTabPoints->SetValue(index++,P3d.X());
      myTabPoints->SetValue(index++,P3d.Y()); 
      myTabPoints->SetValue(index++,P3d.Z());

      }
      for ( jp2d = 1 ; jp2d <= myNbP2d ; jp2d++)

      {  P2d=TabP2d.Value(jp2d);

      myTabPoints->SetValue(index++,P2d.X());
      myTabPoints->SetValue(index++,P2d.Y()); 
      }
    }
  }
  Init();
}
//
//=======================================================================
//function : Init
//purpose  : Initializes the tables of constraints
//           and verifies if the problem is not over-constrained.
//           This method is used in the Create and the method SetConstraint. 
//=======================================================================
//
void AppDef_Variational::Init()
{

  Standard_Integer ipoint,jp2d,jp3d,index,jndex;
  Standard_Integer CurMultyPoint;
  TColgp_Array1OfVec TabV3d(1, Max(1,myNbP3d));
  TColgp_Array1OfVec2d TabV2d(1, Max(1,myNbP2d));
  TColgp_Array1OfVec TabV3dcurv(1, Max(1,myNbP3d));
  TColgp_Array1OfVec2d TabV2dcurv(1, Max(1,myNbP2d));

  gp_Vec Vt3d, Vc3d;
  gp_Vec2d Vt2d, Vc2d;

  myNbConstraints=myConstraints->Length();
  if (myNbConstraints < 0) throw Standard_ConstructionError();

  myTypConstraints = new TColStd_HArray1OfInteger(1,Max(1,2*myNbConstraints));
  myTabConstraints = new TColStd_HArray1OfReal(1,Max(1,2*myDimension*myNbConstraints));
  myTtheta = new TColStd_HArray1OfReal(1,Max(1,(2 * myNbP2d + 6 * myNbP3d) * myNbConstraints));
  myTfthet = new TColStd_HArray1OfReal(1,Max(1,(2 * myNbP2d + 6 * myNbP3d) * myNbConstraints));


  //
  // Table of types initialization
  Standard_Integer iconstr;
  index=1;
  jndex=1;
  CurMultyPoint = 1;
  myNbPassPoints=0;
  myNbTangPoints=0;
  myNbCurvPoints=0;
  AppParCurves_Constraint valcontr;

  for ( iconstr = myConstraints->Lower() ; iconstr <= myConstraints->Upper() ; iconstr++)
  {
    ipoint=(myConstraints->Value(iconstr)).Index();

    valcontr=(myConstraints->Value(iconstr)).Constraint();
    switch (valcontr) {
                             case AppParCurves_NoConstraint:
                               CurMultyPoint -= myNbP3d * 6 + myNbP2d * 2;
                               break ;
                             case AppParCurves_PassPoint: 
                               myTypConstraints->SetValue(index++,ipoint);
                               myTypConstraints->SetValue(index++,0);
                               myNbPassPoints++;
                               if(myNbP2d !=0 ) jndex=jndex+4*myNbP2d;
                               if(myNbP3d !=0 ) jndex=jndex+6*myNbP3d;
                               break ;
                             case AppParCurves_TangencyPoint:
                               myTypConstraints->SetValue(index++,ipoint);
                               myTypConstraints->SetValue(index++,1); 
                               myNbTangPoints++;
                               if(myNbP2d !=0 && myNbP3d == 0 ) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV2d) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp2d=1;jp2d<=myNbP2d;jp2d++)
                                 {  
                                   Vt2d=TabV2d.Value(jp2d);
                                   Vt2d.Normalize();
                                   myTabConstraints->SetValue(jndex++,Vt2d.X());
                                   myTabConstraints->SetValue(jndex++,Vt2d.Y());
                                   jndex=jndex+2;
                                   InitTthetaF(2, valcontr, CurMultyPoint + (jp2d - 1) * 2, jndex - 4);
                                 }
                               } 
                               if(myNbP3d !=0 && myNbP2d == 0) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV3d) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp3d=1;jp3d<=myNbP3d;jp3d++)
                                 {  
                                   Vt3d=TabV3d.Value(jp3d);
                                   Vt3d.Normalize();    
                                   myTabConstraints->SetValue(jndex++,Vt3d.X());

                                   myTabConstraints->SetValue(jndex++,Vt3d.Y()); 

                                   myTabConstraints->SetValue(jndex++,Vt3d.Z());
                                   jndex=jndex+3;
                                   InitTthetaF(3, valcontr, CurMultyPoint + (jp3d - 1) * 6, jndex - 6);
                                 }
                               }
                               if(myNbP3d !=0 && myNbP2d != 0) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV3d,TabV2d) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp3d=1;jp3d<=myNbP3d;jp3d++)
                                 {  
                                   Vt3d=TabV3d.Value(jp3d);
                                   Vt3d.Normalize();    
                                   myTabConstraints->SetValue(jndex++,Vt3d.X());
                                   myTabConstraints->SetValue(jndex++,Vt3d.Y()); 
                                   myTabConstraints->SetValue(jndex++,Vt3d.Z());
                                   jndex=jndex+3;
                                   InitTthetaF(3, valcontr, CurMultyPoint + (jp3d - 1) * 6, jndex - 6);
                                 }

                                 for (jp2d=1;jp2d<=myNbP2d;jp2d++)
                                 {  
                                   Vt2d=TabV2d.Value(jp2d);
                                   Vt2d.Normalize();
                                   myTabConstraints->SetValue(jndex++,Vt2d.X());
                                   myTabConstraints->SetValue(jndex++,Vt2d.Y());
                                   jndex=jndex+2;
                                   InitTthetaF(2, valcontr, CurMultyPoint + (jp2d - 1) * 2 + myNbP3d * 6, jndex - 4);
                                 }
                               }


                               break ;

                             case AppParCurves_CurvaturePoint:
                               myTypConstraints->SetValue(index++,ipoint);
                               myTypConstraints->SetValue(index++,2);
                               myNbCurvPoints++;
                               if(myNbP2d !=0 && myNbP3d == 0) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV2d) == Standard_False )
                                   throw Standard_ConstructionError();
                                 if (AppDef_MyLineTool::Curvature(mySSP,ipoint,TabV2dcurv) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp2d=1;jp2d<=myNbP2d;jp2d++)
                                 {  
                                   Vt2d=TabV2d.Value(jp2d);
                                   Vt2d.Normalize();
                                   Vc2d=TabV2dcurv.Value(jp2d);
                                   if (Abs(Abs(Vc2d.Angle(Vt2d)) - M_PI/2.) > Precision::Angular())
                                     throw Standard_ConstructionError();
                                   myTabConstraints->SetValue(jndex++,Vt2d.X());
                                   myTabConstraints->SetValue(jndex++,Vt2d.Y());
                                   myTabConstraints->SetValue(jndex++,Vc2d.X());
                                   myTabConstraints->SetValue(jndex++,Vc2d.Y());
                                   InitTthetaF(2, valcontr, CurMultyPoint + (jp2d - 1) * 2, jndex - 4);
                                 }
                               } 

                               if(myNbP3d !=0 && myNbP2d == 0 ) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV3d) == Standard_False )
                                   throw Standard_ConstructionError();
                                 if (AppDef_MyLineTool::Curvature(mySSP,ipoint,TabV3dcurv) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp3d=1;jp3d<=myNbP3d;jp3d++)
                                 {  
                                   Vt3d=TabV3d.Value(jp3d);
                                   Vt3d.Normalize();
                                   Vc3d=TabV3dcurv.Value(jp3d);
                                   if ( (Vc3d.Normalized()).IsNormal(Vt3d,Precision::Angular()) == Standard_False) 
                                     throw Standard_ConstructionError();
                                   myTabConstraints->SetValue(jndex++,Vt3d.X());
                                   myTabConstraints->SetValue(jndex++,Vt3d.Y()); 
                                   myTabConstraints->SetValue(jndex++,Vt3d.Z());
                                   myTabConstraints->SetValue(jndex++,Vc3d.X());
                                   myTabConstraints->SetValue(jndex++,Vc3d.Y()); 
                                   myTabConstraints->SetValue(jndex++,Vc3d.Z());
                                   InitTthetaF(3, valcontr, CurMultyPoint + (jp3d - 1) * 6, jndex - 6);
                                 }
                               }
                               if(myNbP3d !=0 && myNbP2d != 0 ) 
                               {
                                 if (AppDef_MyLineTool::Tangency(mySSP,ipoint,TabV3d,TabV2d) == Standard_False )
                                   throw Standard_ConstructionError();
                                 if (AppDef_MyLineTool::Curvature(mySSP,ipoint,TabV3dcurv,TabV2dcurv) == Standard_False)
                                   throw Standard_ConstructionError();
                                 for (jp3d=1;jp3d<=myNbP3d;jp3d++)
                                 {  
                                   Vt3d=TabV3d.Value(jp3d);
                                   Vt3d.Normalize();
                                   Vc3d=TabV3dcurv.Value(jp3d);
                                   if ( (Vc3d.Normalized()).IsNormal(Vt3d,Precision::Angular()) == Standard_False) 
                                     throw Standard_ConstructionError();
                                   myTabConstraints->SetValue(jndex++,Vt3d.X());
                                   myTabConstraints->SetValue(jndex++,Vt3d.Y()); 
                                   myTabConstraints->SetValue(jndex++,Vt3d.Z());
                                   myTabConstraints->SetValue(jndex++,Vc3d.X());
                                   myTabConstraints->SetValue(jndex++,Vc3d.Y()); 
                                   myTabConstraints->SetValue(jndex++,Vc3d.Z());
                                   InitTthetaF(3, valcontr, CurMultyPoint + (jp3d - 1) * 6, jndex - 6);
                                 }
                                 for (jp2d=1;jp2d<=myNbP2d;jp2d++)
                                 {  
                                   Vt2d=TabV2d.Value(jp2d);
                                   Vt2d.Normalize();
                                   Vc2d=TabV2dcurv.Value(jp2d);
                                   if (Abs(Abs(Vc2d.Angle(Vt2d)) - M_PI/2.) > Precision::Angular())
                                     throw Standard_ConstructionError();
                                   myTabConstraints->SetValue(jndex++,Vt2d.X());
                                   myTabConstraints->SetValue(jndex++,Vt2d.Y());
                                   myTabConstraints->SetValue(jndex++,Vc2d.X());
                                   myTabConstraints->SetValue(jndex++,Vc2d.Y());
                                   InitTthetaF(2, valcontr, CurMultyPoint + (jp2d - 1) * 2 + myNbP3d * 6, jndex - 4);
                                 }

                               }
                               break ; 
                             default:
                               throw Standard_ConstructionError();
    }
    CurMultyPoint += myNbP3d * 6 + myNbP2d * 2;
  }
  // OverConstraint Detection
  Standard_Integer MaxSeg;
  if(myWithCutting == Standard_True) MaxSeg = myMaxSegment ;
  else MaxSeg = 1;
  if (((myMaxDegree-myNivCont)*MaxSeg-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
  {
    myIsOverConstr =Standard_True;
    myIsCreated = Standard_False;
  }
  else {
    InitSmoothCriterion();
    myIsCreated = Standard_True;
  }


}
//
//=======================================================================
//function : Approximate
//purpose  : Makes the approximation with the current fields.
//=======================================================================
//
void AppDef_Variational::Approximate()

{
  if (myIsCreated == Standard_False )  throw StdFail_NotDone();


  Standard_Real WQuadratic, WQuality;

  TColStd_Array1OfReal Ecarts(myFirstPoint, myLastPoint); 

  mySmoothCriterion->GetWeight(WQuadratic, WQuality);

  Handle(FEmTool_Curve) TheCurve;  

  mySmoothCriterion->GetCurve(TheCurve);

  //---------------------------------------------------------------------

  TheMotor(mySmoothCriterion, WQuadratic, WQuality, TheCurve, Ecarts);


  if(myWithMinMax && myTolerance < myMaxError)
    Adjusting(mySmoothCriterion, WQuadratic, WQuality, TheCurve, Ecarts);

  //---------------------------------------------------------------------

  Standard_Integer jp2d,jp3d,index,ipole, 
    NbElem = TheCurve->NbElements();

  TColgp_Array1OfPnt TabP3d(1, Max(1,myNbP3d));
  TColgp_Array1OfPnt2d TabP2d(1, Max(1,myNbP2d));
  Standard_Real debfin[2] = {-1., 1};

  gp_Pnt2d P2d;
  gp_Pnt P3d;

  index=0;

  {
    Handle(TColStd_HArray2OfReal) PolynomialIntervalsPtr =
      new TColStd_HArray2OfReal(1,NbElem,1,2) ;

    Handle(TColStd_HArray1OfInteger) NbCoeffPtr = 
      new TColStd_HArray1OfInteger(1, myMaxSegment);

    Standard_Integer size = myMaxSegment * (myMaxDegree + 1) * myDimension ;
    Handle(TColStd_HArray1OfReal) CoeffPtr = new TColStd_HArray1OfReal(1,size);

    CoeffPtr->Init(0.);

    Handle(TColStd_HArray1OfReal) IntervallesPtr = 
      new TColStd_HArray1OfReal(1, NbElem + 1);

    IntervallesPtr->ChangeArray1() = TheCurve->Knots();

    TheCurve->GetPolynom(CoeffPtr->ChangeArray1());

    Standard_Integer ii;

    for(ii = 1; ii <= NbElem; ii++)  
      NbCoeffPtr->SetValue(ii, TheCurve->Degree(ii)+1);


    for (ii = PolynomialIntervalsPtr->LowerRow() ;
      ii <= PolynomialIntervalsPtr->UpperRow() ;ii++) 
    {
      PolynomialIntervalsPtr->SetValue(ii,1,debfin[0]) ;
      PolynomialIntervalsPtr->SetValue(ii,2,debfin[1]) ;
    }
    /*   
    printf("\n =========== Parameters for converting\n");
    printf("nb_courbes : %d \n", NbElem);
    printf("tab_option[4] : %d \n", myNivCont);
    printf("myDimension : %d \n", myDimension);
    printf("myMaxDegree : %d \n", myMaxDegree);
    printf("\n NbcoeffPtr :\n");
    for(ii = 1; ii <= NbElem; ii++)  printf("NbCoeffPtr(%d) = %d \n", ii, NbCoeffPtr->Value(ii));
    size = NbElem*(myMaxDegree + 1) * myDimension;
    printf("\n CoeffPtr :\n");
    for(ii = 1; ii <= size; ii++)  printf("CoeffPtr(%d) = %.8e \n", ii, CoeffPtr->Value(ii));
    printf("\n PolinimialIntervalsPtr :\n");
    for (ii = PolynomialIntervalsPtr->LowerRow() ;
    ii <= PolynomialIntervalsPtr->UpperRow() ;ii++) 
    {
    printf(" %d : [%f, %f] \n", ii, PolynomialIntervalsPtr->Value(ii,1),
    PolynomialIntervalsPtr->Value(ii,2)) ;
    }
    printf("\n IntervallesPtr :\n");
    for (ii = IntervallesPtr->Lower();
    ii <= IntervallesPtr->Upper() - 1; ii++) 
    {
    printf(" %d : [%f, %f] \n", ii, IntervallesPtr->Value(ii),
    IntervallesPtr->Value(ii+1)) ;
    }
    */  
    Convert_CompPolynomialToPoles AConverter(NbElem, 
      myNivCont,
      myDimension,
      myMaxDegree,
      NbCoeffPtr,
      CoeffPtr,
      PolynomialIntervalsPtr,
      IntervallesPtr) ; 
    if (AConverter.IsDone()) 
    {
      Handle(TColStd_HArray2OfReal) PolesPtr ;
      Handle(TColStd_HArray1OfInteger) Mults;
      Standard_Integer NbPoles=AConverter.NbPoles();
      //	Standard_Integer Deg=AConverter.Degree();
      AppParCurves_Array1OfMultiPoint TabMU(1,NbPoles);
      AConverter.Poles(PolesPtr) ;
      AConverter.Knots(myKnots) ;
      AConverter.Multiplicities(Mults) ;

      for (ipole=PolesPtr->LowerRow();ipole<=PolesPtr->UpperRow();ipole++)
      {
        index=PolesPtr->LowerCol();
        /*	    if(myNbP2d !=0 ) 
        {
        for (jp2d=1;jp2d<=myNbP2d;jp2d++)
        {
        P2d.SetX(PolesPtr->Value(ipole,index++));
        P2d.SetY(PolesPtr->Value(ipole,index++));  
        TabP2d.SetValue(jp2d,P2d);
        }
        }*/
        if(myNbP3d !=0 ) 
        {
          for (jp3d=1;jp3d<=myNbP3d;jp3d++)
          {
            //                       cout << "\n Poles(ipole,1)" << PolesPtr->Value(ipole,index);
            P3d.SetX(PolesPtr->Value(ipole,index++));
            //                       cout << "\n Poles(ipole,1)" << PolesPtr->Value(ipole,index);
            P3d.SetY(PolesPtr->Value(ipole,index++));
            //                       cout << "\n Poles(ipole,1)" << PolesPtr->Value(ipole,index);
            P3d.SetZ(PolesPtr->Value(ipole,index++)); 
            TabP3d.SetValue(jp3d,P3d);
          } 
        }
        if(myNbP2d !=0 ) 
        {
          for (jp2d=1;jp2d<=myNbP2d;jp2d++)
          {
            P2d.SetX(PolesPtr->Value(ipole,index++));
            P2d.SetY(PolesPtr->Value(ipole,index++));  
            TabP2d.SetValue(jp2d,P2d);
          }
        }
        if(myNbP2d !=0 && myNbP3d !=0) 
        {
          AppParCurves_MultiPoint aMultiPoint(TabP3d,TabP2d);
          TabMU.SetValue(ipole,aMultiPoint);                     
        }
        else if (myNbP2d !=0)
        {
          AppParCurves_MultiPoint aMultiPoint(TabP2d);
          TabMU.SetValue(ipole,aMultiPoint);                     
        }
        else 
        {

          AppParCurves_MultiPoint aMultiPoint(TabP3d);
          TabMU.SetValue(ipole,aMultiPoint);                     
        }

      }
      AppParCurves_MultiBSpCurve aCurve(TabMU,myKnots->Array1(),Mults->Array1());
      myMBSpCurve=aCurve; 
      myIsDone = Standard_True;
    }
  }

}
//
//=======================================================================
//function : IsCreated
//purpose  : returns True if the creation is done 
//=======================================================================
//
Standard_Boolean AppDef_Variational::IsCreated() const 
{
  return myIsCreated;
}
//
//=======================================================================
//function : IsDone
//purpose  : returns True if the  approximation is ok
//=======================================================================
//
Standard_Boolean AppDef_Variational::IsDone() const 
{
  return myIsDone;
}
//
//=======================================================================
//function : IsOverConstrained
//purpose  : returns True if the problem is overconstrained
//           in this case, approximation cannot be done.
//=======================================================================
//
Standard_Boolean AppDef_Variational::IsOverConstrained() const 
{
  return myIsOverConstr;
}
//
//=======================================================================
//function : Value
//purpose  : returns all the BSpline curves approximating the
//    	     MultiLine SSP after minimization of the parameter.

//=======================================================================
//
AppParCurves_MultiBSpCurve AppDef_Variational::Value() const 
{ 
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myMBSpCurve;

}
//
//=======================================================================
//function : MaxError
//purpose  : returns the maximum of the distances between 
//           the points of the multiline and the approximation 
//           curves.
//=======================================================================
//
Standard_Real AppDef_Variational::MaxError() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myMaxError;
}
//
//=======================================================================
//function : MaxErrorIndex
//purpose  : returns the index of the MultiPoint of ErrorMax
//=======================================================================
//
Standard_Integer AppDef_Variational::MaxErrorIndex() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myMaxErrorIndex;
}
//
//=======================================================================
//function : QuadraticError
//purpose  :  returns the quadratic average of the distances between 
//            the points of the multiline and the approximation 
//            curves.
//=======================================================================
//
Standard_Real AppDef_Variational::QuadraticError() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myCriterium[0];
}
//
//=======================================================================
//function : Distance
//purpose  : returns the distances between the points of the 
//           multiline and the approximation curves.
//=======================================================================
//
void AppDef_Variational::Distance(math_Matrix& mat)

{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  Standard_Integer ipoint,jp2d,jp3d,index;
  TColgp_Array1OfPnt TabP3d(1,Max(1,myNbP3d));
  TColgp_Array1OfPnt2d TabP2d(1, Max(1,myNbP2d));
  Standard_Integer j0 = mat.LowerCol() - myFirstPoint;

  gp_Pnt2d P2d;
  gp_Pnt P3d;


  gp_Pnt Pt3d;
  gp_Pnt2d Pt2d;

  for ( ipoint = myFirstPoint ; ipoint <= myLastPoint ; ipoint++)
  {
    index=1;
    if(myNbP3d !=0 ) {
      AppDef_MyLineTool::Value(mySSP,ipoint,TabP3d);

      for ( jp3d = 1 ; jp3d <= myNbP3d ; jp3d++)

      {  P3d=TabP3d.Value(jp3d);
      myMBSpCurve.Value(index,myParameters->Value(ipoint),Pt3d);
      mat(index++, j0 + ipoint)=P3d.Distance(Pt3d);                                 

      }
    }
    if(myNbP2d !=0 ) {
      if(myNbP3d == 0 ) AppDef_MyLineTool::Value(mySSP,ipoint,TabP2d);
      else AppDef_MyLineTool::Value(mySSP,ipoint,TabP3d,TabP2d);
      for ( jp2d = 1 ; jp2d <= myNbP2d ;jp2d++)

      {  P2d = TabP2d.Value(jp2d);
      myMBSpCurve.Value(index,myParameters->Value(ipoint),Pt2d);
      mat(index++, j0 + ipoint)=P2d.Distance(Pt2d);
      }
    } 
  }

}
//
//=======================================================================
//function : AverageError
//purpose  : returns the average error between          
//           the MultiLine and the approximation.
//=======================================================================
//
Standard_Real AppDef_Variational::AverageError() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myAverageError;
}
//
//=======================================================================
//function : Parameters
//purpose  : returns the parameters uses to the approximations
//=======================================================================
//
const Handle(TColStd_HArray1OfReal)& AppDef_Variational::Parameters() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myParameters;
}
//
//=======================================================================
//function : Knots
//purpose  : returns the knots uses to the approximations
//=======================================================================
//
const Handle(TColStd_HArray1OfReal)& AppDef_Variational::Knots() const 
{
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  return myKnots;
}
//
//=======================================================================
//function : Criterium
//purpose  : returns the values of the quality criterium.
//=======================================================================
//
void AppDef_Variational::Criterium(Standard_Real& VFirstOrder, Standard_Real& VSecondOrder, Standard_Real& VThirdOrder) const 
{     
  if (myIsDone == Standard_False)  throw StdFail_NotDone();
  VFirstOrder=myCriterium[1] ;
  VSecondOrder=myCriterium[2];
  VThirdOrder=myCriterium[3];
}
//
//=======================================================================
//function : CriteriumWeight
//purpose  : returns the Weights (as percent) associed  to the criterium used in
//           the  optimization.
//=======================================================================
//
void AppDef_Variational::CriteriumWeight(Standard_Real& Percent1, Standard_Real& Percent2, Standard_Real& Percent3) const 
{
  Percent1 = myPercent[0];
  Percent2 = myPercent[1];
  Percent3 = myPercent[2] ;
}
//
//=======================================================================
//function : MaxDegree
//purpose  : returns the Maximum Degree used in the approximation 
//=======================================================================
//
Standard_Integer AppDef_Variational::MaxDegree() const 
{
  return myMaxDegree;
}
//
//=======================================================================
//function : MaxSegment
//purpose  : returns the Maximum of segment used in the approximation
//=======================================================================
//
Standard_Integer AppDef_Variational::MaxSegment() const 
{
  return myMaxSegment;
}
//
//=======================================================================
//function : Continuity
//purpose  : returns the Continuity used in the approximation
//=======================================================================
//
GeomAbs_Shape AppDef_Variational::Continuity() const 
{
  return myContinuity;
}
//
//=======================================================================
//function : WithMinMax
//purpose  : returns if the  approximation  search to  minimize the
//           maximum Error or not.
//=======================================================================
//
Standard_Boolean AppDef_Variational::WithMinMax() const 
{
  return myWithMinMax;
}
//
//=======================================================================
//function : WithCutting
//purpose  :  returns if the  approximation can insert new Knots or not.
//=======================================================================
//
Standard_Boolean AppDef_Variational::WithCutting() const 
{
  return myWithCutting;
}
//
//=======================================================================
//function : Tolerance
//purpose  : returns the tolerance used in the approximation.
//=======================================================================
//
Standard_Real AppDef_Variational::Tolerance() const 
{
  return myTolerance;
}
//
//=======================================================================
//function : NbIterations
//purpose  : returns the number of iterations used in the approximation.
//=======================================================================
//
Standard_Integer AppDef_Variational::NbIterations() const 
{
  return myNbIterations;
}
//
//=======================================================================
//function : Dump
//purpose  : Prints on the stream o information on the current state 
//           of the object.
//=======================================================================
//
void AppDef_Variational::Dump(Standard_OStream& o) const 
{
  o << " \nVariational Smoothing " << endl;
  o << " Number of multipoints                   "  << myNbPoints << endl;
  o << " Number of 2d par multipoint "  << myNbP2d << endl;
  o << " Nombre of 3d par multipoint "  << myNbP3d << endl;
  o << " Number of PassagePoint      "  << myNbPassPoints << endl;
  o << " Number of TangencyPoints    "  << myNbTangPoints << endl;
  o << " Number of CurvaturePoints   "  << myNbCurvPoints << endl;
  o << " \nTolerance " << o.setf(ios::scientific) << setprecision(3) << setw(9) << myTolerance;
  if ( WithMinMax()) { o << "  as Max Error." << endl;}
  else { o << "  as size Error." << endl;}
  o << "CriteriumWeights : " << myPercent[0] << " , " 
    << myPercent[1] << " , " << myPercent[2] << endl;

  if (myIsDone ) {
    o << " MaxError             " << setprecision(3) << setw(9) << myMaxError << endl;
    o << " Index of  MaxError   " << myMaxErrorIndex << endl;
    o << " Average Error        " << setprecision(3) << setw(9) << myAverageError << endl;
    o << " Quadratic Error      " << setprecision(3) << setw(9) << myCriterium[0] << endl;
    o << " Tension Criterium    " << setprecision(3) << setw(9) << myCriterium[1] << endl;
    o << " Flexion  Criterium   " << setprecision(3) << setw(9) << myCriterium[2] << endl;
    o << " Jerk  Criterium      " << setprecision(3) << setw(9) << myCriterium[3] << endl;
    o << " NbSegments           "  << myKnots->Length()-1 << endl;
  }
  else
  { if (myIsOverConstr) o << "The probleme is overconstraint " << endl;
  else o << " Erreur dans l''approximation" << endl;
  }   
}
//
//=======================================================================
//function : SetConstraints
//purpose  : Define the constraints to approximate
//           If this value is incompatible with the others fields
//           this method modify nothing and returns false 
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetConstraints(const Handle(AppParCurves_HArray1OfConstraintCouple)& aConstraint)

{
  myConstraints=aConstraint;
  Init();
  if (myIsOverConstr ) return Standard_False;
  else return Standard_True;
}
//
//=======================================================================
//function : SetParameters
//purpose  : Defines the parameters used by the approximations.
//=======================================================================
//
void AppDef_Variational::SetParameters(const Handle(TColStd_HArray1OfReal)& param)
{
  myParameters->ChangeArray1() = param->Array1();
}
//
//=======================================================================
//function : SetKnots
//purpose  : Defines the knots used by the approximations
//    --          If this value is incompatible with the others fields
//    --          this method modify nothing and returns false
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetKnots(const Handle(TColStd_HArray1OfReal)& knots)
{
  myKnots->ChangeArray1() = knots->Array1();
  return Standard_True;
}
//
//=======================================================================
//function : SetMaxDegree
//purpose  : Define the Maximum Degree used in the approximation
//           If this value is incompatible with the others fields
//           this method modify nothing and returns false 
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetMaxDegree(const Standard_Integer Degree)
{
  if (((Degree-myNivCont)*myMaxSegment-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
    return Standard_False; 
  else
  {
    myMaxDegree=Degree;

    InitSmoothCriterion();

    return Standard_True;
  }

}
//
//=======================================================================
//function : SetMaxSegment
//purpose  : Define the maximum number of segments used in the approximation
//           If this value is incompatible with the others fields
//           this method modify nothing and returns false 
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetMaxSegment(const Standard_Integer NbSegment)
{
  if ( myWithCutting == Standard_True && 
    ((myMaxDegree-myNivCont)*NbSegment-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
    return Standard_False;
  else
  {
    myMaxSegment=NbSegment;
    return Standard_True;
  }   
}
//
//=======================================================================
//function : SetContinuity
//purpose  : Define the Continuity used in the approximation 
//           If this value is incompatible with the others fields
//           this method modify nothing and returns false 
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetContinuity(const GeomAbs_Shape C)
{
  Standard_Integer NivCont=0;
  switch (C) {
    case GeomAbs_C0:
      NivCont=0;
      break ;
    case GeomAbs_C1: 
      NivCont=1;
      break ;
    case GeomAbs_C2:  
      NivCont=2;
      break ;
    default:
      throw Standard_ConstructionError();
  }
  if (((myMaxDegree-NivCont)*myMaxSegment-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
    return Standard_False; 
  else
  {
    myContinuity= C;
    myNivCont=NivCont;

    InitSmoothCriterion();
    return Standard_True;
  }
}
//
//=======================================================================
//function : SetWithMinMax
//purpose  : Define if the  approximation  search to  minimize the
//           maximum Error or not.
//=======================================================================
//
void AppDef_Variational::SetWithMinMax(const Standard_Boolean MinMax)
{
  myWithMinMax=MinMax;

  InitSmoothCriterion();
}
//
//=======================================================================
//function : SetWithCutting
//purpose  : Define if the  approximation can insert new Knots or not.
//           If this value is incompatible with the others fields
//           this method modify nothing and returns false 
//=======================================================================
//
Standard_Boolean AppDef_Variational::SetWithCutting(const Standard_Boolean Cutting)
{
  if (Cutting == Standard_False) 
  {
    if (((myMaxDegree-myNivCont)*myKnots->Length()-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
      return Standard_False;

    else
    {
      myWithCutting=Cutting;
      InitSmoothCriterion();
      return Standard_True;
    }
  }
  else
  {
    if (((myMaxDegree-myNivCont)*myMaxSegment-myNbPassPoints-2*myNbTangPoints-3*myNbCurvPoints) < 0 )
      return Standard_False;

    else
    {
      myWithCutting=Cutting;
      InitSmoothCriterion();
      return Standard_True;
    }
  }
}
//
//=======================================================================
//function : SetCriteriumWeight
//purpose  : define the Weights (as percent) associed to the criterium used in
//           the  optimization.
//=======================================================================
//
void AppDef_Variational::SetCriteriumWeight(const Standard_Real Percent1, const Standard_Real Percent2, const Standard_Real Percent3)
{
  if (Percent1 < 0 || Percent2 < 0 || Percent3 < 0 ) throw Standard_DomainError();
  Standard_Real Total = Percent1 + Percent2 + Percent3;
  myPercent[0] = Percent1/Total;
  myPercent[1] = Percent2/Total;
  myPercent[2] = Percent3/Total;

  InitSmoothCriterion();
}
//
//=======================================================================
//function : SetCriteriumWeight
//purpose  :  define the  Weight   (as  percent)  associed  to   the
//            criterium   Order used in   the optimization  : Others
//            weights are updated.
//=======================================================================
//
void AppDef_Variational::SetCriteriumWeight(const Standard_Integer Order, const Standard_Real Percent)
{
  if ( Percent < 0 ) throw Standard_DomainError();
  if ( Order < 1 || Order > 3 ) throw Standard_ConstructionError();
  myPercent[Order-1] = Percent;
  Standard_Real Total = myPercent[0] + myPercent[1] + myPercent[2];
  myPercent[0] = myPercent[0] / Total;
  myPercent[1] = myPercent[1] / Total;
  myPercent[2] = myPercent[2] / Total;

  InitSmoothCriterion();

}
//
//=======================================================================
//function : SetTolerance
//purpose  : define the tolerance used in the approximation.
//=======================================================================
//
void AppDef_Variational::SetTolerance(const Standard_Real Tol)
{
  myTolerance=Tol;
  InitSmoothCriterion();
}
//
//=======================================================================
//function : SetNbIterations
//purpose  : define the number of iterations used in the approximation.
//=======================================================================
//
void AppDef_Variational::SetNbIterations(const Standard_Integer Iter)
{
  myNbIterations=Iter;
}

//====================== Private Methodes =============================//
//=======================================================================
//function : TheMotor
//purpose  : Smoothing's motor like STRIM routine "MOTLIS"
//=======================================================================
void AppDef_Variational::TheMotor(
                                  Handle(AppDef_SmoothCriterion)& J,
                                  //			 const Standard_Real WQuadratic,
                                  const Standard_Real ,
                                  const Standard_Real WQuality,
                                  Handle(FEmTool_Curve)& TheCurve,
                                  TColStd_Array1OfReal& Ecarts) 
{
  // ...

  const Standard_Real BigValue = 1.e37, SmallValue = 1.e-6, SmallestValue = 1.e-9;

  Handle(TColStd_HArray1OfReal) CurrentTi, NewTi, OldTi;  
  Handle(TColStd_HArray2OfInteger) Dependence;
  Standard_Boolean lestim, lconst, ToOptim, iscut;
  Standard_Boolean isnear = Standard_False, again = Standard_True; 
  Standard_Integer NbEst, ICDANA, NumPnt, Iter;
  Standard_Integer MaxNbEst =5; 
  Standard_Real VOCRI[3] = {BigValue, BigValue, BigValue}, EROLD = BigValue,
    VALCRI[3], ERRMAX = BigValue, ERRMOY, ERRQUA;
  Standard_Real CBLONG, LNOLD;
  Standard_Integer NbrPnt = myLastPoint - myFirstPoint + 1;
  Standard_Integer NbrConstraint = myNbPassPoints + myNbTangPoints + myNbCurvPoints;
  Handle(FEmTool_Curve) CCurrent, COld, CNew;
  Standard_Real EpsLength = SmallValue;
  Standard_Real EpsDeg; 

  Standard_Real e1, e2, e3;
  Standard_Real J1min, J2min, J3min;
  Standard_Integer iprog;

  // (0) Init

  J->GetEstimation(e1, e2, e3);
  J1min = 1.e-8; J2min = J3min = (e1 + 1.e-8) * 1.e-6;

  if(e1 < J1min) e1 = J1min;// Like in
  if(e2 < J2min) e2 = J2min;// MOTLIS
  if(e3 < J3min) e3 = J3min;


  J->SetEstimation(e1, e2, e3);

  CCurrent = TheCurve;  
  CurrentTi = new TColStd_HArray1OfReal(1, myParameters->Length());  
  CurrentTi->ChangeArray1() = myParameters->Array1();
  OldTi = new (TColStd_HArray1OfReal) (1, CurrentTi->Length());
  OldTi->ChangeArray1() = CurrentTi->Array1();
  COld = CCurrent; 
  LNOLD = CBLONG = J->EstLength();
  Dependence = J->DependenceTable();

  J->SetCurve(CCurrent);
  FEmTool_Assembly * TheAssembly = 
    new  FEmTool_Assembly (Dependence->Array2(), J->AssemblyTable());

  //============        Optimization      ============================
  //  Standard_Integer inagain = 0;
  while (again) {

    // (1) Loop  Optimization / Estimation
    lestim = Standard_True;
    lconst = Standard_True;
    NbEst = 0;

    J->SetCurve(CCurrent);

    while(lestim) {

      //     (1.1) Curve's Optimization.
      EpsLength = SmallValue * CBLONG / NbrPnt;
      CNew = new (FEmTool_Curve) (CCurrent->Dimension(),
        CCurrent->NbElements(), CCurrent->Base(), EpsLength);
      CNew->Knots() = CCurrent->Knots();

      J->SetParameters(CurrentTi);
      EpsDeg = Min(WQuality * .1, CBLONG * .001);

      Optimization(J, *TheAssembly, lconst, EpsDeg, CNew, CurrentTi->Array1());

      lconst = Standard_False;

      //        (1.2) calcul des criteres de qualites et amelioration
      //              des estimation.
      ICDANA = J->QualityValues(J1min, J2min, J3min, 
        VALCRI[0], VALCRI[1], VALCRI[2]);

      if(ICDANA > 0) lconst = Standard_True;

      J->ErrorValues(ERRMAX, ERRQUA, ERRMOY);

      isnear = ((Sqrt(ERRQUA / NbrPnt) < 2*WQuality) && 
        (myNbIterations > 1));

      //       (1.3) Optimisation des ti par proj orthogonale
      //             et calcul de l'erreur aux points.

      if (isnear) {
        NewTi = new (TColStd_HArray1OfReal) (1, CurrentTi->Length());
        Project(CNew, CurrentTi->Array1(), 
          NewTi->ChangeArray1(),
          Ecarts, NumPnt, 
          ERRMAX, ERRQUA, ERRMOY, 2);
      }
      else  NewTi = CurrentTi;


      //        (1.4) Progression's test
      iprog = 0;
      if ((EROLD > WQuality) && (ERRMAX < 0.95*EROLD)) iprog++;
      if ((EROLD > WQuality) && (ERRMAX < 0.8*EROLD)) iprog++;  
      if ((EROLD > WQuality) && (ERRMAX < WQuality)) iprog++;
      if ((EROLD > WQuality) && (ERRMAX < 0.99*EROLD) 
        && (ERRMAX < 1.1*WQuality)) iprog++;  
      if ( VALCRI[0] < 0.975 * VOCRI[0]) iprog++;
      if ( VALCRI[0] < 0.9 * VOCRI[0]) iprog++; 
      if ( VALCRI[1] < 0.95 * VOCRI[1]) iprog++;
      if ( VALCRI[1] < 0.8 * VOCRI[1]) iprog++;  
      if ( VALCRI[2] < 0.95 * VOCRI[2]) iprog++;
      if ( VALCRI[2] < 0.8 * VOCRI[2]) iprog++;  
      if ((VOCRI[1]>SmallestValue)&&(VOCRI[2]>SmallestValue)) {
        if ((VALCRI[1]/VOCRI[1] + 2*VALCRI[2]/VOCRI[2]) < 2.8) iprog++;
      }

      if (iprog < 2 && NbEst == 0 ) {
        //             (1.5) Invalidation of new knots.
        VALCRI[0] = VOCRI[0];
        VALCRI[1] = VOCRI[1];
        VALCRI[2] = VOCRI[2];
        ERRMAX = EROLD;
        CBLONG =  LNOLD;
        CCurrent = COld;
        CurrentTi = OldTi;

        goto L8000; // exit
      }

      VOCRI[0] = VALCRI[0];
      VOCRI[1] = VALCRI[1];
      VOCRI[2] = VALCRI[2];
      LNOLD = CBLONG;
      EROLD = ERRMAX;    

      CCurrent = CNew;
      CurrentTi = NewTi;

      //       (1.6) Test if the Estimations seems  OK, else repeat
      NbEst++;
      lestim = ( (NbEst<MaxNbEst) && (ICDANA == 2)&& (iprog > 0) );      

      if (lestim && isnear)  {
        //           (1.7) Optimization of ti by ACR.

        std::stable_sort (CurrentTi->begin(), CurrentTi->end());

        Standard_Integer Decima = 4;

        CCurrent->Length(0., 1., CBLONG);
        J->EstLength() = CBLONG;

        ACR(CCurrent, CurrentTi->ChangeArray1(), Decima); 
        lconst = Standard_True;

      }
    }


    //     (2) loop of parametric / geometric optimization

    Iter = 1;
    ToOptim = ((Iter < myNbIterations) && (isnear));

    while(ToOptim) {
      Iter++;
      //     (2.1) Save curent results
      VOCRI[0] = VALCRI[0];
      VOCRI[1] = VALCRI[1];
      VOCRI[2] = VALCRI[2];
      EROLD = ERRMAX;
      LNOLD = CBLONG;
      COld = CCurrent;
      OldTi->ChangeArray1() = CurrentTi->Array1();

      //     (2.2) Optimization des ti by ACR.

      std::stable_sort (CurrentTi->begin(), CurrentTi->end());

      Standard_Integer Decima = 4;

      CCurrent->Length(0., 1., CBLONG);
      J->EstLength() = CBLONG;

      ACR(CCurrent, CurrentTi->ChangeArray1(), Decima); 
      lconst = Standard_True;

      //      (2.3) Optimisation des courbes
      EpsLength = SmallValue * CBLONG / NbrPnt;

      CNew = new (FEmTool_Curve) (CCurrent->Dimension(),
        CCurrent->NbElements(), CCurrent->Base(), EpsLength); 
      CNew->Knots() = CCurrent->Knots();

      J->SetParameters(CurrentTi);

      EpsDeg = Min(WQuality * .1, CBLONG * .001);
      Optimization(J, *TheAssembly, lconst, EpsDeg, CNew, CurrentTi->Array1());

      CCurrent = CNew;

      //      (2.4) calcul des criteres de qualites et amelioration
      //             des estimation.

      ICDANA = J->QualityValues(J1min, J2min, J3min, VALCRI[0], VALCRI[1], VALCRI[2]);
      if(ICDANA > 0) lconst = Standard_True;

      J->GetEstimation(e1, e2, e3);
      //       (2.5) Optimisation des ti par proj orthogonale

      NewTi = new (TColStd_HArray1OfReal) (1, CurrentTi->Length());
      Project(CCurrent, CurrentTi->Array1(), 
        NewTi->ChangeArray1(),
        Ecarts, NumPnt, 
        ERRMAX, ERRQUA, ERRMOY, 2);

      //       (2.6)  Test de non regression

      Standard_Integer iregre = 0;
      if (NbrConstraint < NbrPnt) {
        if ( (ERRMAX > WQuality) && (ERRMAX > 1.05*EROLD)) iregre++;
        if ( (ERRMAX > WQuality) && (ERRMAX > 2*EROLD)) iregre++;
        if ( (EROLD  > WQuality) && (ERRMAX <= 0.5*EROLD)) iregre--;
      }
      Standard_Real E1, E2, E3;
      J->GetEstimation(E1, E2, E3);
      if ( (VALCRI[0] > E1) && (VALCRI[0] > 1.1*VOCRI[0])) iregre++;
      if ( (VALCRI[1] > E2) && (VALCRI[1] > 1.1*VOCRI[1])) iregre++;
      if ( (VALCRI[2] > E3) && (VALCRI[2] > 1.1*VOCRI[2])) iregre++;


      if (iregre >= 2) {
        //      if (iregre >= 1) {
        // (2.7) on restaure l'iteration precedente
        VALCRI[0] = VOCRI[0];
        VALCRI[1] = VOCRI[1];
        VALCRI[2] = VOCRI[2];
        ERRMAX = EROLD;
        CBLONG = LNOLD;
        CCurrent = COld;
        CurrentTi->ChangeArray1() = OldTi->Array1();    
        ToOptim = Standard_False;
      }
      else { // Iteration is Ok.
        CCurrent = CNew;
        CurrentTi = NewTi;
      }
      if (Iter >= myNbIterations) ToOptim = Standard_False;
    }

    // (3) Decoupe eventuelle

    if ( (CCurrent->NbElements() < myMaxSegment) && myWithCutting ) {

      //    (3.1) Sauvgarde de l'etat precedent
      VOCRI[0] = VALCRI[0];
      VOCRI[1] = VALCRI[1];
      VOCRI[2] = VALCRI[2];
      EROLD = ERRMAX;
      COld = CCurrent;
      OldTi->ChangeArray1() = CurrentTi->Array1();

      //       (3.2) On arrange les ti : Trie + recadrage sur (0,1)
      //         ---> On trie, afin d'assurer l'ordre par la suite.

      std::stable_sort (CurrentTi->begin(), CurrentTi->end());

      if ((CurrentTi->Value(1)!= 0.) || 
        (CurrentTi->Value(NbrPnt)!= 1.)) {
          Standard_Real t, DelatT = 
            1.0 /(CurrentTi->Value(NbrPnt)-CurrentTi->Value(1));
          for (Standard_Integer ii=2; ii<NbrPnt; ii++) {
            t = (CurrentTi->Value(ii)-CurrentTi->Value(1))*DelatT;
            CurrentTi->SetValue(ii, t);
          }
          CurrentTi->SetValue(1, 0.);
          CurrentTi->SetValue(NbrPnt, 1.);
      }

      //       (3.3) Insert new Knots

      SplitCurve(CCurrent, CurrentTi->Array1(), EpsLength, CNew, iscut);
      if (!iscut) again = Standard_False;
      else {
        CCurrent =  CNew;
        // New Knots => New Assembly.
        J->SetCurve(CNew);
        delete TheAssembly;
        TheAssembly = new FEmTool_Assembly (Dependence->Array2(),
          J->AssemblyTable());
      }
    }
    else { again = Standard_False;}
  }

  //    ================   Great loop end   ===================

L8000:

  // (4) Compute the best Error.
  NewTi = new (TColStd_HArray1OfReal) (1, CurrentTi->Length());
  Project(CCurrent, CurrentTi->Array1(), 
    NewTi->ChangeArray1(),
    Ecarts, NumPnt, 
    ERRMAX, ERRQUA, ERRMOY, 10);

  // (5) field's update

  TheCurve = CCurrent;
  J->EstLength() = CBLONG;
  myParameters->ChangeArray1() = NewTi->Array1();
  myCriterium[0] = ERRQUA;
  myCriterium[1] = Sqrt(VALCRI[0]);
  myCriterium[2] = Sqrt(VALCRI[1]);
  myCriterium[3] = Sqrt(VALCRI[2]);
  myMaxError = ERRMAX;
  myMaxErrorIndex = NumPnt;
  if(NbrPnt > NbrConstraint)
    myAverageError = ERRMOY / (NbrPnt - NbrConstraint);
  else
    myAverageError = ERRMOY / NbrConstraint;

  delete TheAssembly;
}

//=======================================================================
//function : Optimization
//purpose  :  (like FORTRAN subroutine MINIMI)
//=======================================================================
void AppDef_Variational::Optimization(Handle(AppDef_SmoothCriterion)& J,
                                      FEmTool_Assembly& A,
                                      const Standard_Boolean ToAssemble,
                                      const Standard_Real EpsDeg,
                                      Handle(FEmTool_Curve)& Curve,
                                      const TColStd_Array1OfReal& Parameters) const
{
  Standard_Integer MxDeg = Curve->Base()->WorkDegree(),
    NbElm = Curve->NbElements(),
    NbDim = Curve->Dimension();

  Handle(FEmTool_HAssemblyTable) AssTable;

  math_Matrix H(0, MxDeg, 0, MxDeg);
  math_Vector G(0, MxDeg), Sol(1, A.NbGlobVar());

  Standard_Integer el, dim;

  A.GetAssemblyTable(AssTable);
  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;

  Standard_Real CBLONG = J->EstLength();

  // Updating Assembly
  if (ToAssemble) 
    A.NullifyMatrix();
  A.NullifyVector();


  for (el = 1; el <= NbElm; el++) {
    if (ToAssemble) {
      J->Hessian(el, 1, 1, H);
      for(dim = 1; dim <= NbDim; dim++)
        A.AddMatrix(el, dim, dim, H);
    }

    for(dim = 1; dim <= NbDim; dim++) {
      J->Gradient(el, dim, G);
      A.AddVector(el, dim, G);
    }
  }


  // Solution of system 
  if (ToAssemble) {
    if(NbConstr != 0) { //Treatment of constraints
      AssemblingConstraints(Curve, Parameters, CBLONG, A);
    }      
    A.Solve();
  }
  A.Solution(Sol);


  // Updating J
  J->SetCurve(Curve);
  J->InputVector(Sol, AssTable);

  // Updating Curve and reduction of degree

  Standard_Integer Newdeg;
  Standard_Real MaxError;

  if(NbConstr == 0) {
    for(el = 1; el <= NbElm; el++) {
      Curve->ReduceDegree(el, EpsDeg, Newdeg, MaxError);
    }
  }
  else {

    TColStd_Array1OfReal& TabInt = Curve->Knots();
    Standard_Integer Icnt = 1, p0 = Parameters.Lower() - myFirstPoint, point;
    for(el = 1; el <= NbElm; el++) {     
      while((Icnt < NbConstr) && 
        (Parameters(p0 + myTypConstraints->Value(2 * Icnt - 1)) <= TabInt(el))) Icnt++;
      point = p0 + myTypConstraints->Value(2 * Icnt - 1);
      if(Parameters(point) <= TabInt(el) || Parameters(point) >= TabInt(el + 1))
        Curve->ReduceDegree(el, EpsDeg, Newdeg, MaxError);
      else
        if(Curve->Degree(el) < MxDeg) Curve->SetDegree(el, MxDeg);
    }
  }   
}

void AppDef_Variational::Project(const Handle(FEmTool_Curve)& C,
                                 const TColStd_Array1OfReal& Ti,
                                 TColStd_Array1OfReal& ProjTi,
                                 TColStd_Array1OfReal& Distance,
                                 Standard_Integer& NumPoints,
                                 Standard_Real& MaxErr,
                                 Standard_Real& QuaErr,
                                 Standard_Real& AveErr,
                                 const Standard_Integer NbIterations) const

{
  // Initialisation

  const Standard_Real Seuil = 1.e-9, Eps = 1.e-12;

  MaxErr = QuaErr = AveErr = 0.;

  Standard_Integer Ipnt, NItCv, Iter, i, i0 = -myDimension, d0 = Distance.Lower() - 1;

  Standard_Real TNew, Dist, T0, Dist0, F1, F2, Aux, DF, Ecart;

  Standard_Boolean EnCour;

  TColStd_Array1OfReal ValOfC(1, myDimension), FirstDerOfC(1, myDimension), 
    SecndDerOfC(1, myDimension);

  for(Ipnt = 1; Ipnt <= ProjTi.Length(); Ipnt++) {

    i0 += myDimension;

    TNew = Ti(Ipnt);

    EnCour = Standard_True;
    NItCv = 0;
    Iter = 0;
    C->D0(TNew, ValOfC);

    Dist = 0;
    for(i = 1; i <= myDimension; i++) {
      Aux = ValOfC(i) - myTabPoints->Value(i0 + i);
      Dist += Aux * Aux;
    }
    Dist = Sqrt(Dist);

    // ------- Newton's method for solving (C'(t),C(t) - P) = 0

    while( EnCour ) {

      Iter++;
      T0 = TNew;
      Dist0 = Dist;

      C->D2(TNew, SecndDerOfC);
      C->D1(TNew, FirstDerOfC);

      F1 = F2 = 0.;
      for(i = 1; i <= myDimension; i++) {
        Aux = ValOfC(i) - myTabPoints->Value(i0 + i);
        DF = FirstDerOfC(i);
        F1 += Aux*DF;          // (C'(t),C(t) - P)
        F2 += DF*DF + Aux * SecndDerOfC(i); // ((C'(t),C(t) - P))'
      }

      if(Abs(F2) < Eps) 
        EnCour = Standard_False;
      else {
        // Formula of Newton x(k+1) = x(k) - F(x(k))/F'(x(k))
        TNew -= F1 / F2;
        if(TNew < 0.) TNew = 0.;
        if(TNew > 1.) TNew = 1.;


        // Analysis of result

        C->D0(TNew, ValOfC);

        Dist = 0;
        for(i = 1; i <= myDimension; i++) {
          Aux = ValOfC(i) - myTabPoints->Value(i0 + i);
          Dist += Aux * Aux;
        }
        Dist = Sqrt(Dist);

        Ecart = Dist0 - Dist;

        if(Ecart <= -Seuil) {
          // Pas d'amelioration on s'arrete
          EnCour = Standard_False;
          TNew = T0;
          Dist = Dist0;
        }
        else if(Ecart <= Seuil) 
          // Convergence
          NItCv++;
        else
          NItCv = 0;

        if((NItCv >= 2) || (Iter >= NbIterations)) EnCour = Standard_False;

      }
    }


    ProjTi(Ipnt) = TNew;
    Distance(d0 + Ipnt) = Dist;
    if(Dist > MaxErr) {
      MaxErr = Dist;
      NumPoints = Ipnt;
    }
    QuaErr += Dist * Dist;
    AveErr += Dist;
  }

  NumPoints = NumPoints + myFirstPoint - 1;// Setting NumPoints to interval [myFirstPoint, myLastPoint]

}

void AppDef_Variational::ACR(Handle(FEmTool_Curve)& Curve,
                             TColStd_Array1OfReal& Ti,
                             const Standard_Integer Decima) const
{

  const Standard_Real Eps = 1.e-8;

  TColStd_Array1OfReal& Knots = Curve->Knots(); 
  Standard_Integer NbrPnt = Ti.Length(), TiFirst = Ti.Lower(), TiLast = Ti.Upper(),
    KFirst = Knots.Lower(), KLast = Knots.Upper();

  Standard_Real CbLong, DeltaT, VTest, UNew, UOld, DU, TPara, TOld, DTInv, Ratio;
  Standard_Integer ipnt, ii, IElm, IOld, POld, PCnt, ICnt=0;
  Standard_Integer NbCntr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;

  //     (1) Calcul de la longueur de courbe

  Curve->Length(Ti(TiFirst), Ti(TiLast), CbLong);

  //     (2)  Mise de l'acr dans Ti

  if(NbrPnt >= 2) {

    //     (2.0) Initialisation
    DeltaT = (Ti(TiLast) - Ti(TiFirst)) / Decima;
    VTest = Ti(TiFirst) + DeltaT;

    if(NbCntr > 0) {
      PCnt = myTypConstraints->Value(1) - myFirstPoint + TiFirst;
      ICnt = 1;
    }
    else
      PCnt = TiLast + 1;

    UOld = 0.;

    TOld = Ti(TiFirst);
    POld = TiFirst;

    IElm = KFirst;
    IOld = IElm;

    Ti(TiFirst) = 0.;

    for(ipnt = TiFirst + 1; ipnt <= TiLast; ipnt++) {

      while((ICnt <= NbCntr) && (PCnt < ipnt)) {
        ICnt++;
        PCnt = myTypConstraints->Value(2*ICnt-1) - myFirstPoint + TiFirst;
      }

      TPara = Ti(ipnt);

      if(TPara >= VTest || PCnt == ipnt) {

        if ( Ti(TiLast) - TPara <= 1.e-2*DeltaT) {
          ipnt = TiLast;
          TPara = Ti(ipnt);
        }
        //        (2.2), (2.3) Cacul la longueur de courbe
        Curve->Length(Ti(TiFirst), TPara, UNew);

        UNew /= CbLong;

        while(Knots(IElm + 1) < TPara && IElm < KLast - 1) IElm++;

        //         (2.4) Mise a jours des parametres de decoupe
        DTInv = 1. / (TPara - TOld);
        DU = UNew - UOld;

        for(ii = IOld+1; ii <= IElm; ii++) {
          Ratio = (Knots(ii) - TOld) * DTInv;
          Knots(ii) = UOld + Ratio * DU;
        }

        //           (2.5) Mise a jours des parametres de points.

        //Very strange loop, because it never works (POld+1 > ipnt-1)
        for(ii = POld+1; ii <= ipnt-1; ii++) {
          Ratio = ( Ti(ii) - TOld ) * DTInv;
          Ti(ii) = UOld + Ratio * DU;
        }

        Ti(ipnt) = UNew;

        UOld = UNew;
        IOld = IElm;
        TOld = TPara;
        POld = ipnt;

      }
      //        --> Nouveau seuil parametrique pour le decimage

      if(TPara >= VTest) {
        //	ii = RealToInt((TPara - VTest + Eps) / DeltaT);
        //	VTest += (ii + 1) * DeltaT;
        VTest += Ceiling((TPara - VTest + Eps) / DeltaT) * DeltaT;
        if(VTest > 1. - Eps) VTest = 1.;
      }
    }
  }

  //     --- On ajuste les valeurs extremes

  Ti(TiFirst) = 0.;
  Ti(TiLast) = 1.;
  ii = TiLast - 1;
  while ( Ti(ii) > Knots(KLast) ) {
    Ti(ii) = 1.;
    --ii;
  }
  Knots(KFirst) = 0.;
  Knots(KLast) = 1.;

}

//----------------------------------------------------------//
// Standard_Integer NearIndex                               //
// Purpose: searching nearest index of TabPar corresponding //
// given value T (is similar to fortran routine MSRRE2)     //
//----------------------------------------------------------//

static Standard_Integer NearIndex(const Standard_Real T, 
                                  const TColStd_Array1OfReal& TabPar,
                                  const Standard_Real Eps, Standard_Integer& Flag)
{
  Standard_Integer Loi = TabPar.Lower(), Upi = TabPar.Upper();

  Flag = 0;

  if(T < TabPar(Loi)) { Flag = -1; return Loi;}
  if(T > TabPar(Upi)) { Flag =  1; return Upi;}

  Standard_Integer Ibeg = Loi, Ifin = Upi, Imidl;

  while(Ibeg + 1 != Ifin) {
    Imidl = (Ibeg + Ifin) / 2;
    if((T >= TabPar(Ibeg)) && (T <= TabPar(Imidl)))
      Ifin = Imidl;
    else
      Ibeg = Imidl;
  }

  if(Abs(T - TabPar(Ifin)) < Eps) return Ifin;

  return Ibeg;
}


//----------------------------------------------------------//
// void GettingKnots                                        //
// Purpose: calculating values of new Knots for elements    //
// with degree that is equal Deg                            //
//----------------------------------------------------------//

static void GettingKnots(const TColStd_Array1OfReal& TabPar, 
                         const Handle(FEmTool_Curve)& InCurve,
                         const Standard_Integer Deg,
                         Standard_Integer& NbElm,
                         TColStd_Array1OfReal& NewKnots)

{

  const Standard_Real Eps = 1.e-12;

  TColStd_Array1OfReal& OldKnots = InCurve->Knots();
  Standard_Integer NbMaxOld = InCurve->NbElements(); 
  Standard_Integer NbMax = NewKnots.Upper(), Ipt, Ipt1, Ipt2; 
  Standard_Integer el = 0, i1 = OldKnots.Lower(), i0 = i1 - 1, Flag;
  Standard_Real TPar;

  while((NbElm < NbMax) && (el < NbMaxOld)) {

    el++; i0++; i1++; // i0, i1 are indexes of left and right knots of element el

    if(InCurve->Degree(el) == Deg) {

      NbElm++;

      Ipt1 = NearIndex(OldKnots(i0), TabPar, Eps, Flag);
      if(Flag != 0) Ipt1 = TabPar.Lower();
      Ipt2 = NearIndex(OldKnots(i1), TabPar, Eps, Flag);
      if(Flag != 0) Ipt2 = TabPar.Upper();

      if(Ipt2 - Ipt1 >= 1) {

        Ipt = (Ipt1 + Ipt2) / 2;
        if(2 * Ipt == Ipt1 + Ipt2)
          TPar = 2. * TabPar(Ipt);
        else
          TPar = TabPar(Ipt) + TabPar(Ipt + 1);

        NewKnots(NbElm) = (OldKnots(i0) + OldKnots(i1) + TPar) / 4.;
      }
      else
        NewKnots(NbElm) = (OldKnots(i0) + OldKnots(i1)) / 2.;
    }
  }
}

void AppDef_Variational::SplitCurve(const Handle(FEmTool_Curve)& InCurve,
                                    const TColStd_Array1OfReal& Ti,
                                    const Standard_Real CurveTol,
                                    Handle(FEmTool_Curve)& OutCurve,
                                    Standard_Boolean& iscut) const
{
  Standard_Integer NbElmOld = InCurve->NbElements(); 

  if(NbElmOld >= myMaxSegment) {iscut = Standard_False; return;}
#ifdef OCCT_DEBUG
  Standard_Integer MaxDegree = 
#endif
    InCurve->Base()->WorkDegree();
  Standard_Integer NbElm = NbElmOld;
  TColStd_Array1OfReal NewKnots(NbElm + 1, myMaxSegment);
#ifndef OCCT_DEBUG
  GettingKnots(Ti, InCurve, InCurve->Base()->WorkDegree(), NbElm, NewKnots);
  GettingKnots(Ti, InCurve, InCurve->Base()->WorkDegree() - 1, NbElm, NewKnots);
#else
  GettingKnots(Ti, InCurve, MaxDegree, NbElm, NewKnots);
  GettingKnots(Ti, InCurve, MaxDegree - 1, NbElm, NewKnots);

#endif
  if(NbElm > NbElmOld) {

    iscut = Standard_True;

    OutCurve = new FEmTool_Curve(InCurve->Dimension(), NbElm, InCurve->Base(), CurveTol); 
    TColStd_Array1OfReal& OutKnots = OutCurve->Knots();
    TColStd_Array1OfReal& InKnots = InCurve->Knots();

    Standard_Integer i, i0 = OutKnots.Lower();
    for(i = InKnots.Lower(); i <= InKnots.Upper(); i++) OutKnots(i) = InKnots(i);
    for(i = NbElmOld + 1; i <= NbElm; i++) OutKnots(i + i0) = NewKnots(i);

    std::sort (OutKnots.begin(), OutKnots.end());
  }
  else
    iscut = Standard_False;

}

//=======================================================================
//function : InitSmoothCriterion
//purpose  : Initializes the SmoothCriterion
//=======================================================================
void AppDef_Variational::InitSmoothCriterion()
{

  const Standard_Real Eps2 = 1.e-6, Eps3 = 1.e-9;
  //  const Standard_Real J1 = .01, J2 = .001, J3 = .001;



  Standard_Real Length;

  InitParameters(Length);

  mySmoothCriterion->SetParameters(myParameters);

  Standard_Real E1, E2, E3;

  InitCriterionEstimations(Length, E1,  E2,  E3);
  /*
  J1 = 1.e-8; J2 = J3 = (E1 + 1.e-8) * 1.e-6;

  if(E1 < J1) E1 = J1;
  if(E2 < J2) E2 = J2;
  if(E3 < J3) E3 = J3;
  */
  mySmoothCriterion->EstLength() = Length;
  mySmoothCriterion->SetEstimation(E1, E2, E3);

  Standard_Real WQuadratic, WQuality;

  if(!myWithMinMax && myTolerance != 0.) 
    WQuality = myTolerance;
  else if (myTolerance == 0.)
    WQuality = 1.;
  else
    WQuality = Max(myTolerance, Eps2 * Length);

  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;
  WQuadratic = Sqrt((Standard_Real)(myNbPoints - NbConstr)) * WQuality;
  if(WQuadratic > Eps3) WQuadratic = 1./ WQuadratic;

  if(WQuadratic == 0.) WQuadratic = Max(Sqrt(E1), 1.);


  mySmoothCriterion->SetWeight(WQuadratic, WQuality, 
    myPercent[0], myPercent[1], myPercent[2]);


  Handle(PLib_Base) TheBase = new PLib_HermitJacobi(myMaxDegree, myContinuity);
  Handle(FEmTool_Curve) TheCurve;
  Standard_Integer NbElem;
  Standard_Real CurvTol = Eps2 * Length / myNbPoints;

  // Decoupe de l'intervalle en fonction des contraintes
  if(myWithCutting == Standard_True && NbConstr != 0) {

    InitCutting(TheBase, CurvTol, TheCurve);

  }
  else {

    NbElem = 1;
    TheCurve = new FEmTool_Curve(myDimension, NbElem, TheBase, CurvTol);
    TheCurve->Knots().SetValue(TheCurve->Knots().Lower(), 
      myParameters->Value(myFirstPoint));
    TheCurve->Knots().SetValue(TheCurve->Knots().Upper(), 
      myParameters->Value(myLastPoint));

  }

  mySmoothCriterion->SetCurve(TheCurve);

  return;

}

//
//=======================================================================
//function : InitParameters
//purpose  : Calculation of initial estimation of the multicurve's length
//           and parameters for multipoints.
//=======================================================================
//
void AppDef_Variational::InitParameters(Standard_Real& Length)
{

  const Standard_Real Eps1 = Precision::Confusion() * .01;

  Standard_Real aux, dist;
  Standard_Integer i, i0, i1 = 0, ipoint;


  Length = 0.;  
  myParameters->SetValue(myFirstPoint, Length);

  for(ipoint = myFirstPoint + 1; ipoint <= myLastPoint; ipoint++) {
    i0 = i1; i1 += myDimension;
    dist = 0;
    for(i = 1; i <= myDimension; i++) {
      aux = myTabPoints->Value(i1 + i) - myTabPoints->Value(i0 + i);
      dist += aux * aux;
    }
    Length += Sqrt(dist);
    myParameters->SetValue(ipoint, Length);
  }


  if(Length <= Eps1) 
    throw Standard_ConstructionError("AppDef_Variational::InitParameters");


  for(ipoint = myFirstPoint + 1; ipoint <= myLastPoint - 1; ipoint++) 
    myParameters->SetValue(ipoint, myParameters->Value(ipoint) / Length);

  myParameters->SetValue(myLastPoint, 1.);

  // Avec peu de point il y a sans doute sous estimation ...
  if(myNbPoints < 10) Length *= (1. + 0.1 / (myNbPoints - 1));
}

//
//=======================================================================
//function : InitCriterionEstimations
//purpose  : Calculation of initial estimation of the linear criteria
//           
//=======================================================================
//
void AppDef_Variational::InitCriterionEstimations(const Standard_Real Length, 
                                                  Standard_Real& E1,  
                                                  Standard_Real& E2,  
                                                  Standard_Real& E3) const
{
  E1 = Length * Length;

  const Standard_Real Eps1 = Precision::Confusion() * .01;

  math_Vector VTang1(1, myDimension), VTang2(1, myDimension), VTang3(1, myDimension),
    VScnd1(1, myDimension), VScnd2(1, myDimension), VScnd3(1, myDimension);

  // ========== Treatment of first point =================

  Standard_Integer ipnt = myFirstPoint;

  EstTangent(ipnt, VTang1);
  ipnt++;
  EstTangent(ipnt, VTang2);
  ipnt++;
  EstTangent(ipnt, VTang3);

  ipnt = myFirstPoint;
  EstSecnd(ipnt, VTang1, VTang2, Length, VScnd1);
  ipnt++;
  EstSecnd(ipnt, VTang1, VTang3, Length, VScnd2);

  //  Modified by skv - Fri Apr  8 14:58:12 2005 OCC8559 Begin
  //   Standard_Real Delta = .5 * (myParameters->Value(ipnt) - myParameters->Value(--ipnt));
  Standard_Integer anInd = ipnt;
  Standard_Real    Delta = .5 * (myParameters->Value(anInd) - myParameters->Value(--ipnt));
  //  Modified by skv - Fri Apr  8 14:58:12 2005 OCC8559 End

  if(Delta <= Eps1) Delta = 1.;

  E2 = VScnd1.Norm2() * Delta;

  E3 = (Delta > Eps1) ? VScnd2.Subtracted(VScnd1).Norm2() / (4. * Delta) : 0.;
  // ========== Treatment of internal points =================

  Standard_Integer CurrPoint = 2;

  for(ipnt = myFirstPoint + 1; ipnt < myLastPoint; ipnt++) {

    Delta = .5 * (myParameters->Value(ipnt + 1) - myParameters->Value(ipnt - 1));

    if(CurrPoint == 1) {
      if(ipnt + 1 != myLastPoint) {
        EstTangent(ipnt + 2, VTang3);
        EstSecnd(ipnt + 1, VTang1, VTang3, Length, VScnd2);
      }
      else
        EstSecnd(ipnt + 1, VTang1, VTang2, Length, VScnd2);

      E2 += VScnd1.Norm2() * Delta;
      E3 += (Delta > Eps1) ? VScnd2.Subtracted(VScnd3).Norm2() / (4. * Delta) : 0.;

    }
    else if(CurrPoint == 2) {
      if(ipnt + 1 != myLastPoint) {
        EstTangent(ipnt + 2, VTang1);
        EstSecnd(ipnt + 1, VTang2, VTang1, Length, VScnd3);
      }
      else
        EstSecnd(ipnt + 1, VTang2, VTang3, Length, VScnd3);

      E2 += VScnd2.Norm2() * Delta;
      E3 += (Delta > Eps1) ? VScnd3.Subtracted(VScnd1).Norm2() / (4. * Delta) : 0.;

    }
    else {
      if(ipnt + 1 != myLastPoint) {
        EstTangent(ipnt + 2, VTang2);
        EstSecnd(ipnt + 1, VTang3, VTang2, Length, VScnd1);
      }
      else
        EstSecnd(ipnt + 1, VTang3, VTang1, Length, VScnd1);

      E2 += VScnd3.Norm2() * Delta;
      E3 += (Delta > Eps1) ? VScnd1.Subtracted(VScnd2).Norm2() / (4. * Delta) : 0.;

    }

    CurrPoint++; if(CurrPoint == 4) CurrPoint = 1;
  }

  // ========== Treatment of last point =================

  Delta = .5 * (myParameters->Value(myLastPoint) - myParameters->Value(myLastPoint - 1));
  if(Delta <= Eps1) Delta = 1.;

  Standard_Real aux;

  if(CurrPoint == 1) {

    E2 += VScnd1.Norm2() * Delta;
    aux = VScnd1.Subtracted(VScnd3).Norm2();
    E3 += (Delta > Eps1) ? aux / (4. * Delta) : aux;

  }
  else if(CurrPoint == 2) {

    E2 += VScnd2.Norm2() * Delta;
    aux = VScnd2.Subtracted(VScnd1).Norm2();
    E3 += (Delta > Eps1) ? aux / (4. * Delta) : aux;

  }
  else {

    E2 += VScnd3.Norm2() * Delta;
    aux = VScnd3.Subtracted(VScnd2).Norm2();
    E3 += (Delta > Eps1) ? aux / (4. * Delta) : aux;

  }

  aux = Length * Length;

  E2 *= aux; E3 *= aux;


}

//
//=======================================================================
//function : EstTangent
//purpose  : Calculation of  estimation of the Tangent
//           (see fortran routine MMLIPRI)
//=======================================================================
//

void AppDef_Variational::EstTangent(const Standard_Integer ipnt, 
                                    math_Vector& VTang) const

{
  Standard_Integer i ;
  const Standard_Real Eps1 = Precision::Confusion() * .01;
  const Standard_Real EpsNorm = 1.e-9;

  Standard_Real Wpnt = 1.;


  if(ipnt == myFirstPoint) {
    // Estimation at first point
    if(myNbPoints < 3) 
      Wpnt = 0.;
    else {

      Standard_Integer adr1 = 1, adr2 = adr1 + myDimension, 
        adr3 = adr2 + myDimension;

      math_Vector Pnt1((Standard_Real*)&myTabPoints->Value(adr1), 1, myDimension);
      math_Vector Pnt2((Standard_Real*)&myTabPoints->Value(adr2), 1, myDimension);
      math_Vector Pnt3((Standard_Real*)&myTabPoints->Value(adr3), 1, myDimension);

      // Parabolic interpolation
      // if we have parabolic interpolation: F(t) = A0 + A1*t + A2*t*t,
      // first derivative for t=0 is A1 = ((d2-1)*P1 + P2 - d2*P3)/(d*(1-d))
      //       d= |P2-P1|/(|P2-P1|+|P3-P2|), d2 = d*d
      Standard_Real V1 = Pnt2.Subtracted(Pnt1).Norm();
      Standard_Real V2 = 0.;
      if(V1 > Eps1) V2 = Pnt3.Subtracted(Pnt2).Norm();
      if(V2 > Eps1) {
        Standard_Real d = V1 / (V1 + V2), d1;
        d1 = 1. / (d * (1 - d)); d *= d;
        VTang = ((d - 1.) * Pnt1 + Pnt2 - d * Pnt3) * d1;
      }
      else {
        // Simple 2-point estimation

        VTang = Pnt2 - Pnt1;
      }
    }
  }
  else if (ipnt == myLastPoint) {
    // Estimation at last point
    if(myNbPoints < 3) 
      Wpnt = 0.;
    else {

      Standard_Integer adr1 = (myLastPoint - 3) * myDimension + 1, 
        adr2 = adr1 + myDimension, 
        adr3 = adr2 + myDimension;

      math_Vector Pnt1((Standard_Real*)&myTabPoints->Value(adr1), 1, myDimension);
      math_Vector Pnt2((Standard_Real*)&myTabPoints->Value(adr2), 1, myDimension);
      math_Vector Pnt3((Standard_Real*)&myTabPoints->Value(adr3), 1, myDimension);

      // Parabolic interpolation
      // if we have parabolic interpolation: F(t) = A0 + A1*t + A2*t*t,
      // first derivative for t=1 is 2*A2 + A1 = ((d2+1)*P1 - P2 - d2*P3)/(d*(1-d))
      //       d= |P2-P1|/(|P2-P1|+|P3-P2|), d2 = d*(d-2)
      Standard_Real V1 = Pnt2.Subtracted(Pnt1).Norm();
      Standard_Real V2 = 0.;
      if(V1 > Eps1) V2 = Pnt3.Subtracted(Pnt2).Norm();
      if(V2 > Eps1) {
        Standard_Real d = V1 / (V1 + V2), d1;
        d1 = 1. / (d * (1 - d)); d *= d - 2;
        VTang = ((d + 1.) * Pnt1 - Pnt2 - d * Pnt3) * d1;
      }
      else {
        // Simple 2-point estimation

        VTang = Pnt3 - Pnt2;
      }
    }
  }
  else {

    Standard_Integer adr1 = (ipnt - myFirstPoint - 1) * myDimension + 1, 
      adr2 = adr1 + 2 * myDimension;

    math_Vector Pnt1((Standard_Real*)&myTabPoints->Value(adr1), 1, myDimension);
    math_Vector Pnt2((Standard_Real*)&myTabPoints->Value(adr2), 1, myDimension);

    VTang = Pnt2 - Pnt1;

  }

  Standard_Real Vnorm = VTang.Norm();

  if(Vnorm <= EpsNorm)
    VTang.Init(0.);
  else 
    VTang /= Vnorm;

  // Estimation with constraints

  Standard_Real Wcnt = 0.;
  Standard_Integer IdCnt = 1;

  // Warning!! Here it is suppoused that all points are in range [myFirstPoint, myLastPoint]

  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;
  math_Vector VCnt(1, myDimension, 0.);

  if(NbConstr > 0) {

    while(myTypConstraints->Value(2 * IdCnt - 1) < ipnt &&
      IdCnt <= NbConstr) IdCnt++;
    if((myTypConstraints->Value(2 * IdCnt - 1) == ipnt) &&
      (myTypConstraints->Value(2 * IdCnt) >= 1)) {
        Wcnt = 1.;
        Standard_Integer i0 = 2 * myDimension * (IdCnt - 1), k = 0;
        for( i = 1; i <= myNbP3d; i++) {
          for(Standard_Integer j = 1; j <= 3; j++) 
            VCnt(++k) = myTabConstraints->Value(++i0);
          i0 += 3;
        }
        for(i = 1; i <= myNbP2d; i++) {
          for(Standard_Integer j = 1; j <= 2; j++) 
            VCnt(++k) = myTabConstraints->Value(++i0);
          i0 += 2;
        }
    }
  }

  // Averaging of estimation

  Standard_Real Denom = Wpnt + Wcnt;
  if(Denom == 0.) Denom = 1.;
  else            Denom = 1. / Denom;

  VTang = (Wpnt * VTang + Wcnt * VCnt) * Denom;

  Vnorm = VTang.Norm();

  if(Vnorm <= EpsNorm)
    VTang.Init(0.);
  else 
    VTang /= Vnorm;


}

//
//=======================================================================
//function : EstSecnd
//purpose  : Calculation of  estimation of the second derivative
//           (see fortran routine MLIMSCN)
//=======================================================================
//
void AppDef_Variational::EstSecnd(const Standard_Integer ipnt, 
                                  const math_Vector& VTang1, 
                                  const math_Vector& VTang2, 
                                  const Standard_Real Length, 
                                  math_Vector& VScnd) const
{
  Standard_Integer i ;

  const Standard_Real Eps = 1.e-9;

  Standard_Real Wpnt = 1.;

  Standard_Real aux;

  if(ipnt == myFirstPoint)
    aux = myParameters->Value(ipnt + 1) - myParameters->Value(ipnt);
  else if(ipnt == myLastPoint)
    aux = myParameters->Value(ipnt) - myParameters->Value(ipnt - 1);
  else
    aux = myParameters->Value(ipnt + 1) - myParameters->Value(ipnt - 1);

  if(aux <= Eps)
    aux = 1.;
  else
    aux = 1. / aux;

  VScnd = (VTang2 - VTang1) * aux;

  // Estimation with constraints

  Standard_Real Wcnt = 0.;
  Standard_Integer IdCnt = 1;

  // Warning!! Here it is suppoused that all points are in range [myFirstPoint, myLastPoint]

  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;
  math_Vector VCnt(1, myDimension, 0.);

  if(NbConstr > 0) {

    while(myTypConstraints->Value(2 * IdCnt - 1) < ipnt &&
      IdCnt <= NbConstr) IdCnt++;

    if((myTypConstraints->Value(2 * IdCnt - 1) == ipnt) &&
      (myTypConstraints->Value(2 * IdCnt) >= 2))  {
        Wcnt = 1.;
        Standard_Integer i0 = 2 * myDimension * (IdCnt - 1) + 3, k = 0;
        for( i = 1; i <= myNbP3d; i++) {
          for(Standard_Integer j = 1; j <= 3; j++) 
            VCnt(++k) = myTabConstraints->Value(++i0);
          i0 += 3;
        }
        i0--;
        for(i = 1; i <= myNbP2d; i++) {
          for(Standard_Integer j = 1; j <= 2; j++) 
            VCnt(++k) = myTabConstraints->Value(++i0);
          i0 += 2;
        }
    }
  }

  // Averaging of estimation

  Standard_Real Denom = Wpnt + Wcnt;
  if(Denom == 0.) Denom = 1.;
  else            Denom = 1. / Denom;

  VScnd = (Wpnt * VScnd + (Wcnt * Length) * VCnt) * Denom;

}


//
//=======================================================================
//function : InitCutting
//purpose  : Realisation of curve's cutting a priory accordingly to 
//           constraints (see fortran routine MLICUT)
//=======================================================================
//
void AppDef_Variational::InitCutting(const Handle(PLib_Base)& aBase,
                                     const Standard_Real CurvTol, 
                                     Handle(FEmTool_Curve)& aCurve) const
{

  // Definition of number of elements
  Standard_Integer ORCMx = -1, NCont = 0, i, kk, NbElem;
  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;

  for(i = 1; i <= NbConstr; i++) {
    kk = Abs(myTypConstraints->Value(2 * i)) + 1;
    ORCMx = Max(ORCMx, kk);
    NCont += kk;
  }

  if(ORCMx > myMaxDegree - myNivCont) 
    throw Standard_ConstructionError("AppDef_Variational::InitCutting");

  Standard_Integer NLibre = Max(myMaxDegree - myNivCont - (myMaxDegree + 1) / 4,
    myNivCont + 1);

  NbElem = (NCont % NLibre == 0) ? NCont / NLibre : NCont / NLibre + 1;

  while((NbElem > myMaxSegment) && (NLibre < myMaxDegree - myNivCont)) {

    NLibre++;
    NbElem = (NCont % NLibre == 0) ? NCont / NLibre : NCont / NLibre + 1;

  }


  if(NbElem > myMaxSegment) 
    throw Standard_ConstructionError("AppDef_Variational::InitCutting");


  aCurve = new FEmTool_Curve(myDimension, NbElem, aBase, CurvTol);

  Standard_Integer NCnt = (NCont - 1) / NbElem + 1;
  Standard_Integer NPlus = NbElem - (NCnt * NbElem - NCont);

  TColStd_Array1OfReal& Knot = aCurve->Knots();

  Standard_Integer IDeb = 0, IFin = NbConstr + 1,
    NDeb = 0, NFin = 0,
    IndEl = Knot.Lower(), IUpper = Knot.Upper(),
    NbEl = 0;


  Knot(IndEl) = myParameters->Value(myFirstPoint);
  Knot(IUpper) = myParameters->Value(myLastPoint);

  while(NbElem - NbEl > 1) {

    IndEl++; NbEl++;
    if(NPlus == 0) NCnt--;

    while(NDeb < NCnt && IDeb < IFin) {
      IDeb++;
      NDeb += Abs(myTypConstraints->Value(2 * IDeb)) + 1;
    }

    if(NDeb == NCnt) {
      NDeb = 0;
      if(NPlus == 1 && 
        myParameters->Value(myTypConstraints->Value(2 * IDeb - 1)) > Knot(IndEl - 1))

        Knot(IndEl) = myParameters->Value(myTypConstraints->Value(2 * IDeb - 1));
      else
        Knot(IndEl) = (myParameters->Value(myTypConstraints->Value(2 * IDeb - 1)) +
        myParameters->Value(myTypConstraints->Value(2 * IDeb + 1))) / 2;

    }
    else {
      NDeb -= NCnt;
      Knot(IndEl) = myParameters->Value(myTypConstraints->Value(2 * IDeb - 1));
    }

    NPlus--;
    if(NPlus == 0) NCnt--;

    if(NbElem - NbEl == 1) break;

    NbEl++;

    while(NFin < NCnt && IDeb < IFin) {
      IFin--;
      NFin += Abs(myTypConstraints->Value(2 * IFin)) + 1;
    }

    if(NFin == NCnt) {
      NFin = 0;
      Knot(IUpper + 1 - IndEl) = (myParameters->Value(myTypConstraints->Value(2 * IFin - 1)) +
        myParameters->Value(myTypConstraints->Value(2 * IFin - 3))) / 2;
    }
    else {
      NFin -= NCnt;
      if(myParameters->Value(myTypConstraints->Value(2 * IFin - 1)) < Knot(IUpper - IndEl + 1))
        Knot(IUpper + 1 - IndEl) = myParameters->Value(myTypConstraints->Value(2 * IFin - 1));
      else
        Knot(IUpper + 1 - IndEl) = (myParameters->Value(myTypConstraints->Value(2 * IFin - 1)) +
        myParameters->Value(myTypConstraints->Value(2 * IFin - 3))) / 2;
    }
  }
}

//=======================================================================
//function : Adjusting
//purpose  : Smoothing's  adjusting like STRIM routine "MAJLIS"
//=======================================================================
void AppDef_Variational::Adjusting(
                                   Handle(AppDef_SmoothCriterion)& J,
                                   Standard_Real& WQuadratic,
                                   Standard_Real& WQuality,
                                   Handle(FEmTool_Curve)& TheCurve,
                                   TColStd_Array1OfReal& Ecarts) 
{

  //  cout << "=========== Adjusting =============" << endl;

  /* Initialized data */

  const Standard_Integer mxiter = 2;
  const Standard_Real eps1 = 1e-6;
  Standard_Integer NbrPnt = myLastPoint - myFirstPoint + 1;
  Standard_Integer NbrConstraint = myNbPassPoints + myNbTangPoints + myNbCurvPoints;
  Standard_Real CurvTol = eps1 * J->EstLength() / NbrPnt;


  /* Local variables */
  Standard_Integer iter, ipnt;
  Standard_Real ecart, emold, erold, tpara;
  Standard_Real vocri[4], j1cibl, vtest, vseuil;
  Standard_Integer i, numint, flag;
  TColStd_Array1OfReal tbpoid(myFirstPoint, myLastPoint);
  Standard_Boolean loptim, lrejet;
  Handle(AppDef_SmoothCriterion) JNew;
  Handle(FEmTool_Curve) CNew;
  Standard_Real E1, E2, E3;


  /* (0.b) Initialisations */

  loptim = Standard_True;
  iter = 0;
  tbpoid.Init(1.);


  /* ============   boucle sur le moteur de lissage  ============== */

  vtest = WQuality * .9;
  j1cibl = Sqrt(myCriterium[0] / (NbrPnt - NbrConstraint));

  while(loptim) {

    ++iter;

    /*     (1) Sauvegarde de l'etat precedents */

    vocri[0] = myCriterium[0];
    vocri[1] = myCriterium[1];
    vocri[2] = myCriterium[2];
    vocri[3] = myCriterium[3];
    erold = myMaxError; 
    emold = myAverageError; 

    /*     (2) Augmentation du poids des moindre carre */

    if (j1cibl > vtest) {
      WQuadratic = j1cibl / vtest * WQuadratic;
    }

    /*     (3) Augmentation du poid associe aux points a problemes */

    vseuil = WQuality * .88;

    for (ipnt = myFirstPoint; ipnt <= myLastPoint; ++ipnt) {
      if (Ecarts(ipnt) > vtest) {
        ecart = (Ecarts(ipnt) - vseuil) / WQuality;
        tbpoid(ipnt) = (ecart * 3 + 1.) * tbpoid(ipnt);
      }
    }

    /*     (4) Decoupe force */

    if (TheCurve->NbElements() < myMaxSegment && myWithCutting) {

      numint = NearIndex(myParameters->Value(myMaxErrorIndex), TheCurve->Knots(), 0, flag);

      tpara = (TheCurve->Knots()(numint) + TheCurve->Knots()(numint + 1) + 
        myParameters->Value(myMaxErrorIndex) * 2) / 4;

      CNew = new FEmTool_Curve(myDimension, TheCurve->NbElements() + 1, 
        TheCurve->Base(), CurvTol);

      for(i = 1; i <= numint; i++) CNew->Knots()(i) = TheCurve->Knots()(i);
      for(i = numint + 1; i <= TheCurve->Knots().Length(); i++) 
        CNew->Knots()(i + 1) = TheCurve->Knots()(i);

      CNew->Knots()(numint + 1) = tpara;

    } else {

      CNew = new FEmTool_Curve(myDimension, TheCurve->NbElements(), TheCurve->Base(), CurvTol);

      CNew->Knots() = TheCurve->Knots();
    }


    JNew = new AppDef_LinearCriteria(mySSP, myFirstPoint, myLastPoint);

    JNew->EstLength() = J->EstLength();

    J->GetEstimation(E1, E2, E3);

    JNew->SetEstimation(E1, E2, E3);

    JNew->SetParameters(myParameters);

    JNew->SetWeight(WQuadratic, WQuality, myPercent[0], myPercent[1], myPercent[2]);

    JNew->SetWeight(tbpoid);

    JNew->SetCurve(CNew);

    /*     (5) Relissage */

    TheMotor(JNew, WQuadratic, WQuality, CNew, Ecarts);

    /*     (6) Tests de rejet */

    j1cibl = Sqrt(myCriterium[0] / (NbrPnt - NbrConstraint));
    vseuil = Sqrt(vocri[1]) + (erold - myMaxError) * 4;

    lrejet = ((myMaxError > WQuality) && (myMaxError > erold * 1.01)) || (Sqrt(myCriterium[1]) > vseuil * 1.05);

    if (lrejet) {
      myCriterium[0] = vocri[0];
      myCriterium[1] = vocri[1];
      myCriterium[2] = vocri[2];
      myCriterium[3] = vocri[3];
      myMaxError = erold;
      myAverageError = emold;

      loptim = Standard_False;
    }
    else {
      J = JNew;
      TheCurve = CNew;
      J->SetCurve(TheCurve);
    }

    /*     (7) Test de convergence */

    if (((iter >= mxiter) && (myMaxSegment == CNew->NbElements())) || myMaxError < WQuality) {
      loptim = Standard_False;
    }
  }
}

static Standard_Boolean NotParallel(gp_Vec& T, gp_Vec& V)
{
  V = T;
  V.SetX(V.X() + 1.);
  if (V.CrossMagnitude(T) > 1.e-12)
    return Standard_True;
  V.SetY(V.Y() + 1.);
  if (V.CrossMagnitude(T) > 1.e-12)
    return Standard_True;
  V.SetZ(V.Z() + 1.);
  if (V.CrossMagnitude(T) > 1.e-12)
    return Standard_True;
  return Standard_False;
}

void AppDef_Variational::AssemblingConstraints(const Handle(FEmTool_Curve)& Curve,
                                               const TColStd_Array1OfReal& Parameters,
                                               const Standard_Real CBLONG,
                                               FEmTool_Assembly& A ) const
{

  Standard_Integer MxDeg = Curve->Base()->WorkDegree(),
    NbElm = Curve->NbElements(),
    NbDim = Curve->Dimension();

  TColStd_Array1OfReal G0(0, MxDeg), G1(0, MxDeg), G2(0, MxDeg);
  math_Vector V0((Standard_Real*)&G0(0), 0, MxDeg), 
    V1((Standard_Real*)&G1(0), 0, MxDeg), 
    V2((Standard_Real*)&G2(0), 0, MxDeg);

  Standard_Integer IndexOfConstraint, Ng3d, Ng2d, NBeg2d, NPass, NgPC1, 
    NTang3d, NTang2d,  
    Point, TypOfConstr, 
    p0 = Parameters.Lower() - myFirstPoint,
    curel = 1, el, i, ipnt, ityp, j, k, pnt, curdim,
    jt, Ntheta = 6 * myNbP3d + 2 * myNbP2d;
  Standard_Integer NbConstr = myNbPassPoints + myNbTangPoints + myNbCurvPoints;

  //  Ng3d = 3 * NbConstr + 2 * myNbTangPoints + 5 * myNbCurvPoints;
  //  Ng2d = 2 * NbConstr + 1 * myNbTangPoints + 3 * myNbCurvPoints;
  Ng3d = 3 * NbConstr + 3 * myNbTangPoints + 5 * myNbCurvPoints;
  Ng2d = 2 * NbConstr + 2 * myNbTangPoints + 3 * myNbCurvPoints;
  NBeg2d = Ng3d * myNbP3d;
  //  NgPC1 = NbConstr + myNbCurvPoints;
  NgPC1 = NbConstr + myNbTangPoints + myNbCurvPoints;
  NPass = 0; 
  NTang3d = 3 * NgPC1;
  NTang2d = 2 * NgPC1;

  TColStd_Array1OfReal& Intervals = Curve->Knots();

  Standard_Real t, R1, R2;

  Handle(PLib_Base) myBase = Curve->Base();
  Handle(PLib_HermitJacobi) myHermitJacobi = Handle(PLib_HermitJacobi)::DownCast (myBase);
  Standard_Integer Order = myHermitJacobi->NivConstr() + 1;

  Standard_Real UFirst, ULast, coeff, c0, mfact, mfact1;

  A.NullifyConstraint();

  ipnt = -1;
  ityp = 0;
  for(i = 1; i <= NbConstr; i++) {

    ipnt += 2; ityp += 2;

    Point = myTypConstraints->Value(ipnt);
    TypOfConstr = myTypConstraints->Value(ityp);

    t = Parameters(p0 + Point);

    for(el = curel; el <= NbElm; ) {
      if( t <= Intervals(++el) ) {
        curel = el - 1;
        break;
      }
    }


    UFirst = Intervals(curel); ULast = Intervals(curel + 1);
    coeff = (ULast - UFirst)/2.; c0 = (ULast + UFirst)/2.;

    t = (t - c0) / coeff;

    if(TypOfConstr == 0) {
      myBase->D0(t, G0);
      for(k = 1; k < Order; k++) {
        mfact = Pow(coeff, k);
        G0(k) *= mfact;
        G0(k + Order) *= mfact;
      }
    }
    else if(TypOfConstr == 1) {
      myBase->D1(t, G0, G1);
      for(k = 1; k < Order; k++) {
        mfact = Pow(coeff, k);
        G0(k) *= mfact;
        G0(k + Order) *= mfact;
        G1(k) *= mfact;
        G1(k + Order) *= mfact;
      }
      mfact = 1./coeff;
      for(k = 0; k <= MxDeg; k++) {
        G1(k) *= mfact;
      }
    }
    else {
      myBase->D2(t, G0, G1, G2);
      for(k = 1; k < Order; k++) {
        mfact = Pow(coeff, k);
        G0(k) *= mfact;
        G0(k + Order) *= mfact;
        G1(k) *= mfact;
        G1(k + Order) *= mfact;
        G2(k) *= mfact;
        G2(k + Order) *= mfact;
      }
      mfact = 1. / coeff;
      mfact1 = mfact / coeff;
      for(k = 0; k <= MxDeg; k++) {
        G1(k) *= mfact;
        G2(k) *= mfact1;
      }
    }

    NPass++;

    j = NbDim * (Point - myFirstPoint);
    Standard_Integer n0 = NPass;
    curdim = 0;
    for(pnt = 1; pnt <= myNbP3d; pnt++) {
      IndexOfConstraint = n0;
      for(k = 1; k <= 3; k++) {
        curdim++;
        A.AddConstraint(IndexOfConstraint, curel, curdim, V0, myTabPoints->Value(j + k));
        IndexOfConstraint += NgPC1;
      }
      j +=3;
      n0 += Ng3d;
    }

    n0 = NPass + NBeg2d;
    for(pnt = 1; pnt <= myNbP2d; pnt++) {
      IndexOfConstraint = n0;
      for(k = 1; k <= 2; k++) {
        curdim++;
        A.AddConstraint(IndexOfConstraint, curel, curdim, V0, myTabPoints->Value(j + k));
        IndexOfConstraint += NgPC1;
      }
      j +=2;
      n0 += Ng2d;
    }

    /*    if(TypOfConstr == 1) {

    IndexOfConstraint = NTang3d + 1;
    jt = Ntheta * (i - 1);
    for(pnt = 1; pnt <= myNbP3d; pnt++) {
    for(k = 1; k <= 3; k++) {
    A.AddConstraint(IndexOfConstraint, curel, k, myTtheta->Value(jt + k) *  V1, 0.);
    A.AddConstraint(IndexOfConstraint + 1, curel, k, myTtheta->Value(jt + 3 + k) *  V1, 0.);
    }
    IndexOfConstraint += Ng3d;
    jt += 6;
    }

    IndexOfConstraint = NBeg2d + NTang2d + 1;
    for(pnt = 1; pnt <= myNbP2d; pnt++) {
    for(k = 1; k <= 2; k++) {
    A.AddConstraint(IndexOfConstraint, curel, k, myTtheta->Value(jt + k) * V1, 0.);
    }
    IndexOfConstraint += Ng2d;
    jt += 2;
    }

    NTang3d += 2;
    NTang2d += 1;
    } */
    if(TypOfConstr == 1) {

      NPass++;
      n0 = NPass;
      j = 2 * NbDim * (i - 1);
      curdim = 0;
      for(pnt = 1; pnt <= myNbP3d; pnt++) {
        IndexOfConstraint = n0;
        for(k = 1; k <= 3; k++) {
          curdim++;
          A.AddConstraint(IndexOfConstraint, curel, curdim, V1, CBLONG * myTabConstraints->Value(j + k));
          IndexOfConstraint += NgPC1;
        }
        n0 += Ng3d;
        j += 6;
      }

      n0 = NPass + NBeg2d;
      for(pnt = 1; pnt <= myNbP2d; pnt++) {
        IndexOfConstraint = n0;
        for(k = 1; k <= 2; k++) {
          curdim++;
          A.AddConstraint(IndexOfConstraint, curel, curdim, V1, CBLONG * myTabConstraints->Value(j + k));
          IndexOfConstraint += NgPC1;
        }
        n0 += Ng2d;
        j += 4;
      }
    }
    if(TypOfConstr == 2) {

      NPass++;
      n0 = NPass;
      j = 2 * NbDim * (i - 1);
      curdim = 0;
      for(pnt = 1; pnt <= myNbP3d; pnt++) {
        IndexOfConstraint = n0;
        for(k = 1; k <= 3; k++) {
          curdim++;
          A.AddConstraint(IndexOfConstraint, curel, curdim, V1, CBLONG * myTabConstraints->Value(j + k));
          IndexOfConstraint += NgPC1;
        }
        n0 += Ng3d;
        j += 6;
      }

      n0 = NPass + NBeg2d;
      for(pnt = 1; pnt <= myNbP2d; pnt++) {
        IndexOfConstraint = n0;
        for(k = 1; k <= 2; k++) {
          curdim++;
          A.AddConstraint(IndexOfConstraint, curel, curdim, V1, CBLONG * myTabConstraints->Value(j + k));
          IndexOfConstraint += NgPC1;
        }
        n0 += Ng2d;
        j += 4;
      }

      j = 2 * NbDim * (i - 1) + 3;
      jt = Ntheta * (i - 1);
      IndexOfConstraint = NTang3d + 1;
      curdim = 0;
      for(pnt = 1; pnt <= myNbP3d; pnt++) {
        R1 = 0.; R2 = 0.;
        for(k = 1; k <= 3; k++) { 
          R1 += myTabConstraints->Value(j + k) * myTtheta->Value(jt + k);
          R2 += myTabConstraints->Value(j + k) * myTtheta->Value(jt + 3 + k);
        }
        R1 *= CBLONG * CBLONG;
        R2 *= CBLONG * CBLONG;
        for(k = 1; k <= 3; k++) {
          curdim++;
          if(k > 1) R1 = R2 = 0.;
          A.AddConstraint(IndexOfConstraint, curel, curdim, myTfthet->Value(jt + k) * V2, R1);
          A.AddConstraint(IndexOfConstraint + 1, curel, curdim, myTfthet->Value(jt + 3 + k) * V2, R2);
        }
        IndexOfConstraint += Ng3d;
        j += 6;
        jt += 6;
      }

      j--;
      IndexOfConstraint = NBeg2d + NTang2d + 1;
      for(pnt = 1; pnt <= myNbP2d; pnt++) {
        R1 = 0.;
        for(k = 1; k <= 2; k++) { 
          R1 += myTabConstraints->Value(j + k) * myTtheta->Value(jt + k);
        }
        R1 *= CBLONG * CBLONG;
        for(k = 1; k <= 2; k++) {
          curdim++;
          if(k > 1) R1 = 0.;
          A.AddConstraint(IndexOfConstraint, curel, curdim, myTfthet->Value(jt + k) * V2, R1);
        }
        IndexOfConstraint += Ng2d;
        j += 4;
        jt += 2;
      }

      NTang3d += 2;
      NTang2d += 1;
    }

  }

}

Standard_Boolean AppDef_Variational::InitTthetaF(const Standard_Integer ndimen,
                                                 const AppParCurves_Constraint typcon,
                                                 const Standard_Integer begin,
                                                 const Standard_Integer jndex)
{
  if ((ndimen < 2)||(ndimen >3))
    return Standard_False;
  gp_Vec T, V;
  gp_Vec theta1, theta2;
  gp_Vec F;
  Standard_Real XX, XY, YY, XZ, YZ, ZZ;

  if ((typcon == AppParCurves_TangencyPoint)||(typcon == AppParCurves_CurvaturePoint))
  {
    T.SetX(myTabConstraints->Value(jndex));
    T.SetY(myTabConstraints->Value(jndex + 1));
    if (ndimen == 3)
      T.SetZ(myTabConstraints->Value(jndex + 2));
    else
      T.SetZ(0.);
    if (ndimen == 2)
    {
      V.SetX(0.);
      V.SetY(0.);
      V.SetZ(1.);
    }
    if (ndimen == 3)
      if (!NotParallel(T, V))
        return Standard_False;
    theta1 = V ^ T;
    theta1.Normalize();
    myTtheta->SetValue(begin, theta1.X());
    myTtheta->SetValue(begin + 1, theta1.Y());
    if (ndimen == 3)
    {
      theta2 = T ^ theta1;
      theta2.Normalize();
      myTtheta->SetValue(begin + 2, theta1.Z());
      myTtheta->SetValue(begin + 3, theta2.X());
      myTtheta->SetValue(begin + 4, theta2.Y());
      myTtheta->SetValue(begin + 5, theta2.Z());
    }

    // Calculation of myTfthet
    if (typcon == AppParCurves_CurvaturePoint)
    {
      XX = Pow(T.X(), 2);
      XY = T.X() * T.Y();
      YY = Pow(T.Y(), 2);
      if (ndimen == 2) 
      {
        F.SetX(YY * theta1.X() - XY * theta1.Y());
        F.SetY(XX * theta1.Y() - XY * theta1.X());
        myTfthet->SetValue(begin, F.X());
        myTfthet->SetValue(begin + 1, F.Y());
      }
      if (ndimen == 3)
      {
        XZ = T.X() * T.Z();
        YZ = T.Y() * T.Z();
        ZZ = Pow(T.Z(), 2);

        F.SetX((ZZ + YY) * theta1.X() - XY * theta1.Y() - XZ * theta1.Z());
        F.SetY((XX + ZZ) * theta1.Y() - XY * theta1.X() - YZ * theta1.Z());
        F.SetZ((XX + YY) * theta1.Z() - XZ * theta1.X() - YZ * theta1.Y());
        myTfthet->SetValue(begin, F.X());
        myTfthet->SetValue(begin + 1, F.Y());
        myTfthet->SetValue(begin + 2, F.Z());
        F.SetX((ZZ + YY) * theta2.X() - XY * theta2.Y() - XZ * theta2.Z());
        F.SetY((XX + ZZ) * theta2.Y() - XY * theta2.X() - YZ * theta2.Z());
        F.SetZ((XX + YY) * theta2.Z() - XZ * theta2.X() - YZ * theta2.Y());
        myTfthet->SetValue(begin + 3, F.X());
        myTfthet->SetValue(begin + 4, F.Y());
        myTfthet->SetValue(begin + 5, F.Z());
      }
    }
  }
  return Standard_True;
}

