#pragma once

#include "res\OCC_Resource.h"
#include <Standard_Macro.hxx>
#include <AIS_InteractiveContext.hxx>

// CLengthParamsEdgePage dialog

class CLengthParamsEdgePage : public CDialog
{
  DECLARE_DYNAMIC(CLengthParamsEdgePage)

public:
  CLengthParamsEdgePage(Handle(AIS_InteractiveContext) theAISContext,CWnd* pParent = NULL);   // standard constructor
  virtual ~CLengthParamsEdgePage();
  CButton* GetButton();

// Dialog Data
  enum { IDD = IDD_LengthParamsEdgePage };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
private:
  Handle(AIS_InteractiveContext) myAISContext;
public:
  afx_msg void OnBnClickedChooseEdgeBtn();
};
