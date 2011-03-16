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

import CASCADESamplesJni.Standard_Storable;
import jcas.Standard_Integer;
import CASCADESamplesJni.Quantity_Color;
import jcas.Standard_Boolean;


public class Aspect_ColorMapEntry extends CASCADESamplesJni.Standard_Storable {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_ColorMapEntry() {
   Aspect_ColorMapEntry_Create_1();
}

private final native void Aspect_ColorMapEntry_Create_1();

public Aspect_ColorMapEntry(int index,Quantity_Color rgb) {
   Aspect_ColorMapEntry_Create_2(index,rgb);
}

private final native void Aspect_ColorMapEntry_Create_2(int index,Quantity_Color rgb);

public Aspect_ColorMapEntry(Aspect_ColorMapEntry entry) {
   Aspect_ColorMapEntry_Create_3(entry);
}

private final native void Aspect_ColorMapEntry_Create_3(Aspect_ColorMapEntry entry);

final public void SetValue(int index,Quantity_Color rgb) {
    Aspect_ColorMapEntry_SetValue_1(index,rgb);
}

private final native void Aspect_ColorMapEntry_SetValue_1(int index,Quantity_Color rgb);

final public void SetValue(Aspect_ColorMapEntry entry) {
    Aspect_ColorMapEntry_SetValue_2(entry);
}

private final native void Aspect_ColorMapEntry_SetValue_2(Aspect_ColorMapEntry entry);

native public final void SetColor(Quantity_Color rgb);
native public final Quantity_Color Color();
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
