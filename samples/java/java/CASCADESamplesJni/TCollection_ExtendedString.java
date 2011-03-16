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



public class TCollection_ExtendedString extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public TCollection_ExtendedString() {
}



public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
