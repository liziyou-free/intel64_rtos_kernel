
#ifndef  __X64_ASM_COMMON_HEADER__
#define  __X64_ASM_COMMON_HEADER__


#define X64_ASM_LOCAL_BSS_DATA(name)       .section .bss.name;         \
                                           .type name, %object;        \
                                            name:                      \


#define X64_ASM_BSS_DATA(name)              .global name;               \
                                             X64_LOCAL_BSS_DATA(name)


#define X64_ASM_LOCAL_DATA(name)           .section .data.name;        \
                                           .type name, %object;        \
                                            name:                      \


#define X64_ASM_DATA(name)                 .global name;               \
                                            X64_ASM_LOCAL_DATA(name)


#define X64_CODE32_LOCAL_FUNC_BEGIN(name)  .section .text.name;        \
                                           .type name, %function;      \
                                           .func;                      \
                                           .code32;                    \
                                            name:                      \


#define X64_CODE32_FUNC_BEGIN(name)        .global name ;              \
                                            X64_CODE32_LOCAL_FUNC_BEGIN(name)


#define X64_CODE64_LOCAL_FUNC_BEGIN(name)  .section .text.name;        \
                                           .type name, %function;      \
                                           .func;                      \
                                           .code64;                    \
                                            name:                      \


#define X64_CODE64_FUNC_BEGIN(name)        .global name ;              \
                                            X64_CODE64_LOCAL_FUNC_BEGIN(name)


#define X64_FUNC_END()                     .endfunc;.text

#define IMPORT                             .global
#define EXPORT                             .global
#define DCD                                .word

#define DEF_LABEL(label)                    label:


#endif  /* __X64_ASM_COMMON_HEADER__ */



