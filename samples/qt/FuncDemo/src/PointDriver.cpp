// PointDriver.cpp: implementation of the PointDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "PointDriver.h"

#include <Standard_GUID.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TDataStd_RealArray.hxx>

#include <gp_Pnt.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

IMPLEMENT_STANDARD_HANDLE(PointDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(PointDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& PointDriver::GetID()
{
    static const Standard_GUID id("E9467D43-B11D-42d3-AF10-E91B74D2A3D9");
    return id;
}

// Constructor
PointDriver::PointDriver()
{

}

// Execution.
Standard_Integer PointDriver::Execute(Handle(TFunction_Logbook)& log) const
{
    // Usual check...
    if (Label().IsNull())
        return 1;

    // Take the arguments (x, y, z)
    Handle(TDataStd_RealArray) arr;
    if (!Label().FindAttribute(TDataStd_RealArray::GetID(), arr))
        return 2;
    double x = arr->Value(1);
    double y = arr->Value(2);
    double z = arr->Value(3);

    // Make the result
    TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(gp_Pnt(x, y, z));

    // Set the result
    TNaming_Builder B(Label());
    B.Generated(V);

    return BaseDriver::Execute(log);
}