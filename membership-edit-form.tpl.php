<?php
/**
 * @file sync-form.tpl.php
 *
 * This template handles the layout of the synchronization form between RS and Drupal logins
 *
 * Variables available:
 * - $form : 
 *
 */

if (!empty($q)): 
  // This ensures that, if clean URLs are off, the 'q' is added first so that
  // it shows up first in the URL.
  print $q;
endif; ?>

<?php
  $fieldset = array('Active', 'Synced');
  foreach ($fieldset as $set) {
    $header_row = array(
      t('Login name'),
      t('UID'),
      t('Email'),
      drupal_render($form[$set]['copy']),
      drupal_render($form[$set]['delete']),
    );
    if ($set == 'Synced') {
      $header_row[3] = drupal_render($form[$set]['deleters']);
      $header_row[4] = drupal_render($form[$set]['deletedr']);
    }
    $data_rows = array();
    foreach ($form[$set] as $uid => $element) {
      if ($uid > 0) {
        $row = $element['item']['#value'];
        drupal_render($form[$set][$uid]['item']);  // throw this away
        if ($set == 'Synced') {
          $row[] = drupal_render($form[$set][$uid]['deleters']);
          $row[] = drupal_render($form[$set][$uid]['deletedr']);
        }
        else {
          $row[] = drupal_render($form[$set][$uid]['copy']);
          $row[] = drupal_render($form[$set][$uid]['delete']);
        }
        $data_rows[] = $row;
      }
    }
    $form[$set]['table'] = array(
      '#value' => theme_table($header_row, $data_rows),
    );
  }

  $tablerows = array();

  foreach ($form['Common'] as $uid => $element) {
    if ($uid > 0) {
      $rows['Dr'] = $element['item']['#value']['Dr'];
      array_unshift($rows['Dr'], 'Drpl');
      $form['Common'][$uid]['copy2rs']['#title'] = '';
      $rows['Dr'][] = array(
        'data' => drupal_render($form['Common'][$uid]['copy2rs']),
      );
      $form['Common'][$uid]['deletedr']['#title'] = '';
      $rows['Dr'][] = array(
        'data' => drupal_render($form['Common'][$uid]['deletedr']),
      );
      $rows['Rs'] = $element['item']['#value']['Rs'];
      array_unshift($rows['Rs'], 'RS');
      $form['Common'][$uid]['copy2dr']['#title'] = '';
      $rows['Rs'][] = array(
        'data' => drupal_render($form['Common'][$uid]['copy2dr']),
      );
      $form['Common'][$uid]['deleters']['#title'] = '';
      $rows['Rs'][] = array(
        'data' => drupal_render($form['Common'][$uid]['deleters']),
      );
      $tablerows[] = $rows['Dr'];
      $tablerows[] = $rows['Rs'];
      unset($form['Common'][$uid]['item']);
    }
  }
  $tableheader = array(
    array( 'data' => t('DB'),),
    array( 'data' => t('uid'),),
    array( 'data' => t('name'),),
    array( 'data' => t('pass'),),
    array( 'data' => t('email'),),
    array( 'data' => t('m'),),
    array( 'data' => t('s'),),
    array( 'data' => t('t'),),
    array( 'data' => t('thme'),),
    array( 'data' => t('sig'),),
    array( 'data' => t('sig fmt'),),
    array( 'data' => t('stat'),),
    array( 'data' => t('tzone'),),
    array( 'data' => t('lang'),),
    array( 'data' => t('pic'),),
    array( 'data' => t('data'),),
    array( 'data' => t('roles'),),
    array( 'data' => t('acc'),),
    array( 'data' => t('sndr'),),
    array( 'data' => t('copy'),),
    array( 'data' => t('del'),),
  );
  $action_row = array();
  $action_row[0][] = drupal_render($form['Common']['copy2rs']);
  $action_row[0][] = drupal_render($form['Common']['copy2dr']);
  $action_row[0][] = drupal_render($form['Common']['deleters']);
  $action_row[0][] = drupal_render($form['Common']['deletedr']);

  $form['Common']['content'] = array(
    '#value' => theme_table(NULL, $action_row) . '<br />' . theme_table($tableheader, $tablerows),
  );

  $header_row = array(
    t('Login name'),
    t('UID'),
    t('Email'),
    drupal_render($form['RSOnly']['copy']),
    drupal_render($form['RSOnly']['delete']),
  );
  $data_rows = array();
  foreach ($form['RSOnly'] as $mail => $element) {
    if (is_array($element['item'])) {
      $row = $element['item']['#value'];
      drupal_render($form['RSOnly'][$mail]['item']);  // throw this away
      $row[] = drupal_render($form['RSOnly'][$mail]['copy']);
      $row[] = drupal_render($form['RSOnly'][$mail]['delete']);
      $data_rows[] = $row;
    }
  }
  $form['RSOnly']['table'] = array(
    '#value' => theme_table($header_row, $data_rows),
  );

  print drupal_render($form['Synced']);
  print drupal_render($form['Active']);
  print drupal_render($form['RSOnly']);
  print drupal_render($form); 
?>
