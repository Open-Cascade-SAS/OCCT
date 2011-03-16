// File:	TopOpeBRepDS_ShapeWithState.cxx
// Created:	Mon Sep 20 11:54:00 1999
// Author:	Peter KURNEV
//		<pkv@kivox.nnov.matra-dtv.fr>

#include <TopOpeBRepDS_ShapeWithState.ixx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : TopOpeBRepDS_ShapeWithState
//purpose  : 
//=======================================================================
TopOpeBRepDS_ShapeWithState::TopOpeBRepDS_ShapeWithState():
       myState(TopAbs_UNKNOWN), myIsSplitted(Standard_False) 
{}
//=======================================================================
//function : Part
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& TopOpeBRepDS_ShapeWithState::Part (const TopAbs_State aState) const
{
  static TopTools_ListOfShape myEmptyListOfShape ;
  switch (aState) 
    {
    case TopAbs_IN:
      return myPartIn;
    case TopAbs_OUT:
      return myPartOut;
    case TopAbs_ON:
      return myPartOn;
    default :
      return myEmptyListOfShape;
    }
}

//=======================================================================
//function : AddPart
//purpose  : 
//=======================================================================
  void TopOpeBRepDS_ShapeWithState::AddPart (const TopoDS_Shape& aShape, 
					     const TopAbs_State aState) 
{
  switch (aState) 
    {
    case TopAbs_IN:
      myPartIn.Append(aShape);
      break;
    case TopAbs_OUT:
      myPartOut.Append(aShape);
      break;
    case TopAbs_ON:
     myPartOn.Append(aShape);
      break;
    default :
      break; 
    }
}

//=======================================================================
//function : AddParts
//purpose  : 
//=======================================================================
  void TopOpeBRepDS_ShapeWithState::AddParts (const TopTools_ListOfShape& aListOfShape, 
					      const TopAbs_State aState) 
{
  TopTools_ListIteratorOfListOfShape anIt(aListOfShape);

  switch (aState) 
    {
    case TopAbs_IN:
      for (; anIt.More(); anIt.Next()) {
	myPartIn.Append(anIt.Value());
      }
      break;
    case TopAbs_OUT:
      for (; anIt.More(); anIt.Next()) {
	myPartOut.Append(anIt.Value());
      }
      break;
    case TopAbs_ON:
      for (; anIt.More(); anIt.Next()) {
	myPartOn.Append(anIt.Value());
      }
      break;
    
      default :
	break; 
    }
}


//=======================================================================
//function : SetState
//purpose  : 
//=======================================================================
  void TopOpeBRepDS_ShapeWithState::SetState(const TopAbs_State aState) 
{
  myState=aState;
}
//=======================================================================
//function : State
//purpose  : 
//=======================================================================
  TopAbs_State TopOpeBRepDS_ShapeWithState::State() const
{
  return myState;
}

//=======================================================================
//function : SetIsSplitted
//purpose  : 
//=======================================================================
  void TopOpeBRepDS_ShapeWithState::SetIsSplitted(const Standard_Boolean aFlag) 
{
  myIsSplitted=aFlag;
}
//=======================================================================
//function : IsSplitted
//purpose  : 
//=======================================================================
  Standard_Boolean TopOpeBRepDS_ShapeWithState::IsSplitted() const
{
  return myIsSplitted;
}
