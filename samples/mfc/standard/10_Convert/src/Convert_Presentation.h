// Convert_Presentation.h: interface for the Convert_Presentation class.
// Conversion of elementary geometry to BSpline curves and surfaces
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Convert_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Convert_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

class Quantity_Color;

class Convert_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Convert_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sampleCircle();
  void sampleEllipse();
  void sampleBezier();
  void sampleBezierSurface();
  void sampleCylindricalSurface();
  void sampleRevolSurface();
  void sampleToroidalSurface();
  void sampleConicalSurface(); 
  void sampleSphericalSurface(); 

  void drawCurveAndItsBSpline (Handle(Geom_Curve) theCurve, 
    const Standard_CString theName, TCollection_AsciiString& theText);

  void drawSurfaceAndItsBSpline (const Handle(Geom_Surface) & theSurface, 
    const Standard_CString theName, TCollection_AsciiString& theText);

private:
  // Array of pointers to sample functions
  typedef void (Convert_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

  int myNbFuncs;
};

#endif // !defined(AFX_Convert_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
