
box_grey theme for phptemplate on Drupal
----------------------------------------

box_ is intended to be relatively easy to modify for those that aren't competent in css positioning. Instead of using entirely CSS for layout it is a so called hybrid layout - it uses CSS combined with very simple tables.

It is a standard three column layout using a table for the central three columns with a header above the table and a footer below. This reduces accessibility and adds a little code bloat but has certain advantages:
- the content columns won't overlap if a user inserts wide content
- the content appears in the template (page.tpl.php) in the order it appears on the page
- you can add any sized logo image, the header should expand to fit.

*NOTE for those upgrading*
As of 2006.01.09 the stylesheet has been radically reworked. The colour and border declarations have been merged with those for layout and are no longer clustered at the top of the stylesheet.

Author
------
adrinux (aka Adrian Simmons)
mailto: adrinux@gmail.com
IM: perlucida

Known Problems
--------------
IE5win
  - Places calendar block right margin a few pixels outside the viewport.
  - Gets the padding/margin on menu elements a little wrong, but not unusably so.

IE5mac
  - Has some problems with user pictures enabled, it may (somewhat randomly) increase the left margin on the node content text to match the right margin of the picture.
  - Doesn't honour the styles for links with class="active", these should be pale grey.
  
Everything
  - Fixed width content like the form textareas in the central column can make the sidebars shrink if the browser window is narrow. Again, this looks a little odd but remains perfectly useable.
