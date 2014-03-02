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
   * The field info.
   * @var
   */
  protected $field;

  /**
   * The field instance settings.
   *
   * @var
   */
  protected $instance;

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
   * Get the field name and the field name and field instance.
   *
   * @param $entity_type
   *  The entity type.
   * @param $entity
   *  The entity object or ID.
   */
  public function fieldInfoInstance($entity_type, $entity) {
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $this->field = VoteUpDown::findField($wrapper->type(), $wrapper->value());
    $this->instance = field_info_instance($wrapper->type(), $this->field, $wrapper->getBundle());
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
  public function validate($entity_type, $entity_id, $value, $account = NULL) {
    return TRUE;
  }

  /**
   * @{@inheritdoc}
   */
  public function view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    $elements = array();
    $wrapper = entity_metadata_wrapper($entity_type, $entity);

    $elements[0] = array(
      '#markup' => theme('vote_up_down_' . $this->settings['name'], array(
        'entity_id' => $wrapper->getIdentifier(),
        'entity_type' => $wrapper->type(),
      )),
    );

    return $elements;
  }

  /**
   * @{@inheritdoc}
   */
  public function widget(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
    $element += array(
      'user' => array(
        '#type' => 'textfield',
        '#title' => t('User'),
        '#autocomplete_path' => 'vote_up_down/users-autocomplete',
      ),
      'value' => array(
        '#type' => 'textfield',
        '#title' => t('Value'),
      ),
      '#element_validate' => array('vote_up_down_widget_validate'),
    );

    if (empty($element['#entity'])) {
      $element['#widget'] = TRUE;
      return $element;
    }

    $votes = new VoteUpDown($element['#entity_type'], $element['#entity']);
    $all_votes = $votes->getVotes();
    $vote = empty($items[$delta]['voteid']) ? 0 : $all_votes[$items[$delta]['voteid']];

    $element['#vote'] = $vote;

    if ($vote) {
      $element['user']['#default_value'] = user_load($vote->uid)->name . ' (' . $vote->uid . ')';
      $element['value']['#default_value'] = $vote->value;
    }

    return $element;
  }

  /**
   * @{@inheritdoc}
   */
  public function preprocess(&$variables) {
    // Default value of the variables.
    $variables += array(
      'widget_message_code' => VOTE_UP_DOWN_WIDGET_MESSAGE_DENIED,
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

    // Get default value of the variables.
    $entity_type = $variables['entity_type'];
    $entity_id = $variables['entity_id'];
    $tag = $variables['tag'];
    $readonly = $variables['readonly'];
    $widget_theme = $this->settings['name'];

    // Adding libraries.
    drupal_add_library('system', 'drupal.ajax');
    ctools_add_js('ajax-responder');
    ctools_include('ajax');

    // Define access per operation, for example when the vote up operation is
    // active this means the down operation should be allowed, as the up vote
    // was already.
    $up_access = $down_access = $reset_access = FALSE;

    if (user_access('use vote up/down')) {
      $criteria = array(
        'entity_type' => $entity_type,
        'entity_id' => $entity_id,
        'tag' => $variables['tag'],
      ) + votingapi_current_user_identifier();

      $previous_vote = votingapi_select_single_vote_value($criteria);
      $user_vote = isset($previous_vote) ? $previous_vote : 0;

      if (!$readonly) {
        $up_access = $user_vote <= 0;
        $down_access = $user_vote >= 0;
        $reset_access = ($user_vote != 0) && vote_up_down_access_callback('reset vote up/down votes', $entity_type, $entity_id, NULL, $tag);

        if (!$up_access && !$down_access) {
          // Although implementing modules have requested a not readonly widget,
          // we prevent access.
          $readonly = $variables['readonly'] = TRUE;
        }
      }
    }

    // Prepare links only if it's not readonly, or if readonly but we need to
    // show a message on deny.
    $message_on_deny = variable_get('vud_message_on_deny', FALSE);

    if (!$readonly || ($readonly && $message_on_deny)) {
      // Links should be displayed.
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
      // This is not a read only mode. Allow the user to vote.
      if ($up_access) {
        $token_up = drupal_get_token("vote/$entity_type/$entity_id/1/$tag/$widget_theme");
        $variables['link_up'] = url("vote/$entity_type/$entity_id/1/$tag/$widget_theme/$token_up/nojs");
        $variables['link_class_up'] .= ' use-ajax';
      }

      if ($down_access) {
        $token_down = drupal_get_token("vote/$entity_type/$entity_id/-1/$tag/$widget_theme");
        $variables['link_down'] = url("vote/$entity_type/$entity_id/-1/$tag/$widget_theme/$token_down/nojs");
        $variables['link_class_down'] .= ' use-ajax';
      }

      if ($reset_access) {
        $token_reset = drupal_get_token("votereset/$entity_type/$entity_id/$tag/$widget_theme");
        $variables['link_reset'] = url("votereset/$entity_type/$entity_id/$tag/$widget_theme/$token_reset");
        $variables['link_class_reset'] .= ' use-ajax';
      }
    }
    elseif ($readonly && $message_on_deny) {
      // Readonly widget, but with a deny message.
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
      // Traces of 7.x-1.x branch, don't blame me.
      $widget_message_code = VOTE_UP_DOWN_WIDGET_MESSAGE_DENIED;

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
      'entity_type' => $entity_type,
      'entity_id' => $entity_id,
      'value_type' => 'points',
      'tag' => $tag,
      'function' => 'sum',
    );

    $raw_points = votingapi_select_single_result_value($result_criteria);
    $variables['raw_points'] = $raw_points;
    $vote_result = (int)$raw_points;

    $criteria = array(
      'entity_type' => $entity_type,
      'entity_id' => $entity_id,
      'value_type' => 'points',
      'tag' => $tag,
      'function' => 'count',
    );

    $vote_count = (int)votingapi_select_single_result_value($criteria);

    $variables['vote_count'] = $vote_count;
    $variables['unsigned_points'] = $vote_result;

    if ($vote_result > 0) {
      $variables['class'] = 'positive';
      $variables['points'] = '+' . $vote_result;
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
    $variables['vote_sum'] = ($vote_result) ? $vote_result : 0;
  }
}