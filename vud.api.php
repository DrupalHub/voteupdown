<?php

/**
 * @file
 * Hooks documentations.
 */

/**
 * Allow modules to alter access to the voting operation.
 *
 * @param $perm
 *  String containing the permission required to modify the vote.
 * @param $entity_type
 *  String containing the type of content being voted on.
 * @param $entity_id
 *  Integer containing the unique ID of the content being voted on.
 * @param $value
 *  Integer containing the vote value, 1 for an up vote, -1 for a down vote.
 * @param $tag
 *  String containing the voting API tag.
 * @param $account
 *  Object containing the user voting on the content, NULL for the current user.
 *
 * @return Bool
 *  A boolean forcing access to the vote, pass NULL if the function should not
 *  modify the access restriction.
 */
function hook_vud_access($perm, $entity_type, $entity_id, $value, $tag, $account) {
  // Denies access for all users other than user 1.
  if ($account->uid != 1) {
    return FALSE;
  }
}