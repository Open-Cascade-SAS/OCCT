// File:	BRepGProp_Domain.cxx
// Created:	Tue Dec 13 08:58:57 2005
// Author:	Sergey KHROMOV
//		<skv@dimox>


#include <BRepGProp_Domain.ixx>

//=======================================================================
//function : Next
//purpose  : Sets the index of the arc iterator to the next arc of curve.
//=======================================================================

void BRepGProp_Domain::Next () {

  // skip INTERNAL and EXTERNAL edges

  myExplorer.Next();
  while (myExplorer.More()) {
    TopAbs_Orientation Or = myExplorer.Current().Orientation();
    if ((Or == TopAbs_FORWARD) || (Or == TopAbs_REVERSED)) return;
    myExplorer.Next();
  }
}
