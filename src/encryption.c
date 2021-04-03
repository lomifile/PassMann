#include <stdio.h>
#include <string.h>

#include "encryption.h"
#include "log.h"

int encrypt(const char *target_file, const char *source_file,
            const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])
{
    unsigned char buf_in[CHUNK_SIZE];
    unsigned char buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE *fp_t, *fp_s;
    unsigned long long out_len;
    size_t rlen;
    int eof;
    unsigned char tag;

    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    fwrite(header, 1, sizeof header, fp_t);
    do
    {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, rlen,
                                                   NULL, 0, tag);
        fwrite(buf_out, 1, (size_t)out_len, fp_t);
    } while (!eof);
    fclose(fp_t);
    fclose(fp_s);
    return 0;
}

int decrypt(const char *target_file, const char *source_file,
            const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])
{
    unsigned char buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char buf_out[CHUNK_SIZE];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE *fp_t, *fp_s;
    unsigned long long out_len;
    size_t rlen;
    int eof;
    int ret = -1;
    unsigned char tag;

    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0)
    {
        append_log(time_now(), "Incomplete header");
        goto ret; /* incomplete header */
    }
    do
    {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0)
        {
            append_log(time_now(), "Corrupted chunk");
            goto ret; /* corrupted chunk */
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !eof)
        {
            append_log(time_now(), "Premature end (end of file reached before the end of the stream)");
            goto ret; /* premature end (end of file reached before the end of the stream) */
        }
        fwrite(buf_out, 1, (size_t)out_len, fp_t);
    } while (!eof);

    ret = 0;
ret:
    fclose(fp_t);
    fclose(fp_s);
    return ret;
}

int check_db_file(char *filename)
{
    FILE *fptr;
    fptr = fopen(filename, "rb");

    if (fptr == NULL)
    {
        return -1;
    }
    fclose(fptr);
    return 0;
}