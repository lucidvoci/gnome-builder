####################
Changing Indentation
####################

To change the indentation rules you have two options.
Either globally for your system, or for just the current project.

Project-Wide
------------

If you would like to change settings for just your project, use a ``.editorconfig`` file.
You can add a ``.editorconfig`` file to the root of your project in the editorconfig_ format.

It looks something like:

.. code-block:: ini

   root = true

   [*]
   charset = utf-8
   end_of_line = lf

Globally
--------

If you would like to change the indentation rules for your user, and thereby all projects which do not contain an ``.editorconfig`` file, use the application preferences.
You can access the preferences through the perspective selector or using the ``Command+,`` keyboard shortcut.

First select “Programming Languages” from the sidebar on the left.
Then select the programming language from the list of options.
On the right you will now see a list of preferences that may be tweaked for that language.
Change the indentation level to your desired preference.

.. _editorconfig: http://editorconfig.org/
