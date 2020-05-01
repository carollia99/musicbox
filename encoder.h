extern unsigned volatile char encoder_new_state, encoder_old_state;
extern unsigned volatile char encoder_changed;  // Flag for state change
extern unsigned volatile char encoderA, encoderB;
extern unsigned volatile char encoderVal;
extern unsigned volatile char encoder_changed_up;

void change_note_ifneeded(void);