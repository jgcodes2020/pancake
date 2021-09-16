  .section .rodata
  # Metadata definition
  .global _pancake_rsrc_{name}_begin
  .type {name}, @object
  .global _pancake_rsrc_{name}_end
  .type {name}, object
  
_pancake_rsrc_{name}_begin__:
  .incbin "{fname}"
_pancake_rsrc_{name}_end__: