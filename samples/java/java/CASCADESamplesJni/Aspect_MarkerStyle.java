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

import CASCADESamplesJni.Aspect_TypeOfMarker;
import jcas.Standard_Short;
import CASCADESamplesJni.TColStd_Array1OfReal;
import CASCADESamplesJni.TColStd_Array1OfBoolean;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;
import jcas.Standard_Real;
import CASCADESamplesJni.TShort_Array1OfShortReal;


public class Aspect_MarkerStyle extends jcas.Object {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_MarkerStyle() {
   Aspect_MarkerStyle_Create_1();
}

private final native void Aspect_MarkerStyle_Create_1();

public Aspect_MarkerStyle(short aType) {
   Aspect_MarkerStyle_Create_2(aType);
}

private final native void Aspect_MarkerStyle_Create_2(short aType);

public Aspect_MarkerStyle(TColStd_Array1OfReal aXpoint,TColStd_Array1OfReal aYpoint) {
   Aspect_MarkerStyle_Create_3(aXpoint,aYpoint);
}

private final native void Aspect_MarkerStyle_Create_3(TColStd_Array1OfReal aXpoint,TColStd_Array1OfReal aYpoint);

public Aspect_MarkerStyle(TColStd_Array1OfReal aXpoint,TColStd_Array1OfReal aYpoint,TColStd_Array1OfBoolean aSpoint) {
   Aspect_MarkerStyle_Create_4(aXpoint,aYpoint,aSpoint);
}

private final native void Aspect_MarkerStyle_Create_4(TColStd_Array1OfReal aXpoint,TColStd_Array1OfReal aYpoint,TColStd_Array1OfBoolean aSpoint);

native public final Aspect_MarkerStyle Assign(Aspect_MarkerStyle Other);
native public final short Type();
native public final int Length();
native public final boolean Values(int aRank,Standard_Real aX,Standard_Real aY);
native public final TShort_Array1OfShortReal XValues();
native public final TShort_Array1OfShortReal YValues();
native public final TColStd_Array1OfBoolean SValues();
native public final boolean IsEqual(Aspect_MarkerStyle Other);
native public final boolean IsNotEqual(Aspect_MarkerStyle Other);


public native static void FinalizeValue(long anHID);

public void finalize() {
   synchronized(myCasLock) {
     if ( aVirer != 0 ) FinalizeValue(HID);
     HID = 0;
   }
}

}
