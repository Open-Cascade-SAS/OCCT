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


package SampleHLRJni;

import CASCADESamplesJni.Aspect_GenericColorMap;
import CASCADESamplesJni.Aspect_TypeMap;
import CASCADESamplesJni.Aspect_WidthMap;
import CASCADESamplesJni.Aspect_FontMap;
import CASCADESamplesJni.Aspect_MarkMap;


public class V2d_DefaultMap extends jcas.Object {

 static {
    System.loadLibrary("SampleHLRJni");
 }
            
native public static Aspect_GenericColorMap ColorMap();
native public static Aspect_TypeMap TypeMap();
native public static Aspect_WidthMap WidthMap();
native public static Aspect_FontMap FontMap();
native public static Aspect_MarkMap MarkMap();
public V2d_DefaultMap() {
}



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
