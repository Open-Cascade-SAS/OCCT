// Created on: 1993-03-10
// Created by: JCV
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


#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

#define  POLES    (poles->Array2())
#define  WEIGHTS  (weights->Array2())
#define  UKNOTS   (uknots->Array1())
#define  VKNOTS   (vknots->Array1())
#define  UFKNOTS  (ufknots->Array1())
#define  VFKNOTS  (vfknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

typedef Geom_SurfaceOfLinearExtrusion         SurfaceOfLinearExtrusion;
typedef Geom_Curve                            Curve;
typedef gp_Dir  Dir;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_Vec  Vec;
typedef gp_XYZ  XYZ;

static void LocateSide(const Standard_Real U,
		       const Standard_Integer Side,
		       const Handle(Geom_BSplineCurve)& BSplC,
		       const Standard_Integer NDir,
		       gp_Pnt& P,
		       gp_Vec& D1U,
		       gp_Vec& D2U,
		       gp_Vec& D3U) 
{ 
  Standard_Integer Ideb, Ifin;
  Standard_Real ParTol=Precision::PConfusion()/2;
  BSplC->Geom_BSplineCurve::LocateU(U,ParTol,Ideb,Ifin,Standard_False);   
  if(Side == 1)
    {
      if(Ideb<1) Ideb=1;
      if ((Ideb>=Ifin))  Ifin = Ideb+1;
    }else
  if(Side ==-1)
	{ 
	  if(Ifin > BSplC -> NbKnots()) Ifin=BSplC->NbKnots();
	  if ((Ideb>=Ifin))  Ideb = Ifin-1;
	}

  switch(NDir)
       {
       case 0 :  BSplC->Geom_BSplineCurve::LocalD0(U,Ideb,Ifin,P); break;
       case 1 :  BSplC->Geom_BSplineCurve::LocalD1(U,Ideb,Ifin,P,D1U); break;
       case 2 :  BSplC->Geom_BSplineCurve::LocalD2(U,Ideb,Ifin,P,D1U,D2U); break;
       case 3 :  BSplC->Geom_BSplineCurve::LocalD3(U,Ideb,Ifin,P,D1U,D2U,D3U);break;
	}
   
}

static gp_Vec LocateSideN(const Standard_Real U,
			  const Standard_Integer Side,
			  const Handle(Geom_BSplineCurve)& BSplC,
			  const Standard_Integer Nu,
//			  const	Standard_Integer  Nv ) 
			  const	Standard_Integer ) 
{ 
  Standard_Integer Ideb, Ifin;
  Standard_Real ParTol=Precision::PConfusion()/2;
  BSplC->Geom_BSplineCurve::LocateU(U,ParTol,Ideb,Ifin,Standard_False);   
  if(Side == 1)
    {
      if(Ideb<1) Ideb=1;
      if ((Ideb>=Ifin))  Ifin = Ideb+1;
    }else
      if(Side ==-1)
	{ 
	  if(Ifin > BSplC -> NbKnots()) Ifin=BSplC->NbKnots();
	  if ((Ideb>=Ifin))  Ideb = Ifin-1;
	}
  return BSplC->Geom_BSplineCurve::LocalDN(U,Ideb,Ifin,Nu);
}



//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_SurfaceOfLinearExtrusion::Copy () const 
{
  
  Handle(Geom_SurfaceOfLinearExtrusion) Sr;
  Sr = new SurfaceOfLinearExtrusion (basisCurve, direction);
  return Sr;
}


//=======================================================================
//function : Geom_SurfaceOfLinearExtrusion
//purpose  : 
//=======================================================================

Geom_SurfaceOfLinearExtrusion::Geom_SurfaceOfLinearExtrusion 
  ( const Handle(Geom_Curve)& C, 
    const Dir& V) {

   basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());  // Copy 10-03-93
   direction  = V;
   smooth     = C->Continuity();
 }


//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::UReverse () { 

  basisCurve->Reverse(); 
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfLinearExtrusion::UReversedParameter(const Standard_Real U) const {

  return basisCurve->ReversedParameter(U);
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::VReverse () {

  direction.Reverse(); 
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_SurfaceOfLinearExtrusion::VReversedParameter( const Standard_Real V) const {

  return (-V);
}


//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::SetDirection (const Dir& V) {

   Handle(Geom_Curve) C;
   direction = V;
   C         = basisCurve;
 }


//=======================================================================
//function : SetBasisCurve
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::SetBasisCurve (const Handle(Geom_Curve)& C) {

   smooth = C->Continuity();
   basisCurve = Handle(Geom_Curve)::DownCast(C->Copy());  // Copy 10-03-93
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::Bounds ( Standard_Real& U1, 
					     Standard_Real& U2,
					     Standard_Real& V1, 
					     Standard_Real& V2 ) const {

  V1 = -Precision::Infinite();  V2 = Precision::Infinite();
  U1 = basisCurve->FirstParameter();  U2 = basisCurve->LastParameter();
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::D0 (const Standard_Real U, const Standard_Real V, 
					      Pnt& P               )  const {
  
  XYZ Pxyz = direction.XYZ();
  Pxyz.Multiply (V);
  Pxyz.Add (basisCurve->Value (U).XYZ());
  P.SetXYZ(Pxyz);      
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::D1 ( const Standard_Real U  , 
					 const Standard_Real V  , 
					       Pnt& P  , 
					       Vec& D1U, Vec& D1V) const {

   basisCurve->D1 (U, P, D1U);
   D1V = direction;
   XYZ Pxyz = direction.XYZ();
   Pxyz.Multiply (V);
   Pxyz.Add (P.XYZ());
   P.SetXYZ (Pxyz);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::D2 ( const Standard_Real U  , 
					 const Standard_Real V  ,
					       Pnt& P  ,
					       Vec& D1U, Vec& D1V,
					       Vec& D2U, Vec& D2V, Vec& D2UV)
const {

   basisCurve->D2 (U, P, D1U, D2U);
   D1V = direction;
   D2V.SetCoord  (0.0, 0.0, 0.0);
   D2UV.SetCoord (0.0, 0.0, 0.0);
   XYZ Pxyz = direction.XYZ();
   Pxyz.Multiply (V);
   Pxyz.Add (P.XYZ());
   P.SetXYZ (Pxyz);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::D3 
  ( const Standard_Real U, const Standard_Real V,
    Pnt& P,
    Vec& D1U, Vec& D1V,
    Vec& D2U, Vec& D2V, Vec& D2UV,
    Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV ) const {


   basisCurve->D3 (U, P, D1U, D2U, D3U);
   D1V = direction;
   D2V.SetCoord   (0.0, 0.0, 0.0);
   D3V.SetCoord   (0.0, 0.0, 0.0);
   D3UUV.SetCoord (0.0, 0.0, 0.0);
   D3UVV.SetCoord (0.0, 0.0, 0.0);
   D2UV.SetCoord  (0.0, 0.0, 0.0);
   XYZ Pxyz = direction.XYZ();
   Pxyz.Multiply (V);
   Pxyz.Add (P.XYZ());
   P.SetXYZ (Pxyz);
}
//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_SurfaceOfLinearExtrusion::DN 
  ( const Standard_Real     U, const Standard_Real , 
    const Standard_Integer Nu, const Standard_Integer Nv) const {

  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv <0, " ");
  if (Nu == 0 && Nv == 1)  return Vec (direction);
  else if (Nv == 0)        return basisCurve->DN (U, Nu);
  else                     return Vec (0.0, 0.0, 0.0);
}


//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::LocalD0 (const Standard_Real    U,
					     const Standard_Real    V, 
					     const Standard_Integer USide,
					     gp_Pnt&          P     )  const
{ 
  if((USide != 0) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    { 
      Vec D1U,D2U,D3U;
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

      LocateSide(U,USide,BSplC,0,P,D1U,D2U,D3U);
      XYZ Pxyz = direction.XYZ();
      Pxyz.Multiply (V);
      Pxyz.Add (P.XYZ());
      P.SetXYZ (Pxyz);
    }
  else D0(U,V,P);
}        

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::LocalD1 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer USide, 
				         gp_Pnt&          P,
				         gp_Vec&          D1U, 
				         gp_Vec&          D1V)     const
{ 
  if((USide != 0) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    { 
      Vec D2U,D3U;
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

	  LocateSide(U,USide,BSplC,1,P,D1U,D2U,D3U);
	  D1V = direction;
	  XYZ Pxyz = direction.XYZ();
	  Pxyz.Multiply (V);
	  Pxyz.Add (P.XYZ());
	  P.SetXYZ (Pxyz);
    }
  else D1(U,V,P,D1U,D1V);
}

//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::LocalD2 (const Standard_Real    U,
				   const Standard_Real    V,
				   const Standard_Integer USide,
				         gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V,
				         gp_Vec&          D2U,
				         gp_Vec&          D2V,
				         gp_Vec&          D2UV) const
{
 if((USide != 0) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    { 
      Vec D3U;
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

	  LocateSide(U,USide,BSplC,2,P,D1U,D2U,D3U);
	  D1V = direction;
	  D2V.SetCoord  (0.0, 0.0, 0.0);
	  D2UV.SetCoord (0.0, 0.0, 0.0);
	  XYZ Pxyz = direction.XYZ();
	  Pxyz.Multiply (V);
	  Pxyz.Add (P.XYZ());
	  P.SetXYZ (Pxyz); 
    }
  else D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}

//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::LocalD3 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer USide, 
					 gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V, 
				         gp_Vec&          D2U, 
				         gp_Vec&          D2V, 
				         gp_Vec&          D2UV, 
				         gp_Vec&          D3U,
				         gp_Vec&          D3V,
				         gp_Vec&          D3UUV,
				         gp_Vec&          D3UVV) const
{
  if((USide != 0) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    { 
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);

	  LocateSide(U,USide,BSplC,3,P,D1U,D2U,D3U);
	  D1V = direction;
	  D2V.SetCoord   (0.0, 0.0, 0.0);
	  D3V.SetCoord   (0.0, 0.0, 0.0);
	  D3UUV.SetCoord (0.0, 0.0, 0.0);
	  D3UVV.SetCoord (0.0, 0.0, 0.0);
	  D2UV.SetCoord  (0.0, 0.0, 0.0);
	  XYZ Pxyz = direction.XYZ();
	  Pxyz.Multiply (V);
	  Pxyz.Add (P.XYZ());
	  P.SetXYZ (Pxyz);
    }
  else D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);

}

//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

gp_Vec Geom_SurfaceOfLinearExtrusion::LocalDN  (const Standard_Real    U, 
				      const Standard_Real    V,
				      const Standard_Integer USide,
				      const Standard_Integer Nu,
				      const Standard_Integer Nv) const
{
  Standard_RangeError_Raise_if (Nu + Nv < 1 || Nu < 0 || Nv <0, " ");
  if (Nu == 0 && Nv == 1)  return Vec (direction);
  else if (Nv == 0) {
    if((USide != 0) && basisCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) { 
      Handle( Geom_BSplineCurve) BSplC;
      BSplC= Handle(Geom_BSplineCurve)::DownCast(basisCurve);
      return LocateSideN(U,USide,BSplC,Nu,Nv);
    }
    else
      return DN(U,V,Nu,Nv);
  }      
  return Vec (0.0, 0.0, 0.0);
}

//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfLinearExtrusion::UIso (const Standard_Real U) const {

  Handle(Geom_Line) L; 
  L = new Geom_Line (basisCurve->Value (U), direction);
  return L;
}


//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_SurfaceOfLinearExtrusion::VIso (const Standard_Real V) const {
    
  Vec Vdir (direction);
  Vdir.Multiply (V);
  Handle(Geom_Curve) C;
  C = Handle(Geom_Curve)::DownCast(basisCurve->Translated(Vdir));
  return C;
}


//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsCNu (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0, " ");
  return basisCurve->IsCN (N);
}


//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsCNv (const Standard_Integer ) const {

  return Standard_True;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::Transform (const Trsf& T) {

   direction.Transform   (T);
   basisCurve->Transform (T);
}



//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsUClosed () const { 

  return basisCurve->IsClosed ();
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsUPeriodic () const { 

  return basisCurve->IsPeriodic ();
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsVClosed () const  {

  return Standard_False; 
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_SurfaceOfLinearExtrusion::IsVPeriodic () const { 

  return Standard_False;
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_SurfaceOfLinearExtrusion::TransformParameters(Standard_Real& U,
							Standard_Real& V,
							const gp_Trsf& T) 
const
{
  U = basisCurve->TransformedParameter(U,T);
  if (!Precision::IsInfinite(V)) V *= Abs(T.ScaleFactor());
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_SurfaceOfLinearExtrusion::ParametricTransformation
(const gp_Trsf& T) const
{
  // transformation in the V Direction
  gp_GTrsf2d TV;
  gp_Ax2d Axis(gp::Origin2d(),gp::DX2d());
  TV.SetAffinity(Axis, Abs(T.ScaleFactor()));
  // transformation in the U Direction
  gp_GTrsf2d TU;
  Axis = gp_Ax2d(gp::Origin2d(),gp::DY2d());
  TU.SetAffinity(Axis, basisCurve->ParametricTransformation(T));
 
  return TU * TV;
}
