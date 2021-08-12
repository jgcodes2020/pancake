.. _api_movie:

movie.hpp
==========
Classes to read and write data from Mupen64 input files.

.. cpp:namespace:: pancake
.. cpp:struct:: frame
  
  Represents a frame of input.
  
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

  A single input frame.
  
  .. cpp:member:: m64::button buttons
    
    A bitmask representing buttons.
    
  .. cpp:member:: int8_t stick_x
  
    The joystick's X-value as a signed byte.
    
  .. cpp:member:: int8_t stick_y
  
    The joystick's Y-value as a signed byte.
    
  .. cpp:function:: void apply(sm64& game)
  
    Sets the input variables on the libsm64 instance to the inputs of this frame.
    
    :param game: The game to set inputs on
.. cpp:class:: m64 final

  A Mupen64 input movie.  
  I plan to add metadata access and saving soon, stay tuned!
  
  .. cpp:function:: m64(std::filesystem::path path)
  
    Loads an .m64 from a file path.
  
  **Frame list operations**
  
  .. cpp:type:: iterator
    
    An iterator over the frames of this M64.
    
  .. cpp:type:: const_iterator
  
    A const iterator over the frames of this M64.
    
  .. cpp:function:: m64::frame& operator[](uint32_t index)
  .. cpp:function:: const m64::frame& operator[](uint32_t index) const
    
    Returns a reference to the :cpp:expr:`index`\th input frame of this M64.  
    Does not check input bounds.
    
    :param index: The index of the frame to get
    :return: the requested frame

  .. cpp:function:: m64::frame& at(uint32_t index)
  .. cpp:function:: const m64::frame& at(uint32_t index) const
    
    Returns a reference to the :cpp:expr:`index`\th input frame of this M64.  
    Unlike :cpp:func:`m64::operator[]()`, :cpp:expr:`at()` checks input bounds.
    
    :param index: The index of the frame to get
    :return: the requested frame
    :throws std::out_of_range: if :cpp:expr:`index >= this->size()`
    
  .. cpp:function:: m64::frame& front()
  .. cpp:function:: const m64::frame& front() const
  
    Returns a reference to the first frame of this M64.
    Equivalent to :cpp:expr:`(*this)[0]` or :cpp:expr:`*(this->begin())`.
    
    :return: the first frame of this M64
  
  .. cpp:function:: m64::frame& back()
  .. cpp:function:: const m64::frame& back() const
  
    Returns a reference to the last frame of this M64.
    Equivalent to :cpp:expr:`(*this)[this->size() - 1]` or :cpp:expr:`*(this->end())`.
    
    :return: the first frame of this M64
    
  .. cpp:function:: uint32_t size() const
  
    Returns the number of frames in this M64.
    
    :return: the number of frames
    
  .. cpp:function:: iterator begin()
  .. cpp:function:: const_iterator begin() const
  
    Returns an iterator at the start of this M64.
    
    :return: an iterator at the start of this M64.
    
  .. cpp:function:: iterator end()
  .. cpp:function:: const_iterator end() const
  
    Returns an iterator at the end of this M64.
    
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
    
    :param frame: The frame to append. Will be moved.
    
  .. cpp:function:: void pop_back()
    
    Removes the last frame of this M64.
  
  **Metadata and serialization**
  
  .. cpp:struct:: metadata_s
  
    A struct holding metadata about the M64.
    
    .. cpp:var:: uint32_t version
      
      The version of this M64. Should be 3, otherwise you have a REALLY old file.
      
    .. cpp:var:: uint32_t timestamp
    
      The recording timestamp. Used as a UID of sorts for linking to savestates.  
      I don't know the specifics of the whole UID thing, ask someone who works on Mupen64.
      
    .. cpp:var:: uint32_t num_vis
    
      The length of this M64 in VIs (visual frames). Hex editing tools will usually max this value out.
      
    .. cpp:var:: uint32_t rerecords
    
      The number of rerecords (times that a savestate was used).
      
    .. cpp:var:: uint8_t vis_per_s
    
      Essentially amounts to frames per second.
      
    .. cpp:var:: uint8_t num_controllers
    
      The number of controllers used.
      
    .. cpp:function:: uint32_t num_input_frames()
    
      Returns the number of input frames. This is a function rather than a reference
      in order to avoid copying issues.
      
    .. cpp:enum-class:: start_flags
    
      A flag deciding how to start this M64.
      
      .. cpp:enumerator:: FROM_SNAPSHOT = 0x01
      
        Starts this M64 from a savestate of the same name.
        
      .. cpp:enumerator:: FROM_START = 0x02
      
        Starts this M64 from reset.
        
      .. cpp:enumerator:: FROM_EEPROM = 0x04
      
        Not sure what this does, I just know that this is a thing in new Mupen.
        
    .. cpp:var:: start_flags start_type
    
      The start flag used by this M64.
      
    .. cpp:enum-class:: ctrler_flags
    
      A bitfield for controller info. One can perform bitwise operations on it.
      
      .. cpp:enumerator:: CONTROLLER1_PRESENT = 0x0001
      .. cpp:enumerator:: CONTROLLER2_PRESENT = 0x0002
      .. cpp:enumerator:: CONTROLLER3_PRESENT = 0x0004
      .. cpp:enumerator:: CONTROLLER4_PRESENT = 0x0008
      
        Indicates the presence of a controller.
      
      .. cpp:enumerator:: CONTROLLER1_MEMORY = 0x0010
      .. cpp:enumerator:: CONTROLLER2_MEMORY = 0x0020
      .. cpp:enumerator:: CONTROLLER3_MEMORY = 0x0040
      .. cpp:enumerator:: CONTROLLER4_MEMORY = 0x0080
      
        Indicates the presence of a controller's Memory Pak.
      
      .. cpp:enumerator:: CONTROLLER1_RUMBLE = 0x0100
      .. cpp:enumerator:: CONTROLLER2_RUMBLE = 0x0200
      .. cpp:enumerator:: CONTROLLER3_RUMBLE = 0x0400
      .. cpp:enumerator:: CONTROLLER4_RUMBLE = 0x0800
      
        Indicates the presence of a controller's Rumble Pak.
    
    .. cpp:var:: ctrler_flags controllers
    
      The controller flags for this controller.
      
    .. cpp:var:: std::string rom_name
    
      The internal ROM name, in ASCII.
      
      .. note::
      
        Only the first 31 characters will be serialized, if this field exceeds that length.
        
    .. cpp:var:: uint32_t crc
    
      The CRC of the ROM, taken directly from the ROM file. The field is read as little-endian during
      deserialization.
      
    .. cpp:var:: uint32_t country_code
    
      The country code of the ROM, taken directly from the ROM file. THe field is read as little-endian during
      deserialization.
      
    .. cpp:var:: std::string video_plugin
    .. cpp:var:: std::string input_plugin
    .. cpp:var:: std::string sound_plugin
    .. cpp:var:: std::string rsp_plugin
    
      The emulator plugins used (as ASCII strings).
      
      .. note::
      
        Only the first 63 characters of each of these fields will be serialized, should any of them 
        exceed that length.
    
    .. cpp:var:: std::string authors
    
      The authors of this M64.
      
      .. note::
      
        Only the first 221 characters of this field will be serialized, should this field exceed that length.
      
    .. cpp:var:: std::string description
    
      The description of this M64.
      
      .. note::
      
        Only the first 255 characters of this field will be serialized, should this field exceed that length.
        
  .. cpp:func:: void dump(std::filesystem::path path)
  
    Serializes this M64 to a file.
    
    .. note::
      
      Some metadata fields may be truncated to follow the spec.