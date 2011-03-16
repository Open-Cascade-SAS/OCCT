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
import CASCADESamplesJni.Aspect_FontMapEntry;
import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_FontStyle;


public class Aspect_FontMap extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_FontMap() {
   Aspect_FontMap_Create_0();
}

private final native void Aspect_FontMap_Create_0();

final public void AddEntry(Aspect_FontMapEntry AnEntry) {
    Aspect_FontMap_AddEntry_1(AnEntry);
}

private final native void Aspect_FontMap_AddEntry_1(Aspect_FontMapEntry AnEntry);

final public int AddEntry(Aspect_FontStyle aStyle) {
   return Aspect_FontMap_AddEntry_2(aStyle);
}

private final native int Aspect_FontMap_AddEntry_2(Aspect_FontStyle aStyle);

native public final int Size();
native public final int Index(int aFontmapIndex);
native public final void Dump();
native public final Aspect_FontMapEntry Entry(int AnIndex);



}
