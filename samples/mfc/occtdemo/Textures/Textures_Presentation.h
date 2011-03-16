// Textures_Presentation.h: interface for the Textures_Presentation class.
// Creation of textural presentation of shape
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Textures_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
#define AFX_Textures_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <OCCDemo_Presentation.h>
class TopoDS_Shape;
class TCollection_AsciiString;

class Textures_Presentation : public OCCDemo_Presentation
{
public:
  // Construction
  Textures_Presentation();

public:
  // Iteration on samples
  virtual void DoSample();
  // one phase of iterations

  virtual void Init();

private:
  // display an AIS_TexturedShape based on a given shape with texture with given filename
  // filename can also be an integer value ("2", "5", etc.), in this case
  // a predefined texture from Graphic3d_NameOfTexture2D with number = this value
  // is loaded.
  // the function returns !WAIT_A_SECOND
  Standard_Boolean displayTextured(const TopoDS_Shape&, 
                                   TCollection_AsciiString aTFileName,
                                   Standard_Boolean toDisplayOrigina = Standard_True,
                                   Standard_Boolean toScale = Standard_False,
                                   Standard_Integer nRepeat = 5);
  
  // loads a shape from a given brep file from data dir into a given TopoDS_Shape object
  Standard_Boolean loadShape(TopoDS_Shape&, TCollection_AsciiString);

  // turns 6 diretional lights on/off for brighter demonstration
  void lightsOnOff(Standard_Boolean isOn);

  // Sample functions
  void sampleCustomTexture1();
  void sampleCustomTexture2();
  void sampleCustomTexture3();
//  void samplePredefTexture1();
//  void samplePredefTexture2();

private:
  // Array of pointers to sample functions
  typedef void (Textures_Presentation::*PSampleFuncType)();
  static const PSampleFuncType SampleFuncs[];

};

#endif // !defined(AFX_Textures_Presentation_H__790EED7F_7BA2_11D5_BA4A_0060B0EE18EA__INCLUDED_)
