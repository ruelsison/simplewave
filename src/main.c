/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup from button, advertise, get a connection
 * restart advertising on disconnect and if no new connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified with 'YOUR_JOB' indicates where
 * and how you can customize.
 */

#include "main_includes.h"
#include "step_counter_new.h"
#include "accelerometer.h"
#include "fall_detection.h"
#include "Snapshot.h"
#include "memory_map.h"
#include "timestamp.h"
#include "wdt.h"
#include "averages.h"

#include "ActivityLog.h"

#include "coefficients.h"

// Defines for LED pins
#include "LEDS.H"
//#include "globals.h"

int32_t xRaw;
int32_t yRaw;
int32_t zRaw;
uint16_t clockTicks = 0;
uint16_t count = 0;
uint16_t zeroGticks = 0;
double curr_sv;

float xAngleBeforeImpact;
float yAngleBeforeImpact;
float xAngleAfterImpact;
float yAngleAfterImpact;

//uint16_t nSteps = 0;
float activityToSave = 0;
//uint8_t falls = 0;

uint8_t Dumper = 0;
uint8_t SetTimestamp = 0;
uint8_t ReadDataBuffer = 0;
uint8_t ClearDataBuffer = 0;
uint8_t ReadSnapshots = 0;
uint8_t ClearSnapshots = 0;
uint8_t SaveSnapshot = 0;
uint8_t writeLog = 0;
uint8_t emergencyCall = 0;
uint8_t ReadBigSnapshot = 0;
uint8_t sendRawData = 0;
uint8_t BLESessionActive = 0;
uint8_t clearPerso = 0;
uint8_t rewritePerso = 0;

uint8_t ReadDataBufferPartial = 0;
uint8_t PartialCount1 = 0;
uint8_t PartialCount2 = 0;
uint8_t PartialCountTotal = 0;

uint8_t ReadSnapshotsPartial = 0;
uint8_t PartialSnap1 = 0;
uint8_t PartialSnap2 = 0;
uint8_t PartialSnapTotal = 0;

bool zero_g_detected = false;
uint8_t fall_detected;
bool do_post_processing = false;

char outbuf[20];

#define MAX_USERNAME_LENGTH 20
#define MAX_COEFF_LENGTH 12
/*
char tempUsername[MAX_USERNAME_LENGTH];
uint16_t tempUsernameLength = 0;
uint8_t writeUsername = 0;

char UsernameBuffer[MAX_USERNAME_LENGTH];

char tempXYZ[MAX_COEFF_LENGTH];
uint16_t tempXYZLength = 0;
uint8_t writeXYZ = 0;

char XYZBuffer[MAX_COEFF_LENGTH];

char tempSV[MAX_COEFF_LENGTH];
uint16_t tempSVLength = 0;
uint8_t writeSV = 0;

char SVBuffer[MAX_COEFF_LENGTH];

*/

uint32_t error_flash;

Sensor_Reading* rdg;

void post_comm_processing(void);
void errorLEDs(void);

#define OUTPUT_POWER                -4

void commandAPI_handler(ble_nus_t * p_nus, uint8_t * data, uint16_t length);

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
ble_nus_t                        	    m_nus;

/**@brief Error handler function, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    //nrf_gpio_pin_set(ASSERT_LED_PIN_NO);
	
		errorLEDs();

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    //ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    //NVIC_SystemReset();
		sd_nvic_SystemReset();
}


/**@brief Assert macro callback function.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Service error handler.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the 
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
/*
// YOUR_JOB: Uncomment this function and make it handle error situations sent back to your 
//           application by the services it uses.
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
} */


/**@brief Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    
    ble_uuid_t adv_uuids[] = {{BLE_UUID_NUS_SERVICE, m_nus.uuid_type}};

    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = false;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;
    
    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
}

/**@brief Start advertising.
 */
static void advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
    
    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));
    
    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
    
    //tx_power Radio transmit power in dBm (accepted values are -40, -30, -20, -16, -12, -8, -4, 0, and 4 dBm)
    err_code = sd_ble_gap_tx_power_set(OUTPUT_POWER);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Stop advertising.
 */
static void advertising_stop(void)
{
    //uint32_t err_code;

    // If connected, disconnect
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        sd_ble_gap_disconnect(m_conn_handle,  BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        
    }

    // Stop advertising
    sd_ble_gap_adv_stop();

    // Disable the radio tasks as scytulip suggested
    // https://devzone.nordicsemi.com/question/13984/completely-disabling-bluetooth/
    NRF_RADIO->TASKS_DISABLE;
    
}    

/***TIMERS***/

#define TIMER_50HZ_INTERVAL (APP_TIMER_TICKS(20,APP_TIMER_PRESCALER)) //50Hz sampling rate
#define TIMER_20HZ_INTERVAL (APP_TIMER_TICKS(50,APP_TIMER_PRESCALER)) //20Hz sampling rate
#define TIMER_10HZ_INTERVAL (APP_TIMER_TICKS(100,APP_TIMER_PRESCALER)) //10Hz sampling rate
#define TIMER_1HZ_INTERVAL (APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER)) //1Hz sampling rate
#define TIMER_4HZ_INTERVAL (APP_TIMER_TICKS(250,APP_TIMER_PRESCALER)) //4Hz sampling rate
#define TIMER_3S_SINGLE (APP_TIMER_TICKS(3000,APP_TIMER_PRESCALER)) //3 Second countdown

static void timers_start(void);
//static void timer_stop (void); //(app_timer_id_t timer_id);

app_timer_id_t m_app_timer_id;
app_timer_id_t m_app_call_id;
app_timer_id_t m_app_clock_id;

void makeTheCall(void) {
	uint32_t err_code;
	nrf_gpio_pin_write(LED_FALL_DETECTED,1);
	// set timer to execute callLow() after 3 seconds.
	err_code = app_timer_start(m_app_call_id, TIMER_3S_SINGLE, NULL);
  APP_ERROR_CHECK(err_code);
	
}

void callLow(void * p_context) {
	
	nrf_gpio_pin_write(LED_FALL_DETECTED,0);
	//SaveSnapshot = 1;
	do_post_processing = true;
}

float getR(int x,int y, int z);

void clockOps (void * p_context)
{
	uint16_t tickCount = tickSecond();
	char outbuf[20];
	
	initCoefficients();
	
	if (BLESessionActive == 1)
	{
/*	
		if (fd_check_motion_interrupt())
		{
			sprintf (outbuf, "Motion Interrupt");
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
		}
		
		if (fd_check_zero_g())
		{
			sprintf (outbuf, "ZeroG Interrupt");
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
		}

*/		

		
		if (sendRawData == 1) 
		{
			float p,r;
			
			rdg = rawDataPeek();
			
			sprintf (outbuf, "A: x%04xy%04xz%04x",rdg->val.x_ac,rdg->val.y_ac,rdg->val.z_ac);
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
//			sprintf (outbuf, "R: %04f",getR(rdg->val.x_ac,rdg->val.y_ac,rdg->val.z_ac));
//			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			
			accel_get_angles(&p,&r);
			sprintf (outbuf, "p: %04f, r: %04f",p,r);
			ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
			
			if (checkOrientation() == ORIENTATION_STANDING)
			{
				sprintf (outbuf, "standing");
				ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 8);
			}
			else
			{	
				sprintf (outbuf, "not standing");
				ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 12);
			}
			
			
		}
		
		return;
	}
	
#define INCLUDE_STEP_COUNTER

#ifdef INCLUDE_STEP_COUNTER
	// 5 minute backup, currently at 1sec
	#define DATA_SAVE_INTERVAL (120)
	if (tickCount % DATA_SAVE_INTERVAL == 0)
    {
			
			
			nrf_gpio_pin_set(LED_NO_FALL);
			nrf_delay_ms(40);
			nrf_gpio_pin_clear(LED_NO_FALL);
			
//			if (nSteps > 0) 
			{
				
				activityToSave = getActivityLevel();
				writeLog = 1; //writeLogEntry(nSteps);
				do_post_processing = true;
				updatePeakAverages();
			}
//			else
			{
				nrf_gpio_pin_set(LED_NO_FALL);
				nrf_gpio_pin_clear(LED_RESETTING);
				nrf_delay_ms(40);
				nrf_gpio_pin_clear(LED_NO_FALL);
				nrf_gpio_pin_set(LED_RESETTING);
				nrf_delay_ms(40);
				nrf_gpio_pin_clear(LED_RESETTING);
			}
    }
	#endif  //INCLUDE_STEP_COUNTER
}


/**
 * @brief Timer routine for fall & step detection.
 */
void process_step_count(void * p_context)
{
  //timer_stop(m_app_timer_id);
	
	reset_wdt();
	
	nrf_gpio_pin_toggle(LED_RESETTING); 
	
	if (NRF_NVMC->READY == NVMC_READY_READY_Busy)
  {
        // Do not interfere if flash write is ongoing.
			return;
  }

  
	
	if (SaveSnapshot == 1)
	{
		nrf_gpio_pin_set(LED_RESETTING);
		return;
	}
	
	if (ReadBigSnapshot == 1)
		return;

    
    // store snapshot
		readSensorsAndStore(); 
	
	//process activity levels
		updateAverages();
		curr_sv = latestResultant();

	
	if (BLESessionActive == 1)
		return;
	
		if (curr_sv < getPerso()->freefall_threshold) {
			zero_g_detected = true;	
		}
	
//		if ( checkOrientation() == ORIENTATION_STANDING){
//			if (curr_sv < getPerso()->freefall_threshold){
//					zero_g_detected = true;
//				}
//			}
//		else if (checkOrientation() == ORIENTATION_NOT_STANDING)
//		{
//			if (curr_sv < NOT_STANDING_FREEFALL_THRESHOLD_DEFAULT){
//				zero_g_detected = true;
//			}
//		}
		else {
			zero_g_detected = false;
			
		}
    
    //zero_g_detected = fd_check_zero_g();    
    if (zero_g_detected == true) 
    {        
      nrf_gpio_pin_write(LED_OTHER,1);
			
//			if (count == 0) 
				accel_get_angles(&xAngleBeforeImpact, &yAngleBeforeImpact);			
			
			count = getPerso()->fall_detection_window;
		}
			
			if (count > 0)
			{
				count--;
					
				fall_detected = fd_check_for_impact(curr_sv);
					if (fall_detected == 0xFF) 
					{
							accel_get_angles(&xAngleAfterImpact, &yAngleAfterImpact);
						
	//						if (abs(xAngleAfterImpact - xAngleBeforeImpact) >= getPerso()->x_angle_threshold && 
	//								abs(yAngleAfterImpact - yAngleBeforeImpact) >= getPerso()->y_angle_threshold)
							{                   
									ClearDataBuffer = ClearSnapshots = SetTimestamp = 0;
								
							//		backupRawData();
								
									do_post_processing = true;
									
									emergencyCall = 1;

									zero_g_detected = false;
									fall_detected = 0;
									count = 0;
									
							}
							
					}
					
			}

			if (count == 0)
				nrf_gpio_pin_write(LED_OTHER,0);
 /*   
			else 
			{
				// detect if step occurred
				if (detect_step(xRaw,yRaw,zRaw)==1) 
						{
								nSteps++;
						}
			}
*/
 
}


/**@brief Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
	uint32_t err_code;
    
	// Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
    
    err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, process_step_count);
    APP_ERROR_CHECK(err_code); 
	
    err_code = app_timer_create(&m_app_call_id, APP_TIMER_MODE_SINGLE_SHOT, callLow);
    APP_ERROR_CHECK(err_code); 
	
		err_code = app_timer_create(&m_app_clock_id, APP_TIMER_MODE_REPEATED, clockOps);
    APP_ERROR_CHECK(err_code); 
}

/**@brief Start timers.
*/
static void timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer. */
    uint32_t err_code;
    
    err_code = app_timer_start(m_app_timer_id, TIMER_50HZ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
	
		err_code = app_timer_start(m_app_clock_id, TIMER_1HZ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Stop timer.
 */
/*
static void timer_stop (void)
{
    uint32_t err_code;
    
    err_code = app_timer_stop(m_app_timer_id);
    APP_ERROR_CHECK(err_code); 
	
		err_code = app_timer_stop(m_app_clock_id);
    APP_ERROR_CHECK(err_code); 
    
}

*/

/***TIMERS***/

/**@brief GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile) 
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Initialize services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t err_code;
    static ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof nus_init);
    nus_init.data_handler = commandAPI_handler;
    
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Initialize security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief Connection Parameters Module handler.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Connection Parameters module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Initialize the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Application's BLE Stack event handler.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code = NRF_SUCCESS;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            nrf_gpio_pin_set(LED_OTHER);
						BLESessionActive = 1;
        
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            advertising_start();
				
						BLESessionActive = 0;
						sendRawData = 0;
            
						do_post_processing = true;
            nrf_gpio_pin_clear(LED_OTHER);
            
            break;
            
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;
            
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;
            
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
                //nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);

                // Go to system-off mode (this function will not return; wakeup will cause a reset)
                GPIO_WAKEUP_BUTTON_CONFIG(WAKEUP_BUTTON_PIN);
                
                err_code = sd_power_system_off();    
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Dispatches a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    on_ble_evt(p_ble_evt);
}


/**@brief BLE stack initialization.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    // YOUR_JOB: If the MTU size is changed by the application, the MTU_SIZE parameter to
    //           BLE_STACK_HANDLER_INIT() must be changed accordingly.
    BLE_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                           BLE_L2CAP_MTU_DEF,
                           ble_evt_dispatch,
                           false);
    
    /*
    // Enable BLE stack 
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
    */
    
}


/**@brief Power manager.
 */
/*
static void power_manage(void)
{
    uint32_t err_code = sd_app_event_wait();
    APP_ERROR_CHECK(err_code);
}

*/

void post_comm_processing(void) {
	

	nrf_gpio_pin_set(LED_NO_FALL);

	advertising_stop();
	
  if (SaveSnapshot == 1) 
	{
		
		writeSnapshotToFlash(); 
		SaveSnapshot = 0;    
	}
	
	if (ClearSnapshots == 1)
	{
			
//			eraseAllSnapshots();
			ClearSnapshots = 0;
	}
	
	if (ClearDataBuffer == 1)
	{
			
//			eraseAllStepData();					
			ClearDataBuffer = 0;
	}
	
	if (writeLog == 1) 
	{
//		writeLogEntry(activityToSave);
		writeLog = 0;
		activityToSave = 0;
	}
	
	if (clearPerso == 1)
	{
		revert_to_defaults();
		clearPerso = 0;
	}
	
	if (rewritePerso == 1)
	{
		writeTempToFlashPerso();
		rewritePerso = 0;
	}
/*
	if (writeUsername == 1)
	{
		setUsername(tempUsername);
		//error_flash = ble_flash_block_write((uint32_t *)livePerso->uname, (uint32_t*)tempUsername,UNAME_MAXLEN);
//		error_flash = ble_flash_block_write(USERNAME_ADDRESS, (uint32_t*)tempUsername,5);
//		error_flash = ble_flash_word_write(USERNAME_LENGTH, (uint32_t)tempUsernameLength);
		writeUsername = 0;
	}
	*/
/*
	if (writeXYZ == 1)
	{
		
		error_flash = ble_flash_block_write(XYZ_ADDRESS, (uint32_t*)tempXYZ,3);
		error_flash = ble_flash_word_write(XYZ_LENGTH, (uint32_t)tempXYZLength);
		writeXYZ = 0;
	}

	
	if (writeSV == 1)
	{
		error_flash = ble_flash_block_write(SV_ADDRESS, (uint32_t*)tempSV,3);
		error_flash = ble_flash_word_write(SV_LENGTH, (uint32_t)tempSVLength);
		writeSV = 0;
	}
	
*/	
	
	 advertising_start();
	nrf_gpio_pin_clear(LED_NO_FALL);
	
}

/**@brief Application main function.
					For this project to work, SoftDevice should be pre-loaded.
 */


int main(void)
{
    bool init_status;
    int retValue;
	int i = defaultPerso.fall_detection_window;

    // initialize LEDs
    nrf_gpio_cfg_output(LED_RESETTING);
    nrf_gpio_cfg_output(LED_NO_FALL);
    nrf_gpio_cfg_output(LED_FALL_DETECTED);
		nrf_gpio_cfg_output(LED_OTHER);
		
		welcomeLEDs();

    // Initialize
    timers_init();
    ble_stack_init();
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    sec_params_init();
    twi_master_init();
		
#ifdef TEST_ACTIVITY_LOG		
		testActivityLog();
#endif
		
		initActivityLog();
		initSnapshotBuffer();
		initCoefficients();

    // initialize values
    retValue = 0;

    init_status = mpu6050_init(MPU6050_DEVICE_ADDR);
    if ( false == init_status )
    {
        init_status = mpu6050_init(MPU6050_DEVICE_ADDR+1);
        if ( false == init_status )
        {   
            retValue = -1;
						errorLEDs();
					sd_nvic_SystemReset();
        }
    }
    if ( 0 == retValue )
    {    
      init_status = false;

			
			i = 0;
        while ( false == init_status )
        {
          if (i == 6) 
					{
						errorLEDs();
						sd_nvic_SystemReset();
					}
					
					init_status = accel_setup();
					i++;
					
				}
    }
		
		
    if (0 == retValue) {
			 
			char outbuf[20];
			
			// Start execution - write a log entry with zero steps to indicate startup
				
//				writeLogEntry(999);
        advertising_start();
				setup_wdt();
        timers_start(); // start sampliing
        
        while(1)
        {
					if (ReadSnapshots == 1)
					{

						FullSnapshotEntry* entry = (FullSnapshotEntry*)(SNAPSHOT_DATA_ADDRESS_START) ;
						
						SnapshotHeader *h;
						Sensor_Reading *r;

            sprintf (outbuf, "ID: %s                                   ", getPerso()->uname);    
						ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
                

                for(int i=0;i < getActivityLogSize(); i++)
                {
                  h = &entry->hdr;
									
									sprintf (outbuf, ": SNAPSHOT # %d  \n                           ",(i));
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 16);
									
									sprintf (outbuf, "Version: 2 \n");//
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 12);
									
									sprintf (outbuf, "t:%d n:%d last:%d",h->data.time, h->data.num_of_data_points, h->data.latest_data_point_index);
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
//									ptr++;
								
									r = entry->r;
										
									for (int j = 0; j < NUMBER_OF_ENTRIES; j++)
									{
										#define FLASH_READ_DELAY 40
										nrf_gpio_pin_set(LED_RESETTING); 
		
										sprintf (outbuf, "i: %d T: %04x           ",j,r[j].val.temp);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);

										sprintf (outbuf, "A: x%04xy%04xz%04x",r[j].val.x_ac,r[j].val.y_ac,r[j].val.z_ac);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);
								
										sprintf (outbuf, "G: x%04xy%04xz%04x",r[j].val.x_gy,r[j].val.y_gy,r[j].val.z_gy);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);
										
										nrf_gpio_pin_clear(LED_RESETTING);
										
										nrf_delay_ms(FLASH_READ_DELAY);
										
										//ptr += sizeof(Sensor_Reading);
										

									}
									entry++;
                    
									
                    
                }  	
						
						
						ReadSnapshots = 0;
					}

					if (ReadSnapshotsPartial == 1)
					{
						
						PartialSnapTotal = ((PartialSnap1-('0'))*10)+(PartialSnap2-('0'));
						
						FullSnapshotEntry* entry = (FullSnapshotEntry*)(SNAPSHOT_DATA_ADDRESS_START+(((PartialSnapTotal*sizeof(FullSnapshotEntry)*10))/4)) ;
						
						SnapshotHeader *h;
						Sensor_Reading *r;

            sprintf (outbuf, "ID: %s                                   ", getPerso()->uname);    
						ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
                

                for(int i=0;i < 10; i++)
                {
                  h = &entry->hdr;
									
									sprintf (outbuf, ": SNAPSHOT # %d  \n                           ",(i+(PartialSnapTotal*10)));
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 16);
									
									sprintf (outbuf, "Version: 2 \n");//
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 12);
									
									sprintf (outbuf, "t:%d n:%d last:%d",h->data.time, h->data.num_of_data_points, h->data.latest_data_point_index);
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
//									ptr++;
								
									r = entry->r;
										
									for (int j = 0; j < NUMBER_OF_ENTRIES; j++)
									{
										#define FLASH_READ_DELAY 40
										nrf_gpio_pin_set(LED_RESETTING); 
		
										sprintf (outbuf, "i: %d T: %04x           ",j,r[j].val.temp);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);

										sprintf (outbuf, "A: x%04xy%04xz%04x",r[j].val.x_ac,r[j].val.y_ac,r[j].val.z_ac);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);
								
										sprintf (outbuf, "G: x%04xy%04xz%04x",r[j].val.x_gy,r[j].val.y_gy,r[j].val.z_gy);
										ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
										
										nrf_delay_ms(FLASH_READ_DELAY);
										
										nrf_gpio_pin_clear(LED_RESETTING);
										
										nrf_delay_ms(FLASH_READ_DELAY);
										
										//ptr += sizeof(Sensor_Reading);
										

									}
									entry++;
                    
									
                    
                }  	
						
						
						ReadSnapshotsPartial = 0;
					}
					
					if (ReadDataBuffer == 1)
					{
						LogEntry *addr;
 
			
						sprintf (outbuf, "ID: %s                                   ", getPerso()->uname);
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
							
						addr = (LogEntry *)(STEP_DATA_ADDRESS_START);
							for(int i=0;i < getActivityLogSize(); i++)
							{
								nrf_gpio_pin_set(LED_RESETTING); 
								nrf_delay_ms(20);
								nrf_gpio_pin_clear(LED_RESETTING);
								nrf_delay_ms(20);  
								
									
					//			sprintf ( outbuf, ": h:%d s:%d a:%04f                                   ",addr->item.hour, addr->item.sec, addr->item.activity_level);
								sprintf ( outbuf, "%d:%d - %03f                                   ",addr->item.hour, (addr->item.sec)/60, addr->item.activity_level);
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
									//nrf_delay_ms(1);
								addr++;
									
							}  
							ReadDataBuffer = 0;
					}

					if (ReadDataBufferPartial == 1)
					{
						LogEntry *addr;
 
			
						sprintf (outbuf, "ID: %s                                   ", getPerso()->uname);
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
						
						PartialCountTotal = ((PartialCount1-('0'))*10)+(PartialCount2-('0'));							
						
						addr = (LogEntry *)(STEP_DATA_ADDRESS_START+(2*(PartialCountTotal*1000)));

							for(int i=0;i < 1000; i++)
							{
								nrf_gpio_pin_set(LED_RESETTING); 
								nrf_delay_ms(20);
								nrf_gpio_pin_clear(LED_RESETTING);
								nrf_delay_ms(20);  
								
									
					//			sprintf ( outbuf, ": h:%d s:%d a:%04f                                   ",addr->item.hour, addr->item.sec, addr->item.activity_level);
								sprintf ( outbuf, "%d:%d - %03f                                   ",addr->item.hour, (addr->item.sec)/60, addr->item.activity_level);
									ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
									//nrf_delay_ms(1);
								addr++;
									
							}  
							ReadDataBufferPartial = 0;
					}
				 
					if (Dumper == 1)
					{
						
						sprintf (outbuf, "Log: %d                                   ",getActivityLogSize());
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);   

							Dumper = 0;
					}
					
					if (ReadBigSnapshot == 1)
					{	
						SnapshotHeader *h = getSnapshotHeader();
						
						sprintf (outbuf, "t:%d n:%d last:%d",h->data.time, h->data.num_of_data_points, h->data.latest_data_point_index);
						ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
						
						for (i = 0; i < NUMBER_OF_ENTRIES; i++) //i <= h->data.latest_data_point_index; i++)
						{      
							#define RAM_READ_DELAY 15
							
							sprintf (outbuf, "i: %d T: %04x           ",i,raw_data[i].val.temp);
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
							nrf_delay_ms(RAM_READ_DELAY);

							sprintf (outbuf, "A: x%04xy%04xz%04x",raw_data[i].val.x_ac,raw_data[i].val.y_ac,raw_data[i].val.z_ac);
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
							nrf_delay_ms(RAM_READ_DELAY);

							sprintf (outbuf, "G: x%04xy%04xz%04x",raw_data[i].val.x_gy,raw_data[i].val.y_gy,raw_data[i].val.z_gy);
							ble_nus_send_string(&m_nus, (unsigned char*)outbuf, 20);
							nrf_delay_ms(RAM_READ_DELAY);
							
							
						}
						
						ReadBigSnapshot = 0;
						
					}
												
					if (do_post_processing == true)
					{
						do_post_processing = false;
						
						if (emergencyCall == 1) 
							{
								makeTheCall();
								emergencyCall = 0;
							}
						
						post_comm_processing();
						
					}
				
        // Power Method 1
				//		nrf_gpio_pin_write(LED_OTHER,1);
           // power_manage();
				//		nrf_gpio_pin_write(LED_OTHER,0);
        }
    }
    return retValue;

		
}
