#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <inttypes.h>

#include <argon2.h>

#define HASHLEN 32
#define SALTLEN 16

const uint32_t t_cost = 2;            // 2-pass computation
const uint32_t m_cost = (1 << 10);    // 1 mebibyte memory usage
const uint32_t parallelism = 2;       // number of threads and lanes
const uint32_t pwdlen = 80;

void hash8_2_hash_swap(uint8_t *hash8, uint32_t *hash)
{
    memcpy (hash, hash8, 32);
    //byte order zmieniany do fulltest
    for (int i=0; i < 8; ++i)
    {
       hash[i] = swab32(hash[i]);
    }
}



// Same interface as scanhash_sha256d
// W SHA256 pierwszy blok hashowania jest niezależny od nonce, więc liczony jest przed iterowaniem nonca
int scanhash_argon2d(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
    uint32_t max_nonce, unsigned long *hashes_done)

{
    //later maybe salt should be globalized, and set in main of miner?
    uint8_t salt[SALTLEN] = {'\0'};

    uint32_t n = pdata[19] - 1;
    const uint32_t first_nonce = pdata[19];
    const uint32_t Htarg = ptarget[7];

    //Argon bierze parametry uint8_t, natomiast miner używa uint32_t
    uint8_t pwd [4*64] __attribute__((aligned(128)));
    uint8_t hash8 [HASHLEN] __attribute__((aligned(32)));
    uint32_t work_try;
    uint32_t hash [HASHLEN/4];

    memcpy(pwd, pdata, 80);

    do {
        //zwiększanie nonce'a - zmieniamy na 8bit
        //data[3] = ++n;
        //zakladam, ze najstarszy bajt w slowie 32b jest po lewej (najmniejszy adres)
        ++n;
        memcpy(pwd+76,&n,4);

        //hash - zmieniamy
        argon2d_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, salt, SALTLEN, hash8, HASHLEN);
        //sha256d_ms(hash, data, midstate, prehash);
        memcpy(&work_try, hash8 + 28, 4);
        if (swab32(work_try) <= Htarg) {
            //pdata[19] = data[3]; zmieniamy na:
            memcpy(pdata+19, pwd+76,4);
            //przed fulltest trzeba
            //sha256d_80_swap(hash, pdata);
            hash8_2_hash_swap(hash8,hash);
            if (fulltest(hash, ptarget)) {
                *hashes_done = n - first_nonce + 1;
                return 1;
            }
        }
    } while (n < max_nonce && !work_restart[thr_id].restart);

    *hashes_done = n - first_nonce + 1;
    pdata[19] = n;
    return 0;


}
