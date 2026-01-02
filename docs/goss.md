# GOSS Dose Scoring

GOSS (Geant4 Orchestrated Scoring Suite) includes a sensitive detector system for dose scoring with configurable output.

## Commands

All GOSS commands should be placed **before** `/run/initialize` in your macro file.

| Command | Description | Default |
|---------|-------------|---------|
| `/goss/saveInterval <N>` | Save dose output every N events | 1000000 |
| `/goss/outputFile <name>` | Output file name (without .csv extension) | output |
| `/goss/seed <value>` | Set random seed (any integer) | auto (time-based) |

## Example Usage

```bash
# GOSS configuration (before /run/initialize)
/goss/saveInterval  1000000
/goss/outputFile    dose_results
/goss/seed          12345

/run/initialize

# ... rest of your simulation
/run/beamOn 100000000
```

## Output Format

The dose output is saved as a CSV file with the following columns:

| Column | Description | Units |
|--------|-------------|-------|
| Detector_Number | Copy number of the detector | - |
| x_cm, y_cm, z_cm | Detector position | cm |
| Total_Dose_Gy | Total accumulated dose | Gy |
| Dose_Per_Particle_Gy | Dose per primary particle | Gy |
| Dose_Squared_Sum | Sum of D² for variance | Gy² |
| Mean_Dose_Squared_Gy2 | ⟨D²⟩ | Gy² |
| Uncertainty_3sigma_Gy | 3σ uncertainty of dose/particle | Gy |
| nEvents | Number of events processed | - |

## Random Seed Behavior

- If `/goss/seed` is **not set**: A random seed based on system time (nanoseconds) is used automatically
- If `/goss/seed` is **set**: The specified seed is used, allowing reproducible simulations

## Progress Output

During simulation, GOSS displays progress every `saveInterval` events:

```
+--------------------------------------------------------------------+
|  GOSS  |  1e+06 events  |  192 detectors  |  output.csv  |
+--------------------------------------------------------------------+
|  Dose/particle: 7.62e-14 Gy  |  Error: 2.35%  |
+--------------------------------------------------------------------+
```

The **Error** shows the relative 3σ uncertainty of the maximum dose detector. When this reaches ~1% or less, you have good statistics.

## Multi-threading

- Each thread maintains its own dose accumulation
- Output files may be overwritten between threads (intentional per-thread behavior)
- For combined results, post-process the CSV files or run in single-thread mode

## Detector Configuration

The sensitive detectors are defined in `DetectorConstruction.cc`. Key parameters:

- **Material**: G4_Si (Silicon)
- **Dimensions**: 0.265 × 0.265 × 0.06 cm per detector
- **Grid**: 40×40 detectors per layer, 5 layers
- **Mass**: Automatically calculated from volume and material density
