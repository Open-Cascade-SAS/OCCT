#include <IntPatch_PolygoTool.ixx>



void IntPatch_PolygoTool::Dump(const IntPatch_Polygo& L) { 
  static int num=0;
  num++; 
  cout<<"\n#------------- D u m p     B o x 2 d   ("<<num<<")"<<endl;
  IntPatch_PolygoTool::Bounding(L).Dump();
  cout<<"\n#-----------------------------------------------"<<endl;
  
  Standard_Integer nbs = IntPatch_PolygoTool::NbSegments(L);
  cout<<"\npol2d "<<num<<" "<<nbs<<" ";
  cout<<IntPatch_PolygoTool::DeflectionOverEstimation(L)<<endl;
  
  for(Standard_Integer i=1;i<=nbs;i++) {
    gp_Pnt2d P(IntPatch_PolygoTool::BeginOfSeg(L,i));
    cout<<"pnt2d "<<num<<"  "<< P.X()<<" "<<P.Y()<<endl;
  }
  gp_Pnt2d PF(IntPatch_PolygoTool::EndOfSeg(L,nbs));
  cout<<"pnt2d "<<num<<"  "<< PF.X()<<" "<<PF.Y()<<endl;
}


