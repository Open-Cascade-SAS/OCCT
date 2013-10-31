#pragma once

#include "res\OCC_Resource.h"

// CLenghtParamsEdgesPage dialog

class CLengthParamsEdgesPage : public CDialog
{
private:
  Handle(AIS_InteractiveContext) myAISContext;
  bool myIsAngleDimension;
  TopoDS_Edge myFirstEdge;
  TopoDS_Edge mySecondEdge;
public:
  CLengthParamsEdgesPage (Handle(AIS_InteractiveContext) theAISContext,
                          bool isAngleDimension = false,
                          CWnd* pParent = NULL);   // standard constructor
  virtual ~CLengthParamsEdgesPage();

// Dialog Data
  enum { IDD = IDD_LengthParamsEdgesPage };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedEdge1Btn();
  afx_msg void OnBnClickedEdge2Btn();
};
