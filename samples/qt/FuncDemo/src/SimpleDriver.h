// SimpleDriver.h: interface for the Simple function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SIMPLEDRIVER_H_)
#define _SIMPLEDRIVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_LabelList.hxx>

DEFINE_STANDARD_HANDLE(SimpleDriver, TFunction_Driver)

// A function driver for a Simple function.
class SimpleDriver : public TFunction_Driver
{
public:

    // ID of the function driver
    static const Standard_GUID& GetID();
    
    // Constructor
	SimpleDriver();

	// Returns the arguments of the function
	virtual void Arguments(TDF_LabelList& args) const;

	// Returns the results of the function
	virtual void Results(TDF_LabelList& res) const;

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(SimpleDriver, TFunction_Driver)
};

#endif // !defined(_SIMPLEDRIVER_H_)
