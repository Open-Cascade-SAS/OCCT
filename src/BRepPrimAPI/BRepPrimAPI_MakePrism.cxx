// File:	BRepPrimAPI_MakePrism.cxx
// Created:	Thu Oct 14 14:45:45 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


// Modified by skv - Fri Mar  4 15:50:09 2005
// Add methods for supporting history.

#include <BRepPrimAPI_MakePrism.ixx>
#include <BRepLib.hxx>

// perform checks on the argument

static const TopoDS_Shape& check(const TopoDS_Shape& S)
{
 BRepLib::BuildCurves3d(S);
   return S;
}

//=======================================================================
//function : BRepPrimAPI_MakePrism
//purpose  : 
//=======================================================================

BRepPrimAPI_MakePrism::BRepPrimAPI_MakePrism(const TopoDS_Shape& S,
				     const gp_Vec& V, 
				     const Standard_Boolean Copy,
				     const Standard_Boolean Canonize) :
       myPrism(check(S),V,Copy,Canonize)
{
  Build();
}


//=======================================================================
//function : BRepPrimAPI_MakePrism
//purpose  : 
//=======================================================================

BRepPrimAPI_MakePrism::BRepPrimAPI_MakePrism(const TopoDS_Shape& S, 
				     const gp_Dir& D,
				     const Standard_Boolean Inf, 
				     const Standard_Boolean Copy,
				     const Standard_Boolean Canonize) :
       myPrism(check(S),D,Inf,Copy,Canonize)
{
  Build();
}


//=======================================================================
//function : Prism
//purpose  : 
//=======================================================================

const BRepSweep_Prism&  BRepPrimAPI_MakePrism::Prism()const 
{
  return myPrism;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void  BRepPrimAPI_MakePrism::Build()
{
  myShape = myPrism.Shape();
  Done();
}

//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::FirstShape()
{
  return myPrism.FirstShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::LastShape()
{
  return myPrism.LastShape();
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepPrimAPI_MakePrism::Generated (const TopoDS_Shape& S)
{
  myGenerated.Clear();
  myGenerated.Append(myPrism.Shape (S));
  return myGenerated;

}


// Modified by skv - Fri Mar  4 15:50:09 2005 Begin

//=======================================================================
//function : FirstShape
//purpose  : This method returns the bottom shape of the prism, generated
//           with theShape (subShape of the generating shape)
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::FirstShape(const TopoDS_Shape &theShape)
{
  return myPrism.FirstShape(theShape);
}


//=======================================================================
//function : LastShape
//purpose  : This method returns the top shape of the prism, generated
//           with theShape (subShape of the generating shape)
//=======================================================================

TopoDS_Shape BRepPrimAPI_MakePrism::LastShape(const TopoDS_Shape &theShape)
{
  return myPrism.LastShape(theShape);
}

// Modified by skv - Fri Mar  4 15:50:09 2005 End
