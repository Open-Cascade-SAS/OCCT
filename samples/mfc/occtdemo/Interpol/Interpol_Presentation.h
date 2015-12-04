// Interpol_Presentation.h: interface for the Interpol_Presentation class.
// Presentation class: Interpolation of a set of points to form a curve
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Interpol_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Interpol_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Geom_BSplineCurve;
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

class Interpol_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Interpol_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  void interpolate (Standard_Real[][3], 
    Standard_Integer, TCollection_AsciiString&,
    const TColStd_Array1OfInteger& thePointIndexes,
    const TColStd_Array1OfReal& theAngles);

  // Sample functions
  void sample1();
  void sample2();
  void sample3();

private:
  // Array of pointers to sample functions
  typedef void (Interpol_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Interpol_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
