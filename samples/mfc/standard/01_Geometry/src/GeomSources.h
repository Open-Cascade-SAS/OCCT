// GeomSources.h: interface for the GeomSources class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX__H__FE726813_A2C6_11D1_8DA3_0800369C8A03__INCLUDED_)
#define AFX__H__FE726813_A2C6_11D1_8DA3_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Standard_Macro.hxx"
#include "ResultDialog.h"
#include "GeometryDoc.h"

class GeomSources  
{
public:
  Standard_EXPORT static void gpTest1(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest2(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest3(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest4(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest5(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest6(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest7(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest8(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest9(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest10(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest11(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest12(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest13(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest14(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest15(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest16(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest17(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest18(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest19(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest20(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest21(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest22(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest23(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest24(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest25(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest26(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest27(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest28(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest29(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest30(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest31(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest32(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest33(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest34(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest35(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest36(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest37(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest38(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest39(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest40(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest41(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest42(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest43(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest44(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest45(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest46(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest47(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest48(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest49(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest50(CGeometryDoc* aDoc);
  Standard_EXPORT static void gpTest51(CGeometryDoc* aDoc);
  GeomSources();
  virtual ~GeomSources();

private:

  enum DisplayType{No2DNo3D,   // 0 0 0
    No2D3D  ,   // 0 0 1
    a2DNo3D  ,   // 0 1 0
    a2D3D    };  // 1 1 1
  static  void PreProcess (CGeometryDoc* aDoc,DisplayType aDisplayType);
  static void PostProcess(CGeometryDoc* aDoc,
    UINT anID,
    DisplayType aDisplayType,
    const TCollection_AsciiString& aString,
    Standard_Boolean UpdateViewer = Standard_True,
    Standard_Real Coef = -1);
  static void DisplayPoint(CGeometryDoc* aDoc,
    const gp_Pnt2d& aPoint,
    const char* aText,
    Standard_Boolean UpdateViewer = Standard_False,
    Standard_Real anXoffset =0,
    Standard_Real anYoffset =0,
    Standard_Real TextScale = 0.05);
  static void DisplayPoint(CGeometryDoc* aDoc,
    const gp_Pnt& aPoint,
    const char* aText,
    Standard_Boolean UpdateViewer = Standard_False,
    Standard_Real anXoffset = 0,
    Standard_Real anYoffset = 0,
    Standard_Real aZoffset  = 0,
    Standard_Real TextScale = 0.05);

  static void DisplayCurve(CGeometryDoc* aDoc,
    Handle(Geom2d_Curve) aCurve,
    Standard_Integer aColorIndex = 4,
    Standard_Boolean UpdateViewer = false);
  static void DisplayCurveAndCurvature(CGeometryDoc* aDoc,
    Handle(Geom2d_Curve) aCurve,
    Standard_Integer aColorIndex = 4,
    Standard_Boolean UpdateViewer = false);

  static void DisplayCurve(CGeometryDoc* aDoc,
    Handle(Geom_Curve) aCurve,
    Quantity_NameOfColor aNameOfColor, 
    Standard_Boolean UpdateViewer = false);

  static void DisplayCurve(CGeometryDoc* aDoc,
    Handle(Geom_Curve) aCurve,
    Standard_Boolean UpdateViewer = false);

  static void DisplaySurface  (CGeometryDoc* aDoc,
    Handle(Geom_Surface) aSurface,
    Quantity_NameOfColor aNameOfColor, 
    Standard_Boolean UpdateViewer = false);

  static void DisplaySurface  (CGeometryDoc* aDoc,
    Handle(Geom_Surface) aSurface,
    Standard_Boolean UpdateViewer = false);

  static void ResetView(CGeometryDoc* aDoc);

  static void AddSeparator(CGeometryDoc* aDoc,TCollection_AsciiString& aMessage);


};

#endif // !defined(AFX__H__FE726813_A2C6_11D1_8DA3_0800369C8A03__INCLUDED_)
