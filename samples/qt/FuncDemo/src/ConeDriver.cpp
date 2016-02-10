// ConeDriver.cpp: implementation of the ConeDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "ConeDriver.h"

#include <Standard_GUID.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Real.hxx>

#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

IMPLEMENT_STANDARD_HANDLE(ConeDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(ConeDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& ConeDriver::GetID()
{
    static const Standard_GUID id("73C5C048-59EC-404a-850B-08363D75E63C");
    return id;
}

// Constructor
ConeDriver::ConeDriver()
{

}

// Execution.
Standard_Integer ConeDriver::Execute(Handle(TFunction_Logbook)& log) const
{
    // Usual check...
    if (Label().IsNull())
        return 1;

    // Take the arguments (height)
    Handle(TDataStd_Real) h;
    if (!Label().FindAttribute(TDataStd_Real::GetID(), h))
        return 2;
    double height = h->Get();
    if (height < Precision::Confusion())
        return 3;

    // Take the arguments (top faces)
    Handle(TDF_Reference) ref1, ref2, ref3, ref4;
    if (!Label().FindChild(1).FindChild(1).FindAttribute(TDF_Reference::GetID(), ref1))
        return 4;
    if (!Label().FindChild(1).FindChild(2).FindAttribute(TDF_Reference::GetID(), ref2))
        return 5;
    if (!Label().FindChild(1).FindChild(3).FindAttribute(TDF_Reference::GetID(), ref3))
        return 6;
    if (!Label().FindChild(1).FindChild(4).FindAttribute(TDF_Reference::GetID(), ref4))
        return 7;
    Handle(TNaming_NamedShape) n1, n2, n3, n4;
    if (!ref1->Get().FindAttribute(TNaming_NamedShape::GetID(), n1) || n1->IsEmpty())
        return 8;
    if (!ref2->Get().FindAttribute(TNaming_NamedShape::GetID(), n2) || n2->IsEmpty())
        return 9;
    if (!ref3->Get().FindAttribute(TNaming_NamedShape::GetID(), n3) || n3->IsEmpty())
        return 10;
    if (!ref4->Get().FindAttribute(TNaming_NamedShape::GetID(), n4) || n4->IsEmpty())
        return 11;
    TopoDS_Face F1 = TopoDS::Face(n1->Get());
    TopoDS_Face F2 = TopoDS::Face(n2->Get());
    TopoDS_Face F3 = TopoDS::Face(n3->Get());
    TopoDS_Face F4 = TopoDS::Face(n4->Get());

    // Compute central points
    gp_Pnt p1, p2, p3, p4;
    TopoDS_Edge E1, E2, E3, E4;
    TopExp_Explorer expl(F1, TopAbs_EDGE);
    if (!expl.More())
        return 12;
    E1 = TopoDS::Edge(expl.Current());
    expl.Init(F2, TopAbs_EDGE);
    if (!expl.More())
        return 13;
    E2 = TopoDS::Edge(expl.Current());
    expl.Init(F3, TopAbs_EDGE);
    if (!expl.More())
        return 14;
    E3 = TopoDS::Edge(expl.Current());
    expl.Init(F4, TopAbs_EDGE);
    if (!expl.More())
        return 15;
    E4 = TopoDS::Edge(expl.Current());
    BRepAdaptor_Curve A(E1);
    gp_Circ C1 = A.Circle();
    p1 = C1.Location();
    A.Initialize(E2);
    gp_Circ C2 = A.Circle();
    p2 = C2.Location();
    A.Initialize(E3);
    gp_Circ C3 = A.Circle();
    p3 = C3.Location();
    A.Initialize(E4);
    gp_Circ C4 = A.Circle();
    p4 = C4.Location();

    // Center of cone
    gp_Pnt p(0.25*(p1.X()+p2.X()+p3.X()+p4.X()),
             0.25*(p1.Y()+p2.Y()+p3.Y()+p4.Y()),
             0.25*(p1.Z()+p2.Z()+p3.Z()+p4.Z()));

    // Bottom radius (max distance to points from center)
    double bRadius = 0.0;
    if (p1.Distance(p) > bRadius)
        bRadius = p1.Distance(p);
    if (p2.Distance(p) > bRadius)
        bRadius = p2.Distance(p);
    if (p3.Distance(p) > bRadius)
        bRadius = p3.Distance(p);
    if (p4.Distance(p) > bRadius)
        bRadius = p4.Distance(p);
    bRadius *= 1.5;

    // Top radius
    double tRadius = bRadius / 2.0;

    // Make the result
    BRepPrimAPI_MakeCone mkCone(gp_Ax2(p, gp::DZ()), bRadius, tRadius, height);
    mkCone.Build();
    if (!mkCone.IsDone())
        return 16;
    TopoDS_Shape C = mkCone.Shape();

    // Make another result: a circle for next functions (many cylinders)
    p.Translate(height * gp::DZ());
    gp_Circ TC(gp_Ax2(p, gp::DZ()), 0.8 * tRadius);
    TopoDS_Edge E = BRepBuilderAPI_MakeEdge(TC);
    TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
    TopoDS_Face F = BRepBuilderAPI_MakeFace(W);

    // Set the result
    TNaming_Builder B(Label());
    B.Generated(C);
    TNaming_Builder B2(Label().FindChild(3));
    B2.Generated(F);

    return BaseDriver::Execute(log);
}