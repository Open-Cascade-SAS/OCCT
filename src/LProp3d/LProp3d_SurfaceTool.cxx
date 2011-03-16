// File:        LProp3d_SurfaceTool.cxx
// Created:     Thu Feb 24 16:44:32 1994
// Author:      Laurent BOURESCHE
//              <lbo@nonox>


#include <LProp3d_SurfaceTool.ixx>

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void LProp3d_SurfaceTool::Value(const Handle(Adaptor3d_HSurface)& S, 
                                const Standard_Real U, 
                                const Standard_Real V, 
                                gp_Pnt& P)
{
  P = S->Value(U, V);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void LProp3d_SurfaceTool::D1(const Handle(Adaptor3d_HSurface)& S, 
                             const Standard_Real U, 
                             const Standard_Real V, 
                             gp_Pnt& P, 
                             gp_Vec& D1U, 
                             gp_Vec& D1V)
{
  S->D1(U, V, P, D1U, D1V);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void LProp3d_SurfaceTool::D2(const Handle(Adaptor3d_HSurface)& S, 
                             const Standard_Real U, 
                             const Standard_Real V, 
                             gp_Pnt& P, 
                             gp_Vec& D1U, 
                             gp_Vec& D1V, 
                             gp_Vec& D2U, 
                             gp_Vec& D2V, 
                             gp_Vec& DUV)
{
  S->D2(U, V, P, D1U, D1V, D2U, D2V, DUV);
}

//=======================================================================
//function : DN
//purpose  : 
//=======================================================================
gp_Vec LProp3d_SurfaceTool::DN(const Handle(Adaptor3d_HSurface)& S, 
                               const Standard_Real U, 
                               const Standard_Real V,
                               const Standard_Integer IU,
                               const Standard_Integer IV)
{
  return S->DN(U, V, IU, IV);
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

Standard_Integer LProp3d_SurfaceTool::Continuity
  (const Handle(Adaptor3d_HSurface)& S)
{
  GeomAbs_Shape s = (GeomAbs_Shape) Min(S->UContinuity(),S->VContinuity());
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

void LProp3d_SurfaceTool::Bounds(const Handle(Adaptor3d_HSurface)& S, 
                                 Standard_Real& U1, 
                                 Standard_Real& V1, 
                                 Standard_Real& U2, 
                                 Standard_Real& V2)
{
  U1 = S->FirstUParameter();
  V1 = S->FirstVParameter();
  U2 = S->LastUParameter();
  V2 = S->LastVParameter();
}








