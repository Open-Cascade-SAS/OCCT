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
import CASCADESamplesJni.Aspect_TypeMapEntry;
import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_LineStyle;


public class Aspect_TypeMap extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_TypeMap() {
   Aspect_TypeMap_Create_0();
}

private final native void Aspect_TypeMap_Create_0();

final public void AddEntry(Aspect_TypeMapEntry AnEntry) {
    Aspect_TypeMap_AddEntry_1(AnEntry);
}

private final native void Aspect_TypeMap_AddEntry_1(Aspect_TypeMapEntry AnEntry);

final public int AddEntry(Aspect_LineStyle aStyle) {
   return Aspect_TypeMap_AddEntry_2(aStyle);
}

private final native int Aspect_TypeMap_AddEntry_2(Aspect_LineStyle aStyle);

native public final int Size();
native public final int Index(int aTypemapIndex);
native public final void Dump();
native public final Aspect_TypeMapEntry Entry(int AnIndex);



}
