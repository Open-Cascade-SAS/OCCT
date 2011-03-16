// Simplify_Presentation.h: interface for the Simplify_Presentation class.
// Simplify shape -> create a new shape based on triangulation of a given shape
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Simplify_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Simplify_PRESENTATION_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Simplify_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Simplify_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  void simplify(const TopoDS_Shape& aShape);

  // Sample functions
  void sample(const Standard_CString aFileName);  

};

#endif
