#include <stdio.h>
#include <string.h>

#define MAX_MEDICINES 20
#define NAME_LEN      50
#define TAX_RATE      0.18f   // 18% tax

//Structures

struct Medicine {
    char name[NAME_LEN];
    float price;
    int quantity;
    char dosage[20];
    float amount;
};

struct Patient {
    char name[NAME_LEN];
    int age;
    char gender[10];
    char phone[15];
    int patientId;
};

struct Prescription {
    struct Patient patient;
    struct Medicine meds[MAX_MEDICINES];
    int medCount;
    float subTotal;
    float tax;
    float total;
};

//Simple helpers

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ---------- Input functions ---------- */

void inputPatientDetails(struct Patient *p) {

    printf("Enter patient ID: ");
    scanf("%d", &p->patientId);
    clear_buffer();

    printf("Enter patient name: ");
    fgets(p->name, NAME_LEN, stdin);
    trim_newline(p->name);

    printf("Enter age: ");
    scanf("%d", &p->age);
    clear_buffer();
    if (p->age <= 0) {
        printf("Invalid age. Setting age to 1.\n");
        p->age = 1;
    }

    printf("Enter gender: ");
    fgets(p->gender, sizeof(p->gender), stdin);
    trim_newline(p->gender);

    printf("Enter phone number: ");
    fgets(p->phone, sizeof(p->phone), stdin);
    trim_newline(p->phone);
}

void addMedicines(struct Prescription *pr) {
    int choice = 1;
    pr->medCount = 0;

    while (choice == 1 && pr->medCount < MAX_MEDICINES) {

        struct Medicine *m = &pr->meds[pr->medCount];

        printf("\nMedicine %d:\n", pr->medCount + 1);

        printf("Name: ");
        fgets(m->name, NAME_LEN, stdin);
        trim_newline(m->name);

        printf("Dosage (e.g. 1-0-1): ");
        fgets(m->dosage, sizeof(m->dosage), stdin);
        trim_newline(m->dosage);

        printf("Price per unit: ");
        scanf("%f", &m->price);

        if (m->price < 0) {
            printf("Invalid price. Setting to 0.\n");
            m->price = 0;
        }

        printf("Quantity: ");
        scanf("%d", &m->quantity);
        clear_buffer();

        if (m->quantity <= 0) {
            printf("Invalid quantity. Setting to 1.\n");
            m->quantity = 1;
        }

        m->amount = m->price * m->quantity;

        pr->medCount++;

        if (pr->medCount >= MAX_MEDICINES) {
            printf("Medicine limit reached.\n");
            break;
        }

        printf("Add another medicine? (1=yes / 0=no): ");
        scanf("%d", &choice);
        clear_buffer();
    }
}

// Billing

void calculateBill(struct Prescription *pr) {
    pr->subTotal = 0;

    for (int i = 0; i < pr->medCount; i++) {
        pr->subTotal += pr->meds[i].amount;
    }

    pr->tax = pr->subTotal * TAX_RATE;
    pr->total = pr->subTotal + pr->tax;
}

//Printing

void printInvoice(const struct Prescription *pr) {
    printf("\n================= HOSPITAL BILL =================\n");
    printf("Patient ID : %d\n", pr->patient.patientId);
    printf("Name       : %s\n", pr->patient.name);
    printf("Age/Gender : %d / %s\n", pr->patient.age, pr->patient.gender);
    printf("Phone      : %s\n", pr->patient.phone);

    printf("\nMedicines:\n");
    printf("%-3s %-20s %-10s %-10s %-10s\n",
           "No", "Name", "Price", "Qty", "Amount");

    for (int i = 0; i < pr->medCount; i++) {
        const struct Medicine *m = &pr->meds[i];
        printf("%-3d %-20s %-10.2f %-10d %-10.2f\n",
               i + 1, m->name, m->price, m->quantity, m->amount);
    }

    printf("\nSubtotal : %.2f\n", pr->subTotal);
    printf("Tax (18%%): %.2f\n", pr->tax);
    printf("Total    : %.2f\n", pr->total);
    printf("=================================================\n");
}

// Save to file

void saveInvoiceToFile(const struct Prescription *pr, const char *fname) {
    FILE *fp = fopen(fname, "w");
    if (!fp) {
        printf("Unable to create invoice file.\n");
        return;
    }

    fprintf(fp, "HOSPITAL PRESCRIPTION AND BILL\n");
    fprintf(fp, "Patient ID : %d\n", pr->patient.patientId);
    fprintf(fp, "Name       : %s\n", pr->patient.name);
    fprintf(fp, "Age/Gender : %d / %s\n", pr->patient.age, pr->patient.gender);
    fprintf(fp, "Phone      : %s\n\n", pr->patient.phone);

    fprintf(fp, "Medicines:\n");
    for (int i = 0; i < pr->medCount; i++) {
        const struct Medicine *m = &pr->meds[i];
        fprintf(fp, "%d. %s | %.2f x %d = %.2f\n",
                i + 1, m->name, m->price, m->quantity, m->amount);
    }

    fprintf(fp, "\nSubtotal : %.2f\n", pr->subTotal);
    fprintf(fp, "Tax      : %.2f\n", pr->tax);
    fprintf(fp, "Total    : %.2f\n", pr->total);

    fclose(fp);

    printf("\nInvoice saved to file: %s\n", fname);
}

//Main

int main() {

    struct Prescription pr;

    printf("=== Hospital Prescription and Billing System ===\n\n");

    inputPatientDetails(&pr.patient);
    addMedicines(&pr);
    calculateBill(&pr);
    printInvoice(&pr);
    saveInvoiceToFile(&pr, "invoice.txt");

    return 0;
}
