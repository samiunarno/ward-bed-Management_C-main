#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* FIX: match bed_management.c (5 parameters) */
User* create_user(int id,
                  const char* username,
                  const char* password,
                  UserRole role,
                  UserStatus status) {

    User* user = (User*)malloc(sizeof(User));

    if (user) {
        user->id = id;

        strncpy(user->username, username, MAX_USERNAME - 1);
        user->username[MAX_USERNAME - 1] = '\0';

        strncpy(user->password, password, MAX_PASSWORD - 1);
        user->password[MAX_PASSWORD - 1] = '\0';

        /* FORCE SINGLE ADMIN SYSTEM */
        user->role = ROLE_ADMIN;

        user->status = status;

        user->failed_attempts = 0;
        user->locked_until = 0;
        user->last_login = 0;
        user->last_activity = time(NULL);

        user->security_question[0] = '\0';
        user->security_answer[0] = '\0';

        user->next = NULL;
    }

    return user;
}

/* Only one node exists */
void free_user_list(User* head) {
    if (head != NULL) {
        free(head);
    }
}

void display_user(const User* user) {
    printf("ID: %d, Username: %s, Role: Admin, Status: %s\n",
           user->id,
           user->username,
           user_status_to_string(user->status));
}

/* Role is always admin */
const char* user_role_to_string(UserRole role) {
    return "Admin";
}

UserRole user_string_to_role(const char* str) {
    return ROLE_ADMIN;
}

/* Status handling */
const char* user_status_to_string(UserStatus status) {
    switch(status) {
        case STATUS_PENDING: return "Pending";
        case STATUS_APPROVED: return "Approved";
        case STATUS_REJECTED: return "Rejected";
        default: return "Unknown";
    }
}

UserStatus user_string_to_status(const char* str) {
    if (strcmp(str, "Approved") == 0) return STATUS_APPROVED;
    if (strcmp(str, "Rejected") == 0) return STATUS_REJECTED;
    return STATUS_PENDING;
}

/* Single user search */
User* find_user_by_username(User* user, const char* username) {
    if (user && strcmp(user->username, username) == 0) {
        return user;
    }
    return NULL;
}

/* Password strength */
int is_password_strong(const char* password) {
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;

    if (strlen(password) < MIN_PASSWORD_LENGTH) return 0;

    for (int i = 0; password[i]; i++) {
        if (password[i] >= 'A' && password[i] <= 'Z') has_upper = 1;
        if (password[i] >= 'a' && password[i] <= 'z') has_lower = 1;
        if (password[i] >= '0' && password[i] <= '9') has_digit = 1;
        if (strchr("!@#$%^&*()_+-=[]{}|;:,.<>?", password[i])) has_special = 1;
    }

    return has_upper && has_lower && has_digit && has_special;
}

/* Account control */
int is_account_locked(User* user) {
    if (user->locked_until == 0) return 0;
    return time(NULL) < user->locked_until;
}

void lock_account(User* user) {
    user->locked_until = time(NULL) + 1800;
}

void unlock_account(User* user) {
    user->locked_until = 0;
    user->failed_attempts = 0;
}

void reset_failed_attempts(User* user) {
    user->failed_attempts = 0;
}

void increment_failed_attempts(User* user) {
    user->failed_attempts++;
    if (user->failed_attempts >= MAX_LOGIN_ATTEMPTS) {
        lock_account(user);
    }
}

void update_last_login(User* user) {
    user->last_login = time(NULL);
    user->failed_attempts = 0;
    user->locked_until = 0;
}

void update_last_activity(User* user) {
    user->last_activity = time(NULL);
}

int is_session_expired(User* user) {
    return (time(NULL) - user->last_activity) > SESSION_TIMEOUT;
}