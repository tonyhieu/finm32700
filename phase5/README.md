# Phase 5: HFT Order Book

## Build and run

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
cmake --build build
```

This generates `performance_data_unoptimized.csv` and `performance_data_optimized.csv`.

## Generate chart

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install matplotlib
python plot_performance.py
deactivate
rm -rf .venv
```

This generates `performance_chart_unoptimized.png` and `performance_chart_optimized.png`.
