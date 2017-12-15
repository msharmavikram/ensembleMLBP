# ensembleMLBP
EnsembleMLBP is a ECE511 course project created by Vikram, Umur and Andrew. Here we explore ensemble+ML based branch predictor. Initially we analyzed tournamentBP and its downsides. Then we evaluated weighted majority selection scheme in the Ensemble context.

There are several commands to run different kinds of benchmarks. We assume you have built it to support ARM64 OoO core and that the benchmarks will be running in SE mode.

Command : 
 gem5/rungem5.sh soplex
 gem5/run_microbench.sh
 gem5/run_sebench.sh
 gem5/run_all_benchmarks.sh
 # To reproduce the results discussed in the report, use run_all_benchmarks.sh

To parse and generate data, scripts are provided under the scripts subdirectory. The benchmarks used are precompiled and stored as binaries under benchmark
The EnsembleBP predictor should be selected in the ARM OoO config scripts. EnsembleBP2 was used as an experiment file and it is not discussed in our report.
TriTournament is the 3-way discrete selection. The report is available in the doc folder. Final results are included for your reference under results.

Please feel free to contact us regarding any questions.

