// Transform_Presentation.h: interface for the Transform_Presentation class.
// Transform shapes: apply gp_Trsf or gp_GTrsf transformation
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Transform_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Transform_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Transform_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Transform_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sample1();
  void sample2();

  Standard_Boolean readShape (const Standard_CString theFileName,
    TopoDS_Shape& theShape);

private:
  // Array of pointers to sample functions
  typedef void (Transform_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Transform_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
