// OCC_3dBaseDoc.h: interface for the OCC_3dBaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_3DBASEDOC_H__02CE7BD9_39BE_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_3DBASEDOC_H__02CE7BD9_39BE_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseDoc.h"
#include "DimensionDlg.h"
#include <Standard_Macro.hxx>

class AFX_EXT_CLASS OCC_3dBaseDoc : public OCC_BaseDoc
{
public:

  OCC_3dBaseDoc();
  ~OCC_3dBaseDoc();

  void SetMaterial(Graphic3d_NameOfMaterial Material);


  virtual void DragEvent (const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Standard_Integer theState,
                          const Handle(V3d_View)& theView);

  virtual void InputEvent (const Standard_Integer theMouseX,
                           const Standard_Integer theMouseY,
                           const Handle(V3d_View)& theView);

  virtual void MoveEvent (const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Handle(V3d_View)& theView);

  virtual void ShiftMoveEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Handle(V3d_View)& theView);

  virtual void ShiftDragEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Standard_Integer theState,
                               const Handle(V3d_View)& theView);

  virtual void ShiftInputEvent (const Standard_Integer theMouseX,
                                const Standard_Integer theMouseY,
                                const Handle(V3d_View)& theView);

  virtual void Popup (const Standard_Integer theMouseX,
                      const Standard_Integer theMouseY,
                      const Handle(V3d_View)& theView);

  static void Fit();

  int  OnFileImportBrep_WithInitDir (const wchar_t* InitialDir);

  void OnObjectRayTracingAction();

  // Generated message map functions
protected:
  //{{AFX_MSG(OCC_3dBaseDoc)
  afx_msg void OnFileImportBrep();
  afx_msg void OnFileExportBrep();
  afx_msg void OnObjectErase();
  afx_msg void OnUpdateObjectErase(CCmdUI* pCmdUI);
  afx_msg void OnObjectColor();
  afx_msg void OnUpdateObjectColor(CCmdUI* pCmdUI);
  afx_msg void OnObjectShading();
  afx_msg void OnUpdateObjectShading(CCmdUI* pCmdUI);
  afx_msg void OnObjectWireframe();
  afx_msg void OnUpdateObjectWireframe(CCmdUI* pCmdUI);
  afx_msg void OnObjectTransparency();
  afx_msg void OnUpdateObjectTransparency(CCmdUI* pCmdUI) ;
  afx_msg void OnObjectMaterial();
  afx_msg void OnUpdateObjectMaterial(CCmdUI* pCmdUI);
  afx_msg BOOL OnObjectMaterialRange(UINT nID);
  afx_msg void OnUpdateObjectMaterialRange(CCmdUI* pCmdUI);
  afx_msg void OnObjectDisplayall();
  afx_msg void OnUpdateObjectDisplayall(CCmdUI* pCmdUI);
  afx_msg void OnObjectRemove();
  afx_msg void OnUpdateObjectRemove(CCmdUI* pCmdUI);

  afx_msg void OnUpdateV3dButtons(CCmdUI* pCmdUI);
  afx_msg void OnObjectRayTracing();
  afx_msg void OnObjectShadows();
  afx_msg void OnObjectReflections();
  afx_msg void OnObjectAntiAliasing();

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

protected:
  bool myRayTracingIsOn;
  bool myRaytracedShadowsIsOn;
  bool myRaytracedReflectionsIsOn;
  bool myRaytracedAntialiasingIsOn;
  int myPopupMenuNumber;
};

#endif // !defined(AFX_OCC_3dBaseDoc_H__02CE7BD9_39BE_11D7_8611_0060B0EE281E__INCLUDED_)
