/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_mouse_gesture_state_changed {
    bool is_active;
    int32_t instance_id; // Which gesture processor instance to activate
};

ZMK_EVENT_DECLARE(zmk_mouse_gesture_state_changed);
