// Created on: 1993-03-09
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// xab : 30-Mar-95 introduced cache mechanism for surfaces
// xab : 21-Jun-95 in remove knots sync size of weights and poles
// pmn : 28-Jun-96 Distinction entre la continuite en U et V (bug PRO4625)
// pmn : 07-Jan-97 Centralisation des verif rational (PRO6834)
//       et ajout des InvalideCache() dans les SetPole* (PRO6833)
// pmn : 03-Feb-97 Prise en compte de la periode dans Locate(U/V) (PRO6963)
//                 + bon appel a LocateParameter (PRO6973).
// RBD : 15/10/98 ; Le cache est desormais defini sur [-1,1] (pro15537).

#define No_Standard_OutOfRange
#define No_Standard_DimensionError

#include <Geom_BSplineSurface.jxx>

#include <gp.hxx>
#include <BSplSLib.hxx>
#include <BSplCLib.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfXYZ.hxx>

#include <Geom_BSplineCurve.hxx>

#include <Geom_UndefinedDerivative.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Mutex.hxx>

#define  POLES    (poles->Array2())
#define  WEIGHTS  (weights->Array2())
#define  UKNOTS   (uknots->Array1())
#define  VKNOTS   (vknots->Array1())
#define  UFKNOTS  (ufknots->Array1())
#define  VFKNOTS  (vfknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsCNu
  (const Standard_Integer N) const
{
  Standard_RangeError_Raise_if (N < 0, " ");
  switch (Usmooth) {
  case GeomAbs_CN : return Standard_True;
  case GeomAbs_C0 : return N <= 0;
  case GeomAbs_G1 : return N <= 0;
  case GeomAbs_C1 : return N <= 1;
  case GeomAbs_G2 : return N <= 1;
  case GeomAbs_C2 : return N <= 2;
  case GeomAbs_C3 :
    return N <= 3 ? Standard_True :
           N <= udeg - BSplCLib::MaxKnotMult (umults->Array1(), umults->Lower() + 1, umults->Upper() - 1);
  default:
    return Standard_False;
  }
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsCNv
  (const Standard_Integer N) const
{
  Standard_RangeError_Raise_if (N < 0, " ");
  
  switch (Vsmooth) {
  case GeomAbs_CN : return Standard_True;
  case GeomAbs_C0 : return N <= 0;
  case GeomAbs_G1 : return N <= 0;
  case GeomAbs_C1 : return N <= 1;
  case GeomAbs_G2 : return N <= 1;
  case GeomAbs_C2 : return N <= 2;
  case GeomAbs_C3 :
    return N <= 3 ? Standard_True :
           N <= vdeg - BSplCLib::MaxKnotMult (vmults->Array1(), vmults->Lower() + 1, vmults->Upper() - 1);
  default:
    return Standard_False;
  }
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::D0(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt& P) const 
{
  Standard_Real  new_u(U), new_v(V);
  PeriodicNormalization(new_u, new_v);

  Geom_BSplineSurface* MySurface = (Geom_BSplineSurface *) this;
  Standard_Mutex::Sentry aSentry(MySurface->myMutex);

  if(!IsCacheValid(new_u, new_v))
     MySurface->ValidateCache(new_u, new_v);

 Standard_Real uparameter_11 = (2*ucacheparameter + ucachespanlenght)/2,
               uspanlenght_11 = ucachespanlenght/2,
               vparameter_11 = (2*vcacheparameter + vcachespanlenght)/2,
               vspanlenght_11 = vcachespanlenght/2 ; 
 if (cacheweights.IsNull()) {
      
   BSplSLib::CacheD0(new_u,
		     new_v,
		     udeg,
		     vdeg,
		     uparameter_11,
		     vparameter_11,
		     uspanlenght_11,
		     vspanlenght_11,
		     cachepoles->Array2(),
		     *((TColStd_Array2OfReal*) NULL),
		     P) ;
 }
 else {
   BSplSLib::CacheD0(new_u,
		     new_v,
		     udeg,
		     vdeg,
		     uparameter_11,
		     vparameter_11,
		     uspanlenght_11,
		     vspanlenght_11,
		     cachepoles->Array2(),
		     cacheweights->Array2(),
		     P) ;
 }
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::D1(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt& P,
                             gp_Vec& D1U,
                             gp_Vec& D1V) const
{
  Standard_Real  new_u(U), new_v(V);
  PeriodicNormalization(new_u, new_v);

  Geom_BSplineSurface* MySurface = (Geom_BSplineSurface *) this;
  Standard_Mutex::Sentry aSentry(MySurface->myMutex);

  if(!IsCacheValid(new_u, new_v))
     MySurface->ValidateCache(new_u, new_v);

  Standard_Real uparameter_11 = (2*ucacheparameter + ucachespanlenght)/2,
                uspanlenght_11 = ucachespanlenght/2,
                vparameter_11 = (2*vcacheparameter + vcachespanlenght)/2,
                vspanlenght_11 = vcachespanlenght/2 ;

  if (cacheweights.IsNull()) {
    
    BSplSLib::CacheD1(new_u,
		      new_v,
		      udeg,
		      vdeg,
		      uparameter_11,
		      vparameter_11,
		      uspanlenght_11,
		      vspanlenght_11,
		      cachepoles->Array2(),
		      *((TColStd_Array2OfReal*) NULL),
		      P,
		      D1U,
		      D1V) ;
  }
  else {
    
    BSplSLib::CacheD1(new_u,
		      new_v,
		      udeg,
		      vdeg,
		      uparameter_11,
		      vparameter_11,
		      uspanlenght_11,
		      vspanlenght_11,
		      cachepoles->Array2(),
		      cacheweights->Array2(),
		      P,
		      D1U,
		      D1V) ;
  }
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::D2 (const Standard_Real U, 
			      const Standard_Real V,
			            gp_Pnt&       P,
			            gp_Vec&       D1U,
			            gp_Vec&       D1V,
			            gp_Vec&       D2U,
			            gp_Vec&       D2V,
			            gp_Vec&       D2UV) const
{
  Standard_Real  new_u(U), new_v(V);
  PeriodicNormalization(new_u, new_v);

  Geom_BSplineSurface* MySurface = (Geom_BSplineSurface *) this;
  Standard_Mutex::Sentry aSentry(MySurface->myMutex);

  if(!IsCacheValid(new_u, new_v))
     MySurface->ValidateCache(new_u, new_v);

  Standard_Real uparameter_11 = (2*ucacheparameter + ucachespanlenght)/2,
                uspanlenght_11 = ucachespanlenght/2,
                vparameter_11 = (2*vcacheparameter + vcachespanlenght)/2,
                vspanlenght_11 = vcachespanlenght/2 ;
  if (cacheweights.IsNull()) {
      BSplSLib::CacheD2(new_u,
			new_v,
			udeg,
			vdeg,
			uparameter_11,
			vparameter_11,
			uspanlenght_11,
			vspanlenght_11,
			cachepoles->Array2(),
			*((TColStd_Array2OfReal*) NULL),
			P,
			D1U,
			D1V,
			D2U,
			D2UV,
			D2V); 
    }
    else {
      BSplSLib::CacheD2(new_u,
			new_v,
			udeg,
			vdeg,
			uparameter_11,
			vparameter_11,
			uspanlenght_11,
			vspanlenght_11,
			cachepoles->Array2(),
			cacheweights->Array2(),
			P,
			D1U,
			D1V,
			D2U,
			D2UV,
			D2V); 
    }
  }

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::D3 (const Standard_Real U, 
			      const Standard_Real V,
			            gp_Pnt&       P,
			            gp_Vec&       D1U,
			            gp_Vec&       D1V, 
			            gp_Vec&       D2U,
			            gp_Vec&       D2V,
			            gp_Vec&       D2UV,
			            gp_Vec&       D3U,
			            gp_Vec&       D3V,
			            gp_Vec&       D3UUV,
			            gp_Vec&       D3UVV) const
{
  BSplSLib::D3(U,V,0,0,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
  }

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Geom_BSplineSurface::DN (const Standard_Real    U,
				const Standard_Real    V,
				const Standard_Integer Nu,
				const Standard_Integer Nv ) const
{
  gp_Vec Vn;
  BSplSLib::DN(U,V,Nu,Nv,0,0,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       Vn);
  return Vn;
}

//=======================================================================
//function : LocalValue
//purpose  : 
//=======================================================================

gp_Pnt Geom_BSplineSurface::LocalValue (const Standard_Real    U,
					const Standard_Real    V, 
					const Standard_Integer FromUK1,
					const Standard_Integer ToUK2,
					const Standard_Integer FromVK1, 
					const Standard_Integer ToVK2)  const
{
  gp_Pnt P;
  LocalD0(U,V,FromUK1,ToUK2,FromVK1,ToVK2,P);
  return P;
}

//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocalD0 (const Standard_Real    U,
				   const Standard_Real    V, 
				   const Standard_Integer FromUK1,
				   const Standard_Integer ToUK2,
				   const Standard_Integer FromVK1, 
				   const Standard_Integer ToVK2,
				         gp_Pnt&          P     )  const
{
  Standard_DomainError_Raise_if (FromUK1 == ToUK2 || FromVK1 == ToVK2, 
				 "Geom_BSplineSurface::LocalD0");

  Standard_Real u = U, v = V;
  Standard_Integer uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic,FromUK1,ToUK2,
			    uindex,u);
  uindex = BSplCLib::FlatIndex(udeg,uindex,umults->Array1(),uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic,FromVK1,ToVK2,
			    vindex,v);
  vindex = BSplCLib::FlatIndex(vdeg,vindex,vmults->Array1(),vperiodic);

//  BSplSLib::D0(U,V,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
  BSplSLib::D0(u,v,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       P);
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocalD1 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer FromUK1, 
				   const Standard_Integer ToUK2,
				   const Standard_Integer FromVK1, 
				   const Standard_Integer ToVK2,
				         gp_Pnt&          P,
				         gp_Vec&          D1U, 
				         gp_Vec&          D1V)     const
{
  Standard_DomainError_Raise_if (FromUK1 == ToUK2 || FromVK1 == ToVK2, 
				 "Geom_BSplineSurface::LocalD1");

  Standard_Real u = U, v = V;
  Standard_Integer uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic,FromUK1,ToUK2,
			    uindex,u);
  uindex = BSplCLib::FlatIndex(udeg,uindex,umults->Array1(),uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic,FromVK1,ToVK2,
			    vindex,v);
  vindex = BSplCLib::FlatIndex(vdeg,vindex,vmults->Array1(),vperiodic);

  BSplSLib::D1(u,v,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       P,D1U,D1V);
}

//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocalD2 (const Standard_Real    U,
				   const Standard_Real    V,
				   const Standard_Integer FromUK1,
				   const Standard_Integer ToUK2,
				   const Standard_Integer FromVK1, 
				   const Standard_Integer ToVK2,
				         gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V,
				         gp_Vec&          D2U,
				         gp_Vec&          D2V,
				         gp_Vec&          D2UV) const
{
  Standard_DomainError_Raise_if (FromUK1 == ToUK2 || FromVK1 == ToVK2, 
				 "Geom_BSplineSurface::LocalD2");

  Standard_Real u = U, v = V;
  Standard_Integer uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic,FromUK1,ToUK2,
			    uindex,u);
  uindex = BSplCLib::FlatIndex(udeg,uindex,umults->Array1(),uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic,FromVK1,ToVK2,
			    vindex,v);
  vindex = BSplCLib::FlatIndex(vdeg,vindex,vmults->Array1(),vperiodic);

  BSplSLib::D2(u,v,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       P,D1U,D1V,D2U,D2V,D2UV);
}

//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocalD3 (const Standard_Real    U, 
				   const Standard_Real    V,
				   const Standard_Integer FromUK1, 
				   const Standard_Integer ToUK2,
				   const Standard_Integer FromVK1, 
				   const Standard_Integer ToVK2,
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
  Standard_DomainError_Raise_if (FromUK1 == ToUK2 || FromVK1 == ToVK2, 
				 "Geom_BSplineSurface::LocalD3");

  Standard_Real u = U, v = V;
  Standard_Integer uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic,FromUK1,ToUK2,
			    uindex,u);
  uindex = BSplCLib::FlatIndex(udeg,uindex,umults->Array1(),uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic,FromVK1,ToVK2,
			    vindex,v);
  vindex = BSplCLib::FlatIndex(vdeg,vindex,vmults->Array1(),vperiodic);

  BSplSLib::D3(u,v,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}

//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

gp_Vec Geom_BSplineSurface::LocalDN  (const Standard_Real    U, 
				      const Standard_Real    V,
				      const Standard_Integer FromUK1,
				      const Standard_Integer ToUK2,
				      const Standard_Integer FromVK1,
				      const Standard_Integer ToVK2,
				      const Standard_Integer Nu,
				      const Standard_Integer Nv) const
{
  Standard_DomainError_Raise_if (FromUK1 == ToUK2 || FromVK1 == ToVK2, 
				 "Geom_BSplineSurface::LocalDN");

  Standard_Real u = U, v = V;
  Standard_Integer uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic,FromUK1,ToUK2,
			    uindex,u);
  uindex = BSplCLib::FlatIndex(udeg,uindex,umults->Array1(),uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic,FromVK1,ToVK2,
			    vindex,v);
  vindex = BSplCLib::FlatIndex(vdeg,vindex,vmults->Array1(),vperiodic);

  gp_Vec Vn;
  BSplSLib::DN(u,v,Nu,Nv,uindex,vindex,
	       POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
	       udeg,vdeg,urational,vrational,uperiodic,vperiodic,
	       Vn);
  return Vn;
}

//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

gp_Pnt Geom_BSplineSurface::Pole (const Standard_Integer UIndex,
				  const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if
    (UIndex < 1 || UIndex > poles->ColLength() ||
     VIndex < 1 || VIndex > poles->RowLength(), " ");
  return poles->Value (UIndex, VIndex);
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::Poles (TColgp_Array2OfPnt& P) const
{
  Standard_DimensionError_Raise_if
    (P.ColLength() != poles->ColLength() ||
     P.RowLength() != poles->RowLength(), " ");
  P = poles->Array2();
}

//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_BSplineSurface::UIso (const Standard_Real U) const
{
  TColgp_Array1OfPnt   cpoles(1,poles->RowLength());
  TColStd_Array1OfReal cweights(1,poles->RowLength());

  Handle(Geom_BSplineCurve) C;

  if ( urational || vrational) {
    BSplSLib::Iso(U,Standard_True,POLES,WEIGHTS,UFKNOTS,FMULTS,udeg,uperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,cweights,
			      vknots->Array1(),
			      vmults->Array1(),
			      vdeg,vperiodic);
  }
  else {
    BSplSLib::Iso(U,Standard_True,POLES,
		  *((TColStd_Array2OfReal*) NULL),
		  UFKNOTS,FMULTS,udeg,uperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,
			      vknots->Array1(),
			      vmults->Array1(),
			      vdeg,vperiodic);
  }

  return C;
}

//=======================================================================
//function : UIso
//purpose  : If CheckRational=False, no try to make it non-rational
//=======================================================================

Handle(Geom_Curve) Geom_BSplineSurface::UIso (const Standard_Real U,
					      const Standard_Boolean CheckRational) const
{
  TColgp_Array1OfPnt   cpoles(1,poles->RowLength());
  TColStd_Array1OfReal cweights(1,poles->RowLength());

  Handle(Geom_BSplineCurve) C;

  if ( urational || vrational) {
    BSplSLib::Iso(U,Standard_True,POLES,WEIGHTS,UFKNOTS,FMULTS,udeg,uperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,cweights,
			      vknots->Array1(),
			      vmults->Array1(),
			      vdeg,vperiodic,
			      CheckRational);
  }
  else {
    BSplSLib::Iso(U,Standard_True,POLES,
		  *((TColStd_Array2OfReal*) NULL),
		  UFKNOTS,FMULTS,udeg,uperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,
			      vknots->Array1(),
			      vmults->Array1(),
			      vdeg,vperiodic);
  }

  return C;
}

//=======================================================================
//function : UKnot
//purpose  : 
//=======================================================================

Standard_Real Geom_BSplineSurface::UKnot(const Standard_Integer UIndex) const
{
  Standard_OutOfRange_Raise_if (UIndex < 1 || UIndex > uknots->Length(), " ");
  return uknots->Value (UIndex);
}

//=======================================================================
//function : VKnot
//purpose  : 
//=======================================================================

Standard_Real Geom_BSplineSurface::VKnot(const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if (VIndex < 1 || VIndex > vknots->Length(), " ");
  return vknots->Value (VIndex);
}

//=======================================================================
//function : UKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UKnots (TColStd_Array1OfReal& Ku) const
{
  Standard_DimensionError_Raise_if (Ku.Length() != uknots->Length(), " ");
  Ku = uknots->Array1();
}

//=======================================================================
//function : VKnots
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::VKnots (TColStd_Array1OfReal& Kv) const
{
  Standard_DimensionError_Raise_if (Kv.Length() != vknots->Length(), " ");
  Kv = vknots->Array1();
}

//=======================================================================
//function : UKnotSequence
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UKnotSequence (TColStd_Array1OfReal& Ku) const
{
  Standard_DimensionError_Raise_if (Ku.Length() != ufknots->Length(), " ");
  Ku = ufknots->Array1();
}

//=======================================================================
//function : VKnotSequence
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::VKnotSequence (TColStd_Array1OfReal& Kv) const
{
  Standard_DimensionError_Raise_if (Kv.Length() != vfknots->Length(), " ");
  Kv = vfknots->Array1();
}

//=======================================================================
//function : UMultiplicity
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::UMultiplicity 
  (const Standard_Integer UIndex) const
{
  Standard_OutOfRange_Raise_if (UIndex < 1 || UIndex > umults->Length()," ");
  return umults->Value (UIndex);
}

//=======================================================================
//function : UMultiplicities
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UMultiplicities (TColStd_Array1OfInteger& Mu) const
{
  Standard_DimensionError_Raise_if (Mu.Length() != umults->Length(), " ");
  Mu = umults->Array1();
}

//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_BSplineSurface::VIso (const Standard_Real V) const
{
  TColgp_Array1OfPnt   cpoles(1,poles->ColLength());
  TColStd_Array1OfReal cweights(1,poles->ColLength());

  Handle(Geom_BSplineCurve) C;

  if ( urational || vrational) {
    BSplSLib::Iso(V,Standard_False,POLES,
		  WEIGHTS,
		  VFKNOTS,FMULTS,vdeg,vperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,cweights,
			      uknots->Array1(),
			      umults->Array1(),
			      udeg,uperiodic);
  }
  else {
    BSplSLib::Iso(V,Standard_False,POLES,
		  *((TColStd_Array2OfReal*) NULL),
		  VFKNOTS,FMULTS,vdeg,vperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,
			      uknots->Array1(),
			      umults->Array1(),
			      udeg,uperiodic);
  }

  return C;
}

//=======================================================================
//function : VIso
//purpose  : If CheckRational=False, no try to make it non-rational
//=======================================================================

Handle(Geom_Curve) Geom_BSplineSurface::VIso (const Standard_Real V,
					      const Standard_Boolean CheckRational) const
{
  TColgp_Array1OfPnt   cpoles(1,poles->ColLength());
  TColStd_Array1OfReal cweights(1,poles->ColLength());

  Handle(Geom_BSplineCurve) C;

  if ( urational || vrational) {
    BSplSLib::Iso(V,Standard_False,POLES,
		  WEIGHTS,
		  VFKNOTS,FMULTS,vdeg,vperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,cweights,
			      uknots->Array1(),
			      umults->Array1(),
			      udeg,uperiodic,
			      CheckRational);
  }
  else {
    BSplSLib::Iso(V,Standard_False,POLES,
		  *((TColStd_Array2OfReal*) NULL),
		  VFKNOTS,FMULTS,vdeg,vperiodic,
		  cpoles,cweights);
    C = new Geom_BSplineCurve(cpoles,
			      uknots->Array1(),
			      umults->Array1(),
			      udeg,uperiodic);
  }

  return C;
}

//=======================================================================
//function : VMultiplicity
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::VMultiplicity 
  (const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if (VIndex < 1 || VIndex > vmults->Length()," ");
  return vmults->Value (VIndex);
}

//=======================================================================
//function : VMultiplicities
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::VMultiplicities (TColStd_Array1OfInteger& Mv) const
{
  Standard_DimensionError_Raise_if (Mv.Length() != vmults->Length(), " ");
  Mv = vmults->Array1();
}

//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Geom_BSplineSurface::Weight 
  (const Standard_Integer UIndex,
   const Standard_Integer VIndex ) const
{
  Standard_OutOfRange_Raise_if
    (UIndex < 1 || UIndex > weights->ColLength() ||
     VIndex < 1 || VIndex > weights->RowLength(), " ");
  return weights->Value (UIndex, VIndex);
}

//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::Weights (TColStd_Array2OfReal& W) const
{
  Standard_DimensionError_Raise_if
    (W.ColLength() != weights->ColLength() ||
     W.RowLength() != weights->RowLength(), " ");
  W = weights->Array2();
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::Transform (const gp_Trsf& T)
{
  TColgp_Array2OfPnt & VPoles = poles->ChangeArray2();
  for (Standard_Integer j = VPoles.LowerCol(); j <= VPoles.UpperCol(); j++) {
    for (Standard_Integer i = VPoles.LowerRow(); i <= VPoles.UpperRow(); i++) {
      VPoles (i, j).Transform (T);
    }
  }

  InvalidateCache();
}

//=======================================================================
//function : SetUPeriodic
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUPeriodic ()
{
  Standard_Integer i,j;

  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();
  
  Handle(TColStd_HArray1OfReal) tk = uknots;
  TColStd_Array1OfReal cknots((uknots->Array1())(first),first,last);
  uknots = new TColStd_HArray1OfReal(1,cknots.Length());
  uknots->ChangeArray1() = cknots;

  Handle(TColStd_HArray1OfInteger) tm = umults;
  TColStd_Array1OfInteger cmults((umults->Array1())(first),first,last);
//  Modified by Sergey KHROMOV - Mon Feb 10 10:59:00 2003 Begin
//   cmults(first) = cmults(last) = Max( cmults(first), cmults(last));
  cmults(first) = cmults(last) = Min(udeg, Max( cmults(first), cmults(last)));
//  Modified by Sergey KHROMOV - Mon Feb 10 10:59:00 2003 End
  umults = new TColStd_HArray1OfInteger(1,cmults.Length());
  umults->ChangeArray1() = cmults;

  // compute new number of poles;
  Standard_Integer nbp = BSplCLib::NbPoles(udeg,Standard_True,cmults);
  
  TColgp_Array2OfPnt cpoles(1,nbp,poles->LowerCol(),poles->UpperCol());
  for (i = 1; i <= nbp; i++) {
    for (j = poles->LowerCol(); j <= poles->UpperCol(); j++) {
      cpoles(i,j) = poles->Value(i,j);
    }
  }
  poles = 
    new TColgp_HArray2OfPnt(1,nbp,cpoles.LowerCol(),cpoles.UpperCol());
  poles->ChangeArray2() = cpoles;

  TColStd_Array2OfReal 
    cweights(1,nbp,weights->LowerCol(),weights->UpperCol());
  if (urational || vrational) {
    for (i = 1; i <= nbp; i++) {
      for (j = weights->LowerCol(); j <= weights->UpperCol(); j++) {
	cweights(i,j) = weights->Value(i,j);
      }
    }
  }
  else { 
    for (i = 1; i <= nbp; i++) {
      for (j = weights->LowerCol(); j <= weights->UpperCol(); j++) {
	cweights(i,j) = 1;
      }
    }
  }
  weights = new 
    TColStd_HArray2OfReal(1,nbp,cweights.LowerCol(),cweights.UpperCol());
  weights->ChangeArray2() = cweights;
  

  uperiodic = Standard_True;

  maxderivinvok = 0;
  UpdateUKnots();
}

//=======================================================================
//function : SetVPeriodic
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVPeriodic ()
{
  Standard_Integer i,j;

  Standard_Integer first = FirstVKnotIndex();
  Standard_Integer last  = LastVKnotIndex();
  
  Handle(TColStd_HArray1OfReal) tk = vknots;
  TColStd_Array1OfReal cknots((vknots->Array1())(first),first,last);
  vknots = new TColStd_HArray1OfReal(1,cknots.Length());
  vknots->ChangeArray1() = cknots;


  Handle(TColStd_HArray1OfInteger) tm = vmults;
  TColStd_Array1OfInteger cmults((vmults->Array1())(first),first,last);
//  Modified by Sergey KHROMOV - Mon Feb 10 11:00:33 2003 Begin
//   cmults(first) = cmults(last) = Max( cmults(first), cmults(last));
  cmults(first) = cmults(last) = Min(vdeg, Max( cmults(first), cmults(last)));
//  Modified by Sergey KHROMOV - Mon Feb 10 11:00:34 2003 End
  vmults = new TColStd_HArray1OfInteger(1,cmults.Length());
  vmults->ChangeArray1() = cmults;

  // compute new number of poles;
  Standard_Integer nbp = BSplCLib::NbPoles(vdeg,Standard_True,cmults);
  
  TColgp_Array2OfPnt cpoles(poles->LowerRow(),poles->UpperRow(),1,nbp);
  for (i = poles->LowerRow(); i <= poles->UpperRow(); i++) {
    for (j = 1; j <= nbp; j++) {
      cpoles(i,j) = poles->Value(i,j);
    }
  }
  poles =
    new TColgp_HArray2OfPnt(cpoles.LowerRow(),cpoles.UpperRow(),1,nbp);
  poles->ChangeArray2() = cpoles;

  if (urational || vrational) {
    TColStd_Array2OfReal 
      cweights(weights->LowerRow(),weights->UpperRow(),1,nbp);
    for (i = weights->LowerRow(); i <= weights->UpperRow(); i++) {
      for (j = 1; j <= nbp; j++) {
	cweights(i,j) = weights->Value(i,j);
      }
    }
    weights = new 
      TColStd_HArray2OfReal(cweights.LowerRow(),cweights.UpperRow(),1,nbp);
    weights->ChangeArray2() = cweights;
  }

  vperiodic = Standard_True;

  maxderivinvok = 0;
  UpdateVKnots();
}

//=======================================================================
//function : SetUOrigin
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUOrigin(const Standard_Integer Index)
{
  Standard_NoSuchObject_Raise_if( !uperiodic,
				 "Geom_BSplineSurface::SetUOrigin");

  Standard_Integer i,j,k;
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();

  Standard_DomainError_Raise_if( (Index < first) || (Index > last),
				"Geom_BSplineCurve::SetUOrigine");

  Standard_Integer nbknots = uknots->Length();
  Standard_Integer nbpoles = poles->ColLength();

  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots);
  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();

  Handle(TColStd_HArray1OfInteger) nmults =
    new TColStd_HArray1OfInteger(1,nbknots);
  TColStd_Array1OfInteger& newmults = nmults->ChangeArray1();

  // set the knots and mults
  Standard_Real period = uknots->Value(last) - uknots->Value(first);
  k = 1;
  for ( i = Index; i <= last ; i++) {
    newknots(k) = uknots->Value(i);
    newmults(k) = umults->Value(i);
    k++;
  }
  for ( i = first+1; i <= Index; i++) {
    newknots(k) = uknots->Value(i) + period;
    newmults(k) = umults->Value(i);
    k++;
  }

  Standard_Integer index = 1;
  for (i = first+1; i <= Index; i++) 
    index += umults->Value(i);

  // set the poles and weights
  Standard_Integer nbvp = poles->RowLength();
  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,nbpoles,1,nbvp);
  Handle(TColStd_HArray2OfReal) nweights =
    new TColStd_HArray2OfReal(1,nbpoles,1,nbvp);
  TColgp_Array2OfPnt   & newpoles   = npoles->ChangeArray2();
  TColStd_Array2OfReal & newweights = nweights->ChangeArray2();
  first = poles->LowerRow();
  last  = poles->UpperRow();
  if ( urational || vrational) {
    k = 1;
    for ( i = index; i <= last; i++) {
      for ( j = 1; j <= nbvp; j++) {
	newpoles(k,j)   = poles->Value(i,j);
	newweights(k,j) = weights->Value(i,j);
      }
      k++;
    }
    for ( i = first; i < index; i++) {
      for ( j = 1; j <= nbvp; j++) {
	newpoles(k,j)   = poles->Value(i,j);
	newweights(k,j) = weights->Value(i,j);
      }
      k++;
    }
  }
  else {
    k = 1;
    for ( i = index; i <= last; i++) {
      for ( j = 1; j <= nbvp; j++) {
	newpoles(k,j) = poles->Value(i,j);
      }
      k++;
    }
    for ( i = first; i < index; i++) {
      for ( j = 1; j <= nbvp; j++) {
	newpoles(k,j) = poles->Value(i,j);
      }
      k++;
    }
  }

  poles  = npoles;
  uknots = nknots;
  umults = nmults;
  if (urational || vrational) 
    weights = nweights;
  UpdateUKnots();
  
}

//=======================================================================
//function : SetVOrigin
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVOrigin(const Standard_Integer Index)
{
  Standard_NoSuchObject_Raise_if( !vperiodic,
				 "Geom_BSplineSurface::SetVOrigin");

  Standard_Integer i,j,k;
  Standard_Integer first = FirstVKnotIndex();
  Standard_Integer last  = LastVKnotIndex();

  Standard_DomainError_Raise_if( (Index < first) || (Index > last),
				"Geom_BSplineCurve::SetVOrigine");

  Standard_Integer nbknots = vknots->Length();
  Standard_Integer nbpoles = poles->RowLength();

  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots);
  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();

  Handle(TColStd_HArray1OfInteger) nmults =
    new TColStd_HArray1OfInteger(1,nbknots);
  TColStd_Array1OfInteger& newmults = nmults->ChangeArray1();

  // set the knots and mults
  Standard_Real period = vknots->Value(last) - vknots->Value(first);
  k = 1;
  for ( i = Index; i <= last ; i++) {
    newknots(k) = vknots->Value(i);
    newmults(k) = vmults->Value(i);
    k++;
  }
  for ( i = first+1; i <= Index; i++) {
    newknots(k) = vknots->Value(i) + period;
    newmults(k) = vmults->Value(i);
    k++;
  }

  Standard_Integer index = 1;
  for (i = first+1; i <= Index; i++) 
    index += vmults->Value(i);

  // set the poles and weights
  Standard_Integer nbup = poles->ColLength();
  Handle(TColgp_HArray2OfPnt) npoles =
    new TColgp_HArray2OfPnt(1,nbup,1,nbpoles);
  Handle(TColStd_HArray2OfReal) nweights =
    new TColStd_HArray2OfReal(1,nbup,1,nbpoles);
  TColgp_Array2OfPnt   & newpoles   = npoles->ChangeArray2();
  TColStd_Array2OfReal & newweights = nweights->ChangeArray2();
  first = poles->LowerCol();
  last  = poles->UpperCol();
  if ( urational || vrational) {
    k = 1;
    for ( j = index; j <= last; j++) {
      for ( i = 1; i <= nbup; i++) {
	newpoles(i,k)   = poles->Value(i,j);
	newweights(i,k) = weights->Value(i,j);
      }
      k++;
    }
    for ( j = first; j < index; j++) {
      for ( i = 1; i <= nbup; i++) {
	newpoles(i,k)   = poles->Value(i,j);
	newweights(i,k) = weights->Value(i,j);
      }
      k++;
    }
  }
  else {
    k = 1;
    for ( j = index; j <= last; j++) {
      for ( i = 1; i <= nbup; i++) {
	newpoles(i,k)   = poles->Value(i,j);
      }
      k++;
    }
    for ( j = first; j < index; j++) {
      for ( i = 1; i <= nbup; i++) {
	newpoles(i,k)   = poles->Value(i,j);
      }
      k++;
    }
  }

  poles  = npoles;
  vknots = nknots;
  vmults = nmults;
  if (urational || vrational) 
    weights = nweights;
  UpdateVKnots();
  
}

//=======================================================================
//function : SetUNotPeriodic
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetUNotPeriodic () 
{ 
  if ( uperiodic) {
    Standard_Integer NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize( udeg, umults->Array1(), NbKnots, NbPoles);

    Handle(TColgp_HArray2OfPnt) npoles = 
      new TColgp_HArray2OfPnt(1,NbPoles, 1, poles->RowLength());
    
    Handle(TColStd_HArray1OfReal) nknots 
      = new TColStd_HArray1OfReal(1,NbKnots);

    Handle(TColStd_HArray1OfInteger) nmults
      = new TColStd_HArray1OfInteger(1,NbKnots);

    Handle(TColStd_HArray2OfReal) nweights = new TColStd_HArray2OfReal(1,NbPoles, 1, poles->RowLength(), 0);

    if ( urational || vrational) {

      BSplSLib::Unperiodize(Standard_True         , udeg, 
			    umults->Array1()      , uknots->Array1(),
			    poles->Array2()       , weights->Array2(),
			    nmults->ChangeArray1(), nknots->ChangeArray1(),
			    npoles->ChangeArray2(),
			    nweights->ChangeArray2());
    }
    else {

      BSplSLib::Unperiodize(Standard_True         , udeg, 
			    umults->Array1()      , uknots->Array1(),
			    poles->Array2()       , BSplSLib::NoWeights(),
			    nmults->ChangeArray1(), nknots->ChangeArray1(),
			    npoles->ChangeArray2(),
			    *((TColStd_Array2OfReal*) NULL));
    }
    poles     = npoles;
    weights   = nweights;
    umults    = nmults;
    uknots    = nknots;
    uperiodic = Standard_False;
    
    maxderivinvok = 0;
    UpdateUKnots();
    
  }
}

//=======================================================================
//function : SetVNotPeriodic
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetVNotPeriodic ()
{
  if ( vperiodic) {
    Standard_Integer NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize( vdeg, vmults->Array1(), NbKnots, NbPoles);

    Handle(TColgp_HArray2OfPnt) npoles = 
      new TColgp_HArray2OfPnt(1, poles->ColLength(), 1, NbPoles);
    
    Handle(TColStd_HArray1OfReal) nknots 
      = new TColStd_HArray1OfReal(1,NbKnots);

    Handle(TColStd_HArray1OfInteger) nmults
      = new TColStd_HArray1OfInteger(1,NbKnots) ;

    Handle(TColStd_HArray2OfReal) nweights = new TColStd_HArray2OfReal(1, poles->ColLength(), 1, NbPoles, 0);

    if ( urational || vrational) {

      BSplSLib::Unperiodize(Standard_False        , vdeg, 
			    vmults->Array1()      , vknots->Array1(),
			    poles->Array2()       , weights->Array2(),
			    nmults->ChangeArray1(), nknots->ChangeArray1(),
			    npoles->ChangeArray2(),
			    nweights->ChangeArray2());
    }
    else {

      BSplSLib::Unperiodize(Standard_False        , vdeg, 
			    vmults->Array1()      , vknots->Array1(),
			    poles->Array2()       , BSplSLib::NoWeights(),
			    nmults->ChangeArray1(), nknots->ChangeArray1(),
			    npoles->ChangeArray2(),
			    *((TColStd_Array2OfReal*) NULL));
    }
    poles     = npoles;
    weights   = nweights;
    vmults    = nmults;
    vknots    = nknots;
    vperiodic = Standard_False;
    
    maxderivinvok = 0;
    UpdateVKnots();
    
  }
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsUClosed () const
{   
  if (uperiodic)
    return Standard_True;

  Standard_Boolean Closed     = Standard_True;
  TColgp_Array2OfPnt & VPoles = poles->ChangeArray2();
  Standard_Integer PLower     = VPoles.LowerRow();
  Standard_Integer PUpper     = VPoles.UpperRow();
  Standard_Integer PLength    = VPoles.RowLength();
  Standard_Integer j = VPoles.LowerCol();
  if ( urational || vrational) {
    TColStd_Array2OfReal & VWeights = weights->ChangeArray2();
    Standard_Integer WLower = VWeights.LowerRow();
    Standard_Integer WUpper = VWeights.UpperRow();
    Standard_Real    Alfa = VWeights(WLower,VWeights.LowerCol());
    Alfa /= VWeights(WUpper,VWeights.LowerCol());

    Standard_Integer k = VWeights.LowerCol();
    while (Closed && j <= PLength) {
      Closed = 
	(VPoles (PLower, j).Distance (VPoles (PUpper, j)) <= Precision::Confusion());
      j++;
      Closed = (Closed &&
	       ((VWeights(WLower,k) / VWeights(WUpper,k)) - Alfa) 
		 < Epsilon(Alfa));
      k++;
    }
  }
  else {
    while (Closed && j <= PLength) {
      Closed = 
	(VPoles (PLower, j).Distance (VPoles (PUpper, j)) <= Precision::Confusion());
      j++;
    }
  }
  return Closed; 
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsVClosed () const
{
  if (vperiodic)
    return Standard_True;

  Standard_Boolean Closed     = Standard_True;
  TColgp_Array2OfPnt & VPoles = poles->ChangeArray2();
  Standard_Integer PLower     = VPoles.LowerCol();
  Standard_Integer PUpper     = VPoles.UpperCol();
  Standard_Integer PLength    = VPoles.ColLength();
  Standard_Integer i = VPoles.LowerRow();
  if ( urational || vrational) {
    TColStd_Array2OfReal & VWeights = weights->ChangeArray2();
    Standard_Integer WLower = VWeights.LowerCol();
    Standard_Integer WUpper = VWeights.UpperCol();
    Standard_Real    Alfa = VWeights(VWeights.LowerRow(),WLower);
    Alfa /= VWeights(VWeights.LowerRow(),WUpper);

    Standard_Integer k = VWeights.LowerRow();
    while (Closed && i <= PLength) {
      Closed = 
	(VPoles (i, PLower).Distance (VPoles (i, PUpper)) <= Precision::Confusion());
      i++;
      Closed = (Closed &&
	       ((VWeights(k,WLower) / VWeights(k,WUpper)) - Alfa) 
		 < Epsilon(Alfa));
      k++;
    }
  }
  else {
    while (Closed && i <= PLength) {
      Closed = 
	(VPoles (i, PLower).Distance (VPoles (i, PUpper)) <= Precision::Confusion());
      i++;
    }
  }
  return Closed; 
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsUPeriodic () const 
{
  return uperiodic; 
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsVPeriodic () const 
{ 
  return vperiodic; 
}

//=======================================================================
//function : FirstUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::FirstUKnotIndex () const 
{ 
  if (uperiodic) return 1;
  else return BSplCLib::FirstUKnotIndex(udeg,umults->Array1()); 
}

//=======================================================================
//function : FirstVKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::FirstVKnotIndex () const 
{ 
  if (vperiodic) return 1;
  else return BSplCLib::FirstUKnotIndex(vdeg,vmults->Array1()); 
}

//=======================================================================
//function : LastUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::LastUKnotIndex() const 
{
  if (uperiodic) return uknots->Length();
  else return BSplCLib::LastUKnotIndex(udeg,umults->Array1()); 
}

//=======================================================================
//function : LastVKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::LastVKnotIndex() const 
{
  if (vperiodic) return vknots->Length();
  else return BSplCLib::LastUKnotIndex(vdeg,vmults->Array1()); 
}

//=======================================================================
//function : LocateU
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocateU
  (const Standard_Real     U, 
   const Standard_Real     ParametricTolerance, 
         Standard_Integer& I1,
         Standard_Integer& I2,
   const Standard_Boolean  WithKnotRepetition ) const
{
  Standard_Real NewU =U, vbid = vknots->Value(1);
  Handle(TColStd_HArray1OfReal) TheKnots;
  if (WithKnotRepetition) TheKnots = ufknots;
  else                    TheKnots = uknots;

  PeriodicNormalization(NewU, vbid); //Attention a la periode
  
  const TColStd_Array1OfReal & Knots = TheKnots->Array1();
  Standard_Real UFirst = Knots (1);
  Standard_Real ULast  = Knots (Knots.Length());
  Standard_Real PParametricTolerance = Abs(ParametricTolerance);
  if (Abs (NewU - UFirst) <= PParametricTolerance) { 
    I1 = I2 = 1; 
  }
  else if (Abs (NewU - ULast) <= PParametricTolerance) { 
    I1 = I2 = Knots.Length();
  }
  else if (NewU < UFirst) {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast) {
    I1 = Knots.Length();
    I2 = I1 + 1;
  }
  else {
    I1 = 1;
    BSplCLib::Hunt (Knots, NewU, I1);
    while ( Abs( Knots(I1+1) - NewU) <= PParametricTolerance) I1++;
    if ( Abs( Knots(I1) - NewU) <= PParametricTolerance) {
      I2 = I1;
    }
    else {
      I2 = I1 + 1;
    }
  }
}

//=======================================================================
//function : LocateV
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::LocateV
  (const Standard_Real     V, 
   const Standard_Real     ParametricTolerance, 
         Standard_Integer& I1,
         Standard_Integer& I2,
   const Standard_Boolean  WithKnotRepetition ) const
{
  Standard_Real NewV =V, ubid = uknots->Value(1);  
  Handle(TColStd_HArray1OfReal) TheKnots;
  if (WithKnotRepetition) TheKnots = vfknots;
  else                    TheKnots = vknots;

  PeriodicNormalization(ubid, NewV); //Attention a la periode
  
  const TColStd_Array1OfReal & Knots = TheKnots->Array1();
  Standard_Real VFirst = Knots (1);
  Standard_Real VLast  = Knots (Knots.Length());
  Standard_Real PParametricTolerance = Abs(ParametricTolerance);
  if (Abs (NewV - VFirst) <= PParametricTolerance) { I1 = I2 = 1; }
  else if (Abs (NewV - VLast) <= PParametricTolerance) { 
    I1 = I2 = Knots.Length();
  }
  else if (NewV < VFirst - PParametricTolerance) {
    I2 = 1;
    I1 = 0;
  }
  else if (NewV > VLast + PParametricTolerance) {
    I1 = Knots.Length();
    I2 = I1 + 1;
  }
  else {
    I1 = 1;
    BSplCLib::Hunt (Knots, NewV, I1);
    while ( Abs( Knots(I1+1) - NewV) <= PParametricTolerance) I1++;
    if ( Abs( Knots(I1) - NewV) <= PParametricTolerance) {
      I2 = I1;
    }
    else {
      I2 = I1 + 1;
    }
  }
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::UReverse ()
{
  BSplCLib::Reverse(umults->ChangeArray1());
  BSplCLib::Reverse(uknots->ChangeArray1());
  Standard_Integer last;
  if (uperiodic)
    last = ufknots->Upper() - udeg -1;
  else
    last = poles->UpperRow();
  BSplSLib::Reverse(poles->ChangeArray2(),last,Standard_True);
  if (urational || vrational)
    BSplSLib::Reverse(weights->ChangeArray2(),last,Standard_True);
  UpdateUKnots();
}

//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BSplineSurface::UReversedParameter
  ( const Standard_Real U) const 
{
  return ( uknots->Value( 1) + uknots->Value( uknots->Length()) - U);
}

//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::VReverse ()
{
  BSplCLib::Reverse(vmults->ChangeArray1());
  BSplCLib::Reverse(vknots->ChangeArray1());
  Standard_Integer last;
  if (vperiodic)
    last = vfknots->Upper() - vdeg -1;
  else
    last = poles->UpperCol();
  BSplSLib::Reverse(poles->ChangeArray2(),last,Standard_False);
  if (urational || vrational)
    BSplSLib::Reverse(weights->ChangeArray2(),last,Standard_False);
  UpdateVKnots();
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_BSplineSurface::VReversedParameter
  ( const Standard_Real V) const 
{
  return ( vknots->Value( 1) + vknots->Value( vknots->Length()) - V);
}

//=======================================================================
//function : SetPoleCol
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPoleCol (const Standard_Integer      VIndex,
				      const TColgp_Array1OfPnt&   CPoles)
{
  if (VIndex < 1 || VIndex > poles->RowLength()) {
    Standard_OutOfRange::Raise();
    }
  if (CPoles.Lower() < 1 || CPoles.Lower() > poles->ColLength() || 
      CPoles.Upper() < 1 || CPoles.Upper() > poles->ColLength()) {
    Standard_ConstructionError::Raise();
    }

  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();

  for (Standard_Integer I = CPoles.Lower(); I <= CPoles.Upper(); I++) {
    Poles (I+Poles.LowerRow()-1, VIndex+Poles.LowerCol()-1) = CPoles(I);
  }

  InvalidateCache();
}

//=======================================================================
//function : SetPoleCol
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPoleCol (const Standard_Integer      VIndex,
				      const TColgp_Array1OfPnt&   CPoles,
				      const TColStd_Array1OfReal& CPoleWeights)
{
  SetPoleCol  (VIndex, CPoles);
  SetWeightCol(VIndex, CPoleWeights); 
}

//=======================================================================
//function : SetPoleRow
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPoleRow (const Standard_Integer    UIndex,
				      const TColgp_Array1OfPnt& CPoles)
{
  if (UIndex < 1 || UIndex > poles->ColLength()  ) {
    Standard_OutOfRange::Raise();
    }
  if (CPoles.Lower() < 1 || CPoles.Lower() > poles->RowLength() || 
      CPoles.Upper() < 1 || CPoles.Upper() > poles->RowLength() ) {
    Standard_ConstructionError::Raise();
    }

  TColgp_Array2OfPnt & Poles = poles->ChangeArray2();

  for (Standard_Integer I = CPoles.Lower(); I <= CPoles.Upper(); I++) {
    Poles (UIndex+Poles.LowerRow()-1, I+Poles.LowerCol()-1) = CPoles (I);
  }

  InvalidateCache();
}

//=======================================================================
//function : SetPoleRow
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPoleRow(const Standard_Integer      UIndex,
				     const TColgp_Array1OfPnt &  CPoles,
				     const TColStd_Array1OfReal& CPoleWeights)
{
  SetPoleRow  (UIndex, CPoles);
  SetWeightRow(UIndex, CPoleWeights);  
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPole (const Standard_Integer UIndex,
				   const Standard_Integer VIndex,
				   const gp_Pnt&          P)
{
  poles->SetValue (UIndex+poles->LowerRow()-1, VIndex+poles->LowerCol()-1, P);
  InvalidateCache();
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::SetPole (const Standard_Integer UIndex,
				   const Standard_Integer VIndex,
				   const gp_Pnt&          P, 
				   const Standard_Real    Weight)
{
  SetWeight(UIndex, VIndex, Weight);
  SetPole  (UIndex, VIndex, P);
}

//=======================================================================
//function : MovePoint
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::MovePoint(const Standard_Real U,
				    const Standard_Real V,
 			 	    const gp_Pnt& P,
				    const Standard_Integer UIndex1,
				    const Standard_Integer UIndex2,
				    const Standard_Integer VIndex1,
				    const Standard_Integer VIndex2,
				    Standard_Integer& UFirstModifiedPole,
				    Standard_Integer& ULastmodifiedPole,
				    Standard_Integer& VFirstModifiedPole,
				    Standard_Integer& VLastmodifiedPole)
{
  if (UIndex1 < 1 || UIndex1 > poles->UpperRow() || 
      UIndex2 < 1 || UIndex2 > poles->UpperRow() || UIndex1 > UIndex2 ||
      VIndex1 < 1 || VIndex1 > poles->UpperCol() || 
      VIndex2 < 1 || VIndex2 > poles->UpperCol() || VIndex1 > VIndex2) {
    Standard_OutOfRange::Raise();
  }

  TColgp_Array2OfPnt npoles(1, poles->UpperRow(), 1, poles->UpperCol());
  gp_Pnt P0;
  D0(U, V, P0);
  gp_Vec Displ(P0, P);
  Standard_Boolean rational = (urational || vrational);
  BSplSLib::MovePoint(U, V, Displ, UIndex1, UIndex2, VIndex1, VIndex2, udeg, vdeg,
		      rational, poles->Array2(), weights->Array2(),
		      ufknots->Array1(), vfknots->Array1(), 
		      UFirstModifiedPole, ULastmodifiedPole,
		      VFirstModifiedPole, VLastmodifiedPole,
		      npoles);
  if (UFirstModifiedPole) {
    poles->ChangeArray2() = npoles;
  }
  maxderivinvok = 0;
  InvalidateCache() ;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_BSplineSurface::Bounds (Standard_Real& U1,
				  Standard_Real& U2,
				  Standard_Real& V1,
				  Standard_Real& V2) const
{
  U1 = ufknots->Value (udeg+1);
  U2 = ufknots->Value (ufknots->Upper()-udeg);
  V1 = vfknots->Value (vdeg+1);
  V2 = vfknots->Value (vfknots->Upper()-vdeg);
}

//=======================================================================
//function : MaxDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::MaxDegree () 
{ 
  return BSplCLib::MaxDegree(); 
}

//=======================================================================
//function : IsURational
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsURational () const 
{
  return urational; 
}

//=======================================================================
//function : IsVRational
//purpose  : 
//=======================================================================

Standard_Boolean Geom_BSplineSurface::IsVRational () const 
{
  return vrational; 
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_BSplineSurface::Continuity () const 
{
  return ((Usmooth < Vsmooth) ? Usmooth : Vsmooth) ; 
}

//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::NbUKnots () const 
{
  return uknots->Length(); 
}

//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::NbUPoles () const 
{
  return poles->ColLength(); 
}

//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::NbVKnots () const 
{
  return vknots->Length(); 
}

//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::NbVPoles () const 
{
  return poles->RowLength(); 
}

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::UDegree () const 
{
  return udeg; 
}

//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Geom_BSplineSurface::VDegree () const 
{
  return vdeg; 
}

//=======================================================================
//function : UKnotDistribution
//purpose  : 
//=======================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::UKnotDistribution() const 
{ 
  return uknotSet; 
}


//=======================================================================
//function : VKnotDistribution
//purpose  : 
//=======================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::VKnotDistribution() const 
{
  return vknotSet; 
}

//=======================================================================
//function : InsertUKnots
//purpose  : 
//=======================================================================

void  Geom_BSplineSurface::InsertUKnots
  (const TColStd_Array1OfReal&    Knots, 
   const TColStd_Array1OfInteger& Mults,
   const Standard_Real            ParametricTolerance,
   const Standard_Boolean         Add)
{
  // Check and compute new sizes
  Standard_Integer nbpoles, nbknots;

  if ( !BSplCLib::PrepareInsertKnots(udeg,uperiodic,
				     uknots->Array1(),umults->Array1(),
				     Knots,Mults,nbpoles,nbknots,
				     ParametricTolerance,Add))
    Standard_ConstructionError::Raise("Geom_BSplineSurface::InsertUKnots");
  
  if ( nbpoles == poles->ColLength()) return;

  Handle(TColgp_HArray2OfPnt) npoles
    = new TColgp_HArray2OfPnt(1,nbpoles, 1,poles->RowLength());
  Handle(TColStd_HArray2OfReal) nweights =
    new TColStd_HArray2OfReal(1,nbpoles, 
			      1,poles->RowLength(),
			      1.0);
  Handle(TColStd_HArray1OfReal)    nknots = uknots;
  Handle(TColStd_HArray1OfInteger) nmults = umults;

  if ( nbknots != uknots->Length()) {
    nknots = new TColStd_HArray1OfReal(1,nbknots);
    nmults = new TColStd_HArray1OfInteger(1,nbknots);
  }

  if ( urational || vrational) {
    BSplSLib::InsertKnots(Standard_True,
			  udeg, uperiodic,
			  poles->Array2() , weights->Array2(),
			  uknots->Array1(), umults->Array1(),
			  Knots, Mults,
			  npoles->ChangeArray2(),
			  nweights->ChangeArray2(),
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  ParametricTolerance, Add);
  }
  else {
    BSplSLib::InsertKnots(Standard_True,
			  udeg, uperiodic,
			  poles->Array2() , BSplSLib::NoWeights(),
			  uknots->Array1(), umults->Array1(),
			  Knots, Mults,
			  npoles->ChangeArray2(),
			  *((TColStd_Array2OfReal*) NULL),
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  ParametricTolerance, Add);
  }

  poles = npoles;
  weights = nweights;
  uknots = nknots;
  umults = nmults;
  UpdateUKnots();
				     
}

//=======================================================================
//function : InsertVKnots
//purpose  : 
//=======================================================================

void  Geom_BSplineSurface::InsertVKnots
  (const TColStd_Array1OfReal& Knots, 
   const TColStd_Array1OfInteger& Mults, 
   const Standard_Real ParametricTolerance,
   const Standard_Boolean Add)
{
  // Check and compute new sizes
  Standard_Integer nbpoles, nbknots;

  if ( !BSplCLib::PrepareInsertKnots(vdeg,vperiodic,
				     vknots->Array1(),vmults->Array1(),
				     Knots,Mults,nbpoles,nbknots,
				     ParametricTolerance, Add))
    Standard_ConstructionError::Raise("Geom_BSplineSurface::InsertVKnots");
  
  if ( nbpoles == poles->RowLength()) return;

  Handle(TColgp_HArray2OfPnt) npoles
    = new TColgp_HArray2OfPnt(1,poles->ColLength(), 1,nbpoles);
  Handle(TColStd_HArray2OfReal) nweights =
    new TColStd_HArray2OfReal(1,poles->ColLength(),
			      1,nbpoles,
			      1.0);
  Handle(TColStd_HArray1OfReal)    nknots = vknots;
  Handle(TColStd_HArray1OfInteger) nmults = vmults;

  if ( nbknots != vknots->Length()) {
    nknots = new TColStd_HArray1OfReal(1,nbknots);
    nmults = new TColStd_HArray1OfInteger(1,nbknots);
  }

  if ( urational || vrational) {
    BSplSLib::InsertKnots(Standard_False,
			  vdeg, vperiodic,
			  poles->Array2() , weights->Array2(),
			  vknots->Array1(), vmults->Array1(),
			  Knots, Mults,
			  npoles->ChangeArray2(),
			  nweights->ChangeArray2(),
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  ParametricTolerance, Add);
  }
  else {
    BSplSLib::InsertKnots(Standard_False,
			  vdeg, vperiodic,
			  poles->Array2() , BSplSLib::NoWeights(),
			  vknots->Array1(), vmults->Array1(),
			  Knots, Mults,
			  npoles->ChangeArray2(),
			  *((TColStd_Array2OfReal*) NULL),
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  ParametricTolerance, Add);
  }

  poles = npoles;
  weights = nweights;
  vknots = nknots;
  vmults = nmults;
  UpdateVKnots();
				     
}

//=======================================================================
//function : RemoveUKnot
//purpose  : 
//=======================================================================

Standard_Boolean  Geom_BSplineSurface::RemoveUKnot
  (const Standard_Integer Index, 
   const Standard_Integer M, 
   const Standard_Real Tolerance)
{
  if ( M < 0 ) return Standard_True;
  
  Standard_Integer I1 = FirstUKnotIndex ();
  Standard_Integer I2 = LastUKnotIndex  ();
  
  if ( !uperiodic && (Index <= I1 || Index >= I2) ) {
    Standard_OutOfRange::Raise();
  }
  else if ( uperiodic  && (Index < I1 || Index > I2)) {
    Standard_OutOfRange::Raise();
  }
  
  const TColgp_Array2OfPnt   & oldpoles = poles->Array2();

  Standard_Integer step = umults->Value(Index) - M;
  if (step <= 0 ) return Standard_True;

  Handle(TColgp_HArray2OfPnt) npoles = 
    new TColgp_HArray2OfPnt( 1, oldpoles.ColLength() - step,
			     1, oldpoles.RowLength());
  Handle(TColStd_HArray1OfReal)    nknots = uknots;
  Handle(TColStd_HArray1OfInteger) nmults = umults;

  if ( M == 0) {
    nknots = new TColStd_HArray1OfReal(1,uknots->Length()-1);
    nmults = new TColStd_HArray1OfInteger(1,uknots->Length()-1);
  }
  Handle(TColStd_HArray2OfReal) nweights ;
  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal( 1, npoles->ColLength(),
				1, npoles->RowLength());
    if (!BSplSLib::RemoveKnot(Standard_True,
			      Index,M,udeg,uperiodic,
			      poles->Array2(),weights->Array2(),
			      uknots->Array1(),umults->Array1(),
			      npoles->ChangeArray2(),
			      nweights->ChangeArray2(),
			      nknots->ChangeArray1(),nmults->ChangeArray1(),
			      Tolerance))
      return Standard_False;
  }
  else {
//
// sync the size of the weights
//
    nweights = 
      new TColStd_HArray2OfReal(1, npoles->ColLength(),
				1, npoles->RowLength(),
				1.0e0 );
    if (!BSplSLib::RemoveKnot(Standard_True,
			      Index,M,udeg,uperiodic,
			      poles->Array2(),BSplSLib::NoWeights(),
			      uknots->Array1(),umults->Array1(),
			      npoles->ChangeArray2(),
			      *((TColStd_Array2OfReal*) NULL),
			      nknots->ChangeArray1(),nmults->ChangeArray1(),
			      Tolerance))
      return Standard_False;
  }
  
  poles = npoles;
  weights = nweights;
  uknots = nknots;
  umults = nmults;

  maxderivinvok = 0;
  UpdateUKnots();
  return Standard_True;
}

//=======================================================================
//function : RemoveVKnot
//purpose  : 
//=======================================================================

Standard_Boolean  Geom_BSplineSurface::RemoveVKnot
  (const Standard_Integer Index, 
   const Standard_Integer M,
   const Standard_Real Tolerance)
{
  if ( M < 0 ) return Standard_True;
  
  Standard_Integer I1 = FirstVKnotIndex ();
  Standard_Integer I2 = LastVKnotIndex  ();
  
  if ( !vperiodic && (Index <= I1 || Index >= I2) ) {
    Standard_OutOfRange::Raise();
  }
  else if ( vperiodic  && (Index < I1 || Index > I2)) {
    Standard_OutOfRange::Raise();
  }
  
  const TColgp_Array2OfPnt   & oldpoles = poles->Array2();

  Standard_Integer step = vmults->Value(Index) - M;
  if (step <= 0 ) return Standard_True;

  Handle(TColgp_HArray2OfPnt) npoles = 
    new TColgp_HArray2OfPnt( 1, oldpoles.ColLength(),
			     1, oldpoles.RowLength() - step);
  Handle(TColStd_HArray1OfReal)    nknots = vknots;
  Handle(TColStd_HArray1OfInteger) nmults = vmults;

  if ( M == 0) {
    nknots = new TColStd_HArray1OfReal(1,vknots->Length()-1);
    nmults = new TColStd_HArray1OfInteger(1,vknots->Length()-1);
  }
  Handle(TColStd_HArray2OfReal) nweights ;
  if (urational || vrational) {
    nweights = 
      new TColStd_HArray2OfReal( 1, npoles->ColLength(),
				 1, npoles->RowLength()) ;


    if (!BSplSLib::RemoveKnot(Standard_False,
			      Index,M,vdeg,vperiodic,
			      poles->Array2(),weights->Array2(),
			      vknots->Array1(),vmults->Array1(),
			      npoles->ChangeArray2(),
			      nweights->ChangeArray2(),
			      nknots->ChangeArray1(),nmults->ChangeArray1(),
			      Tolerance))
      return Standard_False;
  }
  else {
//
// sync the size of the weights array
//
    nweights = 
      new TColStd_HArray2OfReal(1, npoles->ColLength(),
				1, npoles->RowLength(),
				1.0e0 );
    if (!BSplSLib::RemoveKnot(Standard_False,
			      Index,M,vdeg,vperiodic,
			      poles->Array2(),BSplSLib::NoWeights(),
			      vknots->Array1(),vmults->Array1(),
			      npoles->ChangeArray2(),
			      *((TColStd_Array2OfReal*) NULL),
			      nknots->ChangeArray1(),nmults->ChangeArray1(),
			      Tolerance))
      return Standard_False;
  }
  
  poles = npoles;
  vknots = nknots;
  vmults = nmults;
  weights = nweights;
  maxderivinvok = 0;
  UpdateVKnots();
  return Standard_True;
}

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

void   Geom_BSplineSurface::Resolution( const Standard_Real  Tolerance3D,
				       Standard_Real&        UTolerance,
				       Standard_Real&        VTolerance)
{
  if(!maxderivinvok){
    BSplSLib::Resolution(poles  ->Array2(),
			 weights->Array2(),
			 uknots ->Array1(),
			 vknots ->Array1(),
			 umults ->Array1(),
			 vmults ->Array1(),
			 udeg,
			 vdeg,
			 urational,
			 vrational,
			 uperiodic,
			 vperiodic,
			 1.,
			 umaxderivinv,
			 vmaxderivinv) ;
    maxderivinvok = 1;
  }
  UTolerance = Tolerance3D * umaxderivinv;
  VTolerance = Tolerance3D * vmaxderivinv;
}
		       



