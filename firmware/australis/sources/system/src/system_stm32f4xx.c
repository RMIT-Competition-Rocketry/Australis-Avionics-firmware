/**
 ******************************************************************************
 * @file    system_stm32f4xx.c
 * @author  MCD Application Team
 * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
 *
 *   This file provides two functions and one global variable to be called from
 *   user application:
 *      - SystemInit(): This function is called at startup just after reset and
 *                      before branch to main program. This call is made inside
 *                      the "startup_stm32f4xx.s" file.
 *
 *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
 *                                  by the user application to setup the SysTick
 *                                  timer or configure other parameters.
 *
 *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
 *                                 be called whenever the core clock is changed
 *                                 during program execution.
 *
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/** @addtogroup CMSIS
 * @{
 */

/** @addtogroup stm32f4xx_system
 * @{
 */

/** @addtogroup STM32F4xx_System_Private_Includes
 * @{
 */

#include "stm32f4xx.h"
#include "config.h"

// Function prototypes
void SystemCoreClockUpdate(void); // Update the variables relating to the system clock.
static void SetSysClock(void);    // Set the system clock.

#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    ((uint32_t)12000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

short AHBPresc[16] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512};
char APBPresc[8]   = {1, 1, 1, 1, 2, 4, 8, 16};

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Defines
 * @{
 */

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM or SDRAM as data memory  */
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)\
 || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx)
/* #define DATA_IN_ExtSRAM */
#endif /* STM32F40xxx || STM32F41xxx || STM32F42xxx || STM32F43xxx || STM32F469xx || STM32F479xx ||\
          STM32F412Zx || STM32F412Vx */
 
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
/* #define DATA_IN_ExtSDRAM */
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F446xx || STM32F469xx ||\
          STM32F479xx */

/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in Flash or Sram, else the vector table is kept at the automatic
     remap of boot address selected */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */
/* #define VECT_TAB_SRAM */
#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM_BASE       /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_SRAM */
#endif                               /* USER_VECT_TAB_ADDRESS */
               /******************************************************************************/

               /**
                * @}
                */

               /** @addtogroup STM32F4xx_System_Private_Macros
                * @{
                */

               /**
                * @}
                */

               /** @addtogroup STM32F4xx_System_Private_Variables
                * @{
                */
               /* This variable is updated in three ways:
                   1) by calling CMSIS function SystemCoreClockUpdate()
                   2) by calling HAL API function HAL_RCC_GetHCLKFreq()
                   3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
                      Note: If you use this function to configure the system clock; then there
                            is no need to call the 2 first functions listed above, since SystemCoreClock
                            variable is updated automatically.
               */

               uint32_t SystemCoreClock = 16800000;
const uint8_t AHBPrescTable[16]         = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]          = {0, 0, 0, 0, 1, 2, 3, 4};
/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_FunctionPrototypes
 * @{
 */

#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

/**
 * @}
 */

/** @addtogroup STM32F4xx_System_Private_Functions
 * @{
 */

/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and External memory
 *         configuration.
 * @param  None
 * @retval None
 */
void SystemInit(void) {
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif

	#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
		SystemInit_ExtMemCtl(); 
	#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

                        /* Configure the Vector Table location -------------------------------------*/
	#if defined(USER_VECT_TAB_ADDRESS)
		SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
	#endif /* USER_VECT_TAB_ADDRESS */

                        /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register, HSI used by default */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  // Set the custom configuration for the system clock.
  // The HAL variables are updated at the end of this function.
  SetSysClock();
}

//******************************************************************************//
// Function: SetSysClock()
// Input : None
// Return : None
// Description : Configure the system clocks
// *****************************************************************************//
static void SetSysClock(void) {
  // By default the High Speed External Osciallator (HSE) is used as the
  // PLL clock source.
  // For the STM32F4 Discovery, the external crystal oscillator is 8MHz
  // For the Emcraft SOM, the external crystal oscillator is 12MHz
  volatile unsigned int StartUpCounter = 0;

  // volatile unsigned int tmp = 0x00;
	#if HSE_USED
		// Enable the High Speed Oscillator
		RCC->CR |= RCC_CR_HSEON;

		// Wait until the High Speed Oscillator is ready, or a timeout reached.
		
		while(((RCC->CR & RCC_CR_HSERDY) == 0) && StartUpCounter!= HSE_STARTUP_TIMEOUT)
		{
			StartUpCounter++;
		}

		// Check to see that the High Speed Oscillator started successfully.
		if((RCC->CR & RCC_CR_HSERDY) == RCC_CR_HSERDY)
		{
			// The oscillator started successfully

			// Enable the power control (PWR) clock
			RCC->APB1ENR |= RCC_APB1ENR_PWREN;

			/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
			PWR->CR |= PWR_CR_VOS;

			// Set the HCLK to the same speed as the system clock (AHB Bus (STM32F407 = 168MHz))
			RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1);			// This should clear the bits.

			// Set the PCLK2 to the HCLK divided by 2. (APB2 Bus (High Speed) (STM32F407 = 84MHz))
			RCC->CFGR |= (0x04 << RCC_CFGR_PPRE2_Pos);

			// Set the PCLK1 to the HCLK divided by 4. (APB1 Bus (Low Speed) (STM32F407 = 42MHz))
			RCC->CFGR |= (0x05 << RCC_CFGR_PPRE1_Pos);

			// Set the PLL configuration register. The PLL_VCO frequency is given by:
			// PLL_VCO = (HSE_VALUE / PLL_M) * PLL_N
			// SYS_CLK = PLL_VCO / PLL_P (168MHz)
			// USB OTG FS, SDIO and RNG Clock = PLL_VCO / PLLQ (48MHz)
			// For the STM32F407, this equates to a PLL_VCO of
			// For the Emcraft board, this equates to a PLL_VCO of 336MHz (Sysclock = 168MHz)

			// Configure the main PLL
			RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (1 << 22) | (PLL_Q << 24);

			// Now that the main configuration is complete, enable the PLL
			RCC->CR |= RCC_CR_PLLON;

			// Wait until the main PLL has started
			while((RCC->CR & RCC_CR_PLLRDY) == 0);

			// Configure Flash prefetch, Instruction cache, Data cache and wait state
			FLASH->ACR &= ~(FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | (FLASH_ACR_LATENCY));
			FLASH->ACR |= (FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | (0x05 << FLASH_ACR_LATENCY_Pos));

			// FLASH->ACR.b.prften = 1; 					// Flash prefetch enable
			// FLASH->ACR.b.icen = 1;						// Instruction cache enable
			// FLASH->ACR.b.dcen = 1;						// Data cache enable
			// FLASH->ACR.b.latency = 0x05;			// 5 wait states.

			// Select the main PLL as system clock source
			RCC->CFGR &= ~(RCC_CFGR_SW_0 | RCC_CFGR_SW_1);
			RCC->CFGR |= RCC_CFGR_SW_1;
			// RCC->CFGR.b.sw0 = 0x00;
			// RCC->CFGR.b.sw1 = 0x01;

			// Wait until the main PLL is used as the system clock source
			while((RCC->CR & RCC_CR_PLLON) != RCC_CR_PLLON);
		}
		else
		{
			// Oscillator failed to start.
		//	HSEStatus = 0x00;

			/* If HSE fails to start-up, the application will have wrong clock
			 configuration. User can add here some code to deal with this error */
		}

	#elif !HSE_USED
		// Adjust the frequency of the internal oscillator
		// First clear the adjustment values (HSICAL and HSITRIM), before setting them
		//RCC->CR &= ~( 0xF << RCC_CR_HSITRIM_Pos);
		//RCC->CR &= ~( 0xFF << RCC_CR_HSICAL_Pos);
		//RCC->CR |= ( (HSITRIM & 0xF) << RCC_CR_HSITRIM_Pos);
		//RCC->CR |= ( (HSICAL & 0xFF) << RCC_CR_HSICAL_Pos);

		if((RCC->CR & RCC_CR_HSIRDY) == RCC_CR_HSIRDY)
		{
			// The oscillator started successfully

			// Enable the power control (PWR) clock
			RCC->APB1ENR |= RCC_APB1ENR_PWREN;

			/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
			PWR->CR |= PWR_CR_VOS;

			// Set the HCLK to the same speed as the system clock (AHB Bus (STM32F407 = 168MHz))
			RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1);			// This should clear the bits.

			// Set the PCLK2 to the HCLK divided by 2. (APB2 Bus (High Speed) (STM32F407 = 84MHz))
			RCC->CFGR |= (0x04 << RCC_CFGR_PPRE2_Pos);

			// Set the PCLK1 to the HCLK divided by 4. (APB1 Bus (Low Speed) (STM32F407 = 42MHz))
			RCC->CFGR |= (0x05 << RCC_CFGR_PPRE1_Pos);

			// Set the PLL configuration register. The PLL_VCO frequency is given by:
			// PLL_VCO = (HSE_VALUE / PLL_M) * PLL_N
			// SYS_CLK = PLL_VCO / PLL_P (168MHz)
			// USB OTG FS, SDIO and RNG Clock = PLL_VCO / PLLQ (48MHz)
			// For the STM32F407, this equates to a PLL_VCO of
			// For the Emcraft board, this equates to a PLL_VCO of 336MHz (Sysclock = 168MHz)

			// Configure the main PLL
			RCC->PLLCFGR = HSIPLL_M | (HSIPLL_N << 6) | (((HSIPLL_P >> 1) -1) << 16) | (HSIPLL_Q << 24);

			// Now that the main configuration is complete, enable the PLL
			RCC->CR |= RCC_CR_PLLON;

			// Wait until the main PLL has started
			while((RCC->CR & RCC_CR_PLLRDY) == 0);

			// Configure Flash prefetch, Instruction cache, Data cache and wait state
			FLASH->ACR &= ~(FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | (FLASH_ACR_LATENCY));
			FLASH->ACR |= (FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | (0x05 << FLASH_ACR_LATENCY_Pos));

			// FLASH->ACR.b.prften = 1; 					// Flash prefetch enable
			// FLASH->ACR.b.icen = 1;						// Instruction cache enable
			// FLASH->ACR.b.dcen = 1;						// Data cache enable
			// FLASH->ACR.b.latency = 0x05;			// 5 wait states.

			// Select the main PLL as system clock source
			RCC->CFGR &= ~(RCC_CFGR_SW_0 | RCC_CFGR_SW_1);
			RCC->CFGR |= RCC_CFGR_SW_1;
			// RCC->CFGR.b.sw0 = 0x00;
			// RCC->CFGR.b.sw1 = 0x01;

			// Wait until the main PLL is used as the system clock source
			while((RCC->CR & RCC_CR_PLLON) != RCC_CR_PLLON);
		}
		{
			// HSI Oscillator failed to start!
		}
	#endif

	#if defined (RTC_ENABLE)
		volatile unsigned int BackupResetCounter = 0;

		// Enable access to the real time clock (use external low-speed oscillator).
		PWR->CR |= PWR_CR_DBP;

		// Reset the backup domain interface and delay.
		RCC->BDCR |= RCC_BDCR_BDRST;
		for(BackupResetCounter = 0; BackupResetCounter <= 5; BackupResetCounter++);
		RCC->BDCR &= ~(RCC_BDCR_BDRST);
		for(BackupResetCounter = 0; BackupResetCounter <= 5; BackupResetCounter++);

		// Clear out the individual bits for the backup domain clocks (including wait states)
		RCC->BDCR &= ~(RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL | RCC_BDCR_LSEON);

		// Now set the bits of interest.
		// Enable the real-time clock peripheral.
		// External LSE as the clock source.
		// Enable the external oscillator.
		RCC->BDCR |= RCC_BDCR_RTCEN | (0x01 << RCC_BDCR_RTCSEL_Pos) | RCC_BDCR_LSEON;

		// Wait for the LSE to become ready.
		while((RCC->BDCR & RCC_BDCR_LSERDY_Msk) != 0x02)

		// Disable access to the backup domain.
		PWR->CR &= ~(PWR_CR_DBP);
	#endif

  // Set the variables relating to the system clock speed.
  SystemCoreClockUpdate();
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 *
 * @note   Each time the core clock (HCLK) changes, this function must be called
 *         to update SystemCoreClock variable value. Otherwise, any configuration
 *         based on this variable will be incorrect.
 *
 * @note   - The system frequency computed by this function is not the real
 *           frequency in the chip. It is calculated based on the predefined
 *           constant and the selected clock source:
 *
 *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *
 *         (*) HSI_VALUE is a constant defined in stm32f4xx_hal_conf.h file (default value
 *             16 MHz) but the real value may vary depending on the variations
 *             in voltage and temperature.
 *
 *         (**) HSE_VALUE is a constant defined in stm32f4xx_hal_conf.h file (its value
 *              depends on the application requirements), user has to ensure that HSE_VALUE
 *              is same as the real frequency of the crystal used. Otherwise, this function
 *              may have wrong result.
 *
 *         - The result of this function could be not correct when using fractional
 *           value for HSE crystal.
 *
 * @param  None
 * @retval None
 */
void SystemCoreClockUpdate(void) {
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp) {
  case 0x00: /* HSI used as system clock source */
    SystemCoreClock = HSI_VALUE;
    break;
  case 0x04: /* HSE used as system clock source */
    SystemCoreClock = HSE_VALUE;
    break;
  case 0x08: /* PLL used as system clock source */

    /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
       SYSCLK = PLL_VCO / PLL_P
       */
    pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
    pllm      = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

    if (pllsource != 0) {
      /* HSE used as PLL clock source */
      pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    } else {
      /* HSI used as PLL clock source */
      pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    }

    pllp            = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> 16) + 1) * 2;
    SystemCoreClock = pllvco / pllp;
    break;
  default:
    SystemCoreClock = HSI_VALUE;
    break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}

#if defined (DATA_IN_ExtSRAM) && defined (DATA_IN_ExtSDRAM)
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx)
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32f4xx.s before jump to main.
  *         This function configures the external memories (SRAM/SDRAM)
  *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmp = 0x00;

  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

  /* Enable GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface clock */
  RCC->AHB1ENR |= 0x000001F8;

  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x00CCC0CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAA0A8A;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0FCF;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x00000000;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA828A;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x00000000;
  
  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0xCCCCCCCC;
  GPIOF->AFR[1]  = 0xCCCCCCCC;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA800AAA;
  /* Configure PFx pins speed to 50 MHz */ 
  GPIOF->OSPEEDR = 0xAA800AAA;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x00000000;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0xCCCCCCCC;
  GPIOG->AFR[1]  = 0xCCCCCCCC;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0xAAAAAAAA;
  /* Configure PGx pins speed to 50 MHz */ 
  GPIOG->OSPEEDR = 0xAAAAAAAA;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x00000000;
  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0x00C0CC00;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAA08A0;
  /* Configure PHx pins speed to 50 MHz */ 
  GPIOH->OSPEEDR = 0xAAAA08A0;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */ 
  GPIOH->PUPDR   = 0x00000000;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0x0028AAAA;
  /* Configure PIx pins speed to 50 MHz */ 
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */ 
  GPIOI->PUPDR   = 0x00000000;
  
/*-- FMC Configuration -------------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR |= 0x00000001;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  FMC_Bank5_6->SDCR[0] = 0x000019E4;
  FMC_Bank5_6->SDTR[0] = 0x01115351;      
  
  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000011; 
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }

  /* Delay */
  for (index = 0; index<1000; index++);
  
  /* PALL command */
  FMC_Bank5_6->SDCMR = 0x00000012;           
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
  
  /* Auto refresh command */
  FMC_Bank5_6->SDCMR = 0x00000073;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  }
 
  /* MRD register program */
  FMC_Bank5_6->SDCMR = 0x00046014;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020; 
  } 
  
  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000027C<<1));
  
  /* Disable write protection */
  tmpreg = FMC_Bank5_6->SDCR[0]; 
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[2]  = 0x00001011;
  FMC_Bank1->BTCR[3]  = 0x00000201;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#if defined(STM32F469xx) || defined(STM32F479xx)
  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[2]  = 0x00001091;
  FMC_Bank1->BTCR[3]  = 0x00110212;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif /* STM32F469xx || STM32F479xx */

  (void)(tmp);
}
#endif                /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F469xx || STM32F479xx */
#elif defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32f4xx.s before jump to main.
 *         This function configures the external memories (SRAM/SDRAM)
 *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void) {
  __IO uint32_t tmp = 0x00;
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
#if defined (DATA_IN_ExtSDRAM)
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

#if defined(STM32F446xx)
  /* Enable GPIOA, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG interface
      clock */
  RCC->AHB1ENR |= 0x0000007D;
#else
  /* Enable GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface 
      clock */
  RCC->AHB1ENR |= 0x000001F8;
#endif /* STM32F446xx */
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

#if defined(STM32F446xx)
  /* Connect PAx pins to FMC Alternate function */
  GPIOA->AFR[0]  |= 0xC0000000;
  GPIOA->AFR[1]  |= 0x00000000;
  /* Configure PDx pins in Alternate function mode */
  GPIOA->MODER   |= 0x00008000;
  /* Configure PDx pins speed to 50 MHz */
  GPIOA->OSPEEDR |= 0x00008000;
  /* Configure PDx pins Output type to push-pull */
  GPIOA->OTYPER  |= 0x00000000;
  /* No pull-up, pull-down for PDx pins */
  GPIOA->PUPDR   |= 0x00000000;

  /* Connect PCx pins to FMC Alternate function */
  GPIOC->AFR[0]  |= 0x00CC0000;
  GPIOC->AFR[1]  |= 0x00000000;
  /* Configure PDx pins in Alternate function mode */
  GPIOC->MODER   |= 0x00000A00;
  /* Configure PDx pins speed to 50 MHz */
  GPIOC->OSPEEDR |= 0x00000A00;
  /* Configure PDx pins Output type to push-pull */
  GPIOC->OTYPER  |= 0x00000000;
  /* No pull-up, pull-down for PDx pins */
  GPIOC->PUPDR   |= 0x00000000;
#endif /* STM32F446xx */

  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0] = 0x000000CC;
  GPIOD->AFR[1] = 0xCC000CCC;
  /* Configure PDx pins in Alternate function mode */
  GPIOD->MODER = 0xA02A000A;
  /* Configure PDx pins speed to 50 MHz */
  GPIOD->OSPEEDR = 0xA02A000A;
  /* Configure PDx pins Output type to push-pull */
  GPIOD->OTYPER = 0x00000000;
  /* No pull-up, pull-down for PDx pins */
  GPIOD->PUPDR = 0x00000000;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0] = 0xC00000CC;
  GPIOE->AFR[1] = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */
  GPIOE->MODER = 0xAAAA800A;
  /* Configure PEx pins speed to 50 MHz */
  GPIOE->OSPEEDR = 0xAAAA800A;
  /* Configure PEx pins Output type to push-pull */
  GPIOE->OTYPER = 0x00000000;
  /* No pull-up, pull-down for PEx pins */
  GPIOE->PUPDR = 0x00000000;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0] = 0xCCCCCCCC;
  GPIOF->AFR[1] = 0xCCCCCCCC;
  /* Configure PFx pins in Alternate function mode */
  GPIOF->MODER = 0xAA800AAA;
  /* Configure PFx pins speed to 50 MHz */
  GPIOF->OSPEEDR = 0xAA800AAA;
  /* Configure PFx pins Output type to push-pull */
  GPIOF->OTYPER = 0x00000000;
  /* No pull-up, pull-down for PFx pins */
  GPIOF->PUPDR = 0x00000000;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0] = 0xCCCCCCCC;
  GPIOG->AFR[1] = 0xCCCCCCCC;
  /* Configure PGx pins in Alternate function mode */
  GPIOG->MODER = 0xAAAAAAAA;
  /* Configure PGx pins speed to 50 MHz */
  GPIOG->OSPEEDR = 0xAAAAAAAA;
  /* Configure PGx pins Output type to push-pull */
  GPIOG->OTYPER = 0x00000000;
  /* No pull-up, pull-down for PGx pins */
  GPIOG->PUPDR = 0x00000000;

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx)  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0x00C0CC00;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAA08A0;
  /* Configure PHx pins speed to 50 MHz */ 
  GPIOH->OSPEEDR = 0xAAAA08A0;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */ 
  GPIOH->PUPDR   = 0x00000000;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0x0028AAAA;
  /* Configure PIx pins speed to 50 MHz */ 
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */ 
  GPIOI->PUPDR   = 0x00000000;
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F469xx || STM32F479xx */

                        /*-- FMC Configuration -------------------------------------------------------*/
  /* Enable the FMC interface clock */
  RCC->AHB3ENR |= 0x00000001;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);

  /* Configure and enable SDRAM bank1 */
#if defined(STM32F446xx)
  FMC_Bank5_6->SDCR[0] = 0x00001954;
#else  
  FMC_Bank5_6->SDCR[0] = 0x000019E4;
#endif /* STM32F446xx */
  FMC_Bank5_6->SDTR[0] = 0x01115351;

  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000011;
  tmpreg             = FMC_Bank5_6->SDSR & 0x00000020;
  while ((tmpreg != 0) && (timeout-- > 0)) {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  /* Delay */
  for (index = 0; index < 1000; index++);

  /* PALL command */
  FMC_Bank5_6->SDCMR = 0x00000012;
  tmpreg             = FMC_Bank5_6->SDSR & 0x00000020;
  timeout            = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0)) {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  /* Auto refresh command */
#if defined(STM32F446xx)
  FMC_Bank5_6->SDCMR = 0x000000F3;
#else  
  FMC_Bank5_6->SDCMR = 0x00000073;
#endif /* STM32F446xx */
  tmpreg  = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0)) {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  /* MRD register program */
#if defined(STM32F446xx)
  FMC_Bank5_6->SDCMR = 0x00044014;
#else  
  FMC_Bank5_6->SDCMR = 0x00046014;
#endif /* STM32F446xx */
  tmpreg  = FMC_Bank5_6->SDSR & 0x00000020;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0)) {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
#if defined(STM32F446xx)
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000050C<<1));
#else    
  FMC_Bank5_6->SDRTR = (tmpreg | (0x0000027C<<1));
#endif /* STM32F446xx */

  /* Disable write protection */
  tmpreg               = FMC_Bank5_6->SDCR[0];
  FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);
#endif                /* DATA_IN_ExtSDRAM */
#endif                /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx || STM32F446xx || STM32F469xx || STM32F479xx */

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)\
 || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx)

#if defined(DATA_IN_ExtSRAM)
/*-- GPIOs Configuration -----------------------------------------------------*/
   /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHB1ENR   |= 0x00000078;
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x00CCC0CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAA0A8A;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0FCF;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */ 
  GPIOD->PUPDR   = 0x00000000;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAA828A;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */ 
  GPIOE->PUPDR   = 0x00000000;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAA000AAA;
  /* Configure PFx pins speed to 100 MHz */ 
  GPIOF->OSPEEDR = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */ 
  GPIOF->PUPDR   = 0x00000000;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0x000000C0;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0x00085AAA;
  /* Configure PGx pins speed to 100 MHz */ 
  GPIOG->OSPEEDR = 0x000CAFFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */ 
  GPIOG->PUPDR   = 0x00000000;
  
/*-- FMC/FSMC Configuration --------------------------------------------------*/
  /* Enable the FMC/FSMC interface clock */
  RCC->AHB3ENR         |= 0x00000001;

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);
  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[2]  = 0x00001011;
  FMC_Bank1->BTCR[3]  = 0x00000201;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#if defined(STM32F469xx) || defined(STM32F479xx)
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);
  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[2]  = 0x00001091;
  FMC_Bank1->BTCR[3]  = 0x00110212;
  FMC_Bank1E->BWTR[2] = 0x0fffffff;
#endif /* STM32F469xx || STM32F479xx */
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx)|| defined(STM32F417xx)\
   || defined(STM32F412Zx) || defined(STM32F412Vx)
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FSMCEN);
  /* Configure and enable Bank1_SRAM2 */
  FSMC_Bank1->BTCR[2]  = 0x00001011;
  FSMC_Bank1->BTCR[3]  = 0x00000201;
  FSMC_Bank1E->BWTR[2] = 0x0FFFFFFF;
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx */

#endif                /* DATA_IN_ExtSRAM */
#endif                /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || \
/* clang-format on */                                                                               \
                      // TEMP
  STM32F429xx || STM32F439xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Vx * / (void)(tmp);
}
#endif                /* DATA_IN_ExtSRAM && DATA_IN_ExtSDRAM */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
