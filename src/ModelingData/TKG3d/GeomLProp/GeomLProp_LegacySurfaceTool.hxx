// Copyright (c) 1992-2014 OPEN CASCADE SAS

#ifndef _GeomLProp_LegacySurfaceTool_HeaderFile
#define _GeomLProp_LegacySurfaceTool_HeaderFile

#include <Standard_DefineAlloc.hxx>

#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class GeomLProp_LegacySurfaceTool
{
public:
  DEFINE_STANDARD_ALLOC

  static void Value(const occ::handle<Geom_Surface>& S, const double U, const double V, gp_Pnt& P)
  {
    P = S->Value(U, V);
  }
  static void D1(const occ::handle<Geom_Surface>& S,
                 const double                     U,
                 const double                     V,
                 gp_Pnt&                          P,
                 gp_Vec&                          D1U,
                 gp_Vec&                          D1V)
  {
    S->D1(U, V, P, D1U, D1V);
  }
  static void D2(const occ::handle<Geom_Surface>& S,
                 const double                     U,
                 const double                     V,
                 gp_Pnt&                          P,
                 gp_Vec&                          D1U,
                 gp_Vec&                          D1V,
                 gp_Vec&                          D2U,
                 gp_Vec&                          D2V,
                 gp_Vec&                          DUV)
  {
    S->D2(U, V, P, D1U, D1V, D2U, D2V, DUV);
  }
  static gp_Vec DN(const occ::handle<Geom_Surface>& S,
                   const double                     U,
                   const double                     V,
                   const int                        IU,
                   const int                        IV)
  {
    return S->DN(U, V, IU, IV);
  }
  static int Continuity(const occ::handle<Geom_Surface>& S)
  {
    const GeomAbs_Shape s = S->Continuity();
    switch (s)
    {
      case GeomAbs_C0: return 0;
      case GeomAbs_C1: return 1;
      case GeomAbs_C2: return 2;
      case GeomAbs_C3: return 3;
      case GeomAbs_G1:
      case GeomAbs_G2: return 0;
      case GeomAbs_CN: return 3;
    }
    return 0;
  }
  static void Bounds(const occ::handle<Geom_Surface>& S,
                     double&                          U1,
                     double&                          V1,
                     double&                          U2,
                     double&                          V2)
  {
    S->Bounds(U1, U2, V1, V2);
  }
};

#endif
