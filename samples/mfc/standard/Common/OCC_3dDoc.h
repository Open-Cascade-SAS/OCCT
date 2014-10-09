// OCC_3dDoc.h: interface for the OCC_3dDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_3DDOC_H__1F4065AD_39C4_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_3DDOC_H__1F4065AD_39C4_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_3dBaseDoc.h"
#include "ResultDialog.h"
#include <Standard_Macro.hxx>

// Event tracker for 3D views with support of advanced message dialog operations
// and dimensions dialog
class AFX_EXT_CLASS OCC_3dDoc : public OCC_3dBaseDoc
{
public:

  OCC_3dDoc (bool theIsResultDialog = true);
  virtual ~OCC_3dDoc();

public: // Dialog operations

  void SetDialogTitle(TCollection_AsciiString theTitle);
  CString GetDialogText();

  void PocessTextInDialog (CString theTitle, CString theMessage);
  void PocessTextInDialog (CString theTitle, const TCollection_AsciiString& theMessage)
  {
    CString aMessage (theMessage.ToCString());
    PocessTextInDialog (theTitle, aMessage);
  }

  void ClearDialog();
  void AddTextInDialog(TCollection_AsciiString& aMessage);

protected:

  afx_msg void OnObjectAddDimensions();
  DECLARE_MESSAGE_MAP()

protected:

  CResultDialog myCResultDialog;
  CDimensionDlg myDimensionDlg;
};

#endif // !defined(AFX_OCC_3DDOC_H__1F4065AD_39C4_11D7_8611_0060B0EE281E__INCLUDED_)
