 Documentation  {#dev_guides__documentation}
=================

@section  OCCT_DM_SECTION_1 Introduction

This document provides practical guidenes for generation and editing of OCCT user documentation.

@section  OCCT_DM_SECTION_2 Prerequisites

<b>Tcl/Tk</b>
The lates version: http://www.tcl.tk/software/tcltk/download.html

<b>Doxygen</b> ( >= 1.8.4 ) 
The latest version: http://www.stack.nl/~dimitri/doxygen/download.html

<b>MathJax</b> (used for rendering math formulas in browser). Download it for local installation or use the MathJax Content Delivery Network.  \(read 
@htmlonly <a href="#OCCT_DM_SECTION_A_9">Formulas</a> @endhtmlonly paragraph for more detailed description\). 
The latest version: http://www.mathjax.org/download/

<b>MiKTeX</b> or equivalent tool (used for PDF document creation)
Latest version: http://miktex.org/download

**Note**: to generate pdf documentation with MiKTeX you should execute gen.bat with MiKTeX environment 
(e.g. into MiKTeX command promt)

@section OCCT_DM_SECTION_2_1 Documentation Generation

Run gendoc.bat from OCCT directory to generate all articles are defined in FILES.txt:

gendoc.bat options:

  * -html                : To generate HTML files (cannot be used with -pdf);
  * -pdf                 : To generate PDF files (cannot be used with -html);
  * -m=<modules_list>    : Specifies list of articles to generate. If it is not specified, all files, mentioned in FILES.txt are processed;
  * -l=<document_name>   : Specifies the article caption for a single document;
  * -h                   : Prints help message;
  * -v                   : Specifies the Verbose mode (info on all script actions is shown).

If you run the command without arguments (like example above) it will generate HTML documentation 
for all articles are defined into FILES.txt.

**Note**: the generation process generates PDF files for each article, 
but in html case it generates common Html page with references to the ones.

For generation of specific article you need:
  * have it's name with relative path (from \%OCCDIR\%/dox/ to the file) contained in FILES.txt 
  (is located into \%OCCDIR\%/dox/ directory).

@verbatim
devs_guid/documentation/documentation.md
@endverbatim

where documentation .md is name of article and devs_guid/documentation/ is relative path of it

  * use this name with -m option in the generation process:

@verbatim
% gen.bat -html -m=devs_guid/documentation/documentation.md
@endverbatim

Multiple files are separated with comma:

@verbatim
% gen.bat -html -m=MD_FILE_1,MD_FILE_2
@endverbatim

To sepcify a article name with -l option, use tcl list to prevent whitespaces incorrect interpretation:

@verbatim
% gen.bat -pdf -m=MD_FILE_1 -l=[list MD File 1 Label]
@endverbatim

@section  OCCT_DM_SECTION_3 Documentation Conventions

This section contains information about conventions in the field of OCCT documentation file format, 
structure of documentation directories, etc.

@subsection  OCCT_DM_SECTION_3_1 File Format

It is proposed to use MarkDown file format for easy maintainance of generic text documents. 
The MarkDown files have a "*.md" extension and are based on rules desribed in 
@htmlonly <a href="#OCCT_DM_SECTION_A">Document Syntax</a> @endhtmlonly section.

@subsection  OCCT_DM_SECTION_3_2 Directory Structure

@image html /dev_guides/documentation/images/documentation_image001.png
@image latex /dev_guides/documentation/images/documentation_image001.png

Every separate article has own folder if images are used in it. These images 
are stored into "images" subfolder.

If you want to use the same image for several articles, you can place the one into "dox/resources" folder.

**Note**: Every article can use any image that is used by others articles. To avoid incorrect image
displaying, use relative path to the image (starting from dox folder). For instance
@verbatim
@image html /dev_guides/snv/images/snv_image001.svg
@endverbatim

Result of generation of the documentation is:

%OCCT_DIR% / doc         - a folder for generated articles;
            * html/             - a directory for generated HTML pages;
            * pdf/              - a directory for generated PDF files.

@section  OCCT_DM_SECTION_4 Adding a New Article

 - Place a new article into folder that is chosen taking into account the place of the article 
at the hierarchy of the documentation. For instance the article about "using SVN working with OCCT 
source code" (svn.md - the file of the article) might be placed into /dox/dev_guides/ . If the article has images then you may create
the own folder of the article and subfolder in it for images. For instance
*/dox/dev_guides/svn/ - for svn.md file
*/dox/dev_guides/svn/images/ - for images

 - Update dox/FILES.txt to add relative path to svn.md. For instance
@verbatim
dev_guides/snv/svn.md
@endverbatim

**Note**: the place of the relative path to an article is connected with the place
into treeview of html version.


Note, that you should specify a file tag, not the document name. 
See <a href="#OCCT_DM_SECTION_A_1">Header section</a> for details.

@section  OCCT_DOC_SECTION_5 Additional Resources

More information about OCCT can be found at http://www.opencascade.org

The information on formula syntax can be found at: 
http://en.wikipedia.org/wiki/Help:Displaying_a_formula

More information on MarkDown and Doxygen syntax can be found at:
http://www.stack.nl/~dimitri/doxygen/manual
