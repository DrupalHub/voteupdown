<?php
/**
 * @file
 *   Thumbs widget.
 */

$plugin = array(
  'title' => t('Thumbs up and down'),
  'description' => t('Thumbs up and down widget.'),
  'class' => 'voteUpDownThumbs',
  'assets' => array(
    'js' => array('thumbs.js'),
    'css' => array('thumbs.css'),
  ),
);

class voteUpDownThumbs extends voteUpDownBase {

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
   * @{@inheritdoc}
   */
  public function preprocess(&$variables) {
    parent::preprocess($variables);

    // Get some settings.
    if (!$this->instance) {
      $this->fieldInfoInstance($variables['entity_type'], $variables['entity_id']);
    }

    $appearance = $this->instance['settings']['appearance'];
    if ($appearance == 'both') {
      // Display up and down. Return early.
      return;
    }

    if ($appearance == 'down_only') {
      $variables['show_up_as_link'] = FALSE;
      $variables['class_up'] = '';
    }

    if ($appearance == 'up_only') {
      $variables['show_down_as_link'] = FALSE;
      $variables['class_down'] = '';
    }
  }
}
