// fall_detection.h

uint8_t fd_check_for_impact (double sum_vector);
bool fd_check_motion_interrupt (void);
bool fd_check_zero_g (void);
void fd_check_for_velocity (int x, int y, int z);

