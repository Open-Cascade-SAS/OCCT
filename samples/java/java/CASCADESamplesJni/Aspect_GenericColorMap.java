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
import CASCADESamplesJni.Aspect_ColorMapEntry;
import jcas.Standard_Integer;
import CASCADESamplesJni.Quantity_Color;


public class Aspect_GenericColorMap extends CASCADESamplesJni.Aspect_ColorMap {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
public Aspect_GenericColorMap() {
   Aspect_GenericColorMap_Create_0();
}

private final native void Aspect_GenericColorMap_Create_0();

final public void AddEntry(Aspect_ColorMapEntry AnEntry) {
    Aspect_GenericColorMap_AddEntry_1(AnEntry);
}

private final native void Aspect_GenericColorMap_AddEntry_1(Aspect_ColorMapEntry AnEntry);

final public int AddEntry(Quantity_Color aColor) {
   return Aspect_GenericColorMap_AddEntry_2(aColor);
}

private final native int Aspect_GenericColorMap_AddEntry_2(Quantity_Color aColor);

native public final void RemoveEntry(int AColorMapEntryIndex);
native public final int FindColorMapIndex(int AColorMapEntryIndex);
native public final Aspect_ColorMapEntry FindEntry(int AColorMapEntryIndex);
native public final int NearestColorMapIndex(Quantity_Color aColor);
native public final Aspect_ColorMapEntry NearestEntry(Quantity_Color aColor);



}
