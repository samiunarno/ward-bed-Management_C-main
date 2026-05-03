#include "services/bed_management.h"
#include "services/report_service.h"
#include "utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 仅管理员菜单 */
void admin_menu(BedManagementSystem* system);

int main() {

    BedManagementSystem* system = create_system();

    printf("========================================\n");
    printf("   医院病床管理系统\n");
    printf("========================================\n");

    printf("\n[系统启动 - 直接管理员模式]\n");

    /* 直接进入（无菜单，无登录） */
    admin_menu(system);

    /* 清理 */
    free_system(system);

    printf("\n系统成功退出。\n");
    return 0;
}

/* ================= 管理员菜单 ================= */

void admin_menu(BedManagementSystem* system) {
    int choice;
    int running = 1;

    while (running) {

        printf("\n========== 管理员菜单 ==========\n");
        printf("1. 添加病房\n");
        printf("2. 删除病房\n");
        printf("3. 添加病床\n");
        printf("4. 删除病床\n");
        printf("5. 接收病人\n");
        printf("6. 转移病人\n");
        printf("7. 病人出院\n");
        printf("8. 查看所有病房\n");
        printf("9. 查看所有病人\n");
        printf("10. 生成报告\n");
        printf("11. 导出报告\n");
        printf("12. 退出系统\n");
        printf("请选择：");

        scanf("%d", &choice);
        getchar();

        switch (choice) {

            case 1: {
                char name[MAX_WARD_NAME], type[MAX_WARD_TYPE];
                int capacity;

                printf("\n--- 添加病房 ---\n");

                printf("病房名称：");
                fgets(name, MAX_WARD_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("病房类型（普通/重症监护室/私人）：");
                fgets(type, MAX_WARD_TYPE, stdin);
                type[strcspn(type, "\n")] = '\0';

                printf("容量：");
                scanf("%d", &capacity);
                getchar();

                if (add_ward(system, name, type, capacity))
                    printf("[+] 病房添加成功。\n");
                else
                    printf("[!] 添加病房失败。\n");

                break;
            }

            case 2: {
                int ward_id;

                printf("\n--- 删除病房 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                getchar();

                if (remove_ward(system, ward_id))
                    printf("[+] 病房删除成功。\n");
                else
                    printf("[!] 未找到病房。\n");

                break;
            }

            case 3: {
                int ward_id, bed_id;

                printf("\n--- 添加病床 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("病床ID：");
                scanf("%d", &bed_id);
                getchar();

                if (add_bed(system, ward_id, bed_id))
                    printf("[+] 病床添加成功。\n");
                else
                    printf("[!] 添加病床失败。\n");

                break;
            }

            case 4: {
                int ward_id, bed_id;

                printf("\n--- 删除病床 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("病床ID：");
                scanf("%d", &bed_id);
                getchar();

                if (remove_bed(system, ward_id, bed_id))
                    printf("[+] 病床删除成功。\n");
                else
                    printf("[!] 未找到病床。\n");

                break;
            }

            case 5: {
                char name[MAX_NAME], gender[MAX_GENDER], contact[MAX_CONTACT], patient_id[MAX_PATIENT_ID];
                int age, ward_id, bed_id;
                char current_date[11];

                get_current_date(current_date, sizeof(current_date));

                printf("\n--- 接收病人 ---\n");

                printf("病人姓名：");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("年龄：");
                scanf("%d", &age);
                getchar();

                printf("性别：");
                fgets(gender, MAX_GENDER, stdin);
                gender[strcspn(gender, "\n")] = '\0';

                printf("联系方式：");
                fgets(contact, MAX_CONTACT, stdin);
                contact[strcspn(contact, "\n")] = '\0';

                printf("病人ID号：");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';

                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("病床ID：");
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
                    printf("[+] 病人接收成功。\n");
                } else {
                    printf("[!] 接收失败。\n");
                }

                break;
            }

            case 6: {
                int patient_id, new_ward_id, new_bed_id;

                printf("\n--- 转移病人 ---\n");

                printf("病人ID：");
                scanf("%d", &patient_id);
                printf("新病房ID：");
                scanf("%d", &new_ward_id);
                printf("新病床ID：");
                scanf("%d", &new_bed_id);
                getchar();

                if (transfer_patient(system, patient_id, new_ward_id, new_bed_id))
                    printf("[+] 病人转移成功。\n");
                else
                    printf("[!] 转移失败。\n");

                break;
            }

            case 7: {
                int patient_id;

                printf("\n--- 病人出院 ---\n");
                printf("病人ID：");
                scanf("%d", &patient_id);
                getchar();

                if (discharge_patient(system, patient_id))
                    printf("[+] 病人出院成功。\n");
                else
                    printf("[!] 出院操作失败。\n");

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
                printf("\n[+] 系统正在关闭...\n");
                running = 0;
                break;

            default:
                printf("[!] 无效选项。\n");
        }
    }
}