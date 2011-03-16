// File:	BRepBuilderAPI_MakeSolid.cxx
// Created:	Fri Jul 23 15:51:52 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepBuilderAPI_MakeSolid.ixx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid()
{
}

//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_CompSolid& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2)
: myMakeSolid(S1,S2)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2, 
				     const TopoDS_Shell& S3)
: myMakeSolid(S1,S2,S3)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So)
: myMakeSolid(So)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So, 
				     const TopoDS_Shell& S)
: myMakeSolid(So,S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeSolid::Add(const TopoDS_Shell& S)
{
  myMakeSolid.Add(S);
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDone() const
{
  return myMakeSolid.IsDone();
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepBuilderAPI_MakeSolid::Solid()const 
{
  return myMakeSolid.Solid();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::operator TopoDS_Solid() const
{
  return Solid();
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDeleted (const TopoDS_Shape& S) 

{
  if(S.ShapeType() == TopAbs_FACE) {
    BRepLib_ShapeModification aStatus = myMakeSolid.FaceStatus(TopoDS::Face(S));
   
    if(aStatus == BRepLib_Deleted) return Standard_True;

  }
    
  return Standard_False;
}

