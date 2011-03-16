// File:	TopoDSToStep_MakeStepVertex.cxx
// Created:	Wed Nov 30 14:53:36 1994
// Author:	Frederic MAUPAS
//		<fma@bibox>

#include <TopoDSToStep_MakeStepVertex.ixx>

#include <GeomToStep_MakeCartesianPoint.hxx>

#include <BRep_Tool.hxx>

#include <StepGeom_CartesianPoint.hxx>
#include <StepShape_VertexPoint.hxx>

#include <gp_Pnt.hxx>
#include <TransferBRep_ShapeMapper.hxx>

#include <TCollection_HAsciiString.hxx>

// Processing of non-manifold topology (ssv; 11.11.2010)
#include <TransferBRep.hxx>
#include <Interface_Static.hxx>

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

TopoDSToStep_MakeStepVertex::TopoDSToStep_MakeStepVertex()
{
  done = Standard_False;
}

TopoDSToStep_MakeStepVertex::TopoDSToStep_MakeStepVertex
(const TopoDS_Vertex& V, 
 TopoDSToStep_Tool& T,
 const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Init(V, T, FP);
}

// ----------------------------------------------------------------------------
// Method  : Init
// Purpose :
// ----------------------------------------------------------------------------

void TopoDSToStep_MakeStepVertex::Init(const TopoDS_Vertex& aVertex, 
                                       TopoDSToStep_Tool& aTool,
                                       const Handle(Transfer_FinderProcess)& FP)
{

  aTool.SetCurrentVertex(aVertex);

  // [BEGIN] Processing non-manifold topology (ssv; 11.11.2010)
  Standard_Boolean isNMMode = Interface_Static::IVal("write.step.nonmanifold");
  if (isNMMode) {
    Handle(StepShape_VertexPoint) aVP;
    Handle(TransferBRep_ShapeMapper) aSTEPMapper = TransferBRep::ShapeMapper(FP, aVertex);
    if ( FP->FindTypedTransient(aSTEPMapper, STANDARD_TYPE(StepShape_VertexPoint), aVP) ) {
      // Non-manifold topology detected
      myError  = TopoDSToStep_VertexOther;
      myResult = aVP;
      done     = Standard_True;
      return;
    }
  }
  // [END] Processing non-manifold topology (ssv; 11.11.2010)
  
  if (aTool.IsBound(aVertex)) {
    myError  = TopoDSToStep_VertexOther;
    done     = Standard_True;
    myResult = aTool.Find(aVertex);  
    return;
  } 

  gp_Pnt P;
  
  P = BRep_Tool::Pnt(aVertex);
  GeomToStep_MakeCartesianPoint MkPoint(P);
  Handle(StepGeom_CartesianPoint) Gpms = MkPoint.Value();
  Handle(StepShape_VertexPoint) Vpms =
    new StepShape_VertexPoint();
  Handle(TCollection_HAsciiString) aName = 
    new TCollection_HAsciiString("");

  Vpms->Init(aName, Gpms);

  aTool.Bind(aVertex,Vpms);
  myError  = TopoDSToStep_VertexDone;
  done     = Standard_True;
  myResult = Vpms;
}


// ----------------------------------------------------------------------------
// Method  : Value
// Purpose :
// ----------------------------------------------------------------------------

const Handle(StepShape_TopologicalRepresentationItem)& TopoDSToStep_MakeStepVertex::Value() const 
{
  StdFail_NotDone_Raise_if(!done,"");
  return myResult;
}

// ----------------------------------------------------------------------------
// Method  : Error
// Purpose :
// ----------------------------------------------------------------------------

TopoDSToStep_MakeVertexError TopoDSToStep_MakeStepVertex::Error() const 
{
  return myError;
}

