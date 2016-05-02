#include "tee_client_api.h"
#include "dx_cclib.h"

#include <stdio.h>

//dummy applet
/*static const uint8_t applet_uuid[] = {0xFF, 0xDE, 0xFA, 0x01, 0xDE, 0xFA, 0x02, 0xDE, \
                                      0xFA, 0x03, 0xDE, 0xFA,  0x04, 0xDE, 0xFA, 0xFF};*/
//keystore applet
static const uint8_t applet_uuid[] = {0x00, 0xDE, 0xFA, 0x01, 0xDE, 0xFA, 0x02, 0xDE, 0xFA, 0x03, 0xDE, 0xFA, 0x04, 0xDE, 0xFA, 0xFF};
#define CMD_TEST_APP_FW 0x00010001
#define CMD_BOOT_TARGET_GET_PUBLIC 0x08 /*!< Check the currently active boot target from userspace */
#define CMD_GET_SECURE_BOOT_STATUS 0x09 /*!< Determine the actual secure boot status of the device */

int main()
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Result ret;
    uint32_t retOrigin = 0;

    ret = DX_CclibInit();
    if (ret != TEEC_SUCCESS)
        return ret;

    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS)
        return ret;

    ret = TEEC_OpenSession(&context, &session,  (TEEC_UUID *)&applet_uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
        return ret;

    ret = TEEC_InvokeCommand(&session, CMD_BOOT_TARGET_GET_PUBLIC, NULL, &retOrigin);
    if (ret != TEEC_SUCCESS)
        return ret;


    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
    DX_CclibFini();

    return ret;
}
