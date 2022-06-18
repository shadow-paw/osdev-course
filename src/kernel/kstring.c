int kstrcmp(const char* a, const char* b) {
    for (; *a && *b; a++, b++) {
        int d = *a - *b;
        if (d != 0) return d;
    }
    if (*a != 0) return -1;
    if (*b != 0) return 1;
    return 0;
}
