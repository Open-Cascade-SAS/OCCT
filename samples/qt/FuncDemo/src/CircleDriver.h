// CircleDriver.h: interface for the Circle function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_CIRCLEDRIVER_H_)
#define _CIRCLEDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(CircleDriver, BaseDriver)

// A Circle function driver.
class CircleDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	CircleDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(CircleDriver, BaseDriver)
};

#endif // !defined(_CIRCLEDRIVER_H_)
