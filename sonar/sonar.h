#pragma once

struct sonar;

void sonar_init();
void sonar_deinit();

struct sonar* sonar_make(int pin_trig, int pin_echo, double* data_ptr);
void sonar_free(struct sonar* s);

// sonar_test performs sonar_trigger, but tracks errors that occur
double sonar_test(struct sonar* s, int* trig_err, int* echo_err, int* time_err);

// sonar_trigger returns the distance found in centimeters
// note: it is recommended to wait at least 60 milliseconds between trigger to prevent echos from previous triggers to interfere with subsequent echo readings
double sonar_trigger(struct sonar* s);

void sonar_get_all(struct sonar** sensors, int length);

