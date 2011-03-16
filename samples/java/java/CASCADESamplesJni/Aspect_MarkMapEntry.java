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
import CASCADESamplesJni.Aspect_MarkerStyle;
import jcas.Standard_Boolean;


public class Aspect_MarkMapEntry extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_MarkMapEntry() {
   Aspect_MarkMapEntry_Create_1();
}

private final native void Aspect_MarkMapEntry_Create_1();

public Aspect_MarkMapEntry(int index,Aspect_MarkerStyle style) {
   Aspect_MarkMapEntry_Create_2(index,style);
}

private final native void Aspect_MarkMapEntry_Create_2(int index,Aspect_MarkerStyle style);

public Aspect_MarkMapEntry(Aspect_MarkMapEntry entry) {
   Aspect_MarkMapEntry_Create_3(entry);
}

private final native void Aspect_MarkMapEntry_Create_3(Aspect_MarkMapEntry entry);

final public void SetValue(int index,Aspect_MarkerStyle style) {
    Aspect_MarkMapEntry_SetValue_1(index,style);
}

private final native void Aspect_MarkMapEntry_SetValue_1(int index,Aspect_MarkerStyle style);

final public void SetValue(Aspect_MarkMapEntry entry) {
    Aspect_MarkMapEntry_SetValue_2(entry);
}

private final native void Aspect_MarkMapEntry_SetValue_2(Aspect_MarkMapEntry entry);

native public final void SetStyle(Aspect_MarkerStyle Style);
native public final Aspect_MarkerStyle Style();
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
