/****************************************************************************
**
** SPDX-License-Identifier: BSD-2-Clause-Patent
**
** SPDX-FileCopyrightText: Copyright (c) 2023 SoftAtHome
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
** this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice,
** this list of conditions and the following disclaimer in the documentation
** and/or other materials provided with the distribution.
**
** Subject to the terms and conditions of this license, each copyright holder
** and contributor hereby grants to those receiving rights under this license
** a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable
** (except for failure to satisfy the conditions of this license) patent license
** to make, have made, use, offer to sell, sell, import, and otherwise transfer
** this software, where such license applies only to those patent claims, already
** acquired or hereafter acquired, licensable by such copyright holder or contributor
** that are necessarily infringed by:
**
** (a) their Contribution(s) (the licensed copyrights of copyright holders and
** non-copyrightable additions of contributors, in source or binary form) alone;
** or
**
** (b) combination of their Contribution(s) with the work of authorship to which
** such Contribution(s) was added by such copyright holder or contributor, if,
** at the time the Contribution is added, such addition causes such combination
** to be necessarily infringed. The patent license shall not apply to any other
** combinations which include the Contribution.
**
** Except as expressly stated above, no rights or licenses from any copyright
** holder or contributor is granted under this license, whether expressly, by
** implication, estoppel or otherwise.
**
** DISCLAIMER
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
** USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <cmocka.h>

#include <amxc/amxc.h>
#include <amxp/amxp_signal.h>
#include <amxp/amxp_slot.h>

#include <amxd/amxd_common.h>
#include <amxd/amxd_dm.h>
#include <amxd/amxd_object.h>
#include <amxd/amxd_parameter.h>
#include <amxd/amxd_function.h>
#include <amxd/amxd_action.h>
#include <amxd/amxd_transaction.h>

#include "test_amxd_transaction_sub.h"

#include <amxc/amxc_macros.h>
static amxd_dm_t dm;

static void test_check_change_event(const char* const sig_name,
                                    const amxc_var_t* const data,
                                    UNUSED void* const priv) {
    const char* path = GET_CHAR(data, "path");
    const char* param_from = GETP_CHAR(data, "parameters.Text.from");
    const char* param_to = GETP_CHAR(data, "parameters.Text.to");
    amxc_var_dump(data, STDOUT_FILENO);

    check_expected(sig_name);
    check_expected(path);
    check_expected(param_from);
    check_expected(param_to);
}

static void test_check_event(const char* const sig_name,
                             UNUSED const amxc_var_t* const data,
                             UNUSED void* const priv) {
    amxc_var_dump(data, STDOUT_FILENO);

    check_expected(sig_name);
}

void test_build_dm(void) {
    amxd_object_t* object = NULL;
    amxd_object_t* templ = NULL;
    amxd_object_t* child = NULL;
    amxd_param_t* param = NULL;

    assert_int_equal(amxd_dm_init(&dm), 0);

    // object Top1 {
    assert_int_equal(amxd_object_new(&object, amxd_object_singleton, "Top1"), 0);
    assert_int_equal(amxd_dm_add_root_object(&dm, object), 0);
    //    string Text;
    assert_int_equal(amxd_param_new(&param, "Text", AMXC_VAR_ID_CSTRING), 0);
    assert_int_equal(amxd_object_add_param(object, param), 0);

    //     object Child {
    assert_int_equal(amxd_object_new(&child, amxd_object_singleton, "Child"), 0);
    assert_int_equal(amxd_object_add_object(object, child), 0);
    //         string Text;
    assert_int_equal(amxd_param_new(&param, "Text", AMXC_VAR_ID_CSTRING), 0);
    assert_int_equal(amxd_object_add_param(child, param), 0);
    //     }
    // }

    // object Top2 {
    assert_int_equal(amxd_object_new(&object, amxd_object_singleton, "Top2"), 0);
    assert_int_equal(amxd_dm_add_root_object(&dm, object), 0);
    //    string Text;
    assert_int_equal(amxd_param_new(&param, "Text", AMXC_VAR_ID_CSTRING), 0);
    assert_int_equal(amxd_object_add_param(object, param), 0);

    //     object Child[] {
    assert_int_equal(amxd_object_new(&templ, amxd_object_template, "Child"), 0);
    assert_int_equal(amxd_object_add_object(object, templ), 0);
    //         string Text;
    assert_int_equal(amxd_param_new(&param, "Text", AMXC_VAR_ID_CSTRING), 0);
    assert_int_equal(amxd_object_add_param(templ, param), 0);
    //         uint32 Number;
    assert_int_equal(amxd_param_new(&param, "Number", AMXC_VAR_ID_UINT32), 0);
    assert_int_equal(amxd_object_add_param(templ, param), 0);
    //         object Child {
    assert_int_equal(amxd_object_new(&child, amxd_object_singleton, "Child"), 0);
    assert_int_equal(amxd_object_add_object(templ, child), 0);
    //         string Text;
    assert_int_equal(amxd_param_new(&param, "Text", AMXC_VAR_ID_CSTRING), 0);
    assert_int_equal(amxd_object_add_param(child, param), 0);
    //         uint32 Number;
    assert_int_equal(amxd_param_new(&param, "Number", AMXC_VAR_ID_UINT32), 0);
    assert_int_equal(amxd_object_add_param(child, param), 0);
    //         }
    //     }
    // }
}

amxd_dm_t* test_get_dm(void) {
    return &dm;
}

void test_clean_dm(void) {
    amxd_dm_clean(&dm);
}

void test_amxd_can_set_sub_object_params(UNUSED void** state) {
    amxd_trans_t transaction;
    amxd_object_t* obj = NULL;
    char* text = NULL;
    amxd_status_t status = amxd_status_ok;

    test_build_dm();

    // read all pending events
    while(amxp_signal_read() == 0) {
    }

    assert_int_equal(amxd_trans_init(&transaction), 0);
    assert_int_equal(amxd_trans_select_pathf(&transaction, "Top1"), 0);
    assert_int_equal(amxd_trans_set_value(cstring_t, &transaction, "Text", "Hello"), 0);
    assert_int_equal(amxd_trans_set_value(cstring_t, &transaction, "Child.Text", "World"), 0);

    amxd_trans_dump(&transaction, STDOUT_FILENO, false);
    assert_int_equal(amxd_trans_apply(&transaction, test_get_dm()), 0);
    amxd_trans_clean(&transaction);

    // check Top1.Text == "Hello"
    obj = amxd_dm_findf(test_get_dm(), "Top1.");
    assert_non_null(obj);
    text = amxd_object_get_value(cstring_t, obj, "Text", &status);
    assert_non_null(text);
    assert_string_equal(text, "Hello");
    free(text);

    // check Top1.Child.Text == "World"
    obj = amxd_dm_findf(test_get_dm(), "Top1.Child.");
    assert_non_null(obj);
    text = amxd_object_get_value(cstring_t, obj, "Text", &status);
    assert_non_null(text);
    assert_string_equal(text, "World");
    free(text);
}

void test_amxd_check_two_change_events_are_send(UNUSED void** state) {
    amxd_dm_t* dm = NULL;
    dm = test_get_dm();

    assert_int_equal(amxp_slot_connect(&dm->sigmngr,
                                       "dm:object-changed",
                                       NULL,
                                       test_check_change_event,
                                       NULL), 0);

    expect_string(test_check_change_event, sig_name, "dm:object-changed");
    expect_string(test_check_change_event, path, "Top1.Child.");
    expect_string(test_check_change_event, param_from, "");
    expect_string(test_check_change_event, param_to, "World");

    expect_string(test_check_change_event, sig_name, "dm:object-changed");
    expect_string(test_check_change_event, path, "Top1.");
    expect_string(test_check_change_event, param_from, "");
    expect_string(test_check_change_event, param_to, "Hello");

    while(amxp_signal_read() == 0) {
    }

    test_clean_dm();
}

void test_amxd_can_create_instance_and_set_sub_object_params(UNUSED void** state) {
    amxd_trans_t transaction;
    amxd_object_t* obj = NULL;
    char* text = NULL;
    amxd_status_t status = amxd_status_ok;

    test_build_dm();

    // read all pending events
    while(amxp_signal_read() == 0) {
    }

    assert_int_equal(amxd_trans_init(&transaction), 0);
    assert_int_equal(amxd_trans_select_pathf(&transaction, "Top2.Child."), 0);
    assert_int_equal(amxd_trans_add_inst(&transaction, 0, NULL), 0);
    assert_int_equal(amxd_trans_set_value(cstring_t, &transaction, "Text", "Hello"), 0);
    assert_int_equal(amxd_trans_set_value(cstring_t, &transaction, "Child.Text", "World"), 0);
    assert_int_equal(amxd_trans_set_value(uint32_t, &transaction, "Number", 100), 0);
    assert_int_equal(amxd_trans_set_value(uint32_t, &transaction, "Child.Number", 101), 0);

    amxd_trans_dump(&transaction, STDOUT_FILENO, false);
    assert_int_equal(amxd_trans_apply(&transaction, test_get_dm()), 0);
    amxd_trans_clean(&transaction);

    obj = amxd_dm_findf(test_get_dm(), "Top2.Child.1.");
    assert_non_null(obj);
    text = amxd_object_get_value(cstring_t, obj, "Text", &status);
    assert_non_null(text);
    assert_string_equal(text, "Hello");
    assert_int_equal(amxd_object_get_value(uint32_t, obj, "Number", &status), 100);
    free(text);

    obj = amxd_dm_findf(test_get_dm(), "Top2.Child.1.Child.");
    assert_non_null(obj);
    text = amxd_object_get_value(cstring_t, obj, "Text", &status);
    assert_non_null(text);
    assert_string_equal(text, "World");
    assert_int_equal(amxd_object_get_value(uint32_t, obj, "Number", &status), 101);
    free(text);
}

void test_amxd_check_two_events_are_send(UNUSED void** state) {
    amxd_dm_t* dm = NULL;
    dm = test_get_dm();

    assert_int_equal(amxp_slot_connect(&dm->sigmngr,
                                       "dm:instance-added",
                                       NULL,
                                       test_check_event,
                                       NULL), 0);

    assert_int_equal(amxp_slot_connect(&dm->sigmngr,
                                       "dm:object-added",
                                       NULL,
                                       test_check_event,
                                       NULL), 0);

    expect_string(test_check_event, sig_name, "dm:instance-added");
    expect_string(test_check_event, sig_name, "dm:object-added");

    while(amxp_signal_read() == 0) {
    }

    test_clean_dm();
}
