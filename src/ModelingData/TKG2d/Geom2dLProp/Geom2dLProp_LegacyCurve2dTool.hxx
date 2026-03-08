// Copyright (c) 1992-2014 OPEN CASCADE SAS

#ifndef _Geom2dLProp_LegacyCurve2dTool_HeaderFile
#define _Geom2dLProp_LegacyCurve2dTool_HeaderFile

#include <Standard_DefineAlloc.hxx>

#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

class Geom2dLProp_LegacyCurve2dTool
{
public:
  DEFINE_STANDARD_ALLOC

  static void Value(const occ::handle<Geom2d_Curve>& C, const double U, gp_Pnt2d& P) { P = C->Value(U); }
  static void D1(const occ::handle<Geom2d_Curve>& C, const double U, gp_Pnt2d& P, gp_Vec2d& V1)
  {
    C->D1(U, P, V1);
  }
  static void D2(const occ::handle<Geom2d_Curve>& C,
                 const double                     U,
                 gp_Pnt2d&                        P,
                 gp_Vec2d&                        V1,
                 gp_Vec2d&                        V2)
  {
    C->D2(U, P, V1, V2);
  }
  static void D3(const occ::handle<Geom2d_Curve>& C,
                 const double                     U,
                 gp_Pnt2d&                        P,
                 gp_Vec2d&                        V1,
                 gp_Vec2d&                        V2,
                 gp_Vec2d&                        V3)
  {
    C->D3(U, P, V1, V2, V3);
  }
  static int Continuity(const occ::handle<Geom2d_Curve>& C)
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
  static double FirstParameter(const occ::handle<Geom2d_Curve>& C) { return C->FirstParameter(); }
  static double LastParameter(const occ::handle<Geom2d_Curve>& C) { return C->LastParameter(); }
};

#endif
