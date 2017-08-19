// SimpleDriver.cpp: implementation of the SimpleDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "SimpleDriver.h"

#include <TDF_Reference.hxx>
#include <TDF_ChildIterator.hxx>
#include <Standard_GUID.hxx>
#include <OSD_Timer.hxx>
#include <TDataStd_Real.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

IMPLEMENT_STANDARD_HANDLE(SimpleDriver,  TFunction_Driver)
IMPLEMENT_STANDARD_RTTIEXT(SimpleDriver, TFunction_Driver)

// ID of the function driver
const Standard_GUID& SimpleDriver::GetID()
{
    static const Standard_GUID id("4534840D-6DCD-440f-9F0E-BDEF1C50D434");
    return id;
}

// Constructor
SimpleDriver::SimpleDriver()
{

}

// Returns the arguments of the function
void SimpleDriver::Arguments(TDF_LabelList& args) const
{
    // A double - relative time of execution
    args.Append(Label());

    // References to other functions through TDF_Reference
    TDF_ChildIterator itr(Label().FindChild(1), false);
    for (; itr.More(); itr.Next())
    {
        Handle(TDF_Reference) ref;
        if (itr.Value().FindAttribute(TDF_Reference::GetID(), ref))
            args.Append(ref->Get());
    }
}


// Returns the results of the function
void SimpleDriver::Results(TDF_LabelList& res) const
{
    // References to other functions through TDF_Reference
    res.Append(Label());
    TDF_ChildIterator itr(Label().FindChild(2), false);
    for (; itr.More(); itr.Next())
    {
        Handle(TDF_Reference) ref;
        if (itr.Value().FindAttribute(TDF_Reference::GetID(), ref))
            res.Append(ref->Get());
    }
}

// Execution.
Standard_Integer SimpleDriver::Execute(Handle(TFunction_Logbook)& ) const
{
	// Check initialization
	if (Label().IsNull())
		return 1;

    // Take the double argument
    Handle(TDataStd_Real) time_keeper;
    if (!Label().FindAttribute(TDataStd_Real::GetID(), time_keeper))
        return 2;
    double times = time_keeper->Get();

    // Make a sphere 10000 * "times" times (it takes about a second on a simple computer).
    int i = 0;
    while (++i < 10000 * times)
    {
        // Call any fucntions taking much time.
        // It is necessary to "see" the execution of a function in real time.
        BRepPrimAPI_MakeSphere mkSphere(100.0);
        mkSphere.Build();
    }

    return 0;
}