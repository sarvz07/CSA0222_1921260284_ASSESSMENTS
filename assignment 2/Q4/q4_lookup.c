#include <stdio.h>

char grade_lookup(int marks)
{
    char grades[11] = {'F','F','F','F','F','D','C','B','A','S','S'};
    if (marks < 0 || marks > 100) return '?';
    return grades[marks / 10];
}

char scholarship(int marks, int income)
{
    char g = grade_lookup(marks);
    if (g == '?' || g == 'F') return 'N';
    if (g == 'S' && income <= 300000) return 'A';
    if ((g == 'S' || g == 'A') && income <= 500000) return 'B';
    if (income <= 800000) return 'C';
    return 'N';
}

int main(void)
{
    int marks[] = {100, 95, 83, 50, 49, -5};
    for (int i = 0; i < 6; i++) printf("%d:%c ", marks[i], grade_lookup(marks[i]));
    printf("\n");
    printf("scholarship(95,200000):%c\n", scholarship(95, 200000));
    printf("scholarship(85,400000):%c\n", scholarship(85, 400000));
    printf("scholarship(75,700000):%c\n", scholarship(75, 700000));
    printf("scholarship(75,900000):%c\n", scholarship(75, 900000));
    return 0;
}
