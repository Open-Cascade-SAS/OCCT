// OCC_2dApp.h: interface for the OCC_2dApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2dAPP_H__425A644A_38ED_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2dAPP_H__425A644A_38ED_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseApp.h"

#include <WNT_GraphicDevice.hxx>

class Standard_EXPORT OCC_2dApp : public OCC_BaseApp  
{
public:
	OCC_2dApp();

	Handle_WNT_GraphicDevice GetGraphicDevice() const { return myGraphicDevice; } ;

private :
    Handle_WNT_GraphicDevice myGraphicDevice;

};

#endif // !defined(AFX_OCC_2dAPP_H__425A644A_38ED_11D7_8611_0060B0EE281E__INCLUDED_)
