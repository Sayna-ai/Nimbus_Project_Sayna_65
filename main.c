#include <stdio.h>
#include <string.h>

#define MAX_MEDICINES 20
#define NAME_LEN 50
#define TAX_RATE 0.18f   // 18% tax

typedef struct {
    char name[NAME_LEN];
    float price;
    int quantity;
    char dosage[20];
    float amount;
} Medicine;

typedef struct {
    char name[NAME_LEN];
    int age;
    char gender[10];
    char phone[15];
    int patientId;
} Patient;

typedef struct {
    Patient patient;
    Medicine meds[MAX_MEDICINES];
    int medCount;
    float subTotal;
    float tax;
    float total;
} Prescription;

void inputPatientDetails(Patient *p) {
    printf("Enter patient ID: ");
    scanf("%d", &p->patientId);
    getchar();

    printf("Enter name: ");
    fgets(p->name, NAME_LEN, stdin);
    p->name[strcspn(p->name, "\n")] = '\0';

    printf("Enter age: ");
    scanf("%d", &p->age);
    getchar();

    printf("Enter gender: ");
    fgets(p->gender, sizeof(p->gender), stdin);
    p->gender[strcspn(p->gender, "\n")] = '\0';

    printf("Enter phone: ");
    fgets(p->phone, sizeof(p->phone), stdin);
    p->phone[strcspn(p->phone, "\n")] = '\0';
}

void addMedicines(Prescription *pr) {
    int ch;
    pr->medCount = 0;

    do {
        if (pr->medCount >= MAX_MEDICINES) {
            printf("Medicine limit reached.\n");
            break;
        }

        Medicine *m = &pr->meds[pr->medCount];

        printf("\nMedicine name: ");
        fgets(m->name, NAME_LEN, stdin);
        m->name[strcspn(m->name, "\n")] = '\0';

        printf("Dosage (e.g. 1-0-1): ");
        fgets(m->dosage, sizeof(m->dosage), stdin);
        m->dosage[strcspn(m->dosage, "\n")] = '\0';

        printf("Price per unit: ");
        scanf("%f", &m->price);

        printf("Quantity: ");
        scanf("%d", &m->quantity);
        if (m->quantity <= 0) 
        {
           printf("Invalid quantity! Setting quantity = 1\n");
              m->quantity = 1;
        }
        getchar();

        m->amount = m->price * (float)m->quantity;

        pr->medCount++;

        printf("Add another medicine? (1 = yes / 0 = no): ");
        scanf("%d", &ch);
        getchar();

    } while (ch == 1);
}

void calculateBill(Prescription *pr) {
    pr->subTotal = 0.0f;

    for (int i = 0; i < pr->medCount; i++) {
        pr->subTotal += pr->meds[i].amount;
    }

    pr->tax = pr->subTotal * TAX_RATE;
    pr->total = pr->subTotal + pr->tax;
}

void printInvoice(const Prescription *pr) {
    printf("\n================= HOSPITAL BILL =================\n");
    printf("Patient ID : %d\n", pr->patient.patientId);
    printf("Name       : %s\n", pr->patient.name);
    printf("Age/Gender : %d / %s\n", pr->patient.age, pr->patient.gender);
    printf("Phone      : %s\n", pr->patient.phone);

    printf("\nMedicines:\n");
    printf("%-3s %-20s %-10s %-10s %-10s\n",
           "No", "Name", "Price", "Qty", "Amount");

    for (int i = 0; i < pr->medCount; i++) {
        const Medicine *m = &pr->meds[i];
        printf("%-3d %-20s %-10.2f %-10d %-10.2f\n",
               i + 1, m->name, m->price, m->quantity, m->amount);
    }

    printf("\nSubtotal : %.2f\n", pr->subTotal);
    printf("Tax (%.0f%%): %.2f\n", TAX_RATE * 100.0f, pr->tax);
    printf("Total   : %.2f\n", pr->total);
    printf("=================================================\n");
}

void saveInvoiceToFile(const Prescription *pr, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error writing invoice file.\n");
        return;
    }

    fprintf(fp, "HOSPITAL PRESCRIPTION AND BILL\n");
    fprintf(fp, "Patient ID : %d\n", pr->patient.patientId);
    fprintf(fp, "Name       : %s\n", pr->patient.name);
    fprintf(fp, "Age/Gender : %d / %s\n", pr->patient.age, pr->patient.gender);
    fprintf(fp, "Phone      : %s\n\n", pr->patient.phone);

    fprintf(fp, "Medicines:\n");
    for (int i = 0; i < pr->medCount; i++) {
        const Medicine *m = &pr->meds[i];
        fprintf(fp, "%d. %s | %.2f x %d = %.2f\n",
                i + 1, m->name, m->price, m->quantity, m->amount);
    }

    fprintf(fp, "\nSubtotal : %.2f\n", pr->subTotal);
    fprintf(fp, "Tax      : %.2f\n", pr->tax);
    fprintf(fp, "Total    : %.2f\n", pr->total);

    fclose(fp);
    printf("Invoice saved to file: %s\n", filename);
}

int main() {
    Prescription pr;

    printf("=== Hospital Prescription and Billing System ===\n\n");

    inputPatientDetails(&pr.patient);
    addMedicines(&pr);
    calculateBill(&pr);
    printInvoice(&pr);

    saveInvoiceToFile(&pr, "invoice.txt");

    return 0;
}

