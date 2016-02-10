// PrismDriver.cpp: implementation of the PrismDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "PrismDriver.h"

#include <Standard_GUID.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Real.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_HANDLE(PrismDriver,BaseDriver)
IMPLEMENT_STANDARD_RTTIEXT(PrismDriver,BaseDriver)

// ID of the function driver
const Standard_GUID& PrismDriver::GetID()
{
    static const Standard_GUID id("D017489C-EFCE-4e57-8FE9-FA3DEF7DACA9");
    return id;
}

// Constructor
PrismDriver::PrismDriver()
{

}

// Execution.
Standard_Integer PrismDriver::Execute(Handle(TFunction_Logbook)& log) const
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

    // Take the arguments (circular face)
    Handle(TDF_Reference) ref;
    TDF_Label Lcircle = Label().FindChild(1).FindChild(1);
    if (!Lcircle.FindAttribute(TDF_Reference::GetID(), ref))
        return 4;
    Handle(TNaming_NamedShape) n;
    if (!ref->Get().FindAttribute(TNaming_NamedShape::GetID(), n) || n->IsEmpty())
        return 5;
    TopoDS_Face F = TopoDS::Face(n->Get());

    // Make the result
    BRepPrimAPI_MakePrism mkPrism(F, gp_Vec(height * gp::DZ()));
    if (!mkPrism.IsDone())
        return 6;
    TopoDS_Shape P = mkPrism.Shape();
    TopoDS_Shape Top = mkPrism.LastShape();

    // Set the result
    TNaming_Builder B(Label());
    B.Generated(P);
    TNaming_Builder B2(Label().FindChild(3));
    B2.Generated(Top);

    return BaseDriver::Execute(log);
}