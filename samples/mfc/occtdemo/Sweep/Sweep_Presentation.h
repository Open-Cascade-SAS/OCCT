// Sweep_Presentation.h: interface for the Sweep_Presentation class.
// Sweeping capabilities presentation
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Sweep_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Sweep_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class Geom2d_Curve;
class Geom_Surface;

class Sweep_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Sweep_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

  Handle(AIS_InteractiveObject) drawCurveOnSurface
    (const Handle(Geom2d_Curve)& aC2d,
     const Handle(Geom_Surface)& aSurf,
     const Quantity_Color& aColor);

private:
  // Sample functions
  void sample1();
  void sample2();
  void sample3();
  void sample4();
  void sample5();

private:
  // Array of pointers to sample functions
  typedef void (Sweep_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Sweep_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
