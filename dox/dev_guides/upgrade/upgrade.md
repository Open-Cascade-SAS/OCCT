Upgrade from older OCCT versions  {#occt_dev_guides__upgrade}
================================

@tableofcontents

@section occt_upgrade_intro Introduction

This document provides technical details on upgrading user applications using previous versions of OCCT, to the current one.

@subsection occt_upgrade_700 Upgrade to OCCT 7.0.0

@subsubsection occt_upgrade_700_persist Removal of legacy persistence

Legacy persistence for shapes and OCAF data based on Storage_Schema (toolkits TKShapeShcema, TLStdLSchema, TKStdSchema, TKXCAFSchema) has been removed in OCCT 7.0.0.
Applications that used these persistence tools for their data need to be updated to use other persistence mechanisms.

The existing data files in standard formats can be converted using OCCT 6.9.0 or previous, as follows.

#### CSFDB files

Files in CSFDB format (usually with extension .csfdb) contain OCCT shape data that can be converted to BRep format. 
The easiest way to do that is to use ImportExport sample provided with OCCT 6.9.0 (or earlier):

- Start ImportExport sample
- Select File / New
- Select File / Import / CSFDB... and specify the file to be converted
- Drag mouse with right button pressed across the view to select all shapes by rectangle
- Select File / Export / BREP... and specify location and name for the resulting file

#### OCAF and XCAF documents

Files containing OCAF data saved in old format usually have extensions .std or .sgd or .dxc (XDE documents).
These files can be converted to XML or binary OCAF formats using DRAW Test Harness commands available in OCCT 6.9.0 or earlier.

For that, start *DRAWEXE* and perform the following commands: 

  * for the conversion of the "*.std" and "*.sgd" file formats to the binary format "*.cbf" (Created document should be in **BinOcaf** format instead of **MDTV-Standard**):

  @code
  Draw[]> pload ALL
  Draw[]> Open [path to *.std or *.sgd file] Doc
  Draw[]> Format Doc BinOcaf
  Draw[]> SaveAs Doc [path to the new file]
  @endcode

  * for the conversion of the "*.dxc" file format to the binary format "*.xbf" (Created document should be in **BinXCAF** format instead of **MDTV-XCAF**):

  @code
  Draw[]> pload ALL
  Draw[]> XOpen [path to *.dxc file] Doc
  Draw[]> Format Doc BinXCAF
  Draw[]> XSave Doc [path to the new file]
  @endcode

On Windows, be careful to replace back slashes in the file path by either direct slash or pairs of back slashes.
Use "XmlOcaf" or "XmlXCAF" instead of "BinOcaf" and "BinXCAF", respectively, to save in XML format instead of binary one.

