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
//:k0 abv 16.12.98: eliminating existing code
// 21.12.98 rln, gka S4054
//:k5 abv 25 Dec 98: PRO8803 1901: extending method of fixing Multi > Degree
// 28.12.98 dce S3767 New messaging system
//#61 rln 05.01.99
//:l1 abv 06.01.99: USA60022.igs 243: fixing missing seam on closed surface 
//:p6 abv 26.02.99: improve messages after call to ConvertToPeriodic
//#75 rln 11.03.99: using message mechanism for periodic B-Spline
//S4181 pdn 15.04.99 implementing of reading IGES elementary surfaces.
//sln 29.12.2001 OCC90 : Method checkBSplineSurfaceStatus and varification of creation of bspline surfaces were added
//=======================================================================

#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <IGESConvGeom.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_BasicSurface.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <Interface_Macros.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>

//:36
//S3767
//S4181
//=======================================================================
//function : CheckBSplineSurface
//purpose  : Check coincidede knots. Check whether knots are in ascending 
//           order and difference between vaues of weights more than 1000. 
//           Send corresponding messages. The function returns Standard_False 
//           if surface can not be created, Standard_True otherwise.
//=======================================================================
static Standard_Boolean checkBSplineSurface(IGESToBRep_BasicSurface*             theSurface,
                                          const Handle(IGESGeom_BSplineSurface)& theBSplineSurface,
                                          TColStd_Array1OfReal&                  SUKnots,
                                          TColStd_Array1OfReal&                  SVKnots,
                                          const TColStd_Array2OfReal&            SWeights)
{
  // check whether difference between vaues of weights more than 1000.
  if(!theBSplineSurface->IsPolynomial()) {
    Standard_Real aMinValue = SWeights.Value(SWeights.LowerRow(), SWeights.LowerCol());
    Standard_Real aMaxValue = SWeights.Value(SWeights.LowerRow(), SWeights.LowerCol());
    for(Standard_Integer i = SWeights.LowerRow(); i<= SWeights.UpperRow(); i++)
    for(Standard_Integer j = SWeights.LowerCol(); j<= SWeights.UpperCol(); j++) {
      if(SWeights.Value(i,j) < aMinValue) aMinValue = SWeights.Value(i,j);
      if(SWeights.Value(i,j) > aMaxValue) aMaxValue = SWeights.Value(i,j);    

      if(aMaxValue - aMinValue > 1000) {
        Message_Msg msg1374("IGES_1374"); // WARNING - Difference between weights is too big.
        theSurface->SendWarning(theBSplineSurface, msg1374);
      }
    }
  }
 
  Standard_Boolean aResult = Standard_True;
  
  //check whether knots are in ascending order.
  Standard_Boolean aWrongOrder = Standard_False;
  Standard_Integer i;
  for (i = SUKnots.Lower(); (i < SUKnots.Upper()) && (!aWrongOrder); i++) 
    if(SUKnots.Value (i+1) < SUKnots.Value (i)) aWrongOrder = Standard_True;
  for (i = SVKnots.Lower(); (i < SVKnots.Upper()) && (!aWrongOrder); i++) 
    if(SVKnots.Value (i+1) < SVKnots.Value (i)) aWrongOrder = Standard_True;
  
  if(aWrongOrder) { 
    Message_Msg msg1373("IGES_1373"); // FAIL - Knots are not in ascending order 
    theSurface->SendFail(theBSplineSurface, msg1373);
    aResult = Standard_False;
  }

  //Fix coincided knots
  if(aResult) {
    ShapeConstruct_Curve::FixKnots(SUKnots);
    ShapeConstruct_Curve::FixKnots(SVKnots);
  }
  
  return aResult;

}


//=======================================================================
//function : IGESToBRep_BasicSurface
//purpose  : 
//=======================================================================

IGESToBRep_BasicSurface::IGESToBRep_BasicSurface()
     :IGESToBRep_CurveAndSurface()
{  
  SetModeTransfer(Standard_False);
}

//=======================================================================
//function : IGESToBRep_BasicSurface
//purpose  : 
//=======================================================================

IGESToBRep_BasicSurface::IGESToBRep_BasicSurface
  (const IGESToBRep_CurveAndSurface& CS)
     :IGESToBRep_CurveAndSurface(CS)
{  
}

//=======================================================================
//function : IGESToBRep_BasicSurface
//purpose  : 
//=======================================================================

IGESToBRep_BasicSurface::IGESToBRep_BasicSurface
  (const Standard_Real    eps,
   const Standard_Real    epsCoeff,
   const Standard_Real    epsGeom,
   const Standard_Boolean mode,
   const Standard_Boolean modeapprox,
   const Standard_Boolean optimized)
     :IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, modeapprox,
				 optimized)
{  
}


//=============================================
//Function : TransferBasicSurface
//Purpose : Choice of the right transfer method
//=============================================

Handle(Geom_Surface) IGESToBRep_BasicSurface::TransferBasicSurface
       (const Handle(IGESData_IGESEntity)& start)
{ 
  Handle(Geom_Surface) resurf;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return resurf;
  }

  try { //:36 by abv 11.12.97: Geom_BSplineSurface raiss if some weights <0
    OCC_CATCH_SIGNALS
    
    //S4054
    if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineSurface))) {
      DeclareAndCast(IGESGeom_BSplineSurface, st128, start);
      resurf = TransferBSplineSurface(st128);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineSurface))) {
      DeclareAndCast(IGESGeom_SplineSurface, st114, start);
      resurf = TransferSplineSurface(st114);
    } //S4181 pdn 15.04.99  implementing of reading IGES elementary surfaces.
    else if (start->IsKind(STANDARD_TYPE(IGESSolid_PlaneSurface))) {
      DeclareAndCast(IGESSolid_PlaneSurface, st190, start);
      resurf = TransferPlaneSurface(st190);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESSolid_CylindricalSurface))) {
      DeclareAndCast(IGESSolid_CylindricalSurface, st192, start);
      resurf = TransferRigthCylindricalSurface(st192);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESSolid_ConicalSurface))) {
      DeclareAndCast(IGESSolid_ConicalSurface, st194, start);
      resurf = TransferRigthConicalSurface(st194);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESSolid_SphericalSurface))) {
      DeclareAndCast(IGESSolid_SphericalSurface, st196, start);
      resurf = TransferSphericalSurface(st196);
    }
    else if (start->IsKind(STANDARD_TYPE(IGESSolid_ToroidalSurface))) {
      DeclareAndCast(IGESSolid_ToroidalSurface, st198, start);
      resurf = TransferToroidalSurface(st198);
    }
    else {
     // AddFail(start, "Spline or BSpline surface expected for TransferBasicSurface");
      return resurf;
    }

  } //:36
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "\n** Exception in IGESToBRep_BasicSurface::TransferBasicSurface : "; 
    anException.Print(cout);
#endif
    (void)anException;
  }

  if (resurf.IsNull()) {
    // AddFail(start,"The IGESEntity cannot be transfered");
  }
  else
    resurf->Scale(gp_Pnt(0,0,0),GetUnitFactor());

  return resurf;
}


//=======================================================================
//function : TransferPlaneSurface
//purpose  : 
//=======================================================================

Handle(Geom_Plane) IGESToBRep_BasicSurface::TransferPlaneSurface(const Handle(IGESSolid_PlaneSurface)& start)
{
  Handle(Geom_Plane) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  
  gp_Pln pln;
  Standard_Boolean Param = start->IsParametrised();
  Handle(IGESGeom_Point) Point = start->LocationPoint();
  Handle(IGESGeom_Direction) Dir = start->Normal();

  if (Point.IsNull()) {
    Message_Msg msg174("XSTEP_174");   
    SendFail(start, msg174);
    // Point Reading Error : Null IGESEntity
    return res;
  }
  if (Dir.IsNull()) {
    Message_Msg msg1280("IGES_1280");
    SendFail(start, msg1280);
    // Direction Reading Error : Null IGESEntity
    return res;
  } 

  gp_Pnt Pt = Point->Value();
  gp_Dir Normale = gp_Dir(Dir->Value());
  if (!Param) {
    pln = gp_Pln(Pt,Normale);
  }
  else {
    Handle(IGESGeom_Direction) refdir = start->ReferenceDir();
    gp_Dir Dirgp = gp_Dir(refdir->Value());
    pln = gp_Pln( gp_Ax3(Pt, Normale, Dirgp));  
  }
  
  return new Geom_Plane(pln); 
}

//=======================================================================
//function : TransferRigthCylindricalSurface
//purpose  : 
//=======================================================================

Handle(Geom_CylindricalSurface) IGESToBRep_BasicSurface::TransferRigthCylindricalSurface
       (const Handle(IGESSolid_CylindricalSurface)& start)
{
  Handle(Geom_CylindricalSurface) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }
  
  Standard_Boolean Param = start->IsParametrised();
  Handle(IGESGeom_Point) Point = start->LocationPoint();
  Handle(IGESGeom_Direction) Axis = start->Axis();
  Standard_Real radius = start->Radius();
  
  if (Point.IsNull()) {
    Message_Msg msg174("XSTEP_174");   
    SendFail(start, msg174);
    // Point Reading Error : Null IGESEntity
    return res;
  }
  if (Axis.IsNull()) {
    Message_Msg msg1280("IGES_1280");
    SendFail(start, msg1280);
    // Direction Reading Error : Null IGESEntity
    return res;
  }
  if (radius < Precision::Confusion()) {
    return res;
  }
    
  gp_Pnt Pt = Point->Value();
  gp_Dir ax = gp_Dir(Axis->Value());
  gp_Ax3 ax3;
  if (!Param) 
    ax3 = gp_Ax3(Pt,ax);
  else {
    Handle(IGESGeom_Direction) refdir = start->ReferenceDir();
    gp_Dir Dir = gp_Dir(refdir->Value());
    gp_Dir vc = Dir^ax;
    if(vc.XYZ().Modulus() < Precision::Confusion()) {
      
      return res;
    }
    ax3 = gp_Ax3(Pt,ax,Dir);
  }
  gp_Cylinder cyl(ax3,radius);
  return new Geom_CylindricalSurface(cyl);
}

//=======================================================================
//function : TransferRigthConicalSurface
//purpose  : 
//=======================================================================

Handle(Geom_ConicalSurface) IGESToBRep_BasicSurface::TransferRigthConicalSurface
       (const Handle(IGESSolid_ConicalSurface)& start)
{
  Handle(Geom_ConicalSurface) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  } 
  
  Standard_Boolean Param = start->IsParametrised();
  Handle(IGESGeom_Point) Point = start->LocationPoint();
  Handle(IGESGeom_Direction) Axis = start->Axis();
  Standard_Real radius = start->Radius();
  Standard_Real angle  = start->SemiAngle()/180.*M_PI;
  
  if (Point.IsNull()) {
    Message_Msg msg174("XSTEP_174");   
    SendFail(start, msg174);
    // Point Reading Error : Null IGESEntity
    return res;
  }
  if (Axis.IsNull()) {
    Message_Msg msg1280("IGES_1280");
    SendFail(start, msg1280);
    // Direction Reading Error : Null IGESEntity
    return res;
  }
  if (angle < Precision::Confusion()||angle > M_PI/2.) {
    return res;
  }
  if (radius < 0) {
    return res;
  }
  if (radius < Precision::Confusion())
    radius = 0.;
  
  gp_Pnt Pt = Point->Value();
  gp_Dir ax = gp_Dir(Axis->Value());
  gp_Ax3 ax3;
  if (!Param) 
    ax3 = gp_Ax3(Pt,ax);
  else {
    Handle(IGESGeom_Direction) refdir = start->ReferenceDir();
    gp_Dir Dir = gp_Dir(refdir->Value());
    gp_Dir vc = Dir^ax;
    if(vc.XYZ().Modulus() < Precision::Confusion()) {
      
      return res;
    }
    ax3 = gp_Ax3(Pt,ax,Dir);
  }
  return new Geom_ConicalSurface(ax3,angle,radius);
}

//=======================================================================
//function : TransferSphericalSurface
//purpose  : 
//=======================================================================

Handle(Geom_SphericalSurface) IGESToBRep_BasicSurface::TransferSphericalSurface
       (const Handle(IGESSolid_SphericalSurface)& start)
{
  Handle(Geom_SphericalSurface) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  } 
  
  Standard_Boolean Param = start->IsParametrised();
  Handle(IGESGeom_Point) Point = start->Center();
  Handle(IGESGeom_Direction) Axis = start->Axis();
  Standard_Real radius = start->Radius();
  
  if (Point.IsNull()) {
    Message_Msg msg174("XSTEP_174");   
    SendFail(start, msg174);
    // Point Reading Error : Null IGESEntity
    return res;
  }
  if (Axis.IsNull()) {
    Message_Msg msg1280("IGES_1280");
    SendFail(start, msg1280);
    // Direction Reading Error : Null IGESEntity
    return res;
  }
  if (radius < Precision::Confusion()){
   
    return res;
  }

  gp_Pnt Pt = Point->Value();
  gp_Dir ax = gp_Dir(Axis->Value());
  gp_Ax3 ax3;
  if (!Param) 
    ax3 = gp_Ax3(Pt,ax);
  else {
    Handle(IGESGeom_Direction) refdir = start->ReferenceDir();
    gp_Dir Dir = gp_Dir(refdir->Value());
    gp_Dir vc = Dir^ax;
    if(vc.XYZ().Modulus() < Precision::Confusion()) {
      
      return res;
    }
    ax3 = gp_Ax3(Pt,ax,Dir);
  }
  return new Geom_SphericalSurface(ax3,radius); 
}

//=======================================================================
//function : TransferToroidalSurface
//purpose  : 
//=======================================================================

Handle(Geom_ToroidalSurface) IGESToBRep_BasicSurface::TransferToroidalSurface
       (const Handle(IGESSolid_ToroidalSurface)& start)
{
  Handle(Geom_ToroidalSurface) res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  } 
  
  Standard_Boolean Param = start->IsParametrised();
  Handle(IGESGeom_Point) Point = start->Center();
  Handle(IGESGeom_Direction) Axis = start->Axis();
  Standard_Real major = start->MajorRadius();
  Standard_Real minor = start->MinorRadius();
  
  if (Point.IsNull()) {
    Message_Msg msg174("XSTEP_174");   
    SendFail(start, msg174);
    // Point Reading Error : Null IGESEntity
    return res;
  }
  if (Axis.IsNull()) {
    Message_Msg msg1280("IGES_1280");
    SendFail(start, msg1280);
    // Direction Reading Error : Null IGESEntity
    return res;
  }
  if (major < Precision::Confusion()||minor < Precision::Confusion()){
   
    return res;
  }

  gp_Pnt Pt = Point->Value();
  gp_Dir ax = gp_Dir(Axis->Value());
  gp_Ax3 ax3;
  if (!Param) 
    ax3 = gp_Ax3(Pt,ax);
  else {
    Handle(IGESGeom_Direction) refdir = start->ReferenceDir();
    gp_Dir Dir = gp_Dir(refdir->Value());
    gp_Dir vc = Dir^ax;
    if(vc.XYZ().Modulus() < Precision::Confusion()) {
      
      return res;
    }
    ax3 = gp_Ax3(Pt,ax,Dir);
  }
  return new Geom_ToroidalSurface(ax3,major,minor);
}
//==================================
//Function : TransferSplineSurface :
//Purpose :
//==================================

Handle(Geom_BSplineSurface) IGESToBRep_BasicSurface::TransferSplineSurface
       (const Handle(IGESGeom_SplineSurface)& start)
{ 
  Handle(Geom_BSplineSurface) resconv;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return resconv;
  }

  Standard_Real epscoef = GetEpsCoeff();
  Standard_Real epsgeom = GetEpsGeom();

  Standard_Integer result = IGESConvGeom::SplineSurfaceFromIGES
    (start, epscoef, epsgeom, resconv);

  switch (result) {
  case 5 : {
    Message_Msg msg1305("IGES_1305");
    SendFail (start, msg1305); // less than on segment in U or V (no result produced)
    return resconv;
  }
  case 4 : {
    Message_Msg msg1190("IGES_1190");
    SendFail (start, msg1190);
    return resconv;
  }
    // case 3 :
    // AddWarning ( start, "Degree is not compatible with code boundary type , C0 is not guaranteed)");
    //case 2 :
    // AddWarning ( start, "Degree is not compatible with code boundary type , C0 is guaranted)");
  default :
      break;
  }
  
  
  //  Checking C2 and C1 continuity :  case 1 :
  //  AddWarning ( start, "The result is not guaranteed to be C0");
  //  ===============================

  IGESConvGeom::IncreaseSurfaceContinuity (resconv, epsgeom, GetContinuity());
  return resconv;
}

//===================================
//Function : TransferBSplineSurface :
//Purpose : 
//===================================

Handle(Geom_BSplineSurface)  IGESToBRep_BasicSurface::TransferBSplineSurface
       (const Handle(IGESGeom_BSplineSurface)& start)
{ 
  Handle(Geom_BSplineSurface)  res;
  if (start.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(start, msg1005);
    return res;
  }

  Standard_Integer  DegreeU = start->DegreeU();
  Standard_Integer  DegreeV = start->DegreeV();
  if ((DegreeU <= 0)  ||  (DegreeU > Geom_BSplineSurface::MaxDegree()) ||
      (DegreeV <= 0)  ||  (DegreeV > Geom_BSplineSurface::MaxDegree())) {
    Message_Msg msg1310("IGES_1310");
    SendFail ( start, msg1310);
    // Improper Input BSpline Degree
    return res;
  }
  
  //  Filling poles array :
  //  =====================
  
  Standard_Integer  NbPolesU = start->NbPolesU();
  Standard_Integer  newNbPolesU = NbPolesU;
  if (NbPolesU<2) {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);
    // Number of poles lower than 2 following U
    return res;
  }
  
  Standard_Integer  NbPolesV = start->NbPolesV();
  Standard_Integer  newNbPolesV = NbPolesV;
  if (NbPolesV<2) {
    Message_Msg msg1195("IGES_1195");
    SendFail(start, msg1195);
    return res;
  }
  
  TColgp_Array2OfPnt  Pole(1, NbPolesU, 1, NbPolesV);
  Standard_Integer             UIndex = Pole.LowerRow();
  Standard_Integer             VIndex = Pole.LowerCol();
  Standard_Integer             i, j; //szv#4:S4163:12Mar99 k unused
  
  if (!GetModeTransfer() && start->HasTransf()) 
    for (i=0; i <= start->UpperIndexU(); i++) {
      for (j=0; j <= start->UpperIndexV(); j++) 
	Pole.SetValue(UIndex, VIndex++, start->TransformedPole(i,j));
      UIndex++;
      VIndex = Pole.LowerCol();
    }
  else
    for (i=0; i <= start->UpperIndexU(); i++) {
      for (j=0; j <= start->UpperIndexV(); j++) 
	Pole.SetValue(UIndex, VIndex++, start->Pole(i,j));
      UIndex++;
      VIndex = Pole.LowerCol();
    }
  
  
  //  KNOTS & MULTIPLICITIES for U :
  //  ==============================
  
  Standard_Integer           NbUKnots = start->NbKnotsU();
  TColStd_Array1OfReal       TempUKnot(1, NbUKnots);
  TColStd_Array1OfInteger    TempUMult(1, NbUKnots);
  TempUMult.Init(1);

  UIndex = TempUKnot.Lower();
  
  
  //  If several identical IGES knots are encountered, corresponding 
  //  multiplicity is increased
  //  ==============================================================
  
  TempUKnot.SetValue(UIndex, start->KnotU(-DegreeU));
  
  for (i = 1-DegreeU; i < NbUKnots-DegreeU; i++) {

    Standard_Real  UKnot1 = start->KnotU(i);
    Standard_Real  UKnot2 = start->KnotU(i-1);

    if (Abs(UKnot1 - UKnot2) <= Epsilon(UKnot2))
      TempUMult.SetValue(UIndex, TempUMult.Value(UIndex) + 1);
    else 
      TempUKnot.SetValue(++UIndex, UKnot1);
  }
  
  //  Final knots & multiplicities arraies are dimensionned so as to be fully 
  //  filled :
  //  =======================================================================
  
  TColStd_Array1OfReal     UKnot(1,UIndex);
  TColStd_Array1OfInteger  UMult(1,UIndex);
  
  Standard_Integer SumOfUMult = 0;

  TColStd_SequenceOfInteger SeqIndexU;
  Standard_Integer DelIndexU;
  Standard_Integer OldSumOfUMult = 0;
  for (i=1 ; i <= UIndex; i++) { //:k5 abv 25 Dec 98: cycle modified
    Standard_Integer aMult = TempUMult.Value(i);
    Standard_Integer maxMult = ( i==1 || i == UIndex ? DegreeU + 1 : DegreeU );
    if (aMult > maxMult) {
      Message_Msg msg1200("IGES_1200");
      msg1200.Arg("U");
      msg1200.Arg("U");
      msg1200.Arg("U");//#61 rln 05.01.99
      SendWarning(start, msg1200); //U Multiplicity > DegreeU (or Degree+1 at end); corrected
      for ( DelIndexU = OldSumOfUMult + 1; aMult > maxMult; DelIndexU++, aMult-- ) {
	newNbPolesU--;
	SeqIndexU.Append(DelIndexU);
      }
    }
    OldSumOfUMult += TempUMult.Value(i); 
    UKnot.SetValue(i, TempUKnot.Value(i));
    UMult.SetValue(i, aMult);
    SumOfUMult += aMult;
  }

  if (SumOfUMult != newNbPolesU + DegreeU + 1) {
    Message_Msg msg1210("IGES_1210");
    msg1210.Arg("U");
    msg1210.Arg("U");
    SendWarning(start, msg1210);
    // Sum of multiplicities following U is not equal to the sum : Count of poles + DegreeU + 1
  }
  
  //  KNOTS & MULTIPLICITIES for V :
  //  ==============================
  
  Standard_Integer         NbVKnots = start->NbKnotsV();
  TColStd_Array1OfReal     TempVKnot(1, NbVKnots);
  TColStd_Array1OfInteger  TempVMult(1, NbVKnots);
  TempVMult.Init(1);
  
  VIndex = TempVKnot.Lower();
  

  //  If several identical IGES knots are encountered, corresponding 
  //  multiplicity is increased
  //  ==============================================================  
  
  TempVKnot.SetValue(VIndex, start->KnotV(-DegreeV));
  
  for (i = 1-DegreeV; i < NbVKnots-DegreeV; i++) {

    Standard_Real  VKnot1 = start->KnotV(i);
    Standard_Real  VKnot2 = start->KnotV(i-1);

    if (Abs(VKnot1 - VKnot2) <= Epsilon(VKnot2))
      TempVMult.SetValue(VIndex, TempVMult.Value(VIndex) + 1);
    else 
      TempVKnot.SetValue(++VIndex, VKnot1);
  }
  
  //  Final knots & multiplicities arraies are dimensionned so as to be fully 
  //  filled :
  //  =======================================================================
  
  TColStd_Array1OfReal     VKnot(1,VIndex);
  TColStd_Array1OfInteger  VMult(1,VIndex);
  
  Standard_Integer SumOfVMult = 0;
   
  TColStd_SequenceOfInteger SeqIndexV;
  Standard_Integer DelIndexV;
  Standard_Integer OldSumOfVMult = 0;
  for (i=1; i <= VIndex; i++) { //:k5 abv 25 Dec 98: cycle modified
    Standard_Integer aMult = TempVMult.Value(i);
    Standard_Integer maxMult = ( i==1 || i == VIndex ? DegreeV + 1 : DegreeV );
    if (aMult > maxMult) {
      Message_Msg msg1200("IGES_1200");//#61 rln 05.01.99
      msg1200.Arg("V");
      msg1200.Arg("V");
      msg1200.Arg("V");
      SendWarning(start, msg1200);
      //V Multiplicity > DegreeV (or Degree+1 at end); corrected
      for ( DelIndexV = OldSumOfVMult + 1; aMult > maxMult; DelIndexV++, aMult-- ) {
	newNbPolesV--;
	SeqIndexV.Append(DelIndexV);
      }
    }
    OldSumOfVMult += TempVMult.Value(i); 
    VKnot.SetValue(i, TempVKnot.Value(i));
    VMult.SetValue(i, aMult);
    SumOfVMult += aMult;
  }  

  if (SumOfVMult != newNbPolesV + DegreeV + 1)  {
    Message_Msg msg1210("IGES_1210");
    msg1210.Arg("V");
    msg1210.Arg("V");
    SendWarning(start, msg1210); 
    // Sum of multiplicities following V is not equal to the sum : Count of poles + Degree V + 1
  }
  
  // Mise a jour du tableau des poles
  TColgp_Array2OfPnt  Poles(1, newNbPolesU, 1, newNbPolesV);
  TColStd_SequenceOfInteger PoleUInd;
  TColStd_SequenceOfInteger PoleVInd;
  for (i=1; i<=NbPolesU; i++) PoleUInd.Append(i);
  for (i=1; i<=NbPolesV; i++) PoleVInd.Append(i);
  UIndex = Poles.LowerRow();
  VIndex = Poles.LowerCol();

  if (( newNbPolesU < NbPolesU) || (newNbPolesV < NbPolesV)) {
    if ( newNbPolesU < NbPolesU) {
      Standard_Integer Offset = 0;
      for (Standard_Integer itab = 1; itab <= SeqIndexU.Length(); itab++) {
	DelIndexU = SeqIndexU.Value(itab) - Offset;
	PoleUInd.Remove(DelIndexU);
	Offset++;
      }
    }
    if ( newNbPolesV < NbPolesV) {
      Standard_Integer Offset = 0;
      for (Standard_Integer itab = 1; itab <= SeqIndexV.Length(); itab++) {
	DelIndexV = SeqIndexV.Value(itab) - Offset;
	PoleVInd.Remove(DelIndexV);
	Offset++;
      }
    }
    Standard_Integer nbUseq = PoleUInd.Length();
    Standard_Integer nbVseq = PoleVInd.Length();
    if (( nbUseq == newNbPolesU)&&(nbVseq == newNbPolesV)) {
      for ( i=1; i<= newNbPolesU; i++) {
	for (j=1; j<= newNbPolesV; j++) 
	  Poles.SetValue(UIndex,VIndex++ , Pole.Value(PoleUInd.Value(i),
						      PoleVInd.Value(j)));
	UIndex++;
	VIndex = Poles.LowerCol();
      }
    }
    else {
       Message_Msg msg1175("IGES_1175");
       SendWarning(start, msg1175);
    }
  }

  else {
    for ( i=1; i<= newNbPolesU; i++) {
      for (j=1; j<= newNbPolesV; j++) 
	Poles.SetValue (UIndex, VIndex++,Pole.Value(i,j));
      UIndex++;
      VIndex = Poles.LowerCol();
    }
  }
    

  //  Building result taking into account transformation if any :
  //  ===========================================================

  if (!GetModeTransfer() && start->HasTransf()) {
    gp_GTrsf GBSplTrsf(start->CompoundLocation());
    gp_Trsf  BSplTrsf;
    if (IGESData_ToolLocation::ConvertLocation(GetEpsilon(),GBSplTrsf,BSplTrsf)) 
      for (i=Poles.LowerRow(); i<=Poles.UpperRow(); i++) 
	for (j=Poles.LowerCol(); j<=Poles.UpperCol(); j++) 
	  Poles.SetValue(i, j, Poles.Value(i,j).Transformed(BSplTrsf));
    else {   
      Message_Msg msg1035("IGES_1035");
      SendWarning(start, msg1035);
    }
  }
  
  
  //  CREATION with the ARRAY of POLES WEIGHTS if any :
  //  =================================================
  
    if (start->IsPolynomial()) 
    {
      //sln 29.12.2001 OCC90 : If surface can not be created do nothing
      TColStd_Array2OfReal  Weight(1, 1, 1, 1);
      if(!checkBSplineSurface(this, start, UKnot, VKnot, Weight)) return res;
      res = new Geom_BSplineSurface(Poles, UKnot, VKnot, UMult, VMult, 
				    DegreeU, DegreeV);
    }

    else {
      TColStd_Array2OfReal  PoleWeight(1, NbPolesU, 1, NbPolesV);
      Standard_Boolean      polynomial          =  Standard_True;
      Standard_Real         WeightReference     =  start->Weight(0,0);
      Standard_Integer      WeightRow           =  PoleWeight.LowerRow();
      Standard_Integer      WeightCol           =  PoleWeight.LowerCol();
      
      for (i=0; i <= start->UpperIndexU(); i++) {
	for (j=0; j <= start->UpperIndexV(); j++) {
	  polynomial = (Abs(start->Weight(i,j) - WeightReference) 
			<= Epsilon(WeightReference)) && polynomial;
	  //:39 by abv 15.12.97
	  Standard_Real weight = start->Weight(i,j);
	  if ( weight < Precision::PConfusion() ) {
	    Message_Msg msg1215("IGES_1215");
	    SendFail (start, msg1215); // Some weights are not positive.
	    return res;
	  }
	  PoleWeight.SetValue(WeightRow, WeightCol++, weight);
//:39	  PoleWeight.SetValue(WeightRow, WeightCol++, start->Weight(i,j));
	}
	WeightRow++;
	WeightCol = PoleWeight.LowerCol();
      }
      if (polynomial) {
	Message_Msg msg1220("IGES_1220");
	const Standard_CString surface ("surface");      
	msg1220.Arg(surface);
	SendWarning(start, msg1220);
     }
      // Mise a jour du tableau des Weight lors de la correction de la multiplicite
      TColStd_Array2OfReal Weight(1, newNbPolesU, 1, newNbPolesV);
      UIndex = Weight.LowerRow();
      VIndex = Weight.LowerCol();
      if (( newNbPolesU < NbPolesU) || (newNbPolesV < NbPolesV)) {
	//Standard_Integer indj = 1; //szv#4:S4163:12Mar99 unused
	for ( i=1; i<= newNbPolesU; i++) {
	  for (j=1; j<= newNbPolesV; j++) 
	    Weight.SetValue(UIndex, VIndex++ , 
			    PoleWeight.Value(PoleUInd.Value(i),PoleVInd.Value(j)));
	  UIndex++;
	  VIndex = Poles.LowerCol();
	}
      }
      else {
	for ( i=1; i<= newNbPolesU; i++) {
	  for (j=1; j<= newNbPolesV; j++) 
	    Weight.SetValue(UIndex, VIndex++ ,PoleWeight.Value(i,j));
	  UIndex++;
	  VIndex = Poles.LowerCol();
	}
      }

      //sln 29.12.2001 OCC90 : If surface can not be created do nothing
      if(!checkBSplineSurface(this, start, UKnot, VKnot, Weight)) return res;
      res = new Geom_BSplineSurface
	(Poles, Weight, UKnot, VKnot, UMult, VMult, DegreeU, DegreeV);
    }
  

  //  Checking C2 and C1 continuity :
  //  ===============================
  
  Standard_Integer icont = GetContinuity();
  //Standard_Boolean isC1 = Standard_True, isC2 = Standard_True; //szv#4:S4163:12Mar99 not needed

  i = res->LastUKnotIndex();
  Standard_Integer FirstIndex = res->FirstUKnotIndex();
  while (--i >= FirstIndex+1) {
    if (icont >=2) {
      if(!res->RemoveUKnot(i, DegreeU-2, GetEpsGeom())) {
	//isC2 = Standard_False; //szv#4:S4163:12Mar99 not needed
	res->RemoveUKnot(i, DegreeU-1, GetEpsGeom()); //szv#4:S4163:12Mar99 `isC1=` not needed
      }
      else
	res->RemoveUKnot(i, DegreeU-1, GetEpsGeom()); //szv#4:S4163:12Mar99 `isC1=` not needed
    }
  }

  i = res->LastVKnotIndex();
  FirstIndex = res->FirstVKnotIndex();
  while (--i >= FirstIndex+1) {
    if (icont >=2) {
      if(!res->RemoveVKnot(i, DegreeV-2, GetEpsGeom())) {
	//isC2 = Standard_False; //szv#4:S4163:12Mar99 not needed
	res->RemoveVKnot(i, DegreeV-1, GetEpsGeom()); //szv#4:S4163:12Mar99 `isC1=` not needed
      }
      else
	res->RemoveVKnot(i, DegreeV-1, GetEpsGeom()); //szv#4:S4163:12Mar99 `isC1=` not needed
    }
  }

  //:h7 abv 14 Jul 98: ims010.igs 2519: closed periodic surface should be forced
  // else some wires which can lie over the seam will be incorrect
  //:l1 abv 6 Jan 99: USA60022 243: force periodicity on any closed surface
  Standard_Boolean isUPeriodic = ( start->IsClosedU() && ( start->IsPeriodicU() ||
							   res->IsUClosed() ) );
  Standard_Boolean isVPeriodic = ( start->IsClosedV() && ( start->IsPeriodicV() ||
							   res->IsVClosed() ) );
  //:k0 abv 16 Dec 98: use ShapeCustom 
  if ( isUPeriodic || isVPeriodic ) {
    Handle(Geom_BSplineSurface) periodicSurf =
      Handle(Geom_BSplineSurface)::DownCast ( ShapeAlgo::AlgoContainer()->ConvertToPeriodic (res) );
    if ( ! periodicSurf.IsNull() ) { //:p6 abv 26 Feb 99: && periodicSurf != res ) {
      //#75 rln 11.03.99: using message mechanism
      Message_Msg msg1221("IGES_1221");
      SendWarning(start, msg1221);//SendWarning(start,"Surface forced to be periodic");
      res = periodicSurf;
    }
  }
  return res;
}




//=======================================================================
//function : DeletePoleRow
//purpose  : 
//=======================================================================
//szv#4:S4163:12Mar99 function never referenced
/*
static void DeletePoleRow
  (const TColgp_Array2OfPnt& Poles,
   const Standard_Integer    Index,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer Offset = 0;
  Standard_Integer ColIndex;
  Standard_Integer RowIndex = NewPoles.LowerRow();
  while (RowIndex <= NewPoles.UpperRow()) {
    ColIndex = NewPoles.LowerCol();
    if (RowIndex == Index)  Offset = 1;
    while (ColIndex <= NewPoles.UpperCol()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex + Offset, ColIndex);
      ColIndex++;
    }
    RowIndex++;
  }
}
*/

//=======================================================================
//function : DeletePoleCol
//purpose  : 
//=======================================================================
//szv#4:S4163:12Mar99 function never referenced
/*
static void DeletePoleCol
  (const TColgp_Array2OfPnt& Poles,
   const Standard_Integer    Index,
         TColgp_Array2OfPnt& NewPoles)
{
  Standard_Integer Offset = 0;
  Standard_Integer RowIndex;
  Standard_Integer ColIndex = NewPoles.LowerCol();
  while (ColIndex <= NewPoles.UpperCol()) {
    RowIndex = NewPoles.LowerRow();
    if (ColIndex == Index)  Offset = 1;
    while (RowIndex <= NewPoles.UpperRow()){
      NewPoles (RowIndex, ColIndex) = Poles (RowIndex, ColIndex + Offset);
      RowIndex++;
    }
    ColIndex++;
  }
}
*/

