<?php

class VoteUpDown {

  /**
   * Get all the votes for a specific entity.
   */
  public static function getResults($entity_type, $entity) {
    list($id) = entity_extract_ids($entity_type, $entity);

    return db_select('votingapi_vote')
      ->fields('votingapi_vote')
      ->condition('entity_id', $id)
      ->condition('entity_type', $entity_type)
      ->execute()
      ->fetchAllAssoc('vote_id');
  }

  /**
   * Get widgets.
   */
  public static function getWidgets() {
    ctools_include('plugins');
    return ctools_get_plugins('vote_up_down', 'widgets');
  }
}