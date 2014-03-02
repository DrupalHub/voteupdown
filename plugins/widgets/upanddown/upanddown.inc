<?php

/**
 * @file
 * Up Down Widget.
 */

/**
 * Plugin decleration.
 */
$plugin = array(
  'title' => t('Up and down'),
  'description' => t('Up and down widget.'),
  'class' => 'voteUpAndDown',
  'assets' => array(
    'css' => array('upanddown.css'),
  ),
);

class voteUpAndDown extends voteUpDownBase {

  public function preprocess(&$variables) {
    parent::preprocess($variables);

    $criteria = array(
      'entity_type' => $variables['entity_type'],
      'entity_id' => $variables['entity_id'],
      'value_type' => 'points',
      'tag' => $variables['tag'],
      'function' => 'positives'
    );

    // todo: This query don't pull the number of the positive vote. Fix this!
    $positives = (int)votingapi_select_single_result_value($criteria);
    $variables['up_points'] = $positives;

    $criteria = array(
      'entity_type' => $variables['entity_type'],
      'entity_id' => $variables['entity_id'],
      'value_type' => 'points',
      'tag' => $variables['tag'],
      'function' => 'negatives'
    );

    // todo: This query don't pull the number of the positive vote. Fix this!
    $negatives = (int)votingapi_select_single_result_value($criteria);
    $variables['down_points'] = $negatives;
  }
}