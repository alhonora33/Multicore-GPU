#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <immintrin.h>
#include <starpu.h>


#define ELEMENT_TYPE float

#define DEFAULT_MESH_WIDTH 2000
#define DEFAULT_MESH_HEIGHT 1000
#define DEFAULT_NB_ITERATIONS 100
#define DEFAULT_NB_REPEAT 1

#define STENCIL_WIDTH 3
#define STENCIL_HEIGHT 3

#define TOP_BOUNDARY_VALUE 10
#define BOTTOM_BOUNDARY_VALUE 5
#define LEFT_BOUNDARY_VALUE -10
#define RIGHT_BOUNDARY_VALUE -5

#define MAX_DISPLAY_COLUMNS 20
#define MAX_DISPLAY_LINES 100

#define EPSILON 1e-3

static const ELEMENT_TYPE stencil_coefs[STENCIL_HEIGHT * STENCIL_WIDTH] =
    {
        0.25 / 3,  0.50 / 3, 0.25 / 3,
        0.50 / 3, -1.00,     0.50 / 3,
        0.25 / 3,  0.50 / 3, 0.25 / 3};

enum e_initial_mesh_type
{
        initial_mesh_zero = 1,
        initial_mesh_random = 2
};

struct s_settings
{
        int mesh_width;
        int mesh_height;
        enum e_initial_mesh_type initial_mesh_type;
        int nb_iterations;
        int nb_repeat;
        int enable_output;
        int enable_verbose;
};

#define PRINT_ERROR(MSG)                                                    \
        do                                                                  \
        {                                                                   \
                fprintf(stderr, "%s:%d - %s\n", __FILE__, __LINE__, (MSG)); \
                exit(EXIT_FAILURE);                                         \
        } while (0)

#define IO_CHECK(OP, RET)                   \
        do                                  \
        {                                   \
                if ((RET) < 0)              \
                {                           \
                        perror((OP));       \
                        exit(EXIT_FAILURE); \
                }                           \
        } while (0)

static void usage(void)
{
        fprintf(stderr, "usage: stencil [OPTIONS...]\n");
        fprintf(stderr, "    --mesh-width  MESH_WIDTH\n");
        fprintf(stderr, "    --mesh-height MESH_HEIGHT\n");
        fprintf(stderr, "    --initial-mesh <zero|random>\n");
        fprintf(stderr, "    --nb-iterations NB_ITERATIONS\n");
        fprintf(stderr, "    --nb-repeat NB_REPEAT\n");
        fprintf(stderr, "    --output\n");
        fprintf(stderr, "    --verbose\n");
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
}

static void init_settings(struct s_settings **pp_settings)
{
        assert(*pp_settings == NULL);
        struct s_settings *p_settings = calloc(1, sizeof(*p_settings));
        if (p_settings == NULL)
        {
                PRINT_ERROR("memory allocation failed");
        }
        p_settings->mesh_width = DEFAULT_MESH_WIDTH;
        p_settings->mesh_height = DEFAULT_MESH_HEIGHT;
        p_settings->initial_mesh_type = initial_mesh_zero;
        p_settings->nb_iterations = DEFAULT_NB_ITERATIONS;
        p_settings->nb_repeat = DEFAULT_NB_REPEAT;
        p_settings->enable_verbose = 0;
        p_settings->enable_output = 0;
        *pp_settings = p_settings;
}

static void parse_cmd_line(int argc, char *argv[], struct s_settings *p_settings)
{
        int i = 1;
        while (i < argc)
        {
                if (strcmp(argv[i], "--mesh-width") == 0)
                {
                        i++;
                        if (i >= argc)
                        {
                                usage();
                        }
                        int value = atoi(argv[i]);
                        if (value < STENCIL_WIDTH)
                        {
                                fprintf(stderr, "invalid MESH_WIDTH argument\n");
                                exit(EXIT_FAILURE);
                        }
                        p_settings->mesh_width = value;
                }
                else if (strcmp(argv[i], "--mesh-height") == 0)
                {
                        i++;
                        if (i >= argc)
                        {
                                usage();
                        }
                        int value = atoi(argv[i]);
                        if (value < STENCIL_HEIGHT)
                        {
                                fprintf(stderr, "invalid MESH_HEIGHT argument\n");
                                exit(EXIT_FAILURE);
                        }
                        p_settings->mesh_height = value;
                }
                else if (strcmp(argv[i], "--initial-mesh") == 0)
                {
                        i++;
                        if (i >= argc)
                        {
                                usage();
                        }
                        if (strcmp(argv[i], "zero") == 0)
                        {
                                p_settings->initial_mesh_type = initial_mesh_zero;
                        }
                        else if (strcmp(argv[i], "random") == 0)
                        {
                                p_settings->initial_mesh_type = initial_mesh_random;
                        }
                        else
                        {
                                fprintf(stderr, "invalid initial mesh type\n");
                                exit(EXIT_FAILURE);
                        }
                }
                else if (strcmp(argv[i], "--nb-iterations") == 0)
                {
                        i++;
                        if (i >= argc)
                        {
                                usage();
                        }
                        int value = atoi(argv[i]);
                        if (value < 1)
                        {
                                fprintf(stderr, "invalid NB_ITERATIONS argument\n");
                                exit(EXIT_FAILURE);
                        }
                        p_settings->nb_iterations = value;
                }
                else if (strcmp(argv[i], "--nb-repeat") == 0)
                {
                        i++;
                        if (i >= argc)
                        {
                                usage();
                        }
                        int value = atoi(argv[i]);
                        if (value < 1)
                        {
                                fprintf(stderr, "invalid NB_REPEAT argument\n");
                                exit(EXIT_FAILURE);
                        }
                        p_settings->nb_repeat = value;
                }
                else if (strcmp(argv[i], "--output") == 0)
                {
                        p_settings->enable_output = 1;
                }
                else if (strcmp(argv[i], "--verbose") == 0)
                {
                        p_settings->enable_verbose = 1;
                }
                else
                {
                        usage();
                }

                i++;
        }

        if (p_settings->enable_output)
        {
                p_settings->nb_repeat = 1;
                if (p_settings->nb_iterations > 100)
                {
                        p_settings->nb_iterations = 100;
                }
        }
}

static void delete_settings(struct s_settings **pp_settings)
{
        assert(*pp_settings != NULL);
        free(*pp_settings);
        pp_settings = NULL;
}

static void allocate_mesh(ELEMENT_TYPE **pp_mesh, struct s_settings *p_settings)
{
        assert(*pp_mesh == NULL);
        ELEMENT_TYPE *p_mesh = calloc(p_settings->mesh_width * p_settings->mesh_height, sizeof(*p_mesh));
        if (p_mesh == NULL)
        {
                PRINT_ERROR("memory allocation failed");
        }
        *pp_mesh = p_mesh;
}

static void delete_mesh(ELEMENT_TYPE **pp_mesh)
{
        assert(*pp_mesh != NULL);
        free(*pp_mesh);
        pp_mesh = NULL;
}

static void init_mesh_zero(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;
        for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
        {
                for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
                {
                        p_mesh[y * p_settings->mesh_width + x] = 0;
                }
        }
}

static void init_mesh_random(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;
        for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
        {
                for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
                {
                        ELEMENT_TYPE value = rand() / (ELEMENT_TYPE)RAND_MAX * 20 - 10;
                        p_mesh[y * p_settings->mesh_width + x] = value;
                }
        }
}
static void init_mesh_values(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        switch (p_settings->initial_mesh_type)
        {
        case initial_mesh_zero:
                init_mesh_zero(p_mesh, p_settings);
                break;

        case initial_mesh_random:
                init_mesh_random(p_mesh, p_settings);
                break;

        default:
                PRINT_ERROR("invalid initial mesh type");
        }
}

static void copy_mesh(ELEMENT_TYPE *p_dst_mesh, const ELEMENT_TYPE *p_src_mesh, struct s_settings *p_settings)
{
        memcpy(p_dst_mesh, p_src_mesh, p_settings->mesh_width * p_settings->mesh_height * sizeof(*p_dst_mesh));
}

static void apply_boundary_conditions(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;

        for (x = 0; x < p_settings->mesh_width; x++)
        {
                for (y = 0; y < margin_y; y++)
                {
                        p_mesh[y * p_settings->mesh_width + x] = TOP_BOUNDARY_VALUE;
                        p_mesh[(p_settings->mesh_height - 1 - y) * p_settings->mesh_width + x] = BOTTOM_BOUNDARY_VALUE;
                }
        }

        for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
        {
                for (x = 0; x < margin_x; x++)
                {
                        p_mesh[y * p_settings->mesh_width + x] = LEFT_BOUNDARY_VALUE;
                        p_mesh[y * p_settings->mesh_width + (p_settings->mesh_width - 1 - x)] = RIGHT_BOUNDARY_VALUE;
                }
        }
}

static void print_settings_csv_header(void)
{
        printf("mesh_width,mesh_height,nb_iterations,nb_repeat");
}

static void print_settings_csv(struct s_settings *p_settings)
{
        printf("%d,%d,%d,%d", p_settings->mesh_width, p_settings->mesh_height, p_settings->nb_iterations, p_settings->nb_repeat);
}

static void print_results_csv_header(void)
{
        printf("rep,timing,check_status");
}

static void print_results_csv(int rep, double timing_in_seconds, int check_status)
{
        printf("%d,%le,%d", rep, timing_in_seconds, check_status);
}

static void print_csv_header(void)
{
        print_settings_csv_header();
        printf(",");
        print_results_csv_header();
        printf("\n");
}

static void print_mesh(const ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        int x;
        int y;

        printf("[\n");
        for (y = 0; y < p_settings->mesh_height; y++)
        {
                if (y >= MAX_DISPLAY_LINES)
                {
                        printf("...\n");
                        break;
                }
                printf("[%03d: ", y);
                for (x = 0; x < p_settings->mesh_width; x++)
                {
                        if (x >= MAX_DISPLAY_COLUMNS)
                        {
                                printf("...");
                                break;
                        }
                        printf(" %+8.2lf", p_mesh[y * p_settings->mesh_width + x]);
                }
                printf("]\n");
        }
        printf("]");
}

static void write_mesh_to_file(FILE *file, const ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        int x;
        int y;
        int ret;

        for (y = 0; y < p_settings->mesh_height; y++)
        {
                for (x = 0; x < p_settings->mesh_width; x++)
                {
                        if (x > 0)
                        {
                                ret = fprintf(file, ",");
                                IO_CHECK("fprintf", ret);
                        }

                        ret = fprintf(file, "%lf", p_mesh[y * p_settings->mesh_width + x]);
                        IO_CHECK("fprintf", ret);
                }

                ret = fprintf(file, "\n");
                IO_CHECK("fprintf", ret);
        }
}


static void adapted_naive_stencil_func(const ELEMENT_TYPE *p_mesh, ELEMENT_TYPE *p_result_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;
        for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
        {
                for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
                {
                        ELEMENT_TYPE value = p_mesh[y * p_settings->mesh_width + x];
                        int stencil_x, stencil_y;
                        for (stencil_x = 0; stencil_x < STENCIL_WIDTH; stencil_x++)
                        {
                                for (stencil_y = 0; stencil_y < STENCIL_HEIGHT; stencil_y++)
                                {
                                        value +=
                                            p_mesh[(y + stencil_y - margin_y) * p_settings->mesh_width + (x + stencil_x - margin_x)] * stencil_coefs[stencil_y * STENCIL_WIDTH + stencil_x];
                                }
                        }
                        p_result_mesh[y * p_settings->mesh_width + x - 1] = value;
                }
        }
}
static void tile_stencil_func(void *buffer[], void *cl_arg)
{   
        ELEMENT_TYPE *p_tile = (ELEMENT_TYPE *)STARPU_MATRIX_GET_PTR(buffer[0]);

        ELEMENT_TYPE *p_tile_result = (ELEMENT_TYPE *)STARPU_MATRIX_GET_PTR(buffer[1]);
        struct s_settings p_settings;
        starpu_codelet_unpack_args(cl_arg, &p_settings) ;
        adapted_naive_stencil_func(p_tile, p_tile_result, &p_settings);
}
static void starpu_stencil_func(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;

        const int block_count = 1;
        const int tile_height = (p_settings->mesh_height - 2) / block_count;

        ELEMENT_TYPE *p_mesh_result = calloc(p_settings->mesh_height * p_settings->mesh_width, sizeof(float));
        
        int ret = starpu_init(NULL);
        STARPU_CHECK_RETURN_VALUE(ret, "starpu_init");

        struct starpu_codelet stencil_codelet = {
                .cpu_funcs = {tile_stencil_func},
		.nbuffers = 2,
		.modes = {STARPU_R}
        };


	starpu_data_handle_t* handles_read = malloc(block_count * sizeof(starpu_data_handle_t));
	starpu_data_handle_t* handles_write = malloc(block_count * sizeof(starpu_data_handle_t));

        

        
        struct s_settings settings_for_tile = {
                .mesh_width = p_settings->mesh_width,
                .mesh_height = tile_height + 2,
                .initial_mesh_type = p_settings->initial_mesh_type,
                .nb_iterations = p_settings->nb_iterations,
                .nb_repeat = p_settings->nb_repeat,
                .enable_output = p_settings->enable_output,
                .enable_verbose = p_settings->enable_verbose
        };




        int remaining_lines = (p_settings->mesh_height - 2) % block_count;
        for (int j = 0; j < block_count; j++) {
                
                int tile_size = (tile_height + 2) * p_settings->mesh_width;
                if((j == block_count - 1)){
                        tile_size += remaining_lines * p_settings->mesh_width;
                        settings_for_tile.mesh_height += (remaining_lines);
                }
                starpu_vector_data_register(handles_read + j, STARPU_MAIN_RAM, (uintptr_t)(p_mesh + (p_settings->mesh_width* (j * tile_height))), tile_size, sizeof(p_mesh[0]));
                starpu_vector_data_register(handles_write + j, STARPU_MAIN_RAM, (uintptr_t)(p_mesh_result + 1 + (p_settings->mesh_width* (j * tile_height))), tile_size - 2, sizeof(p_mesh_result[0]));


        }
        for (int i = 0; i < p_settings->nb_iterations; i++){

                for (int j = 0; j < block_count; j++) {

                        starpu_data_handle_t sub_handle_read = *(handles_read + j);
                        starpu_data_handle_t sub_handle_write = *(handles_write + j);
                 starpu_task_insert(&stencil_codelet, STARPU_R, sub_handle_read, STARPU_R, sub_handle_write,STARPU_VALUE,  &settings_for_tile, sizeof(settings_for_tile), 0);  
                }

                starpu_task_wait_for_all();

                for (int y = margin_y; y < p_settings->mesh_height - margin_y; y++)
                {
                        for (int x = margin_x; x < p_settings->mesh_width - margin_x; x++)
                        {
                                p_mesh[y * p_settings->mesh_width + x] = p_mesh_result[y * p_settings->mesh_width + x];
                        }
                }
        }

        for (int j = 0; j < block_count; j++) {
                starpu_data_unregister(handles_read[j]);
                starpu_data_unregister(handles_write[j]);
        }

        free(handles_read);
        free(handles_write);
        free(p_mesh_result);
        starpu_shutdown();

}


static void avx_stencil_func(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;

        ELEMENT_TYPE *p_temporary_mesh = calloc(p_settings->mesh_width * p_settings->mesh_height, p_settings->mesh_width * p_settings->mesh_height * sizeof(*p_mesh));

        __m256 side_mask =  _mm256_set_ps(0.25 / 3,  0.50 / 3,  0.25 / 3,  0,         0, 0.25 / 3,  0.50 / 3,  0.25 / 3);
        __m256 middle_mask = _mm256_set_ps(0.50 / 3,  0,  0.50 / 3,  0,        0, 0.50 / 3,  0,  0.50 / 3);
        for (y = 0; y < p_settings->mesh_height - margin_y; y++)
        {
                for (x = 0; x + 3 < p_settings->mesh_width ; x+=2)
                {
                int offset_col_1 = (y * p_settings->mesh_width + x);
                int offset_col_2 = ((y + 1)* p_settings->mesh_width + x);
                
                __m128 low1 = _mm_loadu_ps(p_mesh + offset_col_1);  
                __m256 avx1 = _mm256_set_m128(low1, low1); 

                __m128 low2 = _mm_loadu_ps(p_mesh + offset_col_2);  
                __m256 avx2 = _mm256_set_m128(low2, low2); 
                __m256 avx2_middle = avx2;
                avx1 = _mm256_mul_ps(avx1, side_mask);

                avx2 = _mm256_mul_ps(avx2, side_mask);
                avx2_middle = _mm256_mul_ps(avx2_middle, middle_mask);
                avx2_middle = _mm256_add_ps(avx2_middle, avx1);

                __m128 lower_lane = _mm256_castps256_ps128(avx2); 
                __m128 upper_lane = _mm256_extractf128_ps(avx2, 1); 
                lower_lane = _mm_hadd_ps(lower_lane, lower_lane); 
                upper_lane = _mm_hadd_ps(upper_lane, upper_lane); 
                lower_lane = _mm_hadd_ps(lower_lane, lower_lane); 
                upper_lane = _mm_hadd_ps(upper_lane, upper_lane); 

                p_temporary_mesh [offset_col_1 + 1] += _mm_cvtss_f32(lower_lane); 
                p_temporary_mesh [offset_col_1 + 2] += _mm_cvtss_f32(upper_lane); 

                lower_lane = _mm256_castps256_ps128(avx2_middle); 
                upper_lane = _mm256_extractf128_ps(avx2_middle, 1); 
                lower_lane = _mm_hadd_ps(lower_lane, lower_lane); 
                upper_lane = _mm_hadd_ps(upper_lane, upper_lane); 
                lower_lane = _mm_hadd_ps(lower_lane, lower_lane); 
                upper_lane = _mm_hadd_ps(upper_lane, upper_lane); 
                p_temporary_mesh [offset_col_2 + 1] += _mm_cvtss_f32(lower_lane); 
                p_temporary_mesh [offset_col_2 + 2] += _mm_cvtss_f32(upper_lane); 



                }
        }

        for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
        {
                for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
                {
                        p_mesh[y * p_settings->mesh_width + x] = p_temporary_mesh[y * p_settings->mesh_width + x];
                }
        }
        free(p_temporary_mesh);
}

static void naive_stencil_func(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        const int margin_x = (STENCIL_WIDTH - 1) / 2;
        const int margin_y = (STENCIL_HEIGHT - 1) / 2;
        int x;
        int y;

        ELEMENT_TYPE *p_temporary_mesh = malloc(p_settings->mesh_width * p_settings->mesh_height * sizeof(*p_mesh));
        for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
        {
                for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
                {
                        ELEMENT_TYPE value = p_mesh[y * p_settings->mesh_width + x];
                        int stencil_x, stencil_y;
                        for (stencil_x = 0; stencil_x < STENCIL_WIDTH; stencil_x++)
                        {
                                for (stencil_y = 0; stencil_y < STENCIL_HEIGHT; stencil_y++)
                                {
                                        value +=
                                            p_mesh[(y + stencil_y - margin_y) * p_settings->mesh_width + (x + stencil_x - margin_x)] * stencil_coefs[stencil_y * STENCIL_WIDTH + stencil_x];
                                }
                        }
                        p_temporary_mesh[y * p_settings->mesh_width + x] = value;
                }
        }

        for (x = margin_x; x < p_settings->mesh_width - margin_x; x++)
        {
                for (y = margin_y; y < p_settings->mesh_height - margin_y; y++)
                {
                        p_mesh[y * p_settings->mesh_width + x] = p_temporary_mesh[y * p_settings->mesh_width + x];
                }
        }
}

static void run(ELEMENT_TYPE *p_mesh, struct s_settings *p_settings)
{
        int i;
        for (i = 0; i < p_settings->nb_iterations; i++)
        {
                starpu_stencil_func(p_mesh, p_settings);

                if (p_settings->enable_output)
                {
                        char filename[32];
                        snprintf(filename, 32, "run_mesh_%03d.csv", i);
                        FILE *file = fopen(filename, "w");
                        if (file == NULL)
                        {
                                perror("fopen");
                                exit(EXIT_FAILURE);
                        }
                        write_mesh_to_file(file, p_mesh, p_settings);
                        fclose(file);
                }

                if (p_settings->enable_verbose)
                {
                        printf("mesh after iteration %d\n", i);
                        print_mesh(p_mesh, p_settings);
                        printf("\n\n");
                }
        }
}

static int check(const ELEMENT_TYPE *p_mesh, ELEMENT_TYPE *p_mesh_copy, struct s_settings *p_settings)
{
        int i;
        for (i = 0; i < p_settings->nb_iterations; i++)
        {
                naive_stencil_func(p_mesh_copy, p_settings);

                if (p_settings->enable_output)
                {
                        char filename[32];
                        snprintf(filename, 32, "check_mesh_%03d.csv", i);
                        FILE *file = fopen(filename, "w");
                        if (file == NULL)
                        {
                                perror("fopen");
                                exit(EXIT_FAILURE);
                        }
                        write_mesh_to_file(file, p_mesh_copy, p_settings);
                        fclose(file);
                }

                if (p_settings->enable_verbose)
                {
                        printf("check mesh after iteration %d\n", i);
                        print_mesh(p_mesh_copy, p_settings);
                        printf("\n\n");
                }
        }

        int check = 0;
        int x;
        int y;
        for (y = 0; y < p_settings->mesh_height; y++)
        {
                for (x = 0; x < p_settings->mesh_width; x++)
                {
                        ELEMENT_TYPE diff = fabs(p_mesh[y * p_settings->mesh_width + x] - p_mesh_copy[y * p_settings->mesh_width + x]);
                        if (diff > EPSILON)
                        {
                                fprintf(stderr, "check failed [x: %d, y: %d]: run = %lf, check = %lf\n", x, y,
                                        p_mesh[y * p_settings->mesh_width + x],
                                        p_mesh_copy[y * p_settings->mesh_width + x]);
                                check = 1;
                                return check;
                        }
                }
        }

        return check;
}

int main(int argc, char *argv[])
{
        struct s_settings *p_settings = NULL;

        init_settings(&p_settings);
        parse_cmd_line(argc, argv, p_settings);

        ELEMENT_TYPE *p_mesh = NULL;
        allocate_mesh(&p_mesh, p_settings);

        ELEMENT_TYPE *p_mesh_copy = NULL;
        allocate_mesh(&p_mesh_copy, p_settings);

        {
                if (!p_settings->enable_verbose)
                {
                        print_csv_header();
                }

                int rep;
                for (rep = 0; rep < p_settings->nb_repeat; rep++)
                {
                        if (p_settings->enable_verbose)
                        {
                                printf("repeat %d\n", rep);
                        }

                        init_mesh_values(p_mesh, p_settings);
                        apply_boundary_conditions(p_mesh, p_settings);
                        copy_mesh(p_mesh_copy, p_mesh, p_settings);

                        if (p_settings->enable_verbose)
                        {
                                printf("initial mesh\n");
                                print_mesh(p_mesh, p_settings);
                                printf("\n\n");
                        }

                        struct timespec timing_start, timing_end;
                        clock_gettime(CLOCK_MONOTONIC, &timing_start);
                        run(p_mesh, p_settings);
                        clock_gettime(CLOCK_MONOTONIC, &timing_end);
                        double timing_in_seconds = (timing_end.tv_sec - timing_start.tv_sec) + 1.0e-9 * (timing_end.tv_nsec - timing_start.tv_nsec);

                        int check_status = check(p_mesh, p_mesh_copy, p_settings);

                        if (p_settings->enable_verbose)
                        {
                                print_csv_header();
                        }
                        print_settings_csv(p_settings);
                        printf(",");
                        print_results_csv(rep, timing_in_seconds, check_status);
                        printf("\n");
                }
        }

        delete_mesh(&p_mesh_copy);
        delete_mesh(&p_mesh);
        delete_settings(&p_settings);

        return 0;
}