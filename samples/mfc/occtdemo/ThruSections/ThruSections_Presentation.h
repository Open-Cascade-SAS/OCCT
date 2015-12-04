// ThruSections_Presentation.h: interface for the ThruSections_Presentation class.
// Building shapes passed through sections.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ThruSections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_ThruSections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <TopTools_SequenceOfShape.hxx>
//class TCollection_AsciiString;

class ThruSections_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  ThruSections_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sample1();
  void sample2();
  TopoDS_Shape CreateShape(TopTools_SequenceOfShape& aWires,
                           Standard_Boolean IsSolid = Standard_False,
                           Standard_Boolean IsRuled = Standard_False,
                           Standard_Real aPrec = 1.0e-06);
  
private:
  // Array of pointers to sample functions
  typedef void (ThruSections_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_ThruSections_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
