// BaseDriver.cpp: implementation of the BaseDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseDriver.h"

#include <TDF_Reference.hxx>
#include <TDF_ChildIterator.hxx>

#define SLOW
#ifdef SLOW
#include <OSD_Timer.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#endif

IMPLEMENT_STANDARD_HANDLE(BaseDriver,TFunction_Driver)
IMPLEMENT_STANDARD_RTTIEXT(BaseDriver,TFunction_Driver)

// Constructor
BaseDriver::BaseDriver()
{

}

// Returns the arguments of the function
void BaseDriver::Arguments(TDF_LabelList& args) const
{
    // Append all arguments.
    TDF_ChildIterator itr(Label().FindChild(1), false);
    for (; itr.More(); itr.Next())
    {
        Handle(TDF_Reference) ref;
        if (itr.Value().FindAttribute(TDF_Reference::GetID(), ref))
            args.Append(ref->Get());
    }
}


// Returns the results of the function
void BaseDriver::Results(TDF_LabelList& res) const
{
    // Append all results
    TDF_ChildIterator itr(Label().FindChild(2), false);
    for (; itr.More(); itr.Next())
    {
        Handle(TDF_Reference) ref;
        if (itr.Value().FindAttribute(TDF_Reference::GetID(), ref))
            res.Append(ref->Get());
    }
}

// Execution.
Standard_Integer BaseDriver::Execute(Handle(TFunction_Logbook)& ) const
{
#ifdef SLOW
    // Make a boolean operation to slow down the function
    TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(100.0, 2.0 * M_PI / 3.0);
    TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(gp_Pnt(10, 10, 10), 100.0, 2.0 * M_PI / 3.0);
    BRepAlgoAPI_Fuse fuser(S1, S2);
    fuser.Build();
#endif

    // Empty... should be implemented in descendent classes
    return 0;
}