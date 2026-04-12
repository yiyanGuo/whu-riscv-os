#include "defs.h"
#include "types.h"

extern uchar _binary_user_sh_bin_start[];
extern uchar _binary_user_sh_bin_end[];
extern uchar _binary_user_sh_bin_size[];
extern uchar _binary_user_fork_test_bin_start[];
extern uchar _binary_user_fork_test_bin_size[];
extern uchar _binary_user_print0_test_bin_start[];
extern uchar _binary_user_print0_test_bin_size[];
extern uchar _binary_user_goodnight_bin_start[];
extern uchar _binary_user_goodnight_bin_size[];
extern uchar _binary_user_echo_bin_start[];
extern uchar _binary_user_echo_bin_size[];
struct user_program user_programs[] = {
    {"sh", (uint64)_binary_user_sh_bin_start,(uint64)_binary_user_sh_bin_size},
    {"fork_test", (uint64)_binary_user_fork_test_bin_start, (uint64)_binary_user_fork_test_bin_size},
    {"print0_test", (uint64)_binary_user_print0_test_bin_start, (uint64)_binary_user_print0_test_bin_size},
    {"goodnight", (uint64)_binary_user_goodnight_bin_start, (uint64)_binary_user_goodnight_bin_size},
    {"echo", (uint64)_binary_user_echo_bin_start, (uint64)_binary_user_echo_bin_size}
};

uint64 nuser_programs = sizeof(user_programs) / sizeof(user_programs[0]);
