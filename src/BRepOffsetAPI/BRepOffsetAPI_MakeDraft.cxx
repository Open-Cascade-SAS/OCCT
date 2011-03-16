#include <BRepOffsetAPI_MakeDraft.ixx>


BRepOffsetAPI_MakeDraft::BRepOffsetAPI_MakeDraft(const TopoDS_Shape& Shape,
				     const gp_Dir& Dir,
				     const Standard_Real Angle)
                               : myDraft( Shape, Dir, Angle)
{
  NotDone();
}

 void BRepOffsetAPI_MakeDraft::SetOptions(const BRepBuilderAPI_TransitionMode Style,
				    const Standard_Real AngleMin, 
				    const Standard_Real AngleMax)
{
  BRepFill_TransitionStyle style =  BRepFill_Right;
  if (Style == BRepBuilderAPI_RoundCorner) style =  BRepFill_Round;
  myDraft.SetOptions( style, AngleMin,  AngleMax);
}

 void BRepOffsetAPI_MakeDraft::SetDraft(const Standard_Boolean IsInternal)
{
   myDraft.SetDraft(IsInternal);
}

 void BRepOffsetAPI_MakeDraft::Perform(const Standard_Real LengthMax) 
{
  myDraft.Perform( LengthMax);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }
}

 void BRepOffsetAPI_MakeDraft::Perform(const Handle(Geom_Surface)& Surface,
				 const Standard_Boolean KeepInsideSurface) 
{
  myDraft.Perform(Surface, KeepInsideSurface);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  }  
}

 void BRepOffsetAPI_MakeDraft::Perform(const TopoDS_Shape& StopShape,
				 const Standard_Boolean KeepOutSide) 
{
  myDraft.Perform( StopShape, KeepOutSide);
  if (myDraft.IsDone()) {
    Done();
    myShape = myDraft.Shape();
  } 
}

 TopoDS_Shell BRepOffsetAPI_MakeDraft::Shell() const
{
  return myDraft.Shell();
}

const TopTools_ListOfShape& BRepOffsetAPI_MakeDraft::Generated(const TopoDS_Shape& S) 
{
  return myDraft.Generated( S );
}

