#include <stdio.h>
#include <string.h>

#define MAX_MED 20
#define NAME_LEN 50
#define TAX 0.18f

// Medicine structure
struct Medicine {
    char name[NAME_LEN];
    char dose[20];
    float price;
    int qty;
    float total;
};

// Patient structure
struct Patient {
    int id;
    char name[NAME_LEN];
    int age;
    char gender[10];
    char phone[15];
};

// Prescription structure
struct Prescription {
    struct Patient pat;
    struct Medicine medList[MAX_MED];
    int count;
    float subtotal;
    float tax;
    float finalTotal;
};

// Remove newline from string
void trim(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == '\n') {
            s[i] = '\0';
            break;
        }
        i++;
    }
}

// Take patient details
void getPatient(struct Patient *p) {
    printf("Patient ID: ");
    scanf("%d", &(*p).id);
    getchar();

    printf("Name: ");
    fgets((*p).name, NAME_LEN, stdin);
    trim((*p).name);

    printf("Age: ");
    scanf("%d", &(*p).age);
    getchar();
    if ((*p).age <= 0) (*p).age = 1;

    printf("Gender: ");
    fgets((*p).gender, sizeof((*p).gender), stdin);
    trim((*p).gender);

    printf("Phone: ");
    fgets((*p).phone, sizeof((*p).phone), stdin);
    trim((*p).phone);
}

// Take medicine details
void addMedicines(struct Prescription *pr) {
    int more = 1;
    (*pr).count = 0;

    while (more == 1 && (*pr).count < MAX_MED) {
        struct Medicine *m = &(*pr).medList[(*pr).count];

        printf("\nMedicine %d\n", (*pr).count + 1);

        printf("Name: ");
        fgets((*m).name, NAME_LEN, stdin);
        if ((*m).name[0] == '\n') fgets((*m).name, NAME_LEN, stdin);
        trim((*m).name);

        printf("Dosage: ");
        fgets((*m).dose, sizeof((*m).dose), stdin);
        trim((*m).dose);

        printf("Price: ");
        scanf("%f", &(*m).price);

        printf("Quantity: ");
        scanf("%d", &(*m).qty);
        getchar();

        if ((*m).price < 0) (*m).price = 0;
        if ((*m).qty <= 0) (*m).qty = 1;

        (*m).total = (*m).price * (*m).qty;

        (*pr).count++;

        printf("Add more? (1=yes / 0=no): ");
        scanf("%d", &more);
        getchar();
    }
}

// Calculate billing
void calculateBill(struct Prescription *pr) {
    int i;
    (*pr).subtotal = 0;

    for (i = 0; i < (*pr).count; i++) {
        (*pr).subtotal += (*pr).medList[i].total;
    }

    (*pr).tax = (*pr).subtotal * TAX;
    (*pr).finalTotal = (*pr).subtotal + (*pr).tax;
}

// Print invoice on screen
void printInvoice(struct Prescription *pr) {
    int i;

    printf("\n=========== BILL ===========\n");
    printf("Patient ID: %d\n", (*pr).pat.id);
    printf("Name: %s\n", (*pr).pat.name);
    printf("Age: %d\n", (*pr).pat.age);
    printf("Gender: %s\n", (*pr).pat.gender);
    printf("Phone: %s\n", (*pr).pat.phone);

    printf("\nNo  Name                 Price   Qty   Amount\n");

    for (i = 0; i < (*pr).count; i++) {
        struct Medicine m = (*pr).medList[i];
        printf("%-3d %-20s %-7.2f %-5d %-7.2f\n",
               i + 1, m.name, m.price, m.qty, m.total);
    }

    printf("\nSubtotal: %.2f\n", (*pr).subtotal);
    printf("Tax: %.2f\n", (*pr).tax);
    printf("Total: %.2f\n", (*pr).finalTotal);
    printf("============================\n");
}

// Save invoice to file
void saveFile(struct Prescription *pr) {
    FILE *fp = fopen("invoice.txt", "w");
    if (fp == NULL) {
        printf("File error.\n");
        return;
    }

    char buf[200];

    sprintf(buf, "Patient ID: %d\n", (*pr).pat.id);
    fputs(buf, fp);

    sprintf(buf, "Name: %s\n", (*pr).pat.name);
    fputs(buf, fp);

    sprintf(buf, "Age: %d\n", (*pr).pat.age);
    fputs(buf, fp);

    sprintf(buf, "Gender: %s\n", (*pr).pat.gender);
    fputs(buf, fp);

    sprintf(buf, "Phone: %s\n\n", (*pr).pat.phone);
    fputs(buf, fp);

    for (int i = 0; i < (*pr).count; i++) {
        sprintf(buf, "%d. %s | %.2f x %d = %.2f\n",
                i + 1,
                (*pr).medList[i].name,
                (*pr).medList[i].price,
                (*pr).medList[i].qty,
                (*pr).medList[i].total);
        fputs(buf, fp);
    }

    sprintf(buf, "\nSubtotal: %.2f\n", (*pr).subtotal);
    fputs(buf, fp);

    sprintf(buf, "Tax: %.2f\n", (*pr).tax);
    fputs(buf, fp);

    sprintf(buf, "Total: %.2f\n", (*pr).finalTotal);
    fputs(buf, fp);

    fclose(fp);

    printf("\nInvoice saved.\n");
}

// Main function
int main() {
    struct Prescription p;

    printf("=== Hospital Billing System ===\n");

    getPatient(&p.pat);
    addMedicines(&p);
    calculateBill(&p);
    printInvoice(&p);
    saveFile(&p);

    return 0;
}
