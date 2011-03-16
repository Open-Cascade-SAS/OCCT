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

import CASCADESamplesJni.Quantity_Color;


public class Aspect_Background extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_Background() {
   Aspect_Background_Create_1();
}

private final native void Aspect_Background_Create_1();

public Aspect_Background(Quantity_Color AColor) {
   Aspect_Background_Create_2(AColor);
}

private final native void Aspect_Background_Create_2(Quantity_Color AColor);

native public final void SetColor(Quantity_Color AColor);
native public final Quantity_Color Color();


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
