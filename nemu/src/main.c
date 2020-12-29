
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  //int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  //ui_mainloop(is_batch_mode);
  char *a;
  char ch[] = "392-583*170";
  printf("%d\n", expr(ch, a));
  printf("%d\n", *a);
  return 0;
}
