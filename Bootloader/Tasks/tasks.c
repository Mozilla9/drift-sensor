/******************************************************************************/
/* tasks.c                                                                    */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "Protothread\pt.h"
#include "Led\led.h"
#include "Updater\updater.h"
#include "Bootloader\bootloader.h"
#include "Tasks\tasks.h"





#define UPDATE_TASK           1



static uint32_t flag_run_enabled = 0;
static uint32_t enabled_tasks = 0x000000;
static uint16_t upd_state = 0;

static struct pt first_pt;
static struct pt upd_pt;
static struct pt led_pt;



/*
 * First task
 *
 */
static int first_task(struct pt * pt) {
    static uint32_t ticks = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, !(get_sys_tick() % 4) && ticks != get_sys_tick());
        ticks = get_sys_tick();
        flag_run_enabled = 1;
    }

    PT_END(pt);
}


/*
 * Updating task
 *
 */
static int update_task(struct pt * pt) {
    static uint32_t time_count = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {


            switch (upd_state) {
                //--------------------------------------------------------------
                case 0:      // initial state

                    upd_state++;

                    break;

                //--------------------------------------------------------------
                case 1:
                    {

                    }
                    break;

                //--------------------------------------------------------------
                case 2:
                    {

                    }
                    break;

                //--------------------------------------------------------------
                default:
                    break;
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Led task
 *
 */
static int led_task(struct pt * pt) {
    static uint32_t time_count = 0;
    PT_BEGIN(pt);

    while(1) {
        PT_WAIT_UNTIL(pt, flag_run_enabled);
        {

            switch (get_led1_state()) {
                //--------------------------------------------------------------
                case 0:      // off state
                    {
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            time_count = tick + TIMER0_TICK / 50;
                            led1_on();
                        }
                    }
                    break;

                //--------------------------------------------------------------
                case 1:      // on state
                    {
                        uint32_t tick = get_sys_tick();
                        if (tick > time_count) {
                            time_count = tick + TIMER0_TICK;
                            led1_off();
                        }
                    }
                    break;

                //--------------------------------------------------------------
                default:
                    led1_off();
                    break;
            }
        }
        PT_WAIT_UNTIL(pt, !flag_run_enabled);
    }

    PT_END(pt);
}


/*
 * Test update task
 *
 */
static bool_t test_update_task() {
    return BIT_TEST(enabled_tasks, UPDATE_TASK) ? TRUE_T : FALSE_T;
}


/*
 * Enable update task
 *
 */
void enable_update_task() {
    BIT_SET(enabled_tasks, UPDATE_TASK);
}


/*
 * Disable acc task
 *
 */
void disable_update_task() {
    BIT_CLEAR(enabled_tasks, UPDATE_TASK);
}


/*
 * Init tasks
 *
 */
void init_tasks() {
    PT_INIT(&first_pt);
    PT_INIT(&led_pt);
    PT_INIT(&upd_pt);


    DEBUG_PRINTF("\r\nTasks: 0x%8x\r\n", enabled_tasks);
}


/*
 * Get enabled tasks
 *
 */
uint32_t get_en_tasks() {
    return enabled_tasks;
}


/*
 * Run tasks
 *
 */
void run_tasks() {
    first_task(&first_pt);
    led_task(&led_pt);

    if (test_update_task()) {
        update_task(&upd_pt);
    }

    flag_run_enabled = 0;
}











/*
// App: Сброс девайса

void MTASK::Reset() {
    switch(queue.subTask)
    {
        //----------------------------------------------------------------------
        case eSUBTASK1:   // Сбрсываем девайс сторожем
          CORE::ResetDevice();
          EndTask();
          break;

        //----------------------------------------------------------------------
        default:
          queue.subTask = eSUBTASK1;
          queue.phaseTask = ePERFORM_TASK;
          break;
    };
}





// App: Разлочивание чипа

void MTASK::Unlock() {
    switch(queue.subTask)
    {
        //----------------------------------------------------------------------
        case eSUBTASK1:   // Подготовка
          // Запускаем сабаку
          SetTaskTimerOnce(eTASK_WATCHDOG, 60UL);

          queue.subTask = eSUBTASK2;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK2:   // Стираем основное FW
          IAP::EraseFwFlash();
          queue.subTask = eSUBTASK3;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK3:   // Стираем BL
          IAP::EraseBlFlash();
          queue.subTask = eSUBTASK4;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK4:   // Конец
          // Выключаем сабаку
          SetTaskTimerOnce(eTASK_WATCHDOG, TASK_FREE_TIMEOUT);

          EndTask();
          break;

        //----------------------------------------------------------------------
        default:
          queue.subTask = eSUBTASK1;
          queue.phaseTask = ePERFORM_TASK;
          break;
    };
}





// App: Загрузка нового Firmware

void MTASK::Download() {
    switch(queue.subTask)
    {
        //----------------------------------------------------------------------
        case eSUBTASK1:   // Инициализация
          SetTaskTimerOnce(eTASK_WATCHDOG, 300UL);
          queue.subTask = eSUBTASK2;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK2:   // Отслеживаем таймаут
          queue.subTask = (DEVICE::TestFlgDownloading() == TRUE_T) ? eSUBTASK3 : eSUBTASK4;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK3:   // Проверяем данные и записываем
          queue.subTask = (DOWNLOADER::WriteDataToFlash() == TRUE_T) ? eSUBTASK4 : eSUBTASK2;
          DOWNLOADER::TestAck();
          break;

        //----------------------------------------------------------------------
        case eSUBTASK4:   // Проверка CRC
          queue.subTask = eSUBTASK6;
          if(DOWNLOADER::ValidateDownloading() == TRUE_T) {
              queue.subTask = eSUBTASK5;
              SetDelay(2UL);
              AddSubTaskInQueue(eTASK_DELAY);
          };
          DEVICE::ClearFlgDownloading();
          break;

        //----------------------------------------------------------------------
        case eSUBTASK5:   // Перезагрузка
          queue.subTask = eSUBTASK6;
          AddSubTaskInQueue(eTASK_RESET);
          break;

        //----------------------------------------------------------------------
        case eSUBTASK6:   // Конец
          SetTaskTimerOnce(eTASK_WATCHDOG, TASK_FREE_TIMEOUT);

          EndTask();
          break;

        //----------------------------------------------------------------------
        default:
          queue.subTask = eSUBTASK1;
          queue.phaseTask = ePERFORM_TASK;
          break;
    };
}





// App: Обновление Firmware

void MTASK::Update() {
    switch(queue.subTask)
    {
        //----------------------------------------------------------------------
        case eSUBTASK1:   // Инициализация
          // Запускаем сабаку
          SetTaskTimerOnce(eTASK_WATCHDOG, 60UL);

          // Запускаем апдейтер
          DEVICE::SetFlgUpdating();
          UPDATER::ResetUpdater();

          queue.subTask = eSUBTASK2;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK2:   // Стираем предварительно флэш контроллера
          queue.subTask = (UPDATER::EraseFwFlash() == TRUE_T) ? eSUBTASK3 : eSUBTASK10;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK3:   // Считываем данные из внешней флэш
          UPDATER::ReadDataExtFlash();
          queue.subTask = eSUBTASK4;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK4:   // Расшифровка данных
          queue.subTask = eSUBTASK5;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK5:   // Запись данных
          queue.subTask = (UPDATER::WriteDataFw() == TRUE_T) ? eSUBTASK6 : eSUBTASK10;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK6:   // Проверка флага
          queue.subTask = (UPDATER::TestFlgUpdateEnd() == TRUE_T) ? eSUBTASK7 : eSUBTASK3;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK7:   // Запись информации о Fw в флэш контроллера
          queue.subTask = (UPDATER::EndFwUpdate() == TRUE_T) ? eSUBTASK8 : eSUBTASK10;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK8:   // Стираем флаги во внешней флэш
          UPDATER::EraseFlgNeedUpdate();
          queue.subTask = eSUBTASK9;
          break;

        //----------------------------------------------------------------------
        case eSUBTASK9:   // Перезагрузка
          queue.subTask = eSUBTASK10;
          CORE::ResetDevice();
          break;

        //----------------------------------------------------------------------
        case eSUBTASK10:   // Конец
          // Выключаем сабаку
          SetTaskTimerOnce(eTASK_WATCHDOG, TASK_FREE_TIMEOUT);

          // Сбрасываем флаг
          DEVICE::ClearFlgUpdating();

          EndTask();
          break;

        //----------------------------------------------------------------------
        default:
          queue.subTask = eSUBTASK1;
          queue.phaseTask = ePERFORM_TASK;
          break;
    };
}*/


