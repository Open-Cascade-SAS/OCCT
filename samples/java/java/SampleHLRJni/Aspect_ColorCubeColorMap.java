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
import CASCADESamplesJni.Aspect_ColorMapEntry;
import CASCADESamplesJni.Quantity_Color;


public class Aspect_ColorCubeColorMap extends CASCADESamplesJni.Aspect_ColorMap {

 static {
    System.loadLibrary("SampleHLRJni");
 }
            
public Aspect_ColorCubeColorMap(int base_pixel,int redmax,int redmult,int greenmax,int greenmult,int bluemax,int bluemult) {
   Aspect_ColorCubeColorMap_Create_0(base_pixel,redmax,redmult,greenmax,greenmult,bluemax,bluemult);
}

private final native void Aspect_ColorCubeColorMap_Create_0(int base_pixel,int redmax,int redmult,int greenmax,int greenmult,int bluemax,int bluemult);

native public final void ColorCubeDefinition(Standard_Integer base_pixel,Standard_Integer redmax,Standard_Integer redmult,Standard_Integer greenmax,Standard_Integer greenmult,Standard_Integer bluemax,Standard_Integer bluemult);
native public final int FindColorMapIndex(int AColorMapEntryIndex);
native public final Aspect_ColorMapEntry FindEntry(int AColorMapEntryIndex);
native public final int NearestColorMapIndex(Quantity_Color aColor);
native public final Aspect_ColorMapEntry NearestEntry(Quantity_Color aColor);
public Aspect_ColorCubeColorMap() {
}




}
