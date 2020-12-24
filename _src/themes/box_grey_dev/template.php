<?php
function phptemplate_image_gallery($galleries, $images) {
  return _phptemplate_callback('image_gallery', array('galleries' => $galleries, 'images' => $images));
}
?>
