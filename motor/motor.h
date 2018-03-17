#pragma once

struct motor_group;

// pins must be an array of length 8 - it is undefined behavior otherwise
struct motor_group* motor_make_group(int* pins);
void motor_free_group(struct motor_group* mg);

int motor_forward(struct motor_group* mg);
int motor_stop(struct motor_group* mg);
int motor_reverse(struct motor_group* mg);
int motor_rotate_right(struct motor_group* mg);
int motor_rotate_left(struct motor_group* mg);

