// Created on: 1994-11-30
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

