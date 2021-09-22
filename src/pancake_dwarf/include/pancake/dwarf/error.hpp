/******************************************************************
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************/


#ifndef _PANCAKE_DWARF_ERROR_HPP
#define _PANCAKE_DWARF_ERROR_HPP

#include <libdwarf/libdwarf.h>
#include <cmath>
#include <stdexcept>
#include <system_error>
#include <type_traits>
namespace pdwarf {
  enum class dwarf_errc : int {
    unsupported_dwarf_version = 1,  
    memory_map_failed = 2,  
    libelf_failed = 3,  
    no_debug_section = 4,  
    no_line_section = 5,  
    invalid_query_descriptor = 6,  
    io_error = 7,  
    bad_alloc = 8,  
    invalid_argument = 9,  
    mangled_debug_entry = 10, 
    mangled_line_entry = 11, 
    file_open_failed = 12, 
    not_regular_file = 13, 
    bad_file_access = 14, 
    not_object_file = 15, 
    mangled_object_header = 16, 
    end_of_loclists = 17, 
    no_loclists = 18, 
    bad_offset = 19, 
    end_of_section = 20, 
    abbrev_truncated = 21, 
    address_size_bad = 22,
    /* error codes defined by the current libdwarf implementation. */
    dbg_alloc = 23,
    fstat_error = 24,
    fstat_mode_error = 25,
    init_access_wrong = 26,
    elf_begin_error = 27,
    elf_getehdr_error = 28,
    elf_getshdr_error = 29,
    elf_strptr_error = 30,
    debug_info_duplicate = 31,
    debug_info_null = 32,
    debug_abbrev_duplicate = 33,
    debug_abbrev_null = 34,
    debug_aranges_duplicate = 35,
    debug_aranges_null = 36,
    debug_line_duplicate = 37,
    debug_line_null = 38,
    debug_loc_duplicate = 39,
    debug_loc_null = 40,
    debug_macinfo_duplicate = 41,
    debug_macinfo_null = 42,
    debug_pubnames_duplicate = 43,
    debug_pubnames_null = 44,
    debug_str_duplicate = 45,
    debug_str_null = 46,
    cu_length_error = 47,
    version_stamp_error = 48,
    abbrev_offset_error = 49,
    address_size_error = 50,
    debug_info_ptr_null = 51,
    die_null = 52,
    string_offset_bad = 53,
    debug_line_length_bad = 54,
    line_prolog_length_bad = 55,
    line_num_operands_bad = 56,
    line_set_addr_error = 57,
    line_ext_opcode_bad = 58,
    dwarf_line_null = 59,
    incl_dir_num_bad = 60,
    line_file_num_bad = 61,
    alloc_fail = 62,
    no_callback_func = 63,
    sect_alloc = 64,
    file_entry_alloc = 65,
    line_alloc = 66,
    fpgm_alloc = 67,
    incdir_alloc = 68,
    string_alloc = 69,
    chunk_alloc = 70,
    byteoff_err = 71,
    cie_alloc = 72,
    fde_alloc = 73,
    regno_ovfl = 74,
    cie_offs_alloc = 75,
    wrong_address = 76,
    extra_neighbors = 77,
    wrong_tag = 78,
    die_alloc = 79,
    parent_exists = 80,
    dbg_null = 81,
    debugline_error = 82,
    debugframe_error = 83,
    debuginfo_error = 84,
    attr_alloc = 85,
    abbrev_alloc = 86,
    offset_uflw = 87,
    elf_sect_err = 88,
    debug_frame_length_bad = 89,
    frame_version_bad = 90,
    cie_ret_addr_reg_error = 91,
    fde_null = 92,
    fde_dbg_null = 93,
    cie_null = 94,
    cie_dbg_null = 95,
    frame_table_col_bad = 96,
    pc_not_in_fde_range = 97,
    cie_instr_exec_error = 98,
    frame_instr_exec_error = 99,
    fde_ptr_null = 100,
    ret_op_list_null = 101,
    line_context_null = 102,
    dbg_no_cu_context = 103,
    die_no_cu_context = 104,
    first_die_not_cu = 105,
    next_die_ptr_null = 106,
    debug_frame_duplicate = 107,
    debug_frame_null = 108,
    abbrev_decode_error = 109,
    dwarf_abbrev_null = 110,
    attr_null = 111,
    die_bad = 112,
    die_abbrev_bad = 113,
    attr_form_bad = 114,
    attr_no_cu_context = 115,
    attr_form_size_bad = 116,
    attr_dbg_null = 117,
    bad_ref_form = 118,
    attr_form_offset_bad = 119,
    line_offset_bad = 120,
    debug_str_offset_bad = 121,
    string_ptr_null = 122,
    pubnames_version_error = 123,
    pubnames_length_bad = 124,
    global_null = 125,
    global_context_null = 126,
    dir_index_bad = 127,
    loc_expr_bad = 128,
    die_loc_expr_bad = 129,
    addr_alloc = 130,
    offset_bad = 131,
    make_cu_context_fail = 132,
    rel_alloc = 133,
    arange_offset_bad = 134,
    segment_size_bad = 135,
    arange_length_bad = 136,
    arange_decode_error = 137,
    aranges_null = 138,
    arange_null = 139,
    no_file_name = 140,
    no_comp_dir = 141,
    cu_address_size_bad = 142,
    input_attr_bad = 143,
    expr_null = 144,
    bad_expr_opcode = 145,
    expr_length_bad = 146,
    multiple_reloc_in_expr = 147,
    elf_getident_error = 148,
    no_at_mips_fde = 149,
    no_cie_for_fde = 150,
    die_abbrev_list_null = 151,
    debug_funcnames_duplicate = 152,
    debug_funcnames_null = 153,
    debug_funcnames_version_error = 154,
    debug_funcnames_length_bad = 155,
    func_null = 156,
    func_context_null = 157,
    debug_typenames_duplicate = 158,
    debug_typenames_null = 159,
    debug_typenames_version_error = 160,
    debug_typenames_length_bad = 161,
    type_null = 162,
    type_context_null = 163,
    debug_varnames_duplicate = 164,
    debug_varnames_null = 165,
    debug_varnames_version_error = 166,
    debug_varnames_length_bad = 167,
    var_null = 168,
    var_context_null = 169,
    debug_weaknames_duplicate = 170,
    debug_weaknames_null = 171,
    debug_weaknames_version_error = 172,
    debug_weaknames_length_bad = 173,
    weak_null = 174,
    weak_context_null = 175,
    locdesc_count_wrong = 176,
    macinfo_string_null = 177,
    macinfo_string_empty = 178,
    macinfo_internal_error_space = 179,
    macinfo_malloc_fail = 180,
    debugmacinfo_error = 181,
    debug_macro_length_bad = 182,
    debug_macro_max_bad = 183,
    debug_macro_internal_err = 184,
    debug_macro_malloc_space = 185,
    debug_macro_inconsistent = 186,
    df_no_cie_augmentation = 187,
    df_reg_num_too_high = 188,
    df_make_instr_no_init = 189,
    df_new_loc_less_old_loc = 190,
    df_pop_empty_stack = 191,
    df_alloc_fail = 192,
    df_frame_decoding_error = 193,
    debug_loc_section_short = 194,
    frame_augmentation_unknown = 195,
    pubtype_context = 196, /* unused. */
    debug_pubtypes_length_bad = 197,
    debug_pubtypes_version_error = 198,
    debug_pubtypes_duplicate = 199,
    frame_cie_decode_error = 200,
    frame_register_unrepresentable = 201,
    frame_register_count_mismatch = 202,
    link_loop = 203,
    strp_offset_bad = 204,
    debug_ranges_duplicate = 205,
    debug_ranges_offset_bad = 206,
    debug_ranges_missing_end = 207,
    debug_ranges_out_of_mem = 208,
    debug_symtab_err = 209,
    debug_strtab_err = 210,
    reloc_mismatch_index = 211,
    reloc_mismatch_reloc_index = 212,
    reloc_mismatch_strtab_index = 213,
    reloc_section_mismatch = 214,
    reloc_section_missing_index = 215,
    reloc_section_length_odd = 216,
    reloc_section_ptr_null = 217,
    reloc_section_malloc_fail = 218,
    no_elf64_support = 219,
    missing_elf64_support = 220,
    orphan_fde = 221,
    duplicate_inst_block = 222,
    bad_ref_sig8_form = 223,
    attr_exprloc_form_bad = 224,
    form_sec_offset_length_bad = 225,
    not_ref_form = 226,
    debug_frame_length_not_multiple = 227,
    ref_sig8_not_handled = 228,
    debug_frame_possible_address_botch = 229,
    loc_bad_termination = 230,
    symtab_section_length_odd = 231,
    reloc_section_symbol_index_bad = 232,
    reloc_section_reloc_target_size_unknown = 233,
    symtab_section_entrysize_zero = 234,
    line_number_header_error = 235,
    debug_types_null = 236,
    debug_types_duplicate = 237,
    debug_types_only_dwarf4 = 238,
    debug_typeoffset_bad = 239,
    gnu_opcode_error = 240,
    debugpubtypes_error = 241,
    at_fixup_null = 242,
    at_fixup_dup = 243,
    bad_abiname = 244,
    too_many_debug = 245,
    debug_str_offsets_duplicate = 246,
    section_duplication = 247,
    section_error = 248,
    debug_addr_duplicate = 249,
    debug_cu_unavailable_for_form = 250,
    debug_form_handling_incomplete = 251,
    next_die_past_end = 252,
    next_die_wrong_form = 253,
    next_die_no_abbrev_list = 254,
    nested_form_indirect_error = 255,
    cu_die_no_abbrev_list = 256,
    missing_needed_debug_addr_section = 257,
    attr_form_not_addr_index = 258,
    attr_form_not_str_index = 259,
    duplicate_gdb_index = 260,
    erroneous_gdb_index_section = 261,
    gdb_index_count_error = 262,
    gdb_index_count_addr_error = 263,
    gdb_index_index_error = 264,
    gdb_index_cuvec_error = 265,
    duplicate_cu_index = 266,
    duplicate_tu_index = 267,
    xu_type_arg_error = 268,
    xu_impossible_error = 269,
    xu_name_col_error = 270,
    xu_hash_row_error = 271,
    xu_hash_index_error = 272,
    failsafe_errval = 273,
    arange_error = 274,
    pubnames_error = 275,
    funcnames_error = 276,
    typenames_error = 277,
    varnames_error = 278,
    weaknames_error = 279,
    relocs_error = 280,
    attr_outside_section = 281,
    fission_index_wrong = 282,
    fission_version_error = 283,
    next_die_low_error = 284,
    cu_ut_type_error = 285,
    no_such_signature_found = 286,
    signature_section_number_wrong = 287,
    attr_form_not_data8 = 288,
    sig_type_wrong_string = 289,
    missing_required_tu_offset_hash = 290,
    missing_required_cu_offset_hash = 291,
    dwp_missing_dwo_id = 292,
    dwp_sibling_error = 293,
    debug_fission_incomplete = 294,
    fission_secnum_err = 295,
    debug_macro_duplicate = 296,
    debug_names_duplicate = 297,
    debug_line_str_duplicate = 298,
    debug_sup_duplicate = 299,
    no_signature_to_lookup = 300,
    no_tied_addr_available = 301,
    no_tied_sig_available = 302,
    string_not_terminated = 303,
    bad_line_table_operation = 304,
    line_context_botch = 305,
    line_context_index_wrong = 306,
    no_tied_string_available = 307,
    no_tied_file_available = 308,
    cu_type_missing = 309,
    lle_code_unknown = 310,
    loclist_interface_error = 311,
    loclist_index_error = 312,
    interface_not_supported = 313,
    zdebug_requires_zlib = 314,
    zdebug_input_format_odd = 315,
    zlib_buf_error = 316,
    zlib_data_error = 317,
    macro_offset_bad = 318,
    macro_opcode_bad = 319,
    macro_opcode_form_bad = 320,
    unknown_form = 321,
    bad_macro_header_pointer = 322,
    bad_macro_index = 323,
    macro_op_unhandled = 324,
    macro_past_end = 325,
    line_strp_offset_bad = 326,
    string_form_improper = 327,
    elf_flags_not_available = 328,
    leb_improper = 329,
    debug_line_range_zero = 330,
    read_littleendian_error = 331,
    read_bigendian_error = 332,
    reloc_invalid = 333,
    info_header_error = 334,
    aranges_header_error = 335,
    line_offset_wrong_form = 336,
    form_block_length_error = 337,
    zlib_section_short = 338,
    cie_instr_ptr_error = 339,
    fde_instr_ptr_error = 340,
    fission_addition_error = 341,
    header_len_bigger_than_secsize = 342,
    locexpr_off_section_end = 343,
    pointer_section_unknown = 344,
    erroneous_xu_index_section = 345,
    directory_format_count_vs_directories_mismatch = 346,
    compressed_empty_section = 347,
    size_wraparound = 348,
    illogical_tsearch = 349,
    bad_string_form = 350,
    debugstr_error = 351,
    debugstr_unexpected_rel = 352,
    discr_array_error = 353,
    leb_out_error = 354,
    sibling_list_improper = 355,
    loclist_offset_bad = 356,
    line_table_bad = 357,
    debug_loclists_duplicate = 358,
    debug_rnglists_duplicate = 359,
    abbrev_off_end = 360,
    form_string_bad_string = 361,
    augmentation_string_off_end = 362,
    string_off_end_pubnames_like = 363,
    line_string_bad = 364,
    define_file_string_bad = 365,
    macro_string_bad = 366,
    macinfo_string_bad = 367,
    zlib_uncompress_error = 368,
    improper_dwo_id = 369,
    groupnumber_error = 370,
    address_size_zero = 371,
    debug_names_header_error = 372,
    debug_names_aug_string_error = 373,
    debug_names_pad_non_zero = 374,
    debug_names_off_end = 375,
    debug_names_abbrev_overflow = 376,
    debug_names_abbrev_corruption = 377,
    debug_names_null_pointer = 378,
    debug_names_bad_index_arg = 379,
    debug_names_entrypool_offset = 380,
    debug_names_unhandled_form = 381,
    lnct_code_unknown = 382,
    lnct_form_code_not_handled = 383,
    line_header_length_botch = 384,
    string_hashtab_identity_error = 385,
    unit_type_not_handled = 386,
    group_map_alloc = 387,
    group_map_duplicate = 388,
    group_count_error = 389,
    group_internal_error = 390,
    group_load_error = 391,
    group_load_read_error = 392,
    aug_data_length_bad = 393,
    abbrev_missing = 394,
    no_tag_for_die = 395,
    lowpc_wrong_class = 396,
    highpc_wrong_form = 397,
    str_offsets_base_wrong_form = 398,
    data16_outside_section = 399,
    lnct_md5_wrong_form = 400,
    line_header_corrupt = 401,
    str_offsets_nullargument = 402,
    str_offsets_null_dbg = 403,
    str_offsets_no_magic = 404,
    str_offsets_array_size = 405,
    str_offsets_version_wrong = 406,
    str_offsets_array_index_wrong = 407,
    str_offsets_extra_bytes = 408,
    dup_attr_on_die = 409,
    section_name_big = 410,
    file_unavailable = 411,
    file_wrong_type = 412,
    sibling_offset_wrong = 413,
    open_fail = 414,
    offset_size = 415,
    mach_o_segoffset_bad = 416,
    file_offset_bad = 417,
    seek_error = 418,
    read_error = 419,
    elf_class_bad = 420,
    elf_endian_bad = 421,
    elf_version_bad = 422,
    file_too_small = 423,
    path_size_too_small = 424,
    bad_type_size = 425,
    pe_size_small = 426,
    pe_offset_bad = 427,
    pe_string_too_long = 428,
    image_file_unknown_type = 429,
    line_table_lineno_error = 430,
    producer_code_not_available = 431,
    no_elf_support = 432,
    no_stream_reloc_support = 433,
    return_empty_pubnames_error = 434,
    section_size_error = 435,
    internal_null_pointer = 436,
    section_string_offset_bad = 437,
    section_index_bad = 438,
    integer_too_small = 439,
    elf_section_link_error = 440,
    elf_section_group_error = 441,
    elf_section_count_mismatch = 442,
    elf_string_section_missing = 443,
    seek_off_end = 444,
    read_off_end = 445,
    elf_section_error = 446,
    elf_string_section_error = 447,
    mixing_split_dwarf_versions = 448,
    tag_corrupt = 449,
    form_corrupt = 450,
    attr_corrupt = 451,
    abbrev_attr_duplication = 452,
    dwp_signature_mismatch = 453,
    cu_ut_type_value = 454,
    duplicate_gnu_debuglink = 455,
    corrupt_gnu_debuglink = 456,
    corrupt_note_gnu_debugid = 457,
    corrupt_gnu_debugid_size = 458,
    corrupt_gnu_debugid_string = 459,
    hex_string_error = 460,
    decimal_string_error = 461,
    pro_init_extras_unknown = 462,
    pro_init_extras_err = 463,
    null_args_dwarf_add_path = 464,
    dwarf_init_dbg_null = 465,
    elf_reloc_section_error = 466,
    user_declared_error = 467,
    rnglists_error = 468,
    loclists_error = 469,
    section_size_or_offset_large = 470,
    gdbindex_string_error = 471,
    gnu_pubnames_error = 472,
    gnu_pubtypes_error = 473,
    duplicate_gnu_debug_pubnames = 474,
    duplicate_gnu_debug_pubtypes = 475,
    debug_sup_string_error = 476,
    debug_sup_error = 477,
    location_error = 478,
    debuglink_path_short = 479,
    signature_mismatch = 480,
    macro_version_error = 481,
    negative_size = 482,
    udata_value_negative = 483,
  };
  
  struct dwarf_category_impl : std::error_category {
    const char* name() const noexcept override {
      return "DWARF";
    }
    std::string message(int ev) const override {
      char* errmsg = dwarf_errmsg_by_number(ev);
      return std::string(errmsg);
    }
  };
  
  const dwarf_category_impl& dwarf_category();
  
  std::error_code make_error_code(dwarf_errc x);
  std::error_condition make_error_condition(dwarf_errc x);
  
  class no_pubnames_section : public std::logic_error {
    using std::logic_error::logic_error;
  };
  
  class file_not_found : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };
};

namespace std {
  template<>
  struct is_error_code_enum<pdwarf::dwarf_errc> : std::true_type {};
};
#endif