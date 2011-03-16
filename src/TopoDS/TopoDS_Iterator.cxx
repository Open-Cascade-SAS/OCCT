// File:	TopoDS_Iterator.cxx
// Created:	Thu Jan 21 09:11:16 1993
// Author:	Remi LEQUETTE

#define No_Standard_NoSuchObject

#include <TopoDS_Iterator.ixx>


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TopoDS_Iterator::Initialize(const TopoDS_Shape& S,
                                 const Standard_Boolean cumOri,
                                 const Standard_Boolean cumLoc)
{
  if (cumLoc)
    myLocation = S.Location();
  else
    myLocation.Identity();
  if (cumOri)
    myOrientation = S.Orientation();
  else
    myOrientation = TopAbs_FORWARD;
  myShapes.Initialize(S.TShape()->Shapes());
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopoDS_Iterator::Next()
{
  myShapes.Next();
  if (More()) {
    myShape = myShapes.Value();
    myShape.Orientation(TopAbs::Compose(myOrientation,myShape.Orientation()));
    //modified by NIZNHY-PKV Fri Jan 16 07:42:30 2009f
    if (!myLocation.IsIdentity())
    //modified by NIZNHY-PKV Fri Jan 16 07:42:37 2009t
      myShape.Move(myLocation);
  }
}
