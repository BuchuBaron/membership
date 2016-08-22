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

  rs_debug("15: form_state[values]=", $form_state['values']);
  rs_debug("16: _GET=", $_GET);
  $filter = array(
    'member_number' => (!empty($form_state['values']['member_number'])) ? $form_state['values']['member_number'] : $_GET['member_number'],
    'status' => (!empty($form_state['values']['status'])) ? $form_state['values']['status'] : $_GET['status'],
    'paid' => (!empty($form_state['values']['paid'])) ? $form_state['values']['paid'] : $_GET['paid'],
    'firstname' => (!empty($form_state['values']['firstname'])) ? $form_state['values']['firstname'] : $_GET['firstname'],
    'surname' => (!empty($form_state['values']['surname'])) ? $form_state['values']['surname'] : $_GET['surname'],
    'gender' => (!empty($form_state['values']['gender'])) ? $form_state['values']['gender'] : $_GET['gender'],
    'email' => (!empty($form_state['values']['email'])) ? $form_state['values']['email'] : $_GET['email'],
    'card_collection' => (!empty($form_state['values']['card_collection'])) ? $form_state['values']['card_collection'] : $_GET['card_collection'],
  );
  rs_debug("25: filter=", $filter);

  if (user_access('administer membership')) {
    return array(
      '#tree' => TRUE,
      '#theme' => 'theme_membership_list_form',

      'Filter' => array(
        '#type' => 'fieldset',
        '#title' => t("Filter"),
        '#description' => t("Membership List Filter"),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,

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
            'applied' => t('applied'),
            'active' => t('active'),
            'expired' => t('expired'),
            'rejected' => t('rejected'),
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
          '#type' => 'checkboxes',
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
  drupal_add_js(drupal_get_path('module', 'membership') . '/membership-admin.js');
  $header = array(
    array(
      'data' => t('Member'),
      'field' => 'id',
      'valign' => 'bottom',
    ),
    array(
      'data' => t('Status'),
      'field' => 'status',
      'valign' => 'bottom',
      'class' => 'status',
    ),
    array(
      'data' => t('Paid'),
      'field' => 'paid',
      'valign' => 'bottom',
      'class' => 'status',
    ),
    array(
      'data' => t('Card Collection'),
      'field' => 'card_collection',
      'valign' => 'bottom',
      'class' => 'status',
    ),
    array(
      'data' => t('First Name'),
      'field' => 'firstname',
      'valign' => 'bottom',
      'class' => 'identity',
    ),
    array(
      'data' => t('Surname'),
      'field' => 'surname',
      'valign' => 'bottom',
      'class' => 'identity',
    ),
    array(
      'data' => t('Gender'),
      'field' => 'gender',
      'valign' => 'bottom',
      'class' => 'identity',
    ),
    array(
      'data' => t('Cell'),
      'field' => 'cell',
      'valign' => 'bottom',
      'class' => 'contact',
    ),
    array(
      'data' => t('Email'),
      'field' => 'email',
      'valign' => 'bottom',
      'class' => 'contact',
    ),
    array(
      'data' => t('Home Address'),
      'field' => 'address_home',
      'valign' => 'bottom',
      'class' => 'contact',
    ),
    array(
      'data' => t('Postal Address'),
      'field' => 'address_postal',
      'valign' => 'bottom',
      'class' => 'contact',
    ),
    array(
      'data' => t('ICE Name'),
      'field' => 'ice_name',
      'valign' => 'bottom',
      'class' => 'emergency',
    ),
    array(
      'data' => t('ICE Number'),
      'field' => 'ice_number',
      'valign' => 'bottom',
      'class' => 'emergency',
    ),
    array(
      'data' => t('Med Aid Number'),
      'field' => 'medaid_number',
      'valign' => 'bottom',
      'class' => 'emergency',
    ),
    array(
      'data' => t('Med Aid Member Number'),
      'field' => 'medaid_member_number',
      'valign' => 'bottom',
      'class' => 'emergency',
    ),
    array(
      'data' => t('Med Aid Contact Number'),
      'field' => 'medaid_contact_number',
      'valign' => 'bottom',
      'class' => 'emergency',
    ),
  );

  $filterArray = array();
  foreach ($filter as $key => $value) {
    if (!empty($value)) {
      $filterArray[] = "$key like '%$value%'";
    }
  }
  if (!empty($filterArray)) {
    $filterStr = " WHERE " . implode(', and ', $filterArray);
  }
  else {
    $filterStr = '';
  }

  rs_debug("224: filterStr=$filterStr");
  $result = pager_query("SELECT * FROM {membership} $filterStr" . tablesort_sql($header));

  $rows = array();
  while ($record = db_fetch_object($result)) {
    $row = array(
      array(
        'data' => '<input type="checkbox" class="selection" value="' .
        check_plain($record->id) . '" />' . check_plain($record->id),
        'nowrap' => 'nowrap',
        'style' => 'white-space: nowrap',
      ),
    );

    foreach ($header as $column) {
      if ($column['field'] == 'id') {
        // Already done.
        continue;
      }
      $row[] = array(
        'data' => check_plain($record->$column['field']),
        'class' => $column['class'],
      );
    }
    $rows[] = $row;
  }

  // The first row will be the "select all" and action on selection row.
  $action = array(
    array(
      'data' => '<input type="checkbox" id="select-all" value="all" />' . t("<b>Select All</b>"),
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap;width: 0',
    ),
    array(
      'data' => '<select id="action" /><option value="action">' . t('Action') . '</option>' .
      '<option value="export">' . t('export') . '</option>' .
      '<option value="paid">' . t('paid') . '</option>' . '<option value="unpaid">' . t('upaid') . '</option>' .
      '<option value="active">' . t('activate') . '</option>' .
      '<option value="rejected">' . t('reject') . '</option>' .
      '<option value="expired">' . t('expire') . '</option></select>',
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap',
      'align' => 'left',
      'style' => 'white-space: nowrap;width: 0',
    ),
    array(
      'data' => '<input type="button" id="run-action" value="' . t('Run') . '" />',
      'align' => 'left',
      'style' => 'white-space: nowrap;width: 0',
    ),
    array(
      'data' => t("<b>Show:</b>"),
      'align' => 'right',
      'style' => 'padding-left: 3em',
    ),
  );

  foreach (array(t('status'), t('identity'), t('contact'), t('emergency')) as $class) {
    $action[] = array(
      'data' => "<input type=\"checkbox\" id=\"$class\" name=\"$class\" value=\"$class\" />" . $class,
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap;width: 0',
    );
  }

  return theme_table(NULL, array($action), array('style' => 'width: 0')) . '<br />' . theme_table($header, $rows);
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
