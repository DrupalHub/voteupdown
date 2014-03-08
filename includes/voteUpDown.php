<?php

class VoteUpDown {

  /**
   * The votes loaded from the DB.
   *
   * @var
   */
  protected $votes;

  /**
   * Marking the vote we currently dealing with.
   *
   * @var
   */
  protected $vote_id;

  /**
   * The entity type.
   * @var
   */
  protected $entity_type;

  /**
   * @var
   */
  protected $entity;

  /**
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   *
   * @return VoteUpDown
   */
  public function __construct($entity_type, $entity) {
    $this->votes = self::getResults($entity_type, $entity);
    $this->entity_type = $entity_type;
    $this->entity = $entity;

    return $this;
  }

  /**
   * Get the votes.
   */
  public function getVotes() {
    return $this->votes;
  }

  /**
   * Get the entity type.
   */
  public function getEntityType() {
    return $this->entity_type;
  }

  /**
   * Get the entity object.
   */
  public function getEntity() {
    return $this->entity;
  }

  /**
   * Set the context of the class for a specific vote.
   *
   * @param $id
   *  The vote ID we currently handling.
   *
   * @throws Exception
   *  When the given ID don't belong to the entity.
   *
   * @return $this
   */
  public function get($id) {
    $this->vote_id = $id;

    if (!in_array($id, array_keys($this->votes))) {
      throw new Exception(t("The vote @id don't belong to the current entity.", array('@id' => $id)));
    }

    return $this;
  }

  /**
   * Updating the current vote. When changing the entity the vote belong to will
   * eventually lead us to remove the referencing from the field and this quite
   * a work. We can avoid this for now.
   *
   * @param array $values
   *  Values of the vote. Allowed keys:
   *    - vote_id: The vote ID.
   *    - entity_type: The entity type.
   *    - entity_id: the entity ID.
   *    - value_type: The value type.
   *    - tag: The tag type.
   *    - uid: The voter ID.
   *    - vote_source: The vote source.
   *
   *  @see votingapi_set_votes().
   *
   * @throws Exception
   */
  public function updateVote(array $values) {
    if (!$this->vote_id) {
      $this->missingVote();
    }

    $fields = array_keys($values);

    if (in_array('entity_type', $fields) || in_array('entity_id', $fields)) {
      throw new Exception(t("Changing the vote entity information is not allowed via the vote up down handler."));
    }

    $values += $this->votes[$this->vote_id];

    $vote = array($values);

    votingapi_set_votes($vote);
  }

  /**
   * The vote ID is missing.
   */
  private function missingVote() {
    throw new Exception(t("A vote ID is missing."));
  }

  /**
   * Get the base DB query object.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   *
   * @return SelectQuery
   */
  private static function baseQuery($entity_type, $entity) {
    list($id) = entity_extract_ids($entity_type, $entity);

    return db_select('votingapi_vote')
      ->fields('votingapi_vote')
      ->condition('entity_id', $id)
      ->condition('entity_type', $entity_type);
  }

  /**
   * Get all the votes for a specific entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   */
  public static function getResults($entity_type, $entity) {
    return self::baseQuery($entity_type, $entity)
      ->execute()
      ->fetchAllAssoc('vote_id');
  }

  /**
   * Get votes by the user ID for a specific entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   * @param $uid
   *  The user ID.
   *
   * @return
   *  The user votes for this entity.
   */
  public static function getVotesForUser($entity_type, $entity, $uid) {
    $base_query = self::baseQuery($entity_type, $entity);

    return $base_query
      ->condition('uid', $uid)
      ->execute()
      ->fetchAllAssoc('vote_id');
  }

  /**
   * Setting vote for user upon an entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   * @param $value
   *  The value of the vote.
   * @param $uid
   *  The user ID.
   * @param $id
   *  The ID of the vote. Optional.
   *
   * @return Array
   *  Information about the new vote.
   */
  public static function setVote($entity_type, $entity, $value, $uid, $id = NULL) {
    $wrapper = entity_metadata_wrapper($entity_type, $entity);

    $vote = array(
      'entity_type' => $wrapper->type(),
      'entity_id' => $wrapper->getIdentifier(),
      'value' => $value,
      'uid' => $uid,
    );

    if ($id) {
      // Working with a specific vote ID - we need to update using db_update().
      db_update('votingapi_vote')
        ->fields($vote)
        ->condition('vote_id', $id)
        ->execute();

      return $vote + array(
        'vote_id' => $id,
      );
    }
    else {
      $votes = array($vote);
      votingapi_set_votes($votes);
      return $votes[0];
    }
  }

  /**
   * Get widgets.
   */
  public static function getWidgets() {
    ctools_include('plugins');
    return ctools_get_plugins('vud', 'widgets');
  }

  /**
   * Get the vote for the given entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
   */
  public static function findField($entity_type, $entity) {
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $instances = field_info_instances($wrapper->type(), $wrapper->getBundle());

    foreach ($instances as $instance) {
      $field = field_info_field($instance['field_name']);

      if ($field['type'] == 'vud') {
        return $instance['field_name'];
      }
    }
  }
}