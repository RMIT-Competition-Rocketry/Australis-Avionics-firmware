/***********************************************************************************
 * @file        main.c                                                             *
 * @author      Matt Ricci                                                         *
 * @brief       Main application entry point and system initialization.            *
 ***********************************************************************************/

#include "main.h"

#include "devices.h"
#include "tasklist.h"
#include "rcc.h"

#include "tasks.h"

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

 * Initializes microcontroller peripherals, creates the system initialization task,
 * and starts the FreeRTOS scheduler.
 *
 * @return int  Exit status (should never return)
 * =============================================================================== */

int main(void) {

  #ifdef TRACE
    xTraceInitialize();
  #endif

  // Create and start the system initialization task
  TaskHandle_t xSystemInitHandle;
  xTaskCreate(vSystemInit, "SystemInit", 16192, NULL, configMAX_PRIORITIES, &xSystemInitHandle);
  vTaskStartScheduler();

  // The scheduler should never return
  return 0;
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

  // TODO: Replace usage of event groups for xTaskEnableGroup and xMsgReadyGroup
  //       with direct-to-task notifications, for better efficiency and clarity as
  //       these flags aren't shared across multiple tasks.

  // Initialise event groups for task synchronization and message signaling
  xTaskEnableGroup   = xEventGroupCreate(); // 0: FLASH,  1: HIGHRES, 2: LOWRES, 3: LORA, 7: IDLE
  xSystemStatusGroup = xEventGroupCreate();
  xMsgReadyGroup     = xEventGroupCreate();
  xEventGroupSetBits(xMsgReadyGroup, GROUP_MESSAGE_READY_LORA);

  // Initialise USB buffers and mutex
  xUsbTxBuff = xMessageBufferCreate(USB_TX_SIZE);
  xUsbRxBuff = xStreamBufferCreate(USB_RX_SIZE, 1);
  xGpsRxBuff = xStreamBufferCreate(GPS_RX_SIZE, 1);

  xUsbMutex  = xSemaphoreCreateMutex();

  // Initialise LoRa buffer
  xLoRaTxBuff = xMessageBufferCreate(LORA_BUFF_SIZE);

  /* -------------------------- Device Initialization ---------------------------- */

  // TODO: Extract RCC initialisation to hardware specific target files in Target/
  //       subdirectories. These would be specified under an initRCC() function
  //       defined in the target specific source, and called by main() here.

  // Make sure all peripherals we will use are enabled
  RCC_START_PERIPHERAL(AHB1, GPIOA);
  RCC_START_PERIPHERAL(AHB1, GPIOB);
  RCC_START_PERIPHERAL(AHB1, GPIOC);
  RCC_START_PERIPHERAL(AHB1, GPIOD);
  RCC_START_PERIPHERAL(AHB1, GPIOE);
  RCC_START_PERIPHERAL(AHB1, GPIOF);
  RCC_START_PERIPHERAL(APB2, SPI1);
  RCC_START_PERIPHERAL(APB1, SPI3);
  RCC_START_PERIPHERAL(APB2, SPI4);
  RCC_START_PERIPHERAL(APB1, TIM6);
  RCC_START_PERIPHERAL(APB1, USART3);
  RCC_START_PERIPHERAL(APB2, USART6);
  RCC_START_PERIPHERAL(APB2, SYSCFG); // DON'T FORGET TO ENABLE THIS ONE LOL

  // TODO: As with the RCC, extract interrupt configuration to hardware specific
  //       target files in Target/ subdirectories. As some of the IRQ handlers will
  //       need to remain defined in RTOS application source files, the exact names
  //       of the defined functions may be formatted via preprocessor macro.

  // Enable peripheral and external interrupts
  configure_interrupts();

  // Start up drivers
  initDevices();

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

  // TODO: Get rid of this shit vvv

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

  // TODO: Replace task handle struct with static array of handles, i.e.
  //       TaskHandle_t handles[SIZE]

  // TODO: Extract task initialisation to hardware specific target files in Target/
  //       subdirectories. These would be specified under an initTasks() function
  //       defined in the target specific source, and called by main() here.

  xTaskCreate(vHDataAcquisition, "HDataAcq", 512, &_mem, configMAX_PRIORITIES - 2, TaskList_new());
  xTaskCreate(vLDataAcquisition, "LDataAcq", 512, &_mem, configMAX_PRIORITIES - 3, TaskList_new());
  xTaskCreate(vStateUpdate, "StateUpdate", 512, NULL, configMAX_PRIORITIES - 4, TaskList_new());
  xTaskCreate(vFlashBuffer, "FlashData", 512, &_mem, configMAX_PRIORITIES - 1, TaskList_new());
  xTaskCreate(vUsbTransmit, "UsbTx", 256, NULL, configMAX_PRIORITIES - 6, TaskList_new());
  xTaskCreate(vUsbReceive, "UsbRx", 256, &shell, configMAX_PRIORITIES - 6, TaskList_new());
  xTaskCreate(vIdle, "Idle", 256, &_mem, tskIDLE_PRIORITY, TaskList_new());

  initTasks();

  // TODO: Temporarily disabled due to bug related to use of message buffer.
  //       See gpsacquisition.c todo for more detail.

  // xTaskCreate(vGpsTransmit, "GpsRead", 512, NULL, configMAX_PRIORITIES - 6, &handles.xGpsTransmitHandle);

  xTaskResumeAll();

  #ifdef TRACE
    xTraceEnable(TRC_START);
  #endif

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
  EXTI->RTSR        |= 0x02;
  EXTI->IMR         |= 0x02;
  SYSCFG->EXTICR[0] &= ~0xF0;
  SYSCFG->EXTICR[0]  = 0x30;
  __enable_irq();
}
