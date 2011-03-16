// File:	IntTools_PntOnFace.cxx
// Created:	Thu Dec 13 14:16:59 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <IntTools_PntOnFace.ixx>
//=======================================================================
//function : IntTools_PntOnFace::IntTools_PntOnFace
//purpose  : 
//=======================================================================
  IntTools_PntOnFace::IntTools_PntOnFace()
:
  myIsValid(Standard_False),
  myU(99.),
  myV(99.)
{}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::Init(const TopoDS_Face& aF,
				const gp_Pnt& aP,
				const Standard_Real anU,
				const Standard_Real aV)
{
  myFace=aF;
  myPnt=aP;
  myU=anU;
  myV=aV;
}
//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetFace(const TopoDS_Face& aF)
{
  myFace=aF;
}

//=======================================================================
//function : SetPnt
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetPnt(const gp_Pnt& aP)
{
  myPnt=aP;
}
//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetParameters(const Standard_Real anU,
					 const Standard_Real aV)
{
  myU=anU;
  myV=aV;
}
//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetValid(const Standard_Boolean bF)
{
  myIsValid=bF;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
  const TopoDS_Face& IntTools_PntOnFace::Face()const
{
  return myFace;
}
//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================
  const gp_Pnt& IntTools_PntOnFace::Pnt()const
{
  return myPnt;
}
//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::Parameters(Standard_Real& anU,
				      Standard_Real& aV)const
{
  anU=myU;
  aV=myV;
}
//=======================================================================
//function : Valid
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_PntOnFace::Valid()const
{
  return myIsValid;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

