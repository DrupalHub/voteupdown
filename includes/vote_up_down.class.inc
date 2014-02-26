<?php

/**
 * Interface for the widgets of the vote up down field.
 */
interface voteUpDownInterface {

  /**
   * Instance settings.
   */
  public function instanceSettings($form, $form_state);

  /**
   * Validating the value before saving the data to the DB.
   */
  public function validate();

  /**
   * Displaying the values of the form.
   */
  public function view();

  /**
   * When editing the field through the UI.
   */
  public function edit();
}