// PrismDriver.h: interface for the Prism function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PRISMDRIVER_H_)
#define _PRISMDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(PrismDriver, BaseDriver)

// A Prism function driver.
class PrismDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	PrismDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(PrismDriver, BaseDriver)
};

#endif // !defined(_PRISMDRIVER_H_)
