#include "services/bed_management.h"
#include "services/report_service.h"
#include "utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 函数声明
void admin_menu(BedManagementSystem* system);      // 管理员菜单
void staff_menu(BedManagementSystem* system);      // 工作人员菜单
void registration_menu(BedManagementSystem* system); // 注册菜单
void admin_approval_menu(BedManagementSystem* system); // 管理员审批菜单

// 许可证验证功能（已注释）
// int verify_license() {
//     // 检查许可证文件是否存在
//     FILE* license = fopen("license.key", "r");
//     if (!license) {
//         printf("未找到许可证！请联系技术支持。\n");
//         return 0;
//     }
//     // 验证许可证密钥...
//     return 1;
// }

int main() {
    BedManagementSystem* system = create_system(); // 创建系统实例
    User* current_user = NULL;                     // 当前登录用户
    int choice;
    
    printf("========================================\n");
    printf("   医院床位管理系统\n");
    printf("========================================\n");
    
    while (1) {
        printf("\n========== 主菜单 ==========\n");
        printf("1. 登录\n");
        printf("2. 注册\n");
        printf("3. 退出\n");
        printf("请选择：");
        scanf("%d", &choice);
        getchar(); // 清除缓冲区中的换行符
        
        if (choice == 1) {
            char username[MAX_USERNAME], password[MAX_PASSWORD];
            printf("\n--- 登录 ---\n");
            printf("用户名：");
            fgets(username, MAX_USERNAME, stdin);
            username[strcspn(username, "\n")] = '\0';
            
            printf("密码：");
            fgets(password, MAX_PASSWORD, stdin);
            password[strcspn(password, "\n")] = '\0';
            
            // 验证用户身份
            if (authenticate(system, username, password, &current_user)) {
                // 检查账号是否已批准
                if (current_user->status != STATUS_APPROVED) {
                    printf("\n[!] 账号尚未批准，请等待管理员审核。\n");
                    continue;
                }
                
                printf("\n[+] 欢迎，%s（%s）！\n", current_user->username, 
                       user_role_to_string(current_user->role));
                
                // 根据角色进入不同菜单
                if (current_user->role == ROLE_ADMIN) {
                    admin_menu(system);
                } else if (current_user->role == ROLE_STAFF) {
                    staff_menu(system);
                }
            } else {
                printf("\n[!] 用户名或密码错误。\n");
            }
        } else if (choice == 2) {
            registration_menu(system);
        } else {
            printf("\n========================================\n");
            printf("   正在退出系统...\n");
            printf("========================================\n");
            free_system(system); // 释放系统资源
            printf("再见！\n");
            break;
        }
    }
    
    return 0;
}

// 用户注册菜单
void registration_menu(BedManagementSystem* system) {
    char username[MAX_USERNAME], password[MAX_PASSWORD], role[20];
    
    printf("\n--- 用户注册 ---\n");
    printf("用户名：");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = '\0';
    
    printf("密码：");
    fgets(password, MAX_PASSWORD, stdin);
    password[strcspn(password, "\n")] = '\0';
    
    printf("角色（Admin/Staff）：");
    fgets(role, sizeof(role), stdin);
    role[strcspn(role, "\n")] = '\0';
    
    // 调用注册函数
    if (register_user(system, username, password, role)) {
        printf("\n[+] 注册成功！等待管理员审核。\n");
    } else {
        printf("\n[!] 注册失败，用户名可能已存在。\n");
    }
}

// 管理员审批菜单：审核待注册用户
void admin_approval_menu(BedManagementSystem* system) {
    int count = get_pending_users_count(system);
    if (count == 0) {
        printf("\n[+] 没有待审核的注册申请。\n");
        return;
    }
    
    printf("\n--- 待审核用户 ---\n");
    User* pending = get_pending_users(system);
    User* current = pending;
    
    while (current != NULL) {
        display_user(current); // 显示用户信息
        char choice;
        printf("是否批准该用户？（y/n）：");
        scanf(" %c", &choice);
        getchar();
        
        if (choice == 'y' || choice == 'Y') {
            if (approve_user(system, current->username)) {
                printf("[+] 用户已批准。\n");
            } else {
                printf("[!] 批准用户失败。\n");
            }
        } else {
            printf("[-] 用户已拒绝。\n");
        }
        current = current->next;
    }
    
    free_user_list(pending); // 释放链表内存
}

// 管理员主菜单
void admin_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n========== 管理员菜单 ==========\n");
        printf("1. 添加病房\n");
        printf("2. 删除病房\n");
        printf("3. 添加床位\n");
        printf("4. 删除床位\n");
        printf("5. 接收病人\n");
        printf("6. 转院/转病房\n");
        printf("7. 病人出院\n");
        printf("8. 查看所有病房\n");
        printf("9. 查看所有病人\n");
        printf("10. 生成报表\n");
        printf("11. 导出报表\n");
        printf("12. 审核用户注册\n");
        printf("13. 退出登录\n");
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
                printf("病房类型（General/ICU/Private）：");
                fgets(type, MAX_WARD_TYPE, stdin);
                type[strcspn(type, "\n")] = '\0';
                printf("容量：");
                scanf("%d", &capacity);
                getchar();
                
                if (add_ward(system, name, type, capacity)) {
                    printf("[+] 病房添加成功。\n");
                } else {
                    printf("[!] 添加病房失败。\n");
                }
                break;
            }
            case 2: {
                int ward_id;
                printf("\n--- 删除病房 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                getchar();
                if (remove_ward(system, ward_id)) {
                    printf("[+] 病房删除成功。\n");
                } else {
                    printf("[!] 未找到该病房。\n");
                }
                break;
            }
            case 3: {
                int ward_id, bed_id;
                printf("\n--- 添加床位 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("床位ID：");
                scanf("%d", &bed_id);
                getchar();
                if (add_bed(system, ward_id, bed_id)) {
                    printf("[+] 床位添加成功。\n");
                } else {
                    printf("[!] 添加床位失败。\n");
                }
                break;
            }
            case 4: {
                int ward_id, bed_id;
                printf("\n--- 删除床位 ---\n");
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("床位ID：");
                scanf("%d", &bed_id);
                getchar();
                if (remove_bed(system, ward_id, bed_id)) {
                    printf("[+] 床位删除成功。\n");
                } else {
                    printf("[!] 未找到该床位。\n");
                }
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
                printf("病人ID（自定义）：");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("床位ID：");
                scanf("%d", &bed_id);
                getchar();
                
                Patient new_patient;
                new_patient.id = system->next_patient_id;      // 系统内唯一ID
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
                    printf("[!] 接收失败，床位可能已被占用或无效。\n");
                }
                break;
            }
            case 6: {
                int patient_id, new_ward_id, new_bed_id;
                printf("\n--- 转院/转病房 ---\n");
                printf("病人ID：");
                scanf("%d", &patient_id);
                printf("新病房ID：");
                scanf("%d", &new_ward_id);
                printf("新床位ID：");
                scanf("%d", &new_bed_id);
                getchar();
                if (transfer_patient(system, patient_id, new_ward_id, new_bed_id)) {
                    printf("[+] 病人转床成功。\n");
                } else {
                    printf("[!] 转床失败。\n");
                }
                break;
            }
            case 7: {
                int patient_id;
                printf("\n--- 病人出院 ---\n");
                printf("病人ID：");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("[+] 病人出院成功。\n");
                } else {
                    printf("[!] 出院失败，病人可能未入院。\n");
                }
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
                admin_approval_menu(system);
                break;
            case 13:
                printf("\n[+] 正在退出登录...\n");
                return;
            default:
                printf("[!] 无效选项，请重试。\n");
        }
    } while (1);
}

// 工作人员菜单
void staff_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n========== 工作人员菜单 ==========\n");
        printf("1. 查看所有病房\n");
        printf("2. 查看所有病人\n");
        printf("3. 接收病人\n");
        printf("4. 病人出院\n");
        printf("5. 查看报表\n");
        printf("6. 退出登录\n");
        printf("请选择：");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1:
                display_all_wards(system);
                break;
            case 2:
                display_all_patients(system);
                break;
            case 3: {
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
                printf("病人ID（自定义）：");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';
                printf("病房ID：");
                scanf("%d", &ward_id);
                printf("床位ID：");
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
            case 4: {
                int patient_id;
                printf("\n--- 病人出院 ---\n");
                printf("病人ID：");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("[+] 病人出院成功。\n");
                } else {
                    printf("[!] 出院失败。\n");
                }
                break;
            }
            case 5:
                generate_and_display_reports(system);
                break;
            case 6:
                printf("\n[+] 正在退出登录...\n");
                return;
            default:
                printf("[!] 无效选项，请重试。\n");
        }
    } while (1);
}