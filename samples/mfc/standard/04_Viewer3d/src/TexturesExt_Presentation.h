// TexturesExt_Presentation.h: interface for the TexturesExt_Presentation class.
// Creation of textural presentation of shape
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TexturesExt_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_TexturesExt_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCCDemo_Presentation.h"
#include <AIS_Shape.hxx>
class TopoDS_Shape;
class TCollection_AsciiString;

class TexturesExt_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  TexturesExt_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

  virtual void Init();

private:
  // display an AIS_Shape based on a given shape with texture with given filename
  // filename can also be an integer value ("2", "5", etc.), in this case
  // a predefined texture from Graphic3d_NameOfTexture2D with number = this value
  // is loaded.
  Handle(AIS_Shape) Texturize(
    const TopoDS_Shape& aShape, TCollection_AsciiString aTFileName, 
    Standard_Real toScaleU=1.0, Standard_Real toScaleV=1.0, 
    Standard_Real toRepeatU=1.0, Standard_Real toRepeatV=1.0,
    Standard_Real originU=0.0, Standard_Real originV=0.0);
  
  // loads a shape from a given brep file from data dir into a given TopoDS_Shape object
  Standard_Boolean loadShape(TopoDS_Shape&, TCollection_AsciiString);

  // turns 6 diretional lights on/off for brighter demonstration
  void lightsOnOff(Standard_Boolean isOn);

  // Sample functions
  void sampleBottle();
  void sampleTerrain();
  void sampleKitchen();

private:
  // Array of pointers to sample functions
  typedef void (TexturesExt_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_TexturesExt_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
