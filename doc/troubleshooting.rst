###############
Troubleshooting
###############

If you are having trouble with Builder you can help us help you by trying to do some basic troubleshooting.
Here are some steps you can go through to try to discover what is going wrong.

Verbose Output
--------------

You can increase the log verbosity of Builder by adding up to four ``-v`` when launching from the command line.

.. code-block:: sh

   # If running from flatpak
   flatpak run org.gnome.Builder -vvvv

   # If using distribution packages
   gnome-builder -vvvv

Support Log
-----------

Builder has support to generate a support log which can provide us with details.
From the application menu, select “Generate Support Log”.
It will place a log file in your home directory.

Counters
--------

Builder has internal counters which can be useful to debug problems.
Use the command bar (activated by ``Control+Enter``) and type “counters” followed by ``Enter``.
This will bring up a new window containing the current values of the counters.

If Builder has locked up, you can access the counters from outside of Builder.
The command line tool ``dazzle-list-counters``, can be used to access the counters.

.. code-block:: sh

   dazzle-list-counters `pidof gnome-builder`

.. note:: When running Builder from Flatpak, we do not currently expose the counters to the host. Use ``flatpak enter $PID /bin/bash`` to enter the mount namespace and then run ``dazzle-list-counters``.

Test Builder Nightly
--------------------

If you are running the stable branch or an older distribution package, please consider trying our Nightly release to see if the bug has already been fixed.
Doing this before reporting bugs helps reduce the amount of bug traffic we need to look at.
We'll usually ask you to try Nightly anyway before continuing the troubleshooting process.

See :ref:`installing from Flatpak<via_flatpak>` for installation notes.

File a Bug
----------

We can help you troubleshoot!
File a bug if you're stuck and we can help you help us.

See the `Builder Bugzilla`_ for creating a bug report.

.. _`Builder Bugzilla`: https://bugzilla.gnome.org/enter_bug.cgi?product=gnome-builder&component=general
