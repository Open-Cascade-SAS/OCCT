// File:	IntTools_PntOn2Faces.cxx
// Created:	Thu Dec 13 14:26:44 2001
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <IntTools_PntOn2Faces.ixx>

//=======================================================================
//function : IntTools_PntOn2Faces::IntTools_PntOn2Faces
//purpose  : 
//=======================================================================
  IntTools_PntOn2Faces::IntTools_PntOn2Faces()
:
  myIsValid(Standard_False)
{}
//=======================================================================
//function : IntTools_PntOn2Faces::IntTools_PntOn2Faces
//purpose  : 
//=======================================================================
  IntTools_PntOn2Faces::IntTools_PntOn2Faces(const IntTools_PntOnFace& aP1,
					     const IntTools_PntOnFace& aP2)
:
  myIsValid(Standard_False)
{
  myPnt1=aP1;
  myPnt2=aP2;
}
//=======================================================================
//function : SetP1
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetP1(const IntTools_PntOnFace& aP)
{
  myPnt1=aP;
}
//=======================================================================
//function : SetP2
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetP2(const IntTools_PntOnFace& aP)
{
  myPnt2=aP;
}
//=======================================================================
//function : P1
//purpose  : 
//=======================================================================
  const IntTools_PntOnFace& IntTools_PntOn2Faces::P1()const
{
  return myPnt1;
}
//=======================================================================
//function : P2
//purpose  : 
//=======================================================================
  const IntTools_PntOnFace& IntTools_PntOn2Faces::P2()const 
{
  return myPnt2;
}

//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================
  void IntTools_PntOn2Faces::SetValid(const Standard_Boolean bF)
{
  myIsValid=bF;
}
//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_PntOn2Faces::IsValid()const
{
  return myIsValid;
}

