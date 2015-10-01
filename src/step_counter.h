//step_counter.h


void init_step_counter(void);


/**@brief Function for reading an ADC pin
 */
unsigned char adc_start(unsigned char input);

void process_count(void);
uint32_t get_steps(void);

void log_adc(void);
void process_log(void);
