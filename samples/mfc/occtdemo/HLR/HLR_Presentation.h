// HLR_Presentation.h: interface for the HLR_Presentation class.
// Hidden lines removal
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLR_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_HLR_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class TopoDS_Shape;
class Prs3d_Projector;
class gp_Trsf;
class TopLoc_Location;


class HLR_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  HLR_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  Handle(Prs3d_Projector) Projector(gp_Trsf& theTrsf);
  Standard_Boolean HLR(const TopoDS_Shape&, const Standard_Integer nbIso);
  Standard_Boolean PolyHLR(const TopoDS_Shape&);
  Standard_Boolean drawHLR(TopoDS_Shape&, TopoDS_Shape&, const TopLoc_Location&, 
    const enum Quantity_NameOfColor);

  // Sample functions
  void sampleCylinder();
  void sampleTorus();
  void sampleBrepShape2();
  void sampleBrepShape1();

private:
  // Array of pointers to sample functions
  typedef void (HLR_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_HLR_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
