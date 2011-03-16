// Extrema_Presentation.h: interface for the Extrema_Presentation class.
// Calculation of extrema between geometries or shapes
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Extrema_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Extrema_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class gp_Pnt;

class Extrema_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Extrema_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  void displayExtemaEdge(const gp_Pnt&, const gp_Pnt&);
  void doShapeShapeExtrema(const TopoDS_Shape&, const TopoDS_Shape&,
    TCollection_AsciiString& theText);

  // Sample functions
  void sampleCurveCurve();
  void sampleCurveSur();
  void sampleSurSur();
  void sampleShapeShape();
  void sampleShellShell();

private:
  // Array of pointers to sample functions
  typedef void (Extrema_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Extrema_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
