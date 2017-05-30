// GeometryDoc.h : interface of the CGeometryDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OCC_3dBaseDoc.h"
#include "ResultDialog.h"

#include <AIS_Point.hxx>

class CGeometryDoc : public OCC_3dBaseDoc
{
public:
  void Put2DOnTop(bool isMax = true);
  void Put3DOnTop(bool isMax = true);
  void Fit2DViews();
  void Set3DViewsZoom(const Standard_Real& Coef );
  void Fit3DViews(Standard_Real Coef);
  void simplify(const TopoDS_Shape& aShape);


  //-------------------- 2D -------------------//

  void DragEvent2D       (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Standard_Integer  TheState,
                          const Handle(V3d_View)& aView   );
  void InputEvent2D      (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Handle(V3d_View)& aView   );
  void MoveEvent2D       (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Handle(V3d_View)& aView   );
  void ShiftMoveEvent2D  (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Handle(V3d_View)& aView   );
  void ShiftDragEvent2D  (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Standard_Integer  TheState,
                          const Handle(V3d_View)& aView   );
  void ShiftInputEvent2D (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Handle(V3d_View)& aView   );
  void Popup2D           (const Standard_Integer  x       ,
                          const Standard_Integer  y       ,
                          const Handle(V3d_View)& aView   );

  //-------------------- 3D -------------------//

  virtual void Popup (const Standard_Integer theMouseX,
                        const Standard_Integer theMouseY,
                        const Handle(V3d_View)& theView);

  virtual void InputEvent (const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Handle(V3d_View)& theView);

  Handle(AIS_InteractiveObject) drawSurface
    (const Handle(Geom_Surface)& theSurface,
    const Quantity_Color& theColor,
    const Standard_Boolean toDisplay);

  Standard_Boolean WaitForInput (unsigned long aMilliSeconds);
  // Waits for a user input or a period of time has been elapsed

  Handle(AIS_Point) drawPoint (const gp_Pnt& thePnt,
                              const Quantity_Color& theColor = Quantity_Color(Quantity_NOC_GREEN),
                              const Standard_Boolean toDisplay = Standard_True);
  // creates a presentation of the given point
  // and displays it in the viewer if toDisplay = Standard_True

  Handle(AIS_Shape) drawShape (const TopoDS_Shape& theShape,
                              const Graphic3d_NameOfMaterial theMaterial = Graphic3d_NOM_BRASS,
                              const Standard_Boolean toDisplay = Standard_True);
  // creates a presentation of the given shape with the given material
  // (color is default for a given material)
  // and displays it in the viewer if toDisplay = Standard_True

protected: // create from serialization only
  CGeometryDoc();
  DECLARE_DYNCREATE(CGeometryDoc)

  // Attributes
public:

  // Operations
public:

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGeometryDoc)
public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);
  virtual void OnCloseDocument();
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CGeometryDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

  // Generated message map functions
protected:
  //{{AFX_MSG(CGeometryDoc)
  afx_msg void OnWindowNew2d();
  afx_msg void OnBUTTONTest1();
  afx_msg void OnBUTTONTest2();
  afx_msg void OnBUTTONTest3();
  afx_msg void OnBUTTONTest4();
  afx_msg void OnBUTTONTest5();
  afx_msg void OnBUTTONTest6();
  afx_msg void OnBUTTONTest7();
  afx_msg void OnBUTTONTest8();
  afx_msg void OnBUTTONTest9();
  afx_msg void OnBUTTONTest10();
  afx_msg void OnBUTTONTest11();
  afx_msg void OnBUTTONTest12();
  afx_msg void OnBUTTONTest13();
  afx_msg void OnBUTTONTest14();
  afx_msg void OnBUTTONTest15();
  afx_msg void OnBUTTONTest16();
  afx_msg void OnBUTTONTest17();
  afx_msg void OnBUTTONTest18();
  afx_msg void OnBUTTONTest19();
  afx_msg void OnBUTTONTest20();
  afx_msg void OnBUTTONTest21();
  afx_msg void OnBUTTONTest22();
  afx_msg void OnBUTTONTest23();
  afx_msg void OnBUTTONTest24();
  afx_msg void OnBUTTONTest25();
  afx_msg void OnBUTTONTest26();
  afx_msg void OnBUTTONTest27();
  afx_msg void OnBUTTONTest28();
  afx_msg void OnBUTTONTest29();
  afx_msg void OnBUTTONTest30();
  afx_msg void OnBUTTONTest31();
  afx_msg void OnBUTTONTest32();
  afx_msg void OnBUTTONTest33();
  afx_msg void OnBUTTONTest34();
  afx_msg void OnBUTTONTest35();
  afx_msg void OnBUTTONTest36();
  afx_msg void OnBUTTONTest37();
  afx_msg void OnBUTTONTest38();
  afx_msg void OnBUTTONTest39();
  afx_msg void OnBUTTONTest40();
  afx_msg void OnBUTTONTest41();
  afx_msg void OnBUTTONTest42();
  afx_msg void OnBUTTONTest43();
  afx_msg void OnBUTTONTest44();
  afx_msg void OnBUTTONTest45();
  afx_msg void OnBUTTONTest46();
  afx_msg void OnBUTTONTest47();
  afx_msg void OnBUTTONTest48();
  afx_msg void OnBUTTONTest49();
  afx_msg void OnBUTTONTest50();
  afx_msg void OnUpdateBUTTONTest1(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest2(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest3(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest4(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest5(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest6(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest7(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest8(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest9(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest10(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest11(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest12(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest13(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest14(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest15(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest16(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest17(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest18(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest19(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest20(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest21(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest22(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest23(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest24(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest25(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest26(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest27(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest28(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest29(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest30(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest31(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest32(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest33(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest34(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest35(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest36(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest37(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest38(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest39(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest40(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest41(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest42(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest43(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest44(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest45(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest46(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest47(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest48(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest49(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONTest50(CCmdUI* pCmdUI);
  afx_msg void OnCreateSol();
  afx_msg void OnSimplify();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

public:
  int Current;
  void Minimize3D();
  void Minimize2D();
  Handle(V3d_Viewer) GetViewer2D() { return myViewer2D; };
  Handle(AIS_InteractiveContext)& GetISessionContext() { return myAISContext2D; };
  BOOL FitMode;

public:
  CResultDialog myCResultDialog;

private:
  Handle(V3d_Viewer) myViewer2D;
  Handle(AIS_InteractiveContext) myAISContext2D;


};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
