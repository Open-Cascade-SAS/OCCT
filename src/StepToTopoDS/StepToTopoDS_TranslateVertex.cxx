// File:	StepToTopoDS_TranslateVertex.cxx
// Created:	Tue Jan  3 15:00:17 1995
// Author:	Frederic MAUPAS
//		<fma@stylox>
//:   gka 09.04.99: S4136: eliminate BRepAPI::Precision()

#include <StepToTopoDS_TranslateVertex.ixx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <TopoDS.hxx>

#include <TopoDS_Vertex.hxx>
#include <BRep_Builder.hxx>
//#include <BRepAPI.hxx>

#include <StepGeom_Point.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepShape_Vertex.hxx>

#include <Geom_CartesianPoint.hxx>
#include <Precision.hxx>

// For I-DEAS-like processing (ssv; 15.11.2010)
#include <TCollection_HAsciiString.hxx>

// ============================================================================
// Method  : StepToTopoDS_TranslateVertex::StepToTopoDS_TranslateVertex
// Purpose : Empty Constructor
// ============================================================================

StepToTopoDS_TranslateVertex::StepToTopoDS_TranslateVertex()
{
  done = Standard_False;
}

// ============================================================================
// Method  : StepToTopoDS_TranslateVertex::StepToTopoDS_TranslateVertex
// Purpose : Constructor with a Vertex and a Tool
// ============================================================================

StepToTopoDS_TranslateVertex::StepToTopoDS_TranslateVertex(const Handle(StepShape_Vertex)& V, 
                                                           StepToTopoDS_Tool& T,
                                                           StepToTopoDS_NMTool& NMTool)
{
  Init(V, T, NMTool);
}

// ============================================================================
// Method  : Init
// Purpose : Init with a Vertex and a Tool
// ============================================================================

void StepToTopoDS_TranslateVertex::Init(const Handle(StepShape_Vertex)& aVertex, 
				                                StepToTopoDS_Tool& aTool,
                                        StepToTopoDS_NMTool& NMTool)
{
  if (!aTool.IsBound(aVertex)) {

    // [BEGIN] Proceed with non-manifold topology (ssv; 14.11.2010)
    if ( NMTool.IsActive() && NMTool.IsBound(aVertex) ) {
      myResult = NMTool.Find(aVertex);
      myError  = StepToTopoDS_TranslateVertexDone;
      done     = Standard_True;
      return;
    }
    // [END] Proceed with non-manifold topology (ssv; 14.11.2010)

    // [BEGIN] Proceed with I-DEAS-like STP (ssv; 15.11.2010)
    const Handle(TCollection_HAsciiString) aVName = aVertex->Name();
    if ( NMTool.IsActive() && NMTool.IsIDEASCase() &&
        !aVName.IsNull() && !aVName->IsEmpty() && NMTool.IsBound(aVName->String()) ) {
      myResult = NMTool.Find(aVName->String());
      myError = StepToTopoDS_TranslateVertexDone;
      done = Standard_True;
      return;
    }
    // [END] Proceed with I-DEAS-like STP (ssv; 15.11.2010)

//:S4136    Standard_Real preci = BRepAPI::Precision();
    const Handle(StepShape_VertexPoint) VP = Handle(StepShape_VertexPoint)::DownCast(aVertex);
    const Handle(StepGeom_Point) P = VP->VertexGeometry();
    const Handle(StepGeom_CartesianPoint) P1 = Handle(StepGeom_CartesianPoint)::DownCast(P);
    Handle(Geom_CartesianPoint) P2;
    StepToGeom_MakeCartesianPoint::Convert(P1,P2);
    BRep_Builder B;
    TopoDS_Vertex V;
    B.MakeVertex(V, P2->Pnt(), Precision::Confusion()); //:S4136: preci
    aTool.Bind(aVertex,V);

    // Register Vertex in NM tool (ssv; 14.11.2010)
    if ( NMTool.IsActive() ) {
      NMTool.Bind(aVertex, V);
      if ( NMTool.IsIDEASCase() && !aVName.IsNull() && !aVName->IsEmpty() )
        NMTool.Bind(aVName->String(), V);
    }

    myResult = V;
  }
  else {
    myResult = TopoDS::Vertex(aTool.Find(aVertex));
  }
  myError  = StepToTopoDS_TranslateVertexDone;
  done     = Standard_True;
}

// ============================================================================
// Method  : Value
// Purpose : Return the mapped Shape
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslateVertex::Value() const 
{
  StdFail_NotDone_Raise_if(!done,"");
  return myResult;
}

// ============================================================================
// Method  : Error
// Purpose : Return the TranslateVertex Error Code
// ============================================================================

StepToTopoDS_TranslateVertexError StepToTopoDS_TranslateVertex::Error() const
{
  return myError;
}
