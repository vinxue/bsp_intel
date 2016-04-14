/*
 * Copyright 2014-2016 (c) Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "intelkeymaster_firmware_api.h"
#include "intelkeymaster_middleware_api.h"

#include <nativehelper/UniquePtr.h>
#include <cutils/properties.h>
#include <hardware/keymaster1.h>

char rsa_2048_modulus_str[] =
    "e0473e8ab8f2284feb9e742ff974"
    "8fa118ed98633c92f52aeb7a2ebe0d"
    "3be60329be766ad10eb6a515d0d2cf"
    "d9bea7930f0c306537899f7958cd3e"
    "85b01f8818524d312584a94b251e36"
    "25b54141edbfee198808e1bb97fc7c"
    "b49b9eaaaf68e9c98d7d0edc53bbc0"
    "fa0034356d6305fbbcc3c700140538"
    "6abbc873cb0f3ef7425f3d33df7b31"
    "5ae036d2a0b66afd47503b169bf36e"
    "3b5162515b715fda83deaf2c58aeb9"
    "abfb3097c3cc9dd9dbe5ef296c1761"
    "39028e8a671e63056d45f40188d2c4"
    "133490845de52c2534e9c6b2478c07"
    "bdae928823b62d066c7770f9f63f3d"
    "ba247f530844747be7aaa85d853b8b"
    "d244acec3de3c89ab46453ab4d24c3"
    "ac69";
char rsa_2048_private_str[] =
    "37784776a5f17698f5ac960dfb83a1"
    "b67564e648bd0597cf8ab8087186f2"
    "669c27a9ecbdd480f0197a80d07309"
    "e6c6a96f925331e57f8b4ac6f4d45e"
    "da45a23269c09fc428c07a4e6edf73"
    "8a15dec97fabd2f2bb47a14f20ea72"
    "fcfe4c36e01ada77bd137cd8d4da10"
    "bb162e94a4662971f175f985fa188f"
    "056cb97ee2816f43ab9d3747612486"
    "cda8c16196c30818a995ec85d38467"
    "791267b3bf21f273710a6925862576"
    "841c5b6712c12d4bd20a2f3299adb7"
    "c135da5e9515abda76e7caf2a3be80"
    "551d073b78bf1162c48ad2b7f4743a"
    "0238ee4d252f7d5e7e6533ccae64cc"
    "b39360075a2fd1e034ec3ae5ce9c40"
    "8ccbf0e25e4114021687b3dd4754ae"
    "81";
uint8_t rsa_2048_modulus[256];
uint8_t rsa_2048_private[256];

char rsa_1024_modulus_str[] =
    "c8d957613bb08ecdd426cdf92e86"
    "e8b4e8168c35e5dcc8ec32aad4ea77"
    "fa7c7de8d3d51f384f3c7b154f951f"
    "bbb179545737adbb4889d5bc0b2ed6"
    "84b2f7b752db24b9498bcc69fdd666"
    "656a800441f70acdd80d71fc8e5e15"
    "52084ed14f245f1641bcf5e6c76564"
    "31d489400e7d7d183e7819d6bbaced"
    "3bc84f1cb55dccfe17";
char rsa_1024_private_str[] =
    "5da5a0ed502471611d02fed4a951f3"
    "dcadbe1d9fdfce42cc868f4a790c79"
    "f963c7944571aa129fd38fcb2c6fc0"
    "56ee8859a63755f46b780196813f3a"
    "5e2bcc087d387738a9ae2f409c288a"
    "3f4fde2f2635e8d0b5540fcf716945"
    "5ba7060b3f9866c67696cab0ff5741"
    "998283450d40d85d5bd108578c2b8d"
    "b666e0427f2c05f9";
uint8_t rsa_1024_modulus[128];
uint8_t rsa_1024_private[128];

char rsa_512_modulus_str[] =
    "b9b426369321e30312ba2bf076a1"
    "e16f307082ac116efef04d3066239c"
    "f7d7aad873b0912f57490da2571252"
    "5688a7241d216b3b3ff2b230646750"
    "548c47445d";
char rsa_512_private_str[] =
    "89c3351e03647e2e477449286839"
    "8b2d9b37bc18be0e239e4d04fca0aa"
    "cfc9f33473762b2f40a9a27c1148a7"
    "5472aae168740b214e8e3b0006a466"
    "5951f9ab19";
uint8_t rsa_512_modulus[64];
uint8_t rsa_512_private[64];

char rsa_2048_e3_modulus_str[] =
    "c428b63bcc2f993365f5c26fd381"
    "0edfb9ef79b74e56309c4516600a95"
    "fe115e1e349007042936ccc88ae5e5"
    "eef535e1633ade4c2fc6dd8721368a"
    "f61644353a067903fb614b738c0a50"
    "3ea6c947346a433e4aba571b4436e9"
    "859e5c08ea4573342073c628f50bf3"
    "895eefd926c9e79c3563f6f0b326d7"
    "874b20d458ea4215dc38d3913297c0"
    "8d3d539bb99ea095c30b142164c3cd"
    "da30a28bed4e441d1f9a821d4dc951"
    "2ff7a71652d34526d0de3073a391b0"
    "1a0d5a1c21ae5690bca4442d2789a7"
    "d39517854f477829660e7f159ad3fe"
    "0d7898687b26b6a73ad7fa5a222ac1"
    "6ae29e2d7bb0e6c3ae52ea295afe51"
    "e01c280d15ed6870e770b274bb41a5"
    "d995";
char rsa_2048_e3_private_str[] =
    "82c5ced2881fbb77994e819fe256"
    "09ea7bf4fbcf898ecb12d8b9955c63"
    "feb63ebecdb55a02c6248885b1ee99"
    "49f8ce9642273edd752f3e5a16245c"
    "a40ed8237c0450ad5240dcf7b2b18a"
    "d46f30da22f182298726e4bcd8249b"
    "ae6992b09c2e4ccd6af7d970a35d4d"
    "063f4a90c4869a682397f9f5ccc48f"
    "af876b383b46d6b93c505cba836fdd"
    "cf6c58bef8e26dd3b5ddad8e926d01"
    "5e82a5e2b6c49d911d8aee949d19c9"
    "766ef7894cb45c351910524144145c"
    "3acb53c46116320ba5627d096feaaf"
    "1bae36dd7877fda42bb107598e679b"
    "ddc07b440e10736cd736efabaf90cd"
    "74d6a430c34e911dd95f8992b1b989"
    "d2672aae46d93b047b3fea9732bea5"
    "2273";

char rsa_3072_modulus_str[] =
    "bc7786c3f04ac98034d480ddfc4c"
    "9369c2070dff0662db6a50465e33ca"
    "c29f23a634c45670d8e58f33d8a35e"
    "5e9166ab9fb5fce592b030b0b4c830"
    "1d83d50d46ee7128874d6370b5638e"
    "bcc1d3a35ab5219298b8ff4b26b2cb"
    "9035db3f2f47c6b5a9da473d146654"
    "610da2960d881683258efdfb5edfaf"
    "d0d4253a7fe0f5eb6f1200bc4d62b9"
    "8396b4885ede67794639f0141dbea3"
    "930b98774dc59c40a43a38d89ff195"
    "70de2bf0b6c9ac84e7537aa959174a"
    "96c070b3d0d93f3fdb381e2b730246"
    "1074528aa2528a05a0539e7b41bd5f"
    "21e86845a15807498406b793262b23"
    "3905115da5c6702d5e90ae13b5a258"
    "b5e8024f5858afe68e2683af1db571"
    "9ba9b8469e2a5b3b367a6a0daaf4d6"
    "1c20234b63bb524efbc9917d220d8e"
    "98337484bd4734f17e146815fdbe70"
    "0b0d8b1d61a9469b7ab295583e648a"
    "f1820790e017a0de32ea3b16830d78"
    "0fe9456f191cae4209056ad80e5e69"
    "95b6fe7f930c32f45e9095fbade4be"
    "bc47ff73b58592fddb0117124f8012"
    "64067694c0a1821452d9";
char rsa_3072_private_str[] =
    "6d8adb9dcf70e8a5c80f2bc7daf977"
    "45252d8c890a61333f1be18aa777d6"
    "9f40cd3e265047cf09beae57e0bcdd"
    "228e167b0200383f868106744d570a"
    "10b228c16d9dfffc8492a3d40a45d0"
    "98a8366e669158efc3b73f2b030926"
    "8ce71e5690d582897301ad12ca5789"
    "01afda9f8ad5eaec4dac7a0aaee135"
    "f3d89fa3ec5183c24f28764a547397"
    "7465024e81de8ab2b2c2442ef5bb85"
    "a05bc21c461b87c6ecc055c8c15627"
    "003b71f20c4e66013e98310e43676b"
    "2ff751ccb638f918a609831e9a7aa3"
    "ddabf38baef115594b0d66c5e80bcb"
    "c6a48cec0e731d68a503f9adf948a8"
    "51b489cd9b845683148a76c67ca70c"
    "b63ebba3d81c1d95fce9ef4794f877"
    "8ae2debcb19697f8ef6f688555bbde"
    "dea188551bf142b1aa036db5741205"
    "6a44313bce9d08f100ac195dfdce36"
    "3b2dc17d2002b035c62a8954b8783e"
    "58e43eeb28b92bb4c3e685b1ea07d2"
    "67c769b5aa3d13caddba8f5ba4da56"
    "84852c5e6e1b349cc735fdf7d6fb73"
    "b6df336a82ad5f9024df59e8612724"
    "5c2e46d40eab03ca01";
uint8_t rsa_3072_modulus[384];
uint8_t rsa_3072_private[384];

char rsa_4096_modulus_str[] =
    "d3194fb9360f385ac6cd4d9c24c1"
    "355e0efa758e76d6fe105c5874cc51"
    "e97dca70ef62e48ac27bf0ea4e3a14"
    "2c7ac2e036a1b4aca784c1eb36e938"
    "0ad9437474ad3f7a8a6d3ead951b25"
    "715d4ab637620cad6bf17944528889"
    "bac58f0f1884043c51df99b063e4d6"
    "490e0982435c9a67ce4b78c757eb4a"
    "b3623fd275c3e50ed6e1de2618d70e"
    "f4c1dd3288dfb432c42a1ea211ba2c"
    "a71a81f4ee2a6f9e38961b6d246cee"
    "cf6db0076dc85042e46a23e9db7b64"
    "4bef773e5d59d6baf85439355877b9"
    "33826328326051f80616a2d0005a3e"
    "d0930cad1dca4593a57b55d060dadb"
    "019624dff0a16e046201fccd212989"
    "358a029ab3e1da0bf9ef0b07331a53"
    "72092397aa6e89ae0a464509f70b9d"
    "1e0f390d938d12a6818fc4856a5518"
    "5e8fcf6998a2ad443588dd9fb51874"
    "09d2dc0e908a306dfa5d5e0b8ae5ae"
    "cc58ee2a0ad3ef9896920006cfd4ce"
    "3748a9b10db4e697a9bc4958a3d4df"
    "30791afd99dd9145388860d56b7860"
    "e0f1f0a2fd1b477a4db58d57e33132"
    "8f41aa0f70c9d45ec6e0004f780a14"
    "731acfbc9cb0614ee7a56f82e11188"
    "16c5d571b5a5dba93fade68c5e5f6f"
    "3b67927f9afdc462284aee993cb93a"
    "de9cb2e769790364b6cade4b44b5ab"
    "dc320ad32a4406bf2f9d2e49ae7e3a"
    "8ecae534c4e77d94e1db339e06e202"
    "2ff2e4395f2fdd91b269630e020064"
    "1e262299fcf1bace9b8e716421ea17"
    "6bdb1b";
char rsa_4096_private_str[] =
    "8117306e95857733e3f0b66e2ef7"
    "cbf7e9f6f59e9a43dcf80a50b9941c"
    "df49625befb82d0441ec6326159264"
    "b5d2988b5e3d42ec7640b4096ff9ec"
    "a08dbe37600676142cc4a398a8df01"
    "e5b26436cbe574d53476656f077da1"
    "a40a9df2604e92fc8420d5ae4d582a"
    "4660842619a56e72de6fbebb40a38d"
    "79e7321a349407e62c37d18ec08ffb"
    "0067f129af1405f1298e8c4f15c220"
    "fe4b327fa28966da7dc6259d77b1f9"
    "d3f542b938444ff8a80fd44875449e"
    "cfe4efa9be926ac2c98b4fb4b495fa"
    "366a9cd27f03f1fc5754ce599e6d91"
    "8e20ee968aea1fe42b4521f41cd699"
    "28fe352c90cd5726ecfc4428f9079a"
    "7598210193d1207344730eee672cc9"
    "2ca917870f91eceb10bc686e53a6df"
    "08f81752d1948bb38174509594d608"
    "a43c400be0793de714e24a45da2079"
    "27ac1918e2b5b02b7fa01b19e93a7d"
    "f6c4b475aca514eaf0db587a26fea6"
    "ac29e90e4063cc18ad5176effd8710"
    "4f61affa3efce6ac4a0e31fb758fa1"
    "f94b2c8a21d4106a29affe50dc33b7"
    "8db2a00303fcb44ddbf35154a84761"
    "79e24521f7884445cfed3f3f286cd7"
    "7220dac73e4fd91ab049b03326538e"
    "865f2d93d771f971b1d6e9b687e5bc"
    "d03e7d14967eaab6c9c1a0593d8462"
    "ec036b106b77ea39c66d257656936c"
    "8ab0d3c5cef7b387ac68e0bcb1f043"
    "63fd6ddc572774634efbca9f7f1b8d"
    "482098c092f8da12923e8ac1c688fa"
    "ed3961";
uint8_t rsa_4096_modulus[512];
uint8_t rsa_4096_private[512];

void log_buf(const char *prompt, uint8_t buf[], uint32_t size)
{
    char s[128];
    int len;
    uint32_t i, j;

    printf("%s\n", prompt);

    for (i=0; i<size; i+=16) {
        if (i + 15 < size) {
            printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                  buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7],
                  buf[i+8], buf[i+9], buf[i+10], buf[i+11], buf[i+12], buf[i+13], buf[i+14], buf[i+15]);
        }
        else {
            len = 0;
            for (j = i; j<size; j++) {
                len += snprintf(s + len, 128 - len, "%02x ", buf[j]);
            }
            printf("%s\n", s);
        }
    }
}

int intel_keymaster_generate_rsa_keypair(const keymaster_rsa_keygen_params_t* rsa_params,
                                         uint8_t** keyBlob, size_t* keyBlobLength)
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    intel_keymaster_keygen_params_t *keygen_params;
    intel_keymaster_rsa_keygen_params_t *rsa_keygen_params;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;
    intel_keymaster_key_blob_t *key_blob;
    uint32_t rsp_result_and_data_length;

    if ((rsa_params == NULL) || (keyBlob == NULL) || (keyBlobLength == NULL)) {
        return -1;
    }

    if (cmd_buf.get() == NULL) {
        return -1;
    }

    if (rsp_buf.get() == NULL) {
        return -1;
    }

    keygen_params = (intel_keymaster_keygen_params_t *)cmd_buf->cmd_data;
    keygen_params->key_type = KEY_TYPE_RSA;
    rsa_keygen_params = (intel_keymaster_rsa_keygen_params_t *)keygen_params->key_params;
    rsa_keygen_params->modulus_size = rsa_params->modulus_size;
    rsa_keygen_params->reserved = 0;
    rsa_keygen_params->public_exponent = rsa_params->public_exponent;
    cmd_buf->cmd_id = KEYMASTER_CMD_GENERATE_KEYPAIR;
    cmd_buf->cmd_data_length = sizeof(intel_keymaster_rsa_keygen_params_t) +
        offsetof(intel_keymaster_keygen_params_t, key_params);
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return -1;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return -1;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->rsp_result_and_data_length < sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length too small, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return -1;
    }

    key_blob = (intel_keymaster_key_blob_t *)rsp_buf->rsp_data;

    if (key_blob->key_blob_length == 0) {
        printf("key_blob_length is 0\n");
        return -1;
    }

    rsp_result_and_data_length = key_blob->key_blob_length +
        offsetof(intel_keymaster_key_blob_t, key_blob) + sizeof(intel_keymaster_result_t);
    if (rsp_result_and_data_length != rsp_buf->rsp_result_and_data_length) {
        printf("calculated rsp_result_and_data_length %u does not match rsp_result_and_data_length "
              "%u in structure\n",
              rsp_result_and_data_length, rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    if (rsp_buf->rsp_result_and_data_length + offsetof(intel_keymaster_firmware_rsp_t, result) !=
        rsp_len) {
        printf("rsp_result_and_data_length %u is inconsistent with rsp_len %u\n",
              rsp_buf->rsp_result_and_data_length, rsp_len);
        return -1;
    }

    //log_buf("key blob", key_blob->key_blob, key_blob->key_blob_length);

    *keyBlobLength = key_blob->key_blob_length;
    /* freeing memory allocated here is the responsibility of the caller several layers above */
    *keyBlob = (uint8_t *)malloc(key_blob->key_blob_length);
    if (*keyBlob == NULL) {
        printf("memory allocation for keyBlob failed\n");
        return -1;
    }

    memcpy(*keyBlob, key_blob->key_blob, key_blob->key_blob_length);

    return 0;
}

int intel_keymaster_import_rsa_keypair(uint32_t modulus_length, uint8_t modulus[], uint8_t private_exp[],
                                       uint32_t public_exponent_length, uint8_t public_exp[],
                                       uint8_t** keyBlob, size_t* keyBlobLength)
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    intel_keymaster_import_key_t *import_params;
    intel_keymaster_rsa_key_t *rsa_key;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;
    intel_keymaster_key_blob_t *key_blob;
    uint32_t rsp_result_and_data_length;
    uint32_t private_exponent_length;

    if ((keyBlob == NULL) || (keyBlobLength == NULL)) {
        return -1;
    }

    if (cmd_buf.get() == NULL) {
        return -1;
    }

    if (rsp_buf.get() == NULL) {
        return -1;
    }

    private_exponent_length = modulus_length;

    cmd_buf->cmd_id = KEYMASTER_CMD_IMPORT_KEYPAIR;
    cmd_buf->cmd_data_length = modulus_length + public_exponent_length + private_exponent_length +
        offsetof(intel_keymaster_rsa_key_t, bytes) +
        offsetof(intel_keymaster_import_key_t, key_data);
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    if (cmd_len > INTEL_KEYMASTER_MAX_MESSAGE_SIZE) {
        printf("combination of modulus, public exponent and private exponent length is too big,"
              " modulus_length = %u, public_exponent_length = %u, private_exponent_length = %u\n",
              modulus_length, public_exponent_length, private_exponent_length);
        return -1;
    }

    import_params = (intel_keymaster_import_key_t *)cmd_buf->cmd_data;
    import_params->key_type = KEY_TYPE_RSA;
    rsa_key = (intel_keymaster_rsa_key_t *)import_params->key_data;
    rsa_key->modulus_length = modulus_length;
    rsa_key->public_exponent_length = public_exponent_length;
    rsa_key->private_exponent_length = private_exponent_length;
    memcpy(rsa_key->bytes, modulus, modulus_length);
    memcpy(rsa_key->bytes + modulus_length, public_exp, public_exponent_length);
    memcpy(rsa_key->bytes + modulus_length + public_exponent_length, private_exp, private_exponent_length);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return -1;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return -1;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->rsp_result_and_data_length < sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length too small, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return -1;
    }

    key_blob = (intel_keymaster_key_blob_t *)rsp_buf->rsp_data;

    if (key_blob->key_blob_length == 0) {
        printf("key_blob_length is 0\n");
        return -1;
    }

    rsp_result_and_data_length = key_blob->key_blob_length +
        offsetof(intel_keymaster_key_blob_t, key_blob) + sizeof(intel_keymaster_result_t);
    if (rsp_result_and_data_length != rsp_buf->rsp_result_and_data_length) {
        printf("calculated rsp_result_and_data_length %u does not match rsp_result_and_data_length "
              "%u in structure\n",
              rsp_result_and_data_length, rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    if (rsp_buf->rsp_result_and_data_length + offsetof(intel_keymaster_firmware_rsp_t, result) !=
        rsp_len) {
        printf("rsp_result_and_data_length %u is inconsistent with rsp_len %u\n",
              rsp_buf->rsp_result_and_data_length, rsp_len);
        return -1;
    }

    //log_buf("key blob", key_blob->key_blob, key_blob->key_blob_length);

    *keyBlobLength = key_blob->key_blob_length;
    /* freeing memory allocated here is the responsibility of the caller several layers above */
    *keyBlob = (uint8_t *)malloc(key_blob->key_blob_length);
    if (*keyBlob == NULL) {
        printf("memory allocation for keyBlob failed\n");
        return -1;
    }

    memcpy(*keyBlob, key_blob->key_blob, key_blob->key_blob_length);

    return 0;
}

int intel_keymaster_get_public_key(const uint8_t* keyBlob, const size_t keyBlobLength)
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    intel_keymaster_key_blob_t *key_blob;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;
    intel_keymaster_public_key_t *public_key;
    intel_keymaster_rsa_public_key_t *rsa_public_key;
    uint32_t rsp_result_and_data_length;

    if ((keyBlob == NULL) || (keyBlobLength == 0)) {
        return 0;
    }

    if (cmd_buf.get() == NULL) {
        return 0;
    }

    if (rsp_buf.get() == NULL) {
        return 0;
    }

    cmd_buf->cmd_id = KEYMASTER_CMD_GET_KEYPAIR_PUBLIC;
    cmd_buf->cmd_data_length = keyBlobLength + offsetof(intel_keymaster_key_blob_t, key_blob);
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    if (cmd_len > INTEL_KEYMASTER_MAX_MESSAGE_SIZE) {
        printf("keyBlob is too big, keyBlobLength = %zu\n", keyBlobLength);
        return 0;
    }

    key_blob = (intel_keymaster_key_blob_t *)cmd_buf->cmd_data;
    key_blob->key_blob_length = keyBlobLength;
    memcpy(key_blob->key_blob, keyBlob, keyBlobLength);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        //printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return 0;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return 0;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->rsp_result_and_data_length < sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length too small, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return 0;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        //printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return 0;
    }

    public_key = (intel_keymaster_public_key_t *)rsp_buf->rsp_data;

    if (public_key->key_type != KEY_TYPE_RSA) {
        printf("key type other than RSA is not yet supported\n");
        return 0;
    }

    rsa_public_key = (intel_keymaster_rsa_public_key_t *)public_key->public_key_data;

    rsp_result_and_data_length = rsa_public_key->modulus_length +
        rsa_public_key->public_exponent_length +
        offsetof(intel_keymaster_rsa_public_key_t, bytes) +
        offsetof(intel_keymaster_public_key_t, public_key_data) +
        sizeof(intel_keymaster_result_t);
    if (rsp_result_and_data_length != rsp_buf->rsp_result_and_data_length) {
        printf("calculated rsp_result_and_data_length %u does not match rsp_result_and_data_length "
              "%u in structure\n",
              rsp_result_and_data_length, rsp_buf->rsp_result_and_data_length);
        return 0;
    }

    if (rsp_buf->rsp_result_and_data_length + offsetof(intel_keymaster_firmware_rsp_t, result) !=
        rsp_len) {
        printf("rsp_result_and_data_length %u is inconsistent with rsp_len %u\n",
              rsp_buf->rsp_result_and_data_length, rsp_len);
        return 0;
    }

    //log_buf("modulus", rsa_public_key->bytes, rsa_public_key->modulus_length);
    log_buf("public exponent", rsa_public_key->bytes + rsa_public_key->modulus_length, rsa_public_key->public_exponent_length);

    return rsa_public_key->modulus_length;
}

int intel_keymaster_sign_rsa(const uint8_t* keyBlob, const size_t keyBlobLength,
                             const uint8_t* data, const size_t dataLength,
                             uint8_t** signedData, size_t* signedDataLength)
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    intel_keymaster_signing_cmd_data_t *signing_data;
    intel_keymaster_signature_t *signature;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;
    uint32_t rsp_result_and_data_length;

    if ((keyBlob == NULL) || (data == NULL) || (signedData == NULL) || (signedDataLength == NULL))
    {
        return -1;
    }

    if ((keyBlobLength == 0) || (dataLength == 0)) {
        return -1;
    }

    if (cmd_buf.get() == NULL) {
        return -1;
    }

    if (rsp_buf.get() == NULL) {
        return -1;
    }

    cmd_buf->cmd_id = KEYMASTER_CMD_SIGN_DATA;
    cmd_buf->cmd_data_length = keyBlobLength + dataLength +
        offsetof(intel_keymaster_signing_cmd_data_t, buffer);
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    if (cmd_len > INTEL_KEYMASTER_MAX_MESSAGE_SIZE) {
        printf("combination of keyblob and data length is too big, keyBlobLength = %zu, "
              "dataLength = %zu\n",
              keyBlobLength, dataLength);
        return -1;
    }

    signing_data = (intel_keymaster_signing_cmd_data_t *)cmd_buf->cmd_data;
    signing_data->key_blob_length = keyBlobLength;
    signing_data->data_length = dataLength;
    memcpy(signing_data->buffer, keyBlob, keyBlobLength);
    memcpy(signing_data->buffer + keyBlobLength, data, dataLength);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        //printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return -1;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return -1;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->rsp_result_and_data_length < sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length too small, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        //printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return -1;
    }

    signature = (intel_keymaster_signature_t *)rsp_buf->rsp_data;

    if (signature->signature_length == 0) {
        printf("signature_length is 0\n");
        return -1;
    }

    rsp_result_and_data_length = signature->signature_length +
        offsetof(intel_keymaster_signature_t, signature) + sizeof(intel_keymaster_result_t);
    if (rsp_result_and_data_length != rsp_buf->rsp_result_and_data_length) {
        printf("calculated rsp_result_and_data_length %u does not match rsp_result_and_data_length "
              "%u in structure\n",
              rsp_result_and_data_length, rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    if (rsp_buf->rsp_result_and_data_length + offsetof(intel_keymaster_firmware_rsp_t, result) !=
        rsp_len) {
        printf("rsp_result_and_data_length %u is inconsistent with rsp_len %u\n",
              rsp_buf->rsp_result_and_data_length, rsp_len);
        return -1;
    }

    //log_buf("signature", signature->signature, signature->signature_length);

    *signedDataLength = signature->signature_length;
    /* freeing memory allocated here is the responsibility of the caller several layers above */
    *signedData = (uint8_t *)malloc(signature->signature_length);
    if (*signedData == NULL) {
        printf("memory allocation for signedData failed\n");
        return -1;
    }

    memcpy(*signedData, signature->signature, signature->signature_length);

    return 0;
}

int intel_keymaster_verify_rsa(const uint8_t* keyBlob, const size_t keyBlobLength,
                               const uint8_t* signedData, const size_t signedDataLength,
                               const uint8_t* signature, const size_t signatureLength)
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    intel_keymaster_verification_data_t *verification_data;
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;

    if ((keyBlob == NULL) || (signedData == NULL) || (signature == NULL)) {
        return -1;
    }

    if ((keyBlobLength == 0) || (signedDataLength == 0) || (signatureLength == 0)) {
        return -1;
    }

    if (cmd_buf.get() == NULL) {
        return -1;
    }

    if (rsp_buf.get() == NULL) {
        return -1;
    }

    cmd_buf->cmd_id = KEYMASTER_CMD_VERIFY_DATA;
    cmd_buf->cmd_data_length = keyBlobLength + signedDataLength + signatureLength +
        offsetof(intel_keymaster_verification_data_t, buffer);
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    if (cmd_len > INTEL_KEYMASTER_MAX_MESSAGE_SIZE) {
        printf("combination of keyBlobLength, signedDataLength and signatureLength is too big, "
              "keyBlobLength = %zu, signedDataLength = %zu, signatureLength = %zu\n",
              keyBlobLength, signedDataLength, signatureLength);
        return -1;
    }

    verification_data = (intel_keymaster_verification_data_t *)cmd_buf->cmd_data;
    verification_data->key_blob_length = keyBlobLength;
    verification_data->data_length = signedDataLength;
    verification_data->signature_length = signatureLength;
    memcpy(verification_data->buffer, keyBlob, keyBlobLength);
    memcpy(verification_data->buffer + keyBlobLength, signedData, signedDataLength);
    memcpy(verification_data->buffer + keyBlobLength + signedDataLength, signature,
           signatureLength);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        //printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return -1;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return -1;
    }

    // rsp_result_and_data_length should include the result field only
    if (rsp_buf->rsp_result_and_data_length != sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length unexpected, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return -1;
    }

    return 0;
}

#define h2b(x)	(((x) >= 'a' && (x) <= 'f') ? ((x) - 'a' + 0xa) : ((x) - '0'))

unsigned char hex_to_bin(char hex[2])
{
	return ((h2b(hex[0]) << 4) | h2b(hex[1]));
}

int test_key_blob_use(uint8_t *key_blob, uint32_t key_blob_length)
{
    uint32_t modulus_length;
    uint8_t data[512];
    uint8_t *sig = NULL;
    size_t sig_len;
    int ret = -1;

    printf("Getting public key...\n");

    if ((modulus_length = intel_keymaster_get_public_key(key_blob, key_blob_length)) == 0) {
        fprintf(stderr, "intel_keymaster_get_public_key() failed\n");
        goto exit;
    }

    if (modulus_length > 512) {
        fprintf(stderr, "invalid modulus length %u returned\n", modulus_length);
        goto exit;
    }

    for (int i=0; i<512; i++) {
        data[i] = i;
    }

    printf("Signing data...\n");

    if (intel_keymaster_sign_rsa(key_blob, key_blob_length, data, modulus_length, &sig, &sig_len) < 0) {
        fprintf(stderr, "intel_keymaster_get_public_key() failed\n");
        goto exit;
    }

    if (sig_len != modulus_length) {
        fprintf(stderr, "invalid signature length %zu returned\n", sig_len);
        goto exit;
    }

    printf("Verifying signature...\n");

    if (intel_keymaster_verify_rsa(key_blob, key_blob_length, data, modulus_length, sig, sig_len) < 0) {
        fprintf(stderr, "intel_keymaster_verify_rsa() failed\n");
        goto exit;
    }

    // flip a bit in signature
    sig[0] ^= 1;

    printf("Flipped a bit in signature and verifying again (expecting failure)...\n");

    // expect verification to fail
    if (intel_keymaster_verify_rsa(key_blob, key_blob_length, data, modulus_length, sig, sig_len) == 0) {
        fprintf(stderr, "signature verification succeeded instead of failed\n");
        goto exit;
    }

    sig[0] ^= 1;

    // flip a bit in key blob
    key_blob[15] ^= 1;

    printf("Flipped a bit in key blob and getting public key again (expecting failure)...\n");

    if (intel_keymaster_get_public_key(key_blob, key_blob_length) != 0) {
        fprintf(stderr, "unexpected success return of intel_keymaster_get_public_key()\n");
        goto exit;
    }

    printf("Flipped a bit in key blob and signing data again (expecting failure)...\n");

    if (intel_keymaster_sign_rsa(key_blob, key_blob_length, data, modulus_length, &sig, &sig_len) == 0) {
        fprintf(stderr, "unexpected success return of intel_keymaster_get_public_key()\n");
        goto exit;
    }

    printf("Flipped a bit in key blob and verifying signature again (expecting failure)...\n");

    if (intel_keymaster_verify_rsa(key_blob, key_blob_length, data, modulus_length, sig, sig_len) == 0) {
        fprintf(stderr, "unexpected success return of intel_keymaster_verify_rsa()\n");
        goto exit;
    }

    ret = 0;

    exit:

    if (sig) {
        free(sig);
    }

    return ret;
}

int intel_keymaster_delete_all()
{
    UniquePtr<intel_keymaster_firmware_cmd_t> cmd_buf(
       static_cast<intel_keymaster_firmware_cmd_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    UniquePtr<intel_keymaster_firmware_rsp_t> rsp_buf(
       static_cast<intel_keymaster_firmware_rsp_t *>(malloc(INTEL_KEYMASTER_MAX_MESSAGE_SIZE)));
    uint32_t cmd_len, rsp_len;
    sep_keymaster_return_t sep_ret;

    if (cmd_buf.get() == NULL) {
        return -1;
    }

    if (rsp_buf.get() == NULL) {
        return -1;
    }

    cmd_buf->cmd_id = KEYMASTER_CMD_DELETE_ALL;
    cmd_buf->cmd_data_length = 0;
    cmd_len = cmd_buf->cmd_data_length + offsetof(intel_keymaster_firmware_cmd_t, cmd_data);

    rsp_len = INTEL_KEYMASTER_MAX_MESSAGE_SIZE;
    sep_ret = sep_keymaster_send_cmd((uint8_t *)cmd_buf.get(), cmd_len, (uint8_t *)rsp_buf.get(),
                                     &rsp_len);

    if (sep_ret != SEP_KEYMASTER_SUCCESS) {
        printf("sep_keymaster_send_cmd() returned error %d\n", sep_ret);
        return -1;
    }

    // response should contain at least rsp_id, rsp_result_and_data_length and result
    if (rsp_len < offsetof(intel_keymaster_firmware_rsp_t, rsp_data)) {
        printf("response is too short, rsp_len = %u\n", rsp_len);
        return -1;
    }

    // rsp_result_and_data_length should include at least the result field
    if (rsp_buf->rsp_result_and_data_length < sizeof(intel_keymaster_result_t)) {
        printf("rsp_result_and_data_length too small, rsp_result_and_data_length = %u\n",
              rsp_buf->rsp_result_and_data_length);
        return -1;
    }

    // the result must indicate success
    if (rsp_buf->result != KEYMASTER_RESULT_SUCCESS) {
        printf("firmware returned result is not successful, result = 0x%x\n", rsp_buf->result);
        return -1;
    }

    return 0;
}

int main()
{
    keymaster_rsa_keygen_params_t rsa_keygen_params;
    uint8_t *key_blob;
    size_t key_blob_length;
    uint8_t f4[3] = {0x01, 0x00, 0x01};
    uint8_t three[1] = {3};
    uint32_t i;

    printf("testing generated key size = 2048 bits...\n");

    rsa_keygen_params.modulus_size = 2048;
    rsa_keygen_params.public_exponent = 0x10001;

    if (intel_keymaster_generate_rsa_keypair(&rsa_keygen_params, &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_generate_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing generated key size = 1024 bits...\n");

    rsa_keygen_params.modulus_size = 1024;
    rsa_keygen_params.public_exponent = 0x10001;

    if (intel_keymaster_generate_rsa_keypair(&rsa_keygen_params, &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_generate_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing generated key size = 512 bits...\n");

    rsa_keygen_params.modulus_size = 512;
    rsa_keygen_params.public_exponent = 0x10001;

    if (intel_keymaster_generate_rsa_keypair(&rsa_keygen_params, &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_generate_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing imported key size = 2048 bits...\n");

    if (strlen(rsa_2048_modulus_str) != 512) {
        fprintf(stderr, "unexpected length of rsa_2048_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_2048_private_str) != 512) {
        fprintf(stderr, "unexpected length of rsa_2048_private_str\n");
        goto exit;
    }

    for (i=0; i<512; i+=2) {
        rsa_2048_modulus[i/2] = hex_to_bin(&rsa_2048_modulus_str[i]);
        rsa_2048_private[i/2] = hex_to_bin(&rsa_2048_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(256, rsa_2048_modulus, rsa_2048_private,
                                           sizeof(f4), f4,
                                           &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_import_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing imported key size = 1024 bits...\n");

    if (strlen(rsa_1024_modulus_str) != 256) {
        fprintf(stderr, "unexpected length of rsa_1024_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_1024_private_str) != 256) {
        fprintf(stderr, "unexpected length of rsa_1024_private_str\n");
        goto exit;
    }

    for (i=0; i<256; i+=2) {
        rsa_1024_modulus[i/2] = hex_to_bin(&rsa_1024_modulus_str[i]);
        rsa_1024_private[i/2] = hex_to_bin(&rsa_1024_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(128, rsa_1024_modulus, rsa_1024_private,
                                           sizeof(f4), f4,
                                           &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_import_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing imported key size = 512 bits...\n");

    if (strlen(rsa_512_modulus_str) != 128) {
        fprintf(stderr, "unexpected length of rsa_512_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_512_private_str) != 128) {
        fprintf(stderr, "unexpected length of rsa_512_private_str\n");
        goto exit;
    }

    for (i=0; i<128; i+=2) {
        rsa_512_modulus[i/2] = hex_to_bin(&rsa_512_modulus_str[i]);
        rsa_512_private[i/2] = hex_to_bin(&rsa_512_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(64, rsa_512_modulus, rsa_512_private,
                                           sizeof(f4), f4,
                                           &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_import_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing generated key size = 2048 bits with alternative public exponent...\n");

    rsa_keygen_params.modulus_size = 2048;
    rsa_keygen_params.public_exponent = 3;

    if (intel_keymaster_generate_rsa_keypair(&rsa_keygen_params, &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_generate_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing imported key size = 2048 bits with alternative public exponent...\n");

    if (strlen(rsa_2048_e3_modulus_str) != 512) {
        fprintf(stderr, "unexpected length of rsa_2048_e3_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_2048_e3_private_str) != 512) {
        fprintf(stderr, "unexpected length of rsa_2048_e3_private_str\n");
        goto exit;
    }

    for (i=0; i<512; i+=2) {
        rsa_2048_modulus[i/2] = hex_to_bin(&rsa_2048_e3_modulus_str[i]);
        rsa_2048_private[i/2] = hex_to_bin(&rsa_2048_e3_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(256, rsa_2048_modulus, rsa_2048_private,
                                           sizeof(three), three,
                                           &key_blob, &key_blob_length) < 0) {
        fprintf(stderr, "intel_keymaster_import_rsa_keypair() failed\n");
        goto exit;
    }

    if (test_key_blob_use(key_blob, key_blob_length) < 0) {
        printf(">>> test failed!\n");
    }
    else {
        printf(">>> test passed!\n");
    }

    free(key_blob);

    printf("testing generated key size = 3072 bits...\n");

    rsa_keygen_params.modulus_size = 3072;
    rsa_keygen_params.public_exponent = 0x10001;

    if (intel_keymaster_generate_rsa_keypair(&rsa_keygen_params, &key_blob, &key_blob_length) == 0)
    {
        if (test_key_blob_use(key_blob, key_blob_length) < 0) {
            printf(">>> test failed!\n");
        }
        else {
            printf(">>> test passed!\n");
        }

        free(key_blob);
    }
    else {
        printf("key size of 3072 bits is not supported\n");
    }

    printf("testing imported key size = 3072 bits...\n");

    if (strlen(rsa_3072_modulus_str) != 768) {
        fprintf(stderr, "unexpected length of rsa_3072_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_3072_private_str) != 768) {
        fprintf(stderr, "unexpected length of rsa_3072_private_str\n");
        goto exit;
    }

    for (i=0; i<768; i+=2) {
        rsa_3072_modulus[i/2] = hex_to_bin(&rsa_3072_modulus_str[i]);
        rsa_3072_private[i/2] = hex_to_bin(&rsa_3072_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(384, rsa_3072_modulus, rsa_3072_private,
                                           sizeof(f4), f4,
                                           &key_blob, &key_blob_length) == 0)
    {
        if (test_key_blob_use(key_blob, key_blob_length) < 0) {
            printf(">>> test failed!\n");
        }
        else {
            printf(">>> test passed!\n");
        }

        free(key_blob);
    }
    else {
        printf("key size of 3072 bits is not supported\n");
        // if test of 3072 bits fails, no need to test 4096 bits
        goto exit;
    }

    printf("testing imported key size = 4096 bits...\n");

    if (strlen(rsa_4096_modulus_str) != 1024) {
        fprintf(stderr, "unexpected length of rsa_4096_modulus_str\n");
        goto exit;
    }

    if (strlen(rsa_4096_private_str) != 1024) {
        fprintf(stderr, "unexpected length of rsa_4096_private_str\n");
        goto exit;
    }

    for (i=0; i<1024; i+=2) {
        rsa_4096_modulus[i/2] = hex_to_bin(&rsa_4096_modulus_str[i]);
        rsa_4096_private[i/2] = hex_to_bin(&rsa_4096_private_str[i]);
    }

    if (intel_keymaster_import_rsa_keypair(512, rsa_4096_modulus, rsa_4096_private,
                                           sizeof(f4), f4,
                                           &key_blob, &key_blob_length) == 0)
    {
        if (test_key_blob_use(key_blob, key_blob_length) < 0) {
            printf(">>> test failed!\n");
        }
        else {
            printf(">>> test passed!\n");
        }

        free(key_blob);
    }
    else {
        printf("key size of 4096 bits is not supported\n");
    }

exit:
    //char platform[PATH_MAX] = {0};
    //property_get("ro.board.platform", platform, NULL);

    //if (strcmp(platform, "merrifield") == 0 ||
    //    strcmp(platform, "moorefield") == 0) {
    //    intel_keymaster_delete_all();
    //}

    return 0;
}
