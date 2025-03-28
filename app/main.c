/*
 * Данный пример демонстрирует работу с GPIO и PAD_CONFIG.
 * В примере настраивается вывод, который подключенный к светодиоду, в режим
 * GPIO.
 */

 #include "gpio.h"
 #include "mik32_memory_map.h"
 #include "pad_config.h"
 #include "power_manager.h"
 #include "scr1_timer.h"
 #include "wakeup.h"
 
 #define LED_PIN_NUM (0)
 #define LED_PIN_NUM_2 (8)
 #define BTM_PIN_NUM (10)

 #define LED_PIN_PORT GPIO_0
 #define LED_PIN_PORT_2 GPIO_0
 #define BTM_PIN_PORT GPIO_0


 #define SCR1_TIMER_GET_TIME()                                                  \
   (((uint64_t)(SCR1_TIMER->MTIMEH) << 32) | (SCR1_TIMER->MTIME))
 #define SYSTEM_FREQ_HZ 32000000UL


 
 void SystemClock_Config();
 void GPIO_Init();
 void delay(uint32_t ms);
 
  void mode_0() {
    LED_PIN_PORT->OUTPUT |= (1 << LED_PIN_NUM);
    LED_PIN_PORT_2->OUTPUT |= (1 << LED_PIN_NUM_2);
    delay(200);
  }

  void mode_1() {
    LED_PIN_PORT->OUTPUT = (0 << LED_PIN_NUM);
    LED_PIN_PORT_2->OUTPUT = (1 << LED_PIN_NUM_2);
    delay(100);
    LED_PIN_PORT_2->OUTPUT = (0 << LED_PIN_NUM_2);
    delay(100);
  }

  void mode_2() {
    LED_PIN_PORT_2->OUTPUT = (0 << LED_PIN_NUM_2);
    LED_PIN_PORT->OUTPUT = (1 << LED_PIN_NUM);
    delay(100);
    LED_PIN_PORT->OUTPUT = (0 << LED_PIN_NUM);
    delay(100);
  }

  void mode_3() {
    LED_PIN_PORT_2->OUTPUT = (0 << LED_PIN_NUM_2);
    LED_PIN_PORT->OUTPUT = (1 << LED_PIN_NUM);
    delay(150);
    LED_PIN_PORT->OUTPUT = (0 << LED_PIN_NUM);
    LED_PIN_PORT_2->OUTPUT = (1 << LED_PIN_NUM_2);
    delay(150);
  }

  void mode_4() {
    LED_PIN_PORT_2->OUTPUT = (0 << LED_PIN_NUM_2);
    LED_PIN_PORT->OUTPUT = (0 << LED_PIN_NUM);
    delay(200);
  }

int main() {
  SystemClock_Config();
  GPIO_Init();

  int mode = 0;
  while (1) {
    if ((BTM_PIN_PORT->STATE) & (1 << BTM_PIN_NUM))
    {
      ++mode;
      mode %= 5;
    }
    switch (mode)
    {
    case 0: {
      mode_0();
      break;
    }
    case 1: {
      mode_1();
      break;
    }
    case 2: {
      mode_2();
      break;
    }
    case 3: {
      mode_3();
      break;
    }
    case 4: {
      mode_4();
      break;
    }
    default:
      break;
    }
  }
}
 
void GPIO_Init() {
  PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M;
  // первая функция (порт общего назначения);
  PAD_CONFIG->PORT_0_CFG |= 0 << (LED_PIN_NUM * 2);
  // нагрузочная способность 2 мА;
  PAD_CONFIG->PORT_0_DS |= 0 << (LED_PIN_NUM * 2);
  // резисторы подтяжки отключены
  PAD_CONFIG->PORT_0_PUPD |= 0 << (LED_PIN_NUM * 2);
  LED_PIN_PORT->DIRECTION_OUT = 1 << LED_PIN_NUM;

  PAD_CONFIG->PORT_0_CFG |= 0 << (LED_PIN_NUM_2 * 2);
  PAD_CONFIG->PORT_0_DS |= 0 << (LED_PIN_NUM_2 * 2);
  PAD_CONFIG->PORT_0_PUPD |= 0 << (LED_PIN_NUM_2 * 2);
  LED_PIN_PORT_2->DIRECTION_OUT = 1 << LED_PIN_NUM_2;


  PAD_CONFIG->PORT_0_CFG |= 0 << (BTM_PIN_NUM * 2);
  PAD_CONFIG->PORT_0_DS |= 0 << (BTM_PIN_NUM * 2);
  PAD_CONFIG->PORT_0_PUPD |= 0 << (BTM_PIN_NUM * 2);
  BTM_PIN_PORT->DIRECTION_IN = 1 << BTM_PIN_NUM;
}
 
 void delay(uint32_t ms) {
   uint64_t end_mtimer = SCR1_TIMER_GET_TIME() + ms * (SYSTEM_FREQ_HZ / 1000);
   while (SCR1_TIMER_GET_TIME() < end_mtimer)
     ;
 }

void SystemClock_Config(void) {
  WU->CLOCKS_SYS &=
      ~(0b11 << WU_CLOCKS_SYS_OSC32M_EN_S); // Включить OSC32M и HSI32M
  WU->CLOCKS_BU &=
      ~(0b11 << WU_CLOCKS_BU_OSC32K_EN_S); // Включить OSC32K и LSI32K

  // Поправочный коэффициент HSI32M
  WU->CLOCKS_SYS = (WU->CLOCKS_SYS & (~WU_CLOCKS_SYS_ADJ_HSI32M_M)) |
                   WU_CLOCKS_SYS_ADJ_HSI32M(128);
  // Поправочный коэффициент LSI32K
  WU->CLOCKS_BU = (WU->CLOCKS_BU & (~WU_CLOCKS_BU_ADJ_LSI32K_M)) |
                  WU_CLOCKS_BU_ADJ_LSI32K(8);

  // Автоматический выбор источника опорного тактирования
  WU->CLOCKS_SYS &= ~WU_CLOCKS_SYS_FORCE_32K_CLK_M;

  // ожидание готовности
  while (!(PM->FREQ_STATUS & PM_FREQ_STATUS_OSC32M_M))
    ;

  // переключение на тактирование от OSC32M
  PM->AHB_CLK_MUX = PM_AHB_CLK_MUX_OSC32M_M | PM_AHB_FORCE_MUX_UNFIXED;
  PM->DIV_AHB = 0;   // Задать делитель шины AHB.
  PM->DIV_APB_M = 0; // Задать делитель шины APB_M.
  PM->DIV_APB_P = 0; // Задать делитель шины APB_P.
}
 