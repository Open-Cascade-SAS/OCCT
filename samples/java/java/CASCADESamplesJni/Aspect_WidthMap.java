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

import jcas.Object;
import CASCADESamplesJni.Aspect_WidthMapEntry;
import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_WidthOfLine;
import jcas.Standard_Short;
import jcas.Standard_Real;


public class Aspect_WidthMap extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_WidthMap() {
   Aspect_WidthMap_Create_0();
}

private final native void Aspect_WidthMap_Create_0();

final public void AddEntry(Aspect_WidthMapEntry AnEntry) {
    Aspect_WidthMap_AddEntry_1(AnEntry);
}

private final native void Aspect_WidthMap_AddEntry_1(Aspect_WidthMapEntry AnEntry);

final public int AddEntry(short aStyle) {
   return Aspect_WidthMap_AddEntry_2(aStyle);
}

private final native int Aspect_WidthMap_AddEntry_2(short aStyle);

final public int AddEntry(double aStyle) {
   return Aspect_WidthMap_AddEntry_3(aStyle);
}

private final native int Aspect_WidthMap_AddEntry_3(double aStyle);

native public final int Size();
native public final int Index(int aWidthmapIndex);
native public final Aspect_WidthMapEntry Entry(int AnIndex);
native public final void Dump();



}
