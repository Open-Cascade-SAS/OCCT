// PointDriver.h: interface for the Point function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PointDRIVER_H_)
#define _PointDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(PointDriver, BaseDriver)

// A Point function driver.
class PointDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	PointDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(PointDriver, BaseDriver)
};

#endif // !defined(_PointDRIVER_H_)
