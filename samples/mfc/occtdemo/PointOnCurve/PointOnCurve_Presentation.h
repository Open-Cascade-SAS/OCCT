// PointOnCurve_Presentation.h: interface for the PointOnCurve_Presentation class.
// Calculation of points on Curves
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PointOnCurve_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_PointOnCurve_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class Geom_Curve;

class PointOnCurve_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  PointOnCurve_Presentation();
private:
  Handle(AIS_InteractiveObject) PointOnCurve_Presentation::DrawCurve (
             Handle(Geom_Curve) theCurve, Quantity_NameOfColor theColor);

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

  void Comment(const Standard_CString theString,
      TCollection_AsciiString& theText, const Standard_Real theNum1 , 
      const Standard_Real theNum2 = 0, const Standard_Real theNum3 = 0);
  void Comment(const Standard_CString theString,
      TCollection_AsciiString& theText,const Standard_Real theDistance,
      const Standard_Integer theNum);

private:
  // Array of pointers to sample functions
  typedef void (PointOnCurve_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_PointOnCurve_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
