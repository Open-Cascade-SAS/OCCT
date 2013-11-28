 Documentation System {#dev_guides__documentation}
======================

@tableofcontents

@section  OCCT_DM_SECTION_1 Introduction

This document provides practical guidenes for generation and editing of OCCT user documentation.

@section  OCCT_DM_SECTION_2 Prerequisites

<b>Tcl/Tk</b>
Version 8.5 or 8.6: http://www.tcl.tk/software/tcltk/download.html

<b>Doxygen</b> 
Version 1.8.4 or above: http://www.stack.nl/~dimitri/doxygen/download.html

<b>MathJax</b> (used for rendering math formulas in browser). 
See \ref OCCT_DM_SECTION_A_9 paragraph for more detailed description. 
The latest version: http://www.mathjax.org/download/

<b>MiKTeX</b> or equivalent tool (used for PDF document creation)

Latest version: http://miktex.org/download

**Note**: to generate pdf documentation with MiKTeX you should execute gendoc.bat within MiKTeX environment 
(run gendoc.bat in MiKTeX command promt or update PATH for MiKTeX bin folder). Also in process of pdf generation
MiKTeX can request you to download missing packages if MiKTeX was installed with option below:

@image html /dev_guides/documentation/images/documentation_image002.png
@image latex /dev_guides/documentation/images/documentation_image002.png

If this option is set to "Yes", MiKTeX will download missing packages automatically.

@section OCCT_DM_SECTION_2_1 Documentation Generation

Run gendoc.bat from OCCT directory to generate all articles are defined in FILES.txt:

gendoc.bat options:

  * -html                : To generate HTML files (cannot be used with -pdf);
  * -pdf                 : To generate PDF files (cannot be used with -html);
  * -m=\<modules_list\>    : Specifies list of articles to generate. If it is not specified, all files, mentioned in FILES.txt are processed;
  * -l=\<document_name\>   : Specifies the article caption for a single document;
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
% gendoc.bat -html -m=devs_guid/documentation/documentation.md
@endverbatim

Multiple files are separated with comma:

@verbatim
% gendoc.bat -html -m=MD_FILE_1,MD_FILE_2
@endverbatim

To sepcify a article name with -l option, use quotes to prevent incorrect interpretation of whitespaces:

@verbatim
% gendoc.bat -pdf -m=MD_FILE_1 -l="Label of MD_FILE_1 document"
@endverbatim

@section  OCCT_DM_SECTION_3 Documentation Conventions

This section contains information about conventions in the field of OCCT documentation file format, 
structure of documentation directories, etc.

@subsection  OCCT_DM_SECTION_3_1 File Format

The format used for documentation is MarkDown with Doxygen extensions. 
The MarkDown files have a "*.md" extension and are based on rules desribed in 
\ref OCCT_DM_SECTION_A section.

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

@section  OCCT_DM_SECTION_A Appendix 1: Document Syntax

Each OCCT document file in *.md format has a simple structure.
It can contain: 

| Content type      | Obligation            |
| :---------------- | :-------------------: |
| Header            | M                     |
| Footer            | M                     |
| Plain text        | O                     |
| List              | O                     |
| Table             | O                     |
| Code              | O                     |
| Formula           | O                     |
| Image             | O                     |
| Page numbers      | M (auto generation)   |
| Table of contents | M (auto generation)   |

The legend:

  * M is for Mandatory
  * O is for Optional

@subsection  OCCT_DM_SECTION_A_1 Text Caption (a header)

headings of different levels can be specified with the following code:

@verbatim
Header 1  {#header1}
=======
@endverbatim

  to get

 Header 1
=========

  and with the following code:

@verbatim
Header 2 {#header2}
--------
@endverbatim

to get 

Header 2
---------

Where a word in curly braces is a MarkDown-style reference, which can be used in table of contents.
If you would like to have the table of contents, it is recommended to use \@section, 
\@subsection and \@subsubsection pages instead of MarkDown headers as follows:

@verbatim
  @section Section_Name Section Header
  @subsection SubSection_Name SubSection Header
  @subsubsection SubSubSection_Name SubSubSection Header
@endverbatim

@subsection OCCT_DM_SECTION_A_2 Plain Text

Plain text is a text in a notepad-like format. To insert special symbols, 
like \< , \> or \\, prepend them with \\ character: \\\<, \\\>, \\\\ 
To emphasize some words, write one pair of asterisks ( * ) or underscores ( _ ) across the word 
to make it *italic* and two pairs of these symbols to make a word **Bold**.

@subsection OCCT_DM_SECTION_A_3 Lists

To create a bulleted list, start each line with a hyphen or an asterisk, 
followed by a space. List items can be nested. This code:

@verbatim
* Bullet 1
* Bullet 2
  - Bullet 2a
  - Bullet 2b
* Bullet 3
@endverbatim

produces this list:

* Bullet 1
* Bullet 2
  * Bullet 2a
  * Bullet 2b
* Bullet 3  

To create a numbered list, start each line with number and a period, 
then a space. Numbered lists can also be nested. Thus this code 

@verbatim
1. List item 1
   1. Sub-item 1
   2. Sub-item 2
2. List item 2
3. List item 3
@endverbatim

produces this list:

1. List item 1
   1. Sub-item 1
   2. Sub-item 2
2. List item 2
3. List item 3

Each list item can contain several paragraphs of text; these paragraphs must 
have the same indentation as text after bullet or number in the numbered list 
item (otherwise numbering will be broken). 

Code blocks can be inserted as paragraphs with additional indentation 
(4 spaces more). Note that fenced code blocks do not work within numbered lists
and their use may cause numeration to be reset.


Example of complex nested list:

@verbatim
1. ListItem_1

   Additional paragraph

       code fragment

   One more paragraph

   1. Sub-item 1

          code fragment for sub-item 1

   2. Sub-item 2

      Paragraph for sub-item 2

   Yet one more paragraph for list item 1

2. ListItem_2
@endverbatim

1. List item 1

   Additional paragraph

       code fragment

   One more paragraph

   1. Sub-item 1

          code fragment for sub-item 1

   2. Sub-item 2

      Paragraph for sub-item 2

   Yet one more paragraph for list item 1

2. List item 2

Note that numbers of paragraphs are regenerated so they do not necessarily 
follow numbering of source items.

@subsection  OCCT_DM_SECTION_A_4 Tables

A table consists of a header line, a separator line, and at least one row line. 
Table columns are separated by the pipe (|) character. The following example: 

@verbatim
First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell 
Content Cell  | Content Cell 
@endverbatim

  will produce the following table:

First Header | Second Header
------------ | -------------
Content Cell | Content Cell 
Content Cell | Content Cell 

Column alignment can be controlled via one or two colons at the header separator line: 

@verbatim
| Right | Center | Left  |
| ----: | :----: | :---- |
| 10    | 10     | 10    |
| 1000  | 1000   | 1000  |
@endverbatim

which will looks as follows:

| Right | Center | Left  |
| ----: | :----: | :---- |
| 10    | 10     | 10    |
| 1000  | 1000   | 1000  |

Note that each table raw should be contained in one line of text; complex
tables can be created using HTML tags.

@subsection  OCCT_DM_SECTION_A_5 Code Blocks

It is recommended to indent a code lines with 4 spaces.
A fenced code block does not require indentation, and is defined by a pair of "fence lines". 
Such line consists of 3 or more tilde (~) characters on a line. 
The end of the block should have the same number of tildes. Here is an example:

~~~~~~~~~~~~~~~~~~~~~~~
  a one-line code block
~~~~~~~~~~~~~~~~~~~~~~~

By default the output is the same as for a normal code block.
To highlight the code, the developer has to indicate the typical file extension, 
which corresponds to the programming language, after the opening fence. 
For highlighting according to the C++ language, for instance,  write the following code (the curly braces and dot are optional): 

@verbatim
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
int func(int a,int b) { return a*b; }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@endverbatim

which will produce:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp} 
int func(int a,int b) { return a*b; }
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Verbatim content can be written by using framing \@verbatim \@endverbatim . For instance

    verbatim text


@subsection  OCCT_DM_SECTION_A_6 References

To insert a reference to a website, it is proposed to write a URL. For example: http://en.wikipedia.org
To insert a reference to another part of the same document, the developer can write:

@verbatim
  @htmlonly 
    <a href="#OCCT_DOC_SECTION_5">Doxygen Configuration file</a>
  @endhtmlonly 
@endverbatim

to get a link to paragraph : @htmlonly <a href="#OCCT_DOC_SECTION_5">Doxygen configuration</a> @endhtmlonly 

@subsection  OCCT_DM_SECTION_A_7 Images

To insert image into document the developer can write the following code(in Doxygen-style):

For HTML document:
@verbatim
  @image html /relative/path/to/image/image001.png "Image caption"
@endverbatim

For latex document:
@verbatim
  @image latex /relative/path/to/image/image001.png "Image caption"
@endverbatim

*Note*: When markdown document is used to generate html document the latex insertion is ignored (and vice versa) 
due to this fact you can use image insertions in the pair, like example below:
@verbatim
  @image html /relative/path/to/image/image001.png "Image caption"
  @image latex /relative/path/to/image/image001.png "Image caption"
@endverbatim

The code below tells Doxygen to insert a picture right in the place this code was written:
@verbatim
  @image html /resources/occ_logo.png "OCCT logo"
  @image latex /resources/occ_logo.png "OCCT logo"
@endverbatim

@image html /resources/occ_logo.png "OCCT logo"
@image latex /resources/occ_logo.png "OCCT logo"
 
@subsection  OCCT_DM_SECTION_A_8 Table Of Contents

To get the table of contents at the beginning of the document, write \@tableofcontents tag. 
But it is not needed now because TreeView option for HTML is used.
The TOC in the PDF document will be generated automatically.

@subsection  OCCT_DM_SECTION_A_9 Formulas

Formulas within documents will be generated using MathJax tool.

A developer has to specify these parameters in Doxyfile to enable support of MathJax in Doxygen:

    USE_MATHJAX         = YES
    MATHJAX_FORMAT      = HTML-CSS
    MATHJAX_RELPATH     = http://cdn.mathjax.org/mathjax/latest
    MATHJAX_EXTENSIONS  = TeX/AMSmath TeX/AMSsymbols

To use MathJax tool with the HTML page, it's \<head\> block has to contain 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.html}
    <script type="text/x-mathjax-config">
      MathJax.Hub.Config({
        tex2jax: {inlineMath: [["$","$"],["\\(","\\)"]]},
        displayAlign: "left"
      });
    </script>

    <script type="text/javascript"
      src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
    </script>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

First script configures MathJax to understand separator types and to left allign formulas. 
The second script inserts reference to MathJax tool. 
This tool will always be used when the HTML output will be shown.

Equations can be written by several ways:

1.Unnumbered displayed formulas that are centered on a separate line. 
These formulas should be put between \@f\[ and \@f\] tags. An example: 

@verbatim
@f[
    |I_2|=\left| \int_{0}^T \psi(t)
            \left\{ 
                u(a,t)-
                \int_{\gamma(t)}^a 
                \frac{d\theta}{k(\theta,t)}
                \int_{a}^\theta c(\xi)u_t(\xi,t)\,d\xi
            \right\} dt
        \right|
@f]
@endverbatim

gives the following result:

   @f$
       |I_2|=\left| \int_{0}^T \psi(t)
               \left\{ 
                   u(a,t)-
                   \int_{\gamma(t)}^a 
                   \frac{d\theta}{k(\theta,t)}
                   \int_{a}^\theta c(\xi)u_t(\xi,t)\,d\xi
               \right\} dt
           \right|
   @f$
   
2.Formulas can also be put between @verbatim \begin{align} @endverbatim and @verbatim \end{align} @endverbatim tags. An example: 

@verbatim
  \begin{align}
  \dot{x} & = \sigma(y-x) \\
  \dot{y} & = \rho x - y - xz \\
  \dot{z} & = -\beta z + xy
  \end{align}
@endverbatim

  gives the following result:
@latexonly
  \begin{align}
  \dot{x} & = \sigma(y-x) \\
  \dot{y} & = \rho x - y - xz \\
  \dot{z} & = -\beta z + xy
  \end{align}
@endlatexonly

@htmlonly
  \begin{align}
  \dot{x} & = \sigma(y-x) \\
  \dot{y} & = \rho x - y - xz \\
  \dot{z} & = -\beta z + xy
  \end{align}
@endhtmlonly

3.Inline formulas can be specified using this syntax:

@verbatim
  @f$ \sqrt{3x-1}+(1+x)^2 @f$
@endverbatim

  that leads to the following result: @f$ \sqrt{3x-1}+(1+x)^2 @f$
  