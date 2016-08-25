// Created on: 1995-01-30
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//szv#4 S4163
//eap: Tue Aug 29 11:02:56 2000: Shape Processing moved to upper levels

#include <BRep_Builder.hxx>
#include <BRepToIGES_BREntity.hxx>
#include <BRepToIGES_BRShell.hxx>
#include <BRepToIGES_BRSolid.hxx>
#include <BRepToIGES_BRWire.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Trsf.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Standard_Transient.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <TransferBRep_OrientedShapeMapper.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//#include <ShapeCustom.hxx>
char Name[100];

//=======================================================================
//function : BRepToIGES_BREntity
//purpose  : 
//=======================================================================

BRepToIGES_BREntity::BRepToIGES_BREntity()
: TheUnitFactor(1.0),
  myConvSurface(Interface_Static::IVal("write.convertsurface.mode") != 0),
  myPCurveMode (Interface_Static::IVal("write.surfacecurve.mode")   != 0),
  TheMap       (new Transfer_FinderProcess())
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::Init()
{  
  TheMap = new Transfer_FinderProcess();
  TheUnitFactor = 1.;
  myConvSurface = Interface_Static::IVal("write.convertsurface.mode") != 0;
  myPCurveMode  = Interface_Static::IVal("write.surfacecurve.mode")   != 0;
}

//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================
void BRepToIGES_BREntity::SetModel(const Handle(IGESData_IGESModel)& model)
{  
  TheModel = model;  
  Standard_Real unitfactor = TheModel->GlobalSection().UnitValue();
  if (unitfactor != 1.)
    TheUnitFactor = unitfactor;
}


//=======================================================================
//function : GetModel
//purpose  : 
//=======================================================================
Handle(IGESData_IGESModel) BRepToIGES_BREntity::GetModel() const
{ return TheModel; }


//=======================================================================
//function : GetUnit
//purpose  : 
//=======================================================================
Standard_Real BRepToIGES_BREntity::GetUnit() const
{
  return TheUnitFactor;
}  


//=======================================================================
//function : SetTransferProcess
//purpose  : 
//=======================================================================
void BRepToIGES_BREntity::SetTransferProcess(const Handle(Transfer_FinderProcess)& TP)
{  TheMap = TP;  }


//=======================================================================
//function : GetTransferProcess
//purpose  : 
//=======================================================================
Handle(Transfer_FinderProcess) BRepToIGES_BREntity::GetTransferProcess() const
{ return TheMap; }


//=======================================================================
//function : TransferShape
//purpose  : 
//=======================================================================
Handle(IGESData_IGESEntity) BRepToIGES_BREntity::TransferShape
(const TopoDS_Shape& start)
{
  Handle(IGESData_IGESEntity) res;
//  TopoDS_Shape theShape;
  //Standard_Integer Nb = 1; //szv#4:S4163:12Mar99 not needed

  if (start.IsNull()) return res;

  if (start.ShapeType() == TopAbs_VERTEX) {
    TopoDS_Vertex V = TopoDS::Vertex(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferVertex(V);
  }  
  else if (start.ShapeType() == TopAbs_EDGE) {
    TopoDS_Edge E =  TopoDS::Edge(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferEdge(E, Standard_False);
  }  
  else if (start.ShapeType() == TopAbs_WIRE) {
    TopoDS_Wire W =  TopoDS::Wire(start);
    BRepToIGES_BRWire BW(*this);
    BW.SetModel(GetModel());
    res = BW.TransferWire(W);
  }  
  else { 
    //    la fonction suivante force les surfaces indirectes en
    //    surfaces directes (obligatoire dans IGES) 
//    theShape = ShapeCustom::DirectFaces(start);  
    //sprintf(Name,"res_%d",Nb++);
    //DBRep::Set(Name,theShape);

    if (start.ShapeType() == TopAbs_FACE) {
      TopoDS_Face F =  TopoDS::Face(start);
      BRepToIGES_BRShell BS(*this);
      BS.SetModel(GetModel());
      res = BS.TransferFace(F);
    }  
    else if (start.ShapeType() == TopAbs_SHELL) {
      TopoDS_Shell S =  TopoDS::Shell(start);
      BRepToIGES_BRShell BS(*this);
      BS.SetModel(GetModel());
      res = BS.TransferShell(S);
    }  
    else if (start.ShapeType() == TopAbs_SOLID) {
      TopoDS_Solid M =  TopoDS::Solid(start);
      BRepToIGES_BRSolid BS(*this);
      BS.SetModel(GetModel());
      res = BS.TransferSolid(M);
    }  
    else if (start.ShapeType() == TopAbs_COMPSOLID) {
      TopoDS_CompSolid C =  TopoDS::CompSolid(start);
      BRepToIGES_BRSolid BS(*this);
      BS.SetModel(GetModel());
      res = BS.TransferCompSolid(C);
    }  
    else if (start.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Compound C =  TopoDS::Compound(start);
      BRepToIGES_BRSolid BS(*this);
      BS.SetModel(GetModel());
      res = BS.TransferCompound(C);
    }  
    else {
      // message d`erreur
    }  
  }

  return res;
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::AddFail 
  (const TopoDS_Shape& start,
   const Standard_CString amess)
{
  Handle(TransferBRep_ShapeMapper) Mapper = new TransferBRep_ShapeMapper(start);
  TheMap->AddFail(Mapper, amess);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::AddWarning 
  (const TopoDS_Shape& start,
   const Standard_CString amess)
{
  Handle(TransferBRep_ShapeMapper) Mapper = new TransferBRep_ShapeMapper(start);
  TheMap->AddWarning(Mapper, amess);
}


//=======================================================================
//function : AddFail
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::AddFail 
  (const Handle(Standard_Transient)& start,
   const Standard_CString amess)
{
  Handle(Transfer_TransientMapper) Mapper = new Transfer_TransientMapper(start);
  TheMap->AddFail(Mapper, amess);
}


//=======================================================================
//function : AddWarning
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::AddWarning 
  (const Handle(Standard_Transient)& start,
   const Standard_CString amess)
{
  Handle(Transfer_TransientMapper) Mapper = new Transfer_TransientMapper(start);
  TheMap->AddWarning(Mapper, amess);
}


//=======================================================================
//function : HasShapeResult
//purpose  : 
//=======================================================================

Standard_Boolean BRepToIGES_BREntity::HasShapeResult 
  (const TopoDS_Shape& start) const
{
  Handle(TransferBRep_ShapeMapper) Mapper = new TransferBRep_ShapeMapper(start);
  DeclareAndCast(Transfer_SimpleBinderOfTransient, binder, TheMap->Find(Mapper));
  if (binder.IsNull()) return Standard_False;
  return binder->HasResult();
}


//=======================================================================
//function : GetShapeResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) BRepToIGES_BREntity::GetShapeResult 
  (const TopoDS_Shape& start) const
{
  Handle(Standard_Transient) res;

  Handle(TransferBRep_ShapeMapper) Mapper = new TransferBRep_ShapeMapper(start);
  DeclareAndCast(Transfer_SimpleBinderOfTransient, binder, TheMap->Find(Mapper));
  if (binder.IsNull()) return res;
  if (binder->HasResult())
    res = binder->Result();
  return res;
}


//=======================================================================
//function : SetShapeResult
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::SetShapeResult 
  ( const TopoDS_Shape& start,
    const Handle(Standard_Transient)& result)
{
  Handle(TransferBRep_ShapeMapper) Mapper = new TransferBRep_ShapeMapper(start);
  Handle(Transfer_SimpleBinderOfTransient) binder = new Transfer_SimpleBinderOfTransient;
  binder->SetResult(result);
  TheMap->Bind(Mapper,binder);
}

//=======================================================================
//function : HasShapeResult
//purpose  : 
//=======================================================================

Standard_Boolean BRepToIGES_BREntity::HasShapeResult 
  (const Handle(Standard_Transient)& start) const
{
  Handle(Transfer_TransientMapper) Mapper = new Transfer_TransientMapper(start);
  DeclareAndCast(Transfer_SimpleBinderOfTransient, binder, TheMap->Find(Mapper));
  if (binder.IsNull()) return Standard_False;
  return binder->HasResult();
}


//=======================================================================
//function : GetShapeResult
//purpose  : 
//=======================================================================

Handle(Standard_Transient) BRepToIGES_BREntity::GetShapeResult 
  (const Handle(Standard_Transient)& start) const
{
  Handle(Standard_Transient) res;

  Handle(Transfer_TransientMapper) Mapper = new Transfer_TransientMapper(start);
  DeclareAndCast(Transfer_SimpleBinderOfTransient, binder, TheMap->Find(Mapper));
  if (binder.IsNull()) return res;
  if (binder->HasResult())
    res = binder->Result();
  return res;
}


//=======================================================================
//function : SetShapeResult
//purpose  : 
//=======================================================================

void BRepToIGES_BREntity::SetShapeResult 
  ( const Handle(Standard_Transient)& start,
    const Handle(Standard_Transient)& result)
{
  Handle(Transfer_TransientMapper) Mapper = new Transfer_TransientMapper(start);
  Handle(Transfer_SimpleBinderOfTransient) binder = new Transfer_SimpleBinderOfTransient;
  TheMap->Bind(Mapper,binder);
  binder->SetResult(result);
}

//=======================================================================
//function : GetConvertSurfaceMode
//purpose  : 
//=======================================================================

Standard_Boolean BRepToIGES_BREntity::GetConvertSurfaceMode () const
{
  return myConvSurface;
}
	    
//=======================================================================
//function : GetPCurveMode
//purpose  : 
//=======================================================================

Standard_Boolean BRepToIGES_BREntity::GetPCurveMode () const
{
  return myPCurveMode;
}
	    
//=======================================================================
//function : ~BRepToIGES_BREntity
//purpose  : 
//=======================================================================

BRepToIGES_BREntity::~BRepToIGES_BREntity() 
{} 
