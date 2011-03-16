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
import CASCADESamplesJni.V3d_Viewer;
import CASCADESamplesJni.V3d_PerspectiveView;


public class V3d_OrthographicView extends CASCADESamplesJni.V3d_View {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public V3d_OrthographicView(V3d_Viewer VM) {
   V3d_OrthographicView_Create_1(VM);
}

private final native void V3d_OrthographicView_Create_1(V3d_Viewer VM);

public V3d_OrthographicView(V3d_Viewer VM,V3d_PerspectiveView V) {
   V3d_OrthographicView_Create_2(VM,V);
}

private final native void V3d_OrthographicView_Create_2(V3d_Viewer VM,V3d_PerspectiveView V);

public V3d_OrthographicView(V3d_Viewer VM,V3d_OrthographicView V) {
   V3d_OrthographicView_Create_3(VM,V);
}

private final native void V3d_OrthographicView_Create_3(V3d_Viewer VM,V3d_OrthographicView V);

native public final V3d_OrthographicView Copy();
public V3d_OrthographicView() {
}




}
