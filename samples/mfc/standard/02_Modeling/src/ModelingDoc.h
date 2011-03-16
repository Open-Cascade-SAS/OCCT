// ModelingDoc.h : interface of the CModelingDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <OCC_3dDoc.h>

class CModelingDoc : public OCC_3dDoc
{
	
protected: // create from serialization only
	CModelingDoc();
	DECLARE_DYNCREATE(CModelingDoc)

public:
	virtual ~CModelingDoc();

	void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );
	void Popup          (const Standard_Integer  x       ,
		    			 const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );

// Generated message map functions
protected:
	//{{AFX_MSG(CModelingDoc)
	afx_msg void OnMirror();
	afx_msg void OnMirroraxis();
	afx_msg void OnRotate();
	afx_msg void OnScale();
	afx_msg void OnTranslation();
	afx_msg void OnDisplacement();
	afx_msg void OnDeform();
	afx_msg void OnBox();
	afx_msg void OnCylinder();
	afx_msg void OnCone();
	afx_msg void OnSphere();
	afx_msg void OnTorus();
	afx_msg void OnWedge();
	afx_msg void OnPrism();
	afx_msg void OnRevol();
	afx_msg void OnPipe();
	afx_msg void OnThru();
	afx_msg void OnEvolved();
	afx_msg void OnDraft();
	afx_msg void OnCut();
	afx_msg void OnFuse();
	afx_msg void OnSection();
	afx_msg void OnCommon();
	afx_msg void OnPsection();
	afx_msg void OnBlend();
	afx_msg void OnChamf();
	afx_msg void OnEvolvedblend();
	afx_msg void OnPrismLocal();
	afx_msg void OnRevolLocal();
	afx_msg void OnGlueLocal();
	afx_msg void OnDprismLocal();
	afx_msg void OnPipeLocal();
	afx_msg void OnLinearLocal();
	afx_msg void OnSplitLocal();
	afx_msg void OnThickLocal();
	afx_msg void OnOffsetLocal();
	afx_msg void OnVertex();
	afx_msg void OnEdge();
	afx_msg void OnWire();
	afx_msg void OnFace();
	afx_msg void OnShell();
	afx_msg void OnCompound();
	afx_msg void OnGeometrie();
	afx_msg void OnSewing();
	afx_msg void OnExplorer();
	afx_msg void OnBuilder();
	afx_msg void OnValid();
	afx_msg void OnLinear();
	afx_msg void OnVolume();
	afx_msg void OnSurface();
	afx_msg void OnFillwithtang();
	afx_msg void OnButtonFill();
	afx_msg void OnStopStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int myState;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
