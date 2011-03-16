// TopLProps_Presentation.h: interface for the TopLProps_Presentation class.
// Presentation class: Determine the local properties of shapes.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TopLProps_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_TopLProps_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
//#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

class TopLProps_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  TopLProps_Presentation();

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
  void showEdgeLProps(TopoDS_Edge& theEdge,
                      const Standard_Integer theNbPoints,
                      const Standard_Real thePoints[]);
  void showFaceLProps(TopoDS_Face& theFace,
                      const Standard_Integer theNbPoints,
                      const Standard_Real thePoints[][2]);
  
private:
  // Array of pointers to sample functions
  typedef void (TopLProps_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_TopLProps_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
