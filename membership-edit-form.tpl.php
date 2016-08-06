<?php
/**
 * @file
 * Template for creating/editing a membership record.
 *
 * Variables available:
 * - $form :
 */

if (!empty($q)):
  print $q;
endif;

print drupal_render($form);
