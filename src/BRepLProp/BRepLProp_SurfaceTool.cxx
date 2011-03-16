// File:	BRepLProp_SurfaceTool.cxx
// Created:	Thu Feb 24 16:44:32 1994
// Author:	Laurent BOURESCHE
//		<lbo@nonox>


#include <BRepLProp_SurfaceTool.ixx>

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void BRepLProp_SurfaceTool::Value(const BRepAdaptor_Surface& S, 
				  const Standard_Real U, 
				  const Standard_Real V, 
				  gp_Pnt& P)
{
  P = S.Value(U, V);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void BRepLProp_SurfaceTool::D1(const BRepAdaptor_Surface& S, 
			       const Standard_Real U, 
			       const Standard_Real V, 
			       gp_Pnt& P, 
			       gp_Vec& D1U, 
			       gp_Vec& D1V)
{
  S.D1(U, V, P, D1U, D1V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void BRepLProp_SurfaceTool::D2(const BRepAdaptor_Surface& S, 
			       const Standard_Real U, 
			       const Standard_Real V, 
			       gp_Pnt& P, 
			       gp_Vec& D1U, 
			       gp_Vec& D1V, 
			       gp_Vec& D2U, 
			       gp_Vec& D2V, 
			       gp_Vec& DUV)
{
  S.D2(U, V, P, D1U, D1V, D2U, D2V, DUV);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================
gp_Vec BRepLProp_SurfaceTool::DN(const BRepAdaptor_Surface& S, 
				 const Standard_Real U, 
				 const Standard_Real V,
				 const Standard_Integer IU,
				 const Standard_Integer IV)
{
  return S.DN(U, V, IU, IV);
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

Standard_Integer BRepLProp_SurfaceTool::Continuity
  (const BRepAdaptor_Surface& S)
{
  GeomAbs_Shape s = (GeomAbs_Shape) Min(S.UContinuity(),S.VContinuity());
  switch (s) {
  case GeomAbs_C0:
    return 0;
  case GeomAbs_C1:
    return 1;
  case GeomAbs_C2:
    return 2;
  case GeomAbs_C3:
    return 3;
  case GeomAbs_G1:
    return 0;
  case GeomAbs_G2:
    return 0;
  case GeomAbs_CN:
    return 3;
  };
  return 0;
}


//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void BRepLProp_SurfaceTool::Bounds(const BRepAdaptor_Surface& S, 
				   Standard_Real& U1, 
				   Standard_Real& V1, 
				   Standard_Real& U2, 
				   Standard_Real& V2)
{
  U1 = S.FirstUParameter();
  V1 = S.FirstVParameter();
  U2 = S.LastUParameter();
  V2 = S.LastVParameter();
}








