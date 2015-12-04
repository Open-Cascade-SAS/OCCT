// Glue_Presentation.h: interface for the Glue_Presentation class.
// Glue two solid shapes with contiguous faces
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Glue_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Glue_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class GProp_Glue;
class TCollection_AsciiString;
class TopoDS_Shape;
class AIS_InteractiveObject;
#include <TopTools_ListOfShape.hxx>

class Glue_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Glue_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  // Sample functions

  // displays the shapes to be glued, given same domain faces and edges
  // and at last - the glued shape
  Standard_Boolean displayShapesFaces(TopoDS_Shape&, TopoDS_Shape&,
    TopoDS_Shape&, TopTools_ListOfShape&, TopTools_ListOfShape&,
    TopTools_ListOfShape&, TopTools_ListOfShape&);

  // display a given shape in wire frame mode
  Standard_Boolean drawShapeWf(TopoDS_Shape&,Handle(AIS_InteractiveObject)&);

  // display a given shape in shaded mode
  Standard_Boolean drawShapeSh(TopoDS_Shape&,Handle(AIS_InteractiveObject)&);

  void sampleBoxes();
  void sampleCylinder();
  void sampleMoreBoxes();

private:
  // Array of pointers to sample functions
  typedef void (Glue_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Glue_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
