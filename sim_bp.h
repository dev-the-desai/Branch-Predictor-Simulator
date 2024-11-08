#ifndef SIM_BP_H
#define SIM_BP_H

// Param struct
typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

// Bimodal struct
typedef struct bimodal_params_t{
    unsigned int bimodal_counter;
}bimodal_params_t;

// Gshare struct
typedef struct gshare_params_t{
    unsigned int gshare_counter;
}gshare_params_t;

// Hybrid struct
typedef struct hybrid_params_t{
    unsigned int chooser_counter;
}hybrid_params_t;

#endif
