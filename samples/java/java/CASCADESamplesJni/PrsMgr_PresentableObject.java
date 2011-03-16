// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package CASCADESamplesJni;

import jcas.Object;
import CASCADESamplesJni.PrsMgr_TypeOfPresentation3d;
import jcas.Standard_Short;
import jcas.Standard_Integer;
import CASCADESamplesJni.TColStd_ListOfInteger;
import CASCADESamplesJni.TopLoc_Location;
import jcas.Standard_Boolean;
import CASCADESamplesJni.Prs3d_Presentation;


public class PrsMgr_PresentableObject extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public final short TypeOfPresentation3d();
native public final void SetTypeOfPresentation(short aType);
final public void SetToUpdate(int aMode) {
    PrsMgr_PresentableObject_SetToUpdate_1(aMode);
}

private final native void PrsMgr_PresentableObject_SetToUpdate_1(int aMode);

final public void SetToUpdate() {
    PrsMgr_PresentableObject_SetToUpdate_2();
}

private final native void PrsMgr_PresentableObject_SetToUpdate_2();

native public final void ToBeUpdated(TColStd_ListOfInteger ListOfMode);
native public  void SetLocation(TopLoc_Location aLoc);
native public final boolean HasLocation();
native public final TopLoc_Location Location();
native public  void ResetLocation();
 public void UpdateLocation() {
    PrsMgr_PresentableObject_UpdateLocation_1();
}

private  native void PrsMgr_PresentableObject_UpdateLocation_1();

 public void UpdateLocation(Prs3d_Presentation P) {
    PrsMgr_PresentableObject_UpdateLocation_2(P);
}

private  native void PrsMgr_PresentableObject_UpdateLocation_2(Prs3d_Presentation P);

public PrsMgr_PresentableObject() {
}




}
