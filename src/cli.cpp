#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "cli.hpp"

struct flag {
  char name_short;
  const char *name_long;
  const char *group;
  const char *description;

  bool takes_value;
  bool required;

  flag *next;
};

static char *progname;
static flag *flags;
static flag_result *parsed_flags;

void report_error(const char *err, const char *param) {
  fprintf(stderr, "%s: %s: %s\n", progname, err, param);
  exit(1);
}

flag *lookup_short_flag(char name) {
  for (flag *flag = flags; flag != nullptr; flag = flag->next)
    if (flag->name_short == name)
      return flag;
  char buf[2] = {name, 0};
  report_error("Unknown flag", buf);
  return nullptr;
}

flag *lookup_long_flag(const char *name) {
  for (flag *flag = flags; flag != nullptr; flag = flag->next)
    if (strcmp(flag->name_long, name) == 0)
      return flag;
  report_error("Unknown flag", name);
  return nullptr;
}

flag_result* get_flag_value(const char *name) {
  for (flag_result *flag = parsed_flags; flag != nullptr; flag = flag->next)
    if (strcmp(flag->name_long, name) == 0)
      return flag;
  return nullptr;
}

flag_result* get_flag_group(const char* group) {
  for (flag_result *flag = parsed_flags; flag != nullptr; flag = flag->next)
    if (strcmp(flag->group, group) == 0)
      return flag;
  return nullptr;
}

void check_for_duplicate_groups() {
  for (flag_result *flag1 = parsed_flags; flag1 != nullptr; flag1 = flag1->next)
    for (flag_result *flag2 = flag1->next; flag2 != nullptr; flag2 = flag2->next)
      if (strcmp(flag1->group, flag2->group) == 0 && flag1->group != nullptr)
        report_error("Unexpected option", flag2->name_long);
}

void parse_flags(int argc, char **argv) {
  progname = argv[0];
  parsed_flags = nullptr;
  flag *processing_flag = nullptr;

  for (int i = 1; i < argc; i++) {
    int n = strlen(argv[i]);
    if (n == 0)
      continue;

    if (processing_flag == nullptr && argv[i][0] != '-') {
      report_error("unexpected argument", argv[i]);
    } else if (processing_flag == nullptr && argv[i][0] == '-') {
      if (n == 1)
        report_error("incomplete flag", argv[i]);

      if (argv[i][1] == '-') {
        if (strcmp(&argv[i][2], "help") == 0) print_help();
        processing_flag = lookup_long_flag(&argv[i][2]);
      } else {
        processing_flag = lookup_short_flag(argv[i][1]);
      }

      if (!processing_flag->takes_value) {
        flag_result *neww = (flag_result *)malloc(sizeof(flag_result));
        neww->name_long = processing_flag->name_long;
        neww->group = processing_flag->group;
        neww->value = nullptr;

        if (parsed_flags != nullptr) {
          parsed_flags->next = neww;
        }
        parsed_flags = neww;

        processing_flag = nullptr;
      }
    } else if (processing_flag->takes_value) {
      flag_result *neww = (flag_result *)malloc(sizeof(flag_result));
      neww->name_long = processing_flag->name_long;
      neww->group = processing_flag->group;
      neww->value = argv[i];

      if (parsed_flags != nullptr) {
        parsed_flags->next = neww;
      }
      parsed_flags = neww;

      processing_flag = nullptr;
    }
  }

  if (processing_flag != nullptr && processing_flag->takes_value)
    report_error("flag requires value, but none was provided", processing_flag->name_long);

  for (flag *flag = flags; flag != nullptr; flag = flag->next) {
    flag_result *parsed = get_flag_value(flag->name_long);
    if (flag->required && parsed == nullptr && flag->group == nullptr) 
      report_error("flag required but not provided", flag->name_long);


    if (flag->group != nullptr && get_flag_group(flag->group) == nullptr) 
      report_error("no flags provided for group", flag->group);
    
  }
  check_for_duplicate_groups();
}

void print_parsed_flags() {
  flag_result *flag = parsed_flags;
  while (flag != nullptr) {
    printf("--%s : group '%s', value '%s'\n", flag->name_long, flag->group,
           flag->value);
    flag = flag->next;
  }
}

void print_help() {
  fprintf(stderr, "Usage:\n\t%s [FLAGS]\n\n", progname);

  fprintf(stderr, "Flags:\n");
  for (flag *flag = flags; flag != nullptr; flag = flag->next) {
    fprintf(stderr, "\t");
    int n = 0;
    if (flag->name_short) n += fprintf(stderr, "-%c", flag->name_short);
    if (flag->name_short && flag->name_long) n += fprintf(stderr, ", ");
    if (flag->name_long) n += fprintf(stderr, "--%s", flag->name_long);
    if (flag->takes_value) n += fprintf(stderr, " <VALUE>");

    fprintf(stderr, "%*c", 40-n, ' ');
    fprintf(stderr, "%s\n", flag->description);
  }

  exit(0);
}

void push_flag(char name_short, const char* name, bool takes_value, bool required, const char* group, const char* description) {
  flag *fl = (flag*) malloc(sizeof(flag));
  fl->name_long = name;
  fl->name_short = name_short == ' ' ? 0 : name_short;
  fl->takes_value = takes_value;
  fl->required = required;
  fl->group = group;
  fl->next = flags;
  fl->description = description;
  flags = fl;
}

