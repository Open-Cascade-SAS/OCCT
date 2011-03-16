// Intersections_Presentation.h: interface for the Intersections_Presentation class.
// Intersections of curves and surfaces
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Intersections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Intersections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class GProp_Intersections;
class TCollection_AsciiString;

class Intersections_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Intersections_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:

  void InterCurveCurve();
  void SelfInterCurveCurve();
  void InterCurveSurface();
  void InterSurfaceSurface();

private:
  // Array of pointers to sample functions
  typedef void (Intersections_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Intersections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
