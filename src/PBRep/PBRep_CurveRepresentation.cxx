// File:	PBRep_CurveRepresentation.cxx
// Created:	Mon Jul 26 10:28:58 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <PBRep_CurveRepresentation.ixx>

//=======================================================================
//function : PBRep_CurveRepresentation
//purpose  : 
//=======================================================================

PBRep_CurveRepresentation::PBRep_CurveRepresentation(const PTopLoc_Location& L) :
  myLocation(L)
{
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

PTopLoc_Location  PBRep_CurveRepresentation::Location()const 
{
  return myLocation;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

Handle(PBRep_CurveRepresentation)  PBRep_CurveRepresentation::Next()const 
{
  return myNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  PBRep_CurveRepresentation::Next
  (const Handle(PBRep_CurveRepresentation)& N)
{
  myNext = N;
}

//=======================================================================
//function : IsGCurve
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsGCurve()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurve3D()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurveOnSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsCurveOnClosedSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsRegularity()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygon3D
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygon3D()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnTriangulation()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsPolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnClosedTriangulation()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnSurface()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_CurveRepresentation::IsPolygonOnClosedSurface()const 
{
  return Standard_False;
}
