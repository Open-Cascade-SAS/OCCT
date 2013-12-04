// Projection_Presentation.h: interface for the Projection_Presentation class.
// Projection of points on curves and curves on surfaces
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Projection_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Projection_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Projection_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Projection_Presentation();

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

private:
  // Array of pointers to sample functions
  typedef void (Projection_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif
