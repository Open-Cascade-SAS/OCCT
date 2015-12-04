// Offset2d_Presentation.h: interface for the Offset2d_Presentation class.
// Presentation class: Offset of curves and wires
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Offset2d_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Offset2d_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <Geom2d_Curve.hxx>
class Quantity_Color;
class TopoDS_Wire;

class Offset2d_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Offset2d_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sampleCircle();
  void sampleEllipse();
  void sampleParabola();
  void sampleHyperbola();
  void sampleBezier();
  void samplePBSpline();
  void sampleWire1();
  void sampleWire2();

  void drawAndOffsetCurve2d (Handle(Geom2d_Curve) theCurve,
    const Standard_CString theName,
    TCollection_AsciiString& theText);
  void drawAndOffsetWire (const TopoDS_Wire& theWire,
    const Standard_CString theName,
    TCollection_AsciiString& theText);

private:
  // Array of pointers to sample functions
  typedef void (Offset2d_Presentation::*PSampleFuncType)();
  typedef struct
  {
    PSampleFuncType pFunc;
    int             nNeg;
    Standard_Real   dNeg;
    int             nPos;
    Standard_Real   dPos;
  } SampleDescrType;
  static const SampleDescrType SampleDescrs[];

};

#endif // !defined(AFX_Offset2d_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
