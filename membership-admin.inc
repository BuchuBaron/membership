<?php
/**
 * @file
 * Membership admin functions file.
 */

/**
 * Process the bulk-update functions from the members list screen.
 *
 * This function is called via AJAX and returns JSON.
 * Before any change is made, record all the current values of status/paid per
 * id in a separate audit_log table, so that mistakes can be undone.
 */
function membership_update($action) {
  global $user;
  rs_debug("13: in function membership_update($action) with _POST=", $_POST);
  // Sanitize the membership id list by hand...
  $memberlistarray = explode(',', check_plain($_POST['selected_members']));
  $cleanmemberarray = array();
  foreach ($memberlistarray as $id) {
    if (ctype_digit($id)) {
      $cleanmemberarray[] = (int) $id;
    }
  }
  unset($memberlistarray);
  $cleanlist = implode(',', $cleanmemberarray);

  if ($action == 'paid') {
    $changeColumn = 'paid';
    $newValue = 1;
  }
  elseif ($action == 'unpaid') {
    $changeColumn = 'paid';
    $newValue = 0;
  }
  elseif ($action == 'activate') {
    $changeColumn = 'status';
    $newValue = 'active';
  }
  elseif ($action == 'reject') {
    $changeColumn = 'status';
    $newValue = 'rejected';
  }
  elseif ($action == 'expire') {
    $changeColumn = 'status';
    $newValue = 'expired';
  }

  // Store current values for these members in an audit log that can be used to
  // reset mistakes.
  $changeArray = array();
  $result = db_query("SELECT id, $changeColumn FROM membership WHERE id IN ($cleanlist) ORDER BY id");
  while ($row = db_fetch_object($result)) {
    $changeArray[$row->id] = $row->$changeColumn;
  }
  db_query("INSERT INTO membership_auditlog (uid, attribute, newvalue, oldvalues) VALUES (%d, '%s', '%s', '%s')",
    $user->uid,
    $changeColumn,
    $newValue,
    json_encode($changeArray)
  );

  // Run the action against the records.
  db_query("UPDATE {membership} SET $changeColumn = $newValue
    WHERE id IN ($cleanlist)");

  return drupal_json(array(
    'result' => TRUE,
    'data' => $cleanmemberarray,
  ));
}

// vim: set et ft=php sw=2 ai:
