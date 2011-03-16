// File:	BRepPrimAPI_MakeRevol.cxx
// Created:	Thu Oct 14 14:46:16 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


// Modified by skv - Fri Mar  4 15:50:09 2005
// Add methods for supporting history.

#include <BRepPrimAPI_MakeRevol.ixx>
#include <BRepLib.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_TEdge.hxx>

// perform checks on the argument

static const TopoDS_Shape& check(const TopoDS_Shape& S)
{
 BRepLib::BuildCurves3d(S);
   return S;
}

//=======================================================================
//function : BRepPrimAPI_MakeRevol
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S, 
				     const gp_Ax1& A, 
				     const Standard_Real D, 
				     const Standard_Boolean Copy) :
       myRevol(check(S),A,D,Copy)
{
  Build();
}


//=======================================================================
//function : BRepPrimAPI_MakeRevol
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeRevol::BRepPrimAPI_MakeRevol(const TopoDS_Shape& S, 
				     const gp_Ax1& A, 
				     const Standard_Boolean Copy) :
       myRevol(check(S),A,Copy)
{
  Build();
}


//=======================================================================
//function : Revol
//purpose  : 
//=======================================================================

const BRepSweep_Revol&  BRepPrimAPI_MakeRevol::Revol() const 
{
  return myRevol;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void  BRepPrimAPI_MakeRevol::Build()
{
  myShape = myRevol.Shape();
  Done();
// Modified by skv - Fri Mar  4 15:50:09 2005 Begin
  myDegenerated.Clear();

  TopExp_Explorer anExp(myShape, TopAbs_EDGE);

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &anEdge = anExp.Current();
    Handle(BRep_TEdge)  aTEdge = Handle(BRep_TEdge)::DownCast(anEdge.TShape());

    if (aTEdge->Degenerated())
      myDegenerated.Append(anEdge);
  }
// Modified by skv - Fri Mar  4 15:50:09 2005 End
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape()
{
  return myRevol.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape()
{
  return myRevol.LastShape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakeRevol::Generated (const TopoDS_Shape& S)
{
  myGenerated.Clear();
  if (!myRevol.Shape (S).IsNull())
    myGenerated.Append (myRevol.Shape (S));
  return myGenerated;
}


// Modified by skv - Fri Mar  4 15:50:09 2005 Begin

//=======================================================================
//function : FirstShape
//purpose  : This method returns the shape of the beginning of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::FirstShape(const TopoDS_Shape &theShape)
{
  return myRevol.FirstShape(theShape);
}


//=======================================================================
//function : LastShape
//purpose  : This method returns the shape of the end of the revolution,
//           generated with theShape (subShape of the generating shape).
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakeRevol::LastShape(const TopoDS_Shape &theShape)
{
  return myRevol.LastShape(theShape);
}

//=======================================================================
//function : HasDegenerated
//purpose  : 
//=======================================================================

Standard_Boolean BRepPrimAPI_MakeRevol::HasDegenerated () const
{
  return (!myDegenerated.IsEmpty());
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakeRevol::Degenerated () const
{
  return myDegenerated;
}
// Modified by skv - Fri Mar  4 15:50:09 2005 End
