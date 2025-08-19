// #include "lvgl_port.h"
// #include "usart.h"
// #include "stdio.h"
// #include "delay.h"
// lv_indev_t *indev_touchpad; // 改为静态或全局变量
// static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
// {
//     uint32_t width    = area->x2 - area->x1 + 1;
//     uint32_t height   = area->y2 - area->y1 + 1;
//     uint32_t px_count = width * height;

//     lv_draw_sw_rgb565_swap(px_map, px_count);
//     ST7789_Fill_Rect(area->x1, area->y1, area->x2, area->y2, px_map);
//     lv_display_flush_ready(disp);
// }

// static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
// {
//     static int32_t last_x = 0;
//     static int32_t last_y = 0;
//     uint16_t x, y;
//     uint8_t gesture, points;
//     CST816_Get_Touch(&x, &y, &gesture, &points);
//     printf("touch x:%d y:%d gesture:%d points:%d\r\n", x, y, gesture, points);
//     if (points > 0) {
//         last_x      = x;
//         last_y      = y;
//         data->state = LV_INDEV_STATE_PRESSED;
//     } else {
//         data->state = LV_INDEV_STATE_RELEASED;
//     }

//     /*Set the last pressed coordinates*/
//     data->point.x = last_x;
//     data->point.y = last_y;
// }

// void LVGL_Port_Init(void)
// {
//     // 初始化 LVGL
//     lv_init();
//     Delay_Init();
//     Soft_I2C_Init(); // 软件 I2C
//     Soft_SPI_Init(); // 软件 SPI
//     ST7789_Init();   // 显示屏
//     CST816_Init();   // 触摸屏
//     // 配置显示驱动
//     lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
//     lv_display_set_flush_cb(disp, disp_flush);

//     // 设置显示缓冲区
//     static lv_color_t buf[SCREEN_WIDTH * 10];
//     lv_display_set_buffers(disp, buf, NULL, SCREEN_WIDTH * 10, LV_DISPLAY_RENDER_MODE_PARTIAL);

//     // 配置触摸输入驱动
//     indev_touchpad = lv_indev_create();
//     lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
//     lv_indev_set_read_cb(indev_touchpad, touchpad_read);

//     // printf("LVGL Port Initialized\r\n");
// }
