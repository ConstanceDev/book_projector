/*
 * {- join("\n * ", @autowarntext) -}
 *
 * Copyright 2000-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */





#ifndef OPENSSL_ASN1T_H
# define OPENSSL_ASN1T_H
# pragma once

# include <openssl/macros.h>
# ifndef OPENSSL_NO_DEPRECATED_3_0
#  define HEADER_ASN1T_H
# endif

# include <stddef.h>
# include <openssl/e_os2.h>
# include <openssl/asn1.h>

# ifdef OPENSSL_BUILD_SHLIBCRYPTO
#  undef OPENSSL_EXTERN
#  define OPENSSL_EXTERN OPENSSL_EXPORT
# endif

/* ASN1 template defines, structures and functions */

#ifdef  __cplusplus
extern "C" {
#endif

/*-
 * These are the possible values for the itype field of the
 * ASN1_ITEM structure and determine how it is interpreted.
 *
 * For PRIMITIVE types the underlying type
 * determines the behaviour if items is NULL.
 *
 * Otherwise templates must contain a single
 * template and the type is treated in the
 * same way as the type specified in the template.
 *
 * For SEQUENCE types the templates field points
 * to the members, the size field is the
 * structure size.
 *
 * For CHOICE types the templates field points
 * to each possible member (typically a union)
 * and the 'size' field is the offset of the
 * selector.
 *
 * The 'funcs' field is used for application-specific
 * data and functions.
 *
 * The EXTERN type uses a new style d2i/i2d.
 * The new style should be used where possible
 * because it avoids things like the d2i IMPLICIT
 * hack.
 *
 * MSTRING is a multiple string type, it is used
 * for a CHOICE of character strings where the
 * actual strings all occupy an ASN1_STRING
 * structure. In this case the 'utype' field
 * has a special meaning, it is used as a mask
 * of acceptable types using the B_ASN1 constants.
 *
 * NDEF_SEQUENCE is the same as SEQUENCE except
 * that it will use indefinite length constructed
 * encoding if requested.
 *
 */

# define ASN1_ITYPE_PRIMITIVE            0x0
# define ASN1_ITYPE_SEQUENCE             0x1
# define ASN1_ITYPE_CHOICE               0x2
/* unused value                          0x3 */
# define ASN1_ITYPE_EXTERN               0x4
# define ASN1_ITYPE_MSTRING              0x5
# define ASN1_ITYPE_NDEF_SEQUENCE        0x6

/* Macro to obtain ASN1_ADB pointer from a type (only used internally) */
# define ASN1_ADB_ptr(iptr) ((const ASN1_ADB *)((iptr)()))

/* Macros for start and end of ASN1_ITEM definition */

# define ASN1_ITEM_start(itname) \
        const ASN1_ITEM * itname##_it(void) \
        { \
                static const ASN1_ITEM local_it = {

# define static_ASN1_ITEM_start(itname) \
        static ASN1_ITEM_start(itname)

# define ASN1_ITEM_end(itname) \
                }; \
        return &local_it; \
        }

/* Macros to aid ASN1 template writing */

# define ASN1_ITEM_TEMPLATE(tname) \
        static const ASN1_TEMPLATE tname##_item_tt

# define ASN1_ITEM_TEMPLATE_END(tname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_PRIMITIVE,\
                -1,\
                &tname##_item_tt,\
                0,\
                NULL,\
                0,\
                #tname \
        ASN1_ITEM_end(tname)
# define static_ASN1_ITEM_TEMPLATE_END(tname) \
        ;\
        static_ASN1_ITEM_start(tname) \
                ASN1_ITYPE_PRIMITIVE,\
                -1,\
                &tname##_item_tt,\
                0,\
                NULL,\
                0,\
                #tname \
        ASN1_ITEM_end(tname)

/* This is a ASN1 type which just embeds a template */

/*-
 * This pair helps declare a SEQUENCE. We can do:
 *
 *      ASN1_SEQUENCE(stname) = {
 *              ... SEQUENCE components ...
 *      } ASN1_SEQUENCE_END(stname)
 *
 *      This will produce an ASN1_ITEM called stname_it
 *      for a structure called stname.
 *
 *      If you want the same structure but a different
 *      name then use:
 *
 *      ASN1_SEQUENCE(itname) = {
 *              ... SEQUENCE components ...
 *      } ASN1_SEQUENCE_END_name(stname, itname)
 *
 *      This will create an item called itname_it using
 *      a structure called stname.
 */

# define ASN1_SEQUENCE(tname) \
        static const ASN1_TEMPLATE tname##_seq_tt[]

# define ASN1_SEQUENCE_END(stname) ASN1_SEQUENCE_END_name(stname, stname)

# define static_ASN1_SEQUENCE_END(stname) static_ASN1_SEQUENCE_END_name(stname, stname)

# define ASN1_SEQUENCE_END_name(stname, tname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(stname),\
                #tname \
        ASN1_ITEM_end(tname)

# define static_ASN1_SEQUENCE_END_name(stname, tname) \
        ;\
        static_ASN1_ITEM_start(tname) \
                ASN1_ITYPE_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

# define ASN1_NDEF_SEQUENCE(tname) \
        ASN1_SEQUENCE(tname)

# define ASN1_NDEF_SEQUENCE_cb(tname, cb) \
        ASN1_SEQUENCE_cb(tname, cb)

# define ASN1_SEQUENCE_cb(tname, cb) \
        static const ASN1_AUX tname##_aux = {NULL, 0, 0, 0, cb, 0, NULL}; \
        ASN1_SEQUENCE(tname)

# define ASN1_SEQUENCE_const_cb(tname, const_cb) \
        static const ASN1_AUX tname##_aux = \
            {NULL, ASN1_AFLG_CONST_CB, 0, 0, NULL, 0, const_cb}; \
        ASN1_SEQUENCE(tname)

# define ASN1_SEQUENCE_cb_const_cb(tname, cb, const_cb) \
        static const ASN1_AUX tname##_aux = \
            {NULL, ASN1_AFLG_CONST_CB, 0, 0, cb, 0, const_cb}; \
        ASN1_SEQUENCE(tname)

# define ASN1_SEQUENCE_ref(tname, cb) \
        static const ASN1_AUX tname##_aux = {NULL, ASN1_AFLG_REFCOUNT, offsetof(tname, references), offsetof(tname, lock), cb, 0, NULL}; \
        ASN1_SEQUENCE(tname)

# define ASN1_SEQUENCE_enc(tname, enc, cb) \
        static const ASN1_AUX tname##_aux = {NULL, ASN1_AFLG_ENCODING, 0, 0, cb, offsetof(tname, enc), NULL}; \
        ASN1_SEQUENCE(tname)

# define ASN1_NDEF_SEQUENCE_END(tname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_NDEF_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(tname),\
                #tname \
        ASN1_ITEM_end(tname)
# define static_ASN1_NDEF_SEQUENCE_END(tname) \
        ;\
        static_ASN1_ITEM_start(tname) \
                ASN1_ITYPE_NDEF_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(tname),\
                #tname \
        ASN1_ITEM_end(tname)


# define ASN1_SEQUENCE_END_enc(stname, tname) ASN1_SEQUENCE_END_ref(stname, tname)

# define ASN1_SEQUENCE_END_cb(stname, tname) ASN1_SEQUENCE_END_ref(stname, tname)
# define static_ASN1_SEQUENCE_END_cb(stname, tname) static_ASN1_SEQUENCE_END_ref(stname, tname)

# define ASN1_SEQUENCE_END_ref(stname, tname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                &tname##_aux,\
                sizeof(stname),\
                #tname \
        ASN1_ITEM_end(tname)
# define static_ASN1_SEQUENCE_END_ref(stname, tname) \
        ;\
        static_ASN1_ITEM_start(tname) \
                ASN1_ITYPE_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                &tname##_aux,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

# define ASN1_NDEF_SEQUENCE_END_cb(stname, tname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_NDEF_SEQUENCE,\
                V_ASN1_SEQUENCE,\
                tname##_seq_tt,\
                sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
                &tname##_aux,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

/*-
 * This pair helps declare a CHOICE type. We can do:
 *
 *      ASN1_CHOICE(chname) = {
 *              ... CHOICE options ...
 *      ASN1_CHOICE_END(chname)
 *
 *      This will produce an ASN1_ITEM called chname_it
 *      for a structure called chname. The structure
 *      definition must look like this:
 *      typedef struct {
 *              int type;
 *              union {
 *                      ASN1_SOMETHING *opt1;
 *                      ASN1_SOMEOTHER *opt2;
 *              } value;
 *      } chname;
 *
 *      the name of the selector must be 'type'.
 *      to use an alternative selector name use the
 *      ASN1_CHOICE_END_selector() version.
 */

# define ASN1_CHOICE(tname) \
        static const ASN1_TEMPLATE tname##_ch_tt[]

# define ASN1_CHOICE_cb(tname, cb) \
        static const ASN1_AUX tname##_aux = {NULL, 0, 0, 0, cb, 0, NULL}; \
        ASN1_CHOICE(tname)

# define ASN1_CHOICE_END(stname) ASN1_CHOICE_END_name(stname, stname)

# define static_ASN1_CHOICE_END(stname) static_ASN1_CHOICE_END_name(stname, stname)

# define ASN1_CHOICE_END_name(stname, tname) ASN1_CHOICE_END_selector(stname, tname, type)

# define static_ASN1_CHOICE_END_name(stname, tname) static_ASN1_CHOICE_END_selector(stname, tname, type)

# define ASN1_CHOICE_END_selector(stname, tname, selname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_CHOICE,\
                offsetof(stname,selname) ,\
                tname##_ch_tt,\
                sizeof(tname##_ch_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

# define static_ASN1_CHOICE_END_selector(stname, tname, selname) \
        ;\
        static_ASN1_ITEM_start(tname) \
                ASN1_ITYPE_CHOICE,\
                offsetof(stname,selname) ,\
                tname##_ch_tt,\
                sizeof(tname##_ch_tt) / sizeof(ASN1_TEMPLATE),\
                NULL,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

# define ASN1_CHOICE_END_cb(stname, tname, selname) \
        ;\
        ASN1_ITEM_start(tname) \
                ASN1_ITYPE_CHOICE,\
                offsetof(stname,selname) ,\
                tname##_ch_tt,\
                sizeof(tname##_ch_tt) / sizeof(ASN1_TEMPLATE),\
                &tname##_aux,\
                sizeof(stname),\
                #stname \
        ASN1_ITEM_end(tname)

/* This helps with the template wrapper form of ASN1_ITEM */

# define ASN1_EX_TEMPLATE_TYPE(flags, tag, name, type) { \
        (flags), (tag), 0,\
        #name, ASN1_ITEM_ref(type) }

/* These help with SEQUENCE or CHOICE components */

/* used to declare other types */

# define ASN1_EX_TYPE(flags, tag, stname, field, type) { \
        (flags), (tag), offsetof(stname, field),\
        #field, ASN1_ITEM_ref(type) }

/* implicit and explicit helper macros */

# define ASN1_IMP_EX(stname, field, type, tag, ex) \
         ASN1_EX_TYPE(ASN1_TFLG_IMPLICIT | (ex), tag, stname, field, type)

# define ASN1_EXP_EX(stname, field, type, tag, ex) \
         ASN1_EX_TYPE(ASN1_TFLG_EXPLICIT | (ex), tag, stname, field, type)

/* Any defined by macros: the field used is in the table itself */

# define ASN1_ADB_OBJECT(tblname) { ASN1_TFLG_ADB_OID, -1, 0, #tblname, tblname##_adb }
# define ASN1_ADB_INTEGER(tblname) { ASN1_TFLG_ADB_INT, -1, 0, #tblname, tblname##_adb }

/* Plain simple type */
# define ASN1_SIMPLE(stname, field, type) ASN1_EX_TYPE(0,0, stname, field, type)
/* Embedded simple type */
# define ASN1_EMBED(stname, field, type) ASN1_EX_TYPE(ASN1_TFLG_EMBED,0, stname, field, type)

/* OPTIONAL simple type */
# define ASN1_OPT(stname, field, type) ASN1_EX_TYPE(ASN1_TFLG_OPTIONAL, 0, stname, field, type)
# define ASN1_OPT_EMBED(stname, field, type) ASN1_EX_TYPE(ASN1_TFLG_OPTIONAL|ASN1_TFLG_EMBED, 0, stname, field, type)

/* IMPLICIT tagged simple type */
# define ASN1_IMP(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, 0)
# define ASN1_IMP_EMBED(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_EMBED)

/* IMPLICIT tagged OPTIONAL simple type */
# define ASN1_IMP_OPT(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL)
# define ASN1_IMP_OPT_EMBED(stname, field, type, tag) ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL|ASN1_TFLG_EMBED)

/* Same as above but EXPLICIT */

# define ASN1_EXP(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, 0)
# define ASN1_EXP_EMBED(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_EMBED)
# define ASN1_EXP_OPT(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL)
# define ASN1_EXP_OPT_EMBED(stname, field, type, tag) ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL|ASN1_TFLG_EMBED)

/* SEQUENCE OF type */
# define ASN1_SEQUENCE_OF(stname, field, type) \
                ASN1_EX_TYPE(ASN1_TFLG_SEQUENCE_OF, 0, stname, field, type)

/* OPTIONAL SEQUENCE OF */
# define ASN1_SEQUENCE_OF_OPT(stname, field, type) \
                ASN1_EX_TYPE(ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL, 0, stname, field, type)

/* Same as above but for SET OF */

# define ASN1_SET_OF(stname, field, type) \
                ASN1_EX_TYPE(ASN1_TFLG_SET_OF, 0, stname, field, type)

# define ASN1_SET_OF_OPT(stname, field, type) \
                ASN1_EX_TYPE(ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL, 0, stname, field, type)

/* Finally compound types of SEQUENCE, SET, IMPLICIT, EXPLICIT and OPTIONAL */

# define ASN1_IMP_SET_OF(stname, field, type, tag) \
                        ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF)

# define ASN1_EXP_SET_OF(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF)

# define ASN1_IMP_SET_OF_OPT(stname, field, type, tag) \
                        ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL)

# define ASN1_EXP_SET_OF_OPT(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SET_OF|ASN1_TFLG_OPTIONAL)

# define ASN1_IMP_SEQUENCE_OF(stname, field, type, tag) \
                        ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF)

# define ASN1_IMP_SEQUENCE_OF_OPT(stname, field, type, tag) \
                        ASN1_IMP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL)

# define ASN1_EXP_SEQUENCE_OF(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF)

# define ASN1_EXP_SEQUENCE_OF_OPT(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_SEQUENCE_OF|ASN1_TFLG_OPTIONAL)

/* EXPLICIT using indefinite length constructed form */
# define ASN1_NDEF_EXP(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_NDEF)

/* EXPLICIT OPTIONAL using indefinite length constructed form */
# define ASN1_NDEF_EXP_OPT(stname, field, type, tag) \
                        ASN1_EXP_EX(stname, field, type, tag, ASN1_TFLG_OPTIONAL|ASN1_TFLG_NDEF)

/* Macros for the ASN1_ADB structure */

# define ASN1_ADB(name) \
        static const ASN1_ADB_TABLE name##_adbtbl[]

# define ASN1_ADB_END(name, flags, field, adb_cb, def, none) \
        ;\
        static const ASN1_ITEM *name##_adb(void) \
        { \
        static const ASN1_ADB internal_adb = \
                {\
                flags,\
                offsetof(name, field),\
                adb_cb,\
                name##_adbtbl,\
                sizeof(name##_adbtbl) / sizeof(ASN1_ADB_TABLE),\
                def,\
                none\
                }; \
                return (const ASN1_ITEM *) &internal_adb; \
        } \
        void dummy_function(void)

# define ADB_ENTRY(val, template) {val, template}

# define ASN1_ADB_TEMPLATE(name) \
        static const ASN1_TEMPLATE name##_tt

/*
 * This is the ASN1 template structure that defines a wrapper round the
 * actual type. It determines the actual position of the field in the value
 * structure, various flags such as OPTIONAL and the field name.
 */

struct ASN1_TEMPLATE_st {
    unsigned long flags;        /* Various flags */
    long tag;                   /* tag, not used if no tagging */
    unsigned long offset;       /* Offset of this field in structure */
    const char *field_name;     /* Field name */
    ASN1_ITEM_EXP *item;        /* Relevant ASN1_ITEM or ASN1_ADB */
};

/* Macro to extract ASN1_ITEM and ASN1_ADB pointer from ASN1_TEMPLATE */

# define ASN1_TEMPLATE_item(t) (t->item_ptr)
# define ASN1_TEMPLATE_adb(t) (t->item_ptr)

typedef struct ASN1_ADB_TABLE_st ASN1_ADB_TABLE;
typedef struct ASN1_ADB_st ASN1_ADB;

struct ASN1_ADB_st {
    unsigned long flags;        /* Various flags */
    unsigned long offset;       /* Offset of selector field */
    int (*adb_cb)(long *psel);  /* Application callback */
    const ASN1_ADB_TABLE *tbl;  /* Table of possible types */
    long tblcount;              /* Number of entries in tbl */
    const ASN1_TEMPLATE *default_tt; /* Type to use if no match */
    const ASN1_TEMPLATE *null_tt; /* Type to use if selector is NULL */
};

struct ASN1_ADB_TABLE_st {
    long value;                 /* NID for an object or value for an int */
    const ASN1_TEMPLATE tt;     /* item for this value */
};

/* template flags */

/* Field is optional */
# define ASN1_TFLG_OPTIONAL      (0x1)

/* Field is a SET OF */
# define ASN1_TFLG_SET_OF        (0x1 << 1)

/* Field is a SEQUENCE OF */
# define ASN1_TFLG_SEQUENCE_OF   (0x2 << 1)

/*
 * Special case: this refers to a SET OF that will be sorted into DER order
 * when encoded *and* the corresponding STACK will be modified to match the
 * new order.
 */
# define ASN1_TFLG_SET_ORDER     (0x3 << 1)

/* Mask for SET OF or SEQUENCE OF */
# define ASN1_TFLG_SK_MASK       (0x3 << 1)

/*
 * These flags mean the tag should be taken from the tag field. If EXPLICIT
 * then the underlying type is used for the inner tag.
 */

/* IMPLICIT tagging */
# define ASN1_TFLG_IMPTAG        (0x1 << 3)

/* EXPLICIT tagging, inner tag from underlying type */
# define ASN1_TFLG_EXPTAG        (0x2 << 3)

# define ASN1_TFLG_TAG_MASK      (0x3 << 3)

/* context specific IMPLICIT */
# define ASN1_TFLG_IMPLICIT      (ASN1_TFLG_IMPTAG|ASN1_TFLG_CONTEXT)

/* context specific EXPLICIT */
# define ASN1_TFLG_EXPLICIT      (ASN1_TFLG_EXPTAG|ASN1_TFLG_CONTEXT)

/*
 * If tagging is in force these determine the type of tag to use. Otherwise
 * the tag is determined by the underlying type. These values reflect the
 * actual octet format.
 */

/* Universal tag */
# define ASN1_TFLG_UNIVERSAL     (0x0<<6)
/* Application tag */
# define ASN1_TFLG_APPLICATION   (0x1<<6)
/* Context specific tag */
# define ASN1_TFLG_CONTEXT       (0x2<<6)
/* Private tag */
# define ASN1_TFLG_PRIVATE       (0x3<<6)

# define ASN1_TFLG_TAG_CLASS     (0x3<<6)

/*
 * These are for ANY DEFINED BY type. In this case the 'item' field points to
 * an ASN1_ADB structure which contains a table of values to decode the
 * relevant type
 */

# define ASN1_TFLG_ADB_MASK      (0x3<<8)

# define ASN1_TFLG_ADB_OID       (0x1<<8)

# define ASN1_TFLG_ADB_INT       (0x1<<9)

/*
 * This flag when present in a SEQUENCE OF, SET OF or EXPLICIT causes
 * indefinite length constructed encoding to be used if required.
 */

# define ASN1_TFLG_NDEF          (0x1<<11)

/* Field is embedded and not a pointer */
# define ASN1_TFLG_EMBED         (0x1 << 12)

/* This is the actual ASN1 item itself */

struct ASN1_ITEM_st {
    char itype;                 /* The item type, primitive, SEQUENCE, CHOICE
                                 * or extern */
    long utype;                 /* underlying type */
    const ASN1_TEMPLATE *templates; /* If SEQUENCE or CHOICE this contains
                                     * the contents */
    long tcount;                /* Number of templates if SEQUENCE or CHOICE */
    const void *funcs;          /* further data and type-specific functions */
    /* funcs can be ASN1_PRIMITIVE_FUNCS*, ASN1_EXTERN_FUNCS*, or ASN1_AUX* */
    long size;                  /* Structure size (usually) */
    const char *sname;          /* Structure name */
};

/*
 * Cache for ASN1 tag and length, so we don't keep re-reading it for things
 * like CHOICE
 */

struct ASN1_TLC_st {
    char valid;                 /* Values below are valid */
    int ret;                    /* return value */
    long plen;                  /* length */
    int ptag;                   /* class value */
    int pclass;                 /* class value */
    int hdrlen;                 /* header length */
};

/* Typedefs for ASN1 function pointers */
typedef int ASN1_ex_d2i(ASN1_VALUE **pval, const unsigned char **in, long len,
                        const ASN1_ITEM *it, int tag, int aclass, char opt,
                        ASN1_TLC *ctx);

typedef int ASN1_ex_d2i_ex(ASN1_VALUE **pval, const unsigned char **in, long len,
                           const ASN1_ITEM *it, int tag, int aclass, char opt,
                           ASN1_TLC *ctx, OSSL_LIB_CTX *libctx,
                           const char *propq);
typedef int ASN1_ex_i2d(const ASN1_VALUE **pval, unsigned char **out,
                        const ASN1_ITEM *it, int tag, int aclass);
typedef int ASN1_ex_new_func(ASN1_VALUE **pval, const ASN1_ITEM *it);
typedef int ASN1_ex_new_ex_func(ASN1_VALUE **pval, const ASN1_ITEM *it,
                                OSSL_LIB_CTX *libctx, const char *propq);
typedef void ASN1_ex_free_func(ASN1_VALUE **pval, const ASN1_ITEM *it);

typedef int ASN1_ex_print_func(BIO *out, const ASN1_VALUE **pval,
                               int indent, const char *fname,
                               const ASN1_PCTX *pctx);

typedef int ASN1_primitive_i2c(const ASN1_VALUE **pval, unsigned char *cont,
                               int *putype, const ASN1_ITEM *it);
typedef int ASN1_primitive_c2i(ASN1_VALUE **pval, const unsigned char *cont,
                               int len, int utype, char *free_cont,
                               const ASN1_ITEM *it);
typedef int ASN1_primitive_print(BIO *out, const ASN1_VALUE **pval,
                                 const ASN1_ITEM *it, int indent,
                                 const ASN1_PCTX *pctx);

typedef struct ASN1_EXTERN_FUNCS_st {
    void *app_data;
    ASN1_ex_new_func *asn1_ex_new;
    ASN1_ex_free_func *asn1_ex_free;
    ASN1_ex_free_func *asn1_ex_clear;
    ASN1_ex_d2i *asn1_ex_d2i;
    ASN1_ex_i2d *asn1_ex_i2d;
    ASN1_ex_print_func *asn1_ex_print;
    ASN1_ex_new_ex_func *asn1_ex_new_ex;
    ASN1_ex_d2i_ex *asn1_ex_d2i_ex;
} ASN1_EXTERN_FUNCS;

typedef struct ASN1_PRIMITIVE_FUNCS_st {
    void *app_data;
    unsigned long flags;
    ASN1_ex_new_func *prim_new;
    ASN1_ex_free_func *prim_free;
    ASN1_ex_free_func *prim_clear;
    ASN1_primitive_c2i *prim_c2i;
    ASN1_primitive_i2c *prim_i2c;
    ASN1_primitive_print *prim_print;
} ASN1_PRIMITIVE_FUNCS;

/*
 * This is the ASN1_AUX structure: it handles various miscellaneous
 * requirements. For example the use of reference counts and an informational
 * callback. The "informational callback" is called at various points during
 * the ASN1 encoding and decoding. It can be used to provide minor
 * customisation of the structures used. This is most useful where the
 * supplied routines *almost* do the right thing but need some extra help at
 * a few points. If the callback returns zero then it is assumed a fatal
 * error has occurred and the main operation should be abandoned. If major
 * changes in the default behaviour are required then an external type is
 * more appropriate.
 * For the operations ASN1_OP_I2D_PRE, ASN1_OP_I2D_POST, ASN1_OP_PRINT_PRE, and
 * ASN1_OP_PRINT_POST, meanwhile a variant of the callback with const parameter
 * 'in' is provided to make clear statically that its input is not modified. If
 * and only if this variant is in use the flag ASN1_AFLG_CONST_CB must be set.
 */

typedef int ASN1_aux_cb(int operation, ASN1_VALUE **in, const ASN1_ITEM *it,
                        void *exarg);
typedef int ASN1_aux_const_cb(int operation, const ASN1_VALUE **in,
                              const ASN1_ITEM *it, void *exarg);

typedef struct ASN1_AUX_st {
    void *app_data;
    int flags;
    int ref_offset;             /* Offset of reference value */
    int ref_lock;               /* Offset of lock value */
    ASN1_aux_cb *asn1_cb;
    int enc_offset;             /* Offset of ASN1_ENCODING structure */
    ASN1_aux_const_cb *asn1_const_cb; /* for ASN1_OP_I2D_ and ASN1_OP_PRINT_ */
} ASN1_AUX;

/* For print related callbacks exarg points to this structure */
typedef struct ASN1_PRINT_ARG_st {
    BIO *out;
    int indent;
    const ASN1_PCTX *pctx;
} ASN1_PRINT_ARG;

/* For streaming related callbacks exarg points to this structure */
typedef struct ASN1_STREAM_ARG_st {
    /* BIO to stream through */
    BIO *out;
    /* BIO with filters appended */
    BIO *ndef_bio;
    /* Streaming I/O boundary */
    unsigned char **boundary;
} ASN1_STREAM_ARG;

/* Flags in ASN1_AUX */

/* Use a reference count */
# define ASN1_AFLG_REFCOUNT      1
/* Save the encoding of structure (useful for signatures) */
# define ASN1_AFLG_ENCODING      2
/* The Sequence length is invalid */
# define ASN1_AFLG_BROKEN        4
/* Use the new asn1_const_cb */
# define ASN1_AFLG_CONST_CB      8

/* operation values for asn1_cb */

# define ASN1_OP_NEW_PRE         0
# define ASN1_OP_NEW_POST        1
# define ASN1_OP_FREE_PRE        2
# define ASN1_OP_FREE_POST       3
# define ASN1_OP_D2I_PRE         4
# define ASN1_OP_D2I_POST        5
# define ASN1_OP_I2D_PRE         6
# define ASN1_OP_I2D_POST        7
# define ASN1_OP_PRINT_PRE       8
# define ASN1_OP_PRINT_POST      9
# define ASN1_OP_STREAM_PRE      10
# define ASN1_OP_STREAM_POST     11
# define ASN1_OP_DETACHED_PRE    12
# define ASN1_OP_DETACHED_POST   13
# define ASN1_OP_DUP_PRE         14
# define ASN1_OP_DUP_POST        15
# define ASN1_OP_GET0_LIBCTX     16
# define ASN1_OP_GET0_PROPQ      17

/* Macro to implement a primitive type */
# define IMPLEMENT_ASN1_TYPE(stname) IMPLEMENT_ASN1_TYPE_ex(stname, stname, 0)
# define IMPLEMENT_ASN1_TYPE_ex(itname, vname, ex) \
                                ASN1_ITEM_start(itname) \
                                        ASN1_ITYPE_PRIMITIVE, V_##vname, NULL, 0, NULL, ex, #itname \
                                ASN1_ITEM_end(itname)

/* Macro to implement a multi string type */
# define IMPLEMENT_ASN1_MSTRING(itname, mask) \
                                ASN1_ITEM_start(itname) \
                                        ASN1_ITYPE_MSTRING, mask, NULL, 0, NULL, sizeof(ASN1_STRING), #itname \
                                ASN1_ITEM_end(itname)

# define IMPLEMENT_EXTERN_ASN1(sname, tag, fptrs) \
        ASN1_ITEM_start(sname) \
                ASN1_ITYPE_EXTERN, \
                tag, \
                NULL, \
                0, \
                &fptrs, \
                0, \
                #sname \
        ASN1_ITEM_end(sname)

/* Macro to implement standard functions in terms of ASN1_ITEM structures */

# define IMPLEMENT_ASN1_FUNCTIONS(stname) IMPLEMENT_ASN1_FUNCTIONS_fname(stname, stname, stname)

# define IMPLEMENT_ASN1_FUNCTIONS_name(stname, itname) IMPLEMENT_ASN1_FUNCTIONS_fname(stname, itname, itname)

# define IMPLEMENT_ASN1_FUNCTIONS_ENCODE_name(stname, itname) \
                        IMPLEMENT_ASN1_FUNCTIONS_ENCODE_fname(stname, itname, itname)

# define IMPLEMENT_STATIC_ASN1_ALLOC_FUNCTIONS(stname) \
                IMPLEMENT_ASN1_ALLOC_FUNCTIONS_pfname(static, stname, stname, stname)

# define IMPLEMENT_ASN1_ALLOC_FUNCTIONS(stname) \
                IMPLEMENT_ASN1_ALLOC_FUNCTIONS_fname(stname, stname, stname)

# define IMPLEMENT_ASN1_ALLOC_FUNCTIONS_pfname(pre, stname, itname, fname) \
        pre stname *fname##_new(void) \
        { \
                return (stname *)ASN1_item_new(ASN1_ITEM_rptr(itname)); \
        } \
        pre void fname##_free(stname *a) \
        { \
                ASN1_item_free((ASN1_VALUE *)a, ASN1_ITEM_rptr(itname)); \
        }

# define IMPLEMENT_ASN1_ALLOC_FUNCTIONS_fname(stname, itname, fname) \
        stname *fname##_new(void) \
        { \
                return (stname *)ASN1_item_new(ASN1_ITEM_rptr(itname)); \
        } \
        void fname##_free(stname *a) \
        { \
                ASN1_item_free((ASN1_VALUE *)a, ASN1_ITEM_rptr(itname)); \
        }

# define IMPLEMENT_ASN1_FUNCTIONS_fname(stname, itname, fname) \
        IMPLEMENT_ASN1_ENCODE_FUNCTIONS_fname(stname, itname, fname) \
        IMPLEMENT_ASN1_ALLOC_FUNCTIONS_fname(stname, itname, fname)

# define IMPLEMENT_ASN1_ENCODE_FUNCTIONS_fname(stname, itname, fname) \
        stname *d2i_##fname(stname **a, const unsigned char **in, long len) \
        { \
                return (stname *)ASN1_item_d2i((ASN1_VALUE **)a, in, len, ASN1_ITEM_rptr(itname));\
        } \
        int i2d_##fname(const stname *a, unsigned char **out) \
        { \
                return ASN1_item_i2d((const ASN1_VALUE *)a, out, ASN1_ITEM_rptr(itname));\
        }

# define IMPLEMENT_ASN1_NDEF_FUNCTION(stname) \
        int i2d_##stname##_NDEF(const stname *a, unsigned char **out) \
        { \
                return ASN1_item_ndef_i2d((const ASN1_VALUE *)a, out, ASN1_ITEM_rptr(stname));\
        }

# define IMPLEMENT_STATIC_ASN1_ENCODE_FUNCTIONS(stname) \
        static stname *d2i_##stname(stname **a, \
                                   const unsigned char **in, long len) \
        { \
                return (stname *)ASN1_item_d2i((ASN1_VALUE **)a, in, len, \
                                               ASN1_ITEM_rptr(stname)); \
        } \
        static int i2d_##stname(const stname *a, unsigned char **out) \
        { \
                return ASN1_item_i2d((const ASN1_VALUE *)a, out, \
                                     ASN1_ITEM_rptr(stname)); \
        }

# define IMPLEMENT_ASN1_DUP_FUNCTION(stname) \
        stname * stname##_dup(const stname *x) \
        { \
        return ASN1_item_dup(ASN1_ITEM_rptr(stname), x); \
        }

# define IMPLEMENT_ASN1_PRINT_FUNCTION(stname) \
        IMPLEMENT_ASN1_PRINT_FUNCTION_fname(stname, stname, stname)

# define IMPLEMENT_ASN1_PRINT_FUNCTION_fname(stname, itname, fname) \
        int fname##_print_ctx(BIO *out, const stname *x, int indent, \
                                                const ASN1_PCTX *pctx) \
        { \
                return ASN1_item_print(out, (const ASN1_VALUE *)x, indent, \
                        ASN1_ITEM_rptr(itname), pctx); \
        }

/* external definitions for primitive types */

DECLARE_ASN1_ITEM(ASN1_BOOLEAN)
DECLARE_ASN1_ITEM(ASN1_TBOOLEAN)
DECLARE_ASN1_ITEM(ASN1_FBOOLEAN)
DECLARE_ASN1_ITEM(ASN1_SEQUENCE)
DECLARE_ASN1_ITEM(CBIGNUM)
DECLARE_ASN1_ITEM(BIGNUM)
DECLARE_ASN1_ITEM(INT32)
DECLARE_ASN1_ITEM(ZINT32)
DECLARE_ASN1_ITEM(UINT32)
DECLARE_ASN1_ITEM(ZUINT32)
DECLARE_ASN1_ITEM(INT64)
DECLARE_ASN1_ITEM(ZINT64)
DECLARE_ASN1_ITEM(UINT64)
DECLARE_ASN1_ITEM(ZUINT64)

# ifndef OPENSSL_NO_DEPRECATED_3_0
/*
 * LONG and ZLONG are strongly discouraged for use as stored data, as the
 * underlying C type (long) differs in size depending on the architecture.
 * They are designed with 32-bit longs in mind.
 */
DECLARE_ASN1_ITEM(LONG)
DECLARE_ASN1_ITEM(ZLONG)
# endif



SKM_DEFINE_STACK_OF_INTERNAL(ASN1_VALUE, ASN1_VALUE, ASN1_VALUE)
#define sk_ASN1_VALUE_num(sk) OPENSSL_sk_num(ossl_check_const_ASN1_VALUE_sk_type(sk))
#define sk_ASN1_VALUE_value(sk, idx) ((ASN1_VALUE *)OPENSSL_sk_value(ossl_check_const_ASN1_VALUE_sk_type(sk), (idx)))
#define sk_ASN1_VALUE_new(cmp) ((STACK_OF(ASN1_VALUE) *)OPENSSL_sk_new(ossl_check_ASN1_VALUE_compfunc_type(cmp)))
#define sk_ASN1_VALUE_new_null() ((STACK_OF(ASN1_VALUE) *)OPENSSL_sk_new_null())
#define sk_ASN1_VALUE_new_reserve(cmp, n) ((STACK_OF(ASN1_VALUE) *)OPENSSL_sk_new_reserve(ossl_check_ASN1_VALUE_compfunc_type(cmp), (n)))
#define sk_ASN1_VALUE_reserve(sk, n) OPENSSL_sk_reserve(ossl_check_ASN1_VALUE_sk_type(sk), (n))
#define sk_ASN1_VALUE_free(sk) OPENSSL_sk_free(ossl_check_ASN1_VALUE_sk_type(sk))
#define sk_ASN1_VALUE_zero(sk) OPENSSL_sk_zero(ossl_check_ASN1_VALUE_sk_type(sk))
#define sk_ASN1_VALUE_delete(sk, i) ((ASN1_VALUE *)OPENSSL_sk_delete(ossl_check_ASN1_VALUE_sk_type(sk), (i)))
#define sk_ASN1_VALUE_delete_ptr(sk, ptr) ((ASN1_VALUE *)OPENSSL_sk_delete_ptr(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr)))
#define sk_ASN1_VALUE_push(sk, ptr) OPENSSL_sk_push(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr))
#define sk_ASN1_VALUE_unshift(sk, ptr) OPENSSL_sk_unshift(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr))
#define sk_ASN1_VALUE_pop(sk) ((ASN1_VALUE *)OPENSSL_sk_pop(ossl_check_ASN1_VALUE_sk_type(sk)))
#define sk_ASN1_VALUE_shift(sk) ((ASN1_VALUE *)OPENSSL_sk_shift(ossl_check_ASN1_VALUE_sk_type(sk)))
#define sk_ASN1_VALUE_pop_free(sk, freefunc) OPENSSL_sk_pop_free(ossl_check_ASN1_VALUE_sk_type(sk),ossl_check_ASN1_VALUE_freefunc_type(freefunc))
#define sk_ASN1_VALUE_insert(sk, ptr, idx) OPENSSL_sk_insert(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr), (idx))
#define sk_ASN1_VALUE_set(sk, idx, ptr) ((ASN1_VALUE *)OPENSSL_sk_set(ossl_check_ASN1_VALUE_sk_type(sk), (idx), ossl_check_ASN1_VALUE_type(ptr)))
#define sk_ASN1_VALUE_find(sk, ptr) OPENSSL_sk_find(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr))
#define sk_ASN1_VALUE_find_ex(sk, ptr) OPENSSL_sk_find_ex(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr))
#define sk_ASN1_VALUE_find_all(sk, ptr, pnum) OPENSSL_sk_find_all(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_type(ptr), pnum)
#define sk_ASN1_VALUE_sort(sk) OPENSSL_sk_sort(ossl_check_ASN1_VALUE_sk_type(sk))
#define sk_ASN1_VALUE_is_sorted(sk) OPENSSL_sk_is_sorted(ossl_check_const_ASN1_VALUE_sk_type(sk))
#define sk_ASN1_VALUE_dup(sk) ((STACK_OF(ASN1_VALUE) *)OPENSSL_sk_dup(ossl_check_const_ASN1_VALUE_sk_type(sk)))
#define sk_ASN1_VALUE_deep_copy(sk, copyfunc, freefunc) ((STACK_OF(ASN1_VALUE) *)OPENSSL_sk_deep_copy(ossl_check_const_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_copyfunc_type(copyfunc), ossl_check_ASN1_VALUE_freefunc_type(freefunc)))
#define sk_ASN1_VALUE_set_cmp_func(sk, cmp) ((sk_ASN1_VALUE_compfunc)OPENSSL_sk_set_cmp_func(ossl_check_ASN1_VALUE_sk_type(sk), ossl_check_ASN1_VALUE_compfunc_type(cmp)))





/* Functions used internally by the ASN1 code */

int ASN1_item_ex_new(ASN1_VALUE **pval, const ASN1_ITEM *it);
void ASN1_item_ex_free(ASN1_VALUE **pval, const ASN1_ITEM *it);

int ASN1_item_ex_d2i(ASN1_VALUE **pval, const unsigned char **in, long len,
                     const ASN1_ITEM *it, int tag, int aclass, char opt,
                     ASN1_TLC *ctx);

int ASN1_item_ex_i2d(const ASN1_VALUE **pval, unsigned char **out,
                     const ASN1_ITEM *it, int tag, int aclass);

/* Legacy compatibility */
# define IMPLEMENT_ASN1_FUNCTIONS_const(name) IMPLEMENT_ASN1_FUNCTIONS(name)
# define IMPLEMENT_ASN1_ENCODE_FUNCTIONS_const_fname(stname, itname, fname) \
         IMPLEMENT_ASN1_ENCODE_FUNCTIONS_fname(stname, itname, fname)

#ifdef  __cplusplus
}
#endif
#endif
