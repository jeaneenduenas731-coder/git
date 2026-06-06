#ifndef NN_DENOISE_API_H
#define NN_DENOISE_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum nn_mode {
    NN_MODE_DENOISE_M1 = 1,
    NN_MODE_DENOISE_M2 = 2,
    NN_MODE_DENOISE_M3 = 3
} nn_mode;

const char* nn_denoise_get_version();

void* nn_denoise_create(int sample_rate, int frame_len, nn_mode mode);
void nn_denoise_destroy(void* st);
void nn_denoise_process(void* st, const float* mic_buf, float* out_buf);

int nn_denoise_query_mem_size(int sample_rate, int frame_len, nn_mode mode);

int nn_denoise_query_scratch_size(int sample_rate, int frame_len, nn_mode mode);

void* nn_denoise_init(void* p_mem, void* p_scratch, int sample_rate, int frame_len, nn_mode mode);

// query sram memory to copy flash memory
int nn_denoise_query_mem_ram_size();

// copy flash memory into sram to improve performance
void nn_denoise_set_ram(void* st, void* p_mem);

int nn_set_max_suppress(void* st, float db_value);

// get current internal minimal suppression value, which is within [0, 1]
float nn_get_max_suppress(void* st);

#ifdef __cplusplus
}
#endif

#endif  // NN_DENOISE_API_H
