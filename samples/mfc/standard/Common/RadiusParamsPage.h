#pragma once

#include "res\OCC_Resource.h"
// CRadiusParamsPage dialog

class CRadiusParamsPage : public CDialog
{
  DECLARE_DYNAMIC(CRadiusParamsPage)
private:
  Handle(AIS_InteractiveContext) myAISContext;
  Standard_Boolean myIsDiameterDimension;
public:
  CRadiusParamsPage(const Handle(AIS_InteractiveContext)& theAISContext,
                    const Standard_Boolean isDiameterDimension = Standard_False,
                    CWnd* pParent = NULL);   // standard constructor
  virtual ~CRadiusParamsPage();

// Dialog Data
  enum { IDD = IDD_RadiusParamsPage };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedObjectBtn();
};
