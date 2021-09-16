  section .rdata
  ; Metadata definition
  global _pancake_rsrc_{name}_begin
  global _pancake_rsrc_{name}_end
  
_pancake_rsrc_{name}_begin:
  incbin "{fname}"
_pancake_rsrc_{name}_end: