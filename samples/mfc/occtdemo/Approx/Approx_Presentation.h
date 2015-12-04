// Approx_Presentation.h: interface for the Approx_Presentation class.
// Presentation class: Approximation of curves and surfaces.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Approx_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Approx_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <Geom_BSplineSurface.hxx>

class Approx_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Approx_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sample1();
  void sample2();
  void sample3();
  void sample4();
  Standard_Boolean DrawModifyBSplineSurface(TColStd_Array2OfReal& ,
                                            Standard_Real ,Standard_Real ,
                                            Standard_Integer ,Standard_Integer ,
                                            Standard_Real,Handle(AIS_InteractiveObject)&,
                                            Handle(AIS_InteractiveObject)&,
                                            Standard_Integer);

  Handle(Geom_BSplineSurface) CreateBSplineSurface(TColStd_Array2OfReal& ,
                                                  Standard_Real ,Standard_Real,
                                                  Standard_Integer);

private:
  // Array of pointers to sample functions
  typedef void (Approx_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];
 
};

#endif // !defined(AFX_Approx_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
