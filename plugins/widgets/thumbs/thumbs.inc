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
  'assets' => array(
    'js' => array('thumbs.js'),
    'css' => array('thumbs.css'),
  ),
  'theme' => 'vote_up_down_thumbs',
);

class voteUpDownThumbs implements voteUpDownInterface {

  public $settings;

  function __construct(array $settings = array()) {
    $this->settings = $settings;
  }

  /**
   * @{@inheritdoc}
   */
  public function instanceSettings($settings) {
    $form = array();

    $form['appearance'] = array(
      '#type' => 'radios',
      '#title' => t('Appearance'),
      '#description' => t('Choose how to widget will appear.'),
      '#options' => array(
        'up_only' => t('Only up'),
        'both' => t('Up and down'),
        'down_only' => t('Only down'),
      ),
      '#default_value' => empty($settings['appearance']) ? 'both' : $settings['appearance'],
    );

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

    $foo = theme('melingo_dictionary_term_translate_word');
    dpm($foo);

    foreach ($items as $delta => $item) {
      $elements[$delta] = array(
        '#markup' => 'a',
      );
    }

    return $elements;
  }

  /**
   * When editing the field through the UI.
   */
  public function widget(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
    // todo: Handle this later.
  }
}
