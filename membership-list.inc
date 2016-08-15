<?php
/**
 * @file
 * Membership list form and functions file.
 */

/**
 * Implements hook_form().
 */
function membership_list_form(&$form_state = NULL) {
  // Use a wide theme here to accomodate the wide table.
  global $custom_theme;
  $custom_theme = 'garland';
  rs_debug("11: _GET=", $_GET);

  $filter = array(
    'member_number' => $_GET['member_number'],
  );

  if (user_access('administer membership')) {
    return array(
      '#tree' => TRUE,
      '#theme' => 'theme_membership_list_form',

      'Filter' => array(
        '#type' => 'fieldset',
        '#title' => t("Filter"),
        '#description' => t("Membership List Filter"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,

        'member_number' => array(
          '#type' => 'textfield',
          '#title' => t('Member Number'),
          '#default_value' => $record->id,
        ),
        'status' => array(
          '#type' => 'checkboxes',
          '#title' => t('Status'),
          '#description' => t(
            'applied = not yet approved, active = approved and paid up,
            expired = not paid for this period, rejected = denied or expelled'
          ),
          '#options' => array(
            'applied',
            'active',
            'expired',
            'rejected',
          ),
        ),
        'paid' => array(
          '#type' => 'checkboxes',
          '#title' => t('Paid'),
          '#options' => array(
            'paid' => t('Paid'),
            'unpaid' => t('Unpaid'),
          ),
        ),
        'firstname' => array(
          '#type' => 'textfield',
          '#title' => t('First Name'),
        ),
        'surname' => array(
          '#type' => 'textfield',
          '#title' => t('Surname'),
        ),
        'gender' => array(
          '#type' => 'checkboxes',
          '#title' => t('Gender'),
          '#options' => array(
            'male' => t('Male'),
            'female' => t('Female'),
          ),
        ),
        'email' => array(
          '#type' => 'textfield',
          '#title' => t('Email'),
        ),
        'card_collection' => array(
          '#type' => 'select',
          '#title' => t('Card Collection'),
          '#options' => array(
            'post' => t('Postal address.'),
            'collect_meeting' => t('Club meeting.'),
            'collect_stellenbosch' => t('Stellenbosch'),
            'collect_cape_town' => t('Cape Town'),
          ),
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Filter'),
        ),
      ),
      'table' => array(
        '#type' => 'markup',
        '#value' => membership_list($filter),
      ),
    );
  }
}

/**
 * Produce an HTML sortable table of all the members.
 */
function membership_list($filter) {
  rs_debug("106: In function membership_list(filter=", $filter);
  $header = array(
    array(
      'data' => t('Member Number'),
      'field' => 'id',
    ),
    array(
      'data' => t('Status'),
      'field' => 'status',
    ),
    array(
      'data' => t('Paid'),
      'field' => 'paid',
    ),
    array(
      'data' => t('First Name'),
      'field' => 'firstname',
    ),
    array(
      'data' => t('Surname'),
      'field' => 'surname',
    ),
    array(
      'data' => t('Gender'),
      'field' => 'gender',
    ),
    array(
      'data' => t('Email'),
      'field' => 'email',
    ),
    array(
      'data' => t('Card Collection'),
      'field' => 'card_collection',
    ),
    array(
      'data' => t('Home Address'),
      'field' => 'address_home',
    ),
    array(
      'data' => t('Postal Address'),
      'field' => 'address_postal',
    ),
    array(
      'data' => t('Cell'),
      'field' => 'cell',
    ),
    array(
      'data' => t('ICE Name'),
      'field' => 'ice_name',
    ),
    array(
      'data' => t('ICE Number'),
      'field' => 'ice_number',
    ),
    array(
      'data' => t('Med Aid Number'),
      'field' => 'medaid_number',
    ),
    array(
      'data' => t('Med Aid Member Number'),
      'field' => 'medaid_member_number',
    ),
    array(
      'data' => t('Med Aid Contact Number'),
      'field' => 'medaid_contact_number',
    ),
  );

  $result = pager_query("SELECT * FROM {membership} " . tablesort_sql($header));

  $rows = array();
  while ($record = db_fetch_object($result)) {
    $row = array();
    foreach ($header as $column) {
      $row[] = $record->$column['field'];
    }
    $rows[] = $row;
  }

  return theme_table($header, $rows);
}

/**
 * Implements hook_form_submit().
 *
 * If the filter button was pressed, call the form with filter parameters.
 */
function membership_list_form_submit($form, &$form_state) {
  rs_debug("194: form_state[values]=", $form_state['values']);
  $query_pair_array = array();
  foreach ($form_state['values']['Filter'] as $key => $value) {
    if (is_array($value)) {
      $subarray = array();
      foreach ($value as $k => $v) {
        if ($v > 0) {
          $subarray[] = $k;
        }
      }
      $query_pair_array[] = "$key=" . implode(',', $subarray);
    }
    else {
      $query_pair_array[] = "$key=$value";
    }
  }
  $query = implode('&', $query_pair_array);

  $form_state['redirect'] = array(
    'membership/list',
    $query,
  );
}

// vim: set et ft=php sw=2 ai:
