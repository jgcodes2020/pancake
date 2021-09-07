/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/

#ifndef _PANCAKE_DWARF_ENUMS_HPP_
#define _PANCAKE_DWARF_ENUMS_HPP_

#include <cstdint>
#include <iostream>
#include <stdexcept>

#undef ERROR

namespace pdwarf {
  struct return_code {
    enum {
      ok = 0,
      error = 1,
      no_entry = -1
    };
  };
  enum class attr_type : uint16_t {
    sibling = 0x01,
    location = 0x02,
    name = 0x03,
    ordering = 0x09,
    subscr_data = 0x0a,
    byte_size = 0x0b,
    bit_offset = 0x0c,
    bit_size = 0x0d,
    element_list = 0x0f,
    stmt_list = 0x10,
    low_pc = 0x11,
    high_pc = 0x12,
    language = 0x13,
    member = 0x14,
    discr = 0x15,
    discr_value = 0x16,
    visibility = 0x17,
    import = 0x18,
    string_length = 0x19,
    common_reference = 0x1a,
    comp_dir = 0x1b,
    const_value = 0x1c,
    containing_type = 0x1d,
    default_value = 0x1e,
    inline_ = 0x20,
    is_optional = 0x21,
    lower_bound = 0x22,
    producer = 0x25,
    prototyped = 0x27,
    return_addr = 0x2a,
    start_scope = 0x2c,
    bit_stride = 0x2e, 
    stride_size = 0x2e, 
    upper_bound = 0x2f,
    abstract_origin = 0x31,
    accessibility = 0x32,
    address_class = 0x33,
    artificial = 0x34,
    base_types = 0x35,
    calling_convention = 0x36,
    count = 0x37,
    data_member_location = 0x38,
    decl_column = 0x39,
    decl_file = 0x3a,
    decl_line = 0x3b,
    declaration = 0x3c,
    discr_list = 0x3d, 
    encoding = 0x3e,
    external = 0x3f,
    frame_base = 0x40,
    friend_ = 0x41,
    identifier_case = 0x42,
    macro_info = 0x43, 
    namelist_item = 0x44,
    priority = 0x45,
    segment = 0x46,
    specification = 0x47,
    static_link = 0x48,
    type = 0x49,
    use_location = 0x4a,
    variable_parameter = 0x4b,
    virtuality = 0x4c,
    vtable_elem_location = 0x4d,
    allocated = 0x4e, 
    associated = 0x4f, 
    data_location = 0x50, 
    byte_stride = 0x51, 
    stride = 0x51, 
    entry_pc = 0x52, 
    use_UTF8 = 0x53, 
    extension = 0x54, 
    ranges = 0x55, 
    trampoline = 0x56, 
    call_column = 0x57, 
    call_file = 0x58, 
    call_line = 0x59, 
    description = 0x5a, 
    binary_scale = 0x5b, 
    decimal_scale = 0x5c, 
    small_ = 0x5d, 
    decimal_sign = 0x5e, 
    digit_count = 0x5f, 
    picture_string = 0x60, 
    mutable_ = 0x61, 
    threads_scaled = 0x62, 
    explicit_ = 0x63, 
    object_pointer = 0x64, 
    endianity = 0x65, 
    elemental = 0x66, 
    pure = 0x67, 
    recursive = 0x68, 
    signature = 0x69, 
    main_subprogram = 0x6a, 
    data_bit_offset = 0x6b, 
    const_expr = 0x6c, 
    enum_class = 0x6d, 
    linkage_name = 0x6e, 
    string_length_bit_size = 0x6f, 
    string_length_byte_size = 0x70, 
    rank = 0x71, 
    str_offsets_base = 0x72, 
    addr_base = 0x73, 
    rnglists_base = 0x74, 
    dwo_id = 0x75, 
    dwo_name = 0x76, 
    reference = 0x77, 
    rvalue_reference = 0x78, 
    macros = 0x79, 
    call_all_calls = 0x7a, 
    call_all_source_calls = 0x7b, 
    call_all_tail_calls = 0x7c, 
    call_return_pc = 0x7d, 
    call_value = 0x7e, 
    call_origin = 0x7f, 
    call_parameter = 0x80, 
    call_pc = 0x81, 
    call_tail_call = 0x82, 
    call_target = 0x83, 
    call_target_clobbered = 0x84, 
    call_data_location = 0x85, 
    call_data_value = 0x86, 
    noreturn = 0x87, 
    alignment = 0x88, 
    export_symbols = 0x89, 
    deleted = 0x8a, 
    defaulted = 0x8b, 
    loclists_base = 0x8c
  };
  // Generated using VSCode find and replace
  inline std::ostream& operator<<(std::ostream& out, attr_type attr) {
    switch (attr) {
      case attr_type::sibling: {
        return out << "sibling";
      } break;
      case attr_type::location: {
        return out << "location";
      } break;
      case attr_type::name: {
        return out << "name";
      } break;
      case attr_type::ordering: {
        return out << "ordering";
      } break;
      case attr_type::subscr_data: {
        return out << "subscr_data";
      } break;
      case attr_type::byte_size: {
        return out << "byte_size";
      } break;
      case attr_type::bit_offset: {
        return out << "bit_offset";
      } break;
      case attr_type::bit_size: {
        return out << "bit_size";
      } break;
      case attr_type::element_list: {
        return out << "element_list";
      } break;
      case attr_type::stmt_list: {
        return out << "stmt_list";
      } break;
      case attr_type::low_pc: {
        return out << "low_pc";
      } break;
      case attr_type::high_pc: {
        return out << "high_pc";
      } break;
      case attr_type::language: {
        return out << "language";
      } break;
      case attr_type::member: {
        return out << "member";
      } break;
      case attr_type::discr: {
        return out << "discr";
      } break;
      case attr_type::discr_value: {
        return out << "discr_value";
      } break;
      case attr_type::visibility: {
        return out << "visibility";
      } break;
      case attr_type::import: {
        return out << "import";
      } break;
      case attr_type::string_length: {
        return out << "string_length";
      } break;
      case attr_type::common_reference: {
        return out << "common_reference";
      } break;
      case attr_type::comp_dir: {
        return out << "comp_dir";
      } break;
      case attr_type::const_value: {
        return out << "const_value";
      } break;
      case attr_type::containing_type: {
        return out << "containing_type";
      } break;
      case attr_type::default_value: {
        return out << "default_value";
      } break;
      case attr_type::inline_: {
        return out << "inline_";
      } break;
      case attr_type::is_optional: {
        return out << "is_optional";
      } break;
      case attr_type::lower_bound: {
        return out << "lower_bound";
      } break;
      case attr_type::producer: {
        return out << "producer";
      } break;
      case attr_type::prototyped: {
        return out << "prototyped";
      } break;
      case attr_type::return_addr: {
        return out << "return_addr";
      } break;
      case attr_type::start_scope: {
        return out << "start_scope";
      } break;
      case attr_type::bit_stride: {
        return out << "bit_stride/stride_size";
      } break;
      case attr_type::upper_bound: {
        return out << "upper_bound";
      } break;
      case attr_type::abstract_origin: {
        return out << "abstract_origin";
      } break;
      case attr_type::accessibility: {
        return out << "accessibility";
      } break;
      case attr_type::address_class: {
        return out << "address_class";
      } break;
      case attr_type::artificial: {
        return out << "artificial";
      } break;
      case attr_type::base_types: {
        return out << "base_types";
      } break;
      case attr_type::calling_convention: {
        return out << "calling_convention";
      } break;
      case attr_type::count: {
        return out << "count";
      } break;
      case attr_type::data_member_location: {
        return out << "data_member_location";
      } break;
      case attr_type::decl_column: {
        return out << "decl_column";
      } break;
      case attr_type::decl_file: {
        return out << "decl_file";
      } break;
      case attr_type::decl_line: {
        return out << "decl_line";
      } break;
      case attr_type::declaration: {
        return out << "declaration";
      } break;
      case attr_type::discr_list: {
        return out << "discr_list";
      } break;
      case attr_type::encoding: {
        return out << "encoding";
      } break;
      case attr_type::external: {
        return out << "external";
      } break;
      case attr_type::frame_base: {
        return out << "frame_base";
      } break;
      case attr_type::friend_: {
        return out << "friend_";
      } break;
      case attr_type::identifier_case: {
        return out << "identifier_case";
      } break;
      case attr_type::macro_info: {
        return out << "macro_info";
      } break;
      case attr_type::namelist_item: {
        return out << "namelist_item";
      } break;
      case attr_type::priority: {
        return out << "priority";
      } break;
      case attr_type::segment: {
        return out << "segment";
      } break;
      case attr_type::specification: {
        return out << "specification";
      } break;
      case attr_type::static_link: {
        return out << "static_link";
      } break;
      case attr_type::type: {
        return out << "type";
      } break;
      case attr_type::use_location: {
        return out << "use_location";
      } break;
      case attr_type::variable_parameter: {
        return out << "variable_parameter";
      } break;
      case attr_type::virtuality: {
        return out << "virtuality";
      } break;
      case attr_type::vtable_elem_location: {
        return out << "vtable_elem_location";
      } break;
      case attr_type::allocated: {
        return out << "allocated";
      } break;
      case attr_type::associated: {
        return out << "associated";
      } break;
      case attr_type::data_location: {
        return out << "data_location";
      } break;
      case attr_type::byte_stride: {
        return out << "byte_stride/stride";
      } break;
      case attr_type::entry_pc: {
        return out << "entry_pc";
      } break;
      case attr_type::use_UTF8: {
        return out << "use_UTF8";
      } break;
      case attr_type::extension: {
        return out << "extension";
      } break;
      case attr_type::ranges: {
        return out << "ranges";
      } break;
      case attr_type::trampoline: {
        return out << "trampoline";
      } break;
      case attr_type::call_column: {
        return out << "call_column";
      } break;
      case attr_type::call_file: {
        return out << "call_file";
      } break;
      case attr_type::call_line: {
        return out << "call_line";
      } break;
      case attr_type::description: {
        return out << "description";
      } break;
      case attr_type::binary_scale: {
        return out << "binary_scale";
      } break;
      case attr_type::decimal_scale: {
        return out << "decimal_scale";
      } break;
      case attr_type::small_: {
        return out << "small_";
      } break;
      case attr_type::decimal_sign: {
        return out << "decimal_sign";
      } break;
      case attr_type::digit_count: {
        return out << "digit_count";
      } break;
      case attr_type::picture_string: {
        return out << "picture_string";
      } break;
      case attr_type::mutable_: {
        return out << "mutable_";
      } break;
      case attr_type::threads_scaled: {
        return out << "threads_scaled";
      } break;
      case attr_type::explicit_: {
        return out << "explicit_";
      } break;
      case attr_type::object_pointer: {
        return out << "object_pointer";
      } break;
      case attr_type::endianity: {
        return out << "endianity";
      } break;
      case attr_type::elemental: {
        return out << "elemental";
      } break;
      case attr_type::pure: {
        return out << "pure";
      } break;
      case attr_type::recursive: {
        return out << "recursive";
      } break;
      case attr_type::signature: {
        return out << "signature";
      } break;
      case attr_type::main_subprogram: {
        return out << "main_subprogram";
      } break;
      case attr_type::data_bit_offset: {
        return out << "data_bit_offset";
      } break;
      case attr_type::const_expr: {
        return out << "const_expr";
      } break;
      case attr_type::enum_class: {
        return out << "enum_class";
      } break;
      case attr_type::linkage_name: {
        return out << "linkage_name";
      } break;
      case attr_type::string_length_bit_size: {
        return out << "string_length_bit_size";
      } break;
      case attr_type::string_length_byte_size: {
        return out << "string_length_byte_size";
      } break;
      case attr_type::rank: {
        return out << "rank";
      } break;
      case attr_type::str_offsets_base: {
        return out << "str_offsets_base";
      } break;
      case attr_type::addr_base: {
        return out << "addr_base";
      } break;
      case attr_type::rnglists_base: {
        return out << "rnglists_base";
      } break;
      case attr_type::dwo_id: {
        return out << "dwo_id";
      } break;
      case attr_type::dwo_name: {
        return out << "dwo_name";
      } break;
      case attr_type::reference: {
        return out << "reference";
      } break;
      case attr_type::rvalue_reference: {
        return out << "rvalue_reference";
      } break;
      case attr_type::macros: {
        return out << "macros";
      } break;
      case attr_type::call_all_calls: {
        return out << "call_all_calls";
      } break;
      case attr_type::call_all_source_calls: {
        return out << "call_all_source_calls";
      } break;
      case attr_type::call_all_tail_calls: {
        return out << "call_all_tail_calls";
      } break;
      case attr_type::call_return_pc: {
        return out << "call_return_pc";
      } break;
      case attr_type::call_value: {
        return out << "call_value";
      } break;
      case attr_type::call_origin: {
        return out << "call_origin";
      } break;
      case attr_type::call_parameter: {
        return out << "call_parameter";
      } break;
      case attr_type::call_pc: {
        return out << "call_pc";
      } break;
      case attr_type::call_tail_call: {
        return out << "call_tail_call";
      } break;
      case attr_type::call_target: {
        return out << "call_target";
      } break;
      case attr_type::call_target_clobbered: {
        return out << "call_target_clobbered";
      } break;
      case attr_type::call_data_location: {
        return out << "call_data_location";
      } break;
      case attr_type::call_data_value: {
        return out << "call_data_value";
      } break;
      case attr_type::noreturn: {
        return out << "noreturn";
      } break;
      case attr_type::alignment: {
        return out << "alignment";
      } break;
      case attr_type::export_symbols: {
        return out << "export_symbols";
      } break;
      case attr_type::deleted: {
        return out << "deleted";
      } break;
      case attr_type::defaulted: {
        return out << "defaulted";
      } break;
      case attr_type::loclists_base: {
        return out << "loclists_base";
      } break;
      default: throw std::runtime_error("AAAAAAAAAAAAAAA");
    }
    return out;
  }
  enum class attr_form {
    addr = 0x01,
    block2 = 0x03,
    block4 = 0x04,
    data2 = 0x05,
    data4 = 0x06,
    data8 = 0x07,
    string = 0x08,
    block = 0x09,
    block1 = 0x0a,
    data1 = 0x0b,
    flag = 0x0c,
    sdata = 0x0d,
    strp = 0x0e,
    udata = 0x0f,
    ref_addr = 0x10,
    ref1 = 0x11,
    ref2 = 0x12,
    ref4 = 0x13,
    ref8 = 0x14,
    ref_udata = 0x15,
    indirect = 0x16,
    sec_offset = 0x17, 
    exprloc = 0x18, 
    flag_present = 0x19, 
    strx = 0x1a, 
    addrx = 0x1b, 
    ref_sup4 = 0x1c, 
    strp_sup = 0x1d, 
    data16 = 0x1e, 
    line_strp = 0x1f, 
    ref_sig8 = 0x20, 
    implicit_const = 0x21, 
    loclistx = 0x22, 
    rnglistx = 0x23, 
    ref_sup8 = 0x24, 
    strx1 = 0x25, 
    strx2 = 0x26, 
    strx3 = 0x27, 
    strx4 = 0x28, 
    addrx1 = 0x29, 
    addrx2 = 0x2a, 
    addrx3 = 0x2b, 
    addrx4 = 0x2c
  };
  enum class die_tag : uint16_t {
    array_type = 0x01,
    class_type = 0x02,
    entry_point = 0x03,
    enumeration_type = 0x04,
    formal_parameter = 0x05,
    imported_declaration = 0x08,
    label = 0x0a,
    lexical_block = 0x0b,
    member = 0x0d,
    pointer_type = 0x0f,
    reference_type = 0x10,
    compile_unit = 0x11,
    string_type = 0x12,
    structure_type = 0x13,
    subroutine_type = 0x15,
    typedef_ = 0x16,
    union_type = 0x17,
    unspecified_parameters = 0x18,
    variant = 0x19,
    common_block = 0x1a,
    common_inclusion = 0x1b,
    inheritance = 0x1c,
    inlined_subroutine = 0x1d,
    module = 0x1e,
    ptr_to_member_type = 0x1f,
    set_type = 0x20,
    subrange_type = 0x21,
    with_stmt = 0x22,
    access_declaration = 0x23,
    base_type = 0x24,
    catch_block = 0x25,
    const_type = 0x26,
    constant = 0x27,
    enumerator = 0x28,
    file_type = 0x29,
    friend_ = 0x2a,
    namelist = 0x2b,
    namelist_item = 0x2c, 
    namelist_items = 0x2c, 
    packed_type = 0x2d,
    subprogram = 0x2e,
    template_type_parameter = 0x2f, 
    template_type_param = 0x2f, 
    template_value_parameter = 0x30, 
    template_value_param = 0x30, 
    thrown_type = 0x31,
    try_block = 0x32,
    variant_part = 0x33,
    variable = 0x34,
    volatile_type = 0x35,
    dwarf_procedure = 0x36,  
    restrict_type = 0x37,  
    interface_type = 0x38,  
    namespace_ = 0x39,  
    imported_module = 0x3a,  
    unspecified_type = 0x3b,  
    partial_unit = 0x3c,  
    imported_unit = 0x3d,
    mutable_type = 0x3e, 
    condition = 0x3f,  
    shared_type = 0x40,  
    type_unit = 0x41,  
    rvalue_reference_type = 0x42,  
    template_alias = 0x43,  
    coarray_type = 0x44,  
    generic_subrange = 0x45,  
    dynamic_type = 0x46,  
    atomic_type = 0x47,  
    call_site = 0x48,  
    call_site_parameter = 0x49,  
    skeleton_unit = 0x4a,  
    immutable_type = 0x4b,  
    lo_user = 0x4080
  };
  // Generated using VSCode find and replace
  inline std::ostream& operator<<(std::ostream& out, die_tag tag) {
    out << "DWARF tag ";
    switch (tag) {
      case die_tag::array_type: {
        return out << "array_type";
      } break;
      case die_tag::class_type: {
        return out << "class_type";
      } break;
      case die_tag::entry_point: {
        return out << "entry_point";
      } break;
      case die_tag::enumeration_type: {
        return out << "enumeration_type";
      } break;
      case die_tag::formal_parameter: {
        return out << "formal_parameter";
      } break;
      case die_tag::imported_declaration: {
        return out << "imported_declaration";
      } break;
      case die_tag::label: {
        return out << "label";
      } break;
      case die_tag::lexical_block: {
        return out << "lexical_block";
      } break;
      case die_tag::member: {
        return out << "member";
      } break;
      case die_tag::pointer_type: {
        return out << "pointer_type";
      } break;
      case die_tag::reference_type: {
        return out << "reference_type";
      } break;
      case die_tag::compile_unit: {
        return out << "compile_unit";
      } break;
      case die_tag::string_type: {
        return out << "string_type";
      } break;
      case die_tag::structure_type: {
        return out << "structure_type";
      } break;
      case die_tag::subroutine_type: {
        return out << "subroutine_type";
      } break;
      case die_tag::typedef_: {
        return out << "typedef_";
      } break;
      case die_tag::union_type: {
        return out << "union_type";
      } break;
      case die_tag::unspecified_parameters: {
        return out << "unspecified_parameters";
      } break;
      case die_tag::variant: {
        return out << "variant";
      } break;
      case die_tag::common_block: {
        return out << "common_block";
      } break;
      case die_tag::common_inclusion: {
        return out << "common_inclusion";
      } break;
      case die_tag::inheritance: {
        return out << "inheritance";
      } break;
      case die_tag::inlined_subroutine: {
        return out << "inlined_subroutine";
      } break;
      case die_tag::module: {
        return out << "module";
      } break;
      case die_tag::ptr_to_member_type: {
        return out << "ptr_to_member_type";
      } break;
      case die_tag::set_type: {
        return out << "set_type";
      } break;
      case die_tag::subrange_type: {
        return out << "subrange_type";
      } break;
      case die_tag::with_stmt: {
        return out << "with_stmt";
      } break;
      case die_tag::access_declaration: {
        return out << "access_declaration";
      } break;
      case die_tag::base_type: {
        return out << "base_type";
      } break;
      case die_tag::catch_block: {
        return out << "catch_block";
      } break;
      case die_tag::const_type: {
        return out << "const_type";
      } break;
      case die_tag::constant: {
        return out << "constant";
      } break;
      case die_tag::enumerator: {
        return out << "enumerator";
      } break;
      case die_tag::file_type: {
        return out << "file_type";
      } break;
      case die_tag::friend_: {
        return out << "friend_";
      } break;
      case die_tag::namelist: {
        return out << "namelist";
      } break;
      case die_tag::namelist_item: {
        return out << "namelist_item(s)";
      } break;
      case die_tag::packed_type: {
        return out << "packed_type";
      } break;
      case die_tag::subprogram: {
        return out << "subprogram";
      } break;
      case die_tag::template_type_parameter: {
        return out << "template_type_parameter";
      } break;
      case die_tag::template_value_parameter: {
        return out << "template_value_parameter";
      } break;
      case die_tag::thrown_type: {
        return out << "thrown_type";
      } break;
      case die_tag::try_block: {
        return out << "try_block";
      } break;
      case die_tag::variant_part: {
        return out << "variant_part";
      } break;
      case die_tag::variable: {
        return out << "variable";
      } break;
      case die_tag::volatile_type: {
        return out << "volatile_type";
      } break;
      case die_tag::dwarf_procedure: {
        return out << "dwarf_procedure";
      } break;
      case die_tag::restrict_type: {
        return out << "restrict_type";
      } break;
      case die_tag::interface_type: {
        return out << "interface_type";
      } break;
      case die_tag::namespace_: {
        return out << "namespace_";
      } break;
      case die_tag::imported_module: {
        return out << "imported_module";
      } break;
      case die_tag::unspecified_type: {
        return out << "unspecified_type";
      } break;
      case die_tag::partial_unit: {
        return out << "partial_unit";
      } break;
      case die_tag::imported_unit: {
        return out << "imported_unit";
      } break;
      case die_tag::mutable_type: {
        return out << "mutable_type";
      } break;
      case die_tag::condition: {
        return out << "condition";
      } break;
      case die_tag::shared_type: {
        return out << "shared_type";
      } break;
      case die_tag::type_unit: {
        return out << "type_unit";
      } break;
      case die_tag::rvalue_reference_type: {
        return out << "rvalue_reference_type";
      } break;
      case die_tag::template_alias: {
        return out << "template_alias";
      } break;
      case die_tag::coarray_type: {
        return out << "coarray_type";
      } break;
      case die_tag::generic_subrange: {
        return out << "generic_subrange";
      } break;
      case die_tag::dynamic_type: {
        return out << "dynamic_type";
      } break;
      case die_tag::atomic_type: {
        return out << "atomic_type";
      } break;
      case die_tag::call_site: {
        return out << "call_site";
      } break;
      case die_tag::call_site_parameter: {
        return out << "call_site_parameter";
      } break;
      case die_tag::skeleton_unit: {
        return out << "skeleton_unit";
      } break;
      case die_tag::immutable_type: {
        return out << "immutable_type";
      } break;
      case die_tag::lo_user: {
        return out << "lo_user";
      } break;
    }
    return out;
  }
  
  enum class encoding : uint16_t {
    address = 0x01,
    boolean = 0x02,
    complex_float = 0x03,
    floating_point = 0x04,
    signed_int = 0x05,
    signed_char = 0x06,
    unsigned_int = 0x07,
    unsigned_char = 0x08,
    imaginary_float = 0x09,  /* DWARF3 */
    packed_decimal = 0x0a,  /* DWARF3f */
    numeric_string = 0x0b,  /* DWARF3f */
    edited = 0x0c,  /* DWARF3f */
    signed_fixed = 0x0d,  /* DWARF3f */
    unsigned_fixed = 0x0e,  /* DWARF3f */
    decimal_float = 0x0f,  /* DWARF3f */
    UTF = 0x10,  /* DWARF4 */
    UCS = 0x11,  /* DWARF5 */
    ASCII = 0x12,  /* DWARF5 */
  };
  
  inline std::ostream& operator<<(std::ostream& out, encoding enc) {
    switch (enc) {
      case encoding::address: {
        out << "address";
      } break;
      case encoding::boolean: {
        out << "boolean";
      } break;
      case encoding::complex_float: {
        out << "complex_float";
      } break;
      case encoding::floating_point: {
        out << "floating_point";
      } break;
      case encoding::signed_int: {
        out << "signed_int";
      } break;
      case encoding::signed_char: {
        out << "signed_char";
      } break;
      case encoding::unsigned_int: {
        out << "unsigned_int";
      } break;
      case encoding::unsigned_char: {
        out << "unsigned_char";
      } break;
      case encoding::imaginary_float: {
        out << "imaginary_float";
      } break;
      case encoding::packed_decimal: {
        out << "packed_decimal";
      } break;
      case encoding::numeric_string: {
        out << "numeric_string";
      } break;
      case encoding::edited: {
        out << "edited";
      } break;
      case encoding::signed_fixed: {
        out << "signed_fixed";
      } break;
      case encoding::unsigned_fixed: {
        out << "unsigned_fixed";
      } break;
      case encoding::decimal_float: {
        out << "decimal_float";
      } break;
      case encoding::UTF: {
        out << "UTF";
      } break;
      case encoding::UCS: {
        out << "UCS";
      } break;
      case encoding::ASCII: {
        out << "ASCII";
      } break;
      default: throw std::runtime_error("AAAAAAAAAA");
    }
    return out;
    
  }
}
#endif