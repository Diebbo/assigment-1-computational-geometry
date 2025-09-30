struct flag_result {
  const char *name_long;
  const char *group;
  char *value;

  flag_result *next;
};

void push_flag(const char short_name, const char* name, bool takes_value, bool required, const char* group, const char* description);
void parse_flags(int argc, char **argv);
flag_result* get_flag_value(const char *name);
flag_result* get_flag_group(const char* group);
void print_parsed_flags();
void print_help();
