<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
        "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $language ?>" xml:lang="<?php print $language ?>">
<head>
  <title><?php print $head_title ?></title>
  <meta http-equiv="Content-Style-Type" content="text/css" />
  <?php print $head ?>
  <?php print $styles ?>
  <script type="text/javascript">

/***********************************************
* Dynamic Ajax Content- © Dynamic Drive DHTML code library (www.dynamicdrive.com)
* This notice MUST stay intact for legal use
* Visit Dynamic Drive at http://www.dynamicdrive.com/ for full source code
***********************************************/

var bustcachevar=1 //bust potential caching of external pages after initial request? (1=yes, 0=no)
var loadedobjects=""
var rootdomain="http://"+window.location.hostname

function ajaxpage(url, containerid){
var page_request = false
if (window.XMLHttpRequest) // if Mozilla, Safari etc
page_request = new XMLHttpRequest()
else if (window.ActiveXObject){ // if IE
try {
page_request = new ActiveXObject("Msxml2.XMLHTTP")
} 
catch (e){
try{
page_request = new ActiveXObject("Microsoft.XMLHTTP")
}
catch (e){}
}
}
else
return false
page_request.onreadystatechange=function(){
loadpage(page_request, containerid)
}
if (bustcachevar) //if bust caching of external page
var bustcacheparameter=(url.indexOf("?")!=-1)? "&"+new Date().getTime() : "?"+new Date().getTime()
page_request.open('GET', url+bustcacheparameter, true)
page_request.send(null)
}

function loadpage(page_request, containerid){
if (page_request.readyState == 4 && (page_request.status==200 || window.location.href.indexOf("http")==-1))
document.getElementById(containerid).innerHTML=page_request.responseText
}

function loadobjs(){
if (!document.getElementById)
return
for (i=0; i<arguments.length; i++){
var file=arguments[i]
var fileref=""
if (loadedobjects.indexOf(file)==-1){ //Check to see if this object has not already been added to page before proceeding
if (file.indexOf(".js")!=-1){ //If object is a js file
fileref=document.createElement('script')
fileref.setAttribute("type","text/javascript");
fileref.setAttribute("src", file);
}
else if (file.indexOf(".css")!=-1){ //If object is a css file
fileref=document.createElement("link")
fileref.setAttribute("rel", "stylesheet");
fileref.setAttribute("type", "text/css");
fileref.setAttribute("href", file);
}
}
if (fileref!=""){
document.getElementsByTagName("head").item(0).appendChild(fileref)
loadedobjects+=file+" " //Remember this object as being already added to page
}
}
}

</script>
</head>
<body <?php print theme("onload_attribute"); ?>>
<div id="header">
  <?php if ($search_box): ?>
  <form action="<?php print $search_url ?>" method="post">
    <div id="search">
      <input class="form-text" type="text" size="15" value="" name="edit[keys]" /><input class="form-submit" type="submit" value="<?php print $search_button_text ?>" />
    </div>
  </form>
  <?php endif; ?>
  <?php if ($logo) : ?>
  <a href="<?php print url() ?>" title="Index Page"><img src="<?php print($logo) ?>" alt="Logo" /></a>
  <?php endif; ?>
  <?php if ($site_name) : ?>
    <h1 id="site-name"><a href="<?php print url() ?>" title="Index Page"><?php print($site_name) ?></a></h1>
  <?php endif;?>
  <?php if ($site_slogan) : ?>
    <span id="site-slogan"><?php print($site_slogan) ?></span>
  <?php endif;?>
  <br class="clear" />
</div>
<div id="top-nav">
  <?php if (count($secondary_links)) : ?>
    <ul id="secondary">
    <?php foreach ($secondary_links as $link): ?>
      <li><?php print $link?></li>
    <?php endforeach; ?>
    </ul>
  <?php endif; ?>
  
  <?php if (count($primary_links)) : ?>
    <ul id="primary">
    <?php 
	$i = 1;
	foreach ($primary_links as $link): ?>
      <li>
	  <?php 
	  print $link; 
	  $number = count($primary_links);
	  if ($i != $number){
	  echo "&nbsp;&nbsp;|";
	  }
	  $i++;
	  ?>
	  </li>
    <?php endforeach; ?>
    </ul>
  <?php endif; ?>

<div id="scroller">
<DIV ID="TICKER" STYLE="overflow:hidden; width:100%"  onmouseover="TICKER_PAUSED=true" onmouseout="TICKER_PAUSED=false">
<? include "ticker.txt"?>
</DIV>
<script src="/drupal/themes/box_grey_dev/webticker_lib.js" language="javascript"></script>
</div>
</div>

<table id="content">
  <tr>
    <?php if ($sidebar_left != ""): ?>
      <td class="sidebar" id="sidebar-left">
        <?php print $sidebar_left ?>
      </td>
    <?php endif; ?>    
        <td class="main-content" id="content-<?php print $layout ?>">
        <?php if ($title != ""): ?>
          <h2 class="content-title"><?php print $title ?></h2>
        <?php endif; ?>
        <?php if ($tabs != ""): ?>
          <?php print $tabs ?>
        <?php endif; ?>
        
        <?php if ($mission != ""): ?>
          <div id="mission"><?php print $mission ?></div>
        <?php endif; ?>
        
        <?php if ($help != ""): ?>
          <p id="help"><?php print $help ?></p>
        <?php endif; ?>
        
        <?php if ($messages != ""): ?>
          <div id="message"><?php print $messages ?></div>
        <?php endif; ?>
        
        <!-- start main content -->
        <?php print($content) ?>
        <!-- end main content -->
        </td><!-- mainContent -->
    <?php if ($sidebar_right != ""): ?>
    <td class="sidebar" id="sidebar-right">
        <?php print $sidebar_right ?>
    </td>
    <?php endif; ?>
	    <!--<td valign="top" width="120">
<iframe src="http://rcm.amazon.com/e/cm?t=wwwcontechsco-20
&o=1&p=11&l=ez&f=ifr&f=ifr" width="120" height="600" scrolling="no" border="0" frameborder="0" style="border:none;"></iframe>
    </td>
-->
  </tr>
</table>
<?php print $breadcrumb ?>
<div id="footer">
  <?php if ($footer_message) : ?>
    <p><?php print $footer_message;?></p>
  <?php endif; ?>


Validate <a href="http://validator.w3.org/check/referer">XHTML</a> or <a href="http://jigsaw.w3.org/css-validator/check/referer">CSS</a>.
</div><!-- footer -->
 <?php print $closure;?>
  </body>
</html>

