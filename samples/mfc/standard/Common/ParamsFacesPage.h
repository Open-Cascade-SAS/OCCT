#pragma once

#include "res\OCC_Resource.h"
// CParamsFacesPage dialog

class CParamsFacesPage : public CDialog
{
  DECLARE_DYNAMIC(CParamsFacesPage)
private:
  Handle(AIS_InteractiveContext) myAISContext;
  bool myIsAngleDimension;
  TopoDS_Face myFirstFace;
  TopoDS_Face mySecondFace;
public:
  CParamsFacesPage (Handle(AIS_InteractiveContext) theAISContext,
                    bool isAngleDimension = false,
                    CWnd* pParent = NULL);    // standard constructor

  virtual ~CParamsFacesPage();

  // Dialog Data
  enum { IDD = IDD_ParamsFacesPage };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedFacesbtn1();
  afx_msg void OnBnClickedFacesbtn2();
};
