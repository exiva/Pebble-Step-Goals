#include "app.h"
#include "window_menu.h"
#include "window_goal.h"

static Window *window;
static int step_goal = 4;

int getStepGoal() {
    return step_goal;
}

void setStepGoal(int choice) {
  step_goal = choice;
}

static void init(void) {
  if (persist_exists(GOAL)) {
    step_goal = persist_read_int(GOAL);
  }

  if (!app_worker_is_running()) {
    app_worker_launch();
  }

  window = window_create();
  if (launch_reason() == APP_LAUNCH_WORKER) {
    window_set_window_handlers(window, (WindowHandlers) {
      .load = goal_window_load,
      .unload = goal_window_unload,
    });
  } else {
    window_set_window_handlers(window, (WindowHandlers) {
      .load = menu_window_load,
      .unload = menu_window_unload,
    });
  }
  window_stack_push(window, true);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
