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
import CASCADESamplesJni.Aspect_TypeOfColorMap;
import jcas.Standard_Integer;
import CASCADESamplesJni.Aspect_ColorMapEntry;
import CASCADESamplesJni.Quantity_Color;


public class Aspect_ColorMap extends CASCADESamplesJni.MMgt_TShared {

 static {
    System.loadLibrary("CASCADESamplesJni");
 }
            
native public final short Type();
native public final int Size();
native public final int Index(int aColormapIndex);
native public final void Dump();
native public final Aspect_ColorMapEntry Entry(int AColorMapIndex);
native public  int FindColorMapIndex(int AColorMapEntryIndex);
native public  Aspect_ColorMapEntry FindEntry(int AColorMapEntryIndex);
native public  int NearestColorMapIndex(Quantity_Color aColor);
native public  Aspect_ColorMapEntry NearestEntry(Quantity_Color aColor);
public Aspect_ColorMap() {
}




}
