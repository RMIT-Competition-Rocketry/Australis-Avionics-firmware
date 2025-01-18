/***********************************************************************************
 * @file        main.c                                                             *
 * @author      Matt Ricci                                                         *
 * @brief       Main application entry point and system initialization.            *
 ***********************************************************************************/

#include "main.h"

long hDummyIdx = 0;
long lDummyIdx = 0;

// RTOS event groups
EventGroupHandle_t xTaskEnableGroup;   // 0: FLASH,  1: HIGHRES, 2: LOWRES, 3: LORA, 7: IDLE
EventGroupHandle_t xSystemStatusGroup; // 0-2: Flight state, 3: Payload, 4: Aerobrakes
EventGroupHandle_t xMsgReadyGroup;     // 0: LORA, 1: USB

// RTOS message buffers
MessageBufferHandle_t xLoRaTxBuff;
MessageBufferHandle_t xUsbTxBuff;

StreamBufferHandle_t xUsbRxBuff;
StreamBufferHandle_t xGpsRxBuff;

// RTOS mutexes
SemaphoreHandle_t xUsbMutex;

/* =============================================================================== */
/**
 * @brief Main application entry point.
 *
 * Initializes microcontroller peripherals, creates the system initialization task,
 * and starts the FreeRTOS scheduler.
 *
 * @return int  Exit status (should never return)
 * =============================================================================== */

int main(void) {

#ifdef TRACE
  xTraceInitialize();
#endif

  // Initialise clock sources and peripheral busses
  configure_RCC_APB1();
  configure_RCC_APB2();
  configure_RCC_AHB1();

  // Initialize GPIO pins for peripherals
  configure_MISC_GPIO();
  configure_UART3_GPS();
  configure_SPI1_Sensor_Suite();
  configure_SPI3_LoRa();
  configure_SPI4_Flash();
  configure_interrupts();

  // Initialise timers
  TIM6init();
  TIM7init();

  // Configure CAN
  CANGPIO_config();
  CAN_Peripheral_config();

#ifdef FLIGHT_TEST
  GPIOB->ODR ^= 0x8000;
  GPIOD->ODR ^= 0x8000;
#endif

  // Send AB ground test message over CAN
  unsigned int CANHigh = 0;
  unsigned int CANLow  = 0;
  unsigned int id      = 0x603;
  CAN_TX(CAN_AB, 8, CANHigh, CANLow, id);

  // Create and start the system initialization task
  TaskHandle_t xSystemInitHandle;
  xTaskCreate(vSystemInit, "SystemInit", 16192, NULL, configMAX_PRIORITIES, &xSystemInitHandle);
  vTaskStartScheduler();

  // The scheduler should never return
  return 0;
}

/* =============================================================================== */
/**
 * @brief Initialisation task for device drivers
 *
 * Performs the initialization of system device drivers and adds their handles to the
 * device vector.
 **
 * =============================================================================== */

void vDeviceInit() {
  /* ----------------------------- Flash Initialization -------------------------- */

  // Initialise SPI flash driver
  static Flash flash;
  static DeviceHandle_t flashHandle __attribute__((section(".device_flash"), unused));
  flashHandle = Flash_init(
      &flash, "Flash", FLASH_PORT, FLASH_CS, FLASH_PAGE_SIZE, FLASH_PAGE_COUNT
  );

  /* -------------------------- Communication Initialization ---------------------- */

  // Initialise USB UART driver
  static UART usb;
  static DeviceHandle_t usbHandle __attribute__((section(".device_usb"), unused));
  usbHandle = UART_init(
      &usb, "USB", USB_INTERFACE, USB_PORT, USB_PINS, USB_BAUD, OVER8
  );

  // Initialise LoRa driver
  static LoRa lora;
  static DeviceHandle_t loraHandle __attribute__((section(".device_lora"), unused));
  loraHandle = LoRa_init(
      &lora, "LoRa", LORA_PORT, LORA_CS, BW500, SF9, CR5
  );

  /* ------------------------------ Sensor Initialization ------------------------- */

  /* ACCELEROMETER */

  // Initialise low g accelerometer driver and device handle
  static KX134_1211 lAccel;
  static DeviceHandle_t lAccelHandle __attribute__((section(".device_lAccel"), unused));
  lAccelHandle = KX134_1211_init(
      &lAccel, "LAccel", ACCEL_PORT_1, ACCEL_CS_1, ACCEL_SCALE_LOW, ACCEL_AXES_1, ACCEL_SIGN_1
  );

  // Initialise high g accelerometer driver and device handle
  static KX134_1211 hAccel;
  static DeviceHandle_t hAccelHandle __attribute__((section(".device_hAccel"), unused));
  hAccelHandle = KX134_1211_init(
      &hAccel, "HAccel", ACCEL_PORT_2, ACCEL_CS_2, ACCEL_SCALE_HIGH, ACCEL_AXES_2, ACCEL_SIGN_2
  );

  // Initialise current accelerometer device handle
  static DeviceHandle_t accelHandle __attribute__((section(".device_Accel"), unused));
  memcpy(accelHandle.name, "Accel", DEVICE_NAME_LENGTH);
  accelHandle.device = &lAccel;

  /*  GYROSCOPE */

  // Initialise gyroscope driver and device handle
  static A3G4250D gyro;
  static DeviceHandle_t gyroHandle __attribute__((section(".device_gyro"), unused));
  gyroHandle = A3G4250D_init(
      &gyro, "Gyro", GYRO_PORT, GYRO_CS, A3G4250D_SENSITIVITY, GYRO_AXES, GYRO_SIGN
  );

  /* BAROMETER */

  // Initialise barometer driver and device handle
  static BMP581 baro;
  static DeviceHandle_t baroHandle __attribute__((section(".device_baro"), unused));
  baroHandle = BMP581_init(
      &baro, "Baro", BARO_PORT, BARO_CS, BMP581_TEMP_SENSITIVITY, BMP581_PRESS_SENSITIVITY
  );

  /* GPS */

  // Initialise GPS driver and device handle
  static GPS gps;
  static DeviceHandle_t gpsHandle __attribute__((section(".device_gps"), unused));
  gpsHandle = GPS_init(
      &gps, "GPS", GPS_INTERFACE, GPS_PORT, GPS_PINS, GPS_BAUD
  );
}

/* =============================================================================== */
/**
 * @brief Initialisation of RTOS tasks
 *
 * Performs initial setup for various peripherals, ensuring all components
 * are ready for data acquisition and system state management. This task also
 * initializes RTOS event groups and message buffers to manage inter-task
 * communication.
 *
 * @todo Refactor context parameters that require write operations to intialise as
 *       pointers within their respective structs.
 *
 * @return void
 * =============================================================================== */

void vSystemInit(void *argument) {

  // Allow time for external devices to finish startup sequences
  vTaskDelay(pdMS_TO_TICKS(500));

  vTaskSuspendAll();

  // Initialise event groups for task synchronization and message signaling
  xTaskEnableGroup   = xEventGroupCreate(); // 0: FLASH,  1: HIGHRES, 2: LOWRES, 3: LORA, 7: IDLE
  xMsgReadyGroup     = xEventGroupCreate();
  xSystemStatusGroup = xEventGroupCreate();
  xEventGroupSetBits(xSystemStatusGroup, GROUP_SYSTEM_STATUS_PAYLOAD | GROUP_SYSTEM_STATUS_AEROBRAKES);

  xEventGroupSetBits(xMsgReadyGroup, GROUP_MESSAGE_READY_LORA);

  // Initialise USB buffers and mutex
  xUsbTxBuff = xMessageBufferCreate(USB_TX_SIZE);
  xUsbRxBuff = xStreamBufferCreate(USB_RX_SIZE, 1);
  xGpsRxBuff = xStreamBufferCreate(GPS_RX_SIZE, 1);

  xUsbMutex  = xSemaphoreCreateMutex();

  // Initialise LoRa buffer
  xLoRaTxBuff = xMessageBufferCreate(LORA_BUFF_SIZE);

  /* -------------------------- Device Initialization ---------------------------- */

  vDeviceInit();

  // Initialise circular memory buffer
  MemBuff _mem;
  static StateHandle_t __attribute__((section(".state_mem"), unused)) mem;
  mem.state = &_mem;
  memcpy(mem.name, "Memory", STATE_NAME_LENGTH);

  uint8_t buff[MEM_BUFF_SIZE];
  MemBuff_init(&_mem, buff, MEM_BUFF_SIZE, FLASH_PAGE_SIZE);

  // Initialise shell
  static Shell shell;
  Shell_init(&shell);

  /* --------------------------- State Initialization -----------------------------*/

  // Tilt state variable
  static StateHandle_t __attribute__((section(".state_tilt"), unused)) tilt;
  static float _tilt = 0.0f;
  tilt.state         = &_tilt;
  memcpy(tilt.name, "Tilt", STATE_NAME_LENGTH);

  // Cosine state variable
  static StateHandle_t __attribute__((section(".state_cosine"), unused)) cosine;
  static float _cosine = 0.0f;
  cosine.state         = &_cosine;
  memcpy(cosine.name, "Cosine", STATE_NAME_LENGTH);

  // Altitude state variable
  static StateHandle_t __attribute__((section(".state_altitude"), unused)) altitude;
  static float _altitude = 0.0f;
  altitude.state         = &_altitude;
  memcpy(altitude.name, "Altitude", STATE_NAME_LENGTH);

  // Velocity state variable
  static StateHandle_t __attribute__((section(".state_velocity"), unused)) velocity;
  static float _velocity = 0.0f;
  velocity.state         = &_velocity;
  memcpy(velocity.name, "Velocity", STATE_NAME_LENGTH);

  // Flight state variable
  static StateHandle_t __attribute__((section(".state_flightState"), unused)) flightState;
  static enum State _flightState = PRELAUNCH;
  flightState.state              = &_flightState;
  memcpy(flightState.name, "FlightState", STATE_NAME_LENGTH);

  // Rotation quaternion state variable
  static StateHandle_t __attribute__((section(".state_qRot"), unused)) qRot;
  static Quaternion _qRot;
  Quaternion_init(&_qRot);
  qRot.state = &_qRot;
  memcpy(qRot.name, "RotationQuaternion", STATE_NAME_LENGTH);

  // Attitude vector state variable
  static StateHandle_t __attribute__((section(".state_vAttitude"), unused)) vAttitude;
  static float _vAttitude[3] = {0, 0, 1};
  vAttitude.state            = _vAttitude;
  memcpy(vAttitude.name, "AttitudeVector", STATE_NAME_LENGTH);

  // Attitude vector state variable
  static StateHandle_t __attribute__((section(".state_vLaunch"), unused)) vLaunch;
  static float _vLaunch[3] = {0, 0, 1};
  vLaunch.state            = _vLaunch;
  memcpy(vLaunch.name, "LaunchVector", STATE_NAME_LENGTH);

  // Sliding window average velocity
  static StateHandle_t __attribute__((section(".state_avgVel"), unused)) avgVel;
  static SlidingWindow _avgVel = {};
  avgVel.state                 = &_avgVel;
  memcpy(avgVel.name, "AvgVelBuffer", STATE_NAME_LENGTH);

  // Sliding window average velocity
  static StateHandle_t __attribute__((section(".state_avgPress"), unused)) avgPress;
  static SlidingWindow _avgPress = {};
  avgPress.state                 = &_avgPress;
  memcpy(avgPress.name, "AvgPressBuffer", STATE_NAME_LENGTH);

  // Initialize pressure sliding window average
  float avgPressBuff[AVG_BUFF_SIZE];
  SlidingWindow_init(avgPress.state, avgPressBuff, AVG_BUFF_SIZE);

  // Initialize velocity sliding window average
  float avgVelBuff[AVG_BUFF_SIZE];
  SlidingWindow_init(avgVel.state, avgVelBuff, AVG_BUFF_SIZE);

  /**********************************************************************************
   *                                    TASK INIT                                   *
   **********************************************************************************/

  static Handles handles;

  /** @todo refactor task names and associated file names */
  xTaskCreate(vHDataAcquisition, "HDataAcq", 512, &_mem, configMAX_PRIORITIES - 2, &handles.xHDataAcquisitionHandle);
  xTaskCreate(vLDataAcquisition, "LDataAcq", 512, &_mem, configMAX_PRIORITIES - 3, &handles.xLDataAcquisitionHandle);
  xTaskCreate(vStateUpdate, "StateUpdate", 512, &handles, configMAX_PRIORITIES - 4, &handles.xStateUpdateHandle);
  xTaskCreate(vFlashBuffer, "FlashData", 512, &_mem, configMAX_PRIORITIES - 1, &handles.xFlashBufferHandle);
  xTaskCreate(vLoRaSample, "LoRaSample", 256, NULL, configMAX_PRIORITIES - 6, &handles.xLoRaSampleHandle);
  xTaskCreate(vLoRaTransmit, "LoRaTx", 256, NULL, configMAX_PRIORITIES - 5, &handles.xLoRaTransmitHandle);
  xTaskCreate(vUsbTransmit, "UsbTx", 256, NULL, configMAX_PRIORITIES - 6, &handles.xUsbTransmitHandle);
  xTaskCreate(vUsbReceive, "UsbRx", 256, &shell, configMAX_PRIORITIES - 6, &handles.xUsbReceiveHandle);
  xTaskCreate(vIdle, "Idle", 256, &_mem, tskIDLE_PRIORITY, &handles.xIdleHandle);
  xTaskCreate(vPayloadTransmit, "PayloadTx", 512, NULL, configMAX_PRIORITIES - 6, &handles.xPayloadTransmitHandle);
  xTaskCreate(vGpsTransmit, "GpsRead", 512, NULL, configMAX_PRIORITIES - 6, &handles.xGpsTransmitHandle);

  buzzer(3215);
  xTaskResumeAll();

#ifdef TRACE
  xTraceEnable(TRC_START);
#endif

  // Suspend the system initialization task (only needs to run once)
  vTaskSuspend(NULL);
}

/**
 * @todo Refactor and document
 */
void configure_interrupts() {
  __disable_irq();
  NVIC_SetPriority(EXTI1_IRQn, 9);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(USART6_IRQn, 10);
  NVIC_EnableIRQ(USART6_IRQn);
  NVIC_SetPriority(USART3_IRQn, 10);
  NVIC_EnableIRQ(USART3_IRQn);
  EXTI->RTSR |= (0x02 | 0x04);
  EXTI->IMR |= (0x02 | 0x04);
  SYSCFG->EXTICR[0] &= (~(0XF0));
  SYSCFG->EXTICR[0] = 0x230;
  __enable_irq();
}

// Unsure of actual fix for linker error
// temporary (lol) solution
void _init() {}
