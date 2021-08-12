.. Pancake documentation master file, created by
   sphinx-quickstart on Sat Jul 10 13:38:13 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

===================================
Welcome to Pancake's documentation!
===================================
Pancake is a C++ interface to libsm64 (included with Wafel).

.. toctree::
   :maxdepth: 2
   :caption: Contents:

.. _about_accessor_expressions:

A preface on accessor expressions
---------------------------------
Accessor expressions are the core method you'll be using to query variables from the game.
I aim to support the same syntax as Wafel API's data paths at some point, but for now, this is how it works:

- Use ``arr[x]`` to get element ``x`` of array ``arr``
- Use ``mytype.y``/``mytype->y`` to get member ``y`` of struct/union ``mytype``
- There is no difference between ``.`` and ``->``, Pancake auto-detects struct pointers and dereferences them
- Current limitations only allow base types to be retrieved this way, no struct/array/union support just yet
- Macro constants and object fields are macro'd in like they would be in C/C++, unlike Wafel. This is done to simplify the parser.

Indices and tables
------------------

* :ref:`genindex`
* :ref:`search`

Overview
^^^^^^^^^

.. toctree::
  self
  api/index