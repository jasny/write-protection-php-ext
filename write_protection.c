/*
  +----------------------------------------------------------------------+
  | Skeleton PHP extension                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2018 NAME                                              |
  +----------------------------------------------------------------------+
  | Permission is hereby granted, free of charge, to any person          |
  | obtaining a copy of this software and associated documentation files |
  | (the "Software"), to deal in the Software without restriction,       |
  | including without limitation the rights to use, copy, modify, merge, |
  | publish, distribute, sublicense, and/or sell copies of the Software, |
  | and to permit persons to whom the Software is furnished to do so,    |
  | subject to the following conditions:                                 |
  |                                                                      |
  | The above copyright notice and this permission notice shall be       |
  | included in all copies or substantial portions of the Software.      |
  |                                                                      |
  | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      |
  | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   |
  | MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                |
  | NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  |
  | BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   |
  | ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    |
  | CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     |
  | SOFTWARE.                                                            |
  +----------------------------------------------------------------------+
  | Author: NAME <EMAIL@EXAMPLE.COM>                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_write_protection.h"
#include "zend_objects_API.h"
#include "zend_interfaces.h"

#if HAVE_WRITE_PROTECTION

zend_class_entry *WriteProtection_ce = NULL;
static zend_object_handlers WriteProtection_object_handlers;

/* Argument info for each function, used for reflection */
ZEND_BEGIN_ARG_INFO_EX(arginfo_write_protection_nop, 0, 1, 0)
    ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
ZEND_END_ARG_INFO()

zend_module_entry write_protection_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_WRITE_PROTECTION_EXTNAME,
    NULL,
    PHP_MINIT(write_protection),
    NULL,
    NULL,
    NULL,
    NULL,
    PHP_WRITE_PROTECTION_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_WRITE_PROTECTION
ZEND_GET_MODULE(write_protection)
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_setPublicProperty, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unsetPublicProperty, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry methods[] = {
    ZEND_ABSTRACT_ME(WriteProtection, setPublicProperty, arginfo_setPublicProperty)
    ZEND_ABSTRACT_ME(WriteProtection, unsetPublicProperty, arginfo_unsetPublicProperty)
    PHP_FE_END
};

ZEND_API zend_object *WriteProtection_new(zend_class_entry *ce)
{
    zend_object *ob;

    ob = zend_object_alloc(sizeof(*ob), ce);
    zend_object_std_init(ob, ce);
    object_properties_init(ob, ce);
    ob->handlers = &WriteProtection_object_handlers;

    return ob;
}

static zend_bool apply_write_protection(zend_class_entry *ce, zend_string *member)
{
    zval *zv;
    zend_property_info *property_info;
    uint32_t flags;
    zend_class_entry *scope;

    if (UNEXPECTED(zend_hash_num_elements(&ce->properties_info) == 0)
        || UNEXPECTED((zv = zend_hash_find(&ce->properties_info, member)) == NULL)) {
        return 0;
    }

    property_info = (zend_property_info*)Z_PTR_P(zv);
    flags = property_info->flags;

    if (~flags & ZEND_ACC_PUBLIC) {
        return 0;
    }

    scope = UNEXPECTED(EG(fake_scope)) ? EG(fake_scope) : zend_get_executed_scope();

    return property_info->ce != scope;
}

static zval *protected_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
    if (apply_write_protection(Z_OBJ_P(object)->ce, Z_STR_P(member))) {
        zend_call_method_with_2_params(object, NULL, NULL, "setPublicProperty", NULL, member, value);
    } else {
        zend_std_write_property(object, member, value, cache_slot);
    }
}

static void protected_unset_property(zval *object, zval *member, void **cache_slot)
{
    if (apply_write_protection(Z_OBJ_P(object)->ce, Z_STR_P(member))) {
        zend_call_method_with_1_params(object, NULL, NULL, "unsetPublicProperty", NULL, member);
    } else {
        zend_std_unset_property(object, member, cache_slot);
    }
}

PHP_MINIT_FUNCTION(write_protection)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "WriteProtection", methods);

    WriteProtection_ce = zend_register_internal_class(&ce);
    WriteProtection_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    WriteProtection_ce->create_object = WriteProtection_new;

    memcpy(&WriteProtection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
    WriteProtection_object_handlers.write_property = protected_write_property;
    WriteProtection_object_handlers.unset_property = protected_unset_property;

    return SUCCESS;
}

#endif
