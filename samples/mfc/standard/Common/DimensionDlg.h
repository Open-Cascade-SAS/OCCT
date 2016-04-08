#pragma once

// DimensionDlg dialog

#include <stdafx.h>

#include "res\OCC_Resource.h"
#include <Standard_Macro.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <Quantity_Color.hxx>

class CDimensionDlg : public CDialog
{
public:
  /// Construction & termination
  CDimensionDlg (CWnd* pParent = NULL);  // standard constructor
  CDimensionDlg (Handle(AIS_InteractiveContext) &theAISContext,
                 CWnd* pParent = NULL);
  virtual ~CDimensionDlg();

  // Methods for data operation
  void SetContext (const Handle(AIS_InteractiveContext) theContext);
  void SetTextModeControlsVisible (bool isVisible);
  void UpdateUnitsListForLength();
  void UpdateUnitsListForAngle();
  void Empty();
  void DeactivateAllStandardModes();
  void UpdateStandardModeForAngle ();
  void UpdateStandardModeForLength ();
  void UpdateStandardMode ();
  const Standard_Real GetFlyout () const;
  const Standard_Boolean GetTextType() const;
  const Standard_Real GetFontHeight() const;
  const Standard_Boolean IsText3dShaded() const;
  const Standard_Boolean IsUnitsDisplayed() const;
  const TCollection_AsciiString GetUnits() const;
  const Quantity_Color GetDimensionColor() const;

public:
  // Dialog Data
  enum { IDD = IDD_Dimension };
  // Initialization of dialog
protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

  //Attributes
private:
  Handle (AIS_InteractiveContext) myAISContext;
  int mySelectedDimType;
  int myFontSize;
  Quantity_Color myDimensionColor;
  CTabCtrl *myLengthParams;
  CTabCtrl *myAngleParams;
  CTabCtrl *myRadiusParams;
  CTabCtrl *myDiameterParams;

  void CreateLengthParamsTab();
  void CreateAngleParamsTab();
  void CreateRadiusParamsTab();
  void CreateDiameterParamsTab();

public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedDimLength();
  afx_msg void OnBnClickedDimAngle();
  afx_msg void OnBnClickedDimRadius();
  afx_msg void OnTcnSelChangeLengthTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTcnSelChangingLengthTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDestroy();
  afx_msg void OnTcnSelChangeAngleTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnTcnSelChangingAngleTab(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedDimDiameter();
  afx_msg void OnBnClicked2dText();
  afx_msg void OnBnClicked3dText();
  afx_msg void OnBnClickedDimensionColor();
  afx_msg void OnClose();
};
