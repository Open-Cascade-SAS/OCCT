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

// modif du 22/10/96 mjm
// ajout du champ TheLength
//:l6 abv 15.01.99: CTS22022: writing full tori
//szv#4:S4163:12Mar99
//S4181 pdn 20.04.99 implementing of writing IGES elementary surfaces.
//szv#10:PRO19566:05Oct99 workaround against weights array loss

#include <gce_MakeLin.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Direction.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Point.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <GeomToIGES_GeomEntity.hxx>
#include <GeomToIGES_GeomPoint.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <GeomToIGES_GeomVector.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IGESConvGeom_GeomBuilder.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_HArray2OfXYZ.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

//=============================================================================
// GeomToIGES_GeomSurface
//=============================================================================
GeomToIGES_GeomSurface::GeomToIGES_GeomSurface()
:GeomToIGES_GeomEntity()
{
  myBRepMode = Standard_False;
  myAnalytic = Standard_False;
}


//=============================================================================
// GeomToIGES_GeomSurface
//=============================================================================

GeomToIGES_GeomSurface::GeomToIGES_GeomSurface(const GeomToIGES_GeomEntity& GE)
     :GeomToIGES_GeomEntity(GE)
{
  myBRepMode = Standard_False;
  myAnalytic = Standard_False;
}


//=============================================================================
// Transfer des Entites Surface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_Surface)& start, 
								    const Standard_Real Udeb,
								    const Standard_Real Ufin, 
								    const Standard_Real Vdeb, 
								    const Standard_Real Vfin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_BoundedSurface))) {
    DeclareAndCast(Geom_BoundedSurface, Bounded, start);
    res = TransferSurface(Bounded, Udeb, Ufin, Vdeb, Vfin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_ElementarySurface))) {
    DeclareAndCast(Geom_ElementarySurface, Elementary, start);
    res = TransferSurface(Elementary, Udeb, Ufin, Vdeb, Vfin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {
    DeclareAndCast(Geom_SweptSurface, Swept, start);
    res = TransferSurface(Swept, Udeb, Ufin, Vdeb, Vfin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    DeclareAndCast(Geom_OffsetSurface, OffsetS, start);
    res = TransferSurface(OffsetS, Udeb, Ufin, Vdeb, Vfin);
  }
  
  return res;
}
 

//=============================================================================
// Transfer des Entites BoundedSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_BoundedSurface)& start, 
								    const Standard_Real Udeb,
								    const Standard_Real Ufin,
								    const Standard_Real Vdeb, 
								    const Standard_Real Vfin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
    DeclareAndCast(Geom_BSplineSurface, BSpline, start);
    res = TransferSurface(BSpline, Udeb, Ufin, Vdeb, Vfin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    DeclareAndCast(Geom_BezierSurface, Bezier, start);
    res = TransferSurface(Bezier, Udeb, Ufin, Vdeb, Vfin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    DeclareAndCast(Geom_RectangularTrimmedSurface, Trimmed, start);
    res = TransferSurface(Trimmed,Udeb, Ufin, Vdeb, Vfin);
  }

  return res;
}


//=============================================================================
// Transfer des Entites BSplineSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_BSplineSurface)& start,
								    const Standard_Real Udeb,
								    const Standard_Real Ufin,
								    const Standard_Real Vdeb,
								    const Standard_Real Vfin)
{
  //  a b-spline surface is defined by :
  //         The U and V Degree (up to 25)
  //         The Poles  (and the weights if it is rational)
  //         The U and V Knots and Multiplicities
  //         
  //  The knot vector   is an  increasing  sequence  of reals without  repetition. 
  //  The multiplicities are the repetition of the knots.
  //           
  //  If the knots are regularly spaced (the difference of two consecutive knots  
  //  is a constant),
  //  the knots repartition (in U or V) is :
  //              - Uniform if all multiplicities are 1.
  //              -  Quasi-uniform if  all multiplicities are  1
  //              but the first and the last which are Degree+1.
  //              -   PiecewiseBezier if  all multiplicites  are
  //              Degree but the   first and the  last which are
  //              Degree+1. 
  //              
  //         The surface may be periodic in U and in V. 
  //              On a U periodic surface if there are k U knots
  //              and the poles table  has p rows.  the U period
  //              is uknot(k) - uknot(1)
  //              
  //              the poles and knots are infinite vectors with :
  //                uknot(i+k) = uknot(i) + period
  //                pole(i+p,j) = pole(i,j)


  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_BSplineSurface) BSpline = new IGESGeom_BSplineSurface;
  Handle(Geom_BSplineSurface) mysurface;
  
  Standard_Boolean PeriodU = start->IsUPeriodic();
  Standard_Boolean PeriodV = start->IsVPeriodic();
  mysurface = Handle(Geom_BSplineSurface)::DownCast(start->Copy());

  Standard_Real Umin = Udeb, Umax = Ufin, Vmin = Vdeb, Vmax = Vfin;
  Standard_Real U0,U1,V0,V1;
  Standard_Real uShift = 0, vShift = 0;
  mysurface->Bounds(U0,U1,V0,V1);

  // fix bounds
  if (!PeriodU) {
    if (Umin < U0)
      Umin = U0;
    if (U1 < Umax)
      Umax = U1;
  }
  else {
    if (Abs(Umin - U0) < Precision::PConfusion())
      Umin = U0;
    if (Abs(Umax - U1) < Precision::PConfusion())
      Umax = U1;
    uShift = ShapeAnalysis::AdjustToPeriod(Umin, U0, U1);
    Umin += uShift;
    Umax += uShift;
    if (Umax - Umin > U1 - U0)
      Umax = Umin + (U1 - U0);
  }
  if (!PeriodV) {
    if (Vmin < V0)
      Vmin = V0;
    if (V1 < Vmax)
      Vmax = V1;
  }
  else {
    if (Abs(Vmin - V0) < Precision::PConfusion())
      Vmin = V0;
    if (Abs(Vmax - V1) < Precision::PConfusion())
      Vmax = V1;
    vShift = ShapeAnalysis::AdjustToPeriod(Vmin, V0, V1);
    Vmin += vShift;
    Vmax += vShift;
    if (Vmax - Vmin > V1 - V0)
      Vmax = Vmin + (V1 - V0);
  }
  //unperiodize surface to get neccessary for IGES standard number of knots and mults
  if ( mysurface->IsUPeriodic() ) {
    // set new origin for periodic BSpline surfaces for synchronization of pcurves ranges
    // and surface bounds (issue 26138)
    if (mysurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
      Standard_Real uMaxShift = 0;
      uMaxShift = ShapeAnalysis::AdjustToPeriod(Ufin, U0, U1);
      if (Abs(uShift - uMaxShift) > Precision::PConfusion()) {
        Handle(Geom_BSplineSurface) aBspl = Handle(Geom_BSplineSurface)::DownCast (mysurface->Copy());
        Standard_Integer aLeft, aRight;
        aBspl->LocateU(Umin, Precision::PConfusion(), aLeft, aRight);
        aBspl->SetUOrigin(aLeft);
        mysurface = aBspl;
      }
    }
    mysurface->SetUNotPeriodic();
  }
  if ( mysurface->IsVPeriodic() ) {
    // set new origin for periodic BSpline surfaces for synchronization of pcurves ranges
    // and surface bounds (issue 26138)
    if (mysurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
      Standard_Real vMaxShift = 0;
      vMaxShift = ShapeAnalysis::AdjustToPeriod(Vfin, V0, V1);
      if (Abs(vShift - vMaxShift) > Precision::PConfusion()) {
        Handle(Geom_BSplineSurface) aBspl = Handle(Geom_BSplineSurface)::DownCast (mysurface->Copy());
        Standard_Integer aLeft, aRight;
        aBspl->LocateV(Vmin, Precision::PConfusion(), aLeft, aRight);
        aBspl->SetVOrigin(aLeft);
        mysurface = aBspl;
      }
    }
    mysurface->SetVNotPeriodic();
  }
 
  Standard_Integer DegU = mysurface->UDegree();
  Standard_Integer DegV = mysurface->VDegree();
  Standard_Boolean CloseU = mysurface->IsUClosed();
  Standard_Boolean CloseV = mysurface->IsVClosed();
  Standard_Boolean RationU = mysurface->IsURational();
  Standard_Boolean RationV = mysurface->IsVRational();
  Standard_Integer NbUPoles = mysurface->NbUPoles();
  Standard_Integer NbVPoles = mysurface->NbVPoles();
  Standard_Integer IndexU = NbUPoles -1;
  Standard_Integer IndexV = NbVPoles -1;
  Standard_Boolean Polynom = !(RationU || RationV); //szv#10:PRO19566:05Oct99 && was wrong

  // filling knots array for U :
  // Sequence des Knots de [-DegU, IndexU+1] dans IGESGeom.
  Standard_Integer Knotindex;
  Standard_Real rtampon;
  Standard_Integer itampon;
  TColStd_Array1OfReal KU(1, NbUPoles+ DegU+ 1);
  mysurface->UKnotSequence(KU);
  itampon = -DegU;
  Handle(TColStd_HArray1OfReal) KnotsU = 
    new TColStd_HArray1OfReal(-DegU,IndexU+1 );
  for ( Knotindex=KU.Lower(); Knotindex<=KU.Upper(); Knotindex++) { 
    rtampon = KU.Value(Knotindex);
    KnotsU->SetValue(itampon, rtampon);
    itampon++;
  }

  // filling knots array for V :
  // Sequence des Knots de [-DegV, IndexV+1] dans IGESGeom.
  TColStd_Array1OfReal KV(1, NbVPoles+ DegV+ 1);
  mysurface->VKnotSequence(KV);
  itampon = -DegV;
  Handle(TColStd_HArray1OfReal) KnotsV = 
    new TColStd_HArray1OfReal(-DegV, IndexV+1);
  for ( Knotindex=KV.Lower(); Knotindex<=KV.Upper(); Knotindex++) { 
    rtampon = KV.Value(Knotindex);
    KnotsV->SetValue(itampon, rtampon);
    itampon++;
  }

  // filling Weights array de [0, IndexU, 0, IndexV]
  // ----------------------------------------------
  Handle(TColStd_HArray2OfReal) Weights = 
    new TColStd_HArray2OfReal(0 , IndexU, 0, IndexV);
  Standard_Integer WeightRow = Weights->LowerRow();
  Standard_Integer WeightCol = Weights->LowerCol();
  Standard_Integer iw, jw;

  if(RationU || RationV) {
    for ( iw = 1; iw<= IndexU+1; iw++) {
      for ( jw = 1; jw<= IndexV+1; jw++)
	Weights->SetValue(WeightRow, WeightCol++, mysurface->Weight(iw,jw));
      WeightRow++;
      WeightCol = Weights->LowerCol();
    }
  } else {
    for ( iw = 1; iw<= IndexU+1; iw++) {
      for ( jw = 1; jw<= IndexV+1; jw++) 
	Weights->SetValue(WeightRow, WeightCol++, 1.0);
      WeightRow++;
      WeightCol = Weights->LowerCol();
    }

  }
      
  // filling Poles array de [0, IndexU, 0, IndexV]
  // ---------------------------------------------
  Handle(TColgp_HArray2OfXYZ) Poles = 
    new TColgp_HArray2OfXYZ(0, IndexU, 0, IndexV);
  Standard_Integer UIndex = Poles->LowerRow();
  Standard_Integer VIndex = Poles->LowerCol();
  Standard_Integer ipole, jpole;
  Standard_Real Xd, Yd, Zd;

  for ( ipole = 1; ipole<= IndexU+1; ipole++) {
    for ( jpole = 1; jpole<= IndexV+1; jpole++) {
      gp_Pnt tempPnt = mysurface-> Pole(ipole, jpole);
      tempPnt.Coord(Xd, Yd, Zd);
      gp_XYZ PXYZ = gp_XYZ( Xd/GetUnit(), Yd/GetUnit(), Zd/GetUnit());
      Poles->SetValue(UIndex, VIndex++, PXYZ);
    }     
    UIndex++;
    VIndex = Poles->LowerCol();
  }

  BSpline-> Init (IndexU, IndexV, DegU, DegV, CloseU, CloseV, Polynom, PeriodU, 
		  PeriodV, KnotsU, KnotsV, Weights, Poles, Umin, Umax, Vmin, Vmax);
  res = BSpline;
  return res;
}


//=============================================================================
// Transfer des Entites BezierSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_BezierSurface)& start,
								    const Standard_Real /*Udeb*/,
								    const Standard_Real /*Ufin*/,
								    const Standard_Real /*Vdeb*/,
								    const Standard_Real /*Vfin*/)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(Geom_BSplineSurface) Bspline = 
    GeomConvert::SurfaceToBSplineSurface(start);
  Standard_Real U1,U2,V1,V2;
  Bspline->Bounds(U1,U2,V1,V2);
  res = TransferSurface(Bspline, U1, U2, V1, V2);
  return res;
}


//=============================================================================
// Transfer des Entites RectangularTrimmedSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_RectangularTrimmedSurface)& start,
								    const Standard_Real Udeb,
								    const Standard_Real Ufin,
								    const Standard_Real Vdeb,
								    const Standard_Real Vfin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(Geom_Surface) st = start->BasisSurface();
  if (st->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) { 
    //message d'erreur pas de trimmed a partir d'une trimmed , 
    //on peut eventuellement ecrire la surface de base : st.
    return res;
  }

  res = TransferSurface(st, Udeb, Ufin, Vdeb, Vfin);
  return res;

}


//=============================================================================
// Transfer des Entites ElementarySurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_ElementarySurface)& start, 
								    const Standard_Real Udeb,
								    const Standard_Real Ufin,
								    const Standard_Real Vdeb,
								    const Standard_Real Vfin)
{
  Handle(IGESData_IGESEntity) res;
  //  All these entities are located in 3D space with an axis
  //  placement (Location point, XAxis, YAxis, ZAxis). It is 
  //  their local coordinate system.

  //S4181 pdn 16.04.99 Hereunder, the implementation of translation of CAS.CADE
  // elementary surfaces into different types of IGES surfaces according to boolean flags
  if (start.IsNull()) {
    return res;
  }
  if (start->IsKind(STANDARD_TYPE(Geom_Plane))) {
    DeclareAndCast(Geom_Plane, Plane, start);
    if(myBRepMode)
      res = TransferPlaneSurface(Plane, Udeb, Ufin, Vdeb, Vfin);
    else
      res = TransferSurface(Plane, Udeb, Ufin, Vdeb, Vfin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    DeclareAndCast(Geom_CylindricalSurface, Cylindrical, start);
    if(myBRepMode&&myAnalytic)
      res = TransferCylindricalSurface(Cylindrical, Udeb, Ufin, Vdeb, Vfin);
    else
      res = TransferSurface(Cylindrical, Udeb, Ufin, Vdeb, Vfin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    DeclareAndCast(Geom_ConicalSurface, Conical, start);
    if(myBRepMode&&myAnalytic)
      res = TransferConicalSurface(Conical, Udeb, Ufin, Vdeb, Vfin);
    else
      res = TransferSurface(Conical, Udeb, Ufin, Vdeb, Vfin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    DeclareAndCast(Geom_SphericalSurface, Spherical, start);
    if(myBRepMode&&myAnalytic)
      res = TransferSphericalSurface(Spherical, Udeb, Ufin, Vdeb, Vfin);
    else
      res = TransferSurface(Spherical, Udeb, Ufin, Vdeb, Vfin);
  }
  else if ( start->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
    DeclareAndCast(Geom_ToroidalSurface, Toroidal, start);
    if(myBRepMode&&myAnalytic)
      res = TransferToroidalSurface(Toroidal, Udeb, Ufin, Vdeb, Vfin);
       else
	 res = TransferSurface(Toroidal, Udeb, Ufin, Vdeb, Vfin);
  }
  
  return res;

}


//=============================================================================
// Transfer des Entites Plane de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface(const Handle(Geom_Plane)& start,
								    const Standard_Real Udeb,
								    const Standard_Real Ufin,
								    const Standard_Real Vdeb,
								    const Standard_Real Vfin)
{
  // on va ecrire une BSplineSurface pour pouvoir etre coherent avec les courbes 2d
  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }
  if (Interface_Static::IVal("write.iges.plane.mode") == 0){
    Handle(IGESGeom_Plane) aPlane = new IGESGeom_Plane;
    Standard_Real A,B,C,D;
    start->Coefficients(A,B,C,D);
    D = -D;// because of difference in Geom_Plane class and Type 108
    gp_XYZ anAttach = start->Location().XYZ().Divided( GetUnit() );
    aPlane->Init (A, B, C, D / GetUnit(), 0, anAttach, 0);
    res = aPlane;
    return res;
  }
  else{
    Handle(IGESGeom_BSplineSurface) BSpline = new IGESGeom_BSplineSurface;
    gp_Pnt P1 ,P2, P3, P4;
    start->D0(Udeb, Vdeb, P1);
    start->D0(Udeb, Vfin, P2);
    start->D0(Ufin, Vdeb, P3);
    start->D0(Ufin, Vfin, P4);
    Handle(TColgp_HArray2OfXYZ) Poles = new TColgp_HArray2OfXYZ(0, 1, 0, 1);
    Standard_Real X,Y,Z;
    P1.Coord(X,Y,Z);
    Poles->SetValue (0, 0, gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()));
    P2.Coord(X,Y,Z);
    Poles->SetValue (0, 1, gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()));
    P3.Coord(X,Y,Z);
    Poles->SetValue (1, 0, gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()));
    P4.Coord(X,Y,Z);
    Poles->SetValue (1, 1, gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()));

    Handle(TColStd_HArray1OfReal) KnotsU = new TColStd_HArray1OfReal(-1,2);
    KnotsU->SetValue(-1, Udeb);
    KnotsU->SetValue(0, Udeb);
    KnotsU->SetValue(1, Ufin);
    KnotsU->SetValue(2, Ufin);

    Handle(TColStd_HArray1OfReal) KnotsV = new TColStd_HArray1OfReal(-1,2);
    KnotsV->SetValue(-1, Vdeb);
    KnotsV->SetValue(0, Vdeb);
    KnotsV->SetValue(1, Vfin);
    KnotsV->SetValue(2, Vfin);

    Handle(TColStd_HArray2OfReal) Weights = 
      new TColStd_HArray2OfReal(0, 1, 0, 1, 1.);

    //#32 rln 19.10.98
    BSpline-> Init ( 1, 1, 1, 1, Standard_False , Standard_False, Standard_True, 
		    Standard_False, Standard_False,
		    KnotsU, KnotsV, Weights, Poles, Udeb, Ufin, Vdeb, Vfin);
    res = BSpline;
    return res;
  }

}


//=============================================================================
// Transfer des Entites CylindricalSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_CylindricalSurface)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  The "ZAxis" is the symmetry axis of the CylindricalSurface, 
  //  it gives the direction of increasing parametric value V.
  //  The parametrization range is :
  //       U [0, 2*PI],  V ]- infinite, + infinite[
  //  The "XAxis" and the "YAxis" define the placement plane of the 
  //  surface (Z = 0, and parametric value V = 0)  perpendicular to 
  //  the symmetry axis. The "XAxis" defines the origin of the 
  //  parameter U = 0.  The trigonometric sense gives the positive 
  //  orientation for the parameter U.

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_SurfaceOfRevolution) Surf = new IGESGeom_SurfaceOfRevolution;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;
  if (Precision::IsNegativeInfinite(Vdeb)) V1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Vfin)) V2 = Precision::Infinite();

  // creation de la generatrice : Generatrix 
  Handle(Geom_Line) Ligne = 
    new Geom_Line (gp_Pnt(start->Cylinder().Radius(), 0.0, 0.0), 
		   gp_Dir(0.0, 0.0, 1.0));
  GeomToIGES_GeomCurve GC(*this);
  Handle(IGESData_IGESEntity) Generatrix = GC.TransferCurve( Ligne, V1, V2);
  gp_Pnt gen1 = Ligne->Value(V1);
  gp_Pnt gen2 = Ligne->Value(V2);
  TheLength = gen1.Distance(gen2);
  

  // creation de l`axe : Axis .
  Handle(IGESGeom_Line) Axis = new IGESGeom_Line;
  //#30 rln 19.10.98 IGES axis = reversed CAS.CADE axis
  //Axis->Init(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(0.0, 0.0, 1.0/GetUnit()));
  //Surf->Init (Axis, Generatrix, U1, U2);
  Axis->Init(gp_XYZ (0, 0, 1.), gp_XYZ (0, 0, 0));  
  Surf->Init (Axis, Generatrix, 2 * M_PI - U2, 2 * M_PI - U1);


  // creation de la Trsf (#124)
  // il faut tenir compte de l`unite pour la matrice de transformation
  // (partie translation).
  IGESConvGeom_GeomBuilder Build;
  Standard_Real xloc,yloc,zloc;
  start->Cylinder().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc, yloc, zloc);
  gp_Ax3 Pos = start->Cylinder().Position();
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);
  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;
    TMat = Build.MakeTransformation(GetUnit());
    Surf->InitTransf(TMat);
  }
  res = Surf;
  return res;

}


//=============================================================================
// Transfer des Entites ConicalSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_ConicalSurface)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  The "ZAxis" is the symmetry axis of the ConicalSurface, 
  //  it gives the direction of increasing parametric value V.
  //  The apex of the surface is on the negative side of this axis.
  //  The parametrization range is  :
  //     U [0, 2*PI],  V ]-infinite, + infinite[
  //  The "XAxis" and the "YAxis" define the placement plane of the 
  //  surface (Z = 0, and parametric value V = 0)  perpendicular to 
  //  the symmetry axis. The "XAxis" defines the origin of the 
  //  parameter U = 0.  The trigonometric sense gives the positive 
  //  orientation for the parameter U.


  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }
  Handle(IGESGeom_SurfaceOfRevolution) Surf = new IGESGeom_SurfaceOfRevolution;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;
  if (Precision::IsNegativeInfinite(Vdeb)) V1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Vfin)) V2 = Precision::Infinite();

  // creation de la generatrice : Generatrix
  Handle(Geom_Line) Ligne = 
    new Geom_Line( gp_Pnt(start->Cone().RefRadius(), 0.0, 0.0), 
		   gp_Dir(sin(start->Cone().SemiAngle()), 0.,
			  cos(start->Cone().SemiAngle())));
  GeomToIGES_GeomCurve GC(*this);
  Handle(IGESData_IGESEntity) Generatrix = GC.TransferCurve( Ligne, V1, V2);
  gp_Pnt gen1 = Ligne->Value(V1);
  gp_Pnt gen2 = Ligne->Value(V2);
//  TheLength = gen1.Distance(gen2)*Cos(start->Cone().SemiAngle());
  TheLength = gen1.Distance(gen2);

  // creation de l`axe : Axis .
  Handle(IGESGeom_Line) Axis = new IGESGeom_Line;
  //#30 rln 19.10.98 IGES axis = reversed CAS.CADE axis
  //Axis->Init(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(0.0, 0.0, 1.0/GetUnit()));
  //Surf->Init (Axis, Generatrix, U1, U2);
  Axis->Init(gp_XYZ (0, 0, 1.), gp_XYZ (0, 0, 0));  
  Surf->Init (Axis, Generatrix, 2 * M_PI - U2, 2 * M_PI - U1);


  // creation de la Trsf (#124)
  // il faut tenir compte de l`unite pour la matrice de transformation
  // (partie translation).
  IGESConvGeom_GeomBuilder Build;
  Standard_Real xloc,yloc,zloc;
  start->Cone().Location().Coord(xloc,yloc,zloc);
  gp_Pnt Loc;
  Loc.SetCoord(xloc, yloc, zloc);
  gp_Ax3 Pos = start->Cone().Position();
  Pos.SetLocation(Loc);
  Build.SetPosition(Pos);
  if (!Build.IsIdentity()){
    Handle(IGESGeom_TransformationMatrix) TMat = 
      new IGESGeom_TransformationMatrix;
    TMat = Build.MakeTransformation(GetUnit());
    Surf->InitTransf(TMat);
  }
  res = Surf;
  return res;

}


//=============================================================================
// Transfer des Entites SphericalSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_SphericalSurface)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  The center of the sphere is the "Location" point of the local
  //  coordinate system.
  //  The V isoparametric curves of the surface  are defined by 
  //  the section of the spherical surface with plane parallel to the
  //  plane (Location, XAxis, YAxis). This plane defines the origin of
  //  parametrization V.
  //  The U isoparametric curves of the surface are defined by the 
  //  section of the spherical surface with plane obtained by rotation
  //  of the plane (Location, XAxis, ZAxis) around ZAxis. This plane
  //  defines the origin of parametrization u.
  //  The parametrization range is  U [0, 2*PI],  V [- PI/2, + PI/2]

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_SurfaceOfRevolution) Surf = new IGESGeom_SurfaceOfRevolution;

  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;

  // creation de la generatrice : Generatrix (1/2 cercle)
  gp_Ax2 Axe(gp::Origin(), -gp::DY(), gp::DX());
  Handle(Geom_Circle) Cercle = 
    new Geom_Circle(Axe, start->Sphere().Radius());
  GeomToIGES_GeomCurve GC(*this);
  Handle(IGESData_IGESEntity) Gen = GC.TransferCurve( Cercle, V1, V2);

  // creation de l`axe : Axis .
  Handle(IGESGeom_Line) Axis = new IGESGeom_Line;
  //#30 rln 19.10.98 IGES axis = reversed CAS.CADE axis
  //Axis->Init(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(0.0, 0.0, 1.0/GetUnit()));
  Axis->Init(gp_XYZ (0, 0, 1.), gp_XYZ (0, 0, 0));  

  if ( Gen->IsKind(STANDARD_TYPE(IGESGeom_CircularArc))) {
    //#30 rln 19.10.98 Surf->Init (Axis, Gen, U1, U2);
    Surf->Init (Axis, Gen, 2 * M_PI - U2, 2 * M_PI - U1);
    IGESConvGeom_GeomBuilder Build;
    Standard_Real xloc,yloc,zloc;
    start->Sphere().Location().Coord(xloc,yloc,zloc);
    gp_Pnt Loc;
    Loc.SetCoord(xloc, yloc, zloc);
    gp_Ax3 Pos = start->Sphere().Position();
    Pos.SetLocation(Loc);
    Build.SetPosition(Pos);
    if (!Build.IsIdentity()){    
      Handle(IGESGeom_TransformationMatrix) TMat = 
	new IGESGeom_TransformationMatrix;
      TMat = Build.MakeTransformation(GetUnit());
      Surf->InitTransf(TMat);
    }
  }
  res = Surf;
  return res;
}


//=============================================================================
// Transfer des Entites ToroidalSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_ToroidalSurface)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  The "Location point" of the axis placement is the center 
  //  of the surface.
  //  The plane (Location, XAxis, ZAxis) defines the origin of the
  //  parametrization U. The plane (Location, XAxis, YAxis)
  //  defines the origin of the parametrization V.
  //  The parametrization range is  U [0, 2*PI],  V [0, 2*PI]


  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_SurfaceOfRevolution) Surf = new IGESGeom_SurfaceOfRevolution;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;

  // creation de la generatrice : Generatrix (cercle)
  gp_Ax2 Axe = gp_Ax2(gp_Pnt((start->Torus().MajorRadius()), 0., 0.),
		      -gp::DY(), gp::DX());
  Handle(Geom_Circle) Cercle = 
    new Geom_Circle(Axe, start->Torus().MinorRadius());
  GeomToIGES_GeomCurve GC(*this);
  Handle(IGESData_IGESEntity) Gen = GC.TransferCurve( Cercle, V1, V2);
  
  // creation de l`axe : Axis .
  Handle(IGESGeom_Line) Axis = new IGESGeom_Line;
  //#30 rln 19.10.98 IGES axis = reversed CAS.CADE axis
  //Axis->Init(gp_XYZ(0.0, 0.0, 0.0), gp_XYZ(0.0, 0.0, 1.0/GetUnit()));
  Axis->Init(gp_XYZ (0, 0, 1.), gp_XYZ (0, 0, 0));  

//:l6 abv: CTS22022: writing full tori:  if ( Gen->IsKind(STANDARD_TYPE(IGESGeom_CircularArc))) {
    //#30 rln 19.10.98 Surf->Init (Axis, Gen, U1, U2);
    Surf->Init (Axis, Gen, 2 * M_PI - U2, 2 * M_PI - U1);
    IGESConvGeom_GeomBuilder Build;
/* //:l6: useless
    Standard_Real xloc,yloc,zloc;
    start->Torus().Location().Coord(xloc,yloc,zloc);
    gp_Pnt Loc;
    Loc.SetCoord(xloc, yloc, zloc);
*/
    gp_Ax3 Pos = start->Torus().Position();
//:l6    Pos.SetLocation(Loc);
    Build.SetPosition(Pos);
    if (!Build.IsIdentity()){
      Handle(IGESGeom_TransformationMatrix) TMat = 
	new IGESGeom_TransformationMatrix;
      TMat = Build.MakeTransformation(GetUnit());
      Surf->InitTransf(TMat);
    }
//:l6  }
  res = Surf;
  return res;

}

//=============================================================================
// Transfer des Entites SweptSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_SweptSurface)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  if (start->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    DeclareAndCast(Geom_SurfaceOfLinearExtrusion, Extrusion, start);
    res = TransferSurface(Extrusion, Udeb, Ufin, Vdeb, Vfin);
  }
  else if (start->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    DeclareAndCast(Geom_SurfaceOfRevolution, Revolution, start);
    res = TransferSurface(Revolution, Udeb, Ufin, Vdeb, Vfin);
  }

  return res;

}

//=============================================================================
// Transfer des Entites SurfaceOfLinearExtrusion de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_SurfaceOfLinearExtrusion)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  This surface is obtained by sweeping a curve in a given direction.
  //  The parametrization range for the parameter U is defined with the
  //  referenced curve.
  //  The parametrization range for the parameter V is 
  //  ]-infinite, + infinite[
  //  The position of the curve gives the origin for the parameter V.


  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_TabulatedCylinder) Surf = new IGESGeom_TabulatedCylinder;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;
  if (Precision::IsNegativeInfinite(Vdeb)) V1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Vfin)) V2 = Precision::Infinite();

  // added by skl 18.07.2005 for OCC9490
  Standard_Real UF,UL,VF,VL;
  start->Bounds(UF,UL,VF,VL);
  U1=UF;
  U2=UL;

  Handle(Geom_Curve) TheCurve = start->BasisCurve();

  //dans IGES l'origine de la generatrice est identique a l'origine 
  //de la directrice , il faut translater la courbe si les deux 
  //points ne sont pas confondus dans Geom et donc la copier !!!!!!!
  gp_Pnt TheEnd = start->Value(U1,V2);
  Standard_Real Xe, Ye, Ze;
  TheEnd.Coord(Xe, Ye, Ze);
  gp_XYZ End = gp_XYZ (Xe/GetUnit(), Ye/GetUnit(), Ze/GetUnit());

  GeomToIGES_GeomCurve GC(*this);
// commented by skl 18.07.2005 for OCC9490
  Handle(Geom_Curve) CopyCurve;
  if ( Abs(V1) > Precision::Confusion()) {
   CopyCurve = Handle(Geom_Curve)::DownCast
     (TheCurve->Translated (start->Value(U1,0.), start->Value(U1,V1)));
  }
  else {
    CopyCurve = TheCurve;
  }
  //Handle(IGESData_IGESEntity) Directrix = GC.TransferCurve( CopyCurve, V1, V2);
  Handle(IGESData_IGESEntity) Directrix = GC.TransferCurve( CopyCurve, U1, U2);
  //Handle(IGESData_IGESEntity) Directrix = GC.TransferCurve( TheCurve, U1, U2);
  //gp_Pnt gen1 = start->Value(U1,V1);
  //TheLength = gen1.Distance(TheEnd);

  Surf->Init (Directrix, End);
  res = Surf;
  return res;

}

//=============================================================================
// Transfer des Entites SurfaceOfRevolution de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_SurfaceOfRevolution)& start, const Standard_Real Udeb, 
 const Standard_Real Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  The surface is obtained by rotating a curve a complete revolution
  //  about an axis. The curve and the axis must be in the same plane.
  //  For a complete surface of revolution the parametric range is
  //  0 <= U <= 2*PI.
  //  The parametric range for V is defined with the revolved curve.
  //  The origin of the U parametrization is given by the position
  //  of the revolved curve (reference). The direction of the revolution
  //  axis defines the positive sense of rotation (trigonometric sense)
  //  corresponding to the increasing of the parametric value U.
  //  The derivatives are always defined for the u direction.
  //  For the v direction the definition of the derivatives depends on
  //  the degree of continuity of the referenced curve.

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_SurfaceOfRevolution) Surf = new IGESGeom_SurfaceOfRevolution;
  Standard_Real U1 = Udeb;
  Standard_Real U2 = Ufin;
  Standard_Real V1 = Vdeb;
  Standard_Real V2 = Vfin;
  if (Precision::IsNegativeInfinite(Vdeb)) V1 = -Precision::Infinite();
  if (Precision::IsPositiveInfinite(Vfin)) V2 = Precision::Infinite();

  // creation de la generatrice : Generatrix 
  Handle(Geom_Curve) Curve = start->BasisCurve();
  GeomToIGES_GeomCurve GC(*this);
  Handle(IGESData_IGESEntity) Generatrix = GC.TransferCurve( Curve, V1, V2);
  //pdn BUC184: decoding a trimmed curve
  while( Curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) aTrCurve = Handle(Geom_TrimmedCurve)::
      DownCast(Curve);
    Curve = aTrCurve->BasisCurve();
  }
  
  if ( Curve->IsKind(STANDARD_TYPE(Geom_Line))) {
    DeclareAndCast(Geom_Line, Line, Curve);
    gp_Pnt gen1 = Line->Value(V1);
    gp_Pnt gen2 = Line->Value(V2);
    TheLength = gen1.Distance(gen2);
  }

  // creation de l`axe : Axis .
  Handle(IGESGeom_Line) Axis = new IGESGeom_Line;
  gp_Ax1 Axe = start->Axis();
  Standard_Real X1,Y1,Z1,X2,Y2,Z2;
  Axe.Location().Coord(X1,Y1,Z1);
  Axe.Direction().Coord(X2,Y2,Z2);

  //#30 rln 19.10.98 IGES axis = reversed CAS.CADE axis
  //Axis->Init(gp_XYZ(X1/GetUnit(),Y1/GetUnit(),Z1/GetUnit()),
  //	     gp_XYZ(X2/GetUnit(),Y2/GetUnit(),Z2/GetUnit()));
  //#36 rln 27.10.98 BUC60328 face 7
  Axis->Init(gp_XYZ(X1/GetUnit(),Y1/GetUnit(),Z1/GetUnit()),
	     gp_XYZ( (X1 - X2) / GetUnit(), (Y1 - Y2) / GetUnit(), (Z1 - Z2) / GetUnit()));

  Surf->Init (Axis, Generatrix, 2 * M_PI - U2, 2 * M_PI - U1);
  res = Surf;
  return res;

}


//=============================================================================
// Transfer des Entites OffsetSurface de Geom vers IGES
// TransferSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSurface
( const Handle(Geom_OffsetSurface)& start, const Standard_Real Udeb, const Standard_Real 
 Ufin, const Standard_Real Vdeb, const Standard_Real Vfin)
{
  //  An offset surface is a surface at constant distance
  //  (Offset) from a basis surface. The distance may be positive
  //  or negative to the preferred side of the surface.
  //  The positive side is defined by the cross product D1u ^ D1v
  //  where D1u and D1v are the tangent vectors of the basis
  //  surface in the U and V parametric directions. The previous 
  //  cross product defines the normal direction to the basis
  //  surface.

  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESGeom_OffsetSurface) Surf = new IGESGeom_OffsetSurface;
  Handle(Geom_Surface) TheSurf = start->BasisSurface();
  Standard_Real U1, U2, V1, V2 , Um, Vm;
  start->Bounds (U1, U2, V1, V2);
  Um = (U1 + U2 ) /2.;
  Vm = (V1 + V2 ) /2.;
  Handle(IGESData_IGESEntity) Surface = TransferSurface
    (TheSurf, Udeb, Ufin, Vdeb, Vfin);
  Standard_Real Distance = start->Offset()/GetUnit();
  GeomLProp_SLProps Prop = GeomLProp_SLProps 
    (TheSurf, Um, Vm, 1, Precision::Confusion());
  gp_Dir Dir = Prop.Normal();
  Standard_Real Xd, Yd, Zd;
  Dir.Coord(Xd, Yd, Zd);
  gp_XYZ Indicator = gp_XYZ(Xd/GetUnit(), Yd/GetUnit(), Zd/GetUnit());

  Surf-> Init (Indicator, Distance, Surface);
  res = Surf;
  return res;

}


//=============================================================================
// Transfer des Entites Plane de Geom vers IGESSolid
// TransferPlaneSurface
//=============================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferPlaneSurface(const Handle(Geom_Plane)& start,
									 const Standard_Real /*Udeb*/,
									 const Standard_Real /*Ufin*/,
									 const Standard_Real /*Vdeb*/,
									 const Standard_Real /*Vfin*/)
{
  //  The parametrization range is  U, V  ]- infinite, + infinite[
  //  The local coordinate system of the plane is defined with
  //  an axis placement two axis.

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESSolid_PlaneSurface) Plsurf = new IGESSolid_PlaneSurface;
  GeomToIGES_GeomPoint GP(*this);

  gp_Pln aPln = start->Pln();

  Handle(Geom_CartesianPoint) mypoint = new Geom_CartesianPoint (aPln.Location()); 
  Handle(IGESGeom_Point) aLocation = GP.TransferPoint(mypoint);

  Handle(IGESGeom_Direction) aNormal = new IGESGeom_Direction;
  aNormal->Init (aPln.Axis().Direction().XYZ());

  Handle(IGESGeom_Direction) aRefDir = new IGESGeom_Direction;
  aRefDir->Init (aPln.XAxis().Direction().XYZ());

  Plsurf->Init (aLocation, aNormal, aRefDir);
  res = Plsurf;
  return res;

}

//=======================================================================
//function : TransferCylindricaSurface
//purpose  : 
//=======================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferCylindricalSurface(const Handle(Geom_CylindricalSurface)& start,
									       const Standard_Real /*Udeb*/,
									       const Standard_Real /*Ufin*/,
									       const Standard_Real /*Vdeb*/,
									       const Standard_Real /*Vfin*/)
{

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESSolid_CylindricalSurface) CylSurf = new IGESSolid_CylindricalSurface;
  GeomToIGES_GeomPoint GP(*this);

  gp_Cylinder aCyl = start->Cylinder();

  Handle(Geom_CartesianPoint) mypoint = new Geom_CartesianPoint (aCyl.Location()); 
  Handle(IGESGeom_Point) aLocation = GP.TransferPoint(mypoint);

  Handle(IGESGeom_Direction) anAxis = new IGESGeom_Direction;
  anAxis->Init (aCyl.Axis().Direction().XYZ());

  Handle(IGESGeom_Direction) aRefDir = new IGESGeom_Direction;
  aRefDir->Init (aCyl.XAxis().Direction().XYZ());

  Standard_Real aRadius = aCyl.Radius() / GetUnit();

  CylSurf->Init (aLocation, anAxis, aRadius, aRefDir);
  res = CylSurf;
  return res;
}


//=======================================================================
//function : TransferConicalSurface
//purpose  : 
//=======================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferConicalSurface(const Handle(Geom_ConicalSurface)& start,
                                                                           const Standard_Real /*Udeb*/,
                                                                           const Standard_Real /*Ufin*/,
                                                                           const Standard_Real /*Vdeb*/,
                                                                           const Standard_Real /*Vfin*/)
{

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESSolid_ConicalSurface) ConSurf = new IGESSolid_ConicalSurface;
  GeomToIGES_GeomPoint GP(*this);

  gp_Cone Con = start->Cone();
  Standard_Real aRadius = Con.RefRadius() / GetUnit();
  Standard_Real angle  = Con.SemiAngle();
  gp_Ax1 Axe = Con.Axis();
  gp_Ax1 XAxe = Con.XAxis();
  gp_Dir XDir = XAxe.Direction();

  Handle(Geom_CartesianPoint) mypoint = new Geom_CartesianPoint(Con.Location());
  if(angle < 0.) {
    gp_Pnt pnt = mypoint->Pnt();
    mypoint->SetPnt(Con.Apex().XYZ()*2-pnt.XYZ());
    angle = -angle;
    XDir.Reverse();
  }
  Handle(IGESGeom_Point) aLocation = GP.TransferPoint(mypoint);

  Handle(IGESGeom_Direction) anAxis = new IGESGeom_Direction;
  anAxis->Init (Axe.Direction().XYZ());

  Handle(IGESGeom_Direction) aRefDir = new IGESGeom_Direction;
  aRefDir->Init (XDir.XYZ());

  ConSurf->Init (aLocation, anAxis, aRadius, angle*180./M_PI, aRefDir);
  res = ConSurf;
  return res;
}


//=======================================================================
//function : TransferSphericalSurface
//purpose  : 
//=======================================================================

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferSphericalSurface(const Handle(Geom_SphericalSurface)& start,
                                                                             const Standard_Real /*Udeb*/,
                                                                             const Standard_Real /*Ufin*/,
                                                                             const Standard_Real /*Vdeb*/,
                                                                             const Standard_Real /*Vfin*/)
{

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESSolid_SphericalSurface) SphSurf = new IGESSolid_SphericalSurface;
  GeomToIGES_GeomPoint GP(*this);

  gp_Sphere aSph = start->Sphere();

  Handle(Geom_CartesianPoint) mypoint = new Geom_CartesianPoint(aSph.Location()); 
  Handle(IGESGeom_Point) aLocation = GP.TransferPoint(mypoint);

  Handle(IGESGeom_Direction) anAxis = new IGESGeom_Direction;
  anAxis->Init (aSph.Position().Axis().Direction().XYZ());

  Handle(IGESGeom_Direction) aRefDir = new IGESGeom_Direction;
  aRefDir->Init (aSph.XAxis().Direction().XYZ());

  Standard_Real aRadius = aSph.Radius() / GetUnit();
  
  SphSurf->Init (aLocation, aRadius, anAxis, aRefDir);
  res = SphSurf;
  return res;
}

Handle(IGESData_IGESEntity) GeomToIGES_GeomSurface::TransferToroidalSurface(const Handle(Geom_ToroidalSurface)& start,
                                                                            const Standard_Real /*Udeb*/,
                                                                            const Standard_Real /*Ufin*/,
                                                                            const Standard_Real /*Vdeb*/,
                                                                            const Standard_Real /*Vfin*/)
{

  Handle(IGESData_IGESEntity) res;
  TheLength = 1;
  if (start.IsNull()) {
    return res;
  }

  Handle(IGESSolid_ToroidalSurface) TorSurf = new IGESSolid_ToroidalSurface;
  GeomToIGES_GeomPoint GP(*this);

  gp_Torus aTor = start->Torus();

  Handle(Geom_CartesianPoint) mypoint = new Geom_CartesianPoint (aTor.Location()); 
  Handle(IGESGeom_Point) aLocation = GP.TransferPoint(mypoint);

  Handle(IGESGeom_Direction) anAxis = new IGESGeom_Direction;
  anAxis->Init (aTor.Axis().Direction().XYZ());

  Handle(IGESGeom_Direction) aRefDir = new IGESGeom_Direction;
  aRefDir->Init (aTor.XAxis().Direction().XYZ());

  Standard_Real aMajor = aTor.MajorRadius() / GetUnit();
  Standard_Real aMinor = aTor.MinorRadius() / GetUnit();
  
  TorSurf->Init (aLocation, anAxis, aMajor, aMinor, aRefDir);
  res = TorSurf;
  return res;
}


//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Real GeomToIGES_GeomSurface::Length() const
{  return TheLength;  }

//=======================================================================
//function : GetBRepMode
//purpose  : 
//=======================================================================

Standard_Boolean GeomToIGES_GeomSurface::GetBRepMode() const
{
  return myBRepMode;
}

//=======================================================================
//function : SetBRepMode
//purpose  : 
//=======================================================================

void GeomToIGES_GeomSurface::SetBRepMode(const Standard_Boolean flag)
{
  myBRepMode = flag;
}

//=======================================================================
//function : GetAnalyticMode
//purpose  : 
//=======================================================================

Standard_Boolean GeomToIGES_GeomSurface::GetAnalyticMode() const
{
  return myAnalytic;
}

void GeomToIGES_GeomSurface::SetAnalyticMode(const Standard_Boolean flag)
{
  myAnalytic = flag;
}

