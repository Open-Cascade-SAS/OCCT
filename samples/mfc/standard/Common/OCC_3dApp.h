// OCC_3dApp.h: interface for the OCC_3dApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_3DAPP_H__FC7278BF_390D_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_3DAPP_H__FC7278BF_390D_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseApp.h"
#include <Standard_Macro.hxx>
#include <Handle_Graphic3d_WNTGraphicDevice.hxx>

class Standard_EXPORT OCC_3dApp : public OCC_BaseApp  
{
public:
	OCC_3dApp();
	virtual ~OCC_3dApp();

	Handle_Graphic3d_WNTGraphicDevice GetGraphicDevice() const { return myGraphicDevice; } ;

protected :
     Handle_Graphic3d_WNTGraphicDevice myGraphicDevice;
};

#endif // !defined(AFX_OCC_3DAPP_H__FC7278BF_390D_11D7_8611_0060B0EE281E__INCLUDED_)
