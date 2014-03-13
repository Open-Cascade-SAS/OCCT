///////////////////////////////////////////////////////////////////////////////
// OCC_StereoConfigDlg.cpp : source file
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OCC_StereoConfigDlg.h"
#include <Graphic3d_Camera.hxx>

BEGIN_MESSAGE_MAP (OCC_StereoConfigDlg, CDialog)

  ON_WM_HSCROLL()
  ON_NOTIFY (UDN_DELTAPOS, IDC_SPIN_FOCUS, OnSpinFocus)
  ON_NOTIFY (UDN_DELTAPOS, IDC_SPIN_IOD, OnSpinIOD)
  ON_BN_CLICKED (IDC_CHECK_FOCUS_RELATIVE, OnCheckFocus)
  ON_BN_CLICKED (IDC_CHECK_IOD_RELATIVE, OnCheckIOD)
  ON_EN_CHANGE (IDC_EDIT_FOCUS, OnChangeFocus)
  ON_EN_CHANGE (IDC_EDIT_IOD, OnChangeIOD)

END_MESSAGE_MAP()

// round up value macro
#define ROUND_UP(X) X = (Round(X * 10000.0) / 10000.0)

// slider tick conversion
#define TO_SLIDER(X) (Standard_Integer)Round(X * 10)

// back conversion from slider ticks
#define FROM_SLIDER(X) X / 10.0

// ============================================================================
// function: SetView
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::SetView (const Handle(V3d_View)& theView)
{
  myView = theView;

  // access initial values
  myIOD = myView->Camera()->IOD();
  myFocus = myView->Camera()->ZFocus();
  mySliderFocus = TO_SLIDER(myFocus);
  myIsRelativeIOD   = (myView->Camera()->GetIODType() == Graphic3d_Camera::IODType_Relative);
  myIsRelativeFocus = (myView->Camera()->ZFocusType() == Graphic3d_Camera::FocusType_Relative);
}

// ============================================================================
// function: DoDataExchange
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::DoDataExchange(CDataExchange* theDX)
{
  CDialog::DoDataExchange(theDX);

  DDX_Text (theDX, IDC_EDIT_IOD, myIOD);
  DDV_MinMaxDouble (theDX, myIOD, -DBL_MAX, DBL_MAX);

  if (myIsRelativeFocus)
  {
    // do slider data exchange
    DDX_Slider (theDX, IDC_SLIDER_FOCUS, (int&)mySliderFocus);
    DDV_MinMaxSlider (theDX, mySliderFocus, TO_SLIDER(0.1), TO_SLIDER(10));

    // show up value in edit field
    Standard_Real aEditValue = FROM_SLIDER (mySliderFocus);
    DDX_Text (theDX, IDC_EDIT_FOCUS, aEditValue);

    // update focus value correspondingly
    myFocus = FROM_SLIDER (mySliderFocus);
  }
  else
  {
    DDX_Text (theDX, IDC_EDIT_FOCUS, myFocus);
    DDV_MinMaxDouble (theDX, myFocus, 50, DBL_MAX);

    mySliderFocus = TO_SLIDER(1.0);
    DDX_Slider (theDX, IDC_SLIDER_FOCUS, (int&)mySliderFocus);
  }

  DDX_Check (theDX, IDC_CHECK_FOCUS_RELATIVE, (int&)myIsRelativeFocus);
  DDX_Check (theDX, IDC_CHECK_IOD_RELATIVE, (int&)myIsRelativeIOD);

  CEdit* aFocusEdit = (CEdit*) GetDlgItem (IDC_EDIT_FOCUS);
  aFocusEdit->EnableWindow (myIsRelativeFocus != Standard_True);

  CSliderCtrl* aSlider = (CSliderCtrl*) GetDlgItem (IDC_SLIDER_FOCUS);
  aSlider->EnableWindow (myIsRelativeFocus == Standard_True);
}

// ============================================================================
// function: OnHScroll
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnHScroll(UINT theSBCode, UINT thePos, CScrollBar* theScrollBar)
{
  UpdateData (true);
  UpdateData (false);
  UpdateCamera();
  CWnd::OnHScroll(theSBCode, thePos, theScrollBar);
}

// ============================================================================
// function: UpdateCamera
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::UpdateCamera()
{
  // update camera properties and redraw view
  const Handle(Graphic3d_Camera)& aCamera = myView->Camera();
  if (aCamera.IsNull())
  {
    return;
  }

  // change IOD
  Graphic3d_Camera::IODType aIODType = 
    myIsRelativeIOD ? Graphic3d_Camera::IODType_Relative :
                      Graphic3d_Camera::IODType_Absolute;

  aCamera->SetIOD (aIODType, myIOD);

  // change Focus
  Graphic3d_Camera::FocusType aFocusType = 
    myIsRelativeFocus ? Graphic3d_Camera::FocusType_Relative :
                        Graphic3d_Camera::FocusType_Absolute;

  aCamera->SetZFocus (aFocusType, myFocus);

  // redraw view
  myView->Redraw();
}

// ============================================================================
// function: OnCheckFocus
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnCheckFocus()
{
  UpdateData (true);

  // change focus to some predefined values
  if (myIsRelativeFocus)
    myFocus = 1.0;
  else
    myFocus = 100.0;

  UpdateData (false);
  UpdateCamera();
}

// ============================================================================
// function: OnCheckIOD
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnCheckIOD()
{
  UpdateData (true);
  UpdateCamera();
}

// ============================================================================
// function: OnChangeFocus
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnChangeFocus()
{
  // keep previous value
  Standard_Real aPrevFocus = myFocus;

  // read data from ui controls
  if (UpdateData (true))
  {
    UpdateCamera();
  }
  else
  {
    // revert back
    myFocus = aPrevFocus;
    UpdateData (false);
  }
}

// ============================================================================
// function: OnChangeIOD
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnChangeIOD()
{
  // keep previous value
  Standard_Real aPrevIOD = myIOD;

  // read data from ui controls
  if (UpdateData (true))
  {
    UpdateCamera();
  }
  else
  {
    // revert back
    myIOD = aPrevIOD;
    UpdateData (false);
  }
}

// ============================================================================
// function: OnSpinFocus
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnSpinFocus (NMHDR* theNMHDR, LRESULT* theResult)
{
  NM_UPDOWN* aNMUpDown = (NM_UPDOWN*)theNMHDR;

  const double aStep = 0.1; // use small incremental step
  const double aDelta = aNMUpDown->iDelta * aStep; // get delta

  // changes value
  myFocus -= (Standard_Real)aDelta;

  // round up value
  ROUND_UP (myFocus);

  // actualize view & ui controls
  UpdateData (false);
  UpdateCamera();

  *theResult = 0;
}

// ============================================================================
// function: OnSpinIOD
// purpose:
// ============================================================================
void OCC_StereoConfigDlg::OnSpinIOD (NMHDR* theNMHDR, LRESULT* theResult)
{
  NM_UPDOWN* aNMUpDown = (NM_UPDOWN*)theNMHDR;

  const double aStep = 0.01; // use small incremental step
  const double aDelta = aNMUpDown->iDelta * aStep; // get delta

  // changes value
  myIOD -= (Standard_Real)aDelta;

  // round up value
  ROUND_UP (myIOD);

  // actualize view & ui controls
  UpdateData (false);
  UpdateCamera();

  *theResult = 0;
}
