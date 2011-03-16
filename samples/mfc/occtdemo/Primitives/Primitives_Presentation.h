// Primitives_Presentation.h: interface for the Primitives_Presentation class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Primitives_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Primitives_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Primitives_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Primitives_Presentation();

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

private:
  // Array of pointers to sample functions
  typedef void (Primitives_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Primitives_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
