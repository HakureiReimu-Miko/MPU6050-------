#ifndef __PERMISSION_H
#define __PERMISSION_H
#include "sys.h"


#define PASSWORD_SIZE 20
typedef struct
{
    uint8_t user[20];
    uint8_t password[20];
    uint8_t loginFlag;
} PermissionTypeDef;

extern PermissionTypeDef manager;
extern PermissionTypeDef admin;

#define MANAGER_PASSWORD_IN_NORFLASH 0x000000
#define ADMIN_PASSWORD_IN_NORFLASH 0x000020


void permissionLogin(void);
void passwordModify(void);
void passwordLogin(uint16_t form_LoginJumpPage);
#endif