#include "app.h"

static MenuLayer *main_menu;
static Layer *info_bar;
static int step_count = 0;

#define SECTIONS 1
#define GOALS 7

static int16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return SECTIONS;
}

static int16_t menu_get_num_rows_callback(MenuLayer *menu_layer, int16_t section_index, void *data) {
  return GOALS;
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return 44;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  GRect bounds = layer_get_bounds(cell_layer);
  GPoint p = GPoint(bounds.size.w - 15, (bounds.size.h / 2));
  switch (cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "2k steps/day", NULL, NULL);
    break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "4k steps/day", NULL, NULL);
    break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "6k steps/day", NULL, NULL);
    break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "8k steps/day", NULL, NULL);
    break;
    case 4:
      menu_cell_basic_draw(ctx, cell_layer, "10k steps/day", NULL, NULL);
    break;
    case 5:
      menu_cell_basic_draw(ctx, cell_layer, "15k steps/day", NULL, NULL);
    break;
    case 6:
      menu_cell_basic_draw(ctx, cell_layer, "20k steps/day", NULL, NULL);
    break;
  }

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorFolly);

  if (menu_cell_layer_is_highlighted(cell_layer)) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  graphics_draw_circle(ctx, p, 7);

  if (cell_index->row == getStepGoal()) {
    graphics_fill_circle(ctx, p, 5);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // step_goal = cell_index->row;
  setStepGoal(cell_index->row);
  persist_write_int(GOAL, cell_index->row);
  AppWorkerMessage packet = {
    .data0 = cell_index->row
  };
  app_worker_send_message(GOAL, &packet);
  menu_layer_reload_data(main_menu);
  layer_mark_dirty(info_bar);
}

static int get_step_count() {
    HealthServiceAccessibilityMask request = health_service_metric_accessible(HealthMetricStepCount,
    time_start_of_today(), time(NULL));
    if(request == HealthServiceAccessibilityMaskAvailable) {
      return (int)health_service_sum_today(HealthMetricStepCount);
    } else {
      return 0;
    }
}

static int get_step_goal(int goal) {
  int value = 0;
  switch (goal) {
    case 0: value = 2000; break;
    case 1: value = 4000; break;
    case 2: value = 6000; break;
    case 3: value = 8000; break;
    case 4: value = 10000;  break;
    case 5: value = 15000;  break;
    case 6: value = 20000;  break;
  };
  return value;
}

static void update_info_bar(Layer *layer, GContext *ctx) {
  int step_goal = get_step_goal(getStepGoal());
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorLavenderIndigo);
  graphics_fill_rect(ctx, GRect(0, 0, (bounds.size.w*step_count)/step_goal, bounds.size.h),0,GCornerNone);

  char* label;
  size_t length;
  length = (size_t)snprintf(NULL, 0, "%d / %d", step_count, step_goal);
  label = malloc(length+1);
  snprintf(label, length+1, "%d / %d", step_count, step_goal);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
    GRect(0, bounds.size.h/2-15, bounds.size.w, bounds.size.h),
    GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  step_count = get_step_count();
  info_bar = layer_create(GRect(0, 0, bounds.size.w, 44));
  layer_add_child(window_layer, info_bar);
  layer_set_update_proc(info_bar, update_info_bar);

  main_menu = menu_layer_create(GRect(0, 44, bounds.size.w, bounds.size.h-44));
  menu_layer_set_highlight_colors(main_menu, GColorTiffanyBlue, GColorWhite);
  menu_layer_set_click_config_onto_window(main_menu, window);

  menu_layer_set_callbacks(main_menu, NULL, (MenuLayerCallbacks) {
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback)menu_get_num_rows_callback,
    .get_cell_height = (MenuLayerGetCellHeightCallback)get_cell_height_callback,
    .draw_row = (MenuLayerDrawRowCallback)menu_draw_row_callback,
    .select_click = (MenuLayerSelectCallback)menu_select_callback,
    .get_num_sections = (MenuLayerGetNumberOfSectionsCallback)menu_get_num_sections_callback,
  });

  layer_add_child(window_layer, menu_layer_get_layer(main_menu));
}

void menu_window_unload(Window *window) {
  menu_layer_destroy(main_menu);
}

void show_main_menu() {
}
