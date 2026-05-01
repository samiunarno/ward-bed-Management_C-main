#ifndef USER_H
#define USER_H

#include <time.h>

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_SECURITY_Q 100
#define MAX_SECURITY_A 100
#define MIN_PASSWORD_LENGTH 8
#define MAX_LOGIN_ATTEMPTS 3
#define SESSION_TIMEOUT 900

/* Keep compatibility but only ADMIN is used */
typedef enum {
    ROLE_ADMIN,
    ROLE_STAFF   // kept ONLY for compatibility (not used logically)
} UserRole;

typedef enum {
    STATUS_PENDING,
    STATUS_APPROVED,
    STATUS_REJECTED
} UserStatus;

typedef struct User {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    UserRole role;   // always set to ROLE_ADMIN in practice
    UserStatus status;
    int failed_attempts;
    time_t locked_until;
    time_t last_login;
    time_t last_activity;
    char security_question[MAX_SECURITY_Q];
    char security_answer[MAX_SECURITY_A];
    struct User* next;
} User;

/* FIXED: restore original function signature expected by bed_management.c */
User* create_user(int id,
                  const char* username,
                  const char* password,
                  UserRole role,
                  UserStatus status);

void free_user_list(User* head);
void display_user(const User* user);

const char* user_role_to_string(UserRole role);
UserRole user_string_to_role(const char* str);

const char* user_status_to_string(UserStatus status);
UserStatus user_string_to_status(const char* str);

User* find_user_by_username(User* user, const char* username);

/* Security */
int is_password_strong(const char* password);
int is_account_locked(User* user);
void lock_account(User* user);
void unlock_account(User* user);
void reset_failed_attempts(User* user);
void increment_failed_attempts(User* user);
void update_last_login(User* user);
void update_last_activity(User* user);
int is_session_expired(User* user);

#endif