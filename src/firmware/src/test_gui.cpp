/**
 * 
 */
#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

#include "trace.h"

TRACE_TAG(test_gui);

#define TFT_LED      32
LV_IMG_DECLARE(logo);

// Locals:
static TFT_eSPI tft = TFT_eSPI();
//static int screenWidth = 320;
//static int screenHeight = 480;
static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
static lv_disp_buf_t disp_buf;

static lv_obj_t *clock_label;


/** Display flushing */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/** Touchpad reading */
static bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;

   // RBU: bool touched = tft.getTouch(&touchX, &touchY, 600);
   bool touched = false;

    if(!touched)
    {
      return false;
    }

    if(touchX > LV_HOR_RES_MAX || touchY > LV_VER_RES_MAX)
    {
      Serial.println("Y or y outside of expected parameters..");
      Serial.print("y:");
      Serial.print(touchX);
      Serial.print(" x:");
      Serial.print(touchY);
    }
    else
    {

      data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
      data->point.x = touchX;
      data->point.y = touchY;
    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

static void lv_main(void)
{
    lv_theme_t * th = lv_theme_material_init(LV_THEME_DEFAULT_COLOR_PRIMARY, LV_THEME_DEFAULT_COLOR_SECONDARY, LV_THEME_MATERIAL_FLAG_LIGHT, LV_THEME_DEFAULT_FONT_SMALL , LV_THEME_DEFAULT_FONT_NORMAL, LV_THEME_DEFAULT_FONT_SUBTITLE, LV_THEME_DEFAULT_FONT_TITLE);     
    lv_theme_set_act(th);

    // Get the current screen
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    // Create a time label
    //clock_label = lv_label_create(scr, NULL);
    //lv_obj_set_pos(clock_label, 0, 0);
    //lv_label_set_text(clock_label, "Hello world");

    lv_obj_t * img = lv_img_create(scr, NULL);
    lv_img_set_src(img, &logo);
    // lv_obj_set_width(img,634);
    // lv_obj_set_size(wp,654,588);
    //lv_obj_set_pos(img, 150, 0);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    
    //static lv_style_t image;
    //lv_style_set_border_color(&image, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    //lv_obj_add_style(img, LV_STATE_DEFAULT, &image);


    TRACE("lv_main OK");
}


static void lvgl_task(void *arg)
{
    lv_init();

    // Enable Display LED
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);

    tft.begin();
    tft.setRotation(1);

    TRACE("TFT GPIO configuration:");
    TRACE("   TFT_MOSI: %d", TFT_MOSI);
    TRACE("   TFT_SCLK: %d", TFT_SCLK);
    TRACE("   TFT_CS: %d", TFT_CS);
    TRACE("   TFT_DC: %d", TFT_DC);
    TRACE("   TFT_RST: %d", TFT_RST);
    TRACE("   TOUCH_CS: %d", TOUCH_CS);

    // RBU:
    //uint16_t calData[5] = { 295, 3493, 320, 3602, 2 };
    //tft.setTouch(calData);

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;   /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);      /*Finally register the driver*/

    lv_main();

    TRACE("lvgl task is running ...");

    while (1)
    {
        lv_task_handler();
         vTaskDelay(20);
    }
}


/** App entry point */
void setup(void)
{
    trace_init();
    xTaskCreate(lvgl_task, "gui", 4096 * 2, NULL, 2, NULL);

    TRACE("Setup OK");
}

void loop(void)
{
    TRACE("Alive");
    vTaskDelay(1000);
}
