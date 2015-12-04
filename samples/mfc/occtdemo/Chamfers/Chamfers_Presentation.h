// Chamfers_Presentation.h: interface for the Chamfers_Presentation class.
// Presentation class: Example on chamfers and fillets
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Chamfers_PRESENTATION_H)
#define AFX_Chamfers_PRESENTATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
#include <ChFi3d_FilletShape.hxx>

class AIS_InteractiveObject;
class TopoDS_Solid;

class Chamfers_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Chamfers_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

private:
  Standard_Boolean createChamferOnBox(
    TopoDS_Solid&, Standard_Real, Standard_Real, 
    Handle(AIS_InteractiveObject)&);

  Standard_Boolean createDiverseChamferOnBox(
    TopoDS_Solid&, Standard_Real, Standard_Real, 
    Standard_Real, Handle(AIS_InteractiveObject)&);

  Standard_Boolean createFilletOnBox(
    TopoDS_Solid&, Standard_Real, ChFi3d_FilletShape, 
    Handle(AIS_InteractiveObject)&);

  // Sample functions
  void sampleBoxChamfer();
  void sampleBoxFillet();
  void sampleBoxCylChamfer();
  void sampleBoxCylAngChamfer();
  void sampleBoxCylFillet();
  void sampleBoxCylAngFillet();

private:
  // Array of pointers to sample functions
  typedef void (Chamfers_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Chamfers_PRESENTATION_H)
