#pragma once

#include <stdint.h>

struct motor_group;

// pins must be an array of length 8 - it is undefined behavior otherwise
struct motor_group* motor_make_group(const int* pins);
void                motor_free_group(struct motor_group* mg);

void motor_forward(struct motor_group* mg);
void motor_stop(struct motor_group* mg);
void motor_reverse(struct motor_group* mg);
void motor_rotate_clockwise(struct motor_group* mg);
void motor_rotate_counterwise(struct motor_group* mg);

