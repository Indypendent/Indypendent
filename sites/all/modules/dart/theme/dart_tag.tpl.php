<?php

/**
 * @file
 * Display the js call to display a DART ad tag.
 *
 * Variables available:
 * - $tag: The full tag object or NULL. If it's NULL, all other
 *         vars listed below will be NULL as well
 * - $json_tag: a js version of $tag.
 * - $attributes: any attributes that should be displayed on the outer-most div.
 * - $show_script_tag: boolean.
 * - $show_noscript_tag: boolean.
 * - $noscript_tag: the <noscript> tag for this DART tag, or empty string.
 * - $static_tag: use this for DART tags that appear in emails.
 *
 * @see template_preprocess_dart_tag()
 */
?>

<div <?php print $attributes; ?>>
  <?php if ($tag->slug): ?>
    <span class="slug"><?php print $tag->slug; ?></span>
  <?php endif; ?>

  <?php if ($show_script_tag): ?>
    <?php if ($load_last): ?>
      <script type="text/javascript">Drupal.DART.settings.loadLastTags['<?php print $tag->machinename; ?>'] = '<?php print $json_tag; ?>';</script>
    <?php else: ?>
      <script type="text/javascript">Drupal.DART.tag('<?php print $json_tag; ?>');</script>
      <?php print $noscript_tag; ?>
    <?php endif; ?>
  <?php else: ?>
    <?php print $static_tag; ?>
  <?php endif; ?>
</div>
