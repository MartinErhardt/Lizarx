#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
do {                                \
    builtin_define_std ("lizarx");      \
    builtin_define_std ("unix");      \
    builtin_assert ("system=lizarx");   \
    builtin_assert ("system=unix");   \
    builtin_define ("__ELF__");     \
    builtin_define ("__LIZARX__");     \
} while(0);
//#define LIB_SPEC "--start-group -lm -lc --end-group"#

#if TARGET_64BIT_DEFAULT
#define SPEC_32 "m32"
#define SPEC_64 "!m32"
#else
#define SPEC_32 "!m64"
#define SPEC_64 "m64"
#endif

#undef ASM_SPEC
#define ASM_SPEC "%{" SPEC_32 ":--32} %{" SPEC_64 ":--64}"

#undef  LINK_SPEC
#define LINK_SPEC "%{" SPEC_64 ":-m elf_x86_64} %{" SPEC_32 ":-m lizarx_i386} \
  %{shared:-shared} \
  %{!shared: \
    %{!static: \
      } \
    %{static:-static}}"