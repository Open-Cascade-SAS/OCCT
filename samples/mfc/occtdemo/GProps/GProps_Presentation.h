// GProps_Presentation.h: interface for the GProps_Presentation class.
// Global Properties of Shapes
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_GProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class GProp_GProps;
class TCollection_AsciiString;

class GProps_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  GProps_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  Standard_Boolean GProps(GProp_GProps& aProps, gp_Ax1&,
    TCollection_AsciiString& theText, TCollection_AsciiString theMassStr);

  void samplePoints();
  void sampleCurves();
  void sampleSurfaces();
  void sampleVolumes();
  void sampleSystem();

private:
  // Array of pointers to sample functions
  typedef void (GProps_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_GProps_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
