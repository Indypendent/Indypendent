<div id="page-wrapper">

  <!-- Header -->
  <div id="header-wrapper">

    <!-- Region: Top Ad Banner -->
    <?php if ($page['top_banner']) : ?>
    <div id="top-banner-region">
      <?php print render($page['top_banner']); ?>
    </div>
    <?php endif; ?>

    <!-- Region: Header -->
    <?php if ($page['header']) : ?>
    <div id="header-region">
      <?php print render($page['header']); ?>
    </div>
    <?php endif; ?>

  </div>
  <!-- // Header -->

  <!-- Center Content -->
  <div id="main-wrapper">

    <!-- Edit/View Tabs -->
    <?php if ($tabs): ?>
      <div class="content-admin-tabs"><?php print render($tabs); ?></div>
    <?php endif; ?>    
    
    <!-- Region: Right Sidebar -->
    <?php if ($page['sidebar_right']) : ?>
    <div id="sidebar-right">
      <?php print render($page['sidebar_right']); ?>
    </div>
    <?php endif; ?>
    
    <!-- Region: Pre Content -->
    <?php if ($page['pre_content']) : ?>
    <div id="pre-content">
      <?php print render($page['pre_content']); ?>
    </div>
    <?php endif; ?>

    <!-- Region: Left Sidebar -->
    <?php if ($page['sidebar_left']) : ?>
    <div id="sidebar-left">
      <?php print render($page['sidebar_left']); ?>
    </div>
    <?php endif; ?>

    <div id="content">
    <!-- Region: Content -->
      <?php print render($page['content']); ?>
    </div>

  </div>
  <!-- // Center Content -->

  <div id="footer-region">
  <!-- Region: Footer -->
    <?php print render($page['footer']); ?>
  </div>

</div>

</body>
</html>