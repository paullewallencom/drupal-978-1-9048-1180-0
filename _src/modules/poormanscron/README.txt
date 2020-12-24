$Id: README.txt,v 1.4 2005/11/01 15:18:48 uwe Exp $

Description
-----------

Poormanscron is a module which runs the Drupal cron operations without
needing the cron application.

For every page view, this module checks to see if our last cron run was more
than 1 hour ago (this period is configurable). If so, the cron hooks are
executed (which, for example, update RSS/syndication blocks), and Drupal
is happy. These cron hooks fire after all HTML is returned to the browser,
so the user who kicks off the cron jobs should not notice any delay.


Requirements
------------

This module requires Drupal 4.7 or a later version.


Installation
------------

1) Copy/upload the poormanscron.module to the modules/ directory of your
   Drupal installation.
2) Enable the poormanscron module in Drupal (administer -> modules).


Configuration
-------------

Poormanscron can be configured at administer -> settings -> poormanscron.


Authors
-------

 * Moshe Weitzman <weitzman@tejasa.com> - original author
 * Uwe Hermann <uwe@hermann-uwe.de> - current maintainer

