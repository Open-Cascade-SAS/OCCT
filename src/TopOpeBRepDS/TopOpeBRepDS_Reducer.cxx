#include <TopOpeBRepDS_Reducer.ixx>
#include <TopOpeBRepDS_EIR.hxx>
#include <TopOpeBRepDS_FIR.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>

//=======================================================================
//function : TopOpeBRepDS_Reducer
//purpose  : 
//=======================================================================
TopOpeBRepDS_Reducer::TopOpeBRepDS_Reducer
(const Handle(TopOpeBRepDS_HDataStructure)& HDS) : myHDS(HDS)
{}

//=======================================================================
//function : ProcessEdgeInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessEdgeInterferences()
{
  TopOpeBRepDS_EIR eir(myHDS);
  eir.ProcessEdgeInterferences();
}

//=======================================================================
//function : ProcessFaceInterferences
//purpose  : 
//=======================================================================
void TopOpeBRepDS_Reducer::ProcessFaceInterferences
(const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& M)
{
  //modified by NIZHNY-MZV  Tue Nov 16 16:12:15 1999
  //FUN_ds_FEIGb1TO0(myHDS,M); //xpu250199

  TopOpeBRepDS_FIR fir(myHDS);
  fir.ProcessFaceInterferences(M);
}
