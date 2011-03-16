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

import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_FontStyle;
import jcas.Standard_Boolean;


public class Aspect_FontMapEntry extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_FontMapEntry() {
   Aspect_FontMapEntry_Create_1();
}

private final native void Aspect_FontMapEntry_Create_1();

public Aspect_FontMapEntry(int index,Aspect_FontStyle style) {
   Aspect_FontMapEntry_Create_2(index,style);
}

private final native void Aspect_FontMapEntry_Create_2(int index,Aspect_FontStyle style);

public Aspect_FontMapEntry(Aspect_FontMapEntry entry) {
   Aspect_FontMapEntry_Create_3(entry);
}

private final native void Aspect_FontMapEntry_Create_3(Aspect_FontMapEntry entry);

final public void SetValue(int index,Aspect_FontStyle style) {
    Aspect_FontMapEntry_SetValue_1(index,style);
}

private final native void Aspect_FontMapEntry_SetValue_1(int index,Aspect_FontStyle style);

final public void SetValue(Aspect_FontMapEntry entry) {
    Aspect_FontMapEntry_SetValue_2(entry);
}

private final native void Aspect_FontMapEntry_SetValue_2(Aspect_FontMapEntry entry);

native public final void SetType(Aspect_FontStyle Style);
native public final Aspect_FontStyle Type();
native public final void SetIndex(int index);
native public final int Index();
native public final void Free();
native public final boolean IsAllocated();
native public final void Dump();


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
