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


package SampleHLRJni;

import jcas.Object;
import jcas.Standard_Integer;
import CASCADESamplesJni.Quantity_Color;
import CASCADESamplesJni.Quantity_NameOfColor;
import jcas.Standard_Short;
import CASCADESamplesJni.Aspect_ColorMapEntry;


public class Aspect_ColorRampColorMap extends CASCADESamplesJni.Aspect_ColorMap {

 static {
    System.loadLibrary("SampleHLRJni");
 }
            
public Aspect_ColorRampColorMap(int basepixel,int dimension,Quantity_Color color) {
   Aspect_ColorRampColorMap_Create_1(basepixel,dimension,color);
}

private final native void Aspect_ColorRampColorMap_Create_1(int basepixel,int dimension,Quantity_Color color);

public Aspect_ColorRampColorMap(int basepixel,int dimension,short colorName) {
   Aspect_ColorRampColorMap_Create_2(basepixel,dimension,colorName);
}

private final native void Aspect_ColorRampColorMap_Create_2(int basepixel,int dimension,short colorName);

native public final void ColorRampDefinition(Standard_Integer basepixel,Standard_Integer dimension,Quantity_Color color);
native public final int FindColorMapIndex(int ColorMapEntryIndex);
native public final Aspect_ColorMapEntry FindEntry(int AColorMapEntryIndex);
native public final int NearestColorMapIndex(Quantity_Color aColor);
native public final Aspect_ColorMapEntry NearestEntry(Quantity_Color aColor);
public Aspect_ColorRampColorMap() {
}




}
