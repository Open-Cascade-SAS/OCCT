# STEP express schema to OCCT classes 

## 1. Introduction

**ExpToCasExe** is an auxiliary tool to generate code for implementation
of new STEP entities into OCCT.
This tool consists of two packages: **ExpToCasExe** and **TKExpress**.

*ExpToCasExe* package is the basic package for generation.
It parses the express schema, makes a list of entities described in the schema, 
and starts generating classes according to the specified lists.
This package has the file *occt_existed_step_entities.lst* witch contains list
of STEP entities implemented in *OCCT* at this moment.

*TKExpress* package generates files with description of the STEP entities
to use in *OCCT* taking into account their dependencies.
Below is a set of generated files using the example
of the *Condition* entity from the *StepAP214* package.

* **StepAP214_Condition.hxx** - contains declaration of the class that describes STEP entity
* **StepAP214_Condition.cxx** - contains definition of the class that describes STEP entity
* **RWStepAP214_RWCondition.hxx** - contains declaration of the class that reads (writes) STEP entity from STEP file
* **RWStepAP214_RWCondition.cxx** - contains definition of the class that reads (writes) STEP entity from STEP file
* **category.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_GeneralModule.cxx* file
* **fillshared.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_GeneralModule.cxx* file
* **inc.txt** - common file for all generated entities with part of code to insert to *StepAP214_Protocol.cxx*
* **newvoid.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_GeneralModule.cxx* file
* **protocol.txt** - common file for all generated entities with part of code to insert to *StepAP214_Protocol.cxx* file
* **readstep.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file
* **reco.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file
* **rwinc.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file
* **steptype.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file
* **typebind.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file
* **writestep.txt** - common file for all generated entities with part of code to insert to *RWStepAP214_ReadWriteModule.cxx* file

## 2. Build

The build process is automated, but has some specifics. *ExpToCasExe* package contains file *exptocas.lex*
which contains rules for lexical analyzer builder (*FLEX*), and file *exptocas.yacc*
which contains rules for syntax analyzer builder (*GNU Bison*).

During build process *FLEX* and *Bison* generate the next files:

 * FlexLexer.h
 * lex.exptocas.cxx
 * exptocas.tab.cxx
 * exptocas.tab.hxx

These files are placed to the source directory ("*src/ExpToCasExe*") due to *CMAKE* script specific.
Then standard Visual Studio build process continues.

There is no necessary to include these files to the *git* repository, due to automatic generation.
To change the behaviour of Express schema parser, the user need to modify original files:
*exptocas.lex* and *exptocas.yacc*.

## 3. Usage

**ExpToCasExe** program has the following call pattern:

~~~~
> ExpToCasExe <schema.exp> [<new.lst> [<existed.lst> [start_index]]]
~~~~

where:

 * **schema.exp** - file with STEP Express schema
 * **new.lst** - file with list of new STEP entities which we have to generate
 * **existed.lst** - file with list of already implemented STEP entities
 * **start_index** - a first index for entity registration
      (value of it you can receive after analyse of *StepAP214_Protocol.cxx* file)

If *new.lst* file is set to "-" then all entities from Express schema are considered as new.

*new.lst* and *existed.lst* are the text files. Line in file describes the class name
of STEP entity and to witch *OCCT* package it's belong. Line has the following format: 

~~~~
item_name package_name [shortname [check_flag(0 or 1) [fillshared_flag(0 or 1) [category]]]]
~~~~

where:

 * **item_name** is the class name of the STEP entity (*ProductDefinitionOrReference* for example)
 * **package_name** is the name of the *OCCT* package (*StepBasic* for example)
 * **shortname** is the short name of the STEP entity
 * **check_flag** is the flag to generate the *Check* function in the class
 * **fillshared_flag** is the flag to generate the *FillShared* function in the class
 * **category** is the name of category to witch entity belongs to

These file can include comments after the symbol "#".

The result of generation consists of several folders:

* two folders (*\_package_name\_* and *RW\_package_name\_*) for each package for which new entities were generated.
* a folder *Registration* with *\*.txt* files with code to copy into common files (see the example in the first section).

For generation, it is easy to copy to separate directory following files:

* **ExpToCasExe.exe**, **TKernel.dll** and **TKExpress.dll** from OCCT
* STEP Express schema
* **occt_existed_step_entities.lst** from package *ExpToCasExe*

create a file with needed entities **new.lst** and from this directory execute command:

~~~~
> ExpToCasExe.exe schema.exp new.lst occt_existed_step_entities.lst 1000
~~~~

## 4. Disclaimer

**NOTE**:
Some STEP schemes may be not fully corresponded to the rules which set in our generator.
If during generation we receive a message about syntax error you can try to edit schema
file in order to avoid it. Such approach is reasonable if number of errors is small.
If there are many errors we have to change existed rules (for debug parser process you can
uncomment string "//aScanner.set_debug(1);" in the file *exptocas.yacc*).
TIP: remove upper case from name after SCHEMA key word.

**NOTE**:
Please, take into account that most of existed entities were generated many years ago.
Some of them can be not corresponded to last versions of schemes.

For example, we don't have classes for entity:

~~~~
  ENTITY founded_item
    SUPERTYPE OF ((ONEOF(b_spline_curve_knot_locator, b_spline_curve_segment, b_spline_surface_knot_locator,
                         b_spline_surface_patch, b_spline_surface_strip,
                         boundary_curve_of_b_spline_or_rectangular_composite_surface, box_domain,
                         character_glyph_style_outline, character_glyph_style_stroke, composite_curve_segment,
                         composite_curve_transition_locator, curve_style, curve_style_font, curve_style_font_and_scaling,
                         curve_style_font_pattern, externally_defined_style, fill_area_style,
                         interpolated_configuration_segment, kinematic_path_segment, plane_angle_and_length_pair,
                         plane_angle_and_ratio_pair, point_style, presentation_style_assignment,
                         rectangular_composite_surface_transition_locator, surface_patch, surface_side_style,
                         surface_style_boundary, surface_style_control_grid, surface_style_fill_area,
                         surface_style_parameter_line, surface_style_segmentation_curve, surface_style_silhouette,
                         surface_style_usage, symbol_style, text_style, view_volume)) ANDOR
                  (ONEOF(character_glyph_style_outline, character_glyph_style_stroke, curve_style, curve_style_font,
                         curve_style_font_and_scaling, curve_style_font_pattern, externally_defined_style,
                         fill_area_style, point_style, presentation_style_assignment, surface_side_style,
                         surface_style_boundary, surface_style_control_grid, surface_style_fill_area,
                         surface_style_parameter_line, surface_style_segmentation_curve, surface_style_silhouette,
                         surface_style_usage, symbol_style, text_style)));
  DERIVE
    users : SET [0 : ?] OF founded_item_select := using_items(SELF, []);
  WHERE
    WR1: SIZEOF(users) > 0;
    WR2: NOT (SELF IN users);
  END_ENTITY;
~~~~

But some of existed entities (for example - *composite_curve_segment*) must be inherited from
this missing entity (at present such existed entities are inherited from *Standard_Transient*):

~~~~
  ENTITY composite_curve_segment
  SUBTYPE OF (founded_item);
    transition : transition_code;
    same_sense : BOOLEAN;
    parent_curve : curve;
  INVERSE
    using_curves : BAG[1:?] OF composite_curve FOR segments;
  WHERE
    wr1 : ('AUTOMOTIVE_DESIGN.BOUNDED_CURVE' IN TYPEOF(parent_curve));
  END_ENTITY;  -- 10303-42: geometry_schema 
~~~~

**NOTE**: To facilitate the work of those who will use this program after you,
it is strongly recommended update the file **occt_existed_step_entities.lst**
after each generation and not to forget to change update date in the first line of this file.
