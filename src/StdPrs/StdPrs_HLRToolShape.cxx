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


#include <BRepAdaptor_Curve.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>
#include <StdPrs_HLRToolShape.hxx>
#include <TopoDS_Shape.hxx>

StdPrs_HLRToolShape::StdPrs_HLRToolShape (
		  const TopoDS_Shape& TheShape,
                  const HLRAlgo_Projector& TheProjector)
{
  Handle(HLRBRep_Algo) Hider = new HLRBRep_Algo();
  Standard_Integer nbIso = 0; // 5;
  Hider->Add(TheShape, nbIso);
  Hider->Projector(TheProjector);
  Hider->Update();
  Hider->Hide();
  MyData = Hider->DataStructure();
  MyCurrentEdgeNumber = 0;
}

Standard_Integer StdPrs_HLRToolShape::NbEdges() const {
  return MyData->NbEdges();
}
void StdPrs_HLRToolShape::InitVisible(const Standard_Integer EdgeNumber) {
  myEdgeIterator.InitVisible
    (MyData->EDataArray().ChangeValue(EdgeNumber).Status());
  MyCurrentEdgeNumber = EdgeNumber;
}
Standard_Boolean StdPrs_HLRToolShape::MoreVisible() const {
  return myEdgeIterator.MoreVisible();
}
void StdPrs_HLRToolShape::NextVisible()  {
  myEdgeIterator.NextVisible();
}
void StdPrs_HLRToolShape::Visible(BRepAdaptor_Curve& TheEdge,
				  Standard_Real& U1,
				  Standard_Real& U2) {

  TheEdge = MyData->EDataArray()
    .ChangeValue(MyCurrentEdgeNumber)
      .ChangeGeometry()
	.Curve();
  Standard_ShortReal t1,t2;
  myEdgeIterator.Visible(U1,t1,U2,t2);
}
void StdPrs_HLRToolShape::InitHidden(const Standard_Integer EdgeNumber) {
  myEdgeIterator.InitHidden
    (MyData->EDataArray().ChangeValue(EdgeNumber).Status());
  MyCurrentEdgeNumber = EdgeNumber;
}
Standard_Boolean StdPrs_HLRToolShape::MoreHidden() const {
  return myEdgeIterator.MoreHidden();
}
void StdPrs_HLRToolShape::NextHidden()  {
   myEdgeIterator.NextHidden();
}
void StdPrs_HLRToolShape::Hidden (BRepAdaptor_Curve& TheEdge,
				  Standard_Real& U1,
				  Standard_Real& U2) {

  TheEdge = MyData->EDataArray()
    .ChangeValue(MyCurrentEdgeNumber)
      .ChangeGeometry()
	.Curve();
  Standard_ShortReal t1,t2;
  myEdgeIterator.Hidden(U1,t1,U2,t2);
}
