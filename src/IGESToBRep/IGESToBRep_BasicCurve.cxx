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

//=======================================================================
//modified: 
// 21.02.2002 skl
// 21.12.98 rln, gka S4054
//:k5 abv 25 Dec 98: PRO8803 1901: extending method of fixing Multi > Degree
// 28.12.98 dce S3767 New messaging system
//#61 rln 05.01.99
//#60 rln 29.12.98 PRO17015
//:l3 abv 11.01.99: CATIA01.igs: using less values for checking short lines
//%11 pdn 12.01.98 CTS22023 correcting used tolerances
//sln 29.12.2001 OCC90 : Method checkBSplineCurve and varification before creation of bspline curves were added
//=======================================================================

#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESConvGeom.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_BasicCurve.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <Interface_Macros.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>

//:36
// S3767
//=======================================================================
//function : CheckBSplineCurve
//purpose  : Check coincidede knots Check whether knots are in ascending 
//           order and difference between vaues of weights more than 1000. 
//           Send corresponding messages. The function returns Standard_False 
//           if curve can not be created, Standard_True otherwise.
//=======================================================================
static Standard_Boolean checkBSplineCurve(IGESToBRep_BasicCurve*               theCurve,
                                          const Handle(IGESGeom_BSplineCurve)& theBSplineCurve,
                                          TColStd_Array1OfReal&                CKnots,
                                          const TColStd_Array1OfReal&          CWeights)
{
  // check whether difference between vaues of weights more than 1000.
  if(!theBSplineCurve->IsPolynomial()) {
    Standard_Real aMinValue = CWeights.Value(CWeights.Lower());
    Standard_Real aMaxValue = CWeights.Value(CWeights.Lower());
    for(Standard_Integer i = CWeights.Lower()+1; i<= CWeights.Upper(); i++) {
      if(CWeights.Value(i) < aMinValue) aMinValue = CWeights.Value(i);
      if(CWeights.Value(i) > aMaxValue) aMaxValue = CWeights.Value(i);    
    }
    if(aMaxValue - aMinValue > 1000) {
      Message_Msg msg1374("IGES_1374"); // WARNING - Difference between weights is too big.
      theCurve->SendWarning(theBSplineCurve, msg1374);
    }
  }
 
  Standard_Boolean aResult = Standard_True;
  
  //check whether knots are in ascending order.
  for (Standard_Integer i = CKnots.Lower(); i < CKnots.Upper(); i++) 
    if(CKnots.Value (i+1) < CKnots.Value (i)) { 
      Message_Msg msg1373("IGES_1373"); // FAIL - Knots are not in ascending order 
      theCurve->SendFail(theBSplineCurve, msg1373);
      aResult = Standard_False;
    }
  //Fix coincided knots
  if(aResult) ShapeConstruct_Curve::FixKnots(CKnots);
  
  return aResult;

}



//=======================================================================
//function : IGESToBRep_BasicCurve
//purpose  : 
//=======================================================================
IGESToBRep_BasicCurve::IGESToBRep_BasicCurve()
     :IGESToBRep_CurveAndSurface()
{  
  SetModeTransfer(Standard_False);
}


//=======================================================================
//function : IGESToBRep_BasicCurve
//purpose  : 
//=======================================================================
IGESToBRep_BasicCurve::IGESToBRep_BasicCurve
  (const IGESToBRep_CurveAndSurface& CS)
     :IGESToBRep_CurveAndSurface(CS)
{  
}


//=======================================================================
//function : IGESToBRep_BasicCurve
//purpose  : 
//=======================================================================
IGESToBRep_BasicCurve::IGESToBRep_BasicCurve
  (const Standard_Real    eps,
   const Standard_Real    epsCoeff,
   const Standard_Real    epsGeom,
   const Standard_Boolean mode,
   const Standard_Boolean modeapprox,
   const Standard_Boolean optimized)
     :IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, 
				 modeapprox,optimized)
{  
}


//=======================================================================
//function : TransferBasicCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) IGESToBRep_BasicCurve::TransferBasicCurve
       (const Handle(IGESData_IGESEntity)& start)
{ 
  Handle(Geom_Curve) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  try { //:36 by abv 11.12.97: Geom_BSplineCurve fails if somw weights are <=0
    OCC_CATCH_SIGNALS
    //S4054
    if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve))) {
      DeclareAndCast(IGESGeom_BSplineCurve, st126, start);
      res = TransferBSplineCurve(st126);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_Line))) {
      DeclareAndCast(IGESGeom_Line, st110, start);
      res = TransferLine(st110);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc))) {
      DeclareAndCast(IGESGeom_CircularArc, st100, start);
      res = TransferCircularArc(st100);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_ConicArc))) {
      DeclareAndCast(IGESGeom_ConicArc, st104, start);
      res = TransferConicArc(st104);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CopiousData))) {
      DeclareAndCast(IGESGeom_CopiousData, st106, start);
      res = TransferCopiousData(st106);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve))) {
      DeclareAndCast(IGESGeom_SplineCurve, st112, start);
      res = TransferSplineCurve(st112);
    }
    else {
      // AddFail(start, "The IGESEntity is not a basic curve.");
      // This case can not occur
      return res;
    }

  } //:36
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "\n** Exception in IGESToBRep_BasicCurve::TransferBasicCurve : "; 
    anException.Print(cout);
#endif
    (void)anException;
  }
  if (res.IsNull()) {
    // AddFail(start, "The IGESEntity cannot be transfered.");
    // The more specific function have ever add a fail message for this entity
  }
  else
    res->Scale(gp_Pnt(0,0,0),GetUnitFactor());
  return res;
}

//=======================================================================
//function : Transfer2dBasicCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) IGESToBRep_BasicCurve::Transfer2dBasicCurve
       (const Handle(IGESData_IGESEntity)& start)
{ 
  Handle(Geom2d_Curve) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  try { //:h8 abv 15 Jul 98: BUC60291 43693: Bspline Multiplicity > Degree+1 -> exception
    OCC_CATCH_SIGNALS
    
    //S4054
    if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve))) {
      DeclareAndCast(IGESGeom_BSplineCurve, st126, start);
      res = Transfer2dBSplineCurve(st126);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_Line))) {
      DeclareAndCast(IGESGeom_Line, st110, start);
      res = Transfer2dLine(st110);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc))) {
      DeclareAndCast(IGESGeom_CircularArc, st100, start);
      res = Transfer2dCircularArc(st100);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_ConicArc))) {
      DeclareAndCast(IGESGeom_ConicArc, st104, start);
      res = Transfer2dConicArc(st104);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_CopiousData))) {
      DeclareAndCast(IGESGeom_CopiousData, st106, start);
      res = Transfer2dCopiousData(st106);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve))) {
      DeclareAndCast(IGESGeom_SplineCurve, st112, start);
      res = Transfer2dSplineCurve(st112);
    }
     else {
      // AddFail(start, "The IGESEntity is not a basic curve.");
      // This case can not occur
      return res;
    }
  } //:h8
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "\n** Exception in IGESToBRep_BasicCurve::Transfer2dBasicCurve : "; 
    anException.Print(cout);
#endif
    (void)anException;
  }
  return res;
}



//=======================================================================
//function : TransferConicArc
//purpose  : 
//=======================================================================
//
// A,B,C,D,E,F are the coefficients recorded in IGES. a,b,c,d,e,f are used to 
// simplify the equations of convertion. They are already used in Euclid. 

Handle(Geom_Curve) IGESToBRep_BasicCurve::TransferConicArc
       (const Handle(IGESGeom_ConicArc)& st)
{ 
  Handle(Geom_Curve) res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }
  // If the Conic is closed, the start and end points will be ignored.
  if (!st->ComputedFormNumber()) {
    Message_Msg msg1155("IGES_1155");
    SendFail(st, msg1155);
//    AddFail(st, "Coefficients do not define correctly a conic.");
    return res;
  }
  
  // Conic = ax2+bxy+cy2+dx+ey+f=0 in the plane z=ZT.
  Standard_Real a,b,c,d,e,f,ZT; 
  st->Equation(a, b, c, d, e, f);

  ZT = st->ZPlane();

  gp_Pnt        center, startPoint, endPoint;
  gp_Dir        mainAxis ,normAxis;
  Standard_Real minorRadius, majorRadius;
  
  if (!GetModeTransfer() && st->HasTransf()) {

    st->TransformedDefinition(center ,mainAxis, minorRadius, majorRadius);
    normAxis = st->TransformedAxis();                            

    startPoint = st->TransformedStartPoint();
    endPoint   = st->TransformedEndPoint();
  }
  else {
    st->Definition(center ,mainAxis, minorRadius, majorRadius);
    normAxis = st->Axis();                            

    startPoint.SetCoord(st->StartPoint().X(), st->StartPoint().Y(), ZT);
    endPoint.SetCoord  (st->EndPoint().X()  , st->EndPoint().Y()  , ZT);
  }
  gp_Ax2 frame(center, normAxis, mainAxis);
  Standard_Real t1 =0.0, t2 =0.0;
  if (st->IsFromEllipse()) {

    //#60 rln 29.12.98 PRO17015 reading back face#67: ellipse with big radii produces
    //small coefficients
    //The dimensions should be also obliged:
    //[a]=[b]=[c]=L^-2
    //if ( (Abs(a-c) <= GetEpsGeom()) && (Abs(b) < GetEpsCoeff()))
    Standard_Real eps2 = Precision::PConfusion() * Precision::PConfusion();
    if ( (Abs(a-c) <= eps2) && (Abs(b) < eps2)) {
    
//                          =================
//                          ==  Circle 3D  ==
//                          =================

      res = new Geom_Circle(frame, minorRadius);
      
      if (!st->IsClosed()) {

	gp_Circ circ(frame, minorRadius);

        t1 = ElCLib::Parameter(circ, startPoint);
        t2 = ElCLib::Parameter(circ, endPoint);
	if (t1 > t2 && (t1 - t2) > Precision::Confusion()) t2 += 2.*M_PI;
	if (Abs(t1 - t2) <= Precision::Confusion())  { // t1 = t2
	  Message_Msg msg1160("IGES_1160");
	  SendWarning(st, msg1160);      
	}
	else
	  res = new Geom_TrimmedCurve(res, t1, t2);
      }
      return res;
    }
    else { 
      // This is a no circular ellipse which will be computed with 
      // the hyperbola at the end of this member.
    }
  }
  else if (st->IsFromParabola()) {
    
    //                         ===================
    //                         ==  Parabola 3D  ==
    //                         ===================
    
    Standard_Real focal = minorRadius;
    // PTV 26.03.2002
    focal = focal/2.;
    gp_Parab parab(frame, focal);
    
    res = new Geom_Parabola(frame, focal);
    
    t1 = ElCLib::Parameter(parab, startPoint);
    t2 = ElCLib::Parameter(parab, endPoint);
    if (Abs(t1 - t2) <= Precision::Confusion()) { // t1 = t2
      Message_Msg msg1160("IGES_1160");
      SendWarning(st, msg1160);      
      //AddWarning(st, "The trim of the parabola is not correct.");
    }
    else 
      // if t1 > t2, the course of the curve is going to be reversed.
      res = new Geom_TrimmedCurve(res, t1, t2);
    
    return res;
  }
  
  // Same computing for the ellipse and the hyperbola.
  
  //              =============================================
  //              ==  Hyperbola or a no circular Ellipse 3D. ==
  //              =============================================
  
  
  if (st->IsFromEllipse()) {
    res = new Geom_Ellipse(frame, majorRadius, minorRadius);
    
    if (!st->IsClosed()) {
      gp_Elips  elips(frame, majorRadius, minorRadius);
      
      t1 = ElCLib::Parameter(elips, startPoint);
      t2 = ElCLib::Parameter(elips, endPoint);
      if (t2 < t1 && (t1 -t2) > Precision::Confusion()) t2 += 2.*M_PI;
      if (Abs(t1 - t2) <= Precision::Confusion()) { // t1 = t2   
	Message_Msg msg1160("IGES_1160");
	SendWarning(st, msg1160);  
	//AddWarning(st, "The trim of the ellipse is not correct, the result will be a ellipse.");
      }
      else
	res = new Geom_TrimmedCurve(res, t1, t2);
    }
  }
  else {
      
    gp_Hypr hpr(frame, majorRadius, minorRadius);
    
    t1 = ElCLib::Parameter(hpr, startPoint);
    t2 = ElCLib::Parameter(hpr, endPoint);
      
    res = new Geom_Hyperbola(frame, majorRadius, minorRadius);
    
    //pdn taking PConfusion for parameters.
    if (Abs(t1 - t2) <= Precision::PConfusion()) { // t1 = t2   
	Message_Msg msg1160("IGES_1160");
	SendWarning(st, msg1160);  
      }
    else if (t1 > t2)
      res = new Geom_TrimmedCurve(res, t2, t1); // inversion des parametres.
    else
      res = new Geom_TrimmedCurve(res, t1, t2); 
  }

  return res;
}


//=======================================================================
//function : Transfer2dConicArc
//purpose  : Transfer 2d of a ConicArc to be used as a boundary of a Face
//=======================================================================

Handle(Geom2d_Curve) IGESToBRep_BasicCurve::Transfer2dConicArc
       (const Handle(IGESGeom_ConicArc)& st)
{ 
  Handle(Geom2d_Curve) res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

 if (!st->ComputedFormNumber()) {
    Message_Msg msg1155("IGES_1155");
    SendFail(st, msg1155);
    return res;
  }

  Standard_Real a,b,c,d,e,f;                          
  // Conic = ax2+bxy+cy2+dx+ey+f=0.
  st->Equation(a, b, c, d, e, f);

  gp_Pnt        center3d;
  gp_Dir        mainAxis3d;
  gp_Pnt2d      startPoint, endPoint;
  Standard_Real minorRadius, majorRadius;
  
  SetEpsilon(1.E-03);
  if (!st->TransformedAxis().IsParallel /*#45 rln 23.11.98 IsEqual*/(st->Axis(), GetEpsilon())) {
    SetModeTransfer(Standard_True); 
    // Only not to use Trsf.
    // the normal axis is not parallel with Z axis.
    SendWarning(st, "The Trsf is not compatible with a transfer2d, it will not applied.");
  } 

  if (!GetModeTransfer() && st->HasTransf()) {

    st->TransformedDefinition(center3d, 
			      mainAxis3d, 
			      minorRadius, 
			      majorRadius);
    startPoint.SetCoord(st->TransformedStartPoint().X(),
			st->TransformedStartPoint().Y());
    endPoint.SetCoord(st->TransformedEndPoint().X(),
		      st->TransformedEndPoint().Y());
  }
  else {
    st->Definition(center3d, mainAxis3d, minorRadius, majorRadius);
    startPoint = st->StartPoint();
    endPoint   = st->EndPoint();
  }
  
  gp_Pnt2d center(center3d.X(), center3d.Y());
  gp_Dir2d mainAxis(mainAxis3d.X(), mainAxis3d.Y());
  gp_Ax2d  frame(center, mainAxis);
  Standard_Real t1 =0.0, t2=0.0;
  if (st->IsFromEllipse()) {

    //#60 rln 29.12.98 PRO17015
    //if ( (Abs(a-c) <= GetEpsGeom()) && (Abs(b) < GetEpsCoeff()))
    Standard_Real eps2 = Precision::PConfusion() * Precision::PConfusion();
    if ( (Abs(a-c) <= eps2) && (Abs(b) < eps2)) {

      //                          =================
      //                          ==  Circle 2D  ==
      //                          =================
      
      res = new Geom2d_Circle(frame, minorRadius);
      //#45 rln 23.11.98
      if (st->TransformedAxis().IsOpposite (st->Axis(), GetEpsilon()))
	res->Reverse();
      
      if (!st->IsClosed()) {

	gp_Circ2d circ = Handle(Geom2d_Circle)::DownCast(res)->Circ2d();//#45 rln (frame, minorRadius);
        
        t1 = ElCLib::Parameter(circ, startPoint);
        t2 = ElCLib::Parameter(circ, endPoint);
	
	if (t2 < t1 && (t1 -t2) > Precision::PConfusion()) t2 += 2.*M_PI;
	if (Abs(t1 - t2) <= Precision::PConfusion()) { // t1 = t2
	  Message_Msg msg1160("IGES_1160");
	  SendWarning(st, msg1160); 
	}
	else 
	  res = new Geom2d_TrimmedCurve(res, t1, t2);
      }
      return res; 
    }
    else { 
      // This is a no circular ellipse, it will be computed with the hyperbola.
    }
  }  
  else if (st->IsFromParabola()) {
    
    //                         ===================
    //                         ==  Parabola 2D  ==
    //                         ===================
    
    Standard_Real focal = minorRadius;
    // PTV 26.03.2002
    focal = focal/2.;
    res = new Geom2d_Parabola(frame, focal);
    //#45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite (st->Axis(), GetEpsilon()))
      res->Reverse();
    
    gp_Parab2d parab = Handle(Geom2d_Parabola)::DownCast(res)->Parab2d();//#45 rln (frame, focal);
    
    t1  = ElCLib::Parameter(parab, startPoint);
    t2  = ElCLib::Parameter(parab, endPoint);
    if (Abs(t1 - t2) <= Precision::PConfusion())  { // t1 = t2   
	Message_Msg msg1160("IGES_1160");
	SendWarning(st, msg1160);  
      }  
    else if (t1 > t2)
      res = new Geom2d_TrimmedCurve(res, t2, t1); // inversion des parametres.
    else 
      res = new Geom2d_TrimmedCurve(res, t1, t2);
    return res;                      
  }
    
  /* Same computing for the ellipse2d and the hyperbola2d. */
  
  //              ============================================
  //              ==  Hyperbola or a no circular Ellipse 2D ==
  //              ============================================
    
  if (st->IsFromEllipse()) {

    res = new Geom2d_Ellipse(frame, majorRadius, minorRadius);
    //#45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite (st->Axis(), GetEpsilon()))
      res->Reverse();

    if (!st->IsClosed()) {

      gp_Elips2d elips = Handle(Geom2d_Ellipse)::DownCast(res)->Elips2d();//#45 rln (frame, majorRadius, minorRadius);
      
      t1  = ElCLib::Parameter(elips, startPoint);
      t2  = ElCLib::Parameter(elips, endPoint);
      if (t2 < t1 && (t1 - t2) > Precision::PConfusion()) t2 += 2.*M_PI;
      if (Abs(t1 - t2) <= Precision::PConfusion())  { // t1 = t2   
	Message_Msg msg1160("IGES_1160");
	SendWarning(st, msg1160);  
      }
      else   
	res = new Geom2d_TrimmedCurve(res, t1, t2);
    }
  }
  else {

    res = new Geom2d_Hyperbola(frame, majorRadius, minorRadius);
    //#45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite (st->Axis(), GetEpsilon()))
      res->Reverse();

    gp_Hypr2d hpr = Handle(Geom2d_Hyperbola)::DownCast(res)->Hypr2d();//#45 rln (frame, majorRadius, minorRadius);
   
    t1 = ElCLib::Parameter(hpr, startPoint);
    t2 = ElCLib::Parameter(hpr, endPoint);
    
    if (Abs(t1 - t2) <= Precision::PConfusion())   { // t1 = t2   
	Message_Msg msg1160("IGES_1160");
	SendWarning(st, msg1160);  
      }
    else if (t1 > t2)
      res = new Geom2d_TrimmedCurve(res, t2, t1); // inversion des parametres.
    else   
      res = new Geom2d_TrimmedCurve(res, t1, t2);
  }

  return res;
}


//=======================================================================
//function : TransferCircularArc
//purpose  : 
//=======================================================================

Handle(Geom_Curve) IGESToBRep_BasicCurve::TransferCircularArc
       (const Handle(IGESGeom_CircularArc)& st)
{ 
  Handle(Geom_Curve) res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st,msg1005);
    return res;
  }

  gp_Dir tNormAxis, tMainAxis;
  gp_Ax2 frame;
  gp_Pnt startPoint, endPoint;

  if (!GetModeTransfer() && st->HasTransf()) {

    tNormAxis = st->TransformedAxis();

    gp_GTrsf loc = st->Location();
    loc.SetTranslationPart (gp_XYZ(0.,0.,0.));
    gp_XYZ mainAxis(1., 0., 0.); 
    loc.Transforms(mainAxis);
    tMainAxis = gp_Dir(mainAxis);

    startPoint = st->TransformedStartPoint();
    endPoint   = st->TransformedEndPoint();
    
    frame = gp_Ax2(st->TransformedCenter(), tNormAxis, tMainAxis);
  }
  else {
    tNormAxis  = st->Axis();
    tMainAxis.SetCoord(1., 0., 0.);

    Standard_Real ZT    = st->ZPlane();
    startPoint.SetCoord(st->StartPoint().X(), st->StartPoint().Y(), ZT);
    endPoint.SetCoord  (st->EndPoint().X()  , st->EndPoint().Y()  , ZT);
    gp_Pnt centerPoint (st->Center().X()    , st->Center().Y()    , ZT);

    frame = gp_Ax2(centerPoint, tNormAxis, tMainAxis);
  }
  
  res = new Geom_Circle(frame, st->Radius());
  
  gp_Circ   circ(frame, st->Radius());
  
  Standard_Real t1 =0.0, t2 =0.0;
 
  t1 = ElCLib::Parameter(circ, startPoint);
  t2 = ElCLib::Parameter(circ, endPoint);

  if ( st->IsClosed() && t1>=GetEpsGeom()) t2 = t1 + 2.*M_PI;
  if (!st->IsClosed() && fabs(t1 - t2) <=Precision::PConfusion()) {    
    // micro-arc
    // cky 27 Aout 1996 : t2-t1 vaut distance(start,end)/rayon
    t2 = t1 + startPoint.Distance(endPoint)/st->Radius();
  }
  if (!st->IsClosed() || t1>=GetEpsGeom()) {
    if (t2 < t1) t2 += 2.*M_PI;
    res = new Geom_TrimmedCurve(res, t1, t2);
  }

  return res;      
}

//=======================================================================
//function : Transfer2dCircularArc
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) IGESToBRep_BasicCurve::Transfer2dCircularArc
       (const Handle(IGESGeom_CircularArc)& st) 
{ 
  Handle(Geom2d_Curve) res;	
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st,msg1005);
    return res;
  }

  gp_XYZ center(st->Center().X(), st->Center().Y(), 0.);
  gp_XYZ mainAxis(1., 0., 0.); 

  SetEpsilon(1.E-03);
  if (!st->TransformedAxis().IsParallel /*#45 rln 23.11.98 IsEqual*/(st->Axis(), GetEpsilon())) {
    SetModeTransfer(Standard_True);          // Only not to use Trsf
    Message_Msg msg1165("IGES_1165");
    SendWarning(st, msg1165); //"The Trsf is not compatible with a transfer2d, it will not applied."
  }

  if (!GetModeTransfer() && st->HasTransf()) {
    gp_GTrsf loc = st->Location();
    loc.Transforms(center);
    loc.SetTranslationPart (gp_XYZ(0.,0.,0.));
    loc.Transforms(mainAxis);
  }
  gp_Pnt2d tCenter(center.X(), center.Y());
  gp_Dir2d tMainAxis(mainAxis.X(), mainAxis.Y());
  gp_Ax2d  frame(tCenter, tMainAxis);
  
  res = new Geom2d_Circle(frame, st->Radius());
  
  gp_Pnt2d startPoint, endPoint;
   if (!GetModeTransfer() && st->HasTransf()) {
    startPoint.SetCoord(st->TransformedStartPoint().X(),
			st->TransformedStartPoint().Y());
    endPoint.SetCoord(st->TransformedEndPoint().X(),
		      st->TransformedEndPoint().Y());
    //#45 rln 23.11.98
    if (st->TransformedAxis().IsOpposite (st->Axis(), GetEpsilon()))
      res->Reverse();
  }
  else {
    startPoint = st->StartPoint();
    endPoint   = st->EndPoint();
  }

  gp_Circ2d circ = Handle(Geom2d_Circle)::DownCast(res)->Circ2d();//#45 rln (frame, st->Radius());
  
  Standard_Real t1 =0.0, t2 =0.0;
  
  t1 = ElCLib::Parameter(circ, startPoint);
  t2 = ElCLib::Parameter(circ, endPoint);
    
  if ( st->IsClosed() && t1>=GetEpsGeom()) t2 = t1 + 2.*M_PI;
  if (!st->IsClosed() && fabs(t1 -t2) <= Precision::PConfusion()) { 
    // micro-arc
    // cky 27 Aout 1996 : t2-t1 vaut distance(start,end)/rayon
    t2 = t1 + startPoint.Distance(endPoint)/st->Radius();
  }
  if (!st->IsClosed() || t1>= GetEpsGeom()) {
    if (t2 < t1) t2 += 2.*M_PI;
    res     = new Geom2d_TrimmedCurve(res, t1, t2);
  }
  return res;
}



//=======================================================================
//function : TransferSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) IGESToBRep_BasicCurve::TransferSplineCurve
       (const Handle(IGESGeom_SplineCurve)& st)
{ 
  Handle(Geom_BSplineCurve) resconv;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st,msg1005);
    return resconv;
  }
  
  Standard_Real epscoef = GetEpsCoeff();
  Standard_Real epsgeom = GetEpsGeom();

  Standard_Integer result = IGESConvGeom::SplineCurveFromIGES(st, epscoef, epsgeom, resconv);
  
  switch (result) {
  case 5 : {
    Message_Msg msg246("XSTEP_246");
    SendFail(st, msg246);
    // less than on segment (no result produced)
    return resconv;
  }
  case 4 : {
    Message_Msg msg1170("IGES_1170");
    SendFail(st, msg1170);
    // Polynomial equation is not correct ( no result produced
    return resconv;}
  case 3 :{
    Message_Msg msg1175("IGES_1175");
    SendFail(st, msg1175);
    // Error during creation of control points ( no result produced)
    return resconv;}
  case 2 :{
    Message_Msg msg1180("IGES_1180");
   SendFail(st, msg1180);
    //SplineType not processed (allowed : max 3) (no result produced)
    return resconv;}
  default :
    break;
  }

  //  Checking C2 and C1 continuity :
  //  ===============================
  IGESConvGeom::IncreaseCurveContinuity (resconv, Min(Precision::Confusion(),epsgeom), GetContinuity());
  return resconv;
}



//=======================================================================
//function : Transfer2dSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) IGESToBRep_BasicCurve::Transfer2dSplineCurve
       (const Handle(IGESGeom_SplineCurve)& st)
{ 
  Handle(Geom2d_BSplineCurve) res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  // 3d transfer First 
  // =================
  // The same Presision as in BSpline 2d is used
  Standard_Real epsGeom = GetEpsGeom();
  SetEpsGeom(Precision::PConfusion());
  Handle(Geom_BSplineCurve) res3d = TransferSplineCurve(st);
  SetEpsGeom(epsGeom);
  if (res3d.IsNull()) 
    return res;        // The transfer was not over the top.

 
  // 2d 
  // ==
  Standard_Integer nbPoles = res3d->NbPoles();
  Standard_Integer nbKnots = res3d->NbKnots();
 
  TColgp_Array1OfPnt2d    bspoles2d(1, nbPoles);
  TColStd_Array1OfReal    knots(1, nbKnots);
  TColStd_Array1OfInteger multi(1, nbKnots);

  res3d->Knots(knots);
  res3d->Multiplicities(multi);

  for (Standard_Integer i = bspoles2d.Lower(); i <= bspoles2d.Upper(); i++) 
    bspoles2d.SetValue(i, gp_Pnt2d(res3d->Pole(i).X(), res3d->Pole(i).Y()));
  
  res = new Geom2d_BSplineCurve (bspoles2d, knots, multi, res3d->Degree());
  return res;
}

//=======================================================================
//function : TransferBSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) IGESToBRep_BasicCurve::TransferBSplineCurve
       (const Handle(IGESGeom_BSplineCurve)&  start)
     
{ 
  Handle(Geom_BSplineCurve)  BSplineRes;
  Handle(Geom_Curve) res;
  
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start,msg1005);
    return res;
  }

  Standard_Integer  Degree = start->Degree();

  if (Degree<=0 || Degree>Geom_BSplineCurve::MaxDegree()) { 
    Message_Msg msg1190("IGES_1190");
    SendFail(start, msg1190);
    // Improper degree either lower or equal to 0 or upper to MaxDegree
      return res;
  }
  
  
  //  Filling poles array :
  //  =====================
  
  Standard_Integer  NbPoles = start->NbPoles();
  Standard_Integer  newNbPoles = NbPoles;
  
  if (NbPoles<2) {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);
    // Transfer aborted for a BSpline Curve : Number of poles lower than 2
    return res;
  }

  TColgp_Array1OfPnt   Pole(1,NbPoles);
  Standard_Integer     PoleIndex = Pole.Lower();
  Standard_Integer     i; //szv#4:S4163:12Mar99 j unused

  if (!GetModeTransfer() && start->HasTransf()) 
    for (i=0; i<=start->UpperIndex(); i++) 
      Pole.SetValue(PoleIndex++, start->TransformedPole(i));
  else
    for (i=0; i<=start->UpperIndex(); i++) 
      Pole.SetValue(PoleIndex++, start->Pole(i));
  
  
  //  Filling knots & multiplicities arraies :
  //  ========================================
  
  Standard_Integer          NbKnots = start->NbKnots();
  TColStd_Array1OfReal      TempKnot(1,NbKnots);
  TColStd_Array1OfInteger   TempMult(1,NbKnots); 
  TempMult.Init(1);
  Standard_Integer          KnotIndex = TempKnot.Lower();

  TempKnot.SetValue(KnotIndex, start->Knot(-Degree));
  
  //  If several identical IGES knots are encountered, corresponding 
  //  multiplicity is increased
  //  ==============================================================

  for (i=1-Degree; i<NbKnots-Degree; i++) {

    Standard_Real Knot1 = start->Knot(i);
    Standard_Real Knot2 = start->Knot(i-1);
//    Standard_Real ek    =  Epsilon(Knot1);

    if (Abs(Knot1 - Knot2) <= Epsilon(Knot1))
      TempMult.SetValue(KnotIndex, TempMult.Value(KnotIndex)+1);
    else 
      TempKnot.SetValue(++KnotIndex, Knot1);
  }
  

  //  Final knots & multiplicities arraies are dimensionned so as to be fully 
  //  filled
  //  =======================================================================
  
  TColStd_Array1OfReal        Knot(1,KnotIndex);
  TColStd_Array1OfInteger     Mult(1,KnotIndex);

  Standard_Integer SumOfMult=0;
  
  TColStd_SequenceOfInteger SeqIndex;
  Standard_Integer DelIndex;
  Standard_Integer OldSumOfMult = 0;
  for (i=1; i <= KnotIndex; i++) { //:k5 abv 25 Dec 98: cycle modified
    Standard_Integer aMult = TempMult.Value(i);
    Standard_Integer maxMult = ( i==1 || i == KnotIndex ? Degree + 1 : Degree );
    if (aMult > maxMult) {
    Message_Msg msg1200("IGES_1200");//#61 rln 05.01.99
    const Standard_CString vide ("");
    msg1200.Arg(vide);
    msg1200.Arg(vide);
    msg1200.Arg(vide);
    SendWarning(start, msg1200);//Multiplicity > Degree (or Degree+1 at end); corrected
      for ( DelIndex = OldSumOfMult + 1; aMult > maxMult; DelIndex++, aMult-- ) {
	newNbPoles--;
	SeqIndex.Append(DelIndex);
      }
    }
    OldSumOfMult += TempMult.Value(i); 
    Knot.SetValue(i, TempKnot.Value(i));
    Mult.SetValue(i, aMult);
    SumOfMult += aMult;
  }

  // Mise a jour du tableau des poles lors de la correction de la multiplicite
  TColgp_Array1OfPnt Poles(1,newNbPoles);
  TColStd_SequenceOfInteger PoleInd;

  if ( newNbPoles < NbPoles) {
    for (i=1; i<=NbPoles; i++) PoleInd.Append(i);
    Standard_Integer Offset = 0;
    for (Standard_Integer itab = 1; itab <= SeqIndex.Length(); itab++) {
      DelIndex = SeqIndex.Value(itab) - Offset;
      PoleInd.Remove(DelIndex);
      Offset++;
    }
    Standard_Integer nbseq = PoleInd.Length();
    if ( nbseq == newNbPoles) {
      Standard_Integer indj = 1;
      for ( i=1; i<= newNbPoles; i++) {
	Poles.SetValue(i, Pole.Value(PoleInd.Value(indj++)));
      }
    }
  }

  else {
    for ( i=1; i<= newNbPoles; i++) {
      Poles.SetValue (i, Pole.Value(i));
    }
  }
    

  if (! (SumOfMult  ==  newNbPoles + Degree + 1)) {
    Message_Msg msg1210("IGES_1210");
    const Standard_CString vide ("");
    msg1210.Arg(vide);
    msg1210.Arg(vide);
    SendWarning(start, msg1210);
    //Sum of multiplicities not equal to the sum : Count of poles + Degree + 1
  }
  
  //  Output BSpline curve with the array of pole weights if any :
  //  ============================================================
  
  TColStd_Array1OfReal Weight(1,newNbPoles);

  if (start->IsPolynomial()) {
//:5    BSplineC = new Geom_BSplineCurve(Poles, Knot, Mult, Degree);  
  }
  else {
    TColStd_Array1OfReal  PoleWeight(1,NbPoles);
    Standard_Boolean      polynomial               = Standard_True;
    Standard_Real         WeightReference          = start->Weight(0);
    Standard_Integer      WeightIndex              = PoleWeight.Lower();
    
    for (i=0; i <= start->UpperIndex(); i++) {
      polynomial = Abs(start->Weight(i) - WeightReference) <= 
	Epsilon(WeightReference) && polynomial;
      //:39 by abv 15.12.97
      Standard_Real weight = start->Weight(i);
      if ( weight < Precision::PConfusion() ) {
	Message_Msg msg1215("IGES_1215");
	SendFail(start, msg1215);
        // Some weights are not positive
        return res;
      }
      PoleWeight.SetValue(WeightIndex++, weight);
//:39      PoleWeight.SetValue(WeightIndex++, start->Weight(i));
    }
    if (polynomial) {
      Message_Msg msg1220("IGES_1220");
      msg1220.Arg("curve");
      SendWarning(start, msg1220);
      // Rational curve is polynomial
    }
    // Mise a jour du tableau des Weight lors de la correction de la multiplicite
    if ( newNbPoles < NbPoles) {
      Standard_Integer indj = 1;
      for ( i=1; i<= newNbPoles; i++) {
	Weight.SetValue(i, PoleWeight.Value(PoleInd.Value(indj++)));
      }
    }
    else {
      for ( i=1; i<= newNbPoles; i++) {
	Weight.SetValue (i, PoleWeight.Value(i));
      }
    }
//:5    BSplineC = new Geom_BSplineCurve(Poles, Weight, Knot, Mult, Degree); 
  }
  
  //sln 29.12.2001 OCC90 : If curve can not be created do nothing
  if(!checkBSplineCurve(this, start, Knot, Weight)) return BSplineRes;

  {
    try {
      OCC_CATCH_SIGNALS
      if (start->IsPolynomial()) 
        BSplineRes = new Geom_BSplineCurve(Poles, Knot, Mult, Degree);  
      else 
        BSplineRes = new Geom_BSplineCurve(Poles, Weight, Knot, Mult, Degree); 
    }
    catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "\n** Exception in IGESToBRep_BasicCurve::TransferBSplineCurve during creation of Geom_BSplineCurve : "; 
      anException.Print(cout);
#endif
      (void)anException;
    }
  }
  
  Standard_Real First = BSplineRes->FirstParameter();
  Standard_Real Last  = BSplineRes->LastParameter();
  Standard_Real Udeb = start->UMin();
  Standard_Real Ufin = start->UMax();
  //%11 pdn 12.01.98 CTS22023 
  //if ( (Udeb-First) > Precision::PConfusion() || (Last-Ufin) > Precision::PConfusion() )
  //  BSplineRes->Segment(Udeb, Ufin);
  //res = BSplineRes;

//  IGESConvGeom::IncreaseCurveContinuity (BSplineRes,Min(Precision::Confusion(),GetEpsGeom()), GetContinuity());
  
  // skl 21.02.2002 (exception in OCC133 and for file
  //                 "/dn04/OS/USINOR/UIdev/src/IsoLim/dat/igs/ps1002-v5.igs")
  Handle(Geom_BSplineCurve) BSplineRes2 = BSplineRes;
  if (((Udeb-First)>-Precision::PConfusion() &&
       (Last-Ufin)>-Precision::PConfusion()) && Udeb<=Ufin ) {
    try {
      OCC_CATCH_SIGNALS
      if (Abs(Ufin-Udeb) > Precision::PConfusion())
        BSplineRes->Segment(Udeb, Ufin);
      res = BSplineRes;
    }
    catch (Standard_Failure) {
      Handle(Geom_TrimmedCurve) gtc = new Geom_TrimmedCurve(BSplineRes2,Udeb,Ufin);
      res = gtc;
    }
  }
  else
    res = BSplineRes;

  return res;
}



//=======================================================================
//function : Transfer2dBSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)  IGESToBRep_BasicCurve::Transfer2dBSplineCurve
       (const Handle(IGESGeom_BSplineCurve)& start)
{ 
  Handle(Geom2d_Curve)  res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  Handle(Geom2d_BSplineCurve)  BSplineC;
  Handle(Geom_BSplineCurve)  Bspline;
  Standard_Boolean IsTrimmed = Standard_False;
  Standard_Real Deb=0., Fin=0.;

  //  3d transfer first :
  //  ===================
//  Standard_Real epsGeom = GetEpsGeom();
//  SetEpsGeom(Precision::PConfusion());
  Handle(Geom_Curve)  res3d = TransferBSplineCurve(start);
//  SetEpsGeom(epsGeom);
  if (res3d.IsNull()) {
    return res;
  }
  
             
  if (res3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    DeclareAndCast(Geom_TrimmedCurve, TrimC, res3d);
    Handle(Geom_Curve) BasicCurve = TrimC->BasisCurve();
    Deb = TrimC->FirstParameter();
    Fin = TrimC->LastParameter();
    IsTrimmed = Standard_True;
    if (BasicCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
      DeclareAndCast(Geom_BSplineCurve, BSpline, BasicCurve);
      Bspline = BSpline;
    }
    else {
      return res;
    }
  }
  else if (res3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    DeclareAndCast(Geom_BSplineCurve, BSpline, res3d);
    Bspline = BSpline;
  }


  //  Creating 2d poles :
  //  ===================

  Standard_Integer       NbPoles  =  Bspline->NbPoles();
  TColgp_Array1OfPnt2d   Pole(1,NbPoles);

  for (Standard_Integer i=1; i<=NbPoles; i++){
    gp_Pnt2d aPole2d(Bspline->Pole(i).X(),Bspline->Pole(i).Y());
    Pole.SetValue(i,aPole2d);
  }

  //  Knots and multiplicities are the same :
  //  =======================================

  Standard_Integer  NbKnots = Bspline->NbKnots();

  TColStd_Array1OfReal Knot(1,NbKnots);
  Bspline->Knots(Knot);

  TColStd_Array1OfInteger Mult(1,NbKnots);
  Bspline->Multiplicities(Mult);

  Standard_Integer  Degree = Bspline->Degree();

  if (Bspline->IsRational()) {
    TColStd_Array1OfReal  Weight(1,NbPoles);
    Bspline->Weights(Weight);
    BSplineC = new Geom2d_BSplineCurve(Pole, Weight, Knot, Mult, Degree);
  }
  else BSplineC = new Geom2d_BSplineCurve(Pole, Knot, Mult, Degree);
  
  res = BSplineC;

  // cas ou la Bspline est trimmee.
  if (IsTrimmed) {
    Handle(Geom2d_TrimmedCurve) TC = new Geom2d_TrimmedCurve
      (BSplineC, Deb, Fin, Standard_True);
    res = TC;
  }
  
  return res;
}



//=======================================================================
//function : TransferLine
//purpose  : 
//=======================================================================

Handle(Geom_Curve)  IGESToBRep_BasicCurve::TransferLine
       (const Handle(IGESGeom_Line)& start)
{ 
  Handle(Geom_Curve)  res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  gp_Pnt Ps,Pe;
  
  if (!GetModeTransfer() && start->HasTransf()) { 
    Ps = start->TransformedStartPoint();
    Pe = start->TransformedEndPoint();
  }
  else {
    Ps = start->StartPoint();
    Pe = start->EndPoint();
  }

  // modif du 15/10/97  : test moins severe
  // beaucoup de points confondus a GetEpsGeom()*GetUnitFactor()
  if (!Ps.IsEqual(Pe,Precision::Confusion())) { //:l3 abv 11 Jan 99: GetEpsGeom()*GetUnitFactor()/10.)) {
    gp_Lin line(Ps, gp_Dir(gp_Vec(Ps,Pe)));
    Standard_Real t1 = ElCLib::Parameter(line, Ps);
    Standard_Real t2 = ElCLib::Parameter(line, Pe);
    Handle(Geom_Line)  Gline = new Geom_Line(line);
    if (Precision::IsNegativeInfinite(t1)) t1 = -Precision::Infinite();
    if (Precision::IsPositiveInfinite(t2)) t2 = Precision::Infinite();
    res = new Geom_TrimmedCurve(Gline, t1, t2);
  }
  else {
    Message_Msg msg1225("IGES_1225");
    SendFail(start, msg1225);
    // StartPoint and EndPoint of the line are the same Point
  }

  return res;
}



//=======================================================================
//function : Transfer2dLine
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)  IGESToBRep_BasicCurve::Transfer2dLine
       (const Handle(IGESGeom_Line)& start)
{ 
  Handle(Geom2d_Curve) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  gp_Pnt2d beg, end;
  
  if (!GetModeTransfer() && start->HasTransf()) {
    beg.SetCoord(start->TransformedStartPoint().X(),
		 start->TransformedStartPoint().Y());
    end.SetCoord(start->TransformedEndPoint().X(),
		 start->TransformedEndPoint().Y());
  }
  else {
    beg.SetCoord(start->StartPoint().X(),
		 start->StartPoint().Y());
    end.SetCoord(start->EndPoint().X(), 
		 start->EndPoint().Y());
  }

  if (!beg.IsEqual(end,Precision::PConfusion())) { //:l3 abv 11 Jan 99: GetEpsCoeff())) {
    gp_Lin2d line2d(beg, gp_Dir2d(gp_Vec2d(beg,end)));
    Standard_Real t1 = ElCLib::Parameter(line2d, beg);
    Standard_Real t2 = ElCLib::Parameter(line2d, end);
    Handle(Geom2d_Line) Gline2d = new Geom2d_Line(line2d);
    if (Precision::IsNegativeInfinite(t1)) t1 = -Precision::Infinite();
    if (Precision::IsPositiveInfinite(t2)) t2 = Precision::Infinite();
    res = new Geom2d_TrimmedCurve(Gline2d, t1, t2);
  }
  //added by rln 18/12/97 CSR# CTS18544 entity 25168 and 31273
  //generating fail the same as above
  else { 
    Message_Msg msg1225("IGES_1225");
    SendFail(start, msg1225); // StartPoint and EndPoint of the 2d line are the same Point
  }
  return res;
}



//=======================================================================
//function : TransferTransformation
//purpose  : 
//=======================================================================

Handle(Geom_Transformation)  IGESToBRep_BasicCurve::TransferTransformation
       (const Handle(IGESGeom_TransformationMatrix)& start)
     
{ 
  Handle(Geom_Transformation) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  gp_Trsf resultat;
  SetEpsilon(1.E-05);  
  if ( IGESData_ToolLocation::ConvertLocation
      (GetEpsilon(),start->Value(),resultat) )
    res = new Geom_Transformation(resultat);
  else {
    Message_Msg msg1036("IGES_1036");
    SendFail(start, msg1036); // Transformation : not a similarity
  } 
  return res;
}




//=======================================================================
//function : TransferCopiousData
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)  IGESToBRep_BasicCurve::TransferCopiousData
       (const Handle(IGESGeom_CopiousData)& start)
     
{ 
  Handle(Geom_BSplineCurve)  res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  
  Standard_Integer  FormNb = start->FormNumber();
  if (!(FormNb==11 || FormNb==12 || FormNb==63)) {
    Message_Msg msg1240("IGES_1240");
    SendWarning( start, msg1240);
    // "Copious Data : Form number is different from 11, 12 or 63 so the vector treatement is skipped");
  }

  Standard_Integer  NbPoints = start->NbPoints();
  if (NbPoints < 2) {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);  // Count of points lower than 2
    return res;
  }

  //  Filling array of poles :
  //  ========================

  TColgp_Array1OfPnt  TempPole(1,NbPoints);
  Standard_Integer    TempIndex = TempPole.Lower();
  
  if (!GetModeTransfer() && start->HasTransf()) {
    TempPole.SetValue(TempIndex,start->TransformedPoint(1));
  }
  else {
    TempPole.SetValue(TempIndex,start->Point(1));
  }    
  
  TempIndex++;
  Standard_Integer i;// svv Jan 10 2000 : porting on DEC
  for (i=2; i <= NbPoints; i++) {
    gp_Pnt  aPole;
    if (!GetModeTransfer() && start->HasTransf()) 
      aPole = start->TransformedPoint(i);
    else 
      aPole = start->Point(i);
    // #2 pdn 7 May 1998 BUC50028 
    //  delete GetUnitFactor()
    //   if (!aPole.IsEqual(TempPole(TempIndex-1),GetEpsGeom()))
    //S4054: some filter must be kept UKI60556 entity 7 (two equal points)
    if (!aPole.IsEqual(TempPole(TempIndex-1), gp::Resolution()))
      TempPole.SetValue(TempIndex++,aPole);
  }

  NbPoints = TempIndex - TempPole.Lower();

  // #1 pdn 7 May 1998  BUC50028 entity 6307
  if ( NbPoints == 1) {
    Message_Msg msg1235("IGES_1235");
    SendFail(start, msg1235);
    // The curve degenerates to a point");
    return res;
  }
  TColgp_Array1OfPnt  Pole(1,NbPoints);

  TempIndex = TempPole.Lower();
  for (i=Pole.Lower(); i<=Pole.Upper(); i++)  
    Pole.SetValue(i,TempPole.Value(TempIndex++));

  
  //  Filling array of knots :
  //  ========================
  
  TColStd_Array1OfReal  Knot(1,NbPoints);
  
  Knot.SetValue(Knot.Lower(),0.0);

  for (i=Knot.Lower()+1; i <= Knot.Upper(); i++) {
    gp_Pnt  Pole1    =  Pole.Value(i);
    gp_Pnt  Pole2    =  Pole.Value(i-1);
    Standard_Real    KnotDist =  Pole1.Distance(Pole2);
    Knot.SetValue(i, Knot.Value(i-1)+KnotDist);
  }
  
  Standard_Integer  Degree = 1;

  TColStd_Array1OfInteger  Mult(1, NbPoints);
  Mult.Init(Degree);
  Mult.SetValue(Mult.Lower(),Degree+1);
  Mult.SetValue(Mult.Upper(),Degree+1);

  res = new Geom_BSplineCurve(Pole, Knot, Mult, Degree);
  
  IGESConvGeom::IncreaseCurveContinuity (res, Max(GetEpsGeom()/10.,Precision::Confusion()), GetContinuity());
  return res;
}


//                    ================================
//                    ==  TRANSFER 2D Copious data  ==
//                    ================================

Handle(Geom2d_BSplineCurve) IGESToBRep_BasicCurve::Transfer2dCopiousData(const Handle(IGESGeom_CopiousData)& start)
{ 
  Handle(Geom2d_BSplineCurve)  res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  
  Standard_Integer  FormNb = start->FormNumber();
  if (!(FormNb==11 || FormNb==12 || FormNb==63)) {
    Message_Msg msg1240("IGES_1240");
    SendWarning( start, msg1240);
    // "Copious Data : Form number is different from 11, 12 or 63 so the vector treatement is skipped");
  }
  
  Standard_Integer  NbPoints = start->NbPoints();
  if (NbPoints < 2) {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);  // Count of points lower than 2
    return res;
  }
  
  //  Filling array of poles :
  //  ========================
  
  TColgp_Array1OfPnt2d  TempPole(1,NbPoints);
  Standard_Integer      TempIndex = TempPole.Lower();
  
  if (!GetModeTransfer() && start->HasTransf()) 
    TempPole.SetValue(TempIndex,gp_Pnt2d(start->TransformedPoint(1).X(),
					 start->TransformedPoint(1).Y()));
  else 
    TempPole.SetValue(TempIndex,gp_Pnt2d(start->Point(1).X(),
					 start->Point(1).Y()));
  
  
  TempIndex++;
  Standard_Integer i;//svv Jan 10 2000 : porting on DEC
  for  (i=2; i <= NbPoints; i++) {
    gp_Pnt2d  aPole;
    if (!GetModeTransfer() && start->HasTransf()) 
      aPole = gp_Pnt2d(start->TransformedPoint(i).X(),
		       start->TransformedPoint(i).Y());
    else 
      aPole = gp_Pnt2d(start->Point(i).X(),
		       start->Point(i).Y());
    //    if (!aPole.IsEqual(TempPole(TempIndex-1), GetEpsCoeff())) //modified by rln 16/12/97 CSR# PRO11641 entity 46GetEpsGeom()*GetUnitFactor()
    //S4054: some filter must be kept UKI60556 entity 7 (two equal points)
    if (!aPole.IsEqual(TempPole(TempIndex-1), gp::Resolution()))
      TempPole.SetValue(TempIndex++,aPole);
  }

  NbPoints = TempIndex - TempPole.Lower();
  //added by rln on 26/12/97 to avoid exception when creating Bspline from one point
  if (NbPoints == 1) {
    Message_Msg msg1235("IGES_1235");
    SendFail(start, msg1235);
    // The curve degenerates to a point");
    return res;
  }
  TColgp_Array1OfPnt2d  Pole(1,NbPoints);

  TempIndex = TempPole.Lower();
  for (i=Pole.Lower(); i<=Pole.Upper(); i++)  
    Pole.SetValue(i,TempPole.Value(TempIndex++));


  //  Filling array of knots :
  //  ========================

  TColStd_Array1OfReal  Knot(1,NbPoints);
  
  Knot.SetValue(Knot.Lower(),0.0);

  for (i=Knot.Lower()+1; i <= Knot.Upper(); i++) {
    gp_Pnt2d  Pole1    =  Pole.Value(i);
    gp_Pnt2d  Pole2    =  Pole.Value(i-1);
    Standard_Real      KnotDist =  Pole1.Distance(Pole2);
    Knot.SetValue(i, Knot.Value(i-1)+KnotDist);
  }
  
  const Standard_Integer  Degree = 1;

  TColStd_Array1OfInteger  Mult(1, NbPoints);
  Mult.Init(Degree);
  Mult.SetValue(Mult.Lower(),Degree+1);
  Mult.SetValue(Mult.Upper(),Degree+1);
  
  res = new Geom2d_BSplineCurve(Pole, Knot, Mult, Degree);
  
  Standard_Real epsGeom = GetEpsGeom();
  Standard_Real anUVResolution = GetUVResolution();
  
  IGESConvGeom::IncreaseCurveContinuity (res, Max(Precision::Confusion(),epsGeom*anUVResolution), GetContinuity());
  return res;
}
