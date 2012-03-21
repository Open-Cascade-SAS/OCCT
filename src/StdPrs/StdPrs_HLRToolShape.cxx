// Copyright (c) 1995-1999 Matra Datavision
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

#include <StdPrs_HLRToolShape.ixx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>

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
