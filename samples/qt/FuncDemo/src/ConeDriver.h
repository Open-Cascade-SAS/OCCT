// ConeDriver.h: interface for the Cone function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CONEDRIVER_H_)
#define _CONEDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(ConeDriver, BaseDriver)

// A Cone function driver.
class ConeDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	ConeDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(ConeDriver, BaseDriver)
};

#endif // !defined(_CONEDRIVER_H_)
