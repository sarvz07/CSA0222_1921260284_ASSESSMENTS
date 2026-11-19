#include <stdio.h>

#define MAX_PATIENTS 100

struct date {
    int day;
    int month;
    int year;
};

struct blood_pressure {
    int systolic;
    int diastolic;
};

struct patient {
    char name[20];
    int age;
    char phone[11];
    struct blood_pressure bp;
    struct date visit;
};

struct patient_register {
    struct patient patients[MAX_PATIENTS];
    int count;
};

char old_name[100][20];
int old_age[100];
char old_phone[100][10];
float old_bp[100];
char old_visit_date[100][8];
int old_count;

void addPatient(struct patient_register *reg, const struct patient *p)
{
    if (reg->count >= MAX_PATIENTS) {
        printf("register full: %s was not added\n", p->name);
        return;
    }
    reg->patients[reg->count] = *p;
    reg->count++;
}

void sortByAge(struct patient_register *reg)
{
    for (int i = 0; i < reg->count - 1; i++) {
        int youngest = i;
        for (int j = i + 1; j < reg->count; j++) {
            if (reg->patients[j].age < reg->patients[youngest].age) {
                youngest = j;
            }
        }
        if (youngest != i) {
            struct patient temp = reg->patients[i];
            reg->patients[i] = reg->patients[youngest];
            reg->patients[youngest] = temp;
        }
    }
}

void printRegister(const struct patient_register *reg)
{
    for (int i = 0; i < reg->count; i++) {
        const struct patient *p = &reg->patients[i];
        printf("%-10s %3d  %-10s  %3d/%3d  %02d/%02d/%04d\n",
               p->name, p->age, p->phone,
               p->bp.systolic, p->bp.diastolic,
               p->visit.day, p->visit.month, p->visit.year);
    }
}

int main(void)
{
    struct patient_register reg = {0};
    struct patient p1 = {"Asha", 34, "9876543210", {120, 80}, {12, 10, 2026}};
    struct patient p2 = {"Ravi", 21, "9123456789", {118, 76}, {11, 10, 2026}};
    struct patient p3 = {"Meena", 45, "9001234567", {132, 88}, {10, 10, 2026}};

    addPatient(&reg, &p1);
    addPatient(&reg, &p2);
    addPatient(&reg, &p3);
    sortByAge(&reg);
    printf("Patients sorted by age:\n");
    printRegister(&reg);

    size_t parallel = sizeof old_name + sizeof old_age + sizeof old_phone +
                      sizeof old_bp + sizeof old_visit_date + sizeof old_count;
    size_t structured = sizeof reg.patients;
    printf("parallel arrays bytes = %zu\n", parallel);
    printf("array of structures bytes = %zu\n", structured);
    printf("per patient: parallel = %zu, structure = %zu\n",
           (parallel - sizeof old_count) / MAX_PATIENTS, sizeof(struct patient));
    return 0;
}
