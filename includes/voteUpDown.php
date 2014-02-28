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
   * Get all the votes for a specific entity.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object.
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

  /**
   * Get the votes.
   */
  public function getVotes() {
    return $this->votes;
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
   * Deleting a vote.
   */
  public function deleteVote() {
    if (!$this->vote_id) {
      $this->missingVote();
    }
  }

  /**
   * The vote ID is missing.
   */
  private function missingVote() {
    throw new Exception(t("A vote ID is missing."));
  }
}