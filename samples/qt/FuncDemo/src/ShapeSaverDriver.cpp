// ShapeSaverDriver.cpp: implementation of the ShapeSaverDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "ShapeSaverDriver.h"

#include <Standard_GUID.hxx>
#include <TopoDS_Compound.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

IMPLEMENT_STANDARD_HANDLE(ShapeSaverDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(ShapeSaverDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& ShapeSaverDriver::GetID()
{
    static const Standard_GUID id("6B77A40E-E074-4fe1-AB9B-ECECA506717A");
    return id;
}

// Constructor
ShapeSaverDriver::ShapeSaverDriver()
{

}

// Execution.
Standard_Integer ShapeSaverDriver::Execute(Handle(TFunction_Logbook)& log) const
{
    // Usual check...
    if (Label().IsNull())
        return 1;

    // A compound of results of all functions
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound(C);

    // Take results of all functions
    TDF_ChildIterator itr(Label().FindChild(1), false);
    for (; itr.More(); itr.Next())
    {
        Handle(TDF_Reference) ref;
        if (itr.Value().FindAttribute(TDF_Reference::GetID(), ref))
        {
            Handle(TNaming_NamedShape) n;
            if (ref->Get().FindAttribute(TNaming_NamedShape::GetID(), n) && !n->IsEmpty())
            {
                B.Add(C, n->Get());
            }
        }
    }

    //BRepTools::Write(C, "result.brep");

    TNaming_Builder Bui(Label());
    Bui.Generated(C);

    return BaseDriver::Execute(log);
}