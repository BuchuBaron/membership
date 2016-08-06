<?php
/**
 * @file
 * Membership form and functions file.
 */

/**
 * Implements hook_form().
 */
function membership_edit_form(&$form_state, $id = 0) {
  if ($id > 0) {
    $result = db_query("SELECT * FROM {membership} WHERE id = %d", $id);
    $record = db_fetchObject($result);
  }
  else {
    $record = new stdClass();
  }

  $form = array(
    '#tree' => TRUE,
    '#theme' => 'theme_membership_edit_form',
  );

  if (user_access('administer membership')) {
    $form['Admin'] = array(
      '#type' => 'fieldset',
      '#description' => t("Membership Administration"),
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
    'email' => array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#default_value' => $record->email,
      '#required' => TRUE,
    ),
    'home_address' => array(
      '#type' => 'textarea',
      '#title' => t('Home Address'),
      '#size' => 4,
      '#default_value' => $record->address_home,
      '#required' => TRUE,
    ),
    'postal_address' => array(
      '#type' => 'textarea',
      '#title' => t('Postal Address'),
      '#size' => 4,
      '#default_value' => (empty($record->address_postal)) ? t('As Above.') : $record->address_postal,
    ),
    'cell' => array(
      '#type' => 'textfield',
      '#title' => t('Cell'),
      '#default_value' => $record->cell,
    ),
    'card_collection' => array(
      '#type' => 'select',
      '#title' => t('Card Collection'),
      '#default_value' => $record->card_collection,
      '#options' => array(
        'post' => t('Post to my postal address.'),
        'collect' => t('Collect at club meeting.'),
        'donford_stellenbosch' => t('Collect from Donford Stellenbosch'),
        'donford_cape_town' => t('Collect from Donford Cape Town'),
      ),
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
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

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
      // Sync the uid based on email addrss if we can look it up.
      $uid = db_result(db_query("SELECT uid FROM {user} WHERE mail like '%s'", $values['email']));
      if ($uid > 0) {
        $uid = $uid;
      }
    }
  }

  $admin_update_str = '';
  $admin_update_vals = array();

  if (user_access('administer membership')) {
    $admin = $form_state['values']['Admin'];
    $admin_update_str = <<< EOS
      status = '%s',
      paid = %d,
EOS;
    $admin_update_vals = array(
      $admin['status'],
      $admin['paid'],
    );
  }

  if ($values['id'] > 0) {
    db_query("UPDATE membership SET
      $admin_update_str
      edited = now(),
      uid = %d,
      firstname = '%s',
      surname = '%s',
      gender = '%s',
      email = '%s',
      address_home = '%s',
      address_postal = '%s',
      cell = '%s',
      card_collection = '%s',
      ice_name = '%s',
      ice_number = '%s',
      medaid_name = '%s',
      medaid_member_number = '%s',
      medaid_contact_number = '%s'
      WHERE id = %d",
      $admin_update_vals,
      $uid,
      $values['firstname'],
      $values['surname'],
      $values['gender'],
      $values['email'],
      $values['home_address'],
      $values['postal_address'],
      $values['cell'],
      $values['card_collection'],
      $values['ice_name'],
      $values['ice_number'],
      $values['medaid_name'],
      $values['medaid_member_number'],
      $admin['member_number']
    );
  }
  else {
    db_query("INSERT INTO membership
      (created, status, paid, uid,
      firstname, surname, gender, email,
      address_home, address_postal, cell,
      card_collection, ice_name, ice_number,
      medaid_name, medaid_member_number, medaid_contact_number)
      VALUES
      (now(), '%s', %d, %d,
      '%s', '%s', '%s', '%s',
      '%s', '%s', '%s',
      '%s', '%s', '%s',
      '%s', '%s', '%s')",
      (!empty($admin['status'])) ? $admin['status'] : 'applied',
      $admin['paid'],
      $uid,
      $values['firstname'], $values['surname'], $values['gender'], $values['email'],
      $values['address_home'], $values['address_postal'], $values['cell'],
      $values['card_collection'], $values['ice_name'], $values['ice_number'],
      $values['medaid_name'], $values['medaid_member_number'], $values['medaid_contact_number']
    );
    $new_id = db_result(db_query("LAST_INSERT_ID()"));
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
}

// vim: set et ft=php sw=2 ai:
