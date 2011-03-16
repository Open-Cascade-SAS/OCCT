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
import jcas.Standard_Boolean;


public class TColStd_Array1OfBoolean extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TColStd_Array1OfBoolean(int Low,int Up) {
   TColStd_Array1OfBoolean_Create_1(Low,Up);
}

private final native void TColStd_Array1OfBoolean_Create_1(int Low,int Up);

public TColStd_Array1OfBoolean(boolean Item,int Low,int Up) {
   TColStd_Array1OfBoolean_Create_2(Item,Low,Up);
}

private final native void TColStd_Array1OfBoolean_Create_2(boolean Item,int Low,int Up);

native public final void Init(boolean V);
native public final void Destroy();
native public final boolean IsAllocated();
native public final TColStd_Array1OfBoolean Assign(TColStd_Array1OfBoolean Other);
native public final int Length();
native public final int Lower();
native public final int Upper();
native public final void SetValue(int Index,boolean Value);
native public final boolean Value(int Index);
native public final boolean ChangeValue(int Index);
public TColStd_Array1OfBoolean() {
}



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
