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
import jcas.Standard_ShortReal;
import jcas.Standard_Boolean;


public class TShort_Array1OfShortReal extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TShort_Array1OfShortReal(int Low,int Up) {
   TShort_Array1OfShortReal_Create_1(Low,Up);
}

private final native void TShort_Array1OfShortReal_Create_1(int Low,int Up);

public TShort_Array1OfShortReal(float Item,int Low,int Up) {
   TShort_Array1OfShortReal_Create_2(Item,Low,Up);
}

private final native void TShort_Array1OfShortReal_Create_2(float Item,int Low,int Up);

native public final void Init(float V);
native public final void Destroy();
native public final boolean IsAllocated();
native public final TShort_Array1OfShortReal Assign(TShort_Array1OfShortReal Other);
native public final int Length();
native public final int Lower();
native public final int Upper();
native public final void SetValue(int Index,float Value);
native public final float Value(int Index);
native public final float ChangeValue(int Index);
public TShort_Array1OfShortReal() {
}



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
