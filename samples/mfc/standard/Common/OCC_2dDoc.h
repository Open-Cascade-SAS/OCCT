// OCC_2dDoc.h: interface for the OCC_2dDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseDoc.h"
#include "ResultDialog.h"
#include <WNT_Window.hxx>
#include <WNT_WDriver.hxx>
#include <AIS2D_InteractiveContext.hxx>

class AFX_EXT_CLASS OCC_2dDoc : public OCC_BaseDoc 
{
	DECLARE_DYNCREATE(OCC_2dDoc)
public:
	OCC_2dDoc();
	virtual ~OCC_2dDoc();

// Operations
public:
	Handle_V2d_Viewer GetViewer2D () { return my2DViewer; };
	void FitAll2DViews(Standard_Boolean UpdateViewer=Standard_False);
	void UpdateResultDialog(UINT anID,TCollection_AsciiString aMessage);
	void UpdateResultDialog(CString& title,TCollection_AsciiString aMessage);
	Handle(AIS2D_InteractiveContext) GetInteractiveContext2D() {return myAISInteractiveContext2D;}; 
// Attributes
protected:
	Handle_V2d_Viewer my2DViewer;
	Handle_AIS2D_InteractiveContext myAISInteractiveContext2D;
	CResultDialog myCResultDialog;
};

#endif // !defined(AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
