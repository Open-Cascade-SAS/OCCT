// File:	BRepOffsetAPI_MakePipeShell.cxx
// Created:	Wed Apr  8 19:11:16 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <BRepOffsetAPI_MakePipeShell.ixx>

#include <GeomFill_PipeError.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BRepOffsetAPI_MakePipeShell::BRepOffsetAPI_MakePipeShell(const TopoDS_Wire& Spine)
                      

{
  myPipe = new (BRepFill_PipeShell) (Spine);
  SetTolerance();
  SetTransitionMode();
  NotDone();
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetMode(const Standard_Boolean IsFrenet) 
{
  myPipe->Set(IsFrenet);
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetMode(const gp_Ax2& Axe) 
{
  myPipe->Set(Axe);
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetMode(const gp_Dir& BiNormal) 
{
  myPipe->Set(BiNormal);
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
 Standard_Boolean BRepOffsetAPI_MakePipeShell::SetMode(const TopoDS_Shape& SpineSupport) 
{
  return myPipe->Set(SpineSupport);
}

//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetMode(const TopoDS_Wire& AuxiliarySpine,
				const Standard_Boolean CurvilinearEquivalence,
				const Standard_Boolean KeepContact) 
{
   myPipe->Set(AuxiliarySpine, CurvilinearEquivalence, KeepContact);
}

//=======================================================================
//function :Add
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::Add(const TopoDS_Shape& Profile,
				       const Standard_Boolean WithContact,
				       const Standard_Boolean WithCorrection) 
{
  myPipe->Add(Profile, WithContact, WithCorrection);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::Add(const TopoDS_Shape& Profile,
				       const TopoDS_Vertex& Location,
				       const Standard_Boolean WithContact,
				       const Standard_Boolean WithCorrection) 
{
  myPipe->Add(Profile, Location, WithContact, WithCorrection);
}

//=======================================================================
//function : SetLaw
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetLaw(const TopoDS_Shape& Profile,
					  const Handle(Law_Function)& L,
					  const Standard_Boolean WithContact,
					  const Standard_Boolean WithCorrection) 
{
  myPipe->SetLaw(Profile, L, WithContact, WithCorrection);
}

//=======================================================================
//function : SetLaw
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetLaw(const TopoDS_Shape& Profile,
					  const Handle(Law_Function)& L,
					  const TopoDS_Vertex& Location,
					  const Standard_Boolean WithContact,
					  const Standard_Boolean WithCorrection) 
{
  myPipe->SetLaw(Profile, L, Location, WithContact, WithCorrection);
}

//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakePipeShell::Delete( const TopoDS_Shape& Profile)
{
  myPipe->Delete(Profile);
}


//=======================================================================
//function : IsReady
//purpose  : 
//=======================================================================
 Standard_Boolean BRepOffsetAPI_MakePipeShell::IsReady() const
{
  return myPipe->IsReady();
}

//=======================================================================
//function : GetStatus
//purpose  : 
//=======================================================================
 BRepBuilderAPI_PipeError BRepOffsetAPI_MakePipeShell::GetStatus() const
{
  BRepBuilderAPI_PipeError Status;
  GeomFill_PipeError stat;
  stat = myPipe->GetStatus();
  switch (stat) {
  case GeomFill_PipeOk :
    {
      Status = BRepBuilderAPI_PipeDone;
      break;
    }
  case  GeomFill_PlaneNotIntersectGuide :
    {
      Status = BRepBuilderAPI_PlaneNotIntersectGuide;
      break;
    }
  case  GeomFill_ImpossibleContact :
    {
      Status = BRepBuilderAPI_ImpossibleContact;
      break;
    }
    default :
      Status = BRepBuilderAPI_PipeNotDone; 
  }
  return Status;
}
//=======================================================================
//function : SetTransitionMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetTolerance(const Standard_Real Tol3d,
					 const Standard_Real BoundTol,
					 const Standard_Real TolAngular)
{
 myPipe->SetTolerance(Tol3d, BoundTol, TolAngular);
}

//=======================================================================
//function : SetTransitionMode
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::SetTransitionMode(const BRepBuilderAPI_TransitionMode Mode)
{
  myPipe->SetTransition( (BRepFill_TransitionStyle)Mode );
}

//=======================================================================
//function :Simulate
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::Simulate(const Standard_Integer N,
				     TopTools_ListOfShape& R) 
{
 myPipe->Simulate(N, R); 
}

//=======================================================================
//function :Build() 
//purpose  : 
//=======================================================================
 void BRepOffsetAPI_MakePipeShell::Build() 
{
  Standard_Boolean Ok;
  Ok = myPipe->Build();
  if (Ok) {
    myShape = myPipe->Shape();
    Done();
  }
  else NotDone(); 
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================
 Standard_Boolean BRepOffsetAPI_MakePipeShell::MakeSolid() 
{
  if (!IsDone()) StdFail_NotDone::Raise("BRepOffsetAPI_MakePipeShell::MakeSolid");
  Standard_Boolean Ok;
  Ok = myPipe->MakeSolid();
  if (Ok) myShape = myPipe->Shape();
  return Ok;
}

//=======================================================================
//function :FirstShape()
//purpose  : 
//=======================================================================
 TopoDS_Shape BRepOffsetAPI_MakePipeShell::FirstShape() 
{
  return myPipe->FirstShape();
}

//=======================================================================
//function : LastShape()
//purpose  : 
//=======================================================================
 TopoDS_Shape BRepOffsetAPI_MakePipeShell::LastShape() 
{
  return myPipe->LastShape();
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& 
BRepOffsetAPI_MakePipeShell::Generated(const TopoDS_Shape& S) 
{
  myPipe->Generated(S, myGenerated);
  return myGenerated;
}

