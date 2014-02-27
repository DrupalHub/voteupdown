<?php

class VoteUpDown {

  /**
   * Get all the votes for a specific entity.
   */
  public static function getResults($entity_type, $entity_id) {
  }

  /**
   * Get widgets.
   */
  public static function getWidgets() {
    ctools_include('plugins');
    return ctools_get_plugins('vote_up_down', 'widgets');
  }
}