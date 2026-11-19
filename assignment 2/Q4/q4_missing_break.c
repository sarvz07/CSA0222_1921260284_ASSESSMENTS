#include <stdio.h>

char grade(int marks)
{
    if (marks < 0 || marks > 100) return '?';
    switch (marks / 10) {
    case 10:
    case 9:
        return 'S';
    case 8:
        return 'A';
    case 7:
        return 'B';
    case 6:
        return 'C';
    case 5:
        return 'D';
    default:
        return 'F';
    }
}

char grade_missing_break(int marks)
{
    char g = '?';
    if (marks < 0 || marks > 100) return '?';
    switch (marks / 10) {
    case 10:
    case 9:
        g = 'S';
        /* fall through */
    case 8:
        g = 'A';
        break;
    case 7:
        g = 'B';
        break;
    case 6:
        g = 'C';
        break;
    case 5:
        g = 'D';
        break;
    default:
        g = 'F';
        break;
    }
    return g;
}

int main(void)
{
    int marks[] = {100, 95, 83, 50, 49, -5};
    printf("Correct: ");
    for (int i = 0; i < 6; i++) printf("%d:%c ", marks[i], grade(marks[i]));
    printf("\n");
    printf("Missing break after S: ");
    for (int i = 0; i < 6; i++) printf("%d:%c ", marks[i], grade_missing_break(marks[i]));
    printf("\n");
    return 0;
}
