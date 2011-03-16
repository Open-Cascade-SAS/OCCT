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
import CASCADESamplesJni.Aspect_WidthOfLine;
import jcas.Standard_Short;
import jcas.Standard_Real;
import jcas.Standard_Boolean;


public class Aspect_WidthMapEntry extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_WidthMapEntry() {
   Aspect_WidthMapEntry_Create_1();
}

private final native void Aspect_WidthMapEntry_Create_1();

public Aspect_WidthMapEntry(int index,short style) {
   Aspect_WidthMapEntry_Create_2(index,style);
}

private final native void Aspect_WidthMapEntry_Create_2(int index,short style);

public Aspect_WidthMapEntry(int index,double width) {
   Aspect_WidthMapEntry_Create_3(index,width);
}

private final native void Aspect_WidthMapEntry_Create_3(int index,double width);

public Aspect_WidthMapEntry(Aspect_WidthMapEntry entry) {
   Aspect_WidthMapEntry_Create_4(entry);
}

private final native void Aspect_WidthMapEntry_Create_4(Aspect_WidthMapEntry entry);

final public void SetValue(int index,short style) {
    Aspect_WidthMapEntry_SetValue_1(index,style);
}

private final native void Aspect_WidthMapEntry_SetValue_1(int index,short style);

final public void SetValue(int index,double width) {
    Aspect_WidthMapEntry_SetValue_2(index,width);
}

private final native void Aspect_WidthMapEntry_SetValue_2(int index,double width);

final public void SetValue(Aspect_WidthMapEntry entry) {
    Aspect_WidthMapEntry_SetValue_3(entry);
}

private final native void Aspect_WidthMapEntry_SetValue_3(Aspect_WidthMapEntry entry);

native public final void SetIndex(int index);
native public final void SetType(short Style);
native public final void SetWidth(double Width);
native public final short Type();
native public final double Width();
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
