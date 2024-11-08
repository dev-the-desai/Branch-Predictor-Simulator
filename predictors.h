/*
    Contains 3 classes, one for each branch predictor.
*/

#include <iostream>
#include <cmath>
#include <inttypes.h>
#include "sim_bp.h"

using namespace std;

// Bimodal Predictor Class
class bimodal_predictor {
private:
    // User-defined parameters
    unsigned long int M2;               // Number of low-order bits forming index

    // Derived parameters
    uint32_t    num_entries;            // Number of prediction table entries(2^M)

    // Access parameters
    uint32_t    addr;                   // Input address(PC)
    char        outcome;                // Outcome of the instruction

    // Prediction Table
    bimodal_params_t *prediction_table = NULL;

public:
    // Performance metrics
    uint32_t    predicts;               // Total number of predictions made
    uint32_t    mispredicts;            // Total number of mispredictions
    float       miss_predict_rate;      // Miss-Prediction rate

    // Bimodal prediction variables
    uint32_t    bimodal_index;
    char        bimodal_predicted_outcome;

    // Bimodal Methods
    bimodal_predictor(unsigned long int M2);
    ~bimodal_predictor();
    void init_bimodal(unsigned long int M2);
    void bimodal_predict(uint32_t addr, char outcome, bool update_flag);
    void update_bimodal_counter(char outcome, uint32_t index);
    void print_bimodal_counters();
};

// Constructor
bimodal_predictor::bimodal_predictor(unsigned long int M2) : M2(M2)
{
    init_bimodal(M2);
}

// Initialization function to set context
void bimodal_predictor::init_bimodal(unsigned long int M2)
{
    // Initialize all parameters
    predicts            = 0;   
    mispredicts         = 0;   
    miss_predict_rate   = 0;

    addr                = 0;
    outcome             = '\0';

    bimodal_index       = 0;
    bimodal_predicted_outcome = 0;

    num_entries = pow(2, M2);

    // Prediction table memory allocation
    prediction_table = new bimodal_params_t[num_entries];
    for (uint32_t i=0; i < num_entries; i++) {
        // Initialize all counters to 2
        prediction_table[i].bimodal_counter = 2;
    }
}

// Prediction function; makes and handles prediction
void bimodal_predictor::bimodal_predict(uint32_t addr, char outcome, bool update_flag)
{
    // Extract index excluding last 2 bits
    bimodal_index = (addr >> 2) & ((1 << M2) - 1);

    // Make prediction
    unsigned int bimodal_prediction = prediction_table[bimodal_index].bimodal_counter;

    // Convert to char for comparison
    bimodal_predicted_outcome = (bimodal_prediction >= 2) ? 't' : 'n';

    // Total number of predictions
    predicts++;

    // If mispredicted
    if (bimodal_predicted_outcome != outcome) {
        mispredicts++;
    }
    
    // Update counter based on flag
    if (update_flag) update_bimodal_counter(outcome, bimodal_index);
}

// Updates the counter
void bimodal_predictor::update_bimodal_counter(char outcome, uint32_t index) {
    if (outcome == 't' && prediction_table[index].bimodal_counter < 3) {
        prediction_table[index].bimodal_counter++;
    }
    else if (outcome == 'n' && prediction_table[index].bimodal_counter > 0) {
        prediction_table[index].bimodal_counter--;
    }
}

// Prints counter contents
void bimodal_predictor::print_bimodal_counters() {
    cout << "FINAL BIMODAL CONTENTS" << endl;
    for (uint32_t i=0; i < num_entries; i++) {
        cout << i << "  " << prediction_table[i].bimodal_counter << endl;
    }
}

// Destructor
bimodal_predictor::~bimodal_predictor() {
    delete[] prediction_table;
}


// Gshare Predictor Class
class gshare_predictor {
private:
    // User-defined parameters
    unsigned long int M1;
    unsigned long int N;

    // Derived parameters
    uint32_t    num_entries;            // Number of prediction table entries(2^m)

    // Access parameters
    uint32_t    addr;                   // Input address(PC)
    char        outcome;                // Outcome of the instruction

    // Prediction Table
    gshare_params_t *prediction_table = NULL;

    // Global Branch History Register
    uint32_t    BHR;

public:
    // Performance metrics
    uint32_t    predicts;               // Total number of predictions made
    uint32_t    mispredicts;            // Total number of mispredictions
    float       miss_predict_rate;      // Miss-Prediction rate

    // Gshare prediction variables
    uint32_t    gshare_index;
    char        gshare_predicted_outcome;

    // Gshare Methods
    gshare_predictor(unsigned long int M1, unsigned long int N);
    ~gshare_predictor();
    void init_gshare(unsigned long int M1, unsigned long int N);
    void gshare_predict(uint32_t addr, char outcome, bool update_flag);
    void update_gshare_counter(char outcome, uint32_t index);
    void print_gshare_counters();
};

// Constructor
gshare_predictor::gshare_predictor(unsigned long int M1, unsigned long int N) : M1(M1), N(N)
{
    init_gshare(M1, N);
}

// Initialization function to set context
void gshare_predictor::init_gshare(unsigned long int M1, unsigned long int N)
{
    // Initialize all parameters
    predicts            = 0;   
    mispredicts         = 0;   
    miss_predict_rate   = 0;

    addr                = 0;
    outcome             = '\0';

    gshare_index        = 0;
    gshare_predicted_outcome = 0;

    num_entries = pow(2, M1);

    // Prediction table memory allocation
    prediction_table = new gshare_params_t[num_entries];
    for (uint32_t i=0; i < num_entries; i++) {
        // Initialize all counters to 2
        prediction_table[i].gshare_counter = 2;
    }

    // Global Branch History Register initialization
    BHR = 0;
}

// Prediction function; makes and handles prediction
void gshare_predictor::gshare_predict(uint32_t addr, char outcome, bool update_flag)
{
    // Extract m bits excluding last 2 bits
    uint32_t m_bits = (addr >> 2) & ((1 << M1) - 1);

    // Extract first n bits from m_bits
    uint32_t n_bits = m_bits >> (M1 - N);

    // XOR'ing bits
    uint32_t n_bits_xor = n_bits^BHR;

    // Calculating index from n bits and BHR
    gshare_index = (n_bits_xor << (M1 - N)) | (m_bits & ((1 << (M1 - N)) - 1));

    // Make prediction
    unsigned int gshare_prediction = prediction_table[gshare_index].gshare_counter;

    // Convert to char for comparison
    gshare_predicted_outcome = (gshare_prediction >= 2) ? 't' : 'n';

    // Total number of predictions
    predicts++;

    // If mispredicted
    if (gshare_predicted_outcome != outcome) {
        mispredicts++;
    }
    
    // Update counter based on flag
    if (update_flag) update_gshare_counter(outcome, gshare_index);

    // Update BHR
    BHR = (BHR >> 1) | (outcome == 't' ? (1 << (N - 1)) : 0);
}

// Updates the counter
void gshare_predictor::update_gshare_counter(char outcome, uint32_t index) {
    if (outcome == 't' && prediction_table[index].gshare_counter < 3) {
        prediction_table[index].gshare_counter++;
    }
    else if (outcome == 'n' && prediction_table[index].gshare_counter > 0) {
        prediction_table[index].gshare_counter--;
    }
}

// Prints counter contents
void gshare_predictor::print_gshare_counters() {
    cout << "FINAL GSHARE CONTENTS" << endl;
    for (uint32_t i=0; i < num_entries; i++) {
        cout << i << "  " << prediction_table[i].gshare_counter << endl;
    }
}

// Destructor
gshare_predictor::~gshare_predictor() {
    delete[] prediction_table;
}


// Hybrid Predictor Class
class hybrid_predictor {
private:
    // User-defined parameters
    unsigned long int   K;
    unsigned long int   M1;
    unsigned long int   M2;
    unsigned long int   N;

    // Derived parameters
    uint32_t            num_entries;            // Number of prediction table entries(2^K)

    // Access parameters
    uint32_t            addr;                   // Input address(PC)
    char                outcome;                // Outcome of the instruction

    // Chooser Table
    hybrid_params_t *   chooser_table = NULL;

    // Crete instance of bimodal
    bimodal_predictor   bimodal_h;

    // Crete instance of gshare
    gshare_predictor    gshare_h;
    
public:
    // Performance metrics
    uint32_t            predicts;               // Total number of predictions made
    uint32_t            mispredicts;            // Total number of mispredictions
    float               miss_predict_rate;      // Miss-Prediction rate

    // Hybrid prediction variable
    unsigned int        hybrid_prediction;

    // Hybrid Methods
    hybrid_predictor(unsigned long int K, unsigned long int M1, unsigned long int M2, unsigned long int N);
    ~hybrid_predictor();
    void init_hybrid(unsigned long int K);
    void hybrid_predict(uint32_t addr, char outcome);
    void print_hybrid_counters();
};

// Constructor
hybrid_predictor::hybrid_predictor(unsigned long int K, unsigned long int M1, unsigned long int M2, unsigned long int N) : K(K), M1(M1), M2(M2), N(N), bimodal_h(M2), gshare_h(M1, N)
{
    init_hybrid(K);
}

// Initialization function to set context
void hybrid_predictor::init_hybrid(unsigned long int K)
{
    // Initialize all parameters
    predicts            = 0;   
    mispredicts         = 0;   
    miss_predict_rate   = 0;

    addr                = 0;
    outcome             = '\0';

    num_entries = pow(2, K);

    // Prediction table memory allocation
    chooser_table = new hybrid_params_t[num_entries];
    for (uint32_t i=0; i < num_entries; i++) {
        // Initialize all counters to 1
        chooser_table[i].chooser_counter = 1;
    }
}

// Prediction function; makes and handles prediction
void hybrid_predictor::hybrid_predict(uint32_t addr, char outcome)
{
    // Make bimodal prediction
    bimodal_h.bimodal_predict(addr, outcome, false);

    // Make gshare prediction
    gshare_h.gshare_predict(addr, outcome, false);

    // Extract index excluding last 2 bits
    uint32_t counter_index = (addr >> 2) & ((1 << K) - 1);

    // Make prediction
    unsigned int hybrid_choose = chooser_table[counter_index].chooser_counter;

    char hybrid_predicted_outcome = (hybrid_choose >= 2) ? gshare_h.gshare_predicted_outcome : bimodal_h.bimodal_predicted_outcome;

    // Total number of predictions
    predicts++;

    // If mispredicted
    if (hybrid_predicted_outcome != outcome) {
        mispredicts++;
    }

    // Update predictor counter
    if (hybrid_choose >= 2) {
        gshare_h.update_gshare_counter(outcome, gshare_h.gshare_index);
    }
    else {
        bimodal_h.update_bimodal_counter(outcome, bimodal_h.bimodal_index);
    }

    // Updates the chooser counter
    if ((gshare_h.gshare_predicted_outcome == outcome) && 
        (bimodal_h.bimodal_predicted_outcome != outcome) && 
        (chooser_table[counter_index].chooser_counter < 3))
    {
        chooser_table[counter_index].chooser_counter++;
    }
    else if ((gshare_h.gshare_predicted_outcome != outcome) && 
             (bimodal_h.bimodal_predicted_outcome == outcome) && 
             (chooser_table[counter_index].chooser_counter > 0))
    {
        chooser_table[counter_index].chooser_counter--;
    }
}

// Prints counter contents
void hybrid_predictor::print_hybrid_counters() {
    cout << "FINAL CHOOSER CONTENTS" << endl;
    for (uint32_t i=0; i < num_entries; i++) {
        cout << i << "  " << chooser_table[i].chooser_counter << endl;
    }

    gshare_h.print_gshare_counters();
    bimodal_h.print_bimodal_counters();
}

// Destructor
hybrid_predictor::~hybrid_predictor() {
    delete[] chooser_table;
}
