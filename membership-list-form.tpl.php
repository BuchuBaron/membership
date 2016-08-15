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

print drupal_render($form);
