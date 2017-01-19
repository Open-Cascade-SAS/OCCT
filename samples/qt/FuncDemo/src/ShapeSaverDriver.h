// ShapeSaverDriver.h: interface for the ShapeSaver function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SHAPESAVERDRIVER_H_)
#define _SHAPESAVERDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDriver.h"

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE(ShapeSaverDriver, BaseDriver)

// A ShapeSaver function driver.
class ShapeSaverDriver : public BaseDriver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	ShapeSaverDriver();

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(ShapeSaverDriver, BaseDriver)
};

#endif // !defined(_SHAPESAVERDRIVER_H_)
