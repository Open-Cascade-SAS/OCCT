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
