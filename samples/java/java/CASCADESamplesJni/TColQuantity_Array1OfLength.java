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
import jcas.Standard_Real;
import jcas.Standard_Boolean;


public class TColQuantity_Array1OfLength extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TColQuantity_Array1OfLength(int Low,int Up) {
   TColQuantity_Array1OfLength_Create_1(Low,Up);
}

private final native void TColQuantity_Array1OfLength_Create_1(int Low,int Up);

public TColQuantity_Array1OfLength(double Item,int Low,int Up) {
   TColQuantity_Array1OfLength_Create_2(Item,Low,Up);
}

private final native void TColQuantity_Array1OfLength_Create_2(double Item,int Low,int Up);

native public final void Init(double V);
native public final void Destroy();
native public final boolean IsAllocated();
native public final TColQuantity_Array1OfLength Assign(TColQuantity_Array1OfLength Other);
native public final int Length();
native public final int Lower();
native public final int Upper();
native public final void SetValue(int Index,double Value);
native public final double Value(int Index);
native public final double ChangeValue(int Index);
public TColQuantity_Array1OfLength() {
}



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
