// CylinderDriver.cpp: implementation of the CylinderDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "CylinderDriver.h"

#include <Standard_GUID.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_RealArray.hxx>

#include <TopoDS.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <Precision.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

IMPLEMENT_STANDARD_HANDLE(CylinderDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(CylinderDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& CylinderDriver::GetID()
{
    static const Standard_GUID id("9CE1FCDA-C8EA-4b09-B1C4-021FCD44F3F3");
    return id;
}

// Constructor
CylinderDriver::CylinderDriver()
{

}

// Execution.
Standard_Integer CylinderDriver::Execute(Handle(TFunction_Logbook)& log) const
{
    // Usual check...
    if (Label().IsNull())
        return 1;

    // Take the arguments (radius, angle, height)
    Handle(TDataStd_RealArray) arr;
    if (!Label().FindAttribute(TDataStd_RealArray::GetID(), arr))
        return 2;
    double radius = arr->Value(1);
    double angle = arr->Value(2);
    double height = arr->Value(3);
    if (radius < Precision::Confusion() || height < Precision::Confusion())
        return 3;

    // Take the arguments (top face)
    Handle(TDF_Reference) ref;
    if (!Label().FindChild(1).FindChild(1).FindAttribute(TDF_Reference::GetID(), ref))
        return 4;
    Handle(TNaming_NamedShape) n;
    if (!ref->Get().FindAttribute(TNaming_NamedShape::GetID(), n) || n->IsEmpty())
        return 5;
    TopoDS_Face F = TopoDS::Face(n->Get());

    // Take the circle
    TopExp_Explorer expl(F, TopAbs_EDGE);
    if (!expl.More())
        return 6;
    TopoDS_Edge E = TopoDS::Edge(expl.Current());
    BRepAdaptor_Curve A(E);
    gp_Circ C = A.Circle();

    // Center of the cylinder
    gp_Pnt p = ElCLib::Value(angle * M_PI / 180.0, C);
    gp_Vec v(p, C.Location());
    v.Normalize();
    p.Translate(radius * v);

    // Make the result
    BRepPrimAPI_MakeCylinder mkCylinder(gp_Ax2(p, gp::DZ()), radius, height);
    mkCylinder.Build();
    if (!mkCylinder.IsDone())
        return 7;
    TopoDS_Shape Cyl = mkCylinder.Shape();

    // Make the top face of the cylinder for next functions
    gp_Circ Ctop(gp_Ax2(p.Translated(height * gp::DZ()), gp::DZ()), radius);
    TopoDS_Edge Etop = BRepBuilderAPI_MakeEdge(Ctop);
    TopoDS_Wire Wtop = BRepBuilderAPI_MakeWire(Etop);
    TopoDS_Face Ftop = BRepBuilderAPI_MakeFace(Wtop);

    // Set the result
    TNaming_Builder B(Label());
    B.Generated(Cyl);
    TNaming_Builder B2(Label().FindChild(3));
    B2.Generated(Ftop);

    return BaseDriver::Execute(log);
}