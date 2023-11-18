#include "SwitchModeHandle.h"


#include "UART/uart_analysis_parameters.h"
#include "UART/uart_config.h"

#include "lwip/sockets.h"
#include "TCPChannelController/TCPChannelController.h"
#include <esp_log.h>

TcpParam tcp_param;
enum WorkMode working_mode = NONE_MODE;
extern UartInitT c1;
extern UartInitT c2;
extern bool c1UartConfigFlag;
extern bool c2UartConfigFlag;
extern const char kHeartRet[5]; // 心跳包发送
extern uint8_t uart1_flag;
extern uint8_t uart2_flag;
TcpTaskHandleT *tcphand1;
TcpTaskHandleT *tcphand2;
extern const int kTcpHandleFatherTaskCurrent;
extern TcpTaskHandleT TCP_TASK_HANDLE[10];
//TcpTaskHandleT *tcphand1;
//TcpTaskHandleT *tcphand2;
QueueHandle_t uart_queue1 = NULL;
QueueHandle_t uart_queue = NULL;
QueueHandle_t uart_queue2 = NULL;
QueueHandle_t uart_queue3 = NULL;
static const char *TAG = "ModeSwitcher";
void UartHandle(void) {
    ESP_LOGI(TAG, "Enter Uart Mode");
    working_mode = UART;
}
void ADCHandle(void) {}
void DACHandle(void) {}
void PwmCollectHandle(void) {}
void PwmSimulationHandle(void) {}
void I2CHandle(void) {}
void SpiHandle(void) {}
void CanHandle(void) {}

void UartTask(int ksock) {
    int written = 0;

    static QueueHandle_t tcp1_to_uart1_queue_ = NULL;
    static QueueHandle_t uart1_to_tcp1_queue_ = NULL;
    static QueueHandle_t uart2_to_tcp2_queue_ = NULL;
    static QueueHandle_t tcp2_to_uart2_queue_ = NULL;
    uart1_to_tcp1_queue_ = xQueueCreate(10, sizeof(events));
    tcp1_to_uart1_queue_ = xQueueCreate(50, sizeof(events));
    uart2_to_tcp2_queue_ = xQueueCreate(10, sizeof(events));
    tcp2_to_uart2_queue_ = xQueueCreate(50, sizeof(events));

    c1.UartTcpQueue.uart_to_tcp_queue_ = &uart1_to_tcp1_queue_;
    c1.UartTcpQueue.tcp_to_uart_queue_ = &tcp1_to_uart1_queue_;
    c2.UartTcpQueue.tcp_to_uart_queue_ = &tcp2_to_uart2_queue_;
    c2.UartTcpQueue.uart_to_tcp_queue_ = &uart2_to_tcp2_queue_;
    printf("c2 uart_to_tcp_queue_:%p\n",(c2.UartTcpQueue.uart_to_tcp_queue_));
    printf("c2 tcp_to_uart_queue_:%p\n",(c2.UartTcpQueue.tcp_to_uart_queue_));

    printf("uart1_to_tcp1_queue_ rx: %p  uart1_tx_tcp_queue tx: %p\n", &uart1_to_tcp1_queue_, &tcp1_to_uart1_queue_);

    // xTaskCreatePinnedToCore(UartRev, "uartr", 5120, (void *)&c1, 10, &xHandle, 0);
    if (c1UartConfigFlag == true)
    {

        if (uart1_flag == 1)
        {//串口1已被占用，重新配置
//            UartSetup(&c1);
            TcpTaskAllDelete(TCP_TASK_HANDLE);
             static TcpParam tp0 =
                {
                    .UartTcpQueue.uart_to_tcp_queue_ = &uart1_to_tcp1_queue_,
                    .UartTcpQueue.tcp_to_uart_queue_ = &tcp1_to_uart1_queue_,
                    .mode = TCP_ALL,
                    .port = CH2,
                };
            tcphand1 = TcpTaskCreate(&tp0);
        }
        else if(uart1_flag == 0)
        {
            CreateUartTask(&c1);
            static TcpParam tp0 =
                {
                    .UartTcpQueue.uart_to_tcp_queue_ = &uart1_to_tcp1_queue_,
                    .UartTcpQueue.tcp_to_uart_queue_ = &tcp1_to_uart1_queue_,
                    .mode = TCP_ALL,
                    .port = CH2,
                };

            tcphand1 = TcpTaskCreate(&tp0);
        }
        c1UartConfigFlag = false;//0
    }

//    if (c2UartConfigFlag == true ) {
//        if (uart2_flag == 1){
//            TcpTaskAllDelete(TCP_TASK_HANDLE);
//            static TcpParam tp2 =
//                    {
//                            .uart_to_tcp_queue_ = &uart2_to_tcp2_queue_,
//                            .tcp_to_uart_queue_ = &tcp2_to_uart2_queue_,
//                            .mode = TCP_ALL,
//                            .port = CH3,
//                    };
//            tcphand2 = TcpTaskCreate(&tp2);
//        }
//        else{
//            CreateUartTask(&c2);
//            static TcpParam tp2 =
//                    {
//                            .uart_to_tcp_queue_ = &uart2_to_tcp2_queue_,
//                            .tcp_to_uart_queue_ = &tcp2_to_uart2_queue_,
//                            .mode = TCP_ALL,
//                            .port = CH3,
//                    };
//            tcphand2 = TcpTaskCreate(&tp2);
//        }
//        c2UartConfigFlag = false;
//    }
//    else if (c2UartConfigFlag == true && uart1_flag == 1) {
//        CreateUartTask(&c2);
//           static TcpParam tp2 =
//            {
//                .uart_to_tcp_queue_ = &uart2_to_tcp2_queue_,
//                .tcp_to_uart_queue_ = &tcp2_to_uart2_queue_,
//                .mode = TCP_ALL,
//                .port = CH3,
//            };
//        tcphand2 = TcpTaskCreate(&tp2);
//        c2UartConfigFlag = false;
//    }

//     do{
//         written=send(ksock, kHeartRet, 5, 0);
//         printf("%d\n",written);
//     }while(written<=0);
//     printf("create uart \n");
     //xTaskCreatePinnedToCore(UartSend, "uartt", 5120, (void *)&c2, 10, &xHandle, 1);
     printf("create uart \n");
    do {
        written = send(ksock, "OK!\r\n", 5, 0);
    } while (written <= 0);
}