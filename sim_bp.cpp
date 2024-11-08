#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include "sim_bp.h"
#include "predictors.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);      
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Initialize all the predictors
    bimodal_predictor   bimodal(params.M2);
    gshare_predictor    gshare(params.M1, params.N);
    bimodal_predictor   bimodal_0(params.M1);
    hybrid_predictor    hybrid(params.K, params.M1, params.M2, params.N);

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        if (outcome == 't') {}
            //printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        else if (outcome == 'n') {}
            //printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        
        // If Bimodal
        if (strcmp(params.bp_name, "bimodal") == 0) {
            bimodal.bimodal_predict(addr, outcome, true);
        }
        // If Gshare
        else if(strcmp(params.bp_name, "gshare") == 0) {
            if (params.N != 0) gshare.gshare_predict(addr, outcome, true);
            else bimodal_0.bimodal_predict(addr, outcome, true);
        }
        // If Hybrid
        else if(strcmp(params.bp_name, "hybrid") == 0) {
            hybrid.hybrid_predict(addr, outcome);
        }
    }

    cout << left << "OUTPUT" << endl;

    // If Bimodal
    if (strcmp(params.bp_name, "bimodal") == 0) {
        cout << left << setw(27) << "number of predictions:"    << bimodal.predicts << endl;
        cout << left << setw(27) << "number of mispredictions:" << bimodal.mispredicts << endl;
        cout << left << setw(27) << "misprediction rate:"       << fixed << setprecision(2) << ((float)bimodal.mispredicts / (float)bimodal.predicts)*100 << "%" << endl;
        bimodal.print_bimodal_counters();
    }
    
    // If Gshare
    else if(strcmp(params.bp_name, "gshare") == 0) {
        if (params.N != 0) {
            cout << left << setw(27) << "number of predictions:"    << gshare.predicts << endl;
            cout << left << setw(27) << "number of mispredictions:" << gshare.mispredicts << endl;
            cout << left << setw(27) << "misprediction rate:"       << fixed << setprecision(2) << ((float)gshare.mispredicts / (float)gshare.predicts)*100 << "%" << endl;
            gshare.print_gshare_counters();
        }
        else {
            cout << left << setw(27) << "number of predictions:"    << bimodal_0.predicts << endl;
            cout << left << setw(27) << "number of mispredictions:" << bimodal_0.mispredicts << endl;
            cout << left << setw(27) << "misprediction rate:"       << fixed << setprecision(2) << ((float)bimodal_0.mispredicts / (float)bimodal_0.predicts)*100 << "%" << endl;
            bimodal_0.print_bimodal_counters();
        }
    }

    // If Hybrid
    else if(strcmp(params.bp_name, "hybrid") == 0) {
        cout << left << setw(27) << "number of predictions:"    << hybrid.predicts << endl;
        cout << left << setw(27) << "number of mispredictions:" << hybrid.mispredicts << endl;
        cout << left << setw(27) << "misprediction rate:"       << fixed << setprecision(2) << ((float)hybrid.mispredicts / (float)hybrid.predicts)*100 << "%" << endl;
        hybrid.print_hybrid_counters();
    }    

    return 0;
}
