// File:	TopOpeBRep_Point2d.cxx
// Created:	Thu Oct 29 09:55:47 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#include <TopOpeBRep_Point2d.ixx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRepDS.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : TopOpeBRep_Point2d
//purpose  : 
//=======================================================================
TopOpeBRep_Point2d::TopOpeBRep_Point2d() :
myhaspint(Standard_False),
myisvertex1(Standard_False),
myparameter1(0.),
myisvertex2(Standard_False),
myparameter2(0.),
myispointofsegment(Standard_False),
myips1(0),myips2(0),myhasancestors(Standard_False),
mystatus(TopOpeBRep_P2DUNK),
myindex(0),
mykeep(Standard_True),
myedgesconfig(TopOpeBRepDS_UNSHGEOMETRY),
mytolerance(0.)
{
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const TopoDS_Vertex& TopOpeBRep_Point2d::Vertex(const Standard_Integer Index) const
{
  if (!IsVertex(Index)) Standard_Failure::Raise("TopOpeBRep_Point2d::Vertex");
  if      (Index == 1) return myvertex1;
  else if (Index == 2) return myvertex2;
  else Standard_Failure::Raise("TopOpeBRep_Point2d::Vertex");
  return myvertex1;
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================
const TopOpeBRepDS_Transition& TopOpeBRep_Point2d::Transition(const Standard_Integer Index) const
{
  if      (Index == 1) return mytransition1;
  else if (Index == 2) return mytransition2;
  else Standard_Failure::Raise("TopOpeBRep_Point2d::Transition");
  return mytransition1;
}

//=======================================================================
//function : ChangeTransition
//purpose  : 
//=======================================================================
TopOpeBRepDS_Transition& TopOpeBRep_Point2d::ChangeTransition(const Standard_Integer Index)
{
  if      (Index == 1) return mytransition1;
  else if (Index == 2) return mytransition2;
  else Standard_Failure::Raise("TopOpeBRep_Point2d::ChangeTransition");
  return mytransition1;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void TopOpeBRep_Point2d::Dump(const Standard_Integer E1index,const Standard_Integer E2index) const
{
#ifdef DEB
  Standard_Real par1 = Parameter(1);
  Standard_Real par2 = Parameter(2);
  
  Standard_Integer index = Index();
  Standard_Boolean keep = Keep();
  Standard_Integer sts = Status();
  Standard_Boolean pos = IsPointOfSegment();
  Standard_Real tol = Tolerance();
  
  const TopOpeBRepDS_Transition& T1 = Transition(1);
  const TopOpeBRepDS_Transition& T2 = Transition(2);
  
  Standard_Boolean isvertex1 = IsVertex(1); TopoDS_Vertex V1; if (isvertex1) V1 = Vertex(1);
  Standard_Boolean isvertex2 = IsVertex(2); TopoDS_Vertex V2; if (isvertex2) V2 = Vertex(2);
  Standard_Boolean isvertex = isvertex1 || isvertex2;

  Standard_Integer ia1,ia2; SegmentAncestors(ia1,ia2);
  cout<<endl<<"p2d "<<index<<"  k="<<keep<<" pos="<<pos;
  switch (sts) {
  case TopOpeBRep_P2DUNK : cout<<" sts=u";break;
  case TopOpeBRep_P2DSGF : cout<<" sts=f";break;
  case TopOpeBRep_P2DSGL : cout<<" sts=l";break;
  case TopOpeBRep_P2DNEW :
    cout<<" sts=n";
    cout<<" anc="<<ia1<<","<<ia2;
    break;
  case TopOpeBRep_P2DINT : cout<<" sts=i";break;
  } // switch
  cout<<" cfg=";TopOpeBRepDS::Print(myedgesconfig,cout);
  cout<<endl;
  
  gp_Pnt P3D = Value();
#ifdef DRAW
  cout<<FUN_tool_PRODINP()<<"P"<<Index()<<" "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z()<<"; # tol = "<<tol<<endl;
#endif
  cout<<"     on (1) :"; cout<<" vertex(1) : "; cout<<(isvertex1)? 1:0;
  cout<<"  T "<<E1index<<"(1) : "; T1.Dump(cout);
  cout<<" par(1) = "<<par1;
  if (isvertex1) {
    P3D = BRep_Tool::Pnt(V1);
    cout<<" PV(1) : "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z();
  }
  cout<<endl;
  
  cout<<"     on (2) :"; cout<<" vertex(2) : "; cout<<(isvertex2)? 1:0;
  cout<<"  T "<<E2index<<"(2) : "; T2.Dump(cout); 
  cout<<" par(2) = "<<par2;
  if (isvertex2) {
    P3D = BRep_Tool::Pnt(V2);
    cout<<" PV(2) : "<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z();
  }
  cout<<endl;
#endif
}
