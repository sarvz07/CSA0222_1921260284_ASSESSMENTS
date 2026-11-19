#include <stdio.h>
#include <string.h>

static const char *const valid_groups[] = {
    "O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"
};

static int valid_group(const char *name)
{
    size_t i;
    if (name == NULL) return 0;
    for (i = 0; i < sizeof valid_groups / sizeof valid_groups[0]; i++) {
        if (strcmp(name, valid_groups[i]) == 0) return 1;
    }
    return 0;
}

static void usage(void)
{
    puts("usage: b2_input [scanf|fgets|missing|noargs|badargs|group GROUP]");
}

static void scanf_demo(void)
{
    FILE *fp = tmpfile();
    char small[8];
    int ch;

    if (fp == NULL) { perror("tmpfile"); return; }
    fputs("ChristopherAlexander\nO+\n", fp);
    rewind(fp);
    if (fscanf(fp, "%7s", small) != 1) { perror("fscanf"); fclose(fp); return; }
    printf("scanf %%7s into char[8]: \"%s\" (stored %zu chars)\n", small, strlen(small));
    printf("leftover in stream:");
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) putchar(ch);
    printf("\n");
    puts("unbounded scanf(\"%s\") would write past char[8]; a width prevents overflow");
    puts("leftover characters would also be read as the next field");
    fclose(fp);
}

static void fgets_demo(void)
{
    FILE *fp = tmpfile();
    char name[16];
    char group[16];
    size_t len;
    int ch;
    int truncated;

    if (fp == NULL) { perror("tmpfile"); return; }
    fputs("BartholomewAlexander\nO+\n", fp);
    rewind(fp);
    if (fgets(name, sizeof name, fp) == NULL) { perror("fgets"); fclose(fp); return; }
    len = strlen(name);
    truncated = len > 0 && name[len - 1] != '\n' && !feof(fp);
    printf("fgets into char[16]: \"%s\" stored=%zu truncated=%d\n", name, len, truncated);
    if (truncated) {
        while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
        }
        puts("remaining characters drained so the next field is aligned");
    }
    if (fgets(group, sizeof group, fp) != NULL) {
        len = strlen(group);
        if (len > 0 && group[len - 1] == '\n') group[len - 1] = '\0';
        printf("next line read correctly: \"%s\"\n", group);
    }
    fclose(fp);

    fp = tmpfile();
    if (fp == NULL) { perror("tmpfile"); return; }
    fputs("Asha\n", fp);
    rewind(fp);
    if (fgets(name, sizeof name, fp) != NULL) {
        len = strlen(name);
        truncated = len > 0 && name[len - 1] != '\n' && !feof(fp);
        if (len > 0 && name[len - 1] == '\n') name[len - 1] = '\0';
        printf("fgets short line: \"%s\" truncated=%d\n", name, truncated);
    }
    fclose(fp);
    puts("fgets never writes past the buffer; check for a missing newline");
}

static int missing_file_demo(void)
{
    FILE *fp = fopen("missing_bloodbank_file.dat", "r");
    if (fp == NULL) {
        perror("fopen missing_bloodbank_file.dat");
        return 1;
    }
    fclose(fp);
    puts("file unexpectedly present");
    return 0;
}

int main(int argc, char **argv)
{
    int status = 0;

    if (argc == 1) {
        puts("default run: all adversarial checks");
        scanf_demo();
        fgets_demo();
        printf("missing file status: %d\n", missing_file_demo());
        printf("group O+ valid: %d, group XX valid: %d\n",
               valid_group("O+"), valid_group("XX"));
        return 0;
    }
    if (strcmp(argv[1], "scanf") == 0 && argc == 2) { scanf_demo(); return 0; }
    if (strcmp(argv[1], "fgets") == 0 && argc == 2) { fgets_demo(); return 0; }
    if (strcmp(argv[1], "missing") == 0 && argc == 2) {
        status = missing_file_demo();
        printf("exit status: %d\n", status);
        return status;
    }
    if (strcmp(argv[1], "noargs") == 0 && argc == 2) {
        puts("simulated call with no arguments:");
        usage();
        return 1;
    }
    if (strcmp(argv[1], "badargs") == 0) {
        printf("too many arguments: argc=%d\n", argc);
        usage();
        return 1;
    }
    if (strcmp(argv[1], "group") == 0 && argc == 3) {
        if (!valid_group(argv[2])) {
            printf("invalid blood group: %s\n", argv[2]);
            return 1;
        }
        printf("valid blood group: %s\n", argv[2]);
        return 0;
    }
    printf("unrecognised arguments: argc=%d\n", argc);
    usage();
    return 1;
}
