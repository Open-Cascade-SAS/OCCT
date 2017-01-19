// CylinderDriver.h: interface for the Cylinder function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CYLINDERDRIVER_H_)
#define _CYLINDERDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(CylinderDriver, BaseDriver)

// A Cylinder function driver.
class CylinderDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	CylinderDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(CylinderDriver, BaseDriver)
};

#endif // !defined(_CYLINDERDRIVER_H_)
