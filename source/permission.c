#include "permission.h"
#include "norflash.h"
#include <STRING.H>
#include "ui.h"
#include "debug.h"
#include "sys.h"

PermissionTypeDef aaa = {0};
PermissionTypeDef manager = {0};
PermissionTypeDef admin = {0};

static uint16_t loginJumpPage;         // 密码登录后跳转的页面
static uint16_t cancelLoginReturnPage; // 取消登录退出页面

void permissionLogin(void)
{
    uint16_t userNum;
    uint8_t passwordEnter[20];
    uint16_t loginButton;
    read_dgus_vp(0xAF00, (uint8_t *)&userNum, 1);
    read_dgus_vp(0xAF02, (uint8_t *)&passwordEnter, 9);
    read_dgus_vp(0xAF20, (uint8_t *)&loginButton, 1);
    if (loginButton == 1)
    {
        if (userNum == 0) // manager
        {
            Nor_Flash_read(MANAGER_PASSWORD_IN_NORFLASH, manager.password, 10);
            // DEBUGINFO("password : %s \n", manager.password);
            // DEBUGINFO("password : %s \n", passwordEnter);

            {
                uint8_t passwordLen;
                passwordLen = strpos(passwordEnter, '\xFF');

                if (strncmp(manager.password, passwordEnter, passwordLen) == 0)
                {
                    manager.loginFlag = 1;
                    // DEBUGINFO("%d\n", loginJumpPage);
                    switch (loginJumpPage)
                    {
                    case 2:
                    case 11:
                    case 22:
                    case 26:
                        Page_Change(loginJumpPage);
                        break;

                    default:
                        Page_Change(cancelLoginReturnPage); // 权限不够,返回先前页面
                        break;
                    }
                }
                else
                {
                    Page_Change(105);
                }
            }
        }
        if (userNum == 1) // admin
        {
            Nor_Flash_read(ADMIN_PASSWORD_IN_NORFLASH, admin.password, 10);
            {
                uint8_t passwordLen;
                passwordLen = strpos(passwordEnter, '\xFF');

                if (strncmp(admin.password, passwordEnter, passwordLen) == 0)
                {
                    admin.loginFlag = 1;
                    Page_Change(loginJumpPage);
                }
                else
                {
                    Page_Change(105);
                }
            }
        }
        loginButton = 0;
        write_dgus_vp(0xAF20, (uint8_t *)&loginButton, 1);
    }

    { // 取消登录
        uint16_t cancelButton;
        read_dgus_vp(0xAF22, (uint8_t *)&cancelButton, 1);
        if (cancelButton == 1)
        {
            Page_Change(cancelLoginReturnPage);
            cancelButton = 0;
            write_dgus_vp(0xAF22, (uint8_t *)&cancelButton, 1);
        }
    }
}

void passwordModify(void)
{
    uint16_t userNum;
    uint8_t oldPassword[20] = {0};
    uint8_t newPassword[20] = {0};
    uint16_t modifyButton;
    read_dgus_vp(0xAF50, (uint8_t *)&userNum, 1);
    read_dgus_vp(0xAF60, (uint8_t *)&oldPassword, 9);
    read_dgus_vp(0xAF70, (uint8_t *)&newPassword, 9);
    read_dgus_vp(0xAF80, (uint8_t *)&modifyButton, 1);
    if (modifyButton == 1)
    {
        if (userNum == 0) // manager
        {
            Nor_Flash_read(MANAGER_PASSWORD_IN_NORFLASH, manager.password, 10);
            // DEBUGINFO("manager password : %s \n", manager.password);
            // DEBUGINFO("oldPassword : %s \n", oldPassword);
            {
                uint8_t newPasswordLen = strpos(newPassword, '\xFF');
                if (newPasswordLen != -1)
                {
                    // DEBUGINFO("newPassword : %s \n", newPassword);
                    // DEBUGINFO("newPasswordLen = %bd", newPasswordLen);
                    if (newPasswordLen >= 4)
                    {
                        uint8_t oldPasswordLen;
                        oldPasswordLen = strpos(oldPassword, '\xFF');
                        if (oldPasswordLen != -1)
                        {
                            if (strncmp(manager.password, oldPassword, oldPasswordLen) == 0)
                            {
                                Nor_Flash_write(MANAGER_PASSWORD_IN_NORFLASH, newPassword, 10);
                                Page_Change(107);
                            }
                            else
                            {
                                Page_Change(106);
                            }
                        }
                    }
                    else if (newPasswordLen < 4)
                    {
                        Page_Change(108);
                    }
                }
            }
        }
        else if (userNum == 1) // admin
        {
            Nor_Flash_read(ADMIN_PASSWORD_IN_NORFLASH, admin.password, 10);
            {
                uint8_t newPasswordLen = strpos(newPassword, '\xFF');
                if (newPasswordLen != -1)
                {
                    if (newPasswordLen >= 4)
                    {
                        uint8_t oldPasswordLen;
                        oldPasswordLen = strpos(oldPassword, '\xFF');
                        if (newPassword != -1)
                        {
                            if (strncmp(admin.password, oldPassword, oldPasswordLen) == 0)
                            {
                                Nor_Flash_write(ADMIN_PASSWORD_IN_NORFLASH, newPassword, 10);
                                Page_Change(107);
                            }
                            else
                            {
                                Page_Change(106);
                            }
                        }
                    }
                    else if (newPasswordLen < 4)
                    {
                        Page_Change(108);
                    }
                }
            }
        }
        modifyButton = 0;
        write_dgus_vp(0xAF80, (uint8_t *)&modifyButton, 1);
    }
}

void passwordLogin(uint16_t form_LoginJumpPage)
{
    loginJumpPage = form_LoginJumpPage;
    read_dgus_vp(PIC_NOW, (uint8_t *)&cancelLoginReturnPage, 1);
    Page_Change(PASSWORD_LOGIN_PAGE);
}