#include <stdint.h>
#include "lvgl.h"
#include "../../startup/multiboot2.h"


#define MY_DISP_HOR_RES  (1920)
#define MY_DISP_VOR_RES  (1080)

#define put_px(x, y, v) ((*((uint32_t *)(p_dispdev->common.framebuffer_addr + \
                         p_dispdev->common.framebuffer_pitch * y + x * 3))) = (v))

struct multiboot_tag_framebuffer *p_dispdev;


static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);


void x64_lvgl_init (void)
{
    static uint32_t buf1[MY_DISP_HOR_RES * MY_DISP_VOR_RES * 4];
//    static uint32_t buf2[MY_DISP_HOR_RES * 10];
    static lv_disp_t * disp;

    lv_init();
    disp = lv_disp_create(p_dispdev->common.framebuffer_width,
                          p_dispdev->common.framebuffer_height);
    lv_disp_set_draw_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISP_RENDER_MODE_PARTIAL);
    lv_disp_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_disp_set_flush_cb(disp, flush_cb);
}


static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    int32_t y;
    uint8_t * fb_tmp = (uint8_t*)p_dispdev->common.framebuffer_addr;
    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    uint32_t px_map_stride = lv_area_get_width(area) * px_size;
    int32_t fb_stride = lv_display_get_horizontal_resolution(disp) * px_size;
    fb_tmp += area->y1 * fb_stride;
    fb_tmp += area->x1 * px_size;
    for(y = area->y1; y <= area->y2; y++) {
        lv_memcpy(fb_tmp, px_map, px_map_stride);
        px_map += px_map_stride;
        fb_tmp += fb_stride;
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_display_flush_ready(disp);
}

//void my_flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * px_map)
//{
//    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
//     *`put_px` is just an example, it needs to be implemented by you.*/
//    int32_t x, y;
//    uint32_t *px = (uint32_t *)px_map;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            put_px(x, y, *px);
//            px++;
//        }
//    }
//
//    /* IMPORTANT!!!
//     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
//    lv_disp_flush_ready(disp);
//}


void lvgl_tick_and_handle (void)
{
  lv_tick_inc(1);
  lv_timer_handler_run_in_period(1);
}


//
//void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
//{
//    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
//     *`put_px` is just an example, it needs to implemented by you.*/
//    int32_t x, y;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            put_px(x, y, *color_p);
//            color_p++;
//        }
//    }
//
//    /* IMPORTANT!!!
//     * Inform the graphics library that you are ready with the flushing*/
//    lv_disp_flush_ready(disp_drv);
//}
//
//void my_gpu_fill_cb(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, const lv_area_t * dest_area, const lv_area_t * fill_area, lv_color_t color);
//{
//    /*It's an example code which should be done by your GPU*/
//    uint32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y < fill_area->y2; y++) {
//        for(x = fill_area->x1; x < fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}
//
//
//void my_rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area)
//{
//  /* Update the areas as needed.
//   * For example it makes the area to start only on 8th rows and have Nx8 pixel height.*/
//   area->y1 = area->y1 & 0x07;
//   area->y2 = (area->y2 & 0x07) + 8;
//}
//
//void my_set_px_cb(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
//{
//   /* Write to the buffer as required for the display.
//    * For example it writes only 1-bit for monochrome displays mapped vertically.*/
//   buf += buf_w * (y >> 3) + x;
//   if(lv_color_brightness(color) > 128) (*buf) |= (1 << (y % 8));
//   else (*buf) &= ~(1 << (y % 8));
//}
//
//void my_monitor_cb(lv_disp_drv_t * disp_drv, uint32_t time, uint32_t px)
//{
//  //printf("%d px refreshed in %d ms\n", time, ms);
//}
//
