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
tbody {
  border: none;
}
</style>
<table style="border:none"><tr>
<td>
<?php
print drupal_render($form['member_number']);
print drupal_render($form['firstname']);
print drupal_render($form['surname']);
print drupal_render($form['email']);
print drupal_render($form['bike']);
print "\n</td><td>&nbsp;</td><td>\n";
print drupal_render($form['status']);
print drupal_render($form['paid']);
print drupal_render($form['gender']);
print drupal_render($form['card_collection']);
print "\n</td></tr></table>";
print drupal_render($form);
