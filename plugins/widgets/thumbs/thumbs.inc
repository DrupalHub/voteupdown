<?php
/**
 * @file
 *   Thumbs widget.
 */

$plugin = array(
  'title' => t('Thumbs up and down'),
  'description' => t('Thumbs up and down widget.'),
  'class' => 'voteUpDownThumbs',
  'alter template variables' => 'vud_widget_thumbs_alter_template_vars',
  'js' => 'thumbs.js',
);

class voteUpDownThumbs implements voteUpDownInterface {

  /**
   * Instance settings.
   *
   * @{@inheritdoc}
   */
  public function instanceSettings($settings) {
    $form = array();

    return $form;
  }

  /**
   * Validating the value before saving the data to the DB.
   */
  public function validate() {

  }

  /**
   * Displaying the values of the form.
   */
  public function view() {

  }

  /**
   * When editing the field through the UI.
   */
  public function edit() {

  }
}

function vud_widget_thumbs_alter_template_vars($template_type, &$variables) {
  $criteria['entity_id'] = $variables['entity_id'];
  $criteria['entity_type'] = $variables['type'];
  $criteria['value_type'] = 'points';
  $criteria['tag'] = $variables['tag'];
  $criteria['function']  = 'sum';
  $vote_sum = votingapi_select_single_result_value($criteria);
  $variables['vote_sum'] = ($vote_sum) ? $vote_sum : 0;
}
