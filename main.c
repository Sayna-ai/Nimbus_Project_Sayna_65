#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* ---------- Constants ---------- */

#define MAX_MEDICINES 20
#define NAME_LEN      50
#define TAX_RATE      0.18f   /* 18% tax */

/* ---------- Data structures ---------- */

typedef struct {
    char name[NAME_LEN];
    float price;
    int quantity;
    char dosage[20];
    float amount;
} Medicine;

typedef struct {
    char name[NAME_LEN];
    int  age;
    char gender[10];
    char phone[15];
    int  patientId;
} Patient;

typedef struct {
    Patient patient;
    Medicine meds[MAX_MEDICINES];
    int   medCount;
    float subTotal;
    float tax;
    float total;
} Prescription;

/* ---------- Safe input helpers ---------- */

int read_line_stdin(char *buf, int size) {
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        clearerr(stdin);  /* reset in case of error */
        return 0;
    }
    size_t len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }
    return 1;
}

int read_int_strict(const char *prompt, int min, int max) {
    char buf[128];
    char *endptr;
    long val;

    while (1) {
        printf("%s", prompt);
        if (!read_line_stdin(buf, sizeof buf))
            continue;

        if (buf[0] == '\0') {
            printf("Input cannot be empty. Please try again.\n");
            continue;
        }

        errno = 0;
        val = strtol(buf, &endptr, 10);
        if (errno != 0) {
            printf("Invalid number. Please try again.\n");
            continue;
        }

        /* skip trailing spaces */
        while (*endptr && isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0') {
            printf("Invalid input. Please enter digits only.\n");
            continue;
        }

        if (val < min || val > max) {
            printf("Please enter a value between %d and %d.\n", min, max);
            continue;
        }

        return (int)val;
    }
}

float read_float_strict(const char *prompt, float min, float max) {
    char buf[128];
    char *endptr;
    float val;

    while (1) {
        printf("%s", prompt);
        if (!read_line_stdin(buf, sizeof buf))
            continue;

        if (buf[0] == '\0') {
            printf("Input cannot be empty. Please try again.\n");
            continue;
        }

        errno = 0;
        val = strtof(buf, &endptr);
        if (errno != 0) {
            printf("Invalid number. Please try again.\n");
            continue;
        }

        while (*endptr && isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0') {
            printf("Invalid input. Please enter a numeric value.\n");
            continue;
        }

        if (val < min || val > max) {
            printf("Please enter a value between %.2f and %.2f.\n", min, max);
            continue;
        }

        return val;
    }
}

void read_nonempty_string(const char *prompt, char *dest, int size) {
    char buf[256];

    while (1) {
        printf("%s", prompt);
        if (!read_line_stdin(buf, sizeof buf))
            continue;

        if (buf[0] == '\0') {
            printf("Input cannot be empty. Please try again.\n");
            continue;
        }

        /* Optional: forbid strings that are only digits/spaces */
        int has_alpha = 0;
        for (size_t i = 0; buf[i] != '\0'; i++) {
            if (isalpha((unsigned char)buf[i])) {
                has_alpha = 1;
                break;
            }
        }
        if (!has_alpha) {
            printf("Please enter a valid text value (not only numbers).\n");
            continue;
        }

        strncpy(dest, buf, (size_t)size - 1);
        dest[size - 1] = '\0';
        return;
    }
}

/* Simple helper when answer must be 0 or 1 */
int read_yes_no_int(const char *prompt) {
    int v;
    while (1) {
        v = read_int_strict(prompt, 0, 1);
        if (v == 0 || v == 1) return v;
    }
}

/* ---------- Core logic ---------- */

void inputPatientDetails(Patient *p) {
    p->patientId = read_int_strict("Enter patient ID: ", 1, 1000000000);

    read_nonempty_string("Enter name: ", p->name, sizeof p->name);

    p->age = read_int_strict("Enter age: ", 0, 130);

    read_nonempty_string("Enter gender: ", p->gender, sizeof p->gender);

    /* Phone: only check non-empty & max length, allow any characters */
    char tmp[32];
    while (1) {
        printf("Enter phone: ");
        if (!read_line_stdin(tmp, sizeof tmp))
            continue;
        if (tmp[0] == '\0') {
            printf("Phone cannot be empty. Please try again.\n");
            continue;
        }
        strncpy(p->phone, tmp, sizeof p->phone - 1);
        p->phone[sizeof p->phone - 1] = '\0';
        break;
    }
}

void addMedicines(Prescription *pr) {
    pr->medCount = 0;

    while (1) {
        if (pr->medCount >= MAX_MEDICINES) {
            printf("Medicine limit reached (%d).\n", MAX_MEDICINES);
            break;
        }

        Medicine *m = &pr->meds[pr->medCount];

        read_nonempty_string("\nMedicine name: ", m->name, sizeof m->name);
        read_nonempty_string("Dosage (e.g. 1-0-1): ", m->dosage, sizeof m->dosage);

        m->price = read_float_strict("Price per unit: ", 0.0f, 1000000.0f);

        m->quantity = read_int_strict("Quantity: ", 1, 1000000);

        m->amount = m->price * (float)m->quantity;

        pr->medCount++;

        if (read_yes_no_int("Add another medicine? (1 = yes / 0 = no): ") == 0)
            break;
    }
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
        /* Show why it failed (permissions, path, etc.) */
        perror("Error opening invoice file");
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

    if (fclose(fp) == EOF) {
        perror("Error closing invoice file");
        return;
    }

    printf("Invoice saved to file: %s\n", filename);
}

/* ---------- Main ---------- */

int main(void) {
    Prescription pr;

    printf("=== Hospital Prescription and Billing System ===\n\n");

    inputPatientDetails(&pr.patient);
    addMedicines(&pr);
    calculateBill(&pr);
    printInvoice(&pr);
    saveInvoiceToFile(&pr, "invoice.txt");

    return 0;
}
