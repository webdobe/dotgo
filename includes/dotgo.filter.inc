<?php

/**
 * Implements hook_filter_info().
 */
function dotgo_filter_info() {
  $filters = array();
  $filters['dotgo'] = array(
    'title' => t('DOTGO'),
    'description' => t('DOTGO Filters'),
    //'prepare callback' => '_dotgo_filter_prepare',
    'process callback' => '_dotgo_filter_process',
    //'tips callback' => '_dotgo_filter_tips',
  );
  return $filters;
}

/**
 * Implements filter prepare callback.
 */
function _dotgo_filter_prepare() {

}

/**
 * Implements filter process callback.
 *
 * There are some issues here.  Some entities still get through.
 * Anchor tag's need work, it would be great to format an html anchor to
 * a Dotgo Anchor or URL depending on internal or external.
 */
function _dotgo_filter_process($text) {
  $filtered = _filter_autop($text);

  $filtered = preg_replace("/<p[^>]*?>/", "", $filtered);
  $filtered = str_replace("</p>", "<br />", $filtered);
  $filtered = preg_replace('/<br\s*?\/?>\s*$/', '', $filtered);
  $filtered = preg_replace('/&nbsp;/', ' ', $filtered);
  $filtered = preg_replace('/&nbsp;/', ' ', $filtered);
  $filtered = preg_replace('/\s+/', ' ', $filtered);

  $filter = new stdClass();
  $filter->settings['allowed_html'] = '<a> <br /> <url> <block> <query> <follow>';
  $filter->settings['filter_html_nofollow'] = 0;
  $filtered = _filter_html($filtered, $filter);

  return $filtered;
}

/**
 * Implements filter tips callback.
 */
function _dotgo_filter_tips() {
}