#include "services/bed_management.h"
#include "services/report_service.h"
#include "utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Admin menu only */
void admin_menu(BedManagementSystem* system);

int main() {

    BedManagementSystem* system = create_system();

    printf("========================================\n");
    printf("   HOSPITAL BED MANAGEMENT SYSTEM\n");
    printf("========================================\n");

    printf("\n[System started - Direct Admin Mode]\n");

    /* DIRECT ENTRY (NO MENU, NO LOGIN) */
    admin_menu(system);

    /* cleanup */
    free_system(system);

    printf("\nSystem exited successfully.\n");
    return 0;
}

/* ================= ADMIN MENU ================= */

void admin_menu(BedManagementSystem* system) {
    int choice;
    int running = 1;

    while (running) {

        printf("\n========== ADMIN MENU ==========\n");
        printf("1. Add Ward\n");
        printf("2. Remove Ward\n");
        printf("3. Add Bed\n");
        printf("4. Remove Bed\n");
        printf("5. Admit Patient\n");
        printf("6. Transfer Patient\n");
        printf("7. Discharge Patient\n");
        printf("8. View All Wards\n");
        printf("9. View All Patients\n");
        printf("10. Generate Reports\n");
        printf("11. Export Reports\n");
        printf("12. Exit System\n");
        printf("Choice: ");

        scanf("%d", &choice);
        getchar();

        switch (choice) {

            case 1: {
                char name[MAX_WARD_NAME], type[MAX_WARD_TYPE];
                int capacity;

                printf("\n--- ADD WARD ---\n");

                printf("Ward Name: ");
                fgets(name, MAX_WARD_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("Ward Type (General/ICU/Private): ");
                fgets(type, MAX_WARD_TYPE, stdin);
                type[strcspn(type, "\n")] = '\0';

                printf("Capacity: ");
                scanf("%d", &capacity);
                getchar();

                if (add_ward(system, name, type, capacity))
                    printf("[+] Ward added successfully.\n");
                else
                    printf("[!] Failed to add ward.\n");

                break;
            }

            case 2: {
                int ward_id;

                printf("\n--- REMOVE WARD ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                getchar();

                if (remove_ward(system, ward_id))
                    printf("[+] Ward removed successfully.\n");
                else
                    printf("[!] Ward not found.\n");

                break;
            }

            case 3: {
                int ward_id, bed_id;

                printf("\n--- ADD BED ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();

                if (add_bed(system, ward_id, bed_id))
                    printf("[+] Bed added successfully.\n");
                else
                    printf("[!] Failed to add bed.\n");

                break;
            }

            case 4: {
                int ward_id, bed_id;

                printf("\n--- REMOVE BED ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();

                if (remove_bed(system, ward_id, bed_id))
                    printf("[+] Bed removed successfully.\n");
                else
                    printf("[!] Bed not found.\n");

                break;
            }

            case 5: {
                char name[MAX_NAME], gender[MAX_GENDER], contact[MAX_CONTACT], patient_id[MAX_PATIENT_ID];
                int age, ward_id, bed_id;
                char current_date[11];

                get_current_date(current_date, sizeof(current_date));

                printf("\n--- ADMIT PATIENT ---\n");

                printf("Patient Name: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("Age: ");
                scanf("%d", &age);
                getchar();

                printf("Gender: ");
                fgets(gender, MAX_GENDER, stdin);
                gender[strcspn(gender, "\n")] = '\0';

                printf("Contact: ");
                fgets(contact, MAX_CONTACT, stdin);
                contact[strcspn(contact, "\n")] = '\0';

                printf("Patient ID: ");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';

                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();

                Patient new_patient;
                new_patient.id = system->next_patient_id;
                strcpy(new_patient.name, name);
                new_patient.age = age;
                strcpy(new_patient.gender, gender);
                strcpy(new_patient.contact, contact);
                strcpy(new_patient.patient_id, patient_id);
                new_patient.bed_id = 0;
                strcpy(new_patient.admission_date, current_date);
                new_patient.discharge_date[0] = '\0';

                if (admit_patient(system, &new_patient, ward_id, bed_id)) {
                    system->next_patient_id++;
                    printf("[+] Patient admitted successfully.\n");
                } else {
                    printf("[!] Admission failed.\n");
                }

                break;
            }

            case 6: {
                int patient_id, new_ward_id, new_bed_id;

                printf("\n--- TRANSFER PATIENT ---\n");

                printf("Patient ID: ");
                scanf("%d", &patient_id);
                printf("New Ward ID: ");
                scanf("%d", &new_ward_id);
                printf("New Bed ID: ");
                scanf("%d", &new_bed_id);
                getchar();

                if (transfer_patient(system, patient_id, new_ward_id, new_bed_id))
                    printf("[+] Patient transferred successfully.\n");
                else
                    printf("[!] Transfer failed.\n");

                break;
            }

            case 7: {
                int patient_id;

                printf("\n--- DISCHARGE PATIENT ---\n");
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                getchar();

                if (discharge_patient(system, patient_id))
                    printf("[+] Patient discharged successfully.\n");
                else
                    printf("[!] Discharge failed.\n");

                break;
            }

            case 8:
                display_all_wards(system);
                break;

            case 9:
                display_all_patients(system);
                break;

            case 10:
                generate_and_display_reports(system);
                break;

            case 11:
                export_all_reports(system);
                break;

            case 12:
                printf("\n[+] Shutting down system...\n");
                running = 0;
                break;

            default:
                printf("[!] Invalid choice.\n");
        }
    }
}