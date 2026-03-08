// Copyright (c) 1992-2014 OPEN CASCADE SAS

#ifndef _GeomLProp_LegacyCurveTool_HeaderFile
#define _GeomLProp_LegacyCurveTool_HeaderFile

#include <Standard_DefineAlloc.hxx>

#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class GeomLProp_LegacyCurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  static void Value(const occ::handle<Geom_Curve>& C, const double U, gp_Pnt& P) { P = C->Value(U); }
  static void D1(const occ::handle<Geom_Curve>& C, const double U, gp_Pnt& P, gp_Vec& V1)
  {
    C->D1(U, P, V1);
  }
  static void D2(const occ::handle<Geom_Curve>& C,
                 const double                   U,
                 gp_Pnt&                        P,
                 gp_Vec&                        V1,
                 gp_Vec&                        V2)
  {
    C->D2(U, P, V1, V2);
  }
  static void D3(const occ::handle<Geom_Curve>& C,
                 const double                   U,
                 gp_Pnt&                        P,
                 gp_Vec&                        V1,
                 gp_Vec&                        V2,
                 gp_Vec&                        V3)
  {
    C->D3(U, P, V1, V2, V3);
  }
  static int Continuity(const occ::handle<Geom_Curve>& C)
  {
    const GeomAbs_Shape s = C->Continuity();
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
  static double FirstParameter(const occ::handle<Geom_Curve>& C) { return C->FirstParameter(); }
  static double LastParameter(const occ::handle<Geom_Curve>& C) { return C->LastParameter(); }
};

#endif
