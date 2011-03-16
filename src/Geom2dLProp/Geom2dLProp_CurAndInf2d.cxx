// File:	Geom2dLProp_CurAndInf2d.cxx
// Created:	Tue Sep  6 15:14:52 1994
// Author:	Yves FRICAUD
//		<yfr@ecolox>


#include <Geom2dLProp_CurAndInf2d.ixx>
#include <Geom2dLProp_NumericCurInf2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <LProp_AnalyticCurInf.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <TColStd_Array1OfReal.hxx>



//=======================================================================
//function : Geom2dLProp_CurAndInf2d
//purpose  : 
//=======================================================================
Geom2dLProp_CurAndInf2d::Geom2dLProp_CurAndInf2d()
{
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::Perform(const Handle(Geom2d_Curve)& C)
{
  PerformCurExt(C);
  PerformInf   (C);
}


//=======================================================================
//function : PerformCurExt
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::PerformCurExt(const Handle(Geom2d_Curve)& C)
{
  isDone = Standard_True;

  Geom2dAdaptor_Curve         CC(C);
  LProp_AnalyticCurInf        AC;
  Geom2dLProp_NumericCurInf2d NC;
  GeomAbs_CurveType           CType = CC.GetType();

  switch (CType) {
  case GeomAbs_Line: 
    break;
  case GeomAbs_Circle:
    break;
  case GeomAbs_Ellipse:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_Hyperbola:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_Parabola:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_BSplineCurve:
    if (CC.Continuity() >= GeomAbs_C3 ) {
      NC.PerformCurExt(C,*this);
      isDone = NC.IsDone();
    }
    else {
      // Decoupage en intervalles C3.
      isDone = Standard_True;
      Standard_Integer NbInt = CC.NbIntervals(GeomAbs_C3);
      TColStd_Array1OfReal Param(1,NbInt+1);
      CC.Intervals(Param,GeomAbs_C3);
      for (Standard_Integer i = 1; i <= NbInt; i++) {
	NC.PerformCurExt(C,Param(i),Param(i+1),*this);
	if (!NC.IsDone()) {isDone = Standard_False;}
      }
    }
    break;

    default : {
      NC.PerformCurExt(C,*this);
      isDone = NC.IsDone();
    }
    break;
  }
  
}


//=======================================================================
//function : PerformInf
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::PerformInf(const Handle(Geom2d_Curve)& C)
{
  isDone = Standard_True;

  Geom2dAdaptor_Curve  CC(C);
  GeomAbs_CurveType    CType = CC.GetType();
  Geom2dLProp_NumericCurInf2d NC;

  switch (CType) {
  case GeomAbs_Line:
    break;
  case GeomAbs_Circle:
    break;
  case GeomAbs_Ellipse:
      break;
  case GeomAbs_Hyperbola:
    break;
  case GeomAbs_Parabola:
    break;
  case GeomAbs_BSplineCurve:
    if (CC.Continuity() >= GeomAbs_C3 ) {
      NC.PerformInf(C,*this);
      isDone = NC.IsDone();
    }
    else {
      // Decoupage en intervalles C3.
      isDone = Standard_True;
      Standard_Integer NbInt = CC.NbIntervals(GeomAbs_C3);
      TColStd_Array1OfReal Param(1,NbInt+1);
      CC.Intervals(Param,GeomAbs_C3);

      for (Standard_Integer i = 1; i <= NbInt; i++) {
	NC.PerformInf(C,Param(i),Param(i+1),*this);
	if (!NC.IsDone()) {isDone = Standard_False;}
      }
    }
    break;

    default : {
      NC.PerformInf(C,*this);
      isDone = NC.IsDone();
    }
    break;
  }  
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dLProp_CurAndInf2d::IsDone() const 
{
  return isDone;
}


