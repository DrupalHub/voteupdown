<?php

/**
 * Interface for the widgets of the vote up down field.
 */
interface voteUpDownInterface {

  /**
   * Instance settings.
   *
   * @param $settings
   *  The settings of the instance.
   */
  public function instanceSettings($settings);

  /**
   * Validating the value before saving the data to the DB.
   *
   * @param $entity_type
   *  The entity type we set the vote for.
   * @param $entity_id
   *  The entity object.
   * @param $value
   *  The value of the point.
   * @param $account
   *  The user object.
   *
   * @throws Exception
   *  When the validate failed throws an error.
   */
  public function validate($entity_type, $entity_id, $value, $account = NULL);

  /**
   * Displaying the values of the form.
   */
  public function view($entity_type, $entity, $field, $instance, $langcode, $items, $display);

  /**
   * Provide a nice widgets when editing the entity.
   */
  public function widget(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element);

  /**
   * Allow each plugin handler to alter the variables of the theme function.
   *
   * @param $variables
   *  The variables of the theme function.
   */
  public function preprocess(&$variables);
}