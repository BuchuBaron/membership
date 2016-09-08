<?php
/**
 * @file
 * Membership form and functions file.
 */

/**
 * Implements hook_form().
 */
function membership_edit_form(&$form_state = NULL, $id = 0) {
  global $user;

  // Check for existing membership record.  And confirm this
  // user may edit this record.
  if ($id == 0) {
    $id = db_result(db_query("SELECT id FROM {membership} WHERE uid=%d", $user->uid));
  }

  if ($id > 0) {
    $result = db_query("SELECT * FROM {membership} WHERE id = %d", $id);
    $record = db_fetch_object($result);
  }
  else {
    $record = new stdClass();
  }

  if (!user_access('administer membership')) {
    // User editing their own record: the uid must match.
    if ($record->uid > 0 && $record->uid != $user->uid) {
      rs_debug("32 denying access with record=", $record, "and user=", $user);
      drupal_goto(drupal_access_denied());
    }
  }

  $form = array(
    '#tree' => TRUE,
    '#theme' => 'theme_membership_edit_form',
  );

  if (user_access('administer membership')) {
    $form['Admin'] = array(
      '#type' => 'fieldset',
      '#title' => t("Membership Administration"),
      '#description' => t("For office use only."),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,

      'member_number' => array(
        '#type' => 'textfield',
        '#title' => t('Member Number'),
        '#default_value' => $record->id,
      ),
      'status' => array(
        '#type' => 'select',
        '#title' => t('Status'),
        '#description' => t(
          'applied = not yet approved, active = approved and paid up,
          expired = not paid for this period, rejected = denied or expelled'
        ),
        '#default_value' => $record->status,
        '#options' => array(
          'applied',
          'active',
          'expired',
          'rejected',
        ),
      ),
      'paid' => array(
        '#type' => 'checkbox',
        '#title' => t('Paid'),
        '#default_value' => $record->paid,
      ),
    );
  }

  $form['Record'] = array(
    'id' => array(
      '#type' => 'value',
      '#value' => $id,
    ),
    'firstname' => array(
      '#type' => 'textfield',
      '#title' => t('First Name'),
      '#default_value' => $record->firstname,
      '#required' => TRUE,
    ),
    'surname' => array(
      '#type' => 'textfield',
      '#title' => t('Surname'),
      '#default_value' => $record->surname,
      '#required' => TRUE,
    ),
    'email' => array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => $record->email,
      '#required' => TRUE,
    ),
    'cell' => array(
      '#type' => 'textfield',
      '#title' => t('Cell'),
      '#default_value' => $record->cell,
    ),
    'bike' => array(
      '#type' => 'textfield',
      '#title' => t('Bike'),
      '#description' => t('What bike do you ride?'),
      '#default_value' => $record->bike,
    ),
    'gender' => array(
      '#type' => 'radios',
      '#title' => t('Gender'),
      '#default_value' => ($record->gender) ? $record->gender : 'male',
      '#options' => array(
        'male' => t('Male'),
        'female' => t('Female'),
      ),
      '#required' => TRUE,
    ),
    'card_collection' => array(
      '#type' => 'select',
      '#title' => t('Card Collection'),
      '#default_value' => $record->card_collection,
      '#options' => array(
        'post' => t('Post to my postal address.'),
        'collect_meeting' => t('Collect at club meeting.'),
        'collect_stellenbosch' => t('Collect from Donford Stellenbosch'),
        'collect_cape_town' => t('Collect from Donford Cape Town'),
      ),
    ),
    'address_home' => array(
      '#type' => 'textarea',
      '#title' => t('Home Address'),
      '#size' => 4,
      '#default_value' => $record->address_home,
      '#required' => TRUE,
    ),
    'address_postal' => array(
      '#type' => 'textarea',
      '#title' => t('Postal Address'),
      '#size' => 4,
      '#default_value' => (empty($record->address_postal)) ? t('As Above.') : $record->address_postal,
    ),
    'ice_name' => array(
      '#type' => 'textfield',
      '#title' => t('ICE Name'),
      '#description' => t('Person to contact In Case of Emergency (ICE)'),
      '#default_value' => $record->ice_name,
      '#required' => TRUE,
    ),
    'ice_number' => array(
      '#type' => 'textfield',
      '#title' => t('ICE Number'),
      '#description' => t('Person to contact In Case of Emergency (ICE)'),
      '#default_value' => $record->ice_number,
      '#required' => TRUE,
    ),
    'medaid_name' => array(
      '#type' => 'textfield',
      '#title' => t('Medical Aid Name'),
      '#description' => t('Your Medical Aid'),
      '#default_value' => $record->medaid_name,
      '#required' => TRUE,
    ),
    'medaid_member_number' => array(
      '#type' => 'textfield',
      '#title' => t('Medical Aid Member Number'),
      '#description' => t('Your Medical Aid Membership Number.'),
      '#default_value' => $record->medaid_member_number,
      '#required' => TRUE,
    ),
    'medaid_contact_number' => array(
      '#type' => 'textfield',
      '#title' => t('Medical Aid Contact Number'),
      '#description' => t('The Emergency Number for your Medical Aid.'),
      '#default_value' => $record->medaid_contact_number,
      '#required' => TRUE,
    ),
    'uid' => array(
      '#type' => 'value',
      '#value' => $record->uid,
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Join'),
  );
  if ($record->id > 0) {
    $form['submit']['#value'] = t('Save');
  }

  return $form;
}

/**
 * Implements hook_form_validate().
 *
 * See if we can find the right uid for this member from the email
 * or from the logged in user.
 */
function membership_edit_form_validate($form, &$form_state) {
  global $user;
  $values = &$form_state['values']['Record'];
  $admin = &$form_state['values']['Admin'];
}

/**
 * Implements hook_form_submit().
 */
function membership_edit_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values']['Record'];
  $admin = $form_state['values']['Admin'];

  if (valid_email_address($values['email'])) {
    if ($user->mail == $values['email']) {
      // User editing their own account, synchronize uids.
      $uid = $user->uid;
    }
    else {
      // Sync the uid based on email address if we can look it up.
      $uid = db_result(db_query("SELECT uid FROM {users} WHERE mail like '%s'", $values['email']));
      if ($uid > 0) {
        $uid = $uid;
      }
    }

    if ($values['uid'] > 0) {
      // Make sure a change in email address updates the drupal user email also.
      db_query("UPDATE {users} SET mail='%s' WHERE  uid=%d", $form_state['values']['email'], $form_state['values']['uid']);
    }
  }

  $admin_update_str = '';
  $admin_update_vals = array();

  if (user_access('administer membership')) {
    $update_str = <<< EOS
      status = '%s',
      paid = %d,
EOS;
    $update_vals = array(
      $admin['status'],
      $admin['paid'],
    );
  }

  // Now complete the rest of the update string and array.
  $update_str .= <<< EOS
    uid = %d,
    firstname = '%s',
    surname = '%s',
    email = '%s',
    cell = '%s',
    bike = '%s',
    gender = '%s',
    card_collection = '%s',
    address_home = '%s',
    address_postal = '%s',
    ice_name = '%s',
    ice_number = '%s',
    medaid_name = '%s',
    medaid_member_number = '%s',
    medaid_contact_number = '%s'
EOS;
  $update_vals = array_merge($update_vals, array(
    $uid,
    $values['firstname'],
    $values['surname'],
    $values['email'],
    $values['cell'],
    $values['bike'],
    $values['gender'],
    $values['card_collection'],
    $values['address_home'],
    $values['address_postal'],
    $values['ice_name'],
    $values['ice_number'],
    $values['medaid_name'],
    $values['medaid_member_number'],
    $values['medaid_contact_number'],
    $values['id'],
  ));

  // If NOT id>0, create the lowest id higher than 50 and use that.
  if (!($values['id'] > 0)) {
    db_query("INSERT INTO membership
      (id,
      created, status, paid, uid,
      firstname, surname, gender, email,
      address_home, address_postal, cell, bike,
      card_collection, ice_name, ice_number,
      medaid_name, medaid_member_number, medaid_contact_number)
      SELECT min(m1.id + 1),
      now(), '%s', %d, %d,
      '%s', '%s', '%s', '%s',
      '%s', '%s', '%s', '%s',
      '%s', '%s', '%s',
      '%s', '%s', '%s'
      FROM {membership} m1
      LEFT JOIN {membership} m2 ON m1.id + 1 = m2.id
      WHERE m2.id IS NULL AND m1.id > 50
      LIMIT 1"
    );
    $new_id = db_result(db_query("SELECT LAST_INSERT_ID()"));
    drupal_set_message(t(
      "Member with id=@id and uid=@uid successfully created.",
      array(
        '@id' => $new_id,
        '@uid' => $uid,
      )
    ));
    if ($user->uid > 0) {
      // Logged in, so allowed to access the edit form.
      $form_state['redirect'] = 'membership/edit/' . $new_id;
    }
    else {
      $form_state['redirect'] = 'join-bmwmcc-club-cape';
    }
  }
  elseif ($values['id'] > 0) {
    db_query("UPDATE membership SET
      edited = now(),
      $update_str
      WHERE id = %d",
      $update_vals
    );
  }
  /*
  else {
    db_query("INSERT INTO membership
      (created, status, paid, uid,
      firstname, surname, gender, email,
      address_home, address_postal, cell, bike,
      card_collection, ice_name, ice_number,
      medaid_name, medaid_member_number, medaid_contact_number)
      VALUES
      (now(), '%s', %d, %d,
      '%s', '%s', '%s', '%s',
      '%s', '%s', '%s', '%s',
      '%s', '%s', '%s',
      '%s', '%s', '%s')",
      (!empty($admin['status'])) ? $admin['status'] : 'applied',
      $admin['paid'],
      $uid,
      $values['firstname'], $values['surname'], $values['gender'], $values['email'],
      $values['address_home'], $values['address_postal'], $values['cell'], $values['bike'],
      $values['card_collection'], $values['ice_name'], $values['ice_number'],
      $values['medaid_name'], $values['medaid_member_number'], $values['medaid_contact_number']
    );

    $new_id = db_result(db_query("SELECT LAST_INSERT_ID()"));
    if ($user->uid > 0) {
      // Logged in, so allowed to access the edit form.
      drupal_goto('membership/edit/' . $new_id);
    }
    else {
      drupal_goto('join-bmwmcc-club-cape');
    }

    if ($new_id > 0) {
      drupal_set_message(t(
        "Member with id=@id and uid=@uid successfully created.",
        array(
          '@id' => $new_id,
          '@uid' => $uid,
        )
      ));
    }
  }
   */
}

// vim: set et ft=php sw=2 ai:
