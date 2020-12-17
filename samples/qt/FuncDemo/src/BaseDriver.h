// BaseDriver.h: interface for the Base function driver.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BASEDRIVER_H_)
#define _BASEDRIVER_H_

#include <Standard_DefineHandle.hxx> 
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_LabelList.hxx>
#include <Standard_Mutex.hxx>

DEFINE_STANDARD_HANDLE(BaseDriver, TFunction_Driver)

// A base function driver.
class BaseDriver : public TFunction_Driver
{
public:
    
    // Constructor
	BaseDriver();

	// Returns the arguments of the function
	virtual void Arguments(TDF_LabelList& args) const;

	// Returns the results of the function
	virtual void Results(TDF_LabelList& res) const;

    // Sets a mutex for execution of the driver.
    void SetMutex(Standard_Mutex* pmutex);

	// Execution.
	virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;

	DEFINE_STANDARD_RTTIEXT(BaseDriver, TFunction_Driver)

protected:
    Standard_Mutex* myMutex;
};

#endif // !defined(_BASEDRIVER_H_)
