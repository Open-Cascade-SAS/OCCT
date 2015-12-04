// LProps_Presentation.h: interface for the LProps_Presentation class.
// Presentation class: Local properties of curves and surfaces
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_LProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <Geom2d_Curve.hxx>
class Quantity_Color;
class TopoDS_Wire;

class LProps_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  LProps_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sampleBezier();
  void samplePBSpline();
  void sampleBezierSurface();

  void showCurveLProps (Handle(Geom_Curve) theCurve,
    const Standard_CString theName,
    const Standard_Integer theNbPoints,
    const Standard_Real thePoints[]);
  // Draws the curve, shows local properties and updates the result dialog

  void showSurfaceLProps (Handle(Geom_Surface) theSurface,
    const Standard_CString theName,
    const Standard_Integer theNbPoints,
    const Standard_Real thePoints[][2]);

private:
  // Array of pointers to sample functions
  typedef void (LProps_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_LProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
