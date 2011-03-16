// File:	Geom_OffsetSurface.cxx
// Created:	Wed Mar 10 09:54:38 1993
// Author:	JCV
//		<fid@phylox>
// Copyright:	Matra Datavision 1993

// Created:	Tue Jun 25 15:42:54 1991
// Author:	JCV
// Modified     04/10/96 : JCT : derivee des surfaces offset utilisation de
//                               CSLib
// Modified     15/11/96 : JPI : ajout equivalent surface pour les surfaces canoniques et modif des methodes D0 D1, ... UIso,VIso
// Modified     18/11/96 : JPI : inversion de l'offsetValue dans UReverse et Vreverse
   
#include <Geom_OffsetSurface.ixx>
#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <BSplSLib.hxx>
#include <BSplCLib.hxx>
#include <CSLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_RangeError.hxx>
#include <Geom_UndefinedValue.hxx>
#include <Geom_UndefinedDerivative.hxx>

#include <Geom_OffsetCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <AdvApprox_ApproxAFunction.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <GeomAbs_IsoType.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_CurveType.hxx>

typedef Handle(Geom_OffsetCurve)   Handle(OffsetCurve);
typedef Geom_OffsetCurve           OffsetCurve;
typedef Handle(Geom_Curve)         Handle(Curve);
typedef Handle(Geom_Surface)       Handle(Surface);
typedef Handle(Geom_OffsetSurface) Handle(OffsetSurface);
typedef Geom_OffsetSurface         OffsetSurface;
typedef Handle(Geom_Geometry)      Handle(Geometry);
typedef gp_Dir  Dir;
typedef gp_Vec  Vec;
typedef gp_Pnt  Pnt;
typedef gp_Trsf Trsf;
typedef gp_XYZ  XYZ;
//=======================================================================
//function : derivatives
//purpose  : 
//=======================================================================

static void derivatives(Standard_Integer MaxOrder,
                        Standard_Integer MinOrder,
                        const Standard_Real U,
                        const Standard_Real V,
                        const Handle(Geom_Surface)& basisSurf,
                        const Standard_Integer Nu,
                        const Standard_Integer Nv,
                        const Standard_Boolean AlongU,
                        const Standard_Boolean AlongV,
                        const Handle(Geom_BSplineSurface)& L,
                        TColgp_Array2OfVec& DerNUV,
                        TColgp_Array2OfVec& DerSurf)
{
      Standard_Integer i,j;
      gp_Pnt P;
      gp_Vec DL1U, DL1V, DL2U , DL2V , DL2UV ,DL3U, DL3UUV, DL3UVV, DL3V;
 
      if (AlongU || AlongV) 
      {
	MaxOrder=0;
	TColgp_Array2OfVec DerSurfL(0,MaxOrder+Nu+1,0,MaxOrder+Nv+1);	
        switch (MinOrder) 
        {
        case 1 :
               L->D1(U, V, P, DL1U, DL1V);
               DerSurfL.SetValue(1, 0, DL1U);
               DerSurfL.SetValue(0, 1, DL1V);
               break;
        case 2 :
               L->D2(U, V, P, DL1U, DL1V, DL2U , DL2V , DL2UV);
               DerSurfL.SetValue(1, 0, DL1U);
               DerSurfL.SetValue(0, 1, DL1V);
               DerSurfL.SetValue(1, 1, DL2UV);
               DerSurfL.SetValue(2, 0, DL2U);
               DerSurfL.SetValue(0, 2, DL2V);
               break;
        case 3 :
               L->D3(U, V, P, DL1U, DL1V, DL2U , DL2V , DL2UV ,DL3U, DL3V ,DL3UUV, DL3UVV);
               DerSurfL.SetValue(1, 0, DL1U);
               DerSurfL.SetValue(0, 1, DL1V);
               DerSurfL.SetValue(1, 1, DL2UV);
               DerSurfL.SetValue(2, 0, DL2U);
               DerSurfL.SetValue(0, 2, DL2V);
               DerSurfL.SetValue(3, 0, DL3U);
               DerSurfL.SetValue(2, 1, DL3UUV);
               DerSurfL.SetValue(1, 2, DL3UVV);
               DerSurfL.SetValue(0, 3, DL3V); 
               break;
        default: 
           break;
	     }

        if(Nu <= Nv) 
	{
	  for(i=0;i<=MaxOrder+1+Nu;i++)
	     for(j=i;j<=MaxOrder+Nv+1;j++)
               if(i+j> MinOrder) 
	       {
	          DerSurfL.SetValue(i,j,L->DN(U,V,i,j));
	          DerSurf.SetValue(i,j,basisSurf->DN(U,V,i,j));
                  if (i!=j && j <= Nu+1) 
                  {
                     DerSurf.SetValue(j,i,basisSurf->DN(U,V,j,i));
                     DerSurfL.SetValue(j,i,L->DN(U,V,j,i));
                  }
	       }
        }
	else
	{
	  for(j=0;j<=MaxOrder+1+Nv;j++)
	     for(i=j;i<=MaxOrder+Nu+1;i++)
               if(i+j> MinOrder) 
	       {
	          DerSurfL.SetValue(i,j,L->DN(U,V,i,j));
	          DerSurf.SetValue(i,j,basisSurf->DN(U,V,i,j));
                  if (i!=j && i <= Nv+1) 
                  {
                     DerSurf.SetValue(j,i,basisSurf->DN(U,V,j,i));
                     DerSurfL.SetValue(j,i,L->DN(U,V,j,i));
                  }
	       }
        }
	for(i=0;i<=MaxOrder+Nu;i++)
	  for(j=0;j<=MaxOrder+Nv;j++)
          {
	    if (AlongU)
	      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurfL,DerSurf));
	    if (AlongV)
	      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf,DerSurfL));
	  }

      }    
      else 
      {

	for(i=0;i<=MaxOrder+Nu+1;i++)
	  for(j=i;j<=MaxOrder+Nv+1;j++)
            if(i+j>MinOrder) 
            {  
          
	      DerSurf.SetValue(i,j,basisSurf->DN(U,V,i,j));
	      if (i!=j) DerSurf.SetValue(j,i,basisSurf->DN(U,V,j,i));
	    }
	for(i=0;i<=MaxOrder+Nu;i++)
	  for(j=0;j<=MaxOrder+Nv;j++)
            {
	       DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));
	    }
      }
   
}
                        

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) Geom_OffsetSurface::Copy () const {

   Handle(OffsetSurface) S;
   S = new OffsetSurface (basisSurf, offsetValue);
   return S;
}





//=======================================================================
//function : Geom_OffsetSurface
//purpose  : 
//=======================================================================

Geom_OffsetSurface::Geom_OffsetSurface ( const Handle(Surface)& S, 
					 const Standard_Real             Offset ) 
 : offsetValue (Offset) 
{
  
  Handle(Geom_OffsetSurface) Off_S;
  Off_S = Handle(Geom_OffsetSurface)::DownCast(S);
  if (!Off_S.IsNull()) {
     offsetValue += Off_S->Offset();
     basisSurf   = Handle(Surface)::DownCast(Off_S->BasisSurface()->Copy());
   }
  else {
    basisSurf = Handle(Surface)::DownCast(S->Copy());
    if (S->Continuity() == GeomAbs_C0) 
      Standard_ConstructionError::Raise("Offset with no C1 Surface");
  }
  equivSurf = Surface();
//
// Tolerance en dur pour l'instant ,mais on devrait la proposer dans le constructeur
// et la mettre en champ, on pourrait utiliser par exemple pour l'extraction d'iso 
// et aussi pour les singularite. Pour les surfaces osculatrices, on l'utilise pour
// detecter si une iso est degeneree.
  Standard_Real Tol = Precision::Confusion(); //0.0001;
  myOscSurf.Init(basisSurf,Tol);
}



//=======================================================================
//function : SetBasisSurface
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::SetBasisSurface (const Handle(Surface)& S) {

  basisSurf = Handle(Surface)::DownCast(S->Copy());
  equivSurf = Surface();
  if (S->Continuity() == GeomAbs_C0)  Standard_ConstructionError::Raise();
}



//=======================================================================
//function : SetOffsetValue
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::SetOffsetValue (const Standard_Real D) {

  offsetValue = D;
  equivSurf = Surface();
}

//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::UReverse () { 

  basisSurf->UReverse(); 
  offsetValue = -offsetValue;
  if(!equivSurf.IsNull()) equivSurf->UReverse();
}


//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::UReversedParameter( const Standard_Real U) const {

  return basisSurf->UReversedParameter( U); 
}


//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::VReverse () {

  basisSurf->VReverse();
  offsetValue = -offsetValue;
  if(!equivSurf.IsNull()) equivSurf->VReverse();
}


//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::VReversedParameter( const Standard_Real V) const {

  return basisSurf->VReversedParameter( V);
}

//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Surface) Geom_OffsetSurface::BasisSurface () const 
{
  return basisSurf;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::Bounds (Standard_Real& U1, 
				 Standard_Real& U2, 
				 Standard_Real& V1, 
				 Standard_Real& V2) const {
   
  basisSurf->Bounds (U1, U2 ,V1, V2);
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom_OffsetSurface::Continuity () const {

  GeomAbs_Shape OffsetShape=GeomAbs_C0;
  switch (basisSurf->Continuity()) {
     case GeomAbs_C0 :        OffsetShape = GeomAbs_C0; break;
     case GeomAbs_G1 :        OffsetShape = GeomAbs_C0; break;
     case GeomAbs_C1 :        OffsetShape = GeomAbs_C0; break;
     case GeomAbs_G2 :        OffsetShape = GeomAbs_C0; break;
     case GeomAbs_C2 :        OffsetShape = GeomAbs_C1; break;
     case GeomAbs_C3 :        OffsetShape = GeomAbs_C2; break;
     case GeomAbs_CN :        OffsetShape = GeomAbs_CN; break;
  }
  return OffsetShape;
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D0 (const Standard_Real U, const Standard_Real V, Pnt& P) const {

  Vec D1U, D1V;
#ifdef CHECK  
  if (basisSurf->Continuity() == GeomAbs_C0) Geom_UndefinedValue::Raise();
#endif
  if (equivSurf.IsNull()){ 
    basisSurf->D1(U, V, P, D1U, D1V);
    SetD0(U,V,P,D1U,D1V);
  }
  else equivSurf->D0(U,V,P); 
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D1 (const Standard_Real U, const Standard_Real V, 
			           Pnt& P, 
			           Vec& D1U, Vec& D1V) const 
{

#ifdef CHECK  
  if (basisSurf->Continuity()==GeomAbs_C0 || 
      basisSurf->Continuity()==GeomAbs_C1) { 
    Geom_UndefinedDerivative::Raise();
  }
#endif
  if (equivSurf.IsNull()) 
  {
    gp_Vec d2u, d2v, d2uv;
    basisSurf->D2(U, V, P, D1U, D1V,  d2u, d2v, d2uv);
    SetD1(U,V,P,D1U,D1V,d2u, d2v, d2uv);
  }
  else {
    equivSurf->D1(U,V,P,D1U,D1V);
  }
}



//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D2 (const Standard_Real U, const Standard_Real V, 
			     Pnt& P, 
			     Vec& D1U, Vec& D1V,
			     Vec& D2U, Vec& D2V, Vec& D2UV) const {
  
#ifdef CHECK  
  GeomAbs_Shape Continuity = basisSurf->Continuity();
  if (Continuity == GeomAbs_C0 || Continuity == GeomAbs_C1 ||
      Continuity == GeomAbs_C2) { Geom_UndefinedDerivative::Raise(); }
#endif
  if (equivSurf.IsNull())
    {
      gp_Vec d3u, d3uuv, d3uvv, d3v;
      basisSurf->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV,
		    d3u,d3v, d3uuv, d3uvv);
      
      SetD2(U,V,P,D1U,D1V,D2U,D2V,D2UV,d3u,d3v, d3uuv, d3uvv);
    }
  else equivSurf->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D3   (const Standard_Real U, const Standard_Real V, 
			       Pnt& P, 
			       Vec& D1U, Vec& D1V, 
			       Vec& D2U, Vec& D2V, Vec& D2UV,
			       Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV) const {
#ifdef CHECK  
  if (!(basisSurf->IsCNu (4) && basisSurf->IsCNv (4))) { 
    Geom_UndefinedDerivative::Raise();
    }
#endif
  if (equivSurf.IsNull())
    {
      
      basisSurf->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV,
		    D3U, D3V, D3UUV, D3UVV);
      SetD3(U, V, P, D1U, D1V, D2U, D2V, D2UV,
		    D3U, D3V, D3UUV, D3UVV);
    }
  else equivSurf->D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}
   



//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Vec Geom_OffsetSurface::DN ( const Standard_Real    U , const Standard_Real    V, 
                             const Standard_Integer Nu, const Standard_Integer Nv) const 
{
  
  Standard_RangeError_Raise_if (Nu < 0 || Nv < 0 || Nu + Nv < 1, " ");
#ifdef CHECK  
  if (!(basisSurf->IsCNu (Nu) && basisSurf->IsCNv (Nv))) { 
    Geom_UndefinedDerivative::Raise();
  }
#endif  
  gp_Vec D(0,0,0);

  if (equivSurf.IsNull())
  {
     Pnt P;
     Vec D1U,D1V;
     basisSurf->D1 (U, V, P, D1U, D1V);

     D = SetDN(U,V,Nu,Nv,D1U,D1V);
   }
  else D=equivSurf->DN(U,V,Nu,Nv);
  return D; 
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D1 
  (const Standard_Real U,  const Standard_Real V,
         Pnt& P,        Pnt& Pbasis, 
         Vec& D1U,      Vec& D1V,      Vec& D1Ubasis , Vec& D1Vbasis,
         Vec& D2Ubasis, Vec& D2Vbasis, Vec& D2UVbasis) const {

     if (basisSurf->Continuity() == GeomAbs_C0 ||
         basisSurf->Continuity() == GeomAbs_C1) { 
       Geom_UndefinedDerivative::Raise();
     }
     basisSurf->D2 (U, V, Pbasis, D1Ubasis, D1Vbasis, D2Ubasis, D2Vbasis,
                    D2UVbasis);
     Vec Ndir = D1Ubasis.Crossed (D1Vbasis);
     Standard_Real R2  = Ndir.SquareMagnitude();
     Standard_Real R   = Sqrt (R2);
     Standard_Real R3  = R * R2;
     Vec DUNdir = D2Ubasis.Crossed (D1Vbasis);
     DUNdir.Add (D1Ubasis.Crossed (D2UVbasis));
     Vec DVNdir = D2UVbasis.Crossed (D1Vbasis);
     DVNdir.Add (D1Ubasis.Crossed (D2Vbasis));
     Standard_Real DRu = Ndir.Dot (DUNdir);
     Standard_Real DRv = Ndir.Dot (DVNdir);
     if (R3 <= gp::Resolution()) {
        if (R2 <= gp::Resolution())  Geom_UndefinedDerivative::Raise();
        DUNdir.Multiply(R);
        DUNdir.Subtract (Ndir.Multiplied (DRu/R));
        DUNdir.Multiply (offsetValue/R2);
        D1U = D1Ubasis.Added (DUNdir);
        DVNdir.Multiply(R);
        DVNdir.Subtract (Ndir.Multiplied (DRv/R));
        DVNdir.Multiply (offsetValue/R2);
        D1V = D1Vbasis.Added (DVNdir);
     }
     else {
       DUNdir.Multiply (offsetValue / R);
       DUNdir.Subtract (Ndir.Multiplied (offsetValue*DRu/R3));
       D1U = D1Ubasis.Added (DUNdir);
       DVNdir.Multiply (offsetValue / R);
       DVNdir.Subtract (Ndir.Multiplied (offsetValue*DRv/R3));
       D1V = D1Vbasis.Added (DVNdir);
     }
     Ndir.Multiply (offsetValue/R);
     P.SetXYZ ((Ndir.XYZ()).Added (Pbasis.XYZ()));
}



//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::D2 
  (const Standard_Real U, const Standard_Real V,
   Pnt& P, Pnt& Pbasis,
   Vec& D1U, Vec& D1V, Vec& D2U, Vec& D2V, Vec& D2UV,
   Vec& D1Ubasis, Vec& D1Vbasis,
   Vec& D2Ubasis, Vec& D2Vbasis, Vec& D2UVbasis, 
   Vec& D3Ubasis, Vec& D3Vbasis, Vec& D3UUVbasis, Vec& D3UVVbasis) const {


     GeomAbs_Shape Continuity = basisSurf->Continuity();
     if (Continuity == GeomAbs_C0 || Continuity == GeomAbs_C1 ||
         Continuity == GeomAbs_C2) { Geom_UndefinedDerivative::Raise(); }
//     Vec D3U, D3V, D3UVV, D3UUV;
     basisSurf->D3 (U, V, P, D1Ubasis, D1Vbasis, D2Ubasis, D2Vbasis, D2UVbasis,
                    D3Ubasis, D3Vbasis, D3UUVbasis, D3UVVbasis);
     Vec Ndir = D1Ubasis.Crossed (D1Vbasis);
     Standard_Real R2  = Ndir.SquareMagnitude();
     Standard_Real R   = Sqrt (R2);
     Standard_Real R3  = R2 * R;
     Standard_Real R4  = R2 * R2;
     Standard_Real R5  = R3 * R2;
     Vec DUNdir = D2Ubasis.Crossed (D1Vbasis);
     DUNdir.Add (D1Ubasis.Crossed (D2UVbasis));
     Vec DVNdir = D2UVbasis.Crossed (D1Vbasis);
     DVNdir.Add (D1Ubasis.Crossed (D2Vbasis));
     Standard_Real DRu = Ndir.Dot (DUNdir);
     Standard_Real DRv = Ndir.Dot (DVNdir);
     Vec D2UNdir = D3Ubasis.Crossed (D1Vbasis);
     D2UNdir.Add (D1Ubasis.Crossed (D3UUVbasis));
     D2UNdir.Add (2.0 * (D2Ubasis.Crossed (D2UVbasis)));
     Vec D2VNdir = D3UVVbasis.Crossed (D1Vbasis);
     D2VNdir.Add (D1Ubasis.Crossed (D3Vbasis));
     D2VNdir.Add (2.0 * (D2UVbasis.Crossed (D2Vbasis)));
     Vec D2UVNdir = D2UVbasis.Crossed (D1Vbasis); 
     D2UVNdir.Add (D1Ubasis.Crossed (D3UVVbasis));
     D2UVNdir.Add (D2Ubasis.Crossed (D2Vbasis));
     Standard_Real D2Ru = Ndir.Dot (D2UNdir) + DUNdir.Dot (DUNdir);
     Standard_Real D2Rv = Ndir.Dot (D2VNdir) + DVNdir.Dot (DVNdir);
     Standard_Real D2Ruv = DVNdir.Dot (DUNdir) + Ndir.Dot (D2UVNdir);

     if (R5 <= gp::Resolution()) {
        //We try another computation but the stability is not very good
        //dixit ISG.
        if (R4 <= gp::Resolution()) Geom_UndefinedDerivative::Raise();
        // V2 = P" (U) :
        // Standard_Real R4 = R2 * R2;
        D2UNdir.Subtract (DUNdir.Multiplied (2.0 * DRu / R2));
        D2UNdir.Subtract (Ndir.Multiplied (D2Ru/R2));
        D2UNdir.Add (Ndir.Multiplied (3.0 * DRu * DRu / R4));
        D2UNdir.Multiply (offsetValue / R);
        D2U = D2Ubasis.Added (D2UNdir);
        D2VNdir.Subtract (DVNdir.Multiplied (2.0 * DRv / R2));
        D2VNdir.Subtract (Ndir.Multiplied (D2Rv/R2));
        D2VNdir.Add (Ndir.Multiplied (3.0 * DRv * DRv / R4));
        D2VNdir.Multiply (offsetValue / R);
        D2V = D2Vbasis.Added (D2VNdir);

        D2UVNdir.Subtract (DUNdir.Multiplied (DRv / R2));
        D2UVNdir.Subtract (DVNdir.Multiplied (DRu / R2));
        D2UVNdir.Subtract (Ndir.Multiplied (D2Ruv / R2));
        D2UVNdir.Add (Ndir.Multiplied (3.0 * DRu * DRv / R4));
        D2UVNdir.Multiply (offsetValue / R);
        D2UV = D2UVbasis.Added (D2UVNdir);

        DUNdir.Multiply(R);
        DUNdir.Subtract (Ndir.Multiplied (DRu/R));
        DUNdir.Multiply (offsetValue/R2);
        D1U = D1Ubasis.Added (DUNdir);
        DVNdir.Multiply(R);
        DVNdir.Subtract (Ndir.Multiplied (DRv/R));
        DVNdir.Multiply (offsetValue/R2);
        D1V = D1Vbasis.Added (DVNdir);
     }
     else {
       D2UNdir.Multiply (offsetValue/R);
       D2UNdir.Subtract (DUNdir.Multiplied (2.0 * offsetValue * DRu / R3));
       D2UNdir.Subtract (Ndir.Multiplied (offsetValue * D2Ru / R3));
       D2UNdir.Add (Ndir.Multiplied (offsetValue * 3.0 * DRu * DRu / R5));
       D2U = D2Ubasis.Added (D2UNdir);

       D2VNdir.Multiply (offsetValue/R);
       D2VNdir.Subtract (DVNdir.Multiplied (2.0 * offsetValue * DRv / R3));
       D2VNdir.Subtract (Ndir.Multiplied (offsetValue * D2Rv / R3));
       D2VNdir.Add (Ndir.Multiplied (offsetValue * 3.0 * DRv * DRv / R5));
       D2V = D2Vbasis.Added (D2VNdir);

       D2UVNdir.Multiply (offsetValue/R);
       D2UVNdir.Subtract (DUNdir.Multiplied (offsetValue * DRv / R3));
       D2UVNdir.Subtract (DVNdir.Multiplied (offsetValue * DRu / R3));
       D2UVNdir.Subtract (Ndir.Multiplied (offsetValue * D2Ruv / R3));
       D2UVNdir.Add (Ndir.Multiplied (3.0 * offsetValue * DRu * DRv / R5));
       D2UV = D2UVbasis.Added (D2UVNdir);

       DUNdir.Multiply (offsetValue / R);
       DUNdir.Subtract (Ndir.Multiplied (offsetValue*DRu/R3));
       D1U = D1Ubasis.Added (DUNdir);
       DVNdir.Multiply (offsetValue / R);
       DVNdir.Subtract (Ndir.Multiplied (offsetValue*DRv/R3));
       D1V = D1Vbasis.Added (DVNdir);
     }
     Ndir.Multiply (offsetValue/R);
     P.SetXYZ ((Ndir.XYZ()).Added (Pbasis.XYZ()));
}



//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::Offset () const { 

  return offsetValue; 
}

//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::LocalD0 (const Standard_Real    U,
				   const Standard_Real    V, 
				   const Standard_Integer USide,
				   const Standard_Integer VSide,
				  gp_Pnt&          P     )  const
{
  if (equivSurf.IsNull()) {
    Vec D1U, D1V;
    Handle(Geom_Surface) Basis = basisSurf;

    // if Basis is Trimmed we take the basis's basis
    Handle(Geom_RectangularTrimmedSurface) RTS; 
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(Basis); 
    if (!RTS.IsNull()) {
      Basis = RTS->BasisSurface();
    }
    
    // BSpline case 
    Handle(Geom_BSplineSurface) BSplS; 
    BSplS = Handle(Geom_BSplineSurface)::DownCast(Basis);
    if (!BSplS.IsNull()) {
      Vec D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV; 
      LocateSides(U,V,USide,VSide,BSplS,1,P,D1U,D1V,D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD0(U,V,P,D1U,D1V);
      return;
    }

    // Extrusion case 
    Handle( Geom_SurfaceOfLinearExtrusion) SE;
    SE= Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Basis);

    if (!SE.IsNull()) {
      SE->LocalD1(U,V,USide,P,D1U,D1V);
      SetD0(U,V,P,D1U,D1V);
      return;
    }

    // Revolution case 
    Handle(Geom_SurfaceOfRevolution) SR;
    SR = Handle(Geom_SurfaceOfRevolution)::DownCast(Basis);
    if (!SR.IsNull()) {
      SR->LocalD1(U,V, VSide,P,D1U,D1V);
      SetD0(U,V, P, D1U,D1V);
      return;
    }
    
    // General cases
    basisSurf->D1(U, V, P, D1U, D1V);
    SetD0(U,V, P, D1U,D1V);
  }
  else
    equivSurf-> D0(U,V,P);
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::LocalD1 (const Standard_Real    U, 
				  const Standard_Real    V,
				  const Standard_Integer USide, 
				  const Standard_Integer VSide,
				         gp_Pnt&          P,
				         gp_Vec&          D1U, 
				         gp_Vec&          D1V)     const
{
  if (equivSurf.IsNull()) {
    Vec D2U,D2V,D2UV ;
    Handle(Geom_Surface) Basis = basisSurf;

    // if Basis is Trimmed we take the basis's basis
    Handle(Geom_RectangularTrimmedSurface) RTS;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(Basis); 
    if (!RTS.IsNull()) {
      Basis = RTS->BasisSurface();
    }
    
    // BSpline case 
    Handle(Geom_BSplineSurface) BSplS; 
    BSplS = Handle(Geom_BSplineSurface)::DownCast(Basis);
    if (!BSplS.IsNull()) {
      Vec D3U,D3V,D3UUV,D3UVV; 
      LocateSides(U,V,USide,VSide,BSplS,2,P,D1U,D1V,D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD1(U,V,P,D1U,D1V,D2U,D2V,D2UV);
      return;
    }

    // Extrusion case 
    Handle( Geom_SurfaceOfLinearExtrusion) SE;
    SE= Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Basis);

    if (!SE.IsNull()) {
      SE->LocalD2(U,V,USide,P,D1U,D1V,D2U,D2V,D2UV);
      SetD1(U,V,P,D1U,D1V,D2U,D2V,D2UV);
      return;
    }

    // Revolution case 
    Handle(Geom_SurfaceOfRevolution) SR;
    SR = Handle(Geom_SurfaceOfRevolution)::DownCast(Basis);
    if (!SR.IsNull()) {
      SR->LocalD2(U,V, VSide,P,D1U,D1V,D2U,D2V,D2UV);
      SetD1(U,V, P, D1U,D1V,D2U,D2V,D2UV);
      return;
    }
    
    // General cases
    basisSurf->D2(U, V, P, D1U, D1V,D2U,D2V,D2UV);
    SetD1(U,V, P, D1U,D1V,D2U,D2V,D2UV);
  }
  else
    equivSurf-> D1(U,V,P,D1U,D1V); 
}

//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::LocalD2 (const Standard_Real    U,
				   const Standard_Real    V,
				   const Standard_Integer USide, 
				   const Standard_Integer VSide,
				         gp_Pnt&          P,
				         gp_Vec&          D1U,
				         gp_Vec&          D1V,
				         gp_Vec&          D2U,
				         gp_Vec&          D2V,
				         gp_Vec&          D2UV) const
{
  if (equivSurf.IsNull()) {
    Handle(Geom_Surface) Basis = basisSurf;
    Vec D3U,D3V,D3UUV,D3UVV;

    // if Basis is Trimmed we take the basis's basis
    Handle(Geom_RectangularTrimmedSurface) RTS;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(Basis); 
    if (!RTS.IsNull()) {
      Basis = RTS->BasisSurface();
    }
    
    // BSpline case 
    Handle(Geom_BSplineSurface) BSplS; 
    BSplS = Handle(Geom_BSplineSurface)::DownCast(Basis);
    if (!BSplS.IsNull()) {
      LocateSides(U,V,USide,VSide,BSplS,3,P,D1U,D1V,D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD2(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      return;
    }

    // Extrusion case 
    Handle( Geom_SurfaceOfLinearExtrusion) SE;
    SE= Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Basis);

    if (!SE.IsNull()) {
      SE->LocalD3(U,V,USide,P,D1U,D1V,
		  D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD2(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      return;
    }

    // Revolution case 
    Handle(Geom_SurfaceOfRevolution) SR;
    SR = Handle(Geom_SurfaceOfRevolution)::DownCast(Basis);
    if (!SR.IsNull()) {
      SR->LocalD3(U,V, VSide,P,D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
      SetD2(U,V, P, D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
      return;
    }
    
    // General cases
    basisSurf->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV,D3U, D3V, D3UUV, D3UVV);
    SetD2(U,V, P, D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
  }  
  else
    equivSurf->  D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
}
//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::LocalD3 (const Standard_Real     U, 
				   const Standard_Real    V,
				   const Standard_Integer USide, 
				   const Standard_Integer VSide,
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
  if (equivSurf.IsNull()) {
    Handle(Geom_Surface) Basis = basisSurf;

    // if Basis is Trimmed we take the basis's basis
    Handle(Geom_RectangularTrimmedSurface) RTS;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(Basis); 
    if (!RTS.IsNull()) {
      Basis = RTS->BasisSurface();
    }
    
    // BSpline case 
    Handle(Geom_BSplineSurface) BSplS; 
    BSplS = Handle(Geom_BSplineSurface)::DownCast(Basis);
    if (!BSplS.IsNull()) {
      LocateSides(U,V,USide,VSide,BSplS,3,P,D1U,D1V,D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      return;
    }

    // Extrusion case 
    Handle( Geom_SurfaceOfLinearExtrusion) SE;
    SE= Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Basis);

    if (!SE.IsNull()) {
      SE->LocalD3(U,V,USide,P,D1U,D1V,
		  D2U,D2V,D2UV,
		  D3U,D3V,D3UUV,D3UVV);
      SetD3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      return;
    }

    // Revolution case 
    Handle(Geom_SurfaceOfRevolution) SR;
    SR = Handle(Geom_SurfaceOfRevolution)::DownCast(Basis);
    if (!SR.IsNull()) {
      SR->LocalD3(U,V, VSide,P,D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
      SetD3(U,V, P, D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
      return;
    }
    
    // General cases
    basisSurf->D3(U, V, P, D1U, D1V, D2U, D2V, D2UV,D3U, D3V, D3UUV, D3UVV);
    SetD3(U,V, P, D1U,D1V, D2U,D2V,D2UV, D3U,D3V,D3UUV,D3UVV);
  }
  else
    equivSurf-> D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
}

//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

gp_Vec Geom_OffsetSurface::LocalDN  (const Standard_Real    U, 
				     const Standard_Real    V,
				     const Standard_Integer USide,
				     const Standard_Integer VSide,
				     const Standard_Integer Nu,
				     const Standard_Integer Nv) const
{  
  gp_Vec D(0,0,0);
  
  if(equivSurf.IsNull()) {

    Vec D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV; 
    Pnt P;
    Handle(Geom_Surface) Basis = basisSurf;
    
    //   if Basis is Trimmed we make the basis`s basis
    Handle(Geom_RectangularTrimmedSurface) RTS;
    RTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(Basis);
    
    if (!RTS.IsNull())  {
      Basis = RTS -> BasisSurface();
    }
    
    //BSpline case 
    Handle(Geom_BSplineSurface) BSplS;
    BSplS = Handle(Geom_BSplineSurface)::DownCast(Basis);
    
    if(!BSplS.IsNull()) {
      LocateSides(U,V,USide,VSide,BSplS,1,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      return  D = SetDN(U,V,Nu,Nv,D1U,D1V);
    }
    
    //Extrusion case
    Handle( Geom_SurfaceOfLinearExtrusion) SE;
    SE = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Basis);
    
    if(!SE.IsNull()) {
      
      SE->LocalD1(U,V,USide,P,D1U,D1V);
      D = SetDN(U,V,Nu,Nv,D1U,D1V);
      return D;
    }
    
    //Revolution case
    Handle(Geom_SurfaceOfRevolution) SR;
    SR = Handle(Geom_SurfaceOfRevolution)::DownCast(Basis);
    
    if(!SR.IsNull()) {
      D = SR->LocalDN(U,V,VSide,Nu,Nv);
      return D = SetDN(U,V,Nu,Nv,D1U,D1V);
    }
    
    //General cases 
    D = basisSurf->DN(U,V,Nu,Nv);
    return D = SetDN(U,V,Nu,Nv,D1U,D1V);
  }
  else 
    return D = equivSurf->DN(U,V,Nu,Nv);
}

//=======================================================================
//function : LocateSides
//purpose  : 
//=======================================================================
void Geom_OffsetSurface::LocateSides(const Standard_Real U,
				     const Standard_Real V,
				     const Standard_Integer USide,
				     const Standard_Integer VSide,
				     const Handle(Geom_BSplineSurface)& BSplS,
				     const Standard_Integer NDir,
				     gp_Pnt& P,
				     gp_Vec& D1U,gp_Vec& D1V,
				     gp_Vec& D2U,gp_Vec& D2V,gp_Vec& D2UV,
				     gp_Vec& D3U,gp_Vec& D3V,gp_Vec& D3UUV,gp_Vec& D3UVV   ) const
{
  Standard_Boolean UIsKnot=Standard_False, VIsKnot=Standard_False ;
  Standard_Integer Ideb, Ifin, IVdeb, IVfin;
  Standard_Real ParTol=Precision::PConfusion()/10.;
  BSplS->Geom_BSplineSurface::LocateU(U,ParTol,Ideb, Ifin, Standard_False); 
  BSplS->Geom_BSplineSurface::LocateV(V,ParTol,IVdeb,IVfin,Standard_False);   
 
  if(Ideb == Ifin ) { //knot

    if(USide == 1) {Ifin++; UIsKnot=Standard_True;}

    else if(USide == -1) {Ideb--; UIsKnot=Standard_True;}

    else {Ideb--; Ifin++;} //USide == 0

  }

  if(Ideb < BSplS->FirstUKnotIndex()) {Ideb = BSplS->FirstUKnotIndex(); Ifin = Ideb + 1;}

  if(Ifin > BSplS->LastUKnotIndex()) {Ifin = BSplS->LastUKnotIndex(); Ideb = Ifin - 1;}


  if(IVdeb == IVfin ) { //knot

    if(VSide == 1) {IVfin++; VIsKnot=Standard_True;}

    else if(VSide == -1) {IVdeb--; VIsKnot=Standard_True;}

    else {IVdeb--; IVfin++;} //VSide == 0

  }

  if(IVdeb < BSplS->FirstVKnotIndex()) {IVdeb = BSplS->FirstVKnotIndex(); IVfin = IVdeb + 1;}

  if(IVfin > BSplS->LastVKnotIndex()) {IVfin = BSplS->LastVKnotIndex(); IVdeb = IVfin - 1;}

    
  if((UIsKnot)||(VIsKnot))
    switch(NDir)
      {
      case 0 :  BSplS->Geom_BSplineSurface::LocalD0(U,V,Ideb,Ifin,IVdeb,IVfin,P); break;
      case 1 :  BSplS->Geom_BSplineSurface::LocalD1(U,V,Ideb,Ifin,IVdeb,IVfin,P,D1U,D1V); break;
      case 2 :  BSplS->Geom_BSplineSurface::LocalD2(U,V,Ideb,Ifin,IVdeb,IVfin,P,D1U,D1V,D2U,D2V,D2UV); break;
      case 3 :  BSplS->Geom_BSplineSurface::LocalD3(U,V,Ideb,Ifin,IVdeb,IVfin,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); break;
      }else   switch(NDir) 
	{
	case 0 :  basisSurf->D0(U,V,P); break;
	case 1 :  basisSurf->D1(U,V,P,D1U,D1V); break;
	case 2 :  basisSurf->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV); break;
	case 3 :  basisSurf->D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV); break;
	}
}


////*************************************************
////
////   EVALUATOR FOR THE ISO-CURVE APPROXIMATION
////
////*************************************************

class Geom_OffsetSurface_UIsoEvaluator : public AdvApprox_EvaluatorFunction
{
 public:
  Geom_OffsetSurface_UIsoEvaluator (const Handle(Geom_Surface)& theSurface,
                                    Standard_Real theU)
  : CurrentSurface(theSurface), IsoPar(theU) {}
                                      
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Geom_Surface) CurrentSurface;
  Standard_Real IsoPar;
};

void Geom_OffsetSurface_UIsoEvaluator::Evaluate(Standard_Integer *,/*Dimension*/
                                                Standard_Real     /*StartEnd*/[2],
                                                Standard_Real    *Parameter,
                                                Standard_Integer *DerivativeRequest,
                                                Standard_Real    *Result,
                                                Standard_Integer *ReturnCode) 
{ 
  gp_Pnt P;
  if (*DerivativeRequest == 0) {
    P = CurrentSurface->Value(IsoPar,*Parameter);
    for (Standard_Integer i = 0; i < 3; i++) 
      Result[i] = P.Coord(i+1);
  }
  else {
    gp_Vec DU,DV;
    CurrentSurface->D1(IsoPar,*Parameter,P,DU,DV);
    for (Standard_Integer i = 0; i < 3; i++)
      Result[i] = DV.Coord(i+1);
  }
  *ReturnCode = 0 ;
}

class Geom_OffsetSurface_VIsoEvaluator : public AdvApprox_EvaluatorFunction
{
 public:
  Geom_OffsetSurface_VIsoEvaluator (const Handle(Geom_Surface)& theSurface,
                                    Standard_Real theV)
  : CurrentSurface(theSurface), IsoPar(theV) {}
                                      
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Handle(Geom_Surface) CurrentSurface;
  Standard_Real IsoPar;
};

void Geom_OffsetSurface_VIsoEvaluator::Evaluate(Standard_Integer *,/*Dimension*/
                                                Standard_Real     /*StartEnd*/[2],
                                                Standard_Real    *Parameter,
                                                Standard_Integer *DerivativeRequest,
                                                Standard_Real    *Result,
                                                Standard_Integer *ReturnCode) 
{ 
  gp_Pnt P;
  if (*DerivativeRequest == 0) {
    P = CurrentSurface->Value(*Parameter,IsoPar);
    for (Standard_Integer i = 0; i < 3; i++) 
      Result[i] = P.Coord(i+1);
  }
  else {
    gp_Vec DU,DV;
    CurrentSurface->D1(*Parameter,IsoPar,P,DU,DV);
    for (Standard_Integer i = 0; i < 3; i++)
      Result[i] = DU.Coord(i+1);
  }
  *ReturnCode = 0 ;
}

//=======================================================================
//function : UIso
//purpose  : The Uiso or the VIso of an OffsetSurface can't be clearly 
//           exprimed as a curve from Geom. So, to extract the U or VIso
//           an Approximation is needed. This approx always will return a 
//           BSplineCurve from Geom.
//=======================================================================

Handle(Geom_Curve) Geom_OffsetSurface::UIso (const Standard_Real UU) const 
{
  if (equivSurf.IsNull()) {
    Standard_Integer Num1 = 0;
    Standard_Integer Num2 = 0;
    Standard_Integer Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1;
    Handle(TColStd_HArray1OfReal) T2;
    Handle(TColStd_HArray1OfReal) T3 =
      new TColStd_HArray1OfReal(1,Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1,U2,V1,V2;
    Bounds(U1,U2,V1,V2);
    GeomAbs_Shape Cont = GeomAbs_C1;
    Standard_Integer MaxSeg = 100, MaxDeg =14;

    Geom_OffsetSurface_UIsoEvaluator ev (this, UU);
    AdvApprox_ApproxAFunction Approx(Num1, Num2, Num3, T1, T2, T3,
                                     V1, V2, Cont,
                                     MaxDeg,MaxSeg, ev);

    Standard_ConstructionError_Raise_if (!Approx.IsDone(),
                                         " Geom_OffsetSurface : UIso");

    Standard_Integer NbPoles = Approx.NbPoles();

    TColgp_Array1OfPnt      Poles( 1, NbPoles);
    TColStd_Array1OfReal    Knots( 1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults( 1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = 
      new Geom_BSplineCurve( Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->UIso(UU);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::Value 
  ( const Standard_Real U,  const Standard_Real V,
//          Pnt& P,        Pnt& Pbasis, 
          Pnt& P,        Pnt& , 
          Vec& D1Ubasis, Vec& D1Vbasis) const {

  if (basisSurf->Continuity() == GeomAbs_C0)  
    Geom_UndefinedValue::Raise();
  
  SetD0(U,V,P,D1Ubasis,D1Vbasis);
}



//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) Geom_OffsetSurface::VIso (const Standard_Real VV) const 
{
  if (equivSurf.IsNull()) {
    Standard_Integer Num1 = 0;
    Standard_Integer Num2 = 0;
    Standard_Integer Num3 = 1;
    Handle(TColStd_HArray1OfReal) T1;
    Handle(TColStd_HArray1OfReal) T2;
    Handle(TColStd_HArray1OfReal) T3 =
      new TColStd_HArray1OfReal(1,Num3);
    T3->Init(Precision::Approximation());
    Standard_Real U1,U2,V1,V2;
    Bounds(U1,U2,V1,V2);
    GeomAbs_Shape Cont = GeomAbs_C1;
    Standard_Integer MaxSeg = 100, MaxDeg =14;  

    Geom_OffsetSurface_VIsoEvaluator ev (this, VV);
    AdvApprox_ApproxAFunction Approx (Num1, Num2, Num3, T1, T2, T3,
                                      U1, U2, Cont, MaxDeg, MaxSeg, ev);

    Standard_ConstructionError_Raise_if (!Approx.IsDone(),
                                         " Geom_OffsetSurface : VIso");

    TColgp_Array1OfPnt      Poles( 1, Approx.NbPoles());
    TColStd_Array1OfReal    Knots( 1, Approx.NbKnots());
    TColStd_Array1OfInteger Mults( 1, Approx.NbKnots());

    Approx.Poles(1, Poles);
    Knots = Approx.Knots()->Array1();
    Mults = Approx.Multiplicities()->Array1();

    Handle(Geom_BSplineCurve) C = 
      new Geom_BSplineCurve( Poles, Knots, Mults, Approx.Degree());
    return C;
  }
  else
    return equivSurf->VIso(VV);
}


//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsCNu (const Standard_Integer N) const {

   Standard_RangeError_Raise_if (N < 0, " ");
   return basisSurf->IsCNu (N+1);
}


//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsCNv (const Standard_Integer N) const {

  Standard_RangeError_Raise_if (N < 0, " ");
  return basisSurf->IsCNv (N+1);
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsUPeriodic () const 
{
  return basisSurf->IsUPeriodic();
}


//=======================================================================
//function : UPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}


//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsVPeriodic () const 
{
  return basisSurf->IsVPeriodic();
}


//=======================================================================
//function : VPeriod
//purpose  : 
//=======================================================================

Standard_Real Geom_OffsetSurface::VPeriod() const
{
  return basisSurf->VPeriod();
}


//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsUClosed () const { 

  Standard_Boolean UClosed;
  Handle(Surface) SBasis = BasisSurface();

  if (SBasis->IsKind (STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) St = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Surface) S = Handle(Surface)::DownCast(St->BasisSurface());
    if (S->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      UClosed = SBasis->IsUClosed();
    }
    else if (S->IsKind (STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) { 
      Handle(Geom_SurfaceOfLinearExtrusion) Extru = 
	Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);

      Handle(Curve) C = Extru->BasisCurve();
      if (C->IsKind (STANDARD_TYPE(Geom_Circle)) || C->IsKind (STANDARD_TYPE(Geom_Ellipse))) {
        UClosed = SBasis->IsUClosed();
      }
      else { UClosed = Standard_False; }
    }
    else if (S->IsKind (STANDARD_TYPE(Geom_SurfaceOfRevolution))) { 
      UClosed = SBasis->IsUClosed();
    }
    else { UClosed = Standard_False; }
  }
  else {
    if (SBasis->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      UClosed = SBasis->IsUClosed();
    }
    else if (SBasis->IsKind (STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) { 
      Handle(Geom_SurfaceOfLinearExtrusion) Extru = 
	Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(SBasis);

      Handle(Curve) C = Extru->BasisCurve();
      UClosed = (C->IsKind(STANDARD_TYPE(Geom_Circle)) || C->IsKind(STANDARD_TYPE(Geom_Ellipse)));
    }
    else if (SBasis->IsKind (STANDARD_TYPE(Geom_SurfaceOfRevolution))) { 
      UClosed = Standard_True; 
    }
    else { UClosed = Standard_False; }
  }  
  return UClosed;
}


//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OffsetSurface::IsVClosed () const {  

  Standard_Boolean VClosed;
  Handle(Surface) SBasis = BasisSurface();

  if (SBasis->IsKind (STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) St = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(SBasis);

    Handle(Surface) S = Handle(Surface)::DownCast(St->BasisSurface());
    if (S->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      VClosed = SBasis->IsVClosed();
    }
    else { VClosed = Standard_False; }
  }
  else {
    if (SBasis->IsKind (STANDARD_TYPE(Geom_ElementarySurface))) {
      VClosed = SBasis->IsVClosed();
    }
    else { VClosed = Standard_False; }
  }
  return VClosed;
}


//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::Transform (const Trsf& T) 
{
   basisSurf->Transform (T);
   offsetValue *= T.ScaleFactor();
   equivSurf.Nullify();
}

//=======================================================================
//function : TransformParameters
//purpose  : 
//=======================================================================

void Geom_OffsetSurface::TransformParameters(Standard_Real& U,
					     Standard_Real& V,
					     const gp_Trsf& T) 
const
{
  basisSurf->TransformParameters(U,V,T);
  if(!equivSurf.IsNull()) equivSurf->TransformParameters(U,V,T);
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

gp_GTrsf2d Geom_OffsetSurface::ParametricTransformation
(const gp_Trsf& T) const
{
  return basisSurf->ParametricTransformation(T);
}

//=======================================================================
//function : Surface
//purpose  : Trouve si elle existe, une surface non offset, equivalente
//           a l'offset surface.
//=======================================================================
Handle(Geom_Surface) Geom_OffsetSurface::Surface() const 
{
  if (offsetValue == 0.0) return  basisSurf; // Cas direct 

  Standard_Real Tol = Precision::Confusion();
  Handle(Geom_Surface) Result, Base;
  Result.Nullify();
  Handle(Standard_Type) TheType = basisSurf->DynamicType();
  Standard_Boolean IsTrimmed;
  Standard_Real U1, V1, U2, V2;

  // Preambule pour les surface trimmes
  if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    Handle(Geom_RectangularTrimmedSurface) S = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(basisSurf);
    Base = S->BasisSurface();
    TheType = Base->DynamicType();
    S->Bounds(U1,U2,V1,V2);  
    IsTrimmed = Standard_True;
  }
   else {
     IsTrimmed = Standard_False;
     Base = basisSurf;
   }

  // Traite les surfaces cannonique
    if (TheType == STANDARD_TYPE(Geom_Plane)) 
  {
    Handle(Geom_Plane) P =
      Handle(Geom_Plane)::DownCast(Base);
    gp_Vec T = P->Position().XDirection()^P->Position().YDirection();
    T *= offsetValue;
    Result = Handle(Geom_Plane)::DownCast(P->Translated(T));
  }
  else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface)) 
  {
    Handle(Geom_CylindricalSurface) C =
      Handle(Geom_CylindricalSurface)::DownCast(Base);
    Standard_Real Radius = C->Radius();
    gp_Ax3 Axis = C->Position();
    if (Axis.Direct()) 
      Radius += offsetValue;
    else 
      Radius -= offsetValue;
    if ( Radius >= Tol ) {
      Result = new Geom_CylindricalSurface( Axis, Radius);
    }
    else if ( Radius <= -Tol ){
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),PI);
      Result = new Geom_CylindricalSurface( Axis, Abs(Radius));
      Result->UReverse();
    }
    else 
    {
// surface degeneree      
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ConicalSurface)) 
  {
    Handle(Geom_ConicalSurface) C =
      Handle(Geom_ConicalSurface)::DownCast(Base);
    Standard_Real Alpha = C->SemiAngle();
    Standard_Real Radius = C->RefRadius() + offsetValue * Cos(Alpha);
    gp_Ax3 Axis = C->Position();
    if ( Radius >= 0.) {
      gp_Vec Z( Axis.Direction());
      Z *= - offsetValue * Sin(Alpha);
      Axis.Translate(Z);
      Result = new Geom_ConicalSurface(Axis, Alpha, Radius);
    }
    else 
    {
// surface degeneree      
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_SphericalSurface)) {
    Handle(Geom_SphericalSurface) S = 
      Handle(Geom_SphericalSurface)::DownCast(Base);
    Standard_Real Radius = S->Radius();
    gp_Ax3 Axis = S->Position();
    if (Axis.Direct()) 
      Radius += offsetValue;
    else 
      Radius -= offsetValue;
    if ( Radius >= Tol) {
      Result = new Geom_SphericalSurface(Axis, Radius);
    }
    else if ( Radius <= -Tol ) {
      Axis.Rotate(gp_Ax1(Axis.Location(),Axis.Direction()),PI);
      Axis.ZReverse();
      Result = new Geom_SphericalSurface(Axis, -Radius);
      Result->UReverse();
    }
    else {
//      surface degeneree
    }
  }
  else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface)) 

  {
    Handle(Geom_ToroidalSurface) 
      S = Handle(Geom_ToroidalSurface)::DownCast(Base);
    Standard_Real MajorRadius = S->MajorRadius();
    Standard_Real MinorRadius = S->MinorRadius();
    gp_Ax3 Axis = S->Position();
    if (MinorRadius <= MajorRadius) 
    {  
      if (Axis.Direct())
	MinorRadius += offsetValue;
      else 
	MinorRadius -= offsetValue;
      if (MinorRadius >= Tol) 
	Result = new Geom_ToroidalSurface(Axis,MajorRadius,MinorRadius);
//      else if (MinorRadius <= -Tol) 
//        Result->UReverse();
      else 
      {
//	surface degeneree
      }
    }
  }

  // S'il le faut on trimme le resultat
  if (IsTrimmed && !Result.IsNull()) {
    Base = Result;
    Result = new Geom_RectangularTrimmedSurface (Base, U1, U2, V1,V2);
  }

  return Result;
}

Standard_Boolean Geom_OffsetSurface::UOsculatingSurface(const Standard_Real U,
							const Standard_Real V,
							Standard_Boolean& t,
							Handle(Geom_BSplineSurface)& L) const
{
  return myOscSurf.UOscSurf(U,V,t,L);
}

Standard_Boolean Geom_OffsetSurface::VOsculatingSurface(const Standard_Real U,
							const Standard_Real V,
							Standard_Boolean& t,
							Handle(Geom_BSplineSurface)& L) const
{
  return myOscSurf.VOscSurf(U,V,t,L);
}


//=======================================================================
//function : 
//purpose  : private
//=======================================================================
void Geom_OffsetSurface::SetD0(const Standard_Real U, const Standard_Real V, 
			       Pnt& P,
			       const Vec& D1U, const Vec& D1V)const 
{
  Standard_Boolean AlongU = Standard_False,
                   AlongV = Standard_False;
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean IsOpposite=Standard_False;
  Standard_Real signe = 1.;
  AlongU = UOsculatingSurface(U,V,IsOpposite,L); 
  AlongV = VOsculatingSurface(U,V,IsOpposite,L);
  if ((AlongV || AlongU) && IsOpposite) signe = -1;

  Standard_Real MagTol=0.000000001;
  Dir Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1U, D1V, MagTol, NStatus, Normal);

  if ((NStatus == CSLib_Defined)) // akm - only in singularities && !AlongU && !AlongV) 
    {
      P.SetXYZ(P.XYZ() + offsetValue * Normal.XYZ());
    }
  else 
    {
      Standard_Integer MaxOrder=3;
      TColgp_Array2OfVec DerNUV(0,MaxOrder,0,MaxOrder);
      TColgp_Array2OfVec DerSurf(0,MaxOrder+1,0,MaxOrder+1);
      Standard_Integer OrderU,OrderV;
      Standard_Real Umin,Umax,Vmin,Vmax;
      Bounds(Umin,Umax,Vmin,Vmax);
      DerSurf.SetValue(1, 0, D1U);
      DerSurf.SetValue(0, 1, D1V);
      derivatives(MaxOrder,1,U,V,basisSurf,0,0,AlongU,AlongV,L,DerNUV,DerSurf);
      
      CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,NStatus,Normal,OrderU,OrderV);
      if (NStatus == CSLib_Defined) 
	P.SetXYZ(P.XYZ() + offsetValue * signe * Normal.XYZ());
      else 
	Geom_UndefinedValue::Raise();
      
    }
}

//=======================================================================
//function : 
//purpose  : private
//=======================================================================/
void Geom_OffsetSurface::SetD1(const Standard_Real U, const Standard_Real V, 
			       Pnt& P, 
			       Vec& D1U, Vec& D1V,
			       const Vec& d2u, const Vec& d2v, const Vec& d2uv ) const
{
  
  Standard_Real MagTol=0.000000001;
  Dir Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1U, D1V, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+1,0,MaxOrder+1);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+2,0,MaxOrder+2);
  Standard_Real Umin,Umax,Vmin,Vmax;
  Bounds(Umin,Umax,Vmin,Vmax);
  DerSurf.SetValue(1, 0, D1U);
  DerSurf.SetValue(0, 1, D1V);
  DerSurf.SetValue(1, 1, d2uv);
  DerSurf.SetValue(2, 0, d2u);
  DerSurf.SetValue(0, 2, d2v);
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean AlongU = Standard_False,
  AlongV = Standard_False;
  Standard_Boolean IsOpposite=Standard_False;
  Standard_Real signe = 1.;
  AlongU = UOsculatingSurface(U,V,IsOpposite,L); 
  AlongV = VOsculatingSurface(U,V,IsOpposite,L);
  if ((AlongV || AlongU) && IsOpposite) signe = -1;
  derivatives(MaxOrder,2,U,V,basisSurf,1,1,AlongU,AlongV,L,DerNUV,DerSurf);
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) Geom_UndefinedValue::Raise();
  
  P.SetXYZ(P.XYZ() + offsetValue * signe * Normal.XYZ());
  
  D1U = DerSurf(1,0)
    + offsetValue * signe * CSLib::DNNormal(1,0,DerNUV,OrderU,OrderV);
  D1V = DerSurf(0,1)
    + offsetValue * signe * CSLib::DNNormal(0,1,DerNUV,OrderU,OrderV);
  
}

//=======================================================================
//function : 
//purpose  : private
//=======================================================================/
void Geom_OffsetSurface::SetD2(const Standard_Real U, const Standard_Real V, 
			       Pnt& P, 
			       Vec& D1U, Vec& D1V,
			       Vec& D2U, Vec& D2V, Vec& D2UV,
			       const Vec& d3u, const Vec& d3v,
			       const Vec& d3uuv, const Vec& d3uvv  ) const 
{
  Standard_Real MagTol=0.000000001;
  Dir Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1U, D1V, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+2,0,MaxOrder+2);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+3,0,MaxOrder+3);
  Standard_Real Umin,Umax,Vmin,Vmax;
  Bounds(Umin,Umax,Vmin,Vmax);
  DerSurf.SetValue(1, 0, D1U);
  DerSurf.SetValue(0, 1, D1V);
  DerSurf.SetValue(1, 1, D2UV);
  DerSurf.SetValue(2, 0, D2U);
  DerSurf.SetValue(0, 2, D2V);
  DerSurf.SetValue(3, 0, d3u);
  DerSurf.SetValue(2, 1, d3uuv);
  DerSurf.SetValue(1, 2, d3uvv);
  DerSurf.SetValue(0, 3, d3v);
  //*********************
  
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean AlongU = Standard_False,
  AlongV = Standard_False;
  Standard_Boolean IsOpposite=Standard_False;
  Standard_Real signe = 1.;
  AlongU = UOsculatingSurface(U,V,IsOpposite,L); 
  AlongV = VOsculatingSurface(U,V,IsOpposite,L);
  if ((AlongV || AlongU) && IsOpposite) signe = -1;    
  derivatives(MaxOrder,3,U,V,basisSurf,2,2,AlongU,AlongV,L,DerNUV,DerSurf);
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) Geom_UndefinedValue::Raise();
  
  
  P.SetXYZ(P.XYZ() + offsetValue * signe * Normal.XYZ());
  
  D1U = DerSurf(1,0)
    + offsetValue * signe * CSLib::DNNormal(1,0,DerNUV,OrderU,OrderV);
  D1V = DerSurf(0,1)
    + offsetValue * signe * CSLib::DNNormal(0,1,DerNUV,OrderU,OrderV);
 
  D2U = basisSurf->DN(U,V,2,0) 
    + signe * offsetValue * CSLib::DNNormal(2,0,DerNUV,OrderU,OrderV);
  D2V = basisSurf->DN(U,V,0,2)     
    + signe * offsetValue * CSLib::DNNormal(0,2,DerNUV,OrderU,OrderV);
  D2UV = basisSurf->DN(U,V,1,1) 
    + signe * offsetValue * CSLib::DNNormal(1,1,DerNUV,OrderU,OrderV);
}


//=======================================================================
//function : 
//purpose  : private
//=======================================================================/
void Geom_OffsetSurface::SetD3(const Standard_Real U, const Standard_Real V, 
			       Pnt& P, 
			       Vec& D1U, Vec& D1V, 
			       Vec& D2U, Vec& D2V, Vec& D2UV,
			       Vec& D3U, Vec& D3V, Vec& D3UUV, Vec& D3UVV ) const 
{
  Standard_Real MagTol=0.000000001;
  Dir Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1U, D1V, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+3,0,MaxOrder+3);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+4,0,MaxOrder+4);
  Standard_Real Umin,Umax,Vmin,Vmax;
  Bounds(Umin,Umax,Vmin,Vmax);
  
  DerSurf.SetValue(1, 0, D1U);
  DerSurf.SetValue(0, 1, D1V);
  DerSurf.SetValue(1, 1, D2UV);
  DerSurf.SetValue(2, 0, D2U);
  DerSurf.SetValue(0, 2, D2V);
  DerSurf.SetValue(3, 0, D3U);
  DerSurf.SetValue(2, 1, D3UUV);
  DerSurf.SetValue(1, 2, D3UVV);
  DerSurf.SetValue(0, 3, D3V);
  
  
  //*********************
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean AlongU = Standard_False,
  AlongV = Standard_False;
  Standard_Boolean IsOpposite=Standard_False;
  Standard_Real signe = 1.;
  AlongU = UOsculatingSurface(U,V,IsOpposite,L); 
  AlongV = VOsculatingSurface(U,V,IsOpposite,L);
  if ((AlongV || AlongU) && IsOpposite) signe = -1;
  derivatives(MaxOrder,3,U,V,basisSurf,3,3,AlongU,AlongV,L,DerNUV,DerSurf);
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) Geom_UndefinedValue::Raise();
  
  
  P.SetXYZ(P.XYZ() + offsetValue * signe * Normal.XYZ());
  
  D1U = DerSurf(1,0)
    + offsetValue * signe * CSLib::DNNormal(1,0,DerNUV,OrderU,OrderV);
  D1V = DerSurf(0,1)
    + offsetValue * signe * CSLib::DNNormal(0,1,DerNUV,OrderU,OrderV);
  
  D2U = basisSurf->DN(U,V,2,0) 
    + signe * offsetValue * CSLib::DNNormal(2,0,DerNUV,OrderU,OrderV);
  D2V = basisSurf->DN(U,V,0,2)     
    + signe * offsetValue * CSLib::DNNormal(0,2,DerNUV,OrderU,OrderV);
  D2UV = basisSurf->DN(U,V,1,1) 
    + signe * offsetValue * CSLib::DNNormal(1,1,DerNUV,OrderU,OrderV);
  D3U = basisSurf->DN(U,V,3,0)
    + signe * offsetValue * CSLib::DNNormal(3,0,DerNUV,OrderU,OrderV);
  D3V = basisSurf->DN(U,V,0,3)     
    + signe * offsetValue * CSLib::DNNormal(0,3,DerNUV,OrderU,OrderV);
  D3UUV = basisSurf->DN(U,V,2,1)
    + signe * offsetValue * CSLib::DNNormal(2,1,DerNUV,OrderU,OrderV);
  D3UVV = basisSurf->DN(U,V,1,2) 
    + signe * offsetValue * CSLib::DNNormal(1,2,DerNUV,OrderU,OrderV);
}


//=======================================================================
//function : SetDN
//purpose  : 
//=======================================================================

Vec Geom_OffsetSurface::SetDN ( const Standard_Real    U , const Standard_Real    V, 
			       const Standard_Integer Nu, const Standard_Integer Nv,
			       const Vec& D1U, const Vec& D1V) const 
{
  gp_Vec D(0,0,0);
  Standard_Real MagTol=0.000000001;
  Dir Normal;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1U, D1V, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+Nu,0,MaxOrder+Nu);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+Nu+1,0,MaxOrder+Nv+1);
  Standard_Real Umin,Umax,Vmin,Vmax;
  Bounds(Umin,Umax,Vmin,Vmax);
  
  DerSurf.SetValue(1, 0, D1U);
  DerSurf.SetValue(0, 1, D1V);
  //*********************
  Handle(Geom_BSplineSurface) L;
  Standard_Boolean AlongU = Standard_False, AlongV = Standard_False;
  //   Is there any osculatingsurface along U or V;
  Standard_Boolean IsOpposite=Standard_False;
  Standard_Real signe = 1.;
  AlongU = UOsculatingSurface(U,V,IsOpposite,L); 
  AlongV = VOsculatingSurface(U,V,IsOpposite,L);
     if ((AlongV || AlongU) && IsOpposite) signe = -1;
  derivatives(MaxOrder,1,U,V,basisSurf,Nu,Nv,AlongU,AlongV,L,DerNUV,DerSurf);
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) Geom_UndefinedValue::Raise();
  
  D = basisSurf->DN(U,V,Nu,Nv)
    + signe * offsetValue * CSLib::DNNormal(Nu,Nv,DerNUV,OrderU,OrderV);
 
  return D;
}

