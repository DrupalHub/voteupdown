<?php

/**
 * Most of the work for the widgets handlers will be done here.
 */
abstract class voteUpDownBase implements voteUpDownInterface {

  /**
   * Holding the plugin definition.
   *
   * @var array
   */
  public $settings;

  /**
   * Constructing the class.
   *
   * @param $settings
   *  The settings of the widget.
   */
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
   * @{@inheritdoc}
   */
  public function validate() {
  }

  /**
   * @{@inheritdoc}
   */
  public function view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $elements = array();

    foreach ($items as $delta => $item) {
    }

    $wrapper = entity_metadata_wrapper($entity_type, $entity);

    $elements[0] = array(
      '#markup' => theme('vote_up_down_' . $this->settings['name'], array(
        'entity_id' => $wrapper->getIdentifier(),
        'entity_type' => $wrapper->getBundle(),
      )),
    );

    return $elements;
  }

  /**
   * @{@inheritdoc}
   */
  public function widget(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
    // todo: Handle this later.
  }

  /**
   * @{@inheritdoc}
   */
  public function preprocess(&$variables) {
    $variables += array(
      'widget_message_code' => 0,
      'readonly' => FALSE,
      'tag' => variable_get('vud_tag', 'vote'),
      'id' => 'widget-' . $variables['entity_type'] . '-' . $variables['entity_id'],
      'link_class_up' => 'vud-link-up',
      'link_class_down' => 'vud-link-down',
      'link_class_reset' => 'vud-link-reset',
      'link_up' => '',
      'link_down' => '',
      'class_up' => 'up-active',
      'class_down' => 'down-active',
      'class_reset' => 'reset-inactive',
      'show_links' => FALSE,
      'show_reset' => FALSE,
      'show_up_as_link' => FALSE,
      'show_down_as_link' => FALSE,
      'reset_long_text' => t('Reset your vote'),
      'reset_short_text' => t('(reset)'),
    );

    drupal_add_library('system', 'drupal.ajax');
    ctools_add_js('ajax-responder');
    ctools_include('ajax');


    // for now.
    return;

    // Define access per operation, for example when the vote up operation is
    // active this means the down operation should be allowed, as the up vote was
    // already.
    $up_access = $down_access = $reset_access = FALSE;
    if (user_access('use vote up/down')) {
      $criteria = array(
          'entity_type' => $type,
          'entity_id' => $entity_id,
          'tag' => $variables['tag'],
        ) + votingapi_current_user_identifier();
      $previous_vote = votingapi_select_single_vote_value($criteria);

      $user_vote = isset($previous_vote) ? $previous_vote : 0;

      if (!$readonly) {

        $up_access = $user_vote <= 0;
        $down_access = $user_vote >= 0;
        $reset_access = ($user_vote != 0) && vud_access_callback('reset vote up/down votes', $type, $entity_id, NULL, $tag);

        if (!$up_access && !$down_access) {
          // Although implementing modules have requested a not readonly widget,
          // we prevent access.
          $readonly = $variables['readonly'] = TRUE;
        }
      }
    }

    // Prepare links only if it's not readonly, or if readonly but we need to show
    // a messgae on deny.
    $message_on_deny = variable_get('vud_message_on_deny', FALSE);
    if (!$readonly || ($readonly && $message_on_deny)) {
      // Widget needs to show links.
      if ($up_access) {
        $variables['class_up'] = 'up-inactive';
        $variables['show_up_as_link'] = TRUE;
      }
      if ($down_access) {
        $variables['class_down'] = 'down-inactive';
        $variables['show_down_as_link'] = TRUE;
      }
      if ($reset_access) {
        $variables['show_reset'] = TRUE;
        $variables['class_reset'] = 'reset-active';
      }
      $variables['show_links'] = TRUE;
    }

    if (!$readonly) {
      // Widget has link(s).
      if ($up_access) {
        $token_up = drupal_get_token("vote/$type/$entity_id/1/$tag/$widget_theme");
        $variables['link_up'] = url("vote/$type/$entity_id/1/$tag/$widget_theme/$token_up/nojs");
        $variables['link_class_up'] .= ' use-ajax';
      }
      if ($down_access) {
        $token_down = drupal_get_token("vote/$type/$entity_id/-1/$tag/$widget_theme");
        $variables['link_down'] = url("vote/$type/$entity_id/-1/$tag/$widget_theme/$token_down/nojs");
        $variables['link_class_down'] .= ' use-ajax';
      }
      if ($reset_access) {
        $token_reset = drupal_get_token("votereset/$type/$entity_id/$tag/$widget_theme");
        $variables['link_reset'] = url("votereset/$type/$entity_id/$tag/$widget_theme/$token_reset");
        $variables['link_class_reset'] .= ' use-ajax';
      }
    }
    elseif ($readonly && $message_on_deny) {
      // Readonly widget, but with a deny messgae.
      ctools_include('modal');
      ctools_modal_add_js();
      if ($up_access) {
        $variables['link_class_up'] .= ' denied ctools-use-modal';
      }
      else {
        $variables['link_class_down'] .= ' denied ctools-use-modal';
      }
      if ($up_access || $down_access) {
        $variables['link_class_reset'] .= ' denied ctools-use-modal';
      }

      // TODO: Get default $widget_message_code or one from $variables.
      $widget_message_code = VUD_WIDGET_MESSAGE_DENIED;
      $variables['link_up'] = url(sprintf('vud/nojs/denied/%d', $widget_message_code));
      $variables['link_down'] = url(sprintf('vud/nojs/denied/%d', $widget_message_code));
      $variables['link_reset'] = url(sprintf('vud/nojs/denied/%d', $widget_message_code));
    }
    else {
      // Readonly widget.
      $variables['link_up'] = '#';
      $variables['link_down'] = '#';
      $variables['link_reset'] = '#';
    }

    $result_criteria = array(
      'entity_type' => $type,
      'entity_id' => $entity_id,
      'value_type' => 'points',
      'tag' => $tag,
      'function' => 'sum'
    );
    $raw_points = votingapi_select_single_result_value($result_criteria);
    $variables['raw_points'] = $raw_points;
    $vote_result = (int)$raw_points;

    $criteria = array(
      'entity_type' => $type,
      'entity_id' => $entity_id,
      'value_type' => 'points',
      'tag' => $tag,
      'function' => 'count'
    );
    $vote_count = (int)votingapi_select_single_result_value($criteria);
    $variables['vote_count'] = $vote_count;

    $variables['unsigned_points'] = $vote_result;

    if ($vote_result > 0) {
      $variables['class'] = 'positive';
      $variables['points'] = '+'. $vote_result;
    }
    else {
      $variables['points'] = $vote_result;
      if ($vote_result < 0) {
        $variables['class'] = 'negative';
      }
      else {
        $variables['class'] = 'neutral';
      }
    }
    $variables['vote_label'] = format_plural(abs($vote_result), 'vote', 'votes');
  }
}