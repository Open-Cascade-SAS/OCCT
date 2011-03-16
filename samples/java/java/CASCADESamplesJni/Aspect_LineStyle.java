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

import CASCADESamplesJni.Aspect_TypeOfLine;
import jcas.Standard_Short;
import CASCADESamplesJni.TColQuantity_Array1OfLength;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;


public class Aspect_LineStyle extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_LineStyle() {
   Aspect_LineStyle_Create_1();
}

private final native void Aspect_LineStyle_Create_1();

public Aspect_LineStyle(short Type) {
   Aspect_LineStyle_Create_2(Type);
}

private final native void Aspect_LineStyle_Create_2(short Type);

public Aspect_LineStyle(TColQuantity_Array1OfLength Style) {
   Aspect_LineStyle_Create_3(Style);
}

private final native void Aspect_LineStyle_Create_3(TColQuantity_Array1OfLength Style);

native public final Aspect_LineStyle Assign(Aspect_LineStyle Other);
final public void SetValues(short Type) {
    Aspect_LineStyle_SetValues_1(Type);
}

private final native void Aspect_LineStyle_SetValues_1(short Type);

final public void SetValues(TColQuantity_Array1OfLength Style) {
    Aspect_LineStyle_SetValues_2(Style);
}

private final native void Aspect_LineStyle_SetValues_2(TColQuantity_Array1OfLength Style);

native public final short Style();
native public final int Length();
native public final TColQuantity_Array1OfLength Values();
native public final boolean IsEqual(Aspect_LineStyle Other);
native public final boolean IsNotEqual(Aspect_LineStyle Other);


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
