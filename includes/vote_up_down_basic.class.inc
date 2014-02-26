<?php

/**
 * Defining base class for other modules.
 */
class voteUpDownBase implements voteUpDownInterface {

  public $settings;

  function __construct(array $settings = array()) {
    $this->settings = $settings;
  }

  /**
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
  public function view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $elements = array();

    return $elements;
  }

  /**
   * When editing the field through the UI.
   */
  public function widget(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
    // todo: Handle this later.
  }
}