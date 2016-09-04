<?php
/**
 * @file
 * Template for listing membership records.
 *
 * Variables available:
 * - $form :
 */

if (!empty($q)):
  print $q;
endif;
?>
<style>
label {
  clear: left;
}
label.option {
  float: left;
  clear: none;
}
div.description {
  clear: left;
}
</style>
<table style="border:none"><tr>
<td>
<?php
print drupal_render($form['Filter']['member_number']);
print drupal_render($form['Filter']['firstname']);
print drupal_render($form['Filter']['surname']);
print drupal_render($form['Filter']['email']);
print drupal_render($form['Filter']['bike']);
print "\n</td><td>\n";
print drupal_render($form['Filter']['status']);
print drupal_render($form['Filter']['paid']);
print drupal_render($form['Filter']['gender']);
print drupal_render($form['Filter']['card_collection']);
print "\n</td></tr></table>";
print drupal_render($form);
