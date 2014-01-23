///////////////////////////////////////////////////////////////////////////////
// OCC_StereoConfigDlg.h : header file
///////////////////////////////////////////////////////////////////////////////

#ifndef OCC_StereoConfigDlg_Header
#define OCC_StereoConfigDlg_Header

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "res\OCC_Resource.h"
#include <V3d_View.hxx>

// Dialog to dynamically configure 3D Viewer stereo
// projection properties.
class AFX_EXT_CLASS OCC_StereoConfigDlg : public CDialog
{
public:

  OCC_StereoConfigDlg (CWnd* theParent = NULL)
    : CDialog (IDD_DIALOG_STEREO, theParent) {}

  void SetView (const Handle(V3d_View)& theView);

protected:

  virtual void DoDataExchange (CDataExchange* theDX);

  void UpdateCamera();

// Implementation
protected:

  afx_msg void OnCheckFocus();
  afx_msg void OnCheckIOD();
  afx_msg void OnChangeFocus();
  afx_msg void OnChangeIOD();
  afx_msg void OnSpinFocus (NMHDR* theNMHDR, LRESULT* theResult);
  afx_msg void OnSpinIOD (NMHDR* theNMHDR, LRESULT* theResult);
  afx_msg void OnHScroll(UINT theSBCode, UINT thePos, CScrollBar* theScrollBar);

  DECLARE_MESSAGE_MAP()

private:

  Standard_Real myIOD;
  Standard_Real myFocus;
  Standard_Integer mySliderFocus;
  Standard_Boolean myIsRelativeIOD;
  Standard_Boolean myIsRelativeFocus;
  Handle(V3d_View) myView;
};

#endif
