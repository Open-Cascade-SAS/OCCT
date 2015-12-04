// Viewer3dDoc.h : interface of the CViewer3dDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
#define AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OCCDemo_Presentation.h"
#include "OffsetDlg.h"
#include "OCC_3dDoc.h"
#include "ResultDialog.h"
#include "User_Cylinder.hxx"


#include <AIS_Trihedron.hxx>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

class COffsetDlg;
class OCCDemo_Presentation;

class CViewer3dDoc : public OCC_3dDoc
{
  DECLARE_DYNCREATE(CViewer3dDoc)

protected: // create from serialization only
  CViewer3dDoc();
  void InitViewButtons();
  void DoSample();

public:
  void SetMyStaticTrihedronAxisIsDisplayed(BOOL IsDisplayed);
  Handle(AIS_Shape) GetBox();
  Handle(AIS_Shape) GetSphere();
  Handle(User_Cylinder) GetCylinder();
  Handle(AIS_Shape) GetOverlappedBox();
  Handle(AIS_Shape) GetOverlappedSphere();
  Handle(AIS_Shape) GetOverlappedCylinder();
  void Start();
  Standard_CString GetDataDir() {return myDataDir;}
  static void Fit();

  virtual ~CViewer3dDoc();
  void UpdateResultMessageDlg (CString theTitle, const TCollection_AsciiString& theMessage);
  void UpdateResultMessageDlg (CString theTitle, CString theMessage);
  virtual BOOL OnNewDocument();
  virtual void Popup (const Standard_Integer x,
                      const Standard_Integer y,
                      const Handle(V3d_View)& aView);

  virtual void DragEvent (const Standard_Integer x,
                          const Standard_Integer y,
                          const Standard_Integer TheState,
                          const Handle(V3d_View)& aView);

  virtual void InputEvent (const Standard_Integer x,
                           const Standard_Integer y,
                           const Handle(V3d_View)& aView);

  virtual void ShiftDragEvent (const Standard_Integer x,
                               const Standard_Integer y,
                               const Standard_Integer TheState,
                               const Handle(V3d_View)& aView);

  virtual void ShiftInputEvent (const Standard_Integer x,
                                const Standard_Integer y,
                                const Handle(V3d_View)& aView);

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  // Generated message map functions
protected:
  BOOL myStaticTrihedronAxisIsDisplayed;	
  //{{AFX_MSG(CViewer3dDoc)
  afx_msg void OnBox();
  afx_msg void OnCylinder();
  afx_msg void OnSphere();
  afx_msg void OnRemoveAll();
  afx_msg void OnNbisos();
  afx_msg void OnFaces();
  afx_msg void OnEdges();
  afx_msg void OnVertices();
  afx_msg void OnNeutral();
  afx_msg void OnUsercylinderChangefacecolor();
  afx_msg void OnFillet3d();
  afx_msg void OnCircle();
  afx_msg void OnLine();
  afx_msg void OnOverlappedBox();
  afx_msg void OnOverlappedCylinder();
  afx_msg void OnOverlappedSphere();
  afx_msg void OnPolygonOffsets();
  afx_msg void OnUpdatePolygonOffsets(CCmdUI* pCmdUI);
  afx_msg void OnUpdateCylinder(CCmdUI* pCmdUI);
  afx_msg void OnUpdateSphere(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBox(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOverlappedCylinder(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOverlappedSphere(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOverlappedBox(CCmdUI* pCmdUI);
  afx_msg void OnObjectRemove();
  afx_msg void OnObjectErase();
  afx_msg void OnObjectDisplayall();
  afx_msg void OnObjectColoredMesh();
  afx_msg void OnUpdateObjectColoredMesh(CCmdUI* pCmdUI);
  afx_msg void OnUpdateObjectShading(CCmdUI* pCmdUI);
  afx_msg void OnUpdateObjectWireframe(CCmdUI* pCmdUI);
  afx_msg void OnOptionsTrihedronDynamicTrihedron();
  afx_msg void OnUpdateOptionsTrihedronDynamicTrihedron(CCmdUI* pCmdUI);
  afx_msg void OnUpdateOptionsTrihedronStaticTrihedron(CCmdUI* pCmdUI);
  afx_msg void OnTextureOn();
  afx_msg void OnBUTTONNext();
  afx_msg void OnBUTTONStart();
  afx_msg void OnBUTTONRepeat();
  afx_msg void OnBUTTONPrev();
  afx_msg void OnBUTTONEnd();
  afx_msg void OnUpdateBUTTONNext(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONPrev(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONStart(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONRepeat(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBUTTONEnd(CCmdUI* pCmdUI);
  afx_msg void OnFileNew();
  afx_msg void OnBUTTONShowResult();
  afx_msg void OnDumpView();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  //Attributes
protected:
  int myState;

private:
  COffsetDlg* myOffsetDlg;

  Handle(User_Cylinder) myCylinder;
  Handle(AIS_Shape) mySphere;
  Handle(AIS_Shape) myBox;
  Handle(AIS_Shape) myOverlappedCylinder;
  Handle(AIS_Shape) myOverlappedSphere;
  Handle(AIS_Shape) myOverlappedBox;
  Handle(AIS_Trihedron) myTrihedron;

  OCCDemo_Presentation *myPresentation;
  char myDataDir[5];         // for "Data\0"
  char myLastPath[MAX_PATH]; // directory of lastly saved file in DumpView()
  bool isTextureSampleStarted;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERDOC_H__4EF39FBA_4EBB_11D1_8D67_0800369C8A03__INCLUDED_)
