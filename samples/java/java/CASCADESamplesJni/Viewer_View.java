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
import jcas.Standard_Boolean;
import jcas.Standard_Integer;
import jcas.Standard_Real;


public class Viewer_View extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public  void Update();
native public final boolean SetImmediateUpdate(boolean onoff);
native public  void WindowFit(int Xmin,int Ymin,int Xmax,int Ymax);
native public  void Place(int x,int y,double aZoomFactor);
public Viewer_View() {
}




}
