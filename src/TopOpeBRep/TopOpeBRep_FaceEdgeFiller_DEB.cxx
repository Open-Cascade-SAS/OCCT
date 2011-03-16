// File:	TopOpeBRep_FaceEdgeFiller_DEB.cxx
// Created:	Fri Jun 16 16:00:24 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#ifdef DEB

#include <TCollection_AsciiString.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

//------------------------------------------------------------------
void FEINT_DUMPPOINTS(TopOpeBRep_FaceEdgeIntersector& FEINT,
		      const TopOpeBRepDS_DataStructure& BDS)
{
  FEINT.InitPoint(); 
  if ( ! FEINT.MorePoint() ) return;
  
  cout<<endl;
  cout<<"---------- F/E : "<<FEINT.NbPoints()<<" p ";
  cout<<endl;
  
  const TopoDS_Shape& FF = FEINT.Shape(1);
  const TopoDS_Shape& EE = FEINT.Shape(2);
  
  Standard_Integer FFindex = BDS.Shape(FF);
  Standard_Integer EEindex = BDS.Shape(EE);
  
  TopAbs_Orientation FFori = FF.Orientation();
  TopAbs_Orientation EEori = EE.Orientation();
  cout<<"FF = "<<FFindex<<" ";TopAbs::Print(FFori,cout);
  cout<<", ";
  cout<<"EE = "<<EEindex<<" ";TopAbs::Print(EEori,cout);
  cout<<endl;
  
  Standard_Integer ip = 1;
  for (; FEINT.MorePoint(); FEINT.NextPoint(), ip++ ) {
    gp_Pnt2d      pUV; FEINT.UVPoint(pUV);
    TopAbs_State  sta = FEINT.State();
    Standard_Real parE = FEINT.Parameter();
    
    TopOpeBRepDS_Transition T1,T2;
    T1 = FEINT.Transition(1,EEori); // EEori bidon
    T2 = FEINT.Transition(2,FFori);
    
    TopoDS_Vertex V1;
    Standard_Boolean isvertexF = FEINT.IsVertex(1,V1);
    TopoDS_Vertex V2;
    Standard_Boolean isvertexE = FEINT.IsVertex(2,V2);
    Standard_Boolean isvertex = isvertexF || isvertexE;
    
    cout<<endl;
    cout<<"P"<<ip<<" : "; 
    gp_Pnt P3D = FEINT.Value(); 
    cout<<"\t"<<P3D.X()<<" "<<P3D.Y()<<" "<<P3D.Z()<<endl;
    
    cout<<"\t"; if (isvertexF) cout<<"IS VERTEX, ";
    cout<<"T on "<<FFindex<<" : "; T1.Dump(cout);
    cout<<" pUV = "<<pUV.X()<<" "<<pUV.Y()<<endl;
    cout<<" sta = "; TopAbs::Print(sta,cout);cout<<endl;
    
    cout<<"\t"; if (isvertexE) cout<<"IS VERTEX, ";
    cout<<"T on "<<EEindex<<" : "; T2.Dump(cout); 
    cout<<" parE = "<<parE<<endl;
  }
}

#endif
