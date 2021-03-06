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

  $filter = array();
  $defaults = array();
  foreach (array(
    'member_number',
    'status',
    'paid',
    'firstname',
    'surname',
    'email',
    'card_collection',
    'gender',
    'bike',
  ) as $key) {
    if (!empty($_GET[$key])) {
      $defaults[$key] = $_GET[$key];
      if (in_array($key, array('status', 'gender', 'card_collection'))) {
        // These are multi-value arrays that are comma-separated. Reconstitute
        // them.
        $filter[$key] = explode(',', $_GET[$key]);
      }
      elseif ($key == 'paid') {
        // An array which should have 0 and 1 for unpaid and paid text values.
        $paidArray = explode(',', $_GET['paid']);
        $options = array();
        if (!empty($paidArray)) {
          foreach ($paidArray as $paidValue) {
            if ($paidValue == 'unpaid') {
              $options[] = 0;
            }
            elseif ($paidValue == 'paid') {
              $options[] = 1;
            }
          }
          $filter[$key] = $options;
        }
      }
      else {
        // Just a straight text value.
        $filter[$key] = $_GET[$key];
      }
    }
  }

  if (user_access('administer membership')) {
    $displayTable = membership_make_display_table();
    // Rowcount carries the rowcount into the bulk actions status line.
    $membersTable = membership_list($filter, $rowcount);

    return array(
      '#tree' => TRUE,

      'Filter' => array(
        '#type' => 'fieldset',
        '#title' => t("Membership List Filter"),
        '#theme' => 'theme_membership_list_form',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,

        'member_number' => array(
          '#type' => 'textfield',
          '#title' => t('Member Number'),
          '#default_value' => $defaults['member_number'],
        ),
        'status' => array(
          '#type' => 'checkboxes',
          '#title' => t('Status'),
          '#description' => t(
            'applied = not yet approved, active = approved and paid up,
            expired = not paid for this period, rejected = denied or expelled,
            life = Honorary Life Member.'
          ),
          '#default_value' => explode(',', $defaults['status']),
          '#options' => array(
            'applied' => t('Applied'),
            'active' => t('Active'),
            'life' => t('Life'),
            'expired' => t('Expired'),
            'rejected' => t('Rejected'),
          ),
        ),
        'paid' => array(
          '#type' => 'checkboxes',
          '#title' => t('Paid'),
          '#default_value' => explode(',', $defaults['paid']),
          '#options' => array(
            'paid' => t('Paid'),
            'unpaid' => t('Unpaid'),
          ),
        ),
        'firstname' => array(
          '#type' => 'textfield',
          '#title' => t('First Name'),
          '#default_value' => $defaults['firstname'],
        ),
        'surname' => array(
          '#type' => 'textfield',
          '#title' => t('Surname'),
          '#default_value' => $defaults['surname'],
        ),
        'gender' => array(
          '#type' => 'checkboxes',
          '#title' => t('Gender'),
          '#default_value' => explode(',', $defaults['gender']),
          '#options' => array(
            'male' => t('Male'),
            'female' => t('Female'),
          ),
        ),
        'email' => array(
          '#type' => 'textfield',
          '#title' => t('Email'),
          '#default_value' => $defaults['email'],
        ),
        'card_collection' => array(
          '#type' => 'checkboxes',
          '#title' => t('Card Collection'),
          '#default_value' => explode(',', $defaults['card_collection']),
          '#options' => array(
            'post' => t('Postal address.'),
            'collect_meeting' => t('Club meeting.'),
            'collect_stellenbosch' => t('Stellenbosch'),
            'collect_cape_town' => t('Cape Town'),
          ),
        ),
        'bike' => array(
          '#type' => 'textfield',
          '#title' => t('Bike'),
          '#default_value' => $defaults['bike'],
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Filter'),
        ),
        'reset' => array(
          '#type' => 'submit',
          '#value' => t('Reset'),
        ),
        'display' => array(
          '#type' => 'markup',
          '#value' => t("<br /><br /><h3>Select what types of column to display</h3>!TableOfMembers",
            array('!TableOfMembers' => $displayTable)),
        ),
      ),
      'Actions' => array(
        '#type' => 'fieldset',
        '#title' => 'Bulk Actions on Selected Records',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'Bulk Actions Table' => array(
          '#type' => 'markup',
          '#value' => membership_make_bulk_actions_table($rowcount),
        ),
      ),
      'table' => array(
        '#type' => 'markup',
        '#value' => $membersTable,
      ),
      'iframe' => array(
        '#type' => 'markup',
        '#value' => '<iframe id="dliframe" style="display: none"></iframe>',
      ),
    );
  }
}

/**
 * Produce an HTML sortable table of all the members.
 */
function membership_list($filter, &$rowcount) {
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
      'data' => t('UID'),
      'field' => 'uid',
      'valign' => 'bottom',
      'class' => 'status',
    ),
    array(
      'data' => t('Created'),
      'field' => 'created',
      'sort' => 'desc',
      'valign' => 'bottom',
      'class' => 'sort',
    ),
    array(
      'data' => t('Edited'),
      'field' => 'edited',
      'valign' => 'bottom',
      'class' => 'sort',
    ),
    array(
      'data' => t('Card'),
      'field' => 'card_collection',
      'valign' => 'bottom',
      'class' => 'sort',
    ),
    array(
      'data' => t('Gender'),
      'field' => 'gender',
      'valign' => 'bottom',
      'class' => 'sort',
    ),
    array(
      'data' => t('Bike'),
      'field' => 'bike',
      'valign' => 'bottom',
      'class' => 'sort',
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
      'data' => t('Email'),
      'field' => 'email',
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
      'data' => t('Med Aid Name'),
      'field' => 'medaid_name',
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
    if (is_array($value)) {
      $delimited_values = "'" . implode("', '", $value) . "'";
      $filterArray[] = "$key in ($delimited_values)";
    }
    elseif (!empty($value)) {
      $filterArray[] = "$key like '%$value%'";
    }
  }
  if (!empty($filterArray)) {
    $filterStr = " WHERE " . implode(' and ', $filterArray);
  }
  else {
    $filterStr = '';
  }

  $pagelen = variable_get('pagelen', 1500);
  $result = pager_query(
    "SELECT * FROM {membership} $filterStr" . tablesort_sql($header),
    $pagelen
  );

  $rows = array();
  $rowcount = 0;
  while ($record = db_fetch_object($result)) {
    $rowcount++;
    $row = array(
      array(
        'data' => '<input type="checkbox" class="selection" value="' .
        check_plain($record->id) . '" />' . l(
          $record->id,
          '/membership/edit/' . check_plain($record->id),
          array(
            'query' => drupal_get_destination(),
          )
        ),
        'nowrap' => 'nowrap',
        'style' => 'white-space: nowrap',
      ),
    );

    foreach ($header as $column) {
      if ($column['field'] == 'id') {
        // Already done.
        continue;
      }
      if ($column['field'] == 'uid') {
        $row[] = array(
          'data' => l(
            $record->uid,
            '/user/' . check_plain($record->uid) . '/edit',
            array(
              'query' => drupal_get_destination(),
            )
          ),
          'nowrap' => 'nowrap',
          'style' => 'white-space: nowrap',
          'class' => $column['class'],
          'id' => $column['field'] . '-' . check_plain($record->id),
        );
        continue;
      }
      $row[] = array(
        'data' => check_plain($record->$column['field']),
        'class' => $column['class'],
        'id' => $column['field'] . '-' . check_plain($record->id),
      );
    }
    $rows[] = $row;
  }

  return theme_table($header, $rows) . theme_pager(NULL, $pagelen);
}

/**
 * Implements hook_form_submit().
 *
 * If the filter button was pressed, call the form with filter parameters.
 */
function membership_list_form_submit($form, &$form_state) {
  $query_pair_array = array();
  foreach ($form_state['values']['Filter'] as $key => $value) {
    if ($key == 'submit') {
      continue;
    }
    if (is_array($value)) {
      $subarray = array();
      foreach ($value as $k => $v) {
        if ($v != '0') {
          $subarray[] = $k;
        }
      }
      if (!empty($subarray)) {
        $query_pair_array[] = "$key=" . implode(',', $subarray);
      }
    }
    else {
      $value = trim($value);
      if (!empty($value)) {
        $query_pair_array[] = "$key=$value";
      }
    }
  }
  if ($form_state['values']['op'] == 'Filter') {
    $query = implode('&', $query_pair_array);
  }
  else {
    // Reset the filter.
    $query = NULL;
  }

  $form_state['redirect'] = array(
    'membership/list',
    $query,
  );
}

/**
 * Make a table of checkboxes to control which columns get displayed.
 */
function membership_make_display_table() {
  $columns = array();
  foreach (array(
    t('status'),
    t('sort'),
    t('identity'),
    t('contact'),
    t('emergency'),
  ) as $class) {
    $columns[] = array(
      'data' => "<input type=\"checkbox\" id=\"$class\" name=\"$class\" value=\"\" />" . $class,
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap;width: 0',
    );
  }
  $rows = array(
    $columns,
  );
  $table = theme_table(NULL, $rows);
  return($table);
}

/**
 * Create a closed fieldset with the bulk action dialogue.
 */
function membership_make_bulk_actions_table($rowcount) {
  // The first row will be the "select all" and action on selection row.
  if (user_access('set paid/unpaid status')) {
    $paidSettings = '<option value="paid">' . t('set to paid') . '</option>' . '<option value="unpaid">' . t('set to unpaid') . '</option>';
  }
  else {
    $paidSettings = '';
  }

  $action = array(
    array(
      'data' => '<input type="checkbox" id="select-all" value="all" />' . t("<b>Select All</b>"),
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap;width: 0',
    ),
    array(
      'data' => '<select id="action" /><option value="action">' . t('Action') . '</option>' .
      '<option value="export">' . t('export') . '</option>' .
      $paidSettings .
      '<option value="active">' . t('activate') . '</option>' .
      '<option value="rejected">' . t('reject') . '</option>' .
      '<option value="expired">' . t('expire') . '</option></select>',
      'nowrap' => 'nowrap',
      'style' => 'white-space: nowrap',
      'align' => 'left',
      'style' => 'white-space: nowrap;width: 0',
    ),
    array(
      'data' => '<input type="button" id="run-action" value="' . t('Apply Action') . '" />',
      'align' => 'left',
      'style' => 'white-space: nowrap;width: 0',
    ),
  );

  $status = array(
    array(
      'data' => t('Total Rows:'),
      'header' => TRUE,
    ),
    array(
      'data' => $rowcount,
      'id' => 'row-count',
    ),
    array(
      'data' => t('Selected:'),
      'header' => TRUE,
    ),
    array(
      'data' => 0,
      'id' => 'selected-count',
    ),
  );
  return theme_table(NULL, array($action, $status), array('style' => 'width: 0'));
}

/**
 * Set/get Session variables about the state of the membership list.
 */
function membership_list_session($action, $var) {
  if (!in_array($action, array('get', 'set')) or
    !in_array($var, array('status', 'sort', 'identity', 'contact', 'emergency'))) {
    drupal_goto('access denied');
  }
  $value = check_plain($_POST['value']);
  if ($action === 'set') {
    $_SESSION[$var] = $value;
  }
  else {
    $value = $_SESSION[$var];
  }
  drupal_json(
    array(
      'status' => TRUE,
      'data' => $value,
    )
  );
}

// vim: set et ft=php sw=2 ai:
