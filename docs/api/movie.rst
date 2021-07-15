.. _api_movie:

movie.hpp
==========
Classes to read (write coming soon) data from Mupen64 input files.

.. cpp:namespace:: pancake
.. cpp:class:: m64 final

  A Mupen64 input movie.  
  I plan to add metadata access and saving soon, stay tuned!
  
  .. cpp:enum-class:: button : uint16_t
  
    An enum representing the button bitmask.
    One can apply bitwise operators to it.
    
    .. cpp:enumerator:: A = 0x8000
      
      The A button.
    
    .. cpp:enumerator:: B = 0x4000
      
      The B button.
    
    .. cpp:enumerator:: Z = 0x2000
      
      The Z button.
    
    .. cpp:enumerator:: START = 0x1000
      
      The START button.
      
    .. cpp:enumerator:: D_UP = 0x0800
      
      The up button on the D-pad.
    
    .. cpp:enumerator:: D_DOWN = 0x0400
      
      The down button on the D-pad.
    
    .. cpp:enumerator:: D_LEFT = 0x0200
      
      The left button on the D-pad.
    
    .. cpp:enumerator:: D_RIGHT = 0x0100
      
      The right button on the D-pad.
    
    .. cpp:enumerator:: L = 0x0020
      
      The L button.
    
    .. cpp:enumerator:: R = 0x0010
      
      The R button.
    
    .. cpp:enumerator:: C_UP = 0x0008
      
      The C-up (also written C^) button.
    
    .. cpp:enumerator:: C_DOWN = 0x0004
      
      The C-down (also written Cv) button.
      
    .. cpp:enumerator:: C_LEFT = 0x0002
      
      The C-left (also written C\< or in TASInput as C) button.
    
    .. cpp:enumerator:: C_RIGHT = 0x0001
      
      The C-right (also written C\>) button.
    
  .. cpp:struct:: frame
  
    A single input frame.
    
    .. cpp:member:: m64::button buttons
      
      A bitmask representing buttons.
      
    .. cpp:member:: int8_t stick_x
    
      The joystick's X-value as a signed byte.
      
    .. cpp:member:: int8_t stick_y
    
      The joystick's Y-value as a signed byte.
      
    .. cpp:function:: void apply(sm64& game)
    
      Sets the input variables on the libsm64 instance to the inputs of this frame.
      
      :param sm64& game: The game to set inputs on
  
  **Frame list operations**
  .. cpp:type: iterator
    
    An iterator over the frames of this M64.
    
  .. cpp:type: const_iterator
  
    A const iterator over the frames of this M64.
    
  .. cpp:function:: m64::frame& operator[](uint32_t index)
    
    Returns a reference to the ``index``th input frame of this M64.  
    Does not check input bounds.
    
    :param uint32_t index: The index of the frame to get
    :return: the requested frame
  
  .. cpp:function m64::frame& at(uint32_t index)
    
    Returns a reference to the ``index``th input frame of this M64.  
    Unlike :cpp:function:`operator[]()`, ``at()`` checks input bounds.
    
    :param uint32_t index: The index of the frame to get
    :return: the requested frame
    :throws std::out_of_range: if ``index >= this->size()``
    
  .. cpp:function:: m64::frame& front()
  
    Returns a reference to the first frame of this M64.  
    Equivalent to ``(*this)[0]`` or ``*(this->begin())``.
    
    :return: the first frame of this M64
  
  .. cpp:function:: m64::frame& back()
  
    Returns a reference to the last frame of this M64.  
    Equivalent to ``(*this)[this->size() - 1]`` or ``*(this->end())``.
    
    :return: the first frame of this M64
    
  .. cpp:function:: uint32_t size()
  
    Returns the number of frames in this M64.
    
    :return: the number of frames
    
  .. cpp:function:: iterator begin()
  
    Returns an iterator at the start of this M64.
    
    :return: an iterator at the start of this M64.
    
  .. cpp:function:: iterator end()
  
    Returns an iterator at the end of this M64.
    
    :return: an iterator at the end of this M64.
    
  .. cpp:function:: const_iterator begin() const
  
    Returns a const iterator at the start of this M64.
    
    :return: a const iterator at the start of this M64.
    
  .. cpp:function:: const_iterator end() const
  
    Returns a const iterator at the end of this M64.
    
    :return: a const iterator at the end of this M64.
    
  .. cpp:function:: const_iterator cbegin() const
  
    Returns a const iterator at the start of this M64.
    
    :return: a const iterator at the start of this M64.
    
  .. cpp:function:: const_iterator cend() const
  
    Returns a const iterator at the end of this M64.
    
    :return: a const iterator at the end of this M64.
    
  .. cpp:function:: void push_back(const m64::frame& frame)
    
    Appends a frame onto the end of this M64.
    
    :param const m64::frame& frame: The frame to append. Will be copied.
    
  .. cpp:function:: void push_back(m64::frame&& frame)
  
    Appends a frame onto the end of this M64.
    
    :param m64::frame&& frame: The frame to append. Will be moved.
    
  .. cpp:function:: void pop_back()
    
    Removes the last frame of this M64.