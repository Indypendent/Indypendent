-- Information --
The DART module provides DART ad tag support for Drupal sites that use Doubleclick as their ad delivery network. DART is now known as "DoubleClick for Advertisers"

-- Requirements & Enhancements --
Some of the information required by this module must be obtained from Doubleclick. See doubleclick.com for more details.

* The 6.x-2.x Branch requires ctools
* The 6.x-2.x Branch integrates with the contexts module for "display rules"
* Although not required, all of the admin screens look *MUCH* better and are *MUCH* more usable if you use vertical tabs
* The dart_taxonomy module requires taxonomy_manager

-- Usage --
You can create DART tags and display them either as a block or by inserting a simple print dart_tag('banner'); into your theme. Every DART tag can include several different type of key|val pairs:

- Static key|val pairs
These pairs do not ever change programmatically. They can be added to all tags globally or to individual tags.

- Evaluated key|val pairs
These key|val pairs have an "evaluate as javascript checkbox". For example a value like i++ would need to be evaluated as javascript.

- Programmatic key|val pairs
These key|value pairs can be added by using hook_dart_get_vars(). These pairs typically have a different value on each page request (often based on URL, or taxonomy terms, or users) and can be set by using the dart_get_vars hook. (ex. section)
For more complex implementations module developers and themers can take advantage of these hooks to customize the dart settings on each page request:

hook_dart_tag_alter(&$tag) - this is called immediately after a tag is loaded
hook_dart_key_vals($tag) - use this to add/alter a key|value pair to a tag.
hook_dart_taxonomy_callbacks() - add your own handler for how taxonomy terms are displayed within tags (dart_taxonomy module only)
hook_dart_tag_settings_data_structure_alter(&$structure) - use this to tell the dart module to include another piece of tag-specific data in the database (advanced users only)

- JS Overrides
Finally, you can override a completely rendered tag at the very last second by including the following js in your module:

  $(document).bind('dart_tag_render', function(event, tag){
    // modify tag however you see fit and then return it.
    // example: 

    tag = tag + '12345';
    return tag;
  });

