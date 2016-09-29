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

  $changeColumn = FALSE;
  if ($action == 'paid') {
    $changeColumn = 'paid';
    $newValue = 1;
  }
  elseif ($action == 'unpaid') {
    $changeColumn = 'paid';
    $newValue = 0;
  }
  elseif ($action == 'active') {
    $changeColumn = 'status';
    $newValue = 'active';
  }
  elseif ($action == 'rejected') {
    $changeColumn = 'status';
    $newValue = 'rejected';
  }
  elseif ($action == 'expired') {
    $changeColumn = 'status';
    $newValue = 'expired';
  }

  if ($changeColumn !== FALSE) {
    // Store current values for these members in an audit log that can be used
    // to reset mistakes.
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
    db_query("UPDATE {membership} SET $changeColumn = '%s'
      WHERE id IN ($cleanlist)", $newValue);

    return drupal_json(array(
      'result' => TRUE,
      'data' => $cleanmemberarray,
    ));
  }
  else {
    // Get here if the action is not valid, or it is an export.
    if ($action == 'export') {
      return membership_export($cleanlist);
    }
  }
  return drupal_json(array(
    'result' => FALSE,
    'data' => array(
      'success' => 0,
    ),
  ));
}

/**
 * Download a CSV of the selected members.
 */
function membership_export($memberlist) {
  $headerArray = array(
    'id',
    'status',
    'paid',
    'created',
    'edited',
    'uid',
    'firstname',
    'surname',
    'gender',
    'email',
    'address_home',
    'address_postal',
    'cell',
    'card_collection',
    'ice_name',
    'ice_number',
    'medaid_name',
    'medaid_member_number',
    'medaid_contact_number',
  );
  $result = db_query("SELECT * FROM {membership} WHERE id IN ($memberlist)");
  $rows = array();
  $rows[] = '"' . implode('","', $headerArray) . '"';
  while ($record = db_fetch_object($result)) {
    $row = array();
    foreach ($headerArray as $column) {
      $row[] = $record->$column;
    }
    $rows[] = '"' . implode('","', $row) . '"';
  }

  $content = implode("\n", $rows);

  // Now we write this to a file, and return the url to download the file.
  // The download URL will serve the file and unlink it when done.
  $file = tempnam(file_directory_temp(), 'BMWmembership.csv');
  if (!$fh = fopen($file, 'w')) {
    drupal_set_message("Cannot open file ($file)");
  }
  else {
    fwrite($fh, $content);
    fclose($fh);
  }

  return drupal_json(array(
    'result' => TRUE,
    'data' => array(
      'path' => $file,
    ),
  ));
}

/**
 * Serves a temporary file for direct download and deletes it.
 */
function membership_download() {
  $path = $_GET['path'];
  // Most of this is only necessary because of IE.
  // Drupal_set_header('Content-Type: text/csv; charset=utf-8');.
  if (file_exists($path)) {
    drupal_set_header('Content-Description: File Transfer');
    drupal_set_header('Content-Type: application/force-download');
    drupal_set_header('Content-Disposition: attachment; filename="BMWmembers.csv"');
    drupal_set_header('Content-Transfer-Encoding: binary');
    drupal_set_header('Expires: 0');
    drupal_set_header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    drupal_set_header('Pragma: public');
    drupal_set_header('Content-Length: ' . filesize($path));
    ob_clean();
    flush();
    print file_get_contents($path);
    unlink($path);
  }
  module_invoke_all('exit');
  exit();
}

/**
 * Set UID fields in the membership table by matching emails.
 */
function membership_uids() {
  $result = db_query("SELECT id, LOWER(email) AS email FROM {membership} WHERE uid IS NULL OR uid=0");
  while ($member = db_fetch_object($result)) {
    $uid = db_result(db_query("SELECT uid FROM {users} WHERE lower(mail) = '%s'", trim($member->email)));
    if ($uid > 0) {
      db_query("UPDATE {membership} SET uid=%d WHERE id = %d", $uid, $member->id);
    }
  }
}

/**
 * Restore a bulk update to a previous state.
 *
 * You will have to find the right record in membership_auditlog to get the date
 * and time, and the userId.  This is manual for now.
 */
function membership_restore($uid, $datetime) {
  $result = db_query("SELECT * FROM {membership_auditlog} WHERE uid=%d AND timestamp='%s'", $uid, $datetime);
  $record = db_fetch_object($result);
  $oldvalues = json_decode($record->oldvalues);

  foreach ($oldvalues as $id => $value) {
    db_query("UPDATE {membership} SET " . $record->attribute . "='%s' WHERE id=%d",
      $value, $id
    );
  }
  drupal_goto('/membership/list');
}

// vim: set et ft=php sw=2 ai:
