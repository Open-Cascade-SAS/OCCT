// SplitShape_Presentation.h: interface for the SplitShape_Presentation class.
// Split shape by wire
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SplitShape_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_SplitShape_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class BRepAlgoAPI_Section;
class TopoDS_Wire;
class TopoDS_Shell;

class SplitShape_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  SplitShape_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions
  void sample1();
  void sample2();

  Standard_Boolean readShell (TCollection_AsciiString& theText);
  Standard_Boolean computeSection (BRepAlgoAPI_Section& aSecAlgo,
    TopoDS_Wire& aSecWire, TCollection_AsciiString& theText);
  Standard_Boolean splitAndComposeLeft (const BRepAlgoAPI_Section& aSecAlgo, 
    const TopoDS_Wire& aSecWire, TopoDS_Shell& aNewShell, TCollection_AsciiString& theText);

private:
  // Array of pointers to sample functions
  typedef void (SplitShape_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

  TopoDS_Shape myShell;

};

#endif // !defined(AFX_SplitShape_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
