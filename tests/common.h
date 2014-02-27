#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

void save_output(void (*testfunc)(), const char *output_filename);
char *read_file(const char *filename, unsigned long *size);

#endif
