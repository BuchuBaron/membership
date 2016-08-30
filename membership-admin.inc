<?php
/**
 * @file
 * Membership admin functions file.
 */

/**
 * Process the bulk-update functions from the members list screen.
 *
 * This function is called via AJAX and returns JSON.
 */
function membership_update($action) {
  rs_debug("13: in function membership_update($action) with _POST=", $_POST);
  return drupal_json(array(
    'result' => TRUE,
    'data' => $_POST,
  ));
}

// vim: set et ft=php sw=2 ai:
