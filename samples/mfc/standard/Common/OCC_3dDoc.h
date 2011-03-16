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

class AFX_EXT_CLASS OCC_3dDoc : public OCC_3dBaseDoc  
{
public:
	void SetDialogTitle(TCollection_AsciiString theTitle);
	CString GetDialogText();
	OCC_3dDoc();
	virtual ~OCC_3dDoc();

	void PocessTextInDialog(char* aTitle, TCollection_AsciiString& aMessage);
	void ClearDialog();
	void AddTextInDialog(TCollection_AsciiString& aMessage);


protected:
	CResultDialog myCResultDialog;
};

#endif // !defined(AFX_OCC_3DDOC_H__1F4065AD_39C4_11D7_8611_0060B0EE281E__INCLUDED_)
