// CircleDriver.cpp: implementation of the CircleDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "CircleDriver.h"

#include <Standard_GUID.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Real.hxx>

#include <TopoDS.hxx>
#include <gp_Circ.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

IMPLEMENT_STANDARD_HANDLE(CircleDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(CircleDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& CircleDriver::GetID()
{
    static const Standard_GUID id("D10515D5-7C4E-4fe3-A7E2-DE2E01859B4D");
    return id;
}

// Constructor
CircleDriver::CircleDriver()
{

}

// Execution.
Standard_Integer CircleDriver::Execute(Handle(TFunction_Logbook)& log) const
{
    // Usual check...
    if (Label().IsNull())
        return 1;

    // Take the arguments (radius)
    Handle(TDataStd_Real) r;
    if (!Label().FindAttribute(TDataStd_Real::GetID(), r))
        return 2;
    double radius = r->Get();
    if (radius < Precision::Confusion())
        return 3;

    // Take the arguments (center point)
    Handle(TDF_Reference) ref;
    TDF_Label Lpoint = Label().FindChild(1).FindChild(1);
    if (!Lpoint.FindAttribute(TDF_Reference::GetID(), ref))
        return 4;
    Handle(TNaming_NamedShape) n;
    if (!ref->Get().FindAttribute(TNaming_NamedShape::GetID(), n) || n->IsEmpty())
        return 5;
    TopoDS_Vertex V = TopoDS::Vertex(n->Get());

    // Make the result
    gp_Circ C(gp_Ax2(BRep_Tool::Pnt(V), gp::DZ()), radius);
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(C);
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
    TopoDS_Face F = BRepBuilderAPI_MakeFace(W);

    // Set the result
    TNaming_Builder B(Label());
    B.Generated(F);

    return BaseDriver::Execute(log);
}