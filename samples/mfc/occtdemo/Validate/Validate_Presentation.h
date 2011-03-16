// Validate_Presentation.h: interface for the Validate_Presentation class.
// Checking validity of shapes
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Validate_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Validate_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>

class Validate_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Validate_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sample(const Standard_CString aFileName);
  void checkShape(const TopoDS_Shape& aShape);

  static Standard_CString myFileNames[];

};

#endif // !defined(AFX_Validate_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
