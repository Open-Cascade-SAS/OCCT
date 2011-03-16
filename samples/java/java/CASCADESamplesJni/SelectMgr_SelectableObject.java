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
import jcas.Standard_Integer;
import CASCADESamplesJni.SelectMgr_Selection;
import jcas.Standard_Boolean;
import CASCADESamplesJni.Prs3d_Presentation;


public class SelectMgr_SelectableObject extends CASCADESamplesJni.PrsMgr_PresentableObject {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public  int NbPossibleSelection();
final public void UpdateSelection() {
    SelectMgr_SelectableObject_UpdateSelection_1();
}

private final native void SelectMgr_SelectableObject_UpdateSelection_1();

final public void UpdateSelection(int aMode) {
    SelectMgr_SelectableObject_UpdateSelection_2(aMode);
}

private final native void SelectMgr_SelectableObject_UpdateSelection_2(int aMode);

native public final void AddSelection(SelectMgr_Selection aSelection,int aMode);
native public final void ClearSelections();
native public final SelectMgr_Selection Selection(int aMode);
native public final boolean HasSelection(int aMode);
native public final void Init();
native public final boolean More();
native public final void Next();
native public final SelectMgr_Selection CurrentSelection();
native public final void ResetLocation();
 public void UpdateLocation() {
    SelectMgr_SelectableObject_UpdateLocation_1();
}

private  native void SelectMgr_SelectableObject_UpdateLocation_1();

 public void UpdateLocation(Prs3d_Presentation P) {
    SelectMgr_SelectableObject_UpdateLocation_2(P);
}

private  native void SelectMgr_SelectableObject_UpdateLocation_2(Prs3d_Presentation P);

public SelectMgr_SelectableObject() {
}




}
