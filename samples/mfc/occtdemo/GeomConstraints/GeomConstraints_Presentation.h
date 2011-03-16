// GeomConstraints_Presentation.h: interface for the GeomConstraints_Presentation class.
// Construct curves and surfaces from constraints
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GeomConstraints_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_GeomConstraints_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class GeomConstraints_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  GeomConstraints_Presentation();

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
  void sample5();
	void sample6();

private:
  // Array of pointers to sample functions
  typedef void (GeomConstraints_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif
