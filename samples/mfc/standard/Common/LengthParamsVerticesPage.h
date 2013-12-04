#pragma once

#include "res\OCC_Resource.h"

// CLengthParamsVerticesPage dialog

class CLengthParamsVerticesPage : public CDialog
{
  DECLARE_DYNAMIC(CLengthParamsVerticesPage)

public:
  CLengthParamsVerticesPage(Handle(AIS_InteractiveContext) theAISContext,CWnd* pParent = NULL);   // standard constructor
  virtual ~CLengthParamsVerticesPage();
  const TopoDS_Vertex& getFirstVertex() const;
  const TopoDS_Vertex& getSecondVertex() const;
// Dialog Data
  enum { IDD = IDD_LengthParamsVerticesPage };
private:
  Handle(AIS_InteractiveContext) myAISContext;
  TopoDS_Vertex myFirstVertex;
  TopoDS_Vertex mySecondVertex;
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedVertex1Btn();
  afx_msg void OnBnClickedVertex2Btn();
};
