// File:	TopOpeBRepTool_C2DF.cxx
// Created:	Mon Mar 23 17:14:26 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#include <TopOpeBRepTool_C2DF.ixx>
#include <TopOpeBRepTool_define.hxx>

//=======================================================================
//function : TopOpeBRepTool_C2DF
//purpose  : 
//=======================================================================

TopOpeBRepTool_C2DF::TopOpeBRepTool_C2DF() {}

//=======================================================================
//function : TopOpeBRepTool_C2DF
//purpose  : 
//=======================================================================

TopOpeBRepTool_C2DF::TopOpeBRepTool_C2DF
(const Handle(Geom2d_Curve)& PC,const Standard_Real f2d,const Standard_Real l2d,const Standard_Real tol,const TopoDS_Face& F)
{
  myPC = PC;myf2d = f2d;myl2d = l2d;mytol = tol;
  myFace = F;
}

//=======================================================================
//function : SetPC
//purpose  : 
//=======================================================================

void TopOpeBRepTool_C2DF::SetPC
(const Handle(Geom2d_Curve)& PC,const Standard_Real f2d,const Standard_Real l2d,const Standard_Real tol)
{
  myPC = PC;myf2d = f2d;myl2d = l2d;mytol = tol;
}

//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================

void TopOpeBRepTool_C2DF::SetFace(const TopoDS_Face& F)
{
  myFace = F;
}

//=======================================================================
//function : PC
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& TopOpeBRepTool_C2DF::PC(Standard_Real& f2d,Standard_Real& l2d,Standard_Real& tol) const
{
  f2d = myf2d;l2d = myl2d;tol = mytol;
  return myPC;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_C2DF::Face() const
{
  return myFace;
}

//=======================================================================
//function : IsPC
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_C2DF::IsPC(const Handle(Geom2d_Curve)& PC) const
{
  Standard_Boolean b = (PC == myPC);
  return b;
}

//=======================================================================
//function : IsFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_C2DF::IsFace(const TopoDS_Face& F) const
{
  Standard_Boolean b = (F.IsEqual(myFace));
  return b;
}

