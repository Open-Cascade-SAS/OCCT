// Created on: 1999-04-27
// Created by: Pavel DURANDIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_CompositeSurface,Geom_Surface)

//=======================================================================
//function : ShapeExtend_CompositeSurface
//purpose  : 
//=======================================================================
ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface(const Handle(TColGeom_HArray2OfSurface)& GridSurf,
							   const ShapeExtend_Parametrisation param)
{
  Init ( GridSurf, param );
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface(const Handle(TColGeom_HArray2OfSurface)& GridSurf,
							   const TColStd_Array1OfReal &UJoints,
							   const TColStd_Array1OfReal &VJoints)
{
  Init ( GridSurf, UJoints, VJoints );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::Init (const Handle(TColGeom_HArray2OfSurface)& GridSurf,
						     const ShapeExtend_Parametrisation param)
{
  if ( GridSurf.IsNull() ) return Standard_False;
  myPatches = GridSurf;
  ComputeJointValues ( param );
  return CheckConnectivity ( Precision::Confusion() );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::Init (const Handle(TColGeom_HArray2OfSurface)& GridSurf,
						     const TColStd_Array1OfReal &UJoints,
						     const TColStd_Array1OfReal &VJoints)
{
  if ( GridSurf.IsNull() ) return Standard_False;
  myPatches = GridSurf;
  
  Standard_Boolean ok = Standard_True;
  if ( ! SetUJointValues ( UJoints ) || ! SetVJointValues ( VJoints ) ) {
    ok = Standard_False;
    ComputeJointValues ( ShapeExtend_Natural );
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeExtend_CompositeSurface::Init: bad joint values" << endl;
#endif
  }
  
  return ( CheckConnectivity ( Precision::Confusion() ) ? ok : Standard_False );
}

//=======================================================================
//function : NbUPatches
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_CompositeSurface::NbUPatches() const
{
  return myPatches->ColLength();
}

//=======================================================================
//function : NbVPatches
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_CompositeSurface::NbVPatches() const
{
  return myPatches->RowLength();
}

//=======================================================================
//function : Patch
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& ShapeExtend_CompositeSurface::Patch(const Standard_Integer i,
								 const Standard_Integer j) const
{
  return myPatches->Value(i,j);
}

//=======================================================================
//function : Patches
//purpose  : 
//=======================================================================

const Handle(TColGeom_HArray2OfSurface)& ShapeExtend_CompositeSurface::Patches() const
{
  return myPatches;
}

//=======================================================================
//function : UJointValues
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) ShapeExtend_CompositeSurface::UJointValues() const
{
  return myUJointValues;
}

//=======================================================================
//function : VJointValues
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) ShapeExtend_CompositeSurface::VJointValues() const
{
  return myVJointValues;
}

//=======================================================================
//function : UJointValue
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::UJointValue(const Standard_Integer i) const
{
  return myUJointValues->Value(i);
}

//=======================================================================
//function : VJointValue
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::VJointValue(const Standard_Integer i) const
{
  return myVJointValues->Value(i);
}

//=======================================================================
//function : SetUJointValues
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::SetUJointValues (const TColStd_Array1OfReal &UJoints)
{
  Standard_Integer NbU = NbUPatches();
  if ( UJoints.Length() != NbU+1 ) return Standard_False;

  Handle(TColStd_HArray1OfReal) UJointValues = new TColStd_HArray1OfReal(1,NbU+1);
  for ( Standard_Integer i=1, j=UJoints.Lower(); i <= NbU+1; i++, j++ ) {
    UJointValues->SetValue ( i, UJoints(j) );
    if ( i >1 && UJoints(j) - UJoints(j-1) < Precision::PConfusion() ) 
      return Standard_False;
  }
  myUJointValues = UJointValues;
  return Standard_True;
}

//=======================================================================
//function : SetVJointValues
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::SetVJointValues (const TColStd_Array1OfReal &VJoints)
{
  Standard_Integer NbV = NbVPatches();
  if ( VJoints.Length() != NbV+1 ) return Standard_False;

  Handle(TColStd_HArray1OfReal) VJointValues = new TColStd_HArray1OfReal(1,NbV+1);
  for ( Standard_Integer i=1, j=VJoints.Lower(); i <= NbV+1; i++, j++ ) {
    VJointValues->SetValue ( i, VJoints(j) );
    if ( i >1 && VJoints(j) - VJoints(j-1) < Precision::PConfusion() ) 
      return Standard_False;
  }
  myVJointValues = VJointValues;
  return Standard_True;
}

//=======================================================================
//function : SetUFirstValue
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::SetUFirstValue (const Standard_Real UFirst)
{
  if ( myUJointValues.IsNull() ) return;
  
  Standard_Real shift = UFirst - myUJointValues->Value(1);
  Standard_Integer NbU = myUJointValues->Length();
  for ( Standard_Integer i=1; i <= NbU; i++ ) {
    myUJointValues->SetValue ( i, myUJointValues->Value(i) + shift );
  }
}

//=======================================================================
//function : SetVFirstValue
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::SetVFirstValue (const Standard_Real VFirst)
{
  if ( myVJointValues.IsNull() ) return;
  
  Standard_Real shift = VFirst - myVJointValues->Value(1);
  Standard_Integer NbV = myVJointValues->Length();
  for ( Standard_Integer i=1; i <= NbV; i++ ) {
    myVJointValues->SetValue ( i, myVJointValues->Value(i) + shift );
  }
}

//=======================================================================
//function : LocateUParameter
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_CompositeSurface::LocateUParameter(const Standard_Real U) const
{
  Standard_Integer nbPatch = NbUPatches();
  for(Standard_Integer i = 2; i <= nbPatch; i++)
    if (U < myUJointValues->Value(i)) return i-1;
  return nbPatch;
}

//=======================================================================
//function : LocateVParameter
//purpose  : 
//=======================================================================

Standard_Integer ShapeExtend_CompositeSurface::LocateVParameter(const Standard_Real V) const
{
  Standard_Integer nbPatch = NbVPatches();
  for(Standard_Integer i = 2; i <= nbPatch; i++)
    if (V < myVJointValues->Value(i)) return i-1;
  return nbPatch;
}

//=======================================================================
//function : LocateUVPoint
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::LocateUVPoint(const gp_Pnt2d& pnt,
						 Standard_Integer& i,
						 Standard_Integer& j) const
{
  i = LocateUParameter(pnt.X());
  j = LocateVParameter(pnt.Y());
}

//=======================================================================
//function : Patch
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& ShapeExtend_CompositeSurface::Patch(const Standard_Real U, 
								const Standard_Real V) const
{
  return myPatches->Value ( LocateUParameter(U), LocateVParameter(V) );
}

//=======================================================================
//function : Patch
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& ShapeExtend_CompositeSurface::Patch(const gp_Pnt2d& pnt) const
{
  return myPatches->Value ( LocateUParameter(pnt.X()), LocateVParameter(pnt.Y()) );
}

//=======================================================================
//function : ULocalToGlobal
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::ULocalToGlobal (const Standard_Integer i,
							    const Standard_Integer j,
							    const Standard_Real u) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scale = ( myUJointValues->Value(i+1) - myUJointValues->Value(i) ) / ( u2 - u1 );
  return u * scale + ( myUJointValues->Value(i) - u1 * scale ); // ! this formula is stable if u1 is infinite
}

//=======================================================================
//function : VLocalToGlobal
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::VLocalToGlobal (const Standard_Integer i,
							    const Standard_Integer j,
							    const Standard_Real v) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scale = ( myVJointValues->Value(j+1) - myVJointValues->Value(j) ) / ( v2 - v1 );
  return v * scale + ( myVJointValues->Value(j) - v1 * scale ); // ! this formula is stable if v1 is infinite
}

//=======================================================================
//function : LocalToGlobal
//purpose  : 
//=======================================================================

gp_Pnt2d ShapeExtend_CompositeSurface::LocalToGlobal (const Standard_Integer i,
						      const Standard_Integer j,
						      const gp_Pnt2d &uv) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scaleu = ( myUJointValues->Value(i+1) - myUJointValues->Value(i) ) / ( u2 - u1 );
  Standard_Real scalev = ( myVJointValues->Value(j+1) - myVJointValues->Value(j) ) / ( v2 - v1 );
  return gp_Pnt2d ( uv.X() * scaleu + ( myUJointValues->Value(i) - u1 * scaleu ), // ! this formula is stable if u1 or v1 is infinite
		    uv.Y() * scalev + ( myVJointValues->Value(j) - v1 * scalev ) );
}

//=======================================================================
//function : UGlobalToLocal
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::UGlobalToLocal (const Standard_Integer i,
							    const Standard_Integer j,
							    const Standard_Real U) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scale = ( u2 - u1 ) / ( myUJointValues->Value(i+1) - myUJointValues->Value(i) );
  return U * scale + ( u1 - myUJointValues->Value(i) * scale ); // ! this formula is stable if u1 is infinite
}

//=======================================================================
//function : VGlobalToLocal
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::VGlobalToLocal (const Standard_Integer i,
							    const Standard_Integer j,
							    const Standard_Real V) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scale = ( v2 - v1 ) / ( myVJointValues->Value(j+1) - myVJointValues->Value(j) );
  return V * scale + ( v1 - myVJointValues->Value(j) * scale ); // ! this formula is stable if v1 is infinite
}

//=======================================================================
//function : GlobalToLocal
//purpose  : 
//=======================================================================

gp_Pnt2d ShapeExtend_CompositeSurface::GlobalToLocal (const Standard_Integer i,
						      const Standard_Integer j,
						      const gp_Pnt2d &UV) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );
  Standard_Real scaleu = ( u2 - u1 ) / ( myUJointValues->Value(i+1) - myUJointValues->Value(i) );
  Standard_Real scalev = ( v2 - v1 ) / ( myVJointValues->Value(j+1) - myVJointValues->Value(j) );
  return gp_Pnt2d ( UV.X() * scaleu + ( u1 - myUJointValues->Value(i) * scaleu ), // ! this formula is stable if u1 or v1 is infinite
		    UV.Y() * scalev + ( v1 - myVJointValues->Value(j) * scalev ) );
}

//=======================================================================
//function : GlobalToLocalTransformation
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::GlobalToLocalTransformation (const Standard_Integer i,
									    const Standard_Integer j,
									    Standard_Real &uFact,
									    gp_Trsf2d &Trsf) const
{
  Standard_Real u1, u2, v1, v2;
  myPatches->Value(i,j)->Bounds ( u1, u2, v1, v2 );

  Standard_Real scaleu = ( u2 - u1 ) / ( myUJointValues->Value(i+1) - myUJointValues->Value(i) );
  Standard_Real scalev = ( v2 - v1 ) / ( myVJointValues->Value(j+1) - myVJointValues->Value(j) );
  gp_Vec2d shift ( u1 / scaleu - myUJointValues->Value(i),
		   v1 / scalev - myVJointValues->Value(j) );

  uFact = scaleu / scalev;
  gp_Trsf2d Shift, Scale;
  if ( shift.X() != 0. || shift.Y() != 0. ) Shift.SetTranslation ( shift );
  if ( scalev != 1. ) Scale.SetScale ( gp_Pnt2d(0,0), scalev );
  Trsf = Scale * Shift;
  return uFact != 1. || Trsf.Form() != gp_Identity;
}

//=======================================================================
// Inherited methods (from Geom_Geometry and Geom_Surface)
//=======================================================================

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::Transform (const gp_Trsf &T)
{
  if ( myPatches.IsNull() ) return;
  for ( Standard_Integer i=1; i <= NbUPatches(); i++ ) 
    for ( Standard_Integer j=1; j <= NbVPatches(); j++ ) 
      Patch(i,j)->Transform ( T );
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Geom_Geometry) ShapeExtend_CompositeSurface::Copy () const
{
  Handle(ShapeExtend_CompositeSurface) surf = new ShapeExtend_CompositeSurface;
  if ( myPatches.IsNull() ) return surf;
  
  Handle(TColGeom_HArray2OfSurface) patches = 
    new TColGeom_HArray2OfSurface ( 1, NbUPatches(), 1, NbVPatches() );
  for ( Standard_Integer i=1; i <= NbUPatches(); i++ ) 
    for ( Standard_Integer j=1; j <= NbVPatches(); j++ ) 
      patches->SetValue ( i, j, Handle(Geom_Surface)::DownCast ( Patch(i,j)->Copy() ) );
  surf->Init ( patches );
  return surf;
}
    
//=======================================================================
//function : UReverse
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::UReverse () 
{
}

//=======================================================================
//function : UReversedParameter
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::UReversedParameter (const Standard_Real U) const
{
  return U;
}

//=======================================================================
//function : VReverse
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::VReverse () 
{
}

//=======================================================================
//function : VReversedParameter
//purpose  : 
//=======================================================================

Standard_Real ShapeExtend_CompositeSurface::VReversedParameter (const Standard_Real V) const
{
  return V;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::Bounds(Standard_Real& U1,
					   Standard_Real& U2,
					   Standard_Real& V1,
					   Standard_Real& V2) const
{
  U1 = UJointValue(1);
  V1 = VJointValue(1);
  U2 = UJointValue(NbUPatches()+1);
  V2 = VJointValue(NbVPatches()+1);
}

//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsUPeriodic () const
{
  return Standard_False;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsVPeriodic () const 
{
  return Standard_False;
}

//=======================================================================
//function : UIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) ShapeExtend_CompositeSurface::UIso (const Standard_Real ) const
{
  Handle(Geom_Curve) dummy;
  return dummy;
}

//=======================================================================
//function : VIso
//purpose  : 
//=======================================================================

Handle(Geom_Curve) ShapeExtend_CompositeSurface::VIso (const Standard_Real ) const
{
  Handle(Geom_Curve) dummy;
  return dummy;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ShapeExtend_CompositeSurface::Continuity () const
{
  return GeomAbs_C0;
}

//=======================================================================
//function : IsCNu
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsCNu (const Standard_Integer N) const
{
  return N <=0;
}

//=======================================================================
//function : IsCNv
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsCNv (const Standard_Integer N) const
{
  return N <=0;
}

//=======================================================================
//function : IsUClosed
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsUClosed () const
{
  return myUClosed;
}

//=======================================================================
//function : IsVClosed
//purpose  : 
//=======================================================================

Standard_Boolean ShapeExtend_CompositeSurface::IsVClosed () const
{
  return myVClosed;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::D0 (const Standard_Real U,
				       const Standard_Real V,
				       gp_Pnt& P) const
{
  Standard_Integer i = LocateUParameter ( U );
  Standard_Integer j = LocateVParameter ( V );
  gp_Pnt2d uv = GlobalToLocal ( i, j, gp_Pnt2d ( U, V ) );
  myPatches->Value(i,j)->D0 ( uv.X(), uv.Y(), P );
}
  
//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::D1 (const Standard_Real U,
				       const Standard_Real V,
				       gp_Pnt& P,
				       gp_Vec& D1U,
				       gp_Vec& D1V) const
{
  Standard_Integer i = LocateUParameter ( U );
  Standard_Integer j = LocateVParameter ( V );
  gp_Pnt2d uv = GlobalToLocal ( i, j, gp_Pnt2d ( U, V ) );
  myPatches->Value(i,j)->D1 ( uv.X(), uv.Y(), P, D1U, D1V );
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::D2 (const Standard_Real U,
				       const Standard_Real V,
				       gp_Pnt& P,
				       gp_Vec& D1U,
				       gp_Vec& D1V,
				       gp_Vec& D2U,
				       gp_Vec& D2V,
				       gp_Vec& D2UV) const
{
  Standard_Integer i = LocateUParameter ( U );
  Standard_Integer j = LocateVParameter ( V );
  gp_Pnt2d uv = GlobalToLocal ( i, j, gp_Pnt2d ( U, V ) );
  myPatches->Value(i,j)->D2 ( uv.X(), uv.Y(), P, D1U, D1V, D2U, D2V, D2UV );
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::D3 (const Standard_Real U,
				       const Standard_Real V,
				       gp_Pnt& P,
				       gp_Vec& D1U,
				       gp_Vec& D1V,
				       gp_Vec& D2U,
				       gp_Vec& D2V,
				       gp_Vec& D2UV,
				       gp_Vec& D3U,
				       gp_Vec& D3V,
				       gp_Vec& D3UUV,
				       gp_Vec& D3UVV) const
{
  Standard_Integer i = LocateUParameter ( U );
  Standard_Integer j = LocateVParameter ( V );
  gp_Pnt2d uv = GlobalToLocal ( i, j, gp_Pnt2d ( U, V ) );
  myPatches->Value(i,j)->D3 ( uv.X(), uv.Y(), P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV );
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec ShapeExtend_CompositeSurface::DN (const Standard_Real U,
					 const Standard_Real V,
					 const Standard_Integer Nu,
					 const Standard_Integer Nv) const
{
  Standard_Integer i = LocateUParameter ( U );
  Standard_Integer j = LocateVParameter ( V );
  gp_Pnt2d uv = GlobalToLocal ( i, j, gp_Pnt2d ( U, V ) );
  return myPatches->Value(i,j)->DN ( uv.X(), uv.Y(), Nu, Nv );
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt ShapeExtend_CompositeSurface::Value (const gp_Pnt2d& pnt) const
{
  Standard_Integer i = LocateUParameter ( pnt.X() );
  Standard_Integer j = LocateVParameter ( pnt.Y() );
  gp_Pnt2d uv = GlobalToLocal ( i, j, pnt );
  gp_Pnt point;
  myPatches->Value(i,j)->D0 ( uv.X(), uv.Y(), point );
  return point;
}

//=======================================================================
//function : ComputeJointValues
//purpose  : 
//=======================================================================

void ShapeExtend_CompositeSurface::ComputeJointValues (const ShapeExtend_Parametrisation param)
{
  Standard_Integer NbU = NbUPatches();
  Standard_Integer NbV = NbVPatches();
  myUJointValues = new TColStd_HArray1OfReal(1,NbU+1);
  myVJointValues = new TColStd_HArray1OfReal(1,NbV+1);
  
  if ( param == ShapeExtend_Natural ) {
    Standard_Real U1, U2, V1, V2, U=0, V=0;
    Standard_Integer i; // svv Jan 10 2000 : porting on DEC
    for ( i = 1; i <= NbU; i++ ) {
      myPatches->Value(i,1)->Bounds(U1,U2,V1,V2);
      if ( i ==1 ) myUJointValues->SetValue ( 1, U = U1 );
      U += ( U2 - U1 );
      myUJointValues->SetValue ( i+1, U );
    }
    for ( i = 1; i <= NbV; i++ ) {
      myPatches->Value(1,i)->Bounds(U1,U2,V1,V2);
      if ( i ==1 ) myVJointValues->SetValue ( 1, V = V1 );
      V += ( V2 - V1 );
      myVJointValues->SetValue ( i+1, V );
    }
  }
  else {
    Standard_Real stepu = 1., stepv = 1.; // suppose param == ShapeExtend_Uniform
    if ( param == ShapeExtend_Unitary ) {
      stepu /= NbU; 
      stepv /= NbV;
    }
    Standard_Integer i; // svv Jan 10 2000 : porting on DEC
    for ( i=0; i <= NbU; i++ )
      myUJointValues->SetValue ( i+1, i * stepu );
    for ( i=0; i <= NbV; i++ )
      myVJointValues->SetValue ( i+1, i * stepv );
  }
}

//=======================================================================
//function : CheckConnectivity
//purpose  : 
//=======================================================================

static inline Standard_Real LimitValue (const Standard_Real &par)
{
  return Precision::IsInfinite(par) ? ( par <0 ? -10000. : 10000. ) : par;
}

static void GetLimitedBounds (const Handle(Geom_Surface) &surf,
			      Standard_Real &U1, Standard_Real &U2,
			      Standard_Real &V1, Standard_Real &V2)
{
  surf->Bounds ( U1, U2, V1, V2 );
  U1 = LimitValue ( U1 );
  U2 = LimitValue ( U2 );
  V1 = LimitValue ( V1 );
  V2 = LimitValue ( V2 );
}
     
Standard_Boolean ShapeExtend_CompositeSurface::CheckConnectivity (const Standard_Real Prec)
{
  const Standard_Integer NPOINTS = 23;
  Standard_Boolean ok = Standard_True;
  Standard_Integer NbU = NbUPatches();
  Standard_Integer NbV = NbVPatches();
  
  // check in u direction
  Standard_Integer i,j; // svv Jan 10 2000 : porting on DEC
  for ( i=1, j = NbU; i <= NbU; j = i++ ) {
    Standard_Real maxdist2 = 0.;
    for ( Standard_Integer k=1; k <= NbV; k++ ) {
      Handle(Geom_Surface) sj = myPatches->Value(j,k);
      Handle(Geom_Surface) si = myPatches->Value(i,k);
      Standard_Real Uj1, Uj2, Vj1, Vj2;
      GetLimitedBounds ( sj, Uj1, Uj2, Vj1, Vj2 );
      Standard_Real Ui1, Ui2, Vi1, Vi2;
      GetLimitedBounds ( si, Ui1, Ui2, Vi1, Vi2 );
      Standard_Real stepj = ( Vj2 - Vj1 ) / ( NPOINTS - 1 );
      Standard_Real stepi = ( Vi2 - Vi1 ) / ( NPOINTS - 1 );
      for ( Standard_Integer isample=0; isample < NPOINTS; isample++ ) {
	Standard_Real parj = Vj1 + stepj * isample;
	Standard_Real pari = Vi1 + stepi * isample;
	Standard_Real dist2 = sj->Value ( Uj2, parj ).SquareDistance ( si->Value ( Ui1, pari ) );
	if ( maxdist2 < dist2 ) maxdist2 = dist2;
      }
    }
    if ( i==1 ) myUClosed = ( maxdist2 <= Prec*Prec );
    else if ( maxdist2 > Prec*Prec ) ok = Standard_False;
  }
  
  // check in v direction
  for ( i=1, j = NbV; i <= NbV; j = i++ ) {
    Standard_Real maxdist2 = 0.;
    for ( Standard_Integer k=1; k <= NbU; k++ ) {
      Handle(Geom_Surface) sj = myPatches->Value(k,j);
      Handle(Geom_Surface) si = myPatches->Value(k,i);
      Standard_Real Uj1, Uj2, Vj1, Vj2;
      GetLimitedBounds ( sj, Uj1, Uj2, Vj1, Vj2 );
      Standard_Real Ui1, Ui2, Vi1, Vi2;
      GetLimitedBounds ( si, Ui1, Ui2, Vi1, Vi2 );
      Standard_Real stepj = ( Uj2 - Uj1 ) / ( NPOINTS - 1 );
      Standard_Real stepi = ( Ui2 - Ui1 ) / ( NPOINTS - 1 );
      for ( Standard_Integer isample=0; isample < NPOINTS; isample++ ) {
	Standard_Real parj = Uj1 + stepj * isample;
	Standard_Real pari = Ui1 + stepi * isample;
	Standard_Real dist2 = sj->Value ( parj, Vj2 ).SquareDistance ( si->Value ( pari, Vi1 ) );
	if ( maxdist2 < dist2 ) maxdist2 = dist2;
      }
    }
    if ( i==1 ) myVClosed = ( maxdist2 <= Prec*Prec );
    else if ( maxdist2 > Prec*Prec ) ok = Standard_False;
  }

#ifdef OCCT_DEBUG
  if ( ! ok ) cout << "Warning: ShapeExtend_CompositeSurface: not connected in 3d" << endl;
#endif
  return ok;
}
