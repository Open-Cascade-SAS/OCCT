// DCA_Presentation.h: interface for the DCA_Presentation class.
// Geometry Direct Construction Algorithms
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCA_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_DCA_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class DCA_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  DCA_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sampleLine();
  void sampleParabola();
  void sampleHyperbola();
  void sampleCircle();
  void sampleEllipse();
  void samplePlane();
  void sampleCylindricalSurface();
  void sampleToroidalSurface();
  void sampleConicalSurface(); 
  void sampleSphericalSurface(); 

  void Comment(const Standard_CString theTitle,TCollection_AsciiString& theText);

private:
  // Array of pointers to sample functions
  typedef void (DCA_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_DCA_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
