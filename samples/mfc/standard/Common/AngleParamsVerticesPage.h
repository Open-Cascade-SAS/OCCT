#pragma once

#include "res\OCC_Resource.h"

// CAngleParamsVerticesPage dialog

class CAngleParamsVerticesPage : public CDialog
{
  DECLARE_DYNAMIC(CAngleParamsVerticesPage)
private:
  Handle(AIS_InteractiveContext) myAISContext;
  TopoDS_Vertex myFirstVertex;
  TopoDS_Vertex mySecondVertex;
  TopoDS_Vertex myThirdVertex;
public:
  CAngleParamsVerticesPage(Handle (AIS_InteractiveContext) theAISContext,
                                   CWnd* pParent = NULL);   // standard constructor
  virtual ~CAngleParamsVerticesPage();

// Dialog Data
  enum { IDD = IDD_AngleParamsVerticesPage };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedVertex1Btn();
  afx_msg void OnBnClickedVertex2Btn();
  afx_msg void OnBnClickedVertex3Btn();
};
