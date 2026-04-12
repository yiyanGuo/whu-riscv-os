

int strlen(const char *str) {
    int len = 0;
    while(*str != '\0') {
        len++;
        str++;
    }
    return len;
}

int strcmp(const char *str1, const char *str2) {
    while(*str1 || *str2) {
        if(*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
    return 1;
}
