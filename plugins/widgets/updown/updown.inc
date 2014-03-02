<?php

/**
 * @file
 * Up Down Widget.
 */

/**
 * Plugin decleration.
 */
$plugin = array(
  'title' => t('Default'),
  'description' => t('Default widget type'),
  'class' => 'UpDown',
  'assets' => array(
    'css' => array('updown.css'),
  ),
);

class UpDown extends VoteUpDown {
}
