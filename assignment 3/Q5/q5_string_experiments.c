#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 256

static size_t own_strlen(const char *s)
{
    size_t n = 0;
    while (s[n] != '\0') n++;
    return n;
}

static void own_strcpy(char *dst, const char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int own_strcmp(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

static void own_upper(char *s)
{
    for (int i = 0; s[i] != '\0'; i++) s[i] = (char)toupper((unsigned char)s[i]);
}

static void own_reverse(const char *s, char *out)
{
    size_t n = own_strlen(s);
    for (size_t i = 0; i < n; i++) out[i] = s[n - 1 - i];
    out[n] = '\0';
}

static int own_word_count(const char *s)
{
    int count = 0;
    int inside = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (isspace((unsigned char)s[i])) inside = 0;
        else if (!inside) {
            count++;
            inside = 1;
        }
    }
    return count;
}

static int own_palindrome(const char *s)
{
    char forward[BUFFER_SIZE];
    char backward[BUFFER_SIZE];
    int f = 0;
    int b = 0;
    size_t n = own_strlen(s);
    for (size_t i = 0; i < n; i++) {
        if (isalpha((unsigned char)s[i])) forward[f++] = (char)toupper((unsigned char)s[i]);
    }
    for (size_t i = n; i > 0; i--) {
        if (isalpha((unsigned char)s[i - 1])) backward[b++] = (char)toupper((unsigned char)s[i - 1]);
    }
    forward[f] = '\0';
    backward[b] = '\0';
    return own_strcmp(forward, backward) == 0;
}

static int lib_word_count(const char *s)
{
    int count = 0;
    int inside = 0;
    size_t n = strlen(s);
    for (size_t i = 0; i < n; i++) {
        if (isspace((unsigned char)s[i])) inside = 0;
        else if (!inside) {
            count++;
            inside = 1;
        }
    }
    return count;
}

static void lib_reverse(const char *s, char *out)
{
    size_t n = strlen(s);
    own_strcpy(out, s);
    for (size_t i = 0; i < n / 2; i++) {
        char t = out[i];
        out[i] = out[n - 1 - i];
        out[n - 1 - i] = t;
    }
}

static int lib_palindrome(const char *s)
{
    char forward[BUFFER_SIZE];
    char backward[BUFFER_SIZE];
    int f = 0;
    int b = 0;
    size_t n = strlen(s);
    for (size_t i = 0; i < n; i++) {
        if (isalpha((unsigned char)s[i])) forward[f++] = (char)toupper((unsigned char)s[i]);
    }
    for (size_t i = n; i > 0; i--) {
        if (isalpha((unsigned char)s[i - 1])) backward[b++] = (char)toupper((unsigned char)s[i - 1]);
    }
    forward[f] = '\0';
    backward[b] = '\0';
    return strcmp(forward, backward) == 0;
}

static void reverse_recursive(const char *s, size_t len, size_t i, char *out)
{
    if (i >= len) {
        out[len] = '\0';
        return;
    }
    out[len - 1 - i] = s[i];
    reverse_recursive(s, len, i + 1, out);
}

static size_t repeated_strlen_scans(const char *s)
{
    size_t scans = 0;
    for (size_t i = 0; i < strlen(s); i++) scans += strlen(s) + 1;
    return scans;
}

static void compare_message(const char *label, const char *message)
{
    char upper[BUFFER_SIZE];
    char reversed_own[BUFFER_SIZE];
    char reversed_lib[BUFFER_SIZE];
    char reversed_recursive[BUFFER_SIZE];

    own_strcpy(upper, message);
    own_upper(upper);
    own_reverse(message, reversed_own);
    lib_reverse(message, reversed_lib);
    reverse_recursive(message, strlen(message), 0, reversed_recursive);

    printf("%-12s own_words = %d, lib_words = %d, own_pal = %d, lib_pal = %d\n",
           label, own_word_count(message), lib_word_count(message),
           own_palindrome(message), lib_palindrome(message));
    printf("%-12s upper = \"%s\", reverse = \"%s\", library reverse match %s, recursive match %s\n",
           "", upper, reversed_own,
           strcmp(reversed_own, reversed_lib) == 0 ? "yes" : "no",
           strcmp(reversed_own, reversed_recursive) == 0 ? "yes" : "no");
}

static void safe_input_demo(void)
{
    FILE *fp = tmpfile();
    char line[128];
    char small[16];
    const char *long_message = "This message is far too long for a sixteen byte buffer";

    if (fp == NULL) {
        fprintf(stderr, "tmpfile failed\n");
        return;
    }
    fputs("Ada Lovelace\n", fp);
    fputs("Madam, I'm Adam\n", fp);
    fputs("Meet\tat\tthe library\n", fp);
    rewind(fp);
    printf("\nfgets lines (newline removed, length computed once)\n");
    while (fgets(line, sizeof line, fp) != NULL) {
        size_t len = own_strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        printf("\"%s\" words = %d, palindrome = %d\n", line, own_word_count(line), own_palindrome(line));
    }
    fclose(fp);

    printf("\nunsafe strcpy would write %zu bytes into a %zu-byte buffer\n",
           strlen(long_message) + 1, sizeof small);
    snprintf(small, sizeof small, "%.*s", (int)(sizeof small - 1), long_message);
    printf("snprintf result: \"%s\"\n", small);
    strncpy(small, long_message, sizeof small - 1);
    small[sizeof small - 1] = '\0';
    printf("strncpy result:  \"%s\"\n", small);
    printf("gets() was removed because it has no way to limit the number of bytes written\n");
}

int main(void)
{
    compare_message("normal", "  Meet at  the library at 5 PM ");
    compare_message("tab", "Meet\tat\tthe library");
    compare_message("punct", "Madam, I'm Adam");
    compare_message("empty", "");
    safe_input_demo();
    printf("\nrepeated strlen in a loop over %zu characters performs %zu byte reads\n",
           strlen("A short feedback message"), repeated_strlen_scans("A short feedback message"));
    return 0;
}
