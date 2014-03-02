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
    $variables['up_points'] =  $this->countVotes($variables['entity_type'], $variables['entity_id'], 'positive');
    $variables['down_points'] = $this->countVotes($variables['entity_type'], $variables['entity_id'], 'negative');
  }

  /**
   * Count how much positive/negative votes for an entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity ID or object.
   * @param $type
   *  The type of the vote: negative or positive.
   */
  private function countVotes($entity_type, $entity, $type) {
    return db_select('votingapi_vote', 'v')
      ->fields('v')
      ->condition('entity_type', $entity_type)
      ->condition('entity_id', $entity)
      ->condition('value', $type == 'negative' ? -1 : 1)
      ->execute()
      ->rowCount();
  }
}