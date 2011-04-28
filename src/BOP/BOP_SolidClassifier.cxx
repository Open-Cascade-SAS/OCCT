// File:	BOP_SolidClassifier.cxx
// NIZNHY-PKV Thu Apr 11 10:42:14 2002

#include <BOP_SolidClassifier.ixx>

#include <BRepClass3d_SolidClassifier.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOP_SolidClassifier::BOP_SolidClassifier()
{
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void BOP_SolidClassifier::Clear() 
{
  myPClassifier = NULL;
  myClassifierMap.Clear();
  myState = TopAbs_UNKNOWN;
  myShell.Nullify();
  mySolid.Nullify();
}

//=======================================================================
//function : LoadSolid
//purpose  : 
//=======================================================================
  void BOP_SolidClassifier::LoadSolid(const TopoDS_Solid& SOL) 
{
  Standard_Boolean found;

  found = myClassifierMap.Contains(SOL);
  if ( !found ) {
    myPClassifier = new BRepClass3d_SolidClassifier(SOL);
    myClassifierMap.Add(SOL, myPClassifier);
  }
  else {
    myPClassifier = myClassifierMap.ChangeFromKey(SOL);
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
  TopAbs_State BOP_SolidClassifier::Classify (const TopoDS_Solid& SOL, 
					      const gp_Pnt& P, 
					      const Standard_Real Tol)
{
  myPClassifier = NULL;
  myState = TopAbs_UNKNOWN;

  LoadSolid(SOL);

  if (myPClassifier == NULL) {
    return myState;
  }  

  myPClassifier->Perform(P,Tol);
  myState = myPClassifier->State();
  const TopoDS_Shape& fres = myPClassifier->Face();
  if (fres.IsNull()) {
    // NYI : in case of removal of EXTERNAL and INTERNAL faces by the
    // classifier BRepClass3d_SolidClassifier, process these faces
    // to generate state ON/Solid when the point is IN/face INTERNAL or EXTERNAL 
    return myState;
  }
  
  TopAbs_Orientation ofres;
  
  ofres = fres.Orientation();

  if      ( ofres == TopAbs_EXTERNAL ) {
    if      ( myState == TopAbs_IN ) {
      myState = TopAbs_OUT;
    }    
    else if ( myState == TopAbs_OUT ){
      myState = TopAbs_OUT;
    }    
    else if ( myState == TopAbs_ON ){
      myState = TopAbs_ON;
    }    
    else if ( myState == TopAbs_UNKNOWN ){
      myState = TopAbs_OUT;
    }
  }

  else if ( ofres == TopAbs_INTERNAL ) {
    if      ( myState == TopAbs_IN ) {
      myState = TopAbs_IN;
    }
    else if ( myState == TopAbs_OUT) {
      myState = TopAbs_IN;
    }
    else if ( myState == TopAbs_ON ) {
      myState = TopAbs_ON;
    }    
    else if ( myState == TopAbs_UNKNOWN ) {
      myState = TopAbs_IN;
    }
  }
  return myState;
}


//=======================================================================
//function : LoadShell
//purpose  : 
//=======================================================================
  void BOP_SolidClassifier::LoadShell(const TopoDS_Shell& SHE) 
{
  Standard_Boolean found;

  found = myClassifierMap.Contains (SHE);
  
  if ( !found ) {
    myBuilder.MakeSolid(mySolid);
    myBuilder.Add(mySolid,SHE);
    TopoDS_Shell* pshe = (TopoDS_Shell*)&SHE; 
    (*pshe).Free(Standard_True);  
    
    myPClassifier = new BRepClass3d_SolidClassifier(mySolid);
    myClassifierMap.Add(SHE, myPClassifier);
  }
  else {
    myPClassifier = myClassifierMap.ChangeFromKey(SHE);
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
  TopAbs_State BOP_SolidClassifier::Classify (const TopoDS_Shell& SHE, 
					      const gp_Pnt& P,
					      const Standard_Real Tol)
{
  myPClassifier = NULL;
  myState = TopAbs_UNKNOWN;

  LoadShell(SHE);
  //
  if (myPClassifier == NULL) {
    return myState;
  }
  
  myPClassifier->Perform(P,Tol);
  myState = myPClassifier->State();
  return myState;
}

//=======================================================================
//function : State
//purpose  : 
//=======================================================================
  TopAbs_State BOP_SolidClassifier::State() const
{
  return myState;
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
  void BOP_SolidClassifier::Destroy()
{
  Standard_Integer i, aNb;
  
  aNb=myClassifierMap.Extent();
  for (i=1; i<=aNb; ++i) {
    BRepClass3d_SolidClassifier* pC=myClassifierMap(i);
    delete pC;
  }
  myClassifierMap.Clear();
}
