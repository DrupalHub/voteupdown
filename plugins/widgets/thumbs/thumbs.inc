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
}
