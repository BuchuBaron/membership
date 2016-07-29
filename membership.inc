<?php
/**
 * @file
 * Membership form and functions file.
 */

/**
 * Implements hook_form().
 */
function membership_edit_form(&$form_state, $id) {
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
      'card_serial' => array(
        '#type' => 'textfield',
        '#title' => t('Card Serial'),
        '#default_value' => $record->card_serial,
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
    'firstname' => array(
      '#type' => 'textfield',
      '#title' => t('First Name'),
      '#default_value' => $record->first_name,
    ),

  );

  return $form;
}

// vim: set et ft=php sw=2 ai:
